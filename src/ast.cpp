#include "ast.hpp"
#include "helpers.hpp"
#include <memory>
#include <utility>

namespace Ast {

// All the type stuff in the same place
Type INode::Type() { return Type::INODE; }
Type IExpression::Type() { return Type::IEXPRESSION; }
Type Program::Type() { return Type::PROGRAM; }
Type Identifier::Type() { return Type::IDENTIFIER; }
Type IntegerLiteral::Type() { return Type::INTEGER_LITERAL; }
Type PrefixExpression::Type() { return Type::PREFIX_EXPRESSION; }
Type InfixExpression::Type() { return Type::INFIX_EXPRESSION; }
Type Boolean::Type() { return Type::BOOLEAN; }
Type BlockStatement::Type() { return Type::BLOCK_STATEMENT; }
Type IfExpression::Type() { return Type::IF_EXPRESSION; }
Type LetStatement::Type() { return Type::LET_STATEMENT; }
Type ReturnStatement::Type() { return Type::RETURN_STATEMENT; }
Type ExpressionStatement::Type() { return Type::EXPRESSION_STATEMENT; }
Type FunctionLiteral::Type() { return Type::FUNCTION_LITERAL; }
Type CallExpression::Type() { return Type::CALL_EXPRESSION; }
Type StringLiteral::Type() { return Type::STRING_LITERAL; }

// Program stuff
std::string Program::TokenLiteral() {
  if (m_statements.size() > 0) {
    return m_statements[0]->TokenLiteral();
  } else {
    return "";
  }
}

std::string Program::String() {
  std::string out;
  for (auto const &s : m_statements) {
    out.append(s->String());
  }
  return out;
}

// Identifier stuff
Identifier::Identifier(Token::Token token, std::string value)
  : m_token(std::move(token)), m_value(std::move(value)) {};
std::string Identifier::TokenLiteral() { return m_token.Literal; }

std::string Identifier::String() { return m_value; }

// Integer Literal stuff
IntegerLiteral::IntegerLiteral(Token::Token token, long int value)
  : m_token(std::move(token)), m_value(value) {}
std::string IntegerLiteral::String() { return m_token.Literal; }

std::string IntegerLiteral::TokenLiteral() { return m_token.Literal; }

// PrefixExpression stuff
PrefixExpression::PrefixExpression(Token::Token t, std::string op)
  : m_token(std::move(t)), m_op(std::move(op)) {}

std::string PrefixExpression::TokenLiteral() { return m_token.Literal; }

std::string PrefixExpression::String() {
  std::string out;
  out.append("(");
  out.append(m_op);
  out.append(m_right->String());
  out.append(")");
  return out;
}

// InfixExpression stuff
InfixExpression::InfixExpression(Token::Token t,
                                 std::unique_ptr<IExpression> left,
                                 std::string op)
  : m_token(std::move(t)), m_left(std::move(left)), m_op(std::move(op)) {}
std::string InfixExpression::TokenLiteral() { return m_token.Literal; }

std::string InfixExpression::String() {
  std::string out;
  out.append("(");
  out.append(m_left->String());
  out.append(" " + m_op + " ");
  out.append(m_right->String());
  out.append(")");
  return out;
}

// Boolean Stuff
Boolean::Boolean(Token::Token token, bool value)
  : m_token(std::move(token)), m_value(value) {};
std::string Boolean::String() { return m_token.Literal; }
std::string Boolean::TokenLiteral() { return m_token.Literal; }

// If Expression Stuff
IfExpression::IfExpression(Token::Token token) : m_token(std::move(token)) {};

std::string IfExpression::String() {
  std::string out;
  out.append("if");
  out.append(this->m_condition->String());
  out.append(" ");
  out.append(this->m_consequence->String());
  if (this->m_alternative != nullptr) {
    out.append("else");
    out.append(this->m_alternative->String());
  }
  return out;
}

std::string IfExpression::TokenLiteral() { return m_token.Literal; }

// LetStatement stuff
LetStatement::LetStatement(Token::Token token) : m_token(std::move(token)) {};

std::string LetStatement::TokenLiteral() { return m_token.Literal; }

std::string LetStatement::String() {
  std::string out;
  out.append(TokenLiteral());
  out.append(" ");
  out.append(m_name->String());
  out.append(" = ");
  if (m_expression != nullptr) {
    out.append(m_expression->String());
  }
  out.append(";");
  return out;
}

// Return Statement Stuff
ReturnStatement::ReturnStatement(Token::Token token)
  : m_token(std::move(token)) {};
std::string ReturnStatement::TokenLiteral() { return m_token.Literal; }

std::string ReturnStatement::String() {
  std::string out;
  out.append(TokenLiteral());
  out.append(" ");
  if (m_returnValue != nullptr) {
    out.append(m_returnValue->String());
  }
  out.append(";");
  return out;
}

// Expression Statement Stuff
ExpressionStatement::ExpressionStatement(Token::Token token)
  : m_token(std::move(token)) {};
std::string ExpressionStatement::TokenLiteral() { return m_token.Literal; }

std::string ExpressionStatement::String() {
  if (m_expression != nullptr) {
    return m_expression->String();
  }
  return "";
}

// Block Statement Stuff
BlockStatement::BlockStatement(Token::Token token)
  : m_token(std::move(token)) {};

std::string BlockStatement::TokenLiteral() { return m_token.Literal; }

std::string BlockStatement::String() {
  std::string out;
  for (auto &statement : this->m_statements) {
    out.append(statement->String());
  }
  return out;
}

// Function Literal Stuff
FunctionLiteral::FunctionLiteral(Token::Token token)
  : m_token(std::move(token)) {};

std::string FunctionLiteral::String() {
  std::string out;
  std::vector<std::string> params;
  params.reserve(this->m_parameters.size());
  for (auto &str : this->m_parameters) {
    params.push_back(str->String());
  }
  out.append(this->TokenLiteral());
  out.append("(");
  out.append(Helpers::combineVecStrWithDelim(params, ", "));
  out.append(") ");
  out.append(this->m_body->String());
  return out;
}

std::string FunctionLiteral::TokenLiteral() { return m_token.Literal; }

// Call Expression Stuff
CallExpression::CallExpression(Token::Token token,
                               std::unique_ptr<IExpression> function)
  : m_token(std::move(token)), m_function(std::move(function)) {}

std::string CallExpression::String() {
  std::string out;
  std::vector<std::string> arguments;
  arguments.reserve(this->m_arguments.size());
  for (auto &str : this->m_arguments) {
    arguments.push_back(str->String());
  }
  out.append(this->m_function->String());
  out.append("(");
  out.append(Helpers::combineVecStrWithDelim(arguments, ", "));
  out.append(")");
  return out;
}

std::string CallExpression::TokenLiteral() { return m_token.Literal; }

// String Expression stuff

StringLiteral::StringLiteral(Token::Token token, std::string value)
  : m_token(std::move(token)), m_value(std::move(value)) {}
std::string StringLiteral::TokenLiteral() { return this->m_token.Literal; }
std::string StringLiteral::String() { return this->m_token.Literal; }

} // namespace Ast
