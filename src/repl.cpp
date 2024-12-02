#include "repl.hpp"
#include "ast.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

namespace Repl {
void Start() {

    std::string scanned;
    Evaluator::Evaluator evaluator;
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
        auto evaluated = evaluator.Eval(&program);
        if (evaluated != nullptr && evaluated.get() != nullptr) {
            std::cout << evaluated.get()->Inspect() << std::endl;
        }
    }
}

void printParserErrors(std::vector<std::string> errors) // namespace Repl
{
    std::cout << "Woops! We ran into some monkey business here!\n"
              << " parser errors:\n";
    for (auto error : errors) {
        std::cout << "\t" << error << std::endl;
    }
}
} // namespace Repl
