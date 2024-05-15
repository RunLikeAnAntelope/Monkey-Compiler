#include "ast.hpp"

namespace Ast {

Program::Program(std::vector<IStatement*> statements):m_statements(statements){}

std::string Program::TokenLiteral(){
    if(m_statements.size() > 0){
        return m_statements[0]->TokenLiteral();
    } else {
        return "";
    }
}

std::string LetStatement::TokenLiteral(){
    return m_token.Literal;
}


} // namespace Ast
