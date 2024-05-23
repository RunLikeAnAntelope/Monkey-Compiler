#include "token.hpp"
namespace Token {


TokenType LookupIdent(std::string ident) {
    if(auto search = keywords.find(ident); search != keywords.end()){
        return search->second;
    }
    return IDENT;
}


std::unordered_map<std::string, TokenType> vecToTokenMap() {
    std::unordered_map<std::string, TokenType> tokenMap;
    for(auto vecRepr: strToTok ){
        tokenMap[vecRepr.Str] = vecRepr.Type;
    }
    return tokenMap;
}

std::unordered_map<TokenType, std::string> vecToTokenStringMap(){
    std::unordered_map<TokenType, std::string> tokenStringMap;
    for(auto vecRepr: strToTok) {
        tokenStringMap[vecRepr.Type] = vecRepr.Str;
    }
    return tokenStringMap;
}



}
