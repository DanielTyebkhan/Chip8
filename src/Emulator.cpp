#include "Emulator.hpp"
#include "Constants.hpp"
#include "InstructionError.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

Chip8::Chip8() { Reset(); }

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
      return false;

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
        _carry = sum > 0xFF;
        break;
      }
      case EightOps::SUB_VX_VY: {
        _carry = *VY > *VX;
        const unsigned int x = *VX;
        const unsigned int y = *VY;
        *VX = static_cast<Byte>(x - y) & 0xFF;
        break;
      }
      case EightOps::SHIFT_RIGHT_VX:
        _carry = ((*VX & 1) != 0);
        *VX >>= 1;
        break;
      case EightOps::SUBN_VX_VY: {
        _carry = *VY > *VX;
        const unsigned int x = *VX;
        const unsigned int y = *VY;
        *VX = static_cast<Byte>(y - x) & 0xFF;
        break;
      }
      case EightOps::SHIFT_LEFT_VX:
        _carry = ((*VX & 0x1000) != 0);
        *VX = (*VX <<= 1) & 0x255;
      }
      return true;

    case Opcodes::ADD_VX_KK:
      *VX += KK;
      return true;

    case Opcodes::JUMP_NNN:
      _programCounter = NNN;
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

    case Opcodes::DRAW: {
      auto *const spriteStart = _memory.begin() + _index;
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
  while (true) {
    const auto now = std::chrono::steady_clock::now();
    if (now - _lastExecution >= TICK_PERIOD) {
      _lastExecution = now;
      _soundTimer.Tick();
      --_delayTimer;
      auto shouldIncrementPc = ExecuteInstruction(FetchInstruction());
      if (shouldIncrementPc) {
        IncrementPC();
      }
      _screen->Update();
    }
  }
}