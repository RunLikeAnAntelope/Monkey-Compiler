#pragma once
#include <string>
#include <unordered_map>
namespace Token {


typedef std::string TokenType;

//enum TokenType {ILLEGAL, EOF_, IDENT, INT, ASSIGN, PLUS, COMMA, SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE, FUNCTION, LET};



struct Token {
    TokenType Type;
    std::string Literal;
};

TokenType LookupIdent(std::string ident);

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

const std::unordered_map<std::string, TokenType> keywords = {
    {"fn", FUNCTION},
    {"let", LET},
    {"true", TRUE},
    {"false", FALSE},
    {"if", IF},
    {"else", ELSE},
    {"return", RETURN}
};
} // namespace Token
