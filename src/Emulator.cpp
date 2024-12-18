#include "Emulator.hpp"
#include "Keyboard.hpp"
#include "Screen.hpp"
#include <memory>

Emulator::Emulator(const std::filesystem::path &programPath)
    : _keyboard(std::make_unique<Keyboard>()),
      _screen(std::make_unique<Screen>()),
      _chip(std::make_unique<Chip8>(_keyboard.get(), _screen.get())),
      _ui(std::make_unique<SdlManager>(Screen::WIDTH, Screen::HEIGHT,
                                       _keyboard.get())) {
  _chip->LoadProgram(programPath);
  _screen->RegisterUpdateCallback([this](std::span<bool> frame) {
    _ui->QueueFrame({frame.begin(), frame.end()});
  });
}

void Emulator::Run() {
  std::thread chipThread{[this]() { _chip->Run(); }};
  _ui->Run();
  _chip->Cancel();
  chipThread.join();
}