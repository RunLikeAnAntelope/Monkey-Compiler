#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"
namespace Parser {


Parser::Parser(Lexer::Lexer& lexer):m_l(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_l.NextToken();
}

Ast::Program* Parser::ParseProgram(){
    Ast::Program* program = new Ast::Program();
    program->m_statements = {};
    while(m_curToken.Type != Token::EOF_){
        Ast::IStatement* stmt = parseStatement();
        if(stmt != nullptr){
            program->m_statements.push_back(stmt);
        }
        nextToken();
    }

    return program;
}

Ast::IStatement* Parser::parseStatement(){
    switch (m_curToken.Type) {
        case Token::LET:
            return parseLetStatement();
            break;
        default:
            return nullptr;
    }
}

Ast::LetStatement* Parser::parseLetStatement(){
    Ast::LetStatement* stmt = new Ast::LetStatement(m_curToken);
    if (!expectPeek(Token::IDENT)){
        return nullptr;
    }

    stmt->m_name = new Ast::Identifier(m_curToken, m_curToken.Literal);
    if(!expectPeek(Token::ASSIGN)) {
        return nullptr;
    }

    while(!curTokenIs(Token::SEMICOLON)){
        nextToken();
    }
    
    return stmt;
}

bool Parser::curTokenIs(Token::TokenType t){
    return m_curToken.Type == t;
}

bool Parser::peekTokenIs(Token::TokenType t){
    return m_peekToken.Type == t;
}

bool Parser::expectPeek(Token::TokenType t){
    if(peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        return false;
    }
}




} //namespace Parser
