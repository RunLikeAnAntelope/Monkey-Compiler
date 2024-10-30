#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>

namespace Evaluator {

std::unique_ptr<Object::IObject> Eval(std::unique_ptr<Ast::INode> node);

} // namespace Evaluator
