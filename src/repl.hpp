#pragma once
#include <string>
#include <vector>
namespace Repl {

const std::string PROMPT = ">>";
void printParserErrors(const std::vector<std::string> &errors);
void Start();

} // namespace Repl
