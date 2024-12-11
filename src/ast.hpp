#pragma once
#include "token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Ast {

enum class Type : std::uint8_t {
  INODE,
  ISTATEMENT,
  IEXPRESSION,
  PROGRAM,
  IDENTIFIER,
  INTEGER_LITERAL,
  PREFIX_EXPRESSION,
  INFIX_EXPRESSION,
  BOOLEAN,
  BLOCK_STATEMENT,
  IF_EXPRESSION,
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
  FUNCTION_LITERAL,
  CALL_EXPRESSION,
  STRING_LITERAL
};

struct INode {
  virtual ~INode() = default;
  virtual std::string TokenLiteral() = 0;
  virtual std::string String() = 0;
  virtual Type Type() = 0;
};

struct IStatement : public INode {
  virtual void statementNode() = 0;
  ~IStatement() override = default;
  enum Type Type() override = 0;
};

struct IExpression : public INode {
  virtual void expressionNode() = 0;
  ~IExpression() override = default;
  enum Type Type() override = 0;
};

struct Program : public INode {
  std::vector<std::unique_ptr<IStatement>> m_statements;
  std::string TokenLiteral() override;
  std::string String() override;
  enum Type Type() override;
};

// the x in "let x = 5;". also the x in "let a = x;:"
struct Identifier : public IExpression {
  Token::Token m_token;
  std::string m_value;

  Identifier(Token::Token token, std::string value);
  std::string TokenLiteral() override;
  void expressionNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct IntegerLiteral : public IExpression {
  Token::Token m_token;
  long int m_value;

  IntegerLiteral(Token::Token token, long int value);
  std::string TokenLiteral() override;
  void expressionNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct PrefixExpression : public IExpression {
  Token::Token m_token;
  std::string m_op;
  std::unique_ptr<IExpression> m_right;

  PrefixExpression(Token::Token t, std::string op);
  std::string TokenLiteral() override;
  void expressionNode() override {};
  std::string String() override;
  enum Type Type() override;
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
  enum Type Type() override;
};

struct Boolean : public IExpression {
  Token::Token m_token;
  bool m_value;

  Boolean(Token::Token token, bool value);
  std::string TokenLiteral() override;
  void expressionNode() override {};
  std::string String() override;
  enum Type Type() override;
};

// Unhappy I need to order this way. Might be better to have seperate files for
// each class, but I am trying to follow the example to the best of my
// abilities.
struct BlockStatement : public IStatement {
  Token::Token m_token;
  std::vector<std::unique_ptr<IStatement>> m_statements;

  explicit BlockStatement(Token::Token token);
  std::string TokenLiteral() override;
  void statementNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct IfExpression : public IExpression {
  Token::Token m_token;
  std::unique_ptr<IExpression> m_condition;
  std::unique_ptr<BlockStatement> m_consequence;
  std::unique_ptr<BlockStatement> m_alternative;

  explicit IfExpression(Token::Token token);
  void expressionNode() override {};
  std::string TokenLiteral() override;
  std::string String() override;
  enum Type Type() override;
};

struct LetStatement : public IStatement {
  Token::Token m_token;
  std::unique_ptr<Identifier> m_name;
  std::unique_ptr<IExpression> m_expression;

  explicit LetStatement(Token::Token token);

  std::string TokenLiteral() override;
  void statementNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct ReturnStatement : public IStatement {
  Token::Token m_token;
  std::unique_ptr<IExpression> m_returnValue;

  explicit ReturnStatement(Token::Token token);
  std::string TokenLiteral() override;
  void statementNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct ExpressionStatement : public IStatement {
  Token::Token m_token;
  std::unique_ptr<IExpression> m_expression;

  explicit ExpressionStatement(Token::Token token);

  std::string TokenLiteral() override;
  void statementNode() override {};
  std::string String() override;
  enum Type Type() override;
};

struct FunctionLiteral : public IExpression {
  Token::Token m_token;
  std::vector<std::unique_ptr<Identifier>> m_parameters;
  std::unique_ptr<BlockStatement> m_body;

  explicit FunctionLiteral(Token::Token token);
  void expressionNode() override {};
  std::string TokenLiteral() override;
  std::string String() override;
  enum Type Type() override;
};

struct CallExpression : public IExpression {
  Token::Token m_token;                    // the '(' token
  std::unique_ptr<IExpression> m_function; // Identifier or FunctionLiteral
  std::vector<std::unique_ptr<IExpression>> m_arguments;

  CallExpression(Token::Token token, std::unique_ptr<IExpression> function);
  void expressionNode() override {};
  std::string TokenLiteral() override;
  std::string String() override;
  enum Type Type() override;
};

struct StringLiteral : public IExpression {
  Token::Token m_token;
  std::string m_value;

  explicit StringLiteral(Token::Token token, std::string m_value);
  void expressionNode() override {};
  std::string TokenLiteral() override;
  std::string String() override;
  enum Type Type() override;
};

} // namespace Ast
