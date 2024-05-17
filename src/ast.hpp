#pragma once
#include "token.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Ast {

struct INode {
    virtual ~INode() = default;
    virtual std::string TokenLiteral() = 0;
};

struct IStatement : public INode {
    virtual void statementNode() = 0;
    virtual ~IStatement() = default;
};

struct IExpression : public INode {
    virtual void expressionNode() = 0;
    virtual ~IExpression() = default;
};

struct Program : public INode{
    //Program(std::vector<IStatement*> statements);

    std::vector<std::unique_ptr<IStatement>> m_statements;
    std::string TokenLiteral() override;
};

// the x in "let x = 5;". also the x in "let a = x;:"
struct Identifier : public IExpression{
    Token::Token m_token;
    std::string m_value;

    Identifier(Token::Token token, std::string value);
    std::string TokenLiteral() override;
    void expressionNode() override {};
};


struct LetStatement : public IStatement{
    Token::Token m_token;
    Identifier* m_name;
    IExpression* m_expression;

    LetStatement(Token::Token token);

    std::string TokenLiteral() override;
    void statementNode() override {};

};

} // namespace Ast


