#pragma once

#include <stdexcept>

class InstructionError : public std::runtime_error {
public:
  explicit InstructionError(int instruction);

private:
  static std::string MakeMessage(int instruction);
};