#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "token.hpp"
#include <cassert>
#include <memory>
namespace Evaluator {

std::unique_ptr<Object::IObject> Evaluator::Eval(Ast::INode *node) {
    if (node == nullptr) {
        return nullptr;
    }

    switch (node->Type()) {
    case Ast::Type::PROGRAM: {
        return evalStatements(dynamic_cast<Ast::Program *>(node)->m_statements);
    }
    case (Ast::Type::EXPRESSION_STATEMENT): {
        return Eval(
            dynamic_cast<Ast::ExpressionStatement *>(node)->m_expression.get());
    }
    case Ast::Type::INTEGER_LITERAL: {
        auto integer = dynamic_cast<Ast::IntegerLiteral *>(node);
        return (std::make_unique<Object::Integer>(integer->m_value));
    }
    case Ast::Type::BOOLEAN: {
        auto boolean = dynamic_cast<Ast::Boolean *>(node);
        return (std::make_unique<Object::Boolean>(boolean->m_value));
    }
    case Ast::Type::PREFIX_EXPRESSION: {
        auto prefixExpr = dynamic_cast<Ast::PrefixExpression *>(node);
        auto right = Eval(prefixExpr->m_right.get());
        return evalPrefixExpression(prefixExpr->m_op, std::move(right));
    }
    case Ast::Type::INFIX_EXPRESSION: {
        auto infixExpr = dynamic_cast<Ast::InfixExpression *>(node);
        auto left = Eval(infixExpr->m_left.get());
        auto right = Eval(infixExpr->m_right.get());
        return evalInfixExpression(infixExpr->m_op, left.get(), right.get());
    }
    default:
        return nullptr;
    }
}

std::unique_ptr<Object::IObject> Evaluator::evalStatements(
    std::vector<std::unique_ptr<Ast::IStatement>> &stmts) {
    std::unique_ptr<Object::IObject> result;
    for (auto &statement : stmts) {
        result = Eval(statement.get());
    }
    return result;
}

std::unique_ptr<Object::IObject>
Evaluator::evalPrefixExpression(std::string op,
                                std::unique_ptr<Object::IObject> right) {
    if (op == "!") {
        return evalBangOperatorExpression(right.get());
    } else if (op == "-") {
        return evalMinusPrefixOperatorExpression(right.get());
    } else {
        return std::make_unique<Object::Null>();
    }
}

std::unique_ptr<Object::IObject>
Evaluator::evalBangOperatorExpression(Object::IObject *right) {
    if (right->Type() == Object::ObjectType::BOOLEAN_OBJ) {
        auto boolean = dynamic_cast<Object::Boolean *>(right);
        if (boolean->m_value) {
            return std::make_unique<Object::Boolean>(false);
        } else {

            return std::make_unique<Object::Boolean>(true);
        }
    }

    if (right->Type() == Object::ObjectType::NULL_OBJ) {
        return std::make_unique<Object::Boolean>(true);
    }

    return std::make_unique<Object::Boolean>(false);
}

std::unique_ptr<Object::IObject>
Evaluator::evalMinusPrefixOperatorExpression(Object::IObject *right) {
    if (right->Type() != Object::ObjectType::INTEGER_OBJ) {
        return std::make_unique<Object::Null>();
    }
    auto value = dynamic_cast<Object::Integer *>(right)->m_value;
    return std::make_unique<Object::Integer>(-value);
}

std::unique_ptr<Object::IObject>
Evaluator::evalInfixExpression(std::string op, Object::IObject *left,
                               Object::IObject *right) {
    if (left->Type() == Object::ObjectType::INTEGER_OBJ &&
        right->Type() == Object::ObjectType::INTEGER_OBJ) {
        return evalIntegerInfixExpression(op, left, right);
    } else {
        return std::make_unique<Object::Null>();
    }
}

std::unique_ptr<Object::IObject>
Evaluator::evalIntegerInfixExpression(std::string op, Object::IObject *left,
                                      Object::IObject *right) {
    auto leftVal = dynamic_cast<Object::Integer *>(left)->m_value;
    auto rightVal = dynamic_cast<Object::Integer *>(right)->m_value;
    auto opIter = Token::tokenMap.find(op);

    if (opIter != Token::tokenMap.end()) {
        switch (opIter->second) {
        case Token::PLUS:
            return std::make_unique<Object::Integer>(leftVal + rightVal);
        case Token::MINUS:
            return std::make_unique<Object::Integer>(leftVal - rightVal);
        case Token::ASTERISK:
            return std::make_unique<Object::Integer>(leftVal * rightVal);
        case Token::SLASH:
            return std::make_unique<Object::Integer>(leftVal / rightVal);
        case Token::LT:
            return std::make_unique<Object::Boolean>(leftVal < rightVal);
        case Token::GT:
            return std::make_unique<Object::Boolean>(leftVal > rightVal);
        case Token::EQ:
            return std::make_unique<Object::Boolean>(leftVal == rightVal);
        case Token::NOT_EQ:
            return std::make_unique<Object::Boolean>(leftVal != rightVal);
        default:
            // left blank on purpose
            break;
        }
    }

    return std::make_unique<Object::Null>();
}

} // namespace Evaluator
