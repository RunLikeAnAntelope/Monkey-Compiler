#include "builtins.hpp"
#include "evaluator.hpp"
#include "format"
namespace Builtins {

static std::shared_ptr<Object::IObject>
len(const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (std::ssize(args) != 1) {
    return Evaluator::Evaluator::newError(std::format(
      "wrong number of arguments. got={}. want=1", std::ssize(args)));
  }

  switch (args[0].get()->Type()) {
  case Object::ObjectType::STRING_OBJ: {
    auto *strObj = dynamic_cast<Object::String *>(args[0].get());
    return std::make_shared<Object::Integer>(std::ssize(strObj->m_value));
  }
  case Object::ObjectType::ARRAY_OBJ: {
    auto *arrObj = dynamic_cast<Object::Array *>(args[0].get());
    return std::make_shared<Object::Integer>(std::ssize(arrObj->m_elements));
  }
  default: {
    return Evaluator::Evaluator::newError(
      std::format("argument to 'len' not supported, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }
  }
}

static std::shared_ptr<Object::IObject>
first(const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (std::ssize(args) != 1) {
    return Evaluator::Evaluator::newError(std::format(
      "wrong number of arguments. got={}. want=1", std::ssize(args)));
  }
  if (args[0].get()->Type() != Object::ObjectType::ARRAY_OBJ) {
    return Evaluator::Evaluator::newError(
      std::format("argument to first must be an ARRAY, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }
  auto arr = dynamic_cast<Object::Array *>(args[0].get());
  auto length = std::ssize(arr->m_elements);
  if (length > 0) {
    return arr->m_elements[0];
  }
  return Evaluator::NULL_O;
}

static std::shared_ptr<Object::IObject>
last(const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (std::ssize(args) != 1) {
    return Evaluator::Evaluator::newError(std::format(
      "wrong number of arguments. got={}. want=1", std::ssize(args)));
  }
  if (args[0].get()->Type() != Object::ObjectType::ARRAY_OBJ) {
    return Evaluator::Evaluator::newError(
      std::format("argument to first must be an ARRAY, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }
  auto arr = dynamic_cast<Object::Array *>(args[0].get());
  auto length = std::ssize(arr->m_elements);
  if (length > 0) {
    return arr->m_elements[static_cast<unsigned long>(length - 1)];
  }
  return Evaluator::NULL_O;
}

static std::shared_ptr<Object::IObject>
rest(const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (std::ssize(args) != 1) {
    return Evaluator::Evaluator::newError(std::format(
      "wrong number of arguments. got={}. want=1", std::ssize(args)));
  }
  if (args[0].get()->Type() != Object::ObjectType::ARRAY_OBJ) {
    return Evaluator::Evaluator::newError(
      std::format("argument to first must be an ARRAY, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }

  auto arr = dynamic_cast<Object::Array *>(args[0].get());
  if (std::ssize(arr->m_elements) > 0) {
    std::vector<std::shared_ptr<Object::IObject>> deepCopy(
      arr->m_elements.begin() + 1, arr->m_elements.end());
    return std::make_shared<Object::Array>(deepCopy);
  }
  return Evaluator::NULL_O;
}

static std::shared_ptr<Object::IObject>
push(const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (std::ssize(args) != 2) {
    return Evaluator::Evaluator::newError(std::format(
      "wrong number of arguments. got={}. want=1", std::ssize(args)));
  }
  if (args[0].get()->Type() != Object::ObjectType::ARRAY_OBJ) {
    return Evaluator::Evaluator::newError(
      std::format("argument to first must be an ARRAY, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }

  auto arr = dynamic_cast<Object::Array *>(args[0].get());

  std::vector<std::shared_ptr<Object::IObject>> deepCopy(
    arr->m_elements.begin(), arr->m_elements.end());
  deepCopy.push_back(args[1]);
  return std::make_shared<Object::Array>(deepCopy);
}

std::unordered_map<std::string, std::shared_ptr<Object::Builtin>> builtins = {
  {"len", std::make_shared<Object::Builtin>(len)},
  {"first", std::make_shared<Object::Builtin>(first)},
  {"last", std::make_shared<Object::Builtin>(last)},
  {"rest", std::make_shared<Object::Builtin>(rest)},
  {"push", std::make_shared<Object::Builtin>(push)},
};
} // namespace Builtins
