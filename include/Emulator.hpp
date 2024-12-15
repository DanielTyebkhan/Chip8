#pragma once

#include "Screen.hpp"
#include "Types.hpp"
#include <array>
#include <cstddef>
#include <memory>
#include <stack>
#include <vector>

class Chip8 {
  enum class Opcodes {
    e_CLEAR_SCREEN = 0x0000,
    e_SET_INDEX = 0xA000,
    e_CALL = 0x2000,
    e_ADD_VX_VY = 0x0004,
  };

public:
  Chip8();

  void Reset();

  void LoadProgram(const std::vector<Byte> &program);

  void Run() noexcept;

private:
  Opcodes GetNextInstruction();

  void IncrementPC();

  void EmulateCycle();

  void InitializeMemory();

  std::unique_ptr<Screen> _screen;

  std::size_t _index = 0;

  /**
   * @brief execute an opcode
   * @return true if the program counter should be incremented
   */
  bool ExecuteOpcode(Opcodes opcode);

  void StackPush(unsigned short val);

  unsigned short StackPop();

  constexpr static std::size_t INITIAL_PROGRAM_COUNTER = 0x200;
  std::size_t _programCounter = INITIAL_PROGRAM_COUNTER;

  inline Byte &Register(std::size_t target) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return _registers[target];
  }

  Byte _carry = 0;

  constexpr static std::size_t NUM_REGISTERS = 15;

  constexpr static std::size_t NUM_CARRY = 1;

  std::array<Byte, NUM_REGISTERS + NUM_CARRY> _registers{};

  constexpr static std::size_t STACK_SIZE = 16;
  using StackUnderlying = std::array<unsigned short, STACK_SIZE>;
  std::stack<unsigned short, StackUnderlying> _stack{};

  constexpr static std::size_t MEMORY_OFFSET_PROGRAM = 0x200;
  constexpr static std::size_t MEMORY_BYTES = 4096;
  std::array<Byte, MEMORY_BYTES> _memory = {};

  constexpr static auto FONT_SET = (std::to_array<Byte>({
      0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10,
      0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10,
      0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
      0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
      0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
      0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
      0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80,
  }));
};
