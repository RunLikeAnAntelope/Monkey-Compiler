#include "object.hpp"
#include <memory>
#include <string>
namespace Object {

// Get ma strings
std::string objectTypeToStr(ObjectType type) {
    switch (type) {
    case ObjectType::INTEGER_OBJ:
        return "Integer";
    case Object::ObjectType::BOOLEAN_OBJ:
        return "Boolean";
    case Object::ObjectType::NULL_OBJ:
        return "Boolean";
    case Object::ObjectType::RETURN_VALUE_OBJ:
        return "Return";
    }
}

// integer
Integer::Integer(long int value) : m_value(value) {}
ObjectType Integer::Type() const { return ObjectType::INTEGER_OBJ; }
std::string Integer::Inspect() const { return std::to_string(m_value); }

// boolean
Boolean::Boolean(bool value) : m_value(value) {}
ObjectType Boolean::Type() const { return ObjectType::BOOLEAN_OBJ; }
std::string Boolean::Inspect() const { return m_value ? "true" : "false"; }

// null
ObjectType Null::Type() const { return ObjectType::NULL_OBJ; }
std::string Null::Inspect() const { return "null"; }

// Return Object
ReturnValue::ReturnValue(std::unique_ptr<IObject> value)
    : m_value(std::move(value)) {}
ObjectType ReturnValue::Type() const { return ObjectType::RETURN_VALUE_OBJ; }
std::string ReturnValue::Inspect() const { return m_value.get()->Inspect(); }

} // namespace Object
