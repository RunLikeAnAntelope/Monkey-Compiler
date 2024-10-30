#include "ast.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"

#include <format>
#include <gtest/gtest.h>

const Object::IObject *testEval(std::string input) {
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    return Eval(program);
}

void testIntegerObject(Object::IObject *object, long int expected) {
    auto intObj = dynamic_cast<const Object::Integer *const>(object);
    ASSERT_NE(intObj, nullptr) << " object is not an Integer";
    ASSERT_EQ(intObj->m_value, expected)
        << std::format("object has wrong value. got {}, expected {}",
                       intObj->m_value, expected);
}

TEST(Evaluator, EvalIntegerExpression) {
    struct test {
        std::string input;
        long int expected;
    };

    std::vector<test> tests = {{"5", 5}, {"10", 10}};

    for (auto tt : tests) {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated, tt.expected);
    }
}
