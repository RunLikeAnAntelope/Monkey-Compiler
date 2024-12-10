#include "token.hpp"
namespace Token {

TokenType LookupIdent(const std::string &ident) {
  if (auto search = keywords.find(ident); search != keywords.end()) {
    return search->second;
  }
  return TokenType::IDENT;
}

std::unordered_map<std::string, TokenType> vecToTokenMap() {
  std::unordered_map<std::string, TokenType> tokenMap;
  for (const auto &vecRepr : strToTok) {
    tokenMap[vecRepr.Str] = vecRepr.Type;
  }
  return tokenMap;
}

std::unordered_map<TokenType, std::string> vecToTokenStringMap() {
  std::unordered_map<TokenType, std::string> tokenStringMap;
  for (const auto &vecRepr : strToTok) {
    tokenStringMap[vecRepr.Type] = vecRepr.Str;
  }
  return tokenStringMap;
}

} // namespace Token
