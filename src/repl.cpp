#include "repl.hpp"
#include "ast.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"
#include <iostream>
#include <memory>

namespace Repl {
void Start() {

  std::string scanned;
  Evaluator::Evaluator evaluator;
  std::shared_ptr<Object::Environment> env =
    std::make_shared<Object::Environment>();
  while (true) {
    std::cout << PROMPT;
    std::getline(std::cin, scanned);

    Lexer::Lexer l(scanned);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    if (std::ssize(p.Errors()) != 0) {
      printParserErrors(p.Errors());
      continue;
    }
    auto evaluated = evaluator.Eval(&program, env);
    if (evaluated != nullptr && evaluated.get() != nullptr) {
      std::cout << evaluated.get()->Inspect() << '\n';
    }
  }
}

void printParserErrors(const std::vector<std::string> &errors) // namespace Repl
{
  std::cout << "Woops! We ran into some monkey business here!\n"
            << " parser errors:\n";
  for (const auto &error : errors) {
    std::cout << "\t" << error << '\n';
  }
}
} // namespace Repl
