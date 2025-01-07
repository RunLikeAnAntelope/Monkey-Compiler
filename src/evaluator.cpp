#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "token.hpp"
#include <cassert>
#include <format>
#include <memory>
#include <vector>
namespace Evaluator {
static std::shared_ptr<Object::Boolean> FALSE =
  std::make_unique<Object::Boolean>(false);
static std::shared_ptr<Object::Boolean> TRUE =
  std::make_shared<Object::Boolean>(true);
static std::shared_ptr<Object::Null> NULL_O = std::make_shared<Object::Null>();

static std::shared_ptr<Object::Boolean> nativeBoolToBoolObject(bool boolean) {
  return boolean ? TRUE : FALSE;
}

// Environment stuff
std::shared_ptr<Object::IObject>
Evaluator::Eval(Ast::INode *node,
                const std::shared_ptr<Object::Environment> &env) {
  if (node == nullptr) {
    return nullptr;
  }

  switch (node->Type()) {
  case Ast::Type::PROGRAM: {
    return evalProgram(dynamic_cast<Ast::Program *>(node), env);
  }
  case (Ast::Type::EXPRESSION_STATEMENT): {
    return Eval(
      dynamic_cast<Ast::ExpressionStatement *>(node)->m_expression.get(), env);
  }
  case Ast::Type::INTEGER_LITERAL: {
    auto *integer = dynamic_cast<Ast::IntegerLiteral *>(node);
    return (std::make_shared<Object::Integer>(integer->m_value));
  }
  case Ast::Type::BOOLEAN: {
    auto *boolean = dynamic_cast<Ast::Boolean *>(node);
    return nativeBoolToBoolObject(boolean->m_value);
  }
  case Ast::Type::PREFIX_EXPRESSION: {
    auto *prefixExpr = dynamic_cast<Ast::PrefixExpression *>(node);
    auto right = Eval(prefixExpr->m_right.get(), env);
    if (isError(right.get())) {
      return right;
    }
    return evalPrefixExpression(prefixExpr->m_op, right);
  }
  case Ast::Type::INFIX_EXPRESSION: {
    auto *infixExpr = dynamic_cast<Ast::InfixExpression *>(node);
    auto left = Eval(infixExpr->m_left.get(), env);
    if (isError(left.get())) {
      return left;
    }
    auto right = Eval(infixExpr->m_right.get(), env);
    if (isError(right.get())) {
      return right;
    }
    return evalInfixExpression(infixExpr->m_op, left.get(), right.get());
  }
  case Ast::Type::BLOCK_STATEMENT: {
    auto *blockStmt = dynamic_cast<Ast::BlockStatement *>(node);
    return evalStatements(blockStmt->m_statements, env);
  }
  case Ast::Type::IF_EXPRESSION: {
    auto *ifStmt = dynamic_cast<Ast::IfExpression *>(node);
    return evalIfExpression(ifStmt, env);
  }
  case Ast::Type::RETURN_STATEMENT: {
    auto *returnStmt = dynamic_cast<Ast::ReturnStatement *>(node);
    auto val = Eval(returnStmt->m_returnValue.get(), env);
    if (isError(val.get())) {
      return val;
    }
    return std::make_shared<Object::ReturnValue>(val);
  }
  case Ast::Type::LET_STATEMENT: {
    auto *letStmt = dynamic_cast<Ast::LetStatement *>(node);
    auto val = Eval(letStmt->m_expression.get(), env);
    if (isError(val.get())) {
      return val;
    }
    env->Set(letStmt->m_name->m_value, val);
    return nullptr;
  }
  case Ast::Type::IDENTIFIER: {
    auto *ident = dynamic_cast<Ast::Identifier *>(node);
    auto lookup = env->Get(ident->m_value);
    if (!lookup.ok) {
      return newError(std::format("identifier not found: {}", ident->m_value));
    }
    return lookup.obj;
  }
  case Ast::Type::FUNCTION_LITERAL: {
    auto *fun = dynamic_cast<Ast::FunctionLiteral *>(node);
    auto params = std::move(fun->m_parameters);
    auto body = std::move(fun->m_body);
    return std::make_shared<Object::Function>(std::move(params), std::move(env),
                                              std::move(body));
  }

  case Ast::Type::CALL_EXPRESSION: {
    auto *callExpr = dynamic_cast<Ast::CallExpression *>(node);
    auto function = Eval(callExpr->m_function.get(), env);
    if (isError(function.get())) {
      return function;
    }
    auto args = evalExpressions(callExpr->m_arguments, env);
    if (std::ssize(args) == 1 && isError(args[0].get())) {
      return args[0];
    }

    return applyFunction(function, args);
  }

  default:
    return nullptr;
  }
}

std::shared_ptr<Object::IObject> Evaluator::applyFunction(
  const std::shared_ptr<Object::IObject> &fn,
  const std::vector<std::shared_ptr<Object::IObject>> &args) {
  if (fn->Type() != Object::ObjectType::FUNCTION_OBJ) {
    return newError(
      std::format("not a function: {}", Object::objectTypeToStr(fn->Type())));
  }
  auto *function = dynamic_cast<Object::Function *>(fn.get());
  auto extendedEnv = extendFunctionEnv(function, args);
  auto evaluated = Eval(function->m_body.get(), extendedEnv);
  return unwrapReturnValue(evaluated);
}

std::shared_ptr<Object::IObject>
Evaluator::unwrapReturnValue(std::shared_ptr<Object::IObject> obj) {
  if (obj->Type() == Object::ObjectType::RETURN_VALUE_OBJ) {
    return dynamic_cast<Object::ReturnValue *>(obj.get())->m_value;
  }
  return obj;
}

std::shared_ptr<Object::Environment> Evaluator::extendFunctionEnv(
  Object::Function *fn,
  const std::vector<std::shared_ptr<Object::IObject>> &args) {
  std::shared_ptr<Object::Environment> env =
    std::make_shared<Object::Environment>(fn->m_env);

  size_t index = 0;
  for (const auto &param : fn->m_parameters) {
    env->Set(param->m_value, args[index]);
    index++;
  }
  return env;
}

std::vector<std::shared_ptr<Object::IObject>> Evaluator::evalExpressions(
  const std::vector<std::unique_ptr<Ast::IExpression>> &exps,
  const std::shared_ptr<Object::Environment> &env) {
  std::vector<std::shared_ptr<Object::IObject>> result;
  for (const auto &e : exps) {
    auto evaluated = Eval(e.get(), env);
    if (isError(evaluated.get())) {
      result.push_back(evaluated);
      return result;
    }
    result.push_back(evaluated);
  }
  return result;
}

std::shared_ptr<Object::IObject>
Evaluator::evalProgram(Ast::Program *program,
                       const std::shared_ptr<Object::Environment> &env) {
  std::shared_ptr<Object::IObject> result;
  for (auto &statement : program->m_statements) {
    result = Eval(statement.get(), env);
    if (result) {
      if (result->Type() == Object::ObjectType::RETURN_VALUE_OBJ) {
        return dynamic_cast<Object::ReturnValue *>(result.get())->m_value;
      } else if (result->Type() == Object::ObjectType::ERROR_OBJ) {
        return result;
      }
    }
  }
  return result;
}

std::shared_ptr<Object::IObject>
Evaluator::evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts,
                          const std::shared_ptr<Object::Environment> &env) {
  std::shared_ptr<Object::IObject> result;
  for (auto &statement : stmts) {
    result = Eval(statement.get(), env);
    if (result != nullptr &&
        (result->Type() == Object::ObjectType::RETURN_VALUE_OBJ ||
         result->Type() == Object::ObjectType::ERROR_OBJ)) {
      return result;
    }
  }
  return result;
}

std::shared_ptr<Object::IObject>
Evaluator::evalPrefixExpression(const std::string &op,
                                const std::shared_ptr<Object::IObject> &right) {
  if (op == "!") {
    return evalBangOperatorExpression(right.get());
  } else if (op == "-") {
    return evalMinusPrefixOperatorExpression(right.get());
  } else {
    return newError(std::format("unknown operator: {}{}", op,
                                Object::objectTypeToStr(right->Type())));
  }
}

std::shared_ptr<Object::IObject>
Evaluator::evalBangOperatorExpression(Object::IObject *right) {
  if (right->Type() == Object::ObjectType::BOOLEAN_OBJ) {
    auto *boolean = dynamic_cast<Object::Boolean *>(right);
    if (boolean->m_value) {
      return FALSE;
    } else {
      return TRUE;
    }
  }

  if (right->Type() == Object::ObjectType::NULL_OBJ) {
    return NULL_O;
  }

  return FALSE;
}

std::shared_ptr<Object::IObject>
Evaluator::evalMinusPrefixOperatorExpression(Object::IObject *right) {
  if (right->Type() != Object::ObjectType::INTEGER_OBJ) {
    return newError(std::format("unknown operator: -{}",
                                Object::objectTypeToStr(right->Type())));
  }
  auto value = dynamic_cast<Object::Integer *>(right)->m_value;
  return std::make_shared<Object::Integer>(-value);
}

std::shared_ptr<Object::IObject>
Evaluator::evalInfixExpression(const std::string &op, Object::IObject *left,
                               Object::IObject *right) {

  if (left->Type() == Object::ObjectType::INTEGER_OBJ &&
      right->Type() == Object::ObjectType::INTEGER_OBJ) {
    return evalIntegerInfixExpression(op, left, right);
  }

  if (left->Type() == Object::ObjectType::BOOLEAN_OBJ &&
      right->Type() == Object::ObjectType::BOOLEAN_OBJ) {
    return evalBooleanInfixExpression(op, left, right);
  }

  if (left->Type() != right->Type()) {
    return newError(std::format("type mismatch: {} {} {}",
                                Object::objectTypeToStr(left->Type()), op,
                                Object::objectTypeToStr(right->Type())));
  }

  return newError(std::format("unknown operator: {} {} {}",
                              Object::objectTypeToStr(left->Type()), op,
                              Object::objectTypeToStr(right->Type())));
}

std::shared_ptr<Object::IObject> Evaluator::evalBooleanInfixExpression(
  const std::string &op, Object::IObject *left, Object::IObject *right) {
  auto *leftPtr = dynamic_cast<Object::Boolean *>(left);
  auto *rightPtr = dynamic_cast<Object::Boolean *>(right);

  auto leftVal = leftPtr->m_value;
  auto rightVal = rightPtr->m_value;
  if (Token::tokenMap.find(op) != Token::tokenMap.end()) {
    switch (Token::tokenMap.at(op)) {
    case Token::EQ:
      return nativeBoolToBoolObject(leftVal == rightVal);
    case Token::NOT_EQ:
      return nativeBoolToBoolObject(leftVal != rightVal);
    default:
      return std::make_shared<Object::Error>(
        std::format("Unsupported infix operator for booleans. Got {} "
                    "expected == or !=",
                    op));
    }
  } else {
    return std::make_shared<Object::Error>(
      std::format("{} is not a valid operator", op));
  }
}

std::shared_ptr<Object::IObject> Evaluator::evalIntegerInfixExpression(
  const std::string &op, Object::IObject *left, Object::IObject *right) {
  auto leftVal = dynamic_cast<Object::Integer *>(left)->m_value;
  auto rightVal = dynamic_cast<Object::Integer *>(right)->m_value;
  auto opIter = Token::tokenMap.find(op);

  if (opIter != Token::tokenMap.end()) {
    switch (opIter->second) {
    case Token::PLUS:
      return std::make_shared<Object::Integer>(leftVal + rightVal);
    case Token::MINUS:
      return std::make_shared<Object::Integer>(leftVal - rightVal);
    case Token::ASTERISK:
      return std::make_shared<Object::Integer>(leftVal * rightVal);
    case Token::SLASH:
      return std::make_shared<Object::Integer>(leftVal / rightVal);
    case Token::LT:
      return nativeBoolToBoolObject(leftVal < rightVal);
    case Token::GT:
      return nativeBoolToBoolObject(leftVal > rightVal);
    case Token::EQ:
      return nativeBoolToBoolObject(leftVal == rightVal);
    case Token::NOT_EQ:
      return nativeBoolToBoolObject(leftVal != rightVal);
    default:
      return newError(std::format("unknown operator: {} {} {}",
                                  Object::objectTypeToStr(left->Type()), op,
                                  Object::objectTypeToStr(right->Type())));
    }
  }
  return NULL_O;
}

bool Evaluator::isTruthy(const Object::IObject *const obj) {
  switch (obj->Type()) {
  case Object::ObjectType::NULL_OBJ:
    return false;
  case Object::ObjectType::BOOLEAN_OBJ: {
    const auto *boolObj = dynamic_cast<const Object::Boolean *>(obj);
    return boolObj->m_value;
  }
  default:
    return true;
  }
}

std::shared_ptr<Object::IObject>
Evaluator::evalIfExpression(const Ast::IfExpression *const ifExpr,
                            const std::shared_ptr<Object::Environment> &env) {
  auto condition = Eval(ifExpr->m_condition.get(), env);
  if (isError(condition.get())) {
    return condition;
  }
  if (isTruthy(condition.get())) {
    return Eval(ifExpr->m_consequence.get(), env);
  } else if (ifExpr->m_alternative != nullptr) {
    return Eval(ifExpr->m_alternative.get(), env);
  } else {
    return NULL_O;
  }
}

std::shared_ptr<Object::Error>
Evaluator::newError(const std::string &errorMsg) {
  return std::make_shared<Object::Error>(errorMsg);
}

bool Evaluator::isError(const Object::IObject *const obj) {
  if (obj != nullptr) {
    return obj->Type() == Object::ObjectType::ERROR_OBJ;
  }
  return false;
}

} // namespace Evaluator
