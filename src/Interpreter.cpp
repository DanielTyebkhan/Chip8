#include "Interpreter.hpp"
#include "Constants.hpp"
#include "InstructionError.hpp"
#include "Screen.hpp"
#include "Types.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

Chip8::Chip8(Keyboard *keyboard, Screen *screen)
    : _keyboard(keyboard), _screen(screen) {
  constexpr static Timer::Duration TIMER_DELAYS{16666667};
  _soundTimer = _timerManager.AddTimer(TIMER_DELAYS, false).lock();
  _delayTimer = _timerManager.AddTimer(TIMER_DELAYS, false).lock();
  constexpr static Timer::Duration CLOCK_SPEED{2000000};
  auto clockTimer = _timerManager.AddTimer(CLOCK_SPEED, true);
  clockTimer.lock()->RegisterCallback(
      [this](auto &&) { RunNextInstruction(); });
}

void Chip8::InitializeMemory() {
  _memory = {};

  std::copy(FONT_SET.begin(), FONT_SET.end(),
            _memory.begin() + MEMORY_OFFSET_FONT);
}

void Chip8::Reset() {
  _soundTimer->SetTicks(0);
  _delayTimer->SetTicks(0);
  InitializeMemory();
  _screen->Clear();
  _stack = {};
  _registers = {};
  _index = 0;
}

void Chip8::LoadProgram(const std::filesystem::path &path) {
  std::ifstream program(path, std::ios::binary);
  if (!program) {
    throw std::runtime_error("Invalid program path: " + path.string());
  }
  unsigned int offset = MEMORY_OFFSET_PROGRAM;
  char byte = 0;
  while (program.read(&byte, 1)) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    _memory[offset] = static_cast<unsigned char>(byte);
    ++offset;
  }
}

constexpr int Chip8::ExtractX(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers)
  return (instruction & 0x0F00) >> 8;
}
constexpr int Chip8::ExtractY(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers)
  return (instruction & 0x00F0) >> 4;
}

constexpr int Chip8::ExtractN(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers)
  return instruction & 0x000F;
}

constexpr int Chip8::ExtractKK(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers)
  return instruction & 0x00FF;
}

constexpr int Chip8::ExtractNNN(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers)
  return instruction & 0x0FFF;
}

int Chip8::FetchInstruction() {
  // NOLINTBEGIN(*-array-index, *-magic-numbers)
  const auto byteOne = _memory[_programCounter];
  const auto byteTwo = _memory[_programCounter + 1];
  return byteOne << Constants::BITS_PER_BYTE | byteTwo;
  // NOLINTEND(*-array-index, *-magic-numbers)
}

void Chip8::StackPush(unsigned short val) {
  if (_stack.size() >= STACK_SIZE) {
    throw std::runtime_error("stack overflow");
  }
  _stack.push(val);
}

unsigned short Chip8::StackPop() {
  if (_stack.empty()) {
    throw std::runtime_error("stack underflow");
  }
  const auto top = _stack.top();
  _stack.pop();
  return top;
}

void Chip8::IncrementPC() { _programCounter += 2; }

// NOLINTNEXTLINE(*cognitive-complexity)
void Chip8::ExecuteInstruction(Instruction instruction) {
  // NOLINTBEGIN(*magic-numbers, *-array-index)
  const auto Y = ExtractY(instruction);
  const auto X = ExtractX(instruction);
  const auto N = ExtractN(instruction);
  const auto KK = ExtractKK(instruction);
  const auto NNN = ExtractNNN(instruction);
  auto *VX = Register(X);
  auto *VY = Register(Y);
  auto *carry = Register(0xF);
  const auto setCarry = [carry](bool value) {
    *carry = static_cast<int>(value);
  };

  const auto firstNibble = static_cast<Opcodes>(instruction & 0xF000);
  const auto lastNibble = static_cast<Opcodes>(instruction & 0x000F);
  if (static_cast<int>(firstNibble) == 0) {
    switch (lastNibble) {
    case Opcodes::ADD_VX_VY: {
      setCarry(*VX > 0xFF - *VY);
      *VX += *VY;
      break;
    }

    case Opcodes::RETURN:
      _programCounter = StackPop();
      break;

    case Opcodes::CLEAR_SCREEN:
      _screen->Clear();
      break;

    default:
      throw InstructionError(instruction);
    }
  } else {
    switch (firstNibble) {

    case Opcodes::LOAD_VX_KK:
      *VX = KK;
      break;

    case Opcodes::E_OPS:
      switch (static_cast<EOps>(lastNibble)) {
      case EOps::SKIP_VX_PRESSED:
        if (_keyboard->IsKeyPressed(*VX)) {
          IncrementPC();
        }
        break;
      case EOps::SKIP_VX_NOT_PRESSED:
        if (!_keyboard->IsKeyPressed(*VX)) {
          IncrementPC();
        }
        break;
      default:
        throw InstructionError(instruction);
      }
      break;

    case Opcodes::F_OPS:
      switch (static_cast<FOps>(instruction & 0x00FF)) {
      case FOps::LOAD_DELAY_VX:
        *VX = static_cast<Byte>(_delayTimer->GetTicks());
        break;
      case FOps::WAIT_KEY_VX: {
        auto keyPress = _keyboard->GetNextKeyPress();
        keyPress.wait();
        *VX = static_cast<Byte>(keyPress.get());
        break;
      }
      case FOps::SET_DELAY_VX:
        _delayTimer->SetTicks(*VX);
        break;
      case FOps::SET_SOUND_VX:
        _soundTimer->SetTicks(*VX);
        break;
      case FOps::ADD_VX_TO_I:
        _index += *VX;
        break;
      case FOps::SET_I_VX_SPRITE:
        _index = MEMORY_OFFSET_FONT + *VX;
        break;
      case FOps::SET_MEM_I_DECIMAL_VX: {
        const auto tc = *VX;
        const Byte hundreds = tc / 100;
        const Byte tens = (tc % 100) / 10;
        const Byte ones = tc % 10;
        _memory[_index] = hundreds;
        _memory[_index + 1] = tens;
        _memory[_index + 2] = ones;
        break;
      }
      case FOps::STORE_MEM_I_V0_TO_VX: {
        std::copy(_registers.begin(), _registers.begin() + X + 1,
                  _memory.begin() + _index);
        break;
      }
      case FOps::LOAD_V0_TO_VX_FROM_MEM_AT_I: {
        std::copy(_memory.begin() + _index, _memory.begin() + _index + X + 1,
                  _registers.begin());
        break;
      }
      default:
        throw InstructionError(instruction);
      }
      break;

    case Opcodes::EIGHT_OPS:
      switch (static_cast<EightOps>(lastNibble)) {
      case EightOps::LOAD_VX_VY:
        *VX = *VY;
        break;
      case EightOps::OR_VX_VY:
        *VX |= *VY;
        break;
      case EightOps::AND_VX_VY:
        *VX &= *VY;
        break;
      case EightOps::XOR_VX_VY:
        *VX ^= *VY;
        break;
      case EightOps::ADD_VX_VY: {
        const auto sum = *VX + *VY;
        *VX = sum & 0xFF;
        setCarry(sum > 0xFF);
        break;
      }
      case EightOps::SUB_VX_VY: {
        const Byte x = *VX;
        const Byte y = *VY;
        *VX = (x - y) & 0xFF;
        setCarry(y <= x);
        break;
      }
      case EightOps::SHIFT_RIGHT_VX: {
        const auto x = *VX;
        *VX >>= 1;
        setCarry(((x & 1) != 0));
        break;
      }
      case EightOps::SUBN_VX_VY: {
        const unsigned int x = *VX;
        const unsigned int y = *VY;
        *VX = static_cast<Byte>(y - x) & 0xFF;
        setCarry(y >= x);
        break;
      }
      case EightOps::SHIFT_LEFT_VX: {
        const auto x = *VX;
        *VX = (x << 1) & 0xFF;
        setCarry((x & 0b10000000) != 0);
        break;
      }
      default:
        throw InstructionError(instruction);
      }
      break;

    case Opcodes::ADD_VX_KK:
      *VX = (*VX + KK) & 0xFF;
      break;

    case Opcodes::JUMP_NNN:
      _programCounter = NNN;
      break;

    case Opcodes::JUMP_V0_NNN:
      _programCounter = NNN + *Register(0);
      break;

    case Opcodes::CALL_NNN:
      StackPush(_programCounter);
      _programCounter = NNN;
      break;

    case Opcodes::SET_INDEX_NNN:
      _index = NNN;
      break;

    case Opcodes::SKIP_VX_EQ_KK:
      if (*VX == KK) {
        IncrementPC();
      }
      break;

    case Opcodes::SKIP_VX_NEQ_KK:
      if (*VX != KK) {
        IncrementPC();
      }
      break;

    case Opcodes::SKIP_VX_EQ_VY:
      if (*VX == *VY) {
        IncrementPC();
      }
      break;

    case Opcodes::SKIP_VX_NEQ_VY:
      if (*VX != *VY) {
        IncrementPC();
      }
      break;

    case Opcodes::RND_VX_KK:
      *VX = _rng.Generate() & NNN;
      break;

    case Opcodes::DRAW: {
      _screen->Draw(*VX, *VY, std::span(_memory).subspan(_index, N));
      break;
    }
    default:
      throw InstructionError(instruction);
    }
  }
  // NOLINTEND(*magic-numbers, *-array-index)
}

void Chip8::RunNextInstruction() {
  const auto nextInstruction = FetchInstruction();
  IncrementPC();
  ExecuteInstruction(nextInstruction);
}

void Chip8::Run() {
  _programCounter = MEMORY_OFFSET_PROGRAM;
  while (!_cancelled) {
    _timerManager.Tick();
  }
}

void Chip8::Cancel() { _cancelled = true; }