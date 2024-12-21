#pragma once

#include "Constants.hpp"
#include "DelayTimer.hpp"
#include "Keyboard.hpp"
#include "Random.hpp"
#include "Screen.hpp"
#include "SoundTimer.hpp"
#include "Types.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <stack>
#include <stdexcept>

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
    JUMP_V0_NNN = 0xB000,
    RETURN = 0x000E,

    // Register
    ADD_VX_VY = 0x0004,
    LOAD_VX_KK = 0x6000,
    ADD_VX_KK = 0x7000,
    RND_VX_KK = 0xC00,

    // branch
    SKIP_VX_EQ_KK = 0x3000,
    SKIP_VX_NEQ_KK = 0x4000,
    SKIP_VX_EQ_VY = 0x5000,
    SKIP_VX_NEQ_VY = 0x9000,

    // keyboard

    E_OPS = 0xE000,
    F_OPS = 0xF000,
    EIGHT_OPS = 0x8000,
  };

  // Last nibble for opcodes of the form 0x8xy*
  enum class EightOps {
    LOAD_VX_VY = 0x0000,
    OR_VX_VY = 0x0001,
    AND_VX_VY = 0x0002,
    XOR_VX_VY = 0x0003,
    ADD_VX_VY = 0x0004,
    SUB_VX_VY = 0x0005,
    SHIFT_RIGHT_VX = 0x0006,
    SUBN_VX_VY = 0x0007,
    SHIFT_LEFT_VX = 0x000E,
  };

  enum class FOps {
    LOAD_DELAY_VX = 0x0007,
    WAIT_KEY_VX = 0x000A,
    SET_DELAY_VX = 0x0015,
    SET_SOUND_VX = 0x0018,
    ADD_VX_TO_I = 0x001E,
    SET_I_VX_SPRITE = 0x0029,
    SET_MEM_I_DECIMAL_VX = 0x0033,
    STORE_MEM_I_V0_TO_VX = 0x0055,
    LOAD_V0_TO_VX_FROM_MEM_AT_I = 0x0065,
  };

  enum class EOps {
    SKIP_VX_PRESSED = 0x000E,
    SKIP_VX_NOT_PRESSED = 0x0001,
  };

public:
  explicit Chip8(Keyboard *keyboard, Screen *screen);

  void Cancel();

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

  /**
   * @brief execute an opcode
   * @return true if the program counter should be incremented
   */
  void ExecuteInstruction(Instruction instruction);

  void StackPush(unsigned short val);

  unsigned short StackPop();

  std::size_t _programCounter = MEMORY_OFFSET_PROGRAM;

  inline Byte *Register(std::size_t target) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return &_registers.at(target);
  }

  Keyboard *_keyboard;

  RandomNumberGenerator _rng{0, Constants::MAX_BYTE,
                             static_cast<int>(time(nullptr))};

  Screen *_screen;

  std::size_t _index = 0;

  DelayTimer _delayTimer;

  SoundTimer _soundTimer{};

  std::chrono::steady_clock::time_point _lastExecution;

  // by popular convention, but can be anywhere 0x0000 - 0x01FF
  static constexpr std::size_t MEMORY_OFFSET_FONT = 0x0050;

  /** 500hz */
  static constexpr std::chrono::steady_clock::duration CPU_TICK_PERIOD =
      std::chrono::nanoseconds{2000000};

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

  std::atomic<bool> _cancelled = false;
};