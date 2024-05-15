#include "repl.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include <iostream>
namespace Repl {
    

void Start(){

    std::string scanned{};
    while(true){
        std::cout << PROMPT;
        std::getline(std::cin, scanned);
        
        Lexer::Lexer l(scanned);
        for(Token::Token tok = l.NextToken(); tok.Type != Token::EOF_; tok = l.NextToken()){
            std::cout << "{Type:" << tok.Type << " Literal:" << tok.Literal << "}\n";
        }
    }
}


} // namespace Repl
