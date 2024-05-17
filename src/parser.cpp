#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"
#include <memory>
namespace Parser {


Parser::Parser(Lexer::Lexer& lexer):m_l(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_l.NextToken();
}

Ast::Program Parser::ParseProgram(){
    Ast::Program program;
    while(m_curToken.Type != Token::EOF_){
        std::unique_ptr<Ast::IStatement> stmt = parseStatement();
        if(stmt != nullptr){
            program.m_statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Ast::IStatement> Parser::parseStatement(){
    switch (m_curToken.Type) {
        case Token::LET:
            return parseLetStatement();
            break;
        default:
            return nullptr;
    }
}

std::unique_ptr<Ast::LetStatement> Parser::parseLetStatement(){
    auto stmt = std::make_unique<Ast::LetStatement>(m_curToken);
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
