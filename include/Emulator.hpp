#pragma once

#include "Screen.hpp"
#include "SoundTimer.hpp"
#include "Types.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <stack>
#include <vector>

class Chip8 {
  using Instruction = int;
  enum class Opcodes {
    // screen
    CLEAR_SCREEN = 0x0000,
    DRAW = 0xD000,

    // Index
    SET_INDEX_NNN = 0xA000,

    // PC
    CALL_NNN = 0x2000,
    JUMP_NNN = 0x1000,

    // Register
    ADD_VX_VY = 0x0004,
    LOAD_VX_KK = 0x6000,
    ADD_VX_KK = 0x7000,
  };

public:
  Chip8();

  void Reset();

  void LoadProgram(const std::filesystem::path &path);

  void Run();

private:
  Instruction FetchInstruction();

  static constexpr int ExtractX(Instruction instruction);
  static constexpr int ExtractY(Instruction instruction);
  static constexpr int ExtractN(Instruction instruction);
  static constexpr int ExtractKK(Instruction instruction);
  static constexpr int ExtractNNN(Instruction instruction);

  void IncrementPC();

  void InitializeMemory();

  std::unique_ptr<Screen> _screen = std::make_unique<Screen>();

  std::size_t _index = 0;

  /**
   * @brief execute an opcode
   * @return true if the program counter should be incremented
   */
  bool ExecuteInstruction(Instruction instruction);

  void StackPush(unsigned short val);

  unsigned short StackPop();

  std::size_t _programCounter = MEMORY_OFFSET_PROGRAM;

  inline Byte &Register(std::size_t target) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return _registers[target];
  }

  uint8_t _delayTimer = 0;

  SoundTimer _soundTimer{};

  std::chrono::steady_clock::time_point _lastExecution;

  // by popular convention, but can be anywhere 0x0000 - 0x01FF
  static constexpr std::size_t MEMORY_OFFSET_FONT = 0x0050;

  /** 60hz */
  static constexpr std::chrono::steady_clock::duration TICK_PERIOD =
      std::chrono::nanoseconds{16666667};

  bool _carry = false;

  constexpr static std::size_t NUM_REGISTERS = 15;

  constexpr static std::size_t NUM_CARRY = 1;

  std::array<Byte, NUM_REGISTERS + NUM_CARRY> _registers{};

  constexpr static std::size_t STACK_SIZE = 16;
  // using StackUnderlying = std::array<unsigned short, STACK_SIZE>;
  std::stack<unsigned short> _stack;

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