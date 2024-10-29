#include "object.hpp"
namespace Object {

// integer
ObjectType Integer::Type() { return INTEGER_OBJ; }
std::string Integer::Inspect() { return std::to_string(m_value); }

} // namespace Object
