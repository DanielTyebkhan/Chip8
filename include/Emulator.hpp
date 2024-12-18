#pragma once

#include "Interpreter.hpp"
#include "Keyboard.hpp"
#include "UI.hpp"
#include <filesystem>
#include <memory>
class Emulator {
public:
  explicit Emulator(const std::filesystem::path &programPath);
  void Run();

private:
  std::unique_ptr<Keyboard> _keyboard;
  std::unique_ptr<Screen> _screen;
  std::unique_ptr<Chip8> _chip;
  std::unique_ptr<SdlManager> _ui;
};