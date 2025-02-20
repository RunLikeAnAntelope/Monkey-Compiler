#pragma once
#include <string>
#include <unordered_map>
#include <vector>
namespace Token {

// typedef std::string TokenType;

enum TokenType {
  ILLEGAL,
  EOF_,
  IDENT,
  INT,
  ASSIGN,
  PLUS,
  MINUS,
  BANG,
  ASTERISK,
  SLASH,
  LT,
  GT,
  EQ,
  NOT_EQ,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  FUNCTION,
  LET,
  TRUE,
  FALSE,
  IF,
  ELSE,
  RETURN,
  STRING,
  LBRACKET,
  RBRACKET,
  COLON,
};

struct Token {
  TokenType Type;
  std::string Literal;
};

struct StringTok {
  std::string Str;
  TokenType Type;
};

const std::vector<StringTok> strToTok = {{.Str = "ILLEGAL", .Type = ILLEGAL},
                                         {.Str = "EOF", .Type = EOF_},

                                         // identifiers + literals
                                         {.Str = "IDENT", .Type = IDENT},
                                         {.Str = "INT", .Type = INT},

                                         // Operators
                                         {.Str = "=", .Type = ASSIGN},
                                         {.Str = "+", .Type = PLUS},
                                         {.Str = "-", .Type = MINUS},
                                         {.Str = "!", .Type = BANG},
                                         {.Str = "*", .Type = ASTERISK},
                                         {.Str = "/", .Type = SLASH},
                                         {.Str = "<", .Type = LT},
                                         {.Str = ">", .Type = GT},
                                         {.Str = "==", .Type = EQ},
                                         {.Str = "!=", .Type = NOT_EQ},

                                         // delimiters
                                         {.Str = ",", .Type = COMMA},
                                         {.Str = ";", .Type = SEMICOLON},
                                         {.Str = "(", .Type = LPAREN},
                                         {.Str = ")", .Type = RPAREN},
                                         {.Str = "{", .Type = LBRACE},
                                         {.Str = "}", .Type = RBRACE},
                                         {.Str = "[", .Type = LBRACKET},
                                         {.Str = "]", .Type = RBRACE},
                                         {.Str = ":", .Type = COLON},
                                         // Keywords
                                         {.Str = "FUNCTION", .Type = FUNCTION},
                                         {.Str = "LET", .Type = LET},
                                         {.Str = "TRUE", .Type = TRUE},
                                         {.Str = "FALSE", .Type = FALSE},
                                         {.Str = "IF", .Type = IF},
                                         {.Str = "ELSE", .Type = ELSE},
                                         {.Str = "RETURN", .Type = RETURN},
                                         {.Str = "STRING", .Type = STRING}};

std::unordered_map<std::string, TokenType> vecToTokenMap();

std::unordered_map<TokenType, std::string> vecToTokenStringMap();

const std::unordered_map<std::string, TokenType> tokenMap = vecToTokenMap();

const std::unordered_map<TokenType, std::string> tokenStringMap =
  vecToTokenStringMap();

const std::unordered_map<std::string, TokenType> keywords = {
  {"fn", FUNCTION}, {"let", LET},   {"true", TRUE},    {"false", FALSE},
  {"if", IF},       {"else", ELSE}, {"return", RETURN}};

TokenType LookupIdent(const std::string &ident);

} // namespace Token
