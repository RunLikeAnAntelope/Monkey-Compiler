#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>
#include <string>
#include <vector>
namespace Evaluator {

extern std::shared_ptr<Object::Null> NULL_O;
extern std::shared_ptr<Object::Boolean> TRUE;
extern std::shared_ptr<Object::Boolean> FALSE;
class Evaluator {
public:
  std::shared_ptr<Object::IObject>
  Eval(Ast::INode *node, const std::shared_ptr<Object::Environment> &env);
  std::shared_ptr<Object::IObject>
  evalProgram(Ast::Program *program,
              const std::shared_ptr<Object::Environment> &env);

  // public so it can be used elsewhere
  static std::shared_ptr<Object::Error> newError(const std::string &errorMsg);

private:
  // methods
  std::shared_ptr<Object::IObject>
  evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts,
                 const std::shared_ptr<Object::Environment> &env);

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

  static std::shared_ptr<Object::IObject>
  evalStringInfixExpression(const std::string &op, Object::IObject *left,
                            Object::IObject *right);

  static bool isTruthy(const Object::IObject *const obj);

  std::shared_ptr<Object::IObject>
  evalIfExpression(const Ast::IfExpression *const ifExpr,
                   const std::shared_ptr<Object::Environment> &env);

  static bool isError(const Object::IObject *const obj);

  std::vector<std::shared_ptr<Object::IObject>>
  evalExpressions(const std::vector<std::unique_ptr<Ast::IExpression>> &exps,
                  const std::shared_ptr<Object::Environment> &env);

  std::shared_ptr<Object::IObject>
  applyFunction(const std::shared_ptr<Object::IObject> &fn,
                const std::vector<std::shared_ptr<Object::IObject>> &args);

  static std::shared_ptr<Object::Environment>
  extendFunctionEnv(Object::Function *fn,
                    const std::vector<std::shared_ptr<Object::IObject>> &args);

  static std::shared_ptr<Object::IObject>
  unwrapReturnValue(std::shared_ptr<Object::IObject> obj);

  static std::shared_ptr<Object::IObject>
  evalIdentifier(Ast::INode *node,
                 const std::shared_ptr<Object::Environment> &env);

  static std::shared_ptr<Object::IObject>
  evalIndexExpression(const std::shared_ptr<Object::IObject> &left,
                      const std::shared_ptr<Object::IObject> &index);

  static std::shared_ptr<Object::IObject>
  evalArrayIndexExpression(const std::shared_ptr<Object::IObject> &array,
                           const std::shared_ptr<Object::IObject> &index);
};

} // namespace Evaluator
