#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
namespace Parser {

enum Precedence {
    EMPTY_PRECEDENCE,
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

const std::unordered_map<Token::TokenType, Precedence> precedences = {
    {Token::EQ, EQUALS},      {Token::NOT_EQ, EQUALS},
    {Token::LT, LESSGREATER}, {Token::GT, LESSGREATER},
    {Token::PLUS, SUM},       {Token::MINUS, SUM},
    {Token::SLASH, PRODUCT},  {Token::ASTERISK, PRODUCT}};

struct Parser {
    using prefixParseFn = std::unique_ptr<Ast::IExpression> (Parser::*)();
    using infixParseFn = std::unique_ptr<Ast::IExpression> (Parser::*)(
        std::unique_ptr<Ast::IExpression> expr);

    Lexer::Lexer &m_l;
    Token::Token m_curToken;
    Token::Token m_peekToken;
    std::unordered_map<Token::TokenType, prefixParseFn> m_prefixParseFns;
    std::unordered_map<Token::TokenType, infixParseFn> m_infixParseFns;
    std::vector<std::string> m_errors;

    Parser(Lexer::Lexer &lexer);

    void nextToken();
    Ast::Program ParseProgram();

    std::unique_ptr<Ast::IStatement> parseStatement();
    std::unique_ptr<Ast::LetStatement> parseLetStatement();
    std::unique_ptr<Ast::ReturnStatement> parseReturnStatement();
    std::unique_ptr<Ast::ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<Ast::IExpression> parseExpression(Precedence precedence);
    std::unique_ptr<Ast::IExpression> parseIdentifier();
    std::unique_ptr<Ast::IExpression> parseIntegerLiteral();
    std::unique_ptr<Ast::IExpression> parsePrefixExpression();
    std::unique_ptr<Ast::IExpression>
    parseInfixExpression(std::unique_ptr<Ast::IExpression> left);
    std::unique_ptr<Ast::IExpression> parseBoolean();
    std::unique_ptr<Ast::IExpression> parseGroupedExpression();

    void registerPrefix(Token::TokenType tokenType, prefixParseFn fn);
    void registerInfix(Token::TokenType, infixParseFn fn);

    bool curTokenIs(Token::TokenType t);
    bool peekTokenIs(Token::TokenType t);
    bool expectPeek(Token::TokenType t);

    Precedence peekPrecedence();
    Precedence curPrecedence();

    std::vector<std::string> Errors();
    void peekError(Token::TokenType t);
    void noPrefixParseFnError(Token::TokenType t);
};

} // namespace Parser
