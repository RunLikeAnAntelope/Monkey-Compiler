#pragma once
#include "token.hpp"
#include <memory>
#include <string>
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

struct Program : public INode {
    std::vector<std::unique_ptr<IStatement>> m_statements;
    std::string TokenLiteral() override;
    std::string String() override;
};

// the x in "let x = 5;". also the x in "let a = x;:"
struct Identifier : public IExpression {
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

struct InfixExpression : public IExpression {
    Token::Token m_token;
    std::unique_ptr<IExpression> m_left;
    std::string m_op;
    std::unique_ptr<IExpression> m_right;

    InfixExpression(Token::Token t, std::unique_ptr<IExpression> left,
                    std::string op);
    void expressionNode() override {};
    std::string TokenLiteral() override;
    std::string String() override;
};

struct Boolean : public IExpression {
    Token::Token m_token;
    bool m_value;

    Boolean(Token::Token token);
    std::string TokenLiteral() override;
    void expressionNode() override {};
    std::string String() override;
};

// Unhappy I need to order this way. Might be better to have seperate files for
// each class, but I am trying to follow the example to the best of my
// abilities.
struct BlockStatement : public IStatement {
    Token::Token m_token;
    std::vector<std::unique_ptr<IStatement>> m_statements;

    BlockStatement(Token::Token token);
    std::string TokenLiteral() override;
    void statementNode() override {};
    std::string String() override;
};

struct IfExpression : public IExpression {
    Token::Token m_token;
    std::unique_ptr<IExpression> m_condition;
    std::unique_ptr<BlockStatement> m_consequence;
    std::unique_ptr<BlockStatement> m_alternative;

    IfExpression(Token::Token token);
    void expressionNode() override {};
    std::string TokenLiteral() override;
    std::string String() override;
};

struct LetStatement : public IStatement {
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
    void statementNode() override {};
    std::string String() override;
};

struct ExpressionStatement : public IStatement {
    Token::Token m_token;
    std::unique_ptr<IExpression> m_expression;

    ExpressionStatement(Token::Token token);

    std::string TokenLiteral() override;
    void statementNode() override {};
    std::string String() override;
};

} // namespace Ast
