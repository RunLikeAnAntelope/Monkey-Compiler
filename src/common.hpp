#pragma once
#include <string>
#include <variant>
namespace Common {

typedef std::variant<long int, std::string, bool, std::monostate> variant;

} // namespace Common
