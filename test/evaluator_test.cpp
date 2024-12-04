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

static std::shared_ptr<Object::IObject> testEval(const std::string &input) {
  Lexer::Lexer l(input);
  Parser::Parser p(l);
  Ast::Program program = p.ParseProgram();
  Evaluator::Evaluator evaluator;
  return evaluator.Eval(&program);
}

static void testIntegerObject(Object::IObject *object,
                              const long int expected) {
  auto intObj = dynamic_cast<Object::Integer *>(object);
  ASSERT_NE(intObj, nullptr)
    << std::format("object is not an Integer, got a {}",
                   Object::objectTypeToStr(object->Type()));
  ASSERT_EQ(intObj->m_value, expected) << std::format(
    "object has wrong value. got {}, expected {}", intObj->m_value, expected);
}

static void testBooleanObject(Object::IObject *object, const bool expected) {
  auto boolObj = dynamic_cast<Object::Boolean *>(object);
  ASSERT_NE(boolObj, nullptr)
    << std::format("object is not a Boolean, got a {}",
                   Object::objectTypeToStr(object->Type()));
  ASSERT_EQ(boolObj->m_value, expected) << std::format(
    "object has wrong value, got {}, expected {}", boolObj->m_value, expected);
}

static void testNullObject(Object::IObject *object) {
  ASSERT_TRUE(object->Type() == Object::ObjectType::NULL_OBJ) << std::format(
    "object is not NULL, got {}", Object::objectTypeToStr(object->Type()));
}

TEST(Evaluator, EvalIntegerExpression) {
  struct test {
    const std::string input;
    const long int expected;
  };

  std::vector<test> tests = {
    {.input = "5", .expected = 5},
    {.input = "10", .expected = 10},
    {.input = "-5", .expected = -5},
    {.input = "-10", .expected = -10},
    {.input = "5 + 5 + 5 + 5 - 10", .expected = 10},
    {.input = "2 * 2 * 2 * 2 * 2", .expected = 32},
    {.input = "-50 + 100 + -50", .expected = 0},
    {.input = "5 * 2 + 10", .expected = 20},
    {.input = "5 + 2 * 10", .expected = 25},
    {.input = "20 + 2 * -10", .expected = 0},
    {.input = "50 / 2 * 2 + 10", .expected = 60},
    {.input = "2 * (5 + 10)", .expected = 30},
    {.input = "3 * 3 * 3 + 10", .expected = 37},
    {.input = "3 * (3 * 3) + 10", .expected = 37},
    {.input = "(5 + 10 * 2 + 15 / 3) * 2 + -10", .expected = 50},
  };

  for (const auto &tt : tests) {
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
    {.input = "true", .expected = true},
    {.input = "false", .expected = false},
    {.input = "1 < 2", .expected = true},
    {.input = "1 > 2", .expected = false},
    {.input = "1 < 1", .expected = false},
    {.input = "1 > 1", .expected = false},
    {.input = "1 == 1", .expected = true},
    {.input = "1 != 1", .expected = false},
    {.input = "1 == 2", .expected = false},
    {.input = "1 != 2", .expected = true},
    {.input = "true == true", .expected = true},
    {.input = "false == false", .expected = true},
    {.input = "true == false", .expected = false},
    {.input = "true != false", .expected = true},
    {.input = "false != true", .expected = true},
    {.input = "(1 < 2) == true", .expected = true},
    {.input = "(1 < 2) == false", .expected = false},
    {.input = "(1 > 2) == true", .expected = false},
    {.input = "(1 > 2) == false", .expected = true},
  };
  for (const auto &tst : tests) {
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
  std::vector<test> tests{{.input = "!true", .expected = false},
                          {.input = "!false", .expected = true},
                          {.input = "!5", .expected = false},
                          {.input = "!!true", .expected = true},
                          {.input = "!!false", .expected = false},
                          {.input = "!!5", .expected = true}};
  for (const auto &tst : tests) {
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
    {.input = "if (true) { 10 }", .expected = 10},
    {.input = "if (false) { 10 }", .expected = std::monostate()},
    {.input = "if (1) { 10 }", .expected = 10},
    {.input = "if (1 < 2) { 10 }", .expected = 10},
    {.input = "if (1 > 2) { 10 }", .expected = std::monostate()},
    {.input = "if (1 > 2) { 10 } else { 20 }", .expected = 20},
    {.input = "if (1 < 2) { 10 } else { 20 }", .expected = 10},
  };

  for (const auto &tst : tests) {
    auto evaluated = testEval(tst.input);
    ASSERT_NE(evaluated, nullptr);
    if (std::holds_alternative<long int>(tst.expected)) {
      testIntegerObject(evaluated.get(), std::get<long int>(tst.expected));
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
    {.input = "return 10;", .expected = 10},
    {.input = "return 10; 9;", .expected = 10},
    {.input = "return 2 * 5; 9;", .expected = 10},
    {.input = "9; return 2 * 5; 9;", .expected = 10},
    {
      .input = "if (10 > 1){if (10 > 1){return 10;}return 1;}",
      .expected = 10,
    },
  };

  for (const auto &tst : tests) {
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
      .input = "5 + true;",
      .expectedMessage = "type mismatch: INTEGER + BOOLEAN",
    },
    {
      .input = "5 + true; 5;",
      .expectedMessage = "type mismatch: INTEGER + BOOLEAN",
    },
    {
      .input = "-true",
      .expectedMessage = "unknown operator: -BOOLEAN",
    },
    {
      .input = "true + false;",
      .expectedMessage = "Unsupported infix operator for booleans. Got + "
                         "expected == or !=",
    },
    {
      .input = "5; true - false; 5",
      .expectedMessage = "Unsupported infix operator for booleans. Got - "
                         "expected == or !=",
    },
    {
      .input = "if (10 > 1) { true + false; }",
      .expectedMessage = "Unsupported infix operator for booleans. Got + "
                         "expected == or !=",
    },
    {
      .input = "if (10 > 1){"
               "  if (10 > 1) {"
               "    return true + false;"
               "  }"
               "  return 1;"
               "}",
      .expectedMessage = "Unsupported infix operator for booleans. Got + "
                         "expected == or !=",
    },
    {
      .input = "foobar",
      .expectedMessage = "identifier not found: foobar",
    },
  };
  for (const auto &tst : tests) {
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

TEST(Evaluator, LetStatements) {
  struct test {
    std::string input;
    const long int expected;
  };

  std::vector<test> tests = {
    {.input = "let a = 5; a;", .expected = 5},
    {.input = "let a = 5 * 5; a;", .expected = 25},
    {.input = "let a = 5; let b = a; b;", .expected = 5},
    {.input = "let a = 5; let b = a; let c = a + b + 5; c;", .expected = 15},
  };

  for (const auto &tst : tests) {
    testIntegerObject(testEval(tst.input).get(), tst.expected);
  }
}

TEST(Evaluator, FunctionObject) {
  std::string input = "fn(x) { x + 2;}";
  auto evaluated = testEval(input);
  ASSERT_EQ(evaluated->Type(), Object::ObjectType::FUNCTION_OBJ)
    << std::format("object is not Function, got={}",
                   Object::objectTypeToStr(evaluated->Type()));

  auto *fun = dynamic_cast<Object::Function *>(evaluated.get());
  ASSERT_EQ(std::ssize(fun->m_parameters), 1)
    << "function has wrong number of parameters";

  ASSERT_EQ(fun->m_parameters[0]->String(), "x") << std::format(
    "parameters is not 'x', got=", fun->m_parameters[0]->String());
  std::string expectedBody = "(x + 2)";

  ASSERT_EQ(fun->m_body->String(), expectedBody) << std::format(
    "body is not {}, got={}", expectedBody, fun->m_body->String());
}
