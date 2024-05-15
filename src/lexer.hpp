#pragma once
#include <token.hpp>
#include <string>
namespace Lexer {


class Lexer {
    private:
        std::string m_input;
        size_t m_position = 0;
        size_t m_readPosition = 0;
        char m_ch = 0;

        std::string readIdentifier();
        std::string readNumber();
        void readChar();
        bool isLetter(char ch);
        void skipWhitespace();
        char peekChar();
    public:
        Lexer(std::string input);  
        ~Lexer();
        Token::Token NextToken();
};


} // namespace Lexer
