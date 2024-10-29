#pragma once
#include <string>
#include <vector>
namespace Repl {

const std::string PROMPT = ">>";
void printParserErrors(std::vector<std::string> errors);
void Start();

} // namespace Repl
