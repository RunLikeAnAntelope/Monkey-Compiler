#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Evaluator {

class Evaluator {
  public:
    std::unique_ptr<Object::IObject> Eval(Ast::INode *node);

  private:
    std::unique_ptr<Object::IObject> evalProgram(Ast::Program *program);

    std::unique_ptr<Object::IObject>
    evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);

    static std::unique_ptr<Object::IObject>
    evalPrefixExpression(const std::string &op,
                         std::unique_ptr<Object::IObject> right);

    static std::unique_ptr<Object::IObject>
    evalBangOperatorExpression(Object::IObject *right);

    static std::unique_ptr<Object::IObject>
    evalMinusPrefixOperatorExpression(Object::IObject *right);

    static std::unique_ptr<Object::IObject>
    evalInfixExpression(const std::string &op, Object::IObject *left,
                        Object::IObject *right);

    static std::unique_ptr<Object::IObject>
    evalIntegerInfixExpression(const std::string &op, Object::IObject *left,
                               Object::IObject *right);

    static std::unique_ptr<Object::IObject>
    evalBooleanInfixExpression(const std::string &op, Object::IObject *left,
                               Object::IObject *right);

    static bool isTruthy(const Object::IObject *const obj);

    std::unique_ptr<Object::IObject>
    evalIfExpression(const Ast::IfExpression *const ifExpr);

    static std::unique_ptr<Object::Error> newError(const std::string &errorMsg);
    static bool isError(const Object::IObject *const obj);
};

} // namespace Evaluator
