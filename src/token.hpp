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
  RBRACKET
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

                                         // Keywords
                                         {.Str = "FUNCTION", .Type = FUNCTION},
                                         {.Str = "LET", .Type = LET},
                                         {.Str = "TRUE", .Type = TRUE},
                                         {.Str = "FALSE", .Type = FALSE},
                                         {.Str = "IF", .Type = IF},
                                         {.Str = "ELSE", .Type = ELSE},
                                         {.Str = "RETURN", .Type = RETURN},
                                         {.Str = "STRING", .Type = STRING},
                                         {.Str = "[", .Type = LBRACKET},
                                         {.Str = "]", .Type = RBRACE}};

std::unordered_map<std::string, TokenType> vecToTokenMap();
std::unordered_map<TokenType, std::string> vecToTokenStringMap();

const std::unordered_map<std::string, TokenType> tokenMap = vecToTokenMap();
const std::unordered_map<TokenType, std::string> tokenStringMap =
  vecToTokenStringMap();
//     {"ILLEGAL", ILLEGAL},
//     {"EOF", EOF_},
//
//     // identifiers + literals
//     {"IDENT", IDENT},
//     {"INT", INT},
//
//     // Operators
//     {"=", ASSIGN},
//     {"+", PLUS},
//     {"-", MINUS},
//     {"!", BANG},
//     {"*", ASTERISK},
//     {"/", SLASH},
//     {"<", LT},
//     {">", GT},
//     {"=", EQ},
//     {"!=", NOT_EQ},
//
//     // delimiters
//     {",", COMMA},
//     {";", SEMICOLON},
//     {"(", LPAREN},
//     {")", RPAREN},
//     {"{", LBRACE},
//     {"}", RBRACE},
//
//     // Keywords
//     {"FUNCTION", FUNCTION},
//     {"LET", LET},
//     {"TRUE", TRUE},
//     {"FALSE", FALSE},
//     {"IF", IF},
//     {"ELSE", ELSE},
//     {"RETURN", RETURN}
// };

/*
const TokenType ILLEGAL = "ILLEGAL";
const TokenType EOF_ = "EOF";

//identifiers + literals
const TokenType IDENT = "IDENT";
const TokenType INT = "INT";

// Operators
const TokenType ASSIGN = "=";
const TokenType PLUS = "+";
const TokenType MINUS = "-";
const TokenType BANG = "!";
const TokenType ASTERISK = "*";
const TokenType SLASH = "/";
const TokenType LT = "<";
const TokenType GT = ">";
const TokenType EQ = "=";
const TokenType NOT_EQ = "!=";

// Delimiters
const TokenType COMMA = ",";
const TokenType SEMICOLON = ";";
const TokenType LPAREN = "(";
const TokenType RPAREN = ")";
const TokenType LBRACE = "{";
const TokenType RBRACE = "}";

// Keywords
const TokenType FUNCTION = "FUNCTION";
const TokenType LET = "LET";
const TokenType TRUE = "TRUE";
const TokenType FALSE = "FALSE";
const TokenType IF = "IF";
const TokenType ELSE = "ELSE";
const TokenType RETURN = "RETURN";
*/
const std::unordered_map<std::string, TokenType> keywords = {
  {"fn", FUNCTION}, {"let", LET},   {"true", TRUE},    {"false", FALSE},
  {"if", IF},       {"else", ELSE}, {"return", RETURN}};

TokenType LookupIdent(const std::string &ident);

} // namespace Token
