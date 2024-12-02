#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "token.hpp"
#include <cassert>
#include <format>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
namespace Evaluator {
std::shared_ptr<Object::Boolean> FALSE =
    std::make_unique<Object::Boolean>(false);
std::shared_ptr<Object::Boolean> TRUE = std::make_shared<Object::Boolean>(true);
std::shared_ptr<Object::Null> NULL_O = std::make_shared<Object::Null>();

std::shared_ptr<Object::Boolean> nativeBoolToBoolObject(bool boolean) {
    return boolean ? TRUE : FALSE;
}
// Environment stuff
Environment::EnvObj Environment::Get(std::string name) {
    if (this->m_environment.contains(name)) {
        return {this->m_environment[name], true};
    } else {
        return {nullptr, false};
    }
}
void Environment::Set(std::string name, std::shared_ptr<Object::IObject> obj) {
    this->m_environment[name] = obj;
}

// Environment stuff
std::shared_ptr<Object::IObject> Evaluator::Eval(Ast::INode *node) {
    if (node == nullptr) {
        return nullptr;
    }

    switch (node->Type()) {
    case Ast::Type::PROGRAM: {
        return evalProgram(dynamic_cast<Ast::Program *>(node));
    }
    case (Ast::Type::EXPRESSION_STATEMENT): {
        return Eval(
            dynamic_cast<Ast::ExpressionStatement *>(node)->m_expression.get());
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
        auto right = Eval(prefixExpr->m_right.get());
        if (isError(right.get())) {
            return right;
        }
        return evalPrefixExpression(prefixExpr->m_op, std::move(right));
    }
    case Ast::Type::INFIX_EXPRESSION: {
        auto *infixExpr = dynamic_cast<Ast::InfixExpression *>(node);
        auto left = Eval(infixExpr->m_left.get());
        if (isError(left.get())) {
            return left;
        }
        auto right = Eval(infixExpr->m_right.get());
        if (isError(right.get())) {
            return right;
        }
        return evalInfixExpression(infixExpr->m_op, left.get(), right.get());
    }
    case Ast::Type::BLOCK_STATEMENT: {
        auto *blockStmt = dynamic_cast<Ast::BlockStatement *>(node);
        return evalStatements(blockStmt->m_statements);
    }
    case Ast::Type::IF_EXPRESSION: {
        auto *ifStmt = dynamic_cast<Ast::IfExpression *>(node);
        return evalIfExpression(ifStmt);
    }
    case Ast::Type::RETURN_STATEMENT: {
        auto *returnStmt = dynamic_cast<Ast::ReturnStatement *>(node);
        auto val = Eval(returnStmt->m_returnValue.get());
        if (isError(val.get())) {
            return val;
        }
        return std::make_shared<Object::ReturnValue>(val);
    }
    case Ast::Type::LET_STATEMENT: {
        auto *letStmt = dynamic_cast<Ast::LetStatement *>(node);
        auto val = Eval(letStmt->m_expression.get());
        if (isError(val.get())) {
            return val;
        }
        env.Set(letStmt->m_name->m_value, val);
        return nullptr;
    }
    case Ast::Type::IDENTIFIER: {
        auto *ident = dynamic_cast<Ast::Identifier *>(node);
        auto lookup = env.Get(ident->m_value);
        if (!lookup.ok) {
            return newError(
                std::format("identifier not found: {}", ident->m_value));
        }
        return lookup.obj;
    }

    default:
        return nullptr;
    }
}

std::shared_ptr<Object::IObject> Evaluator::evalProgram(Ast::Program *program) {
    std::shared_ptr<Object::IObject> result;
    for (auto &statement : program->m_statements) {
        result = Eval(statement.get());
        if (result) {
            if (result->Type() == Object::ObjectType::RETURN_VALUE_OBJ) {
                return dynamic_cast<Object::ReturnValue *>(result.get())
                    ->m_value;
            } else if (result->Type() == Object::ObjectType::ERROR_OBJ) {
                return result;
            }
        }
    }
    return result;
}

std::shared_ptr<Object::IObject> Evaluator::evalStatements(
    std::vector<std::unique_ptr<Ast::IStatement>> &stmts) {
    std::shared_ptr<Object::IObject> result;
    for (auto &statement : stmts) {
        result = Eval(statement.get());
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
                                std::shared_ptr<Object::IObject> right) {
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
            return newError(
                std::format("unknown operator: {} {} {}",
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
Evaluator::evalIfExpression(const Ast::IfExpression *const ifExpr) {
    auto condition = Eval(ifExpr->m_condition.get());
    if (isError(condition.get())) {
        return condition;
    }
    if (isTruthy(condition.get())) {
        return Eval(ifExpr->m_consequence.get());
    } else if (ifExpr->m_alternative != nullptr) {
        return Eval(ifExpr->m_alternative.get());
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
