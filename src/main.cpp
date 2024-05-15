#include "repl.hpp"
#include <iostream>

void run(){
    std::cout << "Hello! This is the Monkey programming languag!\n";
    std::cout << "Feel free to type in commands\n";
    Repl::Start();
}

int main() {
    run();
    return 0;
}
