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
  default: {
    return Evaluator::Evaluator::newError(
      std::format("argument to 'len' not supported, got {}",
                  Object::objectTypeToStr(args[0].get()->Type())));
  }
  }
}

std::unordered_map<std::string, std::shared_ptr<Object::Builtin>> builtins = {
  {"len", std::make_shared<Object::Builtin>(len)}};
} // namespace Builtins
