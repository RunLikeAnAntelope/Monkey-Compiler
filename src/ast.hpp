#pragma once
#include "token.hpp"
#include <string>
#include <vector>

namespace Ast {

struct INode {
    virtual ~INode() = default;
    virtual std::string TokenLiteral()= 0;
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
    Program(std::vector<IStatement*> statements);
    ~Program() = default;

    std::vector<IStatement*> m_statements;
    std::string TokenLiteral() override;
};

struct Identifier {
    Token::Token m_token;
    std::string m_value;
};


struct LetStatement : IStatement{
    LetStatement(){};
    ~LetStatement(){};

    Token::Token m_token;
    Identifier Name;
    IExpression* m_expression;

    std::string TokenLiteral() override;

};

} // namespace Ast


