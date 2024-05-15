#include "token.hpp"
namespace Token {


TokenType LookupIdent(std::string ident) {
    if(auto search = keywords.find(ident); search != keywords.end()){
        return search->second;
    }
    return IDENT;
}



}
