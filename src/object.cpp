#include "object.hpp"
#include "helpers.hpp"
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
namespace Object {

// Get ma strings
std::string objectTypeToStr(ObjectType type) {
  switch (type) {
  case ObjectType::INTEGER_OBJ:
    return "INTEGER";
  case Object::ObjectType::BOOLEAN_OBJ:
    return "BOOLEAN";
  case Object::ObjectType::NULL_OBJ:
    return "NULL";
  case Object::ObjectType::RETURN_VALUE_OBJ:
    return "RETURN";
  case Object::ObjectType::ERROR_OBJ:
    return "ERROR";
  case Object::ObjectType::FUNCTION_OBJ:
    return "FUNCTION";
  case Object::ObjectType::STRING_OBJ:
    return "STRING";
  case Object::ObjectType::BUILTIN_OBJ:
    return "BUILTIN";
  case Object::ObjectType::ARRAY_OBJ:
    return "ARRAY";
  }
}

void Environment::PrintEnv() {
  std::cout << "ENVIRONMENT ################################################\n";
  for (const auto &pair : this->m_environment) {
    std::cout << std::format("key: {}, type: {}", pair.first,
                             Object::objectTypeToStr(pair.second->Type()));
  }
}

Environment::Environment(const std::shared_ptr<Environment> &outerEnv)
  : m_outerEnv(std::weak_ptr<Environment>(outerEnv)) {}

Environment::EnvObj Environment::Get(const std::string &name) {
  std::shared_ptr<Environment> outerEnv = m_outerEnv.lock();
  if (this->m_environment.contains(name)) {
    return {.obj = this->m_environment[name], .ok = true};
  } else if (outerEnv) {
    return outerEnv->Get(name);
  } else {
    return {.obj = nullptr, .ok = false};
  }
}

void Environment::Set(const std::string &name,
                      std::shared_ptr<Object::IObject> obj) {
  this->m_environment[name] = std::move(obj);
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
ReturnValue::ReturnValue(std::shared_ptr<IObject> value)
  : m_value(std::move(value)) {}
ObjectType ReturnValue::Type() const { return ObjectType::RETURN_VALUE_OBJ; }
std::string ReturnValue::Inspect() const { return m_value->Inspect(); }

// Error Object
Error::Error(std::string message) : m_message(std::move(message)) {}
ObjectType Error::Type() const { return ObjectType::ERROR_OBJ; }
std::string Error::Inspect() const {
  return std::format("Error: {}", m_message);
}

// Function Object

Function::Function(std::vector<std::unique_ptr<Ast::Identifier>> parameters,
                   std::shared_ptr<Environment> env,
                   std::unique_ptr<Ast::BlockStatement> body)
  : m_parameters(std::move(parameters)), m_env(std::move(env)),
    m_body(std::move(body)) {}

ObjectType Function::Type() const { return ObjectType::FUNCTION_OBJ; }

std::string Function::Inspect() const {
  std::string out;
  std::vector<std::string> params;
  params.reserve(this->m_parameters.size());
  for (const auto &p : this->m_parameters) {
    params.emplace_back(p->String());
  }
  out.append("fn(");
  out.append(Helpers::combineVecStrWithDelim(params, ","));
  out.append(") {\n");
  out.append(this->m_body->String());
  out.append("\n}");
  return out;
}

// String Object
String::String(std::string value) : m_value(std::move(value)) {}
ObjectType String::Type() const { return ObjectType::STRING_OBJ; }
std::string String::Inspect() const { return m_value; }

// Array object

Array::Array(std::vector<std::shared_ptr<IObject>> elements)
  : m_elements(std::move(elements)) {}
ObjectType Array::Type() const { return ObjectType::ARRAY_OBJ; }
std::string Array::Inspect() const {
  std::string out;
  std::vector<std::string> elements;
  elements.reserve(this->m_elements.size());
  for (const auto &p : this->m_elements) {
    elements.emplace_back(p->Inspect());
  }
  out.append("[");
  out.append(Helpers::combineVecStrWithDelim(elements, ","));
  out.append("]");
  return out;
}

// BuiltinFunction object
Builtin::Builtin(BuiltinFunction fn) : m_fn(std::move(fn)) {}
ObjectType Builtin::Type() const { return ObjectType::BUILTIN_OBJ; }
std::string Builtin::Inspect() const { return "builtin function"; }
} // namespace Object
