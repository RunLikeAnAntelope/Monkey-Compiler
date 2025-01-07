#pragma once
#include <string>
#include <variant>
namespace Common {

using variant = std::variant<long int, std::string, bool, std::monostate>;

} // namespace Common
