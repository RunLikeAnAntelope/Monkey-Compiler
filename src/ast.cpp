#include "ast.hpp"

namespace Ast {

//Program::Program(std::vector<IStatement*> statements):m_statements(statements){}

std::string Program::TokenLiteral(){
    if(m_statements.size() > 0){
        return m_statements[0]->TokenLiteral();
    } else {
        return "";
    }
}

// Identifier stuff

Identifier::Identifier(Token::Token token, std::string value):m_token(token), m_value(value){};
std::string Identifier::TokenLiteral() {
    return m_token.Literal;
}

// LetStatement stuff
LetStatement::LetStatement(Token::Token token):m_token(token){};

std::string LetStatement::TokenLiteral(){
    return m_token.Literal;
}




} // namespace Ast
