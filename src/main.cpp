#include "Emulator.hpp"

int main() {
  Emulator emulator{"../ExamplePrograms/4-flags.ch8"};
  emulator.Run();
  return 0;
}