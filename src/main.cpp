#include "Emulator.hpp"

int main() {
  Emulator emulator{"../ExamplePrograms/ibm_logo.ch8"};
  emulator.Run();
  return 0;
}