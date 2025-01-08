#pragma once
#include "object.hpp"
#include <unordered_map>
namespace Builtins {
extern std::unordered_map<std::string, std::shared_ptr<Object::Builtin>>
  builtins;
}
