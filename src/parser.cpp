#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
namespace Parser {

Parser::Parser(Lexer::Lexer &lexer) : m_l(lexer) {
    nextToken();
    nextToken();
    registerPrefix(Token::IDENT, &Parser::parseIdentifier);
    registerPrefix(Token::INT, &Parser::parseIntegerLiteral);
    registerPrefix(Token::BANG, &Parser::parsePrefixExpression);
    registerPrefix(Token::MINUS, &Parser::parsePrefixExpression);
    registerPrefix(Token::TRUE, &Parser::parseBoolean);
    registerPrefix(Token::FALSE, &Parser::parseBoolean);
    registerPrefix(Token::LPAREN, &Parser::parseGroupedExpression);
    registerPrefix(Token::IF, &Parser::parseIfExpression);

    registerInfix(Token::PLUS, &Parser::parseInfixExpression);
    registerInfix(Token::MINUS, &Parser::parseInfixExpression);
    registerInfix(Token::SLASH, &Parser::parseInfixExpression);
    registerInfix(Token::ASTERISK, &Parser::parseInfixExpression);
    registerInfix(Token::EQ, &Parser::parseInfixExpression);
    registerInfix(Token::NOT_EQ, &Parser::parseInfixExpression);
    registerInfix(Token::LT, &Parser::parseInfixExpression);
    registerInfix(Token::GT, &Parser::parseInfixExpression);
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_l.NextToken();
}

Ast::Program Parser::ParseProgram() {
    Ast::Program program;
    while (m_curToken.Type != Token::EOF_) {
        std::unique_ptr<Ast::IStatement> stmt = parseStatement();
        if (stmt != nullptr) {
            program.m_statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Ast::IStatement> Parser::parseStatement() {
    switch (m_curToken.Type) {
    case Token::LET:
        return parseLetStatement();
        break;
    case Token::RETURN:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<Ast::LetStatement> Parser::parseLetStatement() {
    auto stmt = std::make_unique<Ast::LetStatement>(m_curToken);
    if (!expectPeek(Token::IDENT)) {
        return nullptr;
    }

    stmt->m_name =
        std::make_unique<Ast::Identifier>(m_curToken, m_curToken.Literal);
    if (!expectPeek(Token::ASSIGN)) {
        return nullptr;
    }

    nextToken();

    stmt->m_expression = parseExpression(LOWEST);

    while (!curTokenIs(Token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

std::unique_ptr<Ast::ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<Ast::ReturnStatement>(m_curToken);
    nextToken();
    while (!curTokenIs(Token::SEMICOLON)) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<Ast::ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<Ast::ExpressionStatement>(m_curToken);
    stmt->m_expression = parseExpression(LOWEST);
    if (peekTokenIs(Token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

std::unique_ptr<Ast::IExpression>
Parser::parseExpression(Precedence precedence) {
    prefixParseFn prefix;
    try {
        prefix = m_prefixParseFns.at(m_curToken.Type);
    } catch (std::out_of_range &err) {
        noPrefixParseFnError(m_curToken.Type);
        return nullptr;
    }

    auto leftExp = (this->*prefix)();

    while (!peekTokenIs(Token::SEMICOLON) && precedence < peekPrecedence()) {
        infixParseFn infix;
        try {
            infix = m_infixParseFns.at(m_peekToken.Type);
        } catch (std::out_of_range &err) {
            return leftExp;
        }
        nextToken();
        leftExp = (this->*infix)(std::move(leftExp));
    }

    return leftExp;
}

std::unique_ptr<Ast::IExpression> Parser::parseIdentifier() {
    auto ident =
        std::make_unique<Ast::Identifier>(m_curToken, m_curToken.Literal);
    return ident;
}

std::unique_ptr<Ast::IExpression> Parser::parseIntegerLiteral() {
    auto lit = std::make_unique<Ast::IntegerLiteral>(m_curToken);
    try {
        long int value = stoi(m_curToken.Literal);
        lit->m_value = value;
    } catch (const std::invalid_argument &ia) {
        std::string msg =
            "could not parse " + m_curToken.Literal + "as integer";
        m_errors.push_back(msg);
        return nullptr;
    }

    return lit;
}

std::unique_ptr<Ast::IExpression> Parser::parsePrefixExpression() {
    auto expression =
        std::make_unique<Ast::PrefixExpression>(m_curToken, m_curToken.Literal);
    nextToken();
    expression->m_right = parseExpression(PREFIX);
    return expression;
}

std::unique_ptr<Ast::IExpression>
Parser::parseInfixExpression(std::unique_ptr<Ast::IExpression> left) {
    auto expression = std::make_unique<Ast::InfixExpression>(
        m_curToken, std::move(left), m_curToken.Literal);
    Precedence precedence = curPrecedence();
    nextToken();
    expression->m_right = parseExpression(precedence);
    return expression;
}

std::unique_ptr<Ast::IExpression> Parser::parseBoolean() {
    auto b = std::make_unique<Ast::Boolean>(m_curToken);
    b->m_value = m_curToken.Type == Token::FALSE ? false : true;
    return b;
}

std::unique_ptr<Ast::IExpression> Parser::parseGroupedExpression() {
    nextToken();
    auto exp = parseExpression(LOWEST);
    if (!expectPeek(Token::RPAREN)) {
        return nullptr;
    }
    return exp;
}

std::unique_ptr<Ast::BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<Ast::BlockStatement>(m_curToken);
    nextToken();
    while (!curTokenIs(Token::RBRACE) && !curTokenIs(Token::EOF_)) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            block->m_statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return block;
}

std::unique_ptr<Ast::IExpression> Parser::parseIfExpression() {
    auto expression = std::make_unique<Ast::IfExpression>(m_curToken);
    if (!expectPeek(Token::LPAREN)) {
        return nullptr;
    }

    nextToken();
    expression->m_condition = parseExpression(LOWEST);

    if (!expectPeek(Token::RPAREN)) {
        return nullptr;
    }
    if (!expectPeek(Token::LBRACE)) {
        return nullptr;
    }
    expression->m_consequence = parseBlockStatement();

    if (peekTokenIs(Token::ELSE)) {
        nextToken();
        if (!expectPeek(Token::LBRACE)) {
            return nullptr;
        }
        expression->m_alternative = parseBlockStatement();
    }

    return expression;
}

void Parser::registerPrefix(Token::TokenType tokenType, prefixParseFn fn) {
    m_prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(Token::TokenType tokenType, infixParseFn fn) {
    m_infixParseFns[tokenType] = fn;
}

bool Parser::curTokenIs(Token::TokenType t) { return m_curToken.Type == t; }

bool Parser::peekTokenIs(Token::TokenType t) { return m_peekToken.Type == t; }

bool Parser::expectPeek(Token::TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

Precedence Parser::peekPrecedence() {
    Precedence p;
    try {
        p = precedences.at(m_peekToken.Type);
    }

    catch (std::out_of_range &err) {
        p = LOWEST;
    }
    return p;
}

Precedence Parser::curPrecedence() {
    Precedence p;
    try {
        p = precedences.at(m_curToken.Type);
    }

    catch (std::out_of_range &err) {
        p = LOWEST;
    }
    return p;
}

std::vector<std::string> Parser::Errors() { return m_errors; }

void Parser::peekError(Token::TokenType t) {
    std::string msg = "Expect next token to be " + Token::tokenStringMap.at(t) +
                      ", got " + Token::tokenStringMap.at(m_peekToken.Type) +
                      " instead";
    m_errors.push_back(msg);
}

void Parser::noPrefixParseFnError(Token::TokenType t) {
    std::string msg = "no prefix parse function for " +
                      Token::tokenStringMap.at(t) + " found";
    m_errors.push_back(msg);
}

} // namespace Parser
