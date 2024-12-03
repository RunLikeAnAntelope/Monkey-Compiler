#include "ast.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(TestString, Works) {
    Token::Token tok = {.Type = Token::LET, .Literal = "let"};

    Token::Token firstTok = {.Type = Token::IDENT, .Literal = "myVar"};
    auto firstIdent = std::make_unique<Ast::Identifier>(firstTok, "myVar");

    Token::Token secondTok = {.Type = Token::IDENT, .Literal = "anotherVar"};
    auto secondIdent =
        std::make_unique<Ast::Identifier>(secondTok, "anotherVar");

    auto lStmt = std::make_unique<Ast::LetStatement>(tok);
    lStmt->m_name = std::move(firstIdent);
    lStmt->m_expression = std::move(secondIdent);

    std::vector<std::unique_ptr<Ast::IStatement>> stmts;
    stmts.push_back(std::move(lStmt));

    Ast::Program p;
    p.m_statements = std::move(stmts);

    ASSERT_EQ(p.String(), "let myVar = anotherVar;");
}
