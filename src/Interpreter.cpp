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
    : _keyboard(keyboard), _screen(screen) {}

void Chip8::InitializeMemory() {
  _memory = {};

  std::copy(FONT_SET.begin(), FONT_SET.end(),
            _memory.begin() + MEMORY_OFFSET_FONT);
}

void Chip8::Reset() {
  _soundTimer = {};
  _lastExecution =
      std::chrono::steady_clock::time_point{std::chrono::seconds{0}};
  InitializeMemory();
  _screen->Clear();
  _stack = {};
  _carry = false;
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
bool Chip8::ExecuteInstruction(Instruction instruction) {
  // NOLINTBEGIN(*magic-numbers, *-array-index)
  const auto Y = ExtractY(instruction);
  const auto X = ExtractX(instruction);
  const auto N = ExtractN(instruction);
  const auto KK = ExtractKK(instruction);
  const auto NNN = ExtractNNN(instruction);
  auto *VX = Register(X);
  auto *VY = Register(Y);

  const auto firstNibble = static_cast<Opcodes>(instruction & 0xF000);
  const auto lastNibble = static_cast<Opcodes>(instruction & 0x000F);
  if (static_cast<int>(firstNibble) == 0) {

    switch (lastNibble) {
    case Opcodes::ADD_VX_VY: {
      _carry = *VX > 0xFF - *VY;
      *VX += *VY;
      return true;
    }

    case Opcodes::RETURN:
      _programCounter = StackPop();
      return true;

    case Opcodes::CLEAR_SCREEN:
      _screen->Clear();
      return true;

    default:
      throw InstructionError(instruction);
    }

  } else {

    switch (firstNibble) {

    case Opcodes::LOAD_VX_KK:
      *VX = KK;
      return true;

    case Opcodes::E_OPS:
      switch (static_cast<EOps>(lastNibble)) {
      case EOps::SKIP_VX_PRESSED:
        if (_keyboard->IsKeyPressed(*VX)) {
          IncrementPC();
        }
        return true;
      case EOps::SKIP_VX_NOT_PRESSED:
        if (!_keyboard->IsKeyPressed(*VX)) {
          IncrementPC();
        }
        return true;
      }
      throw InstructionError(instruction);

    case Opcodes::F_OPS:
      switch (static_cast<FOps>(instruction & 0x00FF)) {
      case FOps::LOAD_DELAY_VX:
        *VX = _delayTimer;
        return true;
      case FOps::WAIT_KEY_VX: {
        auto keyPress = _keyboard->GetNextKeyPress();
        keyPress.wait();
        *VX = static_cast<Byte>(keyPress.get());
        return true;
      }
      case FOps::SET_DELAY_VX:
        _delayTimer = *VX;
        return true;
      case FOps::SET_SOUND_VX:
        _soundTimer.SetTimer(*VX);
        return true;
      case FOps::ADD_VX_TO_I:
        _index += *VX;
        return true;
      case FOps::SET_I_VX_SPRITE:
        _index = MEMORY_OFFSET_FONT + *VX;
        return true;
      case FOps::SET_MEM_I_DECIMAL_VX: {
        const auto tc = *VX;
        const Byte hundreds = std::floor(tc / 100);
        const Byte tens = std::floor((tc % 100) / 10);
        const Byte ones = tc % 10;
        _memory[_index] = hundreds / 100;
        _memory[_index + 1] = tens / 10;
        _memory[_index + 2] = ones;
        return true;
      }
      case FOps::STORE_MEM_I_V0_TO_VX: {
        std::copy(_registers.begin(), _registers.begin() + X + 1,
                  _memory.begin() + _index);
        return true;
      }
      case FOps::LOAD_V0_TO_VX_FROM_MEM_AT_I: {
        std::copy(_memory.begin() + _index, _memory.begin() + _index + X,
                  _registers.begin());
        return true;
      }
      }
      throw InstructionError(instruction);

    case Opcodes::EIGHT_OPS:
      switch (static_cast<EightOps>(lastNibble)) {
      case EightOps::LOAD_VX_VY:
        *VX = *VY;
        return true;
      case EightOps::OR_VX_VY:
        *VX |= *VY;
        return true;
      case EightOps::AND_VX_VY:
        *VX &= *VY;
        return true;
      case EightOps::XOR_VX_VY:
        *VX ^= *VY;
        return true;
      case EightOps::ADD_VX_VY: {
        const auto sum = *VX + *VY;
        *VX = sum & 0xFF;
        _carry = sum > 0xFF;
        return true;
      }
      case EightOps::SUB_VX_VY: {
        _carry = *VY > *VX;
        const unsigned int x = *VX;
        const unsigned int y = *VY;
        *VX = static_cast<Byte>(x - y) & 0xFF;
        return true;
      }
      case EightOps::SHIFT_RIGHT_VX:
        _carry = ((*VX & 1) != 0);
        *VX >>= 1;
        return true;
      case EightOps::SUBN_VX_VY: {
        _carry = *VY > *VX;
        const unsigned int x = *VX;
        const unsigned int y = *VY;
        *VX = static_cast<Byte>(y - x) & 0xFF;
        return true;
      }
      case EightOps::SHIFT_LEFT_VX:
        _carry = ((*VX & 0b10000000) != 0);
        *VX = (*VX << 1) & 0xFF;
        return true;
      }
      throw InstructionError(instruction);

    case Opcodes::ADD_VX_KK:
      *VX = (*VX + KK) & 0xFF;
      return true;

    case Opcodes::JUMP_NNN:
      _programCounter = NNN;
      return false;

    case Opcodes::JUMP_V0_NNN:
      _programCounter = NNN + *Register(0);
      return false;

    case Opcodes::CALL_NNN:
      StackPush(_programCounter);
      _programCounter = NNN;
      return false;

    case Opcodes::SET_INDEX_NNN:
      _index = NNN;
      return true;

    case Opcodes::SKIP_VX_EQ_KK:
      if (*VX == KK) {
        IncrementPC();
      }
      return true;

    case Opcodes::SKIP_VX_NEQ_KK:
      if (*VX != KK) {
        IncrementPC();
      }
      return true;

    case Opcodes::SKIP_VX_EQ_VY:
      if (*VX == *VY) {
        IncrementPC();
      }
      return true;

    case Opcodes::SKIP_VX_NEQ_VY:
      if (*VX != *VY) {
        IncrementPC();
      }
      return true;

    case Opcodes::RND_VX_KK:
      *VX = _rng.Generate() & NNN;
      return true;

    case Opcodes::DRAW: {
      auto *const spriteStart = _memory.begin() + _index;
      // NOLINTNEXTLINE(*pointer-arithmetic)
      auto *const spriteEnd = spriteStart + N;
      _screen->Draw(*VX, *VY, std::span(spriteStart, spriteEnd));
      return true;
    }

    default:
      throw InstructionError(instruction);
    }
  }
  // NOLINTEND(*magic-numbers, *-array-index)
  assert(false);
}

void Chip8::Run() {
  _programCounter = MEMORY_OFFSET_PROGRAM;
  while (!_cancelled) {
    const auto now = std::chrono::steady_clock::now();
    if (now - _lastExecution >= TICK_PERIOD) {
      _lastExecution = now;
      _soundTimer.Tick();
      --_delayTimer;
      auto shouldIncrementPc = ExecuteInstruction(FetchInstruction());
      if (shouldIncrementPc) {
        IncrementPC();
      }
    }
  }
}

void Chip8::Cancel() { _cancelled = true; }