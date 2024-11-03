#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>

namespace Evaluator {
class Evaluator {
  public:
    std::unique_ptr<Object::IObject> Eval(Ast::INode *node);

  private:
    std::unique_ptr<Object::IObject>
    evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);

    std::unique_ptr<Object::IObject>
    evalPrefixExpression(std::string op,
                         std::unique_ptr<Object::IObject> right);

    std::unique_ptr<Object::IObject>
    evalBangOperatorExpression(Object::IObject *right);

    std::unique_ptr<Object::IObject>
    evalMinusPrefixOperatorExpression(Object::IObject *right);
};

} // namespace Evaluator
