#include "Emulator.hpp"
#include "InstructionError.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <tuple>

Chip8::Chip8() { Reset(); }

void Chip8::InitializeMemory() {
  _memory = {};

  std::copy(FONT_SET.begin(), FONT_SET.end(),
            _memory.begin() + MEMORY_OFFSET_FONT);
}

void Chip8::Reset() {
  _soundTimer = {};
  _lastExecution = std::chrono::steady_clock::time_point::min();
  InitializeMemory();
  _stack = {};
  _carry = 0;
  _registers = {};
  _programCounter = INITIAL_PROGRAM_COUNTER;
  _index = 0;
}

void Chip8::LoadProgram(const std::vector<Byte> &program) {
  std::copy(program.begin(), program.end(),
            _memory.begin() + MEMORY_OFFSET_PROGRAM);
}

constexpr int Chip8::ExtractX(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers, *-signed-bitwise)
  return instruction & 0x0F00 >> 8;
}
constexpr int Chip8::ExtractY(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers, *-signed-bitwise)
  return instruction & 0x00F0 >> 4;
}

constexpr int Chip8::ExtractN(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers, *-signed-bitwise)
  return instruction & 0x000F;
}

constexpr int Chip8::ExtractKK(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers, *-signed-bitwise)
  return instruction & 0x00FF;
}

constexpr int Chip8::ExtractNNN(int instruction) {
  // NOLINTNEXTLINE(*-magic-numbers, *-signed-bitwise)
  return instruction & 0x0FFF;
}

int Chip8::FetchInstruction() {
  static constexpr int BYTE = 8;
  // NOLINTBEGIN(*-array-index, *-magic-numbers, *-signed-bitwise)
  return _memory[_programCounter] << BYTE | _memory[_programCounter + 1];
  // NOLINTEND(*-array-index, *-magic-numbers, *-signed-bitwise)
}

void Chip8::StackPush(unsigned short val) {
  if (_stack.size() == std::tuple_size_v<decltype(_stack)::container_type>) {
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
  // NOLINTBEGIN(*magic-numbers, *-signed-bitwise, *-array-index)
  const auto Y = ExtractY(instruction);
  const auto X = ExtractX(instruction);
  const auto N = ExtractN(instruction);
  const auto KK = ExtractKK(instruction);
  const auto NNN = ExtractNNN(instruction);
  auto &VX = Register(X);
  auto &VY = Register(Y);

  const auto firstNibble = static_cast<Opcodes>(instruction & 0xF000);
  if (static_cast<int>(firstNibble) == 0) {
    const auto lastNibble = static_cast<Opcodes>(instruction & 0x000F);

    switch (lastNibble) {
    case Opcodes::ADD_VX_VY: {
      _carry = VX > 0xFF - VY;
      VX += VY;
      return true;
    }

    case Opcodes::CLEAR_SCREEN:
      _screen->Clear();
      return true;

    default:
      throw InstructionError(instruction);
    }

  } else {

    switch (firstNibble) {
    case Opcodes::LOAD_VX_KK:
      VX = KK;
      return true;

    case Opcodes::ADD_VX_KK:
      VX += KK;
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

    case Opcodes::DRAW:

    default:
      throw InstructionError(instruction);
    }
  }
  // NOLINTEND(*magic-numbers, *-signed-bitwise, *-array-index)
  assert(false);
}

void Chip8::Run() {
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
      _screen->Refresh();
    }
  }
}