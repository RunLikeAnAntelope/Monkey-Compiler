#include "ast.hpp"
#include <memory>
#include <gtest/gtest.h>

TEST(TestString, Works){
  Token::Token tok = {Token::LET, "let"};

  Token::Token firstTok = {Token::IDENT, "myVar"};
  auto firstIdent = std::make_unique<Ast::Identifier>(firstTok, "myVar");

  Token::Token secondTok = {Token::IDENT, "anotherVar"};
  auto secondIdent = std::make_unique<Ast::Identifier>(secondTok, "anotherVar");
  
  auto lStmt = std::make_unique<Ast::LetStatement>(tok);
  lStmt->m_name = std::move(firstIdent);
  lStmt->m_expression = std::move(secondIdent);


  std::vector<std::unique_ptr<Ast::IStatement>> stmts;
  stmts.push_back(std::move(lStmt));

  Ast::Program p;
  p.m_statements = std::move(stmts);

  ASSERT_EQ(p.String(), "let myVar = anotherVar;");
}
