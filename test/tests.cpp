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
    Test{.expectedToken = Token::LET, .expectedLiteral = "let"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "five"},
    Test{.expectedToken = Token::ASSIGN, .expectedLiteral = "="},
    Test{.expectedToken = Token::INT, .expectedLiteral = "5"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::LET, .expectedLiteral = "let"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "ten"},
    Test{.expectedToken = Token::ASSIGN, .expectedLiteral = "="},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::LET, .expectedLiteral = "let"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "add"},
    Test{.expectedToken = Token::ASSIGN, .expectedLiteral = "="},
    Test{.expectedToken = Token::FUNCTION, .expectedLiteral = "fn"},
    Test{.expectedToken = Token::LPAREN, .expectedLiteral = "("},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "x"},
    Test{.expectedToken = Token::COMMA, .expectedLiteral = ","},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "y"},
    Test{.expectedToken = Token::RPAREN, .expectedLiteral = ")"},
    Test{.expectedToken = Token::LBRACE, .expectedLiteral = "{"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "x"},
    Test{.expectedToken = Token::PLUS, .expectedLiteral = "+"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "y"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::RBRACE, .expectedLiteral = "}"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::LET, .expectedLiteral = "let"},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "result"},
    Test{.expectedToken = Token::ASSIGN, .expectedLiteral = "="},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "add"},
    Test{.expectedToken = Token::LPAREN, .expectedLiteral = "("},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "five"},
    Test{.expectedToken = Token::COMMA, .expectedLiteral = ","},
    Test{.expectedToken = Token::IDENT, .expectedLiteral = "ten"},
    Test{.expectedToken = Token::RPAREN, .expectedLiteral = ")"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::BANG, .expectedLiteral = "!"},
    Test{.expectedToken = Token::MINUS, .expectedLiteral = "-"},
    Test{.expectedToken = Token::SLASH, .expectedLiteral = "/"},
    Test{.expectedToken = Token::ASTERISK, .expectedLiteral = "*"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "5"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "5"},
    Test{.expectedToken = Token::LT, .expectedLiteral = "<"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::GT, .expectedLiteral = ">"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "5"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::IF, .expectedLiteral = "if"},
    Test{.expectedToken = Token::LPAREN, .expectedLiteral = "("},
    Test{.expectedToken = Token::INT, .expectedLiteral = "5"},
    Test{.expectedToken = Token::LT, .expectedLiteral = "<"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::RPAREN, .expectedLiteral = ")"},
    Test{.expectedToken = Token::LBRACE, .expectedLiteral = "{"},
    Test{.expectedToken = Token::RETURN, .expectedLiteral = "return"},
    Test{.expectedToken = Token::TRUE, .expectedLiteral = "true"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::RBRACE, .expectedLiteral = "}"},
    Test{.expectedToken = Token::ELSE, .expectedLiteral = "else"},
    Test{.expectedToken = Token::LBRACE, .expectedLiteral = "{"},
    Test{.expectedToken = Token::RETURN, .expectedLiteral = "return"},
    Test{.expectedToken = Token::FALSE, .expectedLiteral = "false"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::RBRACE, .expectedLiteral = "}"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::EQ, .expectedLiteral = "=="},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::INT, .expectedLiteral = "10"},
    Test{.expectedToken = Token::NOT_EQ, .expectedLiteral = "!="},
    Test{.expectedToken = Token::INT, .expectedLiteral = "9"},
    Test{.expectedToken = Token::SEMICOLON, .expectedLiteral = ";"},
    Test{.expectedToken = Token::STRING, .expectedLiteral = "foobar"},
    Test{.expectedToken = Token::STRING, .expectedLiteral = "foo bar"},
    Test{.expectedToken = Token::STRING, .expectedLiteral = "foo\"bar"},

    Test{.expectedToken = Token::EOF_, .expectedLiteral = ""}};

  std::string input = "let five = 5;\n"
                      "let ten = 10;\n"
                      "let add = fn(x,y) {\n"
                      "    x+y;\n"
                      "};\n"
                      "let result = add(five, ten);\n"
                      "!-/*5\n;"
                      "5 < 10 > 5;\n"
                      "if (5 < 10) {\n"
                      "    return true;\n"
                      "} else {\n"
                      "   return false;\n"
                      "}\n"
                      "10 == 10;\n"
                      "10 != 9;\n"
                      "\"foobar\"\n"
                      "\"foo bar\"\n"
                      "\"foo\\\"bar\"";

  Lexer::Lexer l(input);
  Token::Token tok{};
  for (unsigned int i = 1; const auto &test : tests) {
    tok = l.NextToken();
    ASSERT_EQ(tok.Literal, test.expectedLiteral)
      << "tests[" << i << "] - "
      << "literal wrong: expected=" << test.expectedLiteral
      << ", got=" << tok.Literal;
    ASSERT_EQ(tok.Type, test.expectedToken)
      << "tests[" << i << "] - "
      << "tokentype wrong: expected="
      << Token::tokenStringMap.at(test.expectedToken)
      << ", got=" << Token::tokenStringMap.at(tok.Type);

    i++;
  }
}
