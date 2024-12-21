#include "Emulator.hpp"

int main() {
  Emulator emulator{"../ExamplePrograms/5-quirks.ch8"};
  emulator.Run();
  return 0;
}