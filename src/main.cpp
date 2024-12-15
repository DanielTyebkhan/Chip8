#include "Emulator.hpp"
int main() {
  Chip8 emulator{};
  emulator.LoadProgram("../ExamplePrograms/ibm_logo.ch8");
  emulator.Run();
}