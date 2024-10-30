#include "object.hpp"
#include <string>
namespace Object {

// integer
Integer::Integer(long int value) : m_value(value) {}
ObjectType Integer::Type() { return INTEGER_OBJ; }
std::string Integer::Inspect() { return std::to_string(m_value); }

// boolean
Boolean::Boolean(long int value) : m_value(value) {}
ObjectType Boolean::Type() { return BOOLEAN_OBJ; }
std::string Boolean::Inspect() { return std::to_string(m_value); }

// null
ObjectType Null::Type() { return NULL_OBJ; }
std::string Null::Inspect() { return "null"; }

} // namespace Object
