#include "lexer.hpp"
#include "token.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <utility>

namespace Lexer {

Lexer::Lexer(std::string input) : m_input(std::move(input)) { readChar(); }

Token::Token Lexer::NextToken() {
  Token::Token tok{};
  skipWhitespace();
  switch (m_ch) {
  case 0:
    tok.Type = Token::EOF_;
    tok.Literal = "";
    break;
  case '=':
    if (peekChar() == '=') {
      char ch = m_ch;
      readChar();
      std::string literal = std::string() + ch + m_ch;
      tok.Type = Token::EQ;
      tok.Literal = literal;
    } else {
      tok.Type = Token::ASSIGN;
      ;
      tok.Literal = m_ch;
    }
    break;
  case '+':
    tok.Type = Token::PLUS;
    tok.Literal = m_ch;
    break;
  case '-':
    tok.Type = Token::MINUS;
    tok.Literal = m_ch;
    break;
  case '!':
    if (peekChar() == '=') {
      char ch = m_ch;
      readChar();
      std::string literal = std::string() + ch + m_ch;
      tok.Type = Token::NOT_EQ;
      tok.Literal = literal;
    } else {
      tok.Type = Token::BANG;
      tok.Literal = m_ch;
    }
    break;
  case '*':
    tok.Type = Token::ASTERISK;
    tok.Literal = m_ch;
    break;
  case '/':
    tok.Type = Token::SLASH;
    tok.Literal = m_ch;
    break;
  case '<':
    tok.Type = Token::LT;
    tok.Literal = m_ch;
    break;
  case '>':
    tok.Type = Token::GT;
    tok.Literal = m_ch;
    break;
  case ',':
    tok.Type = Token::COMMA;
    tok.Literal = m_ch;
    break;
  case ';':
    tok.Type = Token::SEMICOLON;
    tok.Literal = m_ch;
    break;
  case '(':
    tok.Type = Token::LPAREN;
    tok.Literal = m_ch;
    break;
  case ')':
    tok.Type = Token::RPAREN;
    tok.Literal = m_ch;
    break;
  case '{':
    tok.Type = Token::LBRACE;
    tok.Literal = m_ch;
    break;
  case '}':
    tok.Type = Token::RBRACE;
    tok.Literal = m_ch;
    break;
  case '"':
    tok.Type = Token::STRING;
    tok.Literal = readString();
    break;
  case '[':
    tok.Type = Token::LBRACKET;
    tok.Literal = m_ch;
    break;
  case ']':
    tok.Type = Token::RBRACKET;
    tok.Literal = m_ch;
    break;
  case ':':
    tok.Type = Token::COLON;
    tok.Literal = m_ch;
    break;
  default:
    if (isLetter(m_ch)) {
      tok.Literal = readIdentifier();
      tok.Type = Token::LookupIdent(tok.Literal);
      return tok;
    } else if (std::isdigit(m_ch)) {
      tok.Type = Token::INT;
      tok.Literal = readNumber();
      return tok;
    } else {
      tok.Type = Token::ILLEGAL;
      tok.Literal = m_ch;
    }
    break;
  }
  readChar();
  return tok;
}

std::string Lexer::readIdentifier() {
  size_t position = m_position;
  while (isLetter(m_ch)) {
    readChar();
  }
  return m_input.substr(position, m_position - position);
}

std::string Lexer::readNumber() {
  size_t position = m_position;
  while (std::isdigit(m_ch)) {
    readChar();
  }
  return m_input.substr(position, m_position - position);
}

std::string Lexer::readString() {
  std::string out;
  readChar(); // get rid of first "
  while (m_ch != '"' && m_ch != 0) {
    if (m_ch == '\\') {
      char peek = peekChar();
      switch (peek) {
      case '"':
        out.append("\"");
        readChar();
        break;
      case 'n':
        out.append("\n");
        readChar();
        break;
      case 't':
        out.append("\t");
        readChar();
        break;
      default:
        out.push_back(m_ch);
      }
    } else {
      out.push_back(m_ch);
    }
    readChar();
  }
  return out;
}

void Lexer::readChar() {
  if (m_readPosition >= m_input.size()) {
    m_ch = 0;
  } else {
    m_ch = m_input[m_readPosition];
  }
  m_position = m_readPosition;
  m_readPosition += 1;
}

bool Lexer::isLetter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}
void Lexer::skipWhitespace() {
  while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r') {
    readChar();
  }
}

char Lexer::peekChar() {
  if (m_readPosition >= m_input.size()) {
    return 0;
  } else {
    return m_input[m_readPosition];
  }
}

} // namespace Lexer
