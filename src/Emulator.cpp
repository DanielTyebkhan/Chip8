#include "Emulator.hpp"
#include "Keyboard.hpp"
#include <memory>

Emulator::Emulator()
    : _keyboard(std::make_unique<Keyboard>()),
      _screen(std::make_unique<Screen>()),
      _chip(std::make_unique<Chip8>(_keyboard.get(), _screen.get())),
      _ui(std::make_unique<UI>(_keyboard.get(), _screen.get())) {}

void Emulator::LoadProgram(const std::filesystem::path &path) {
  _chip->LoadProgram(path);
}

void Emulator::Run() {
  // _chip->Run(); TODO: run on thread
  _ui->Run();
}