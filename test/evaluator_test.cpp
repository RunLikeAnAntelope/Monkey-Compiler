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
    Evaluator::Evaluator evaluator;
    return evaluator.Eval(&program);
}

void testIntegerObject(Object::IObject *object, const long int expected) {
    auto intObj = dynamic_cast<Object::Integer *>(object);
    ASSERT_NE(intObj, nullptr) << " object is not an Integer";
    ASSERT_EQ(intObj->m_value, expected)
        << std::format("object has wrong value. got {}, expected {}",
                       intObj->m_value, expected);
}

void testBooleanObject(Object::IObject *object, const bool expected) {
    auto boolObj = dynamic_cast<Object::Boolean *>(object);
    ASSERT_NE(boolObj, nullptr) << "object is not a Boolean";
    ASSERT_EQ(boolObj->m_value, expected)
        << std::format("object has wrong value, got {}, expected {}",
                       boolObj->m_value, expected);
}

TEST(Evaluator, EvalIntegerExpression) {
    struct test {
        const std::string input;
        const long int expected;
    };

    std::vector<test> tests = {{"5", 5}, {"10", 10}, {"-5", -5}, {"-10", -10}};

    for (auto tt : tests) {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}

TEST(Evaluator, EvalBooleanExpression) {
    struct test {
        const std::string input;
        const bool expected;
    };
    std::vector<test> tests = {{"true", true}, {"false", false}};
    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        testBooleanObject(evaluated.get(), tst.expected);
    }
}

TEST(Evaluator, BangOperator) {
    struct test {
        const std::string input;
        const bool expected;
    };
    std::vector<test> tests{{"!true", false},   {"!false", true},
                            {"!5", false},      {"!!true", true},
                            {"!!false", false}, {"!!5", true}};
    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        testBooleanObject(evaluated.get(), tst.expected);
    }
}
