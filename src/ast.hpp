#pragma once
#include "token.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Ast {

struct INode {
    virtual ~INode() = default;
    virtual std::string TokenLiteral() = 0;
    virtual std::string String() = 0;
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
    std::string String() override;
};

// the x in "let x = 5;". also the x in "let a = x;:"
struct Identifier : public IExpression{
    Token::Token m_token;
    std::string m_value;

    Identifier(Token::Token token, std::string value);
    std::string TokenLiteral() override;
    void expressionNode() override {};
    std::string String() override;
};

struct IntegerLiteral : public IExpression {
    Token::Token m_token;
    long int m_value;

    IntegerLiteral(Token::Token token);
    std::string TokenLiteral() override;
    void expressionNode() override {};
    std::string String() override;
};

struct PrefixExpression : public IExpression {
    Token::Token m_token;
    std::string m_op;
    std::unique_ptr<IExpression> m_right;

    PrefixExpression(Token::Token t, std::string op);
    std::string TokenLiteral() override;
    void expressionNode() override {};
    std::string String() override;
};


struct LetStatement : public IStatement{
    Token::Token m_token;
    std::unique_ptr<Identifier> m_name;
    std::unique_ptr<IExpression> m_expression;

    LetStatement(Token::Token token);

    std::string TokenLiteral() override;
    void statementNode() override {};
    std::string String() override;
};

struct ReturnStatement : public IStatement {
    Token::Token m_token;
    std::unique_ptr<IExpression> m_returnValue;
 
    ReturnStatement(Token::Token token);
    std::string TokenLiteral() override;
    void statementNode() override{};
    std::string String() override;
};

struct ExpressionStatement : public IStatement {
    Token::Token m_token;
    std::unique_ptr<IExpression> m_expression;

    ExpressionStatement(Token::Token token);

    std::string TokenLiteral() override;
    void statementNode() override{};
    std::string String() override;
};


} // namespace Ast


