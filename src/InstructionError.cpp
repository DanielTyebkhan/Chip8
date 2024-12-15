
#include "InstructionError.hpp"
#include <iomanip>
#include <sstream>

InstructionError::InstructionError(int instruction)
    : std::runtime_error(MakeMessage(instruction)) {}

std::string InstructionError::MakeMessage(int instruction) {
  std::stringstream stream;
  static constexpr auto WIDTH = 4;
  stream << "Invalid instruction: " << std::hex << std::uppercase
         << std::setfill('0') << std::setw(WIDTH) << instruction;
  return stream.str();
}