#include "ast.hpp"
#include "common.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"

#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <variant>
using Common::variant;

static std::shared_ptr<Object::IObject> testEval(const std::string &input) {
  Lexer::Lexer l(input);
  Parser::Parser p(l);
  Ast::Program program = p.ParseProgram();
  Evaluator::Evaluator evaluator;
  std::shared_ptr<Object::Environment> env =
    std::make_shared<Object::Environment>();
  auto output = evaluator.Eval(&program, env);
  if (output != nullptr) {
    if (output->Type() == Object::ObjectType::ERROR_OBJ) {
      auto err = dynamic_cast<Object::Error *>(output.get());
      std::cout << err->m_message << "\n";
    }
  }
  return output;
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
    {.input = "\"Hello\" - \"world\"",
     .expectedMessage = "unknown operator: STRING - STRING"},
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

TEST(Evaluator, FunctionApplication) {
  struct test {
    std::string input;
    const long int expected;
  };

  std::vector<test> tests = {
    {.input = "let identity = fn(x) { x; }; identity(5);", .expected = 5},
    {.input = "let identity = fn(x) { return x; }; identity(5);",
     .expected = 5},
    {.input = "let double = fn(x) { x * 2; }; double(5);", .expected = 10},
    {.input = "let add = fn(x, y) { x + y; }; add(5, 5);", .expected = 10},
    {.input = "let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));",
     .expected = 20},
    {.input = "fn(x) { x; }(5)", .expected = 5},
  };
  for (const auto &tst : tests) {
    testIntegerObject(testEval(tst.input).get(), tst.expected);
  }
}
TEST(Evaluator, EnclosingEnvironment) {
  std::string input = "let first = 10;"
                      "let second = 10;"
                      "let third = 10;"
                      "let ourFunction = fn(first) {"
                      "let second = 20;"
                      "first + second + third;"
                      "};"
                      "ourFunction(20) + first + second;";
  testIntegerObject(testEval(input).get(), 70);
}

TEST(Evaluator, Closures) {
  std::string input = "let newAdder = fn(x) { fn(y) {x + y};}; let addTwo = "
                      "newAdder(2); addTwo(2);";
  testIntegerObject(testEval(input).get(), 4);
}

TEST(Evaluator, String) {
  std::string input = "\"Hello World\"";
  auto evaluated = testEval(input);
  auto *str = dynamic_cast<Object::String *>(evaluated.get());
  ASSERT_NE(nullptr, str) << std::format(
    "object is not a string, got={}",
    Object::objectTypeToStr(evaluated.get()->Type()));
}

TEST(Evaluator, StringConcatenation) {
  std::string input = "\"Hello\" + \" \" + \"World!\"";
  auto evaluated = testEval(input);
  auto *str = dynamic_cast<Object::String *>(evaluated.get());
  ASSERT_NE(nullptr, str) << std::format(
    "object is not a string, got={}",
    Object::objectTypeToStr(evaluated.get()->Type()));
  ;
  ASSERT_EQ(str->m_value, "Hello World!") << std::format(
    "String has wrong value. Exprected \"Hello World!\". got={}", str->m_value);
}

TEST(Evaluator, BuiltInLen) {
  struct test {
    const std::string input;
    const variant expected;
  };
  std::vector<test> tests = {
    {.input = "len(\"\")", .expected = 0},
    {.input = "len(\"four\")", .expected = 4},
    {.input = "len(\"hello world\")", .expected = 11},
    {.input = "len(1)",
     .expected = "argument to 'len' not supported, got INTEGER"},
    {.input = "len(\"one\",\"two\")",
     .expected = "wrong number of arguments. got=2. want=1"},
  };
  // int counter = 0;
  // for (const auto &tst : tests) {
  //   std::cout << "whoops" << counter << "\n";
  //   counter++;
  //   auto *evaluated = testEval(tst.input).get();
  //   switch (evaluated->Type()) {
  //   case Object::ObjectType::INTEGER_OBJ: {
  //     testIntegerObject(evaluated, std::get<long int>(tst.expected));
  //     break;
  //   }
  //   case Object::ObjectType::ERROR_OBJ: {
  //     auto errorObj = dynamic_cast<Object::Error *>(evaluated);
  //     if (counter == 4) {
  //       std::cout << std::get<std::string>(tst.expected) << std::endl;
  //       std::cout << errorObj->m_message << std::endl;
  //     }
  //     ASSERT_NE(nullptr, errorObj)
  //       << std::format("object is not Error. got={}",
  //                      Object::objectTypeToStr(evaluated->Type()));
  //
  //     ASSERT_TRUE(std::holds_alternative<std::string>(tst.expected))
  //       << "tst.expected does not hold a std::string";
  //     // ASSERT_EQ(errorObj->m_message, std::get<std::string>(tst.expected))
  //     //   << std::format("wrong error message. expected={}, got ={}",
  //     //                  std::get<std::string>(tst.expected),
  //     //                  errorObj->m_message);
  //     break;
  //   }
  //   default:
  //     FAIL() << "Build in length returned an object type that is not a
  //     integer "
  //               "or an object. got="
  //            << Object::objectTypeToStr(evaluated->Type());
  //   }
  // }
}
