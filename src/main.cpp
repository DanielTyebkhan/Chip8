#include "Emulator.hpp"

int main() {
  Emulator emulator{"../ExamplePrograms/3-corax+.ch8"};
  emulator.Run();
  return 0;
}