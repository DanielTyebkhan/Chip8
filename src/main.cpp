#include "Emulator.hpp"

int main() {
  Emulator emulator{};
  emulator.LoadProgram("../ExamplePrograms/ibm_logo.ch8");
  emulator.Run();
  return 0;
}