#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>
#include <string>
#include <vector>
namespace Evaluator {

class Evaluator {
public:
  std::shared_ptr<Object::IObject> Eval(Ast::INode *node);
  std::shared_ptr<Object::IObject> evalProgram(Ast::Program *program);

private:
  // Environment
  std::shared_ptr<Object::Environment> env =
    std::make_shared<Object::Environment>();

  // methods
  std::shared_ptr<Object::IObject>
  evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);

  static std::shared_ptr<Object::IObject>
  evalPrefixExpression(const std::string &op,
                       const std::shared_ptr<Object::IObject> &right);

  static std::shared_ptr<Object::IObject>
  evalBangOperatorExpression(Object::IObject *right);

  static std::shared_ptr<Object::IObject>
  evalMinusPrefixOperatorExpression(Object::IObject *right);

  static std::shared_ptr<Object::IObject>
  evalInfixExpression(const std::string &op, Object::IObject *left,
                      Object::IObject *right);

  static std::shared_ptr<Object::IObject>
  evalIntegerInfixExpression(const std::string &op, Object::IObject *left,
                             Object::IObject *right);

  static std::shared_ptr<Object::IObject>
  evalBooleanInfixExpression(const std::string &op, Object::IObject *left,
                             Object::IObject *right);

  static bool isTruthy(const Object::IObject *const obj);

  std::shared_ptr<Object::IObject>
  evalIfExpression(const Ast::IfExpression *const ifExpr);

  static std::shared_ptr<Object::Error> newError(const std::string &errorMsg);

  static bool isError(const Object::IObject *const obj);
};

} // namespace Evaluator
