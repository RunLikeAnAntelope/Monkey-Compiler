#include "ast.hpp"

namespace Ast {

// Program stuff
std::string Program::TokenLiteral(){
    if(m_statements.size() > 0){
        return m_statements[0]->TokenLiteral();
    } else {
        return "";
    }
}

std::string Program::String() {
    std::string out;
    for(auto const& s : m_statements) {
        out.append(s->String());
    }
    return out;
}

// Identifier stuff
Identifier::Identifier(Token::Token token, std::string value):m_token(token), m_value(value){};
std::string Identifier::TokenLiteral() {
    return m_token.Literal;
}

std::string Identifier::String() {
    return m_value;
}

// Integer Literal stuff
IntegerLiteral::IntegerLiteral(Token::Token token):m_token(token){};
std::string IntegerLiteral::String(){
    return m_token.Literal;
}

std::string IntegerLiteral::TokenLiteral(){
    return m_token.Literal;
}

// PrefixExpression stuff
PrefixExpression::PrefixExpression(Token::Token t, std::string op):m_token(t), m_op(op){}

std::string PrefixExpression::TokenLiteral() {
    return m_token.Literal;
}

std::string PrefixExpression::String() {
    std::string out;
    out.append("(");
    out.append(m_op);
    out.append(m_right->String());
    out.append(")");
    return out;
}

//InfixExpression stuff
InfixExpression::InfixExpression(Token::Token t, std::unique_ptr<IExpression> left, std::string op):m_token(t), m_left(std::move(left)), m_op(op){}
std::string InfixExpression::TokenLiteral() {
    return m_token.Literal;
}

std::string InfixExpression::String(){
    std::string out;
    out.append("(");
    out.append(m_left->String());
    out.append(" " + m_op + " ");
    out.append(m_right->String());
    out.append(")");
    return out;
}
// LetStatement stuff
LetStatement::LetStatement(Token::Token token):m_token(token){};

std::string LetStatement::TokenLiteral(){
    return m_token.Literal;
}

std::string LetStatement::String(){
    std::string out;
    out.append(TokenLiteral());
    out.append(" ");
    out.append(m_name->String());
    out.append(" = ");
    if(m_expression != nullptr){
        out.append(m_expression->String());
    }
    out.append(";");
    return out;
}

// Return Statement Stuff
ReturnStatement::ReturnStatement(Token::Token token):m_token(token){};
std::string ReturnStatement::TokenLiteral(){
    return m_token.Literal;
}

std::string ReturnStatement::String(){
    std::string out;
    out.append(TokenLiteral());
    out.append(" ");
    if (m_returnValue != nullptr){
        out.append(m_returnValue->String());
    }
    out.append(";");
    return out;
}

// Expression Statement Stuff
ExpressionStatement::ExpressionStatement(Token::Token token):m_token(token){};
std::string ExpressionStatement::TokenLiteral(){
    return m_token.Literal;
}

std::string ExpressionStatement::String(){
    if(m_expression != nullptr){
        return m_expression->String();
    }
    return "";
}


} // namespace Ast
