#include "object.hpp"
#include <string>
namespace Object {

// integer
Integer::Integer(long int value) : m_value(value) {}
ObjectType Integer::Type() { return ObjectType::INTEGER_OBJ; }
std::string Integer::Inspect() { return std::to_string(m_value); }

// boolean
Boolean::Boolean(bool value) : m_value(value) {}
ObjectType Boolean::Type() { return ObjectType::BOOLEAN_OBJ; }
std::string Boolean::Inspect() { return m_value ? "true" : "false"; }

// null
ObjectType Null::Type() { return ObjectType::NULL_OBJ; }
std::string Null::Inspect() { return "null"; }

} // namespace Object
