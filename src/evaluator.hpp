#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>

namespace Evaluator {

std::unique_ptr<Object::IObject>
evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);

std::unique_ptr<Object::IObject> Eval(Ast::INode *node);

} // namespace Evaluator
