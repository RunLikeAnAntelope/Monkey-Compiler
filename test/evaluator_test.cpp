#include "ast.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"

#include <format>
#include <gtest/gtest.h>

std::unique_ptr<Object::IObject> testEval(const std::string input) {
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    return Evaluator::Eval(&program);
}

void testIntegerObject(Object::IObject *object, const long int expected) {
    auto intObj = dynamic_cast<const Object::Integer *const>(object);
    ASSERT_NE(intObj, nullptr) << " object is not an Integer";
    ASSERT_EQ(intObj->m_value, expected)
        << std::format("object has wrong value. got {}, expected {}",
                       intObj->m_value, expected);
}

TEST(Evaluator, EvalIntegerExpression) {
    struct test {
        const std::string input;
        const long int expected;
    };

    std::vector<test> tests = {{"5", 5}, {"10", 10}};

    for (auto tt : tests) {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}
