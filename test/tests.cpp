// tests.cpp
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include <gtest/gtest.h>
#include <parser.hpp>

TEST(TestNextToken, Works) {
  struct Test {
    Token::TokenType expectedToken;
    std::string expectedLiteral;
  };

  std::vector<Test> tests = {
      Test{Token::LET, "let"},     Test{Token::IDENT, "five"},
      Test{Token::ASSIGN, "="},    Test{Token::INT, "5"},
      Test{Token::SEMICOLON, ";"}, Test{Token::LET, "let"},
      Test{Token::IDENT, "ten"},   Test{Token::ASSIGN, "="},
      Test{Token::INT, "10"},      Test{Token::SEMICOLON, ";"},
      Test{Token::LET, "let"},     Test{Token::IDENT, "add"},
      Test{Token::ASSIGN, "="},    Test{Token::FUNCTION, "fn"},
      Test{Token::LPAREN, "("},    Test{Token::IDENT, "x"},
      Test{Token::COMMA, ","},     Test{Token::IDENT, "y"},
      Test{Token::RPAREN, ")"},    Test{Token::LBRACE, "{"},
      Test{Token::IDENT, "x"},     Test{Token::PLUS, "+"},
      Test{Token::IDENT, "y"},     Test{Token::SEMICOLON, ";"},
      Test{Token::RBRACE, "}"},    Test{Token::SEMICOLON, ";"},
      Test{Token::LET, "let"},     Test{Token::IDENT, "result"},
      Test{Token::ASSIGN, "="},    Test{Token::IDENT, "add"},
      Test{Token::LPAREN, "("},    Test{Token::IDENT, "five"},
      Test{Token::COMMA, ","},     Test{Token::IDENT, "ten"},
      Test{Token::RPAREN, ")"},    Test{Token::SEMICOLON, ";"},
      Test{Token::BANG, "!"},      Test{Token::MINUS, "-"},
      Test{Token::SLASH, "/"},     Test{Token::ASTERISK, "*"},
      Test{Token::INT, "5"},       Test{Token::SEMICOLON, ";"},
      Test{Token::INT, "5"},       Test{Token::LT, "<"},
      Test{Token::INT, "10"},      Test{Token::GT, ">"},
      Test{Token::INT, "5"},       Test{Token::SEMICOLON, ";"},
      Test{Token::IF, "if"},       Test{Token::LPAREN, "("},
      Test{Token::INT, "5"},       Test{Token::LT, "<"},
      Test{Token::INT, "10"},      Test{Token::RPAREN, ")"},
      Test{Token::LBRACE, "{"},    Test{Token::RETURN, "return"},
      Test{Token::TRUE, "true"},   Test{Token::SEMICOLON, ";"},
      Test{Token::RBRACE, "}"},    Test{Token::ELSE, "else"},
      Test{Token::LBRACE, "{"},    Test{Token::RETURN, "return"},
      Test{Token::FALSE, "false"}, Test{Token::SEMICOLON, ";"},
      Test{Token::RBRACE, "}"},    Test{Token::INT, "10"},
      Test{Token::EQ, "=="},       Test{Token::INT, "10"},
      Test{Token::SEMICOLON, ";"}, Test{Token::INT, "10"},
      Test{Token::NOT_EQ, "!="},   Test{Token::INT, "9"},
      Test{Token::SEMICOLON, ";"},

      Test{Token::EOF_, ""}};

  std::string input = "let five = 5;\n"
                      "let ten = 10;\n"
                      "let add = fn(x,y) {\n"
                      "    x+y;\n"
                      "};\n"
                      "let result = add(five, ten);\n"
                      "!-/*5\n;"
                      "5 < 10 > 5;"
                      "if (5 < 10) {"
                      "    return true;"
                      "} else {"
                      "   return false;"
                      "}"
                      "10 == 10;"
                      "10 != 9;";

  Lexer::Lexer l(input);

  Token::Token tok{};
  for (unsigned int i = 1; auto test : tests) {
    tok = l.NextToken();
    ASSERT_EQ(tok.Type, test.expectedToken)
        << "tests[" << i << "] - "
        << "tokentype wrong: expected=" << test.expectedToken
        << ", got=" << tok.Type;

    ASSERT_EQ(tok.Literal, test.expectedLiteral)
        << "tests[" << i << "] - "
        << "literal wrong: expected=" << test.expectedToken
        << ", got=" << tok.Type;
    i++;
  }
}

void testLetStatement(Ast::IStatement *s, std::string name) {
  ASSERT_EQ(s->TokenLiteral(), "let");
  Ast::LetStatement *letStmt = dynamic_cast<Ast::LetStatement *>(
      s); // should get an exception if this fails

  ASSERT_EQ(letStmt->m_name->m_value, name);
  ASSERT_EQ(letStmt->m_name->TokenLiteral(), name);
}
TEST(TestParser, Works) {
  std::string input = "let x = 5;\n"
                      "let y = 10;\n"
                      "let foobar = 838383;\n";

  struct test {
    std::string expectedIdentier;
  };

  std::vector<test> tests = {{"x"}, {"y"}, {"foobar"}};

  Lexer::Lexer l(input);
  Parser::Parser p(l);
  Ast::Program program = p.ParseProgram();
  // Doesn't work as program no longer is a pointer
  // ASSERT_FALSE(program == nullptr) << "ParseProgram() returned a nullptr";

  for (unsigned int i = 0; auto test : tests) {
    Ast::IStatement *stmt = program.m_statements[i];
    testLetStatement(stmt, test.expectedIdentier);
    i++;
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
