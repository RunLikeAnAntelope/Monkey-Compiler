#include "ast.hpp"
#include "common.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"

#include <format>
#include <gtest/gtest.h>
#include <memory>
#include <variant>
using Common::variant;

std::unique_ptr<Object::IObject> testEval(const std::string input) {
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    Evaluator::Evaluator evaluator;
    return evaluator.Eval(&program);
}

void testIntegerObject(Object::IObject *object, const long int expected) {
    auto intObj = dynamic_cast<Object::Integer *>(object);
    ASSERT_NE(intObj, nullptr)
        << std::format("object is not an Integer, got a {}",
                       Object::objectTypeToStr(object->Type()));
    ASSERT_EQ(intObj->m_value, expected)
        << std::format("object has wrong value. got {}, expected {}",
                       intObj->m_value, expected);
}

void testBooleanObject(Object::IObject *object, const bool expected) {
    auto boolObj = dynamic_cast<Object::Boolean *>(object);
    ASSERT_NE(boolObj, nullptr)
        << std::format("object is not a Boolean, got a {}",
                       Object::objectTypeToStr(object->Type()));
    ASSERT_EQ(boolObj->m_value, expected)
        << std::format("object has wrong value, got {}, expected {}",
                       boolObj->m_value, expected);
}

void testNullObject(Object::IObject *object) {
    ASSERT_TRUE(object->Type() == Object::ObjectType::NULL_OBJ) << std::format(
        "object is not NULL, got {}", Object::objectTypeToStr(object->Type()));
}

TEST(Evaluator, EvalIntegerExpression) {
    struct test {
        const std::string input;
        const long int expected;
    };

    std::vector<test> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for (auto tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_NE(evaluated, nullptr);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}

TEST(Evaluator, EvalBooleanExpression) {
    struct test {
        const std::string input;
        const bool expected;
    };
    std::vector<test> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };
    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        ASSERT_NE(evaluated, nullptr);
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
        ASSERT_NE(evaluated, nullptr);
        testBooleanObject(evaluated.get(), tst.expected);
    }
}

TEST(Evaluator, IfElseExpression) {
    struct test {
        const std::string input;
        const variant expected;
    };

    std::vector<test> tests = {
        {"if (true) { 10 }", 10},
        {"if (false) { 10 }", std::monostate()},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 }", std::monostate()},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        ASSERT_NE(evaluated, nullptr);
        if (std::holds_alternative<long int>(tst.expected)) {
            testIntegerObject(evaluated.get(),
                              std::get<long int>(tst.expected));
        } else {
            testNullObject(evaluated.get());
        }
    }
}

TEST(Evaluator, TestReturnStatements) {
    struct test {
        const std::string input;
        const long int expected;
    };

    std::vector<test> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {
            "if (10 > 1){if (10 > 1){return 10;}return 1;}",
            10,
        },
    };

    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        ASSERT_NE(evaluated, nullptr);
        testIntegerObject(evaluated.get(), tst.expected);
    }
}

TEST(Evaluator, ErrorHandling) {
    struct test {
        const std::string input;
        const std::string expectedMessage;
    };

    std::vector<test> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "Unsupported infix operator for booleans. Got + expected == or !=",
        },
        {
            "5; true - false; 5",
            "Unsupported infix operator for booleans. Got - expected == or !=",
        },
        {
            "if (10 > 1) { true + false; }",
            "Unsupported infix operator for booleans. Got + expected == or !=",
        },
        {"if (10 > 1){"
         "  if (10 > 1) {"
         "    return true + false;"
         "  }"
         "  return 1;"
         "}",
         "Unsupported infix operator for booleans. Got + expected == or !="},
    };
    for (auto tst : tests) {
        auto evaluated = testEval(tst.input);
        ASSERT_NE(evaluated, nullptr);

        ASSERT_EQ(evaluated->Type(), Object::ObjectType::ERROR_OBJ)
            << std::format("no error object returned, got {}",
                           Object::objectTypeToStr(evaluated->Type()));
        auto errorObj = dynamic_cast<Object::Error *>(evaluated.get());
        ASSERT_EQ(errorObj->m_message, tst.expectedMessage)
            << std::format("wrong error message. expected = {}, got = {}",
                           tst.expectedMessage, errorObj->m_message);
    }
}
