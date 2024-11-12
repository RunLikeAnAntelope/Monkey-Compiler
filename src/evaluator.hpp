#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>

namespace Evaluator {

class Evaluator {
  public:
    std::unique_ptr<Object::IObject> Eval(Ast::INode *node);

  private:
    std::unique_ptr<Object::IObject> evalProgram(Ast::Program *program);

    std::unique_ptr<Object::IObject>
    evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);

    std::unique_ptr<Object::IObject>
    evalPrefixExpression(std::string op,
                         std::unique_ptr<Object::IObject> right);

    std::unique_ptr<Object::IObject>
    evalBangOperatorExpression(Object::IObject *right);

    std::unique_ptr<Object::IObject>
    evalMinusPrefixOperatorExpression(Object::IObject *right);

    std::unique_ptr<Object::IObject> evalInfixExpression(std::string op,
                                                         Object::IObject *right,
                                                         Object::IObject *left);

    std::unique_ptr<Object::IObject>
    evalIntegerInfixExpression(std::string op, Object::IObject *right,
                               Object::IObject *left);

    bool isTruthy(const Object::IObject *const obj) const;

    std::unique_ptr<Object::IObject>
    evalIfExpression(const Ast::IfExpression *const ie);
};

} // namespace Evaluator
