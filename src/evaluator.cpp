#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
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
        return (boolean->m_value ? std::move(TRUE) : std::move(FALSE));
    }
    }
    return nullptr;
}

std::unique_ptr<Object::IObject> Evaluator::evalStatements(
    std::vector<std::unique_ptr<Ast::IStatement>> &stmts) {
    std::unique_ptr<Object::IObject> result;
    for (auto &statement : stmts) {
        result = Eval(statement.get());
    }
    return result;
}

} // namespace Evaluator
