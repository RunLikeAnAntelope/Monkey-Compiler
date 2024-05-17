#include <memory>
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace Parser {


struct Parser {
    Lexer::Lexer &m_l;
    Token::Token m_curToken;
    Token::Token m_peekToken;

    Parser(Lexer::Lexer &lexer);

    void nextToken();

    Ast::Program ParseProgram();

    std::unique_ptr<Ast::IStatement> parseStatement();
    std::unique_ptr<Ast::LetStatement> parseLetStatement();

    bool curTokenIs(Token::TokenType t);
    bool peekTokenIs(Token::TokenType t);
    bool expectPeek(Token::TokenType t);
    
};



} // namespace Parser
