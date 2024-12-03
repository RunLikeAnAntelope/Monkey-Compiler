#include "ast.hpp"
#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <utility>
using Common::variant;
static void testLetStatement(std::unique_ptr<Ast::IStatement> &s,
                             const std::string &name) {
    Ast::IStatement *stmt = s.get();
    Ast::LetStatement *letStmt = dynamic_cast<Ast::LetStatement *>(stmt);
    if (letStmt != nullptr) {
        ASSERT_EQ(s->TokenLiteral(), "let");
        ASSERT_EQ(letStmt->m_name->m_value, name);
        ASSERT_EQ(letStmt->m_name->TokenLiteral(), name);
    } else {
        ASSERT_TRUE(false) << "nullptr into testLetStatement";
    }
}

static void checkParserErrors(const Parser::Parser &p) {
    std::string errors;
    for (const std::string &error : p.m_errors) {
        errors.append("ParserError:" + error + "\n");
    }
    ASSERT_EQ(std::ssize(p.m_errors), 0) << errors << '\n';
}

static void testIntegerLiteral(Ast::IExpression &il, long int value) {
    try {
        auto integ = dynamic_cast<Ast::IntegerLiteral &>(il);
        ASSERT_EQ(integ.m_value, value)
            << "integ->m_value not " << value << ". got=" << integ.m_value;

        ASSERT_EQ(integ.TokenLiteral(), std::to_string(value))
            << "integ->TokenLiteral() not " << value
            << ". got=" << integ.TokenLiteral();

    } catch (const std::bad_cast &e) {
        FAIL() << "Expression not an IntegerLiteral";
    }
}

static void testBooleanLiteral(Ast::IExpression &exp, bool value) {
    try {
        auto boolean = dynamic_cast<Ast::Boolean &>(exp);

        ASSERT_EQ(boolean.m_value, value)
            << "boolean.value not " << value << ". got=" << boolean.m_value;

        std::string txtValue = value ? "true" : "false";
        ASSERT_EQ(boolean.TokenLiteral(), txtValue)
            << "boolean.TokenLiteral not " << boolean.TokenLiteral()
            << ". got=" << txtValue;
    } catch (const std::bad_cast &e) {
        FAIL() << "IExpression not a Boolean";
    }
}

static void testIdentifier(Ast::IExpression &exp, const std::string &value) {
    try {
        auto ident = dynamic_cast<Ast::Identifier &>(exp);

        ASSERT_EQ(value, ident.m_value)
            << "ident->value not " << value << "got=" << ident.m_value;

        ASSERT_EQ(value, ident.TokenLiteral())
            << "ident->TokenLiteral() not " << value
            << "got=" << ident.TokenLiteral();
    } catch (const std::bad_cast &e) {
        FAIL() << "IExpression not a Identifier";
    }
}

static void testLiteralExpression(Ast::IExpression &exp,
                                  variant expectedValue) {
    if (std::holds_alternative<long int>(expectedValue)) {
        testIntegerLiteral(exp, std::get<long int>(expectedValue));
    } else if (std::holds_alternative<std::string>(expectedValue)) {
        testIdentifier(exp, std::get<std::string>(expectedValue));
    } else if (std::holds_alternative<bool>(expectedValue)) {
        testBooleanLiteral(exp, std::get<bool>(expectedValue));
    } else {
        ASSERT_TRUE(false)
            << "type of exp not handled in testLiteralExpression";
    }
}

static void testInfixExpression(Ast::IExpression *exp, variant left,
                                const std::string &op, variant right) {
    auto opExp = dynamic_cast<Ast::InfixExpression *>(exp);
    ASSERT_TRUE(opExp != nullptr) << "exp is not an InfixExpression";

    testLiteralExpression(*opExp->m_left, std::move(left));
    testLiteralExpression(*opExp->m_right, std::move(right));
    ASSERT_EQ(opExp->m_op, op)
        << "exp.Operator is not " << op << ". got=" << opExp->m_op;
}

TEST(Parser, LetStatements) {
    struct LetStmtTest {
        std::string input;
        std::string expectedIdentifier;
        variant expectedValue;
    };

    std::vector<LetStmtTest> tests = {
        {.input = "let x = 5;", .expectedIdentifier = "x", .expectedValue = 5},
        {.input = "let y = true;",
         .expectedIdentifier = "y",
         .expectedValue = true},
        {.input = "let foobar = y;",
         .expectedIdentifier = "foobar",
         .expectedValue = "y"},
    };

    for (const LetStmtTest &test : tests) {
        Lexer::Lexer l(test.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        ASSERT_TRUE(program.m_statements.size() == 1)
            << "program.m_statements does not contain 1 statement. got="
            << program.m_statements.size();

        std::unique_ptr<Ast::IStatement> stmt =
            std::move(program.m_statements[0]);
        testLetStatement(stmt, test.expectedIdentifier);

        Ast::IStatement *lStmt = stmt.get();
        Ast::LetStatement *letStmt = dynamic_cast<Ast::LetStatement *>(lStmt);
        testLiteralExpression(*letStmt->m_expression, test.expectedValue);
    }
}

TEST(Parser, LetParser) {
    std::string input = "let x = 5;\n"
                        "let y = 10;\n"
                        "let foobar = 838383;\n";

    struct test {
        std::string expectedIdentier;
    };

    std::vector<test> tests = {{"x"}, {"y"}, {"foobar"}};

    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);
    // Doesn't work as program no longer is a pointer
    // ASSERT_FALSE(program == nullptr) << "ParseProgram() returned a
    // nullptr";

    for (unsigned int i = 0; const auto &test : tests) {
        testLetStatement(program.m_statements[i], test.expectedIdentier);
        i++;
    }
}

TEST(Parser, ReturnParser) {
    std::string input = "return 5;\n"
                        "return 7;\n"
                        "return 993322;";
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 3) << "program.Statements does not"
                                                 "contain 3 statements. got "
                                              << program.m_statements.size();

    for (auto const &stmt : program.m_statements) {
        Ast::IStatement *iStmt = stmt.get();
        ASSERT_TRUE(dynamic_cast<Ast::ReturnStatement *>(iStmt) != nullptr);
        ASSERT_EQ(stmt->TokenLiteral(), "return") << "TokenLiteral not return";
    }
}

TEST(Parser, ReturnStatements) {
    struct test {
        std::string input;
        variant expectedValue;
    };

    std::vector<test> tests = {{.input = "return 5;", .expectedValue = 5},
                               {.input = "return true;", .expectedValue = true},
                               {"return foobar;", "foobar"}};

    for (const auto &tst : tests) {
        Lexer::Lexer l(tst.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program.m_statements.size(), 1)
            << "program.Statements does not"
               "contain 1 statements. got "
            << program.m_statements.size();

        auto returnStmt =
            dynamic_cast<Ast::ReturnStatement *>(program.m_statements[0].get());

        if (returnStmt == nullptr) {
            FAIL() << "IStatement is not a ReturnStatement";
        }

        ASSERT_EQ(returnStmt->TokenLiteral(), "return")
            << "returnStmt.TokenLiteral not 'return', got "
            << returnStmt->TokenLiteral();

        testLiteralExpression(*returnStmt->m_returnValue, tst.expectedValue);
    }
}

TEST(Parser, IdentifierExpression) {
    std::string input = "foobar;";
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 1);

    Ast::IStatement *iStmt = program.m_statements[0].get();
    auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
    ASSERT_TRUE(stmt != nullptr);

    Ast::IExpression *iExp = stmt->m_expression.get();
    auto ident = dynamic_cast<Ast::Identifier *>(iExp);
    ASSERT_TRUE(ident != nullptr);

    ASSERT_TRUE(ident->m_value == "foobar");
    ASSERT_TRUE(ident->TokenLiteral() == "foobar");
}

TEST(Parser, IntegerLiteralExpression) {
    std::string input = "5;";
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 1);

    Ast::IStatement *iStmt = program.m_statements[0].get();
    auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
    ASSERT_TRUE(stmt != nullptr);

    Ast::IExpression *iExp = stmt->m_expression.get();
    auto intLiteral = dynamic_cast<Ast::IntegerLiteral *>(iExp);
    ASSERT_TRUE(intLiteral != nullptr);

    ASSERT_TRUE(intLiteral->m_value == 5);
    ASSERT_TRUE(intLiteral->TokenLiteral() == "5");
}

TEST(Parser, BooleanLiteralExpression) {
    struct booleanTest {
        std::string input;
        bool expectedBoolean;
    };

    std::vector<booleanTest> tests = {
        {.input = "true;", .expectedBoolean = true}, {"false;", false}};
    for (const booleanTest &btest : tests) {
        Lexer::Lexer l(btest.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program.m_statements.size(), 1);

        Ast::IStatement *iStmt = program.m_statements[0].get();
        auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
        ASSERT_TRUE(stmt != nullptr);

        Ast::IExpression *iExp = stmt->m_expression.get();
        auto boolLiteral = dynamic_cast<Ast::Boolean *>(iExp);
        ASSERT_TRUE(boolLiteral != nullptr);

        ASSERT_TRUE(boolLiteral->m_value == btest.expectedBoolean);
    }
}

TEST(Parser, ParsingPrefixExpressions) {
    struct PrefixTest {
        std::string input;
        std::string op;
        variant value;
    };

    std::vector<PrefixTest> tests = {
        {.input = "!5;", .op = "!", .value = 5},
        {.input = "-15;", .op = "-", .value = 15},
        {.input = "!foobar;", .op = "!", .value = "foobar"},
        {.input = "-foobar;", .op = "-", .value = "foobar"},
        {.input = "!true;", .op = "!", .value = true},
        {.input = "!false;", .op = "!", .value = false},
    };

    for (const auto &test : tests) {
        Lexer::Lexer l(test.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program.m_statements.size(), 1);
        Ast::IStatement *iStmt = program.m_statements[0].get();

        auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
        ASSERT_TRUE(stmt != nullptr);

        Ast::IExpression *iExp = stmt->m_expression.get();
        auto exp = dynamic_cast<Ast::PrefixExpression *>(iExp);
        ASSERT_TRUE(exp != nullptr);

        ASSERT_EQ(exp->m_op, test.op);
        testLiteralExpression(*exp->m_right, test.value);
    }
}

TEST(Parser, ParsingInfixExpressions) {
    struct InfixTest {
        std::string input;
        variant leftValue;
        std::string op;
        variant rightValue;
    };

    std::vector<InfixTest> tests = {
        {.input = "5 + 5;", .leftValue = 5, .op = "+", .rightValue = 5},
        {.input = "5 - 5;", .leftValue = 5, .op = "-", .rightValue = 5},
        {.input = "5 * 5;", .leftValue = 5, .op = "*", .rightValue = 5},
        {.input = "5 / 5;", .leftValue = 5, .op = "/", .rightValue = 5},
        {.input = "5 > 5;", .leftValue = 5, .op = ">", .rightValue = 5},
        {.input = "5 < 5;", .leftValue = 5, .op = "<", .rightValue = 5},
        {.input = "5 == 5;", .leftValue = 5, .op = "==", .rightValue = 5},
        {.input = "5 != 5;", .leftValue = 5, .op = "!=", .rightValue = 5},
        {.input = "true == true;",
         .leftValue = true,
         .op = "==",
         .rightValue = true},
        {.input = "true != false",
         .leftValue = true,
         .op = "!=",
         .rightValue = false},
        {.input = "false == false",
         .leftValue = false,
         .op = "==",
         .rightValue = false},
    };
    for (const auto &test : tests) {
        Lexer::Lexer l(test.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        ASSERT_EQ(program.m_statements.size(), 1);

        Ast::IStatement *iStmt = program.m_statements[0].get();
        auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
        ASSERT_TRUE(stmt != nullptr);

        Ast::IExpression *iExp = stmt->m_expression.get();
        auto exp = dynamic_cast<Ast::InfixExpression *>(iExp);
        ASSERT_TRUE(exp != nullptr);

        testLiteralExpression(*exp->m_left, test.leftValue);
        ASSERT_EQ(exp->m_op, test.op);
        testLiteralExpression(*exp->m_right, test.rightValue);
        testInfixExpression(iExp, test.leftValue, test.op, test.rightValue);
    }
}

TEST(Parser, OperatorPrecedenceParsing) {
    struct OPPTest {
        std::string input;
        std::string expected;
    };

    std::vector<OPPTest> tests = {
        {
            .input = "-a * b",
            .expected = "((-a) * b)",
        },
        {
            .input = "!-a",
            .expected = "(!(-a))",
        },
        {
            .input = "a + b + c",
            .expected = "((a + b) + c)",
        },
        {
            .input = "a + b - c",
            .expected = "((a + b) - c)",
        },
        {
            .input = "a * b * c",
            .expected = "((a * b) * c)",
        },
        {
            .input = "a * b / c",
            .expected = "((a * b) / c)",
        },
        {
            .input = "a + b / c",
            .expected = "(a + (b / c))",
        },
        {
            .input = "a + b * c + d / e - f",
            .expected = "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            .input = "3 + 4; -5 * 5",
            .expected = "(3 + 4)((-5) * 5)",
        },
        {
            .input = "5 > 4 == 3 < 4",
            .expected = "((5 > 4) == (3 < 4))",
        },
        {
            .input = "5 < 4 != 3 > 4",
            .expected = "((5 < 4) != (3 > 4))",
        },
        {
            .input = "3 + 4 * 5 == 3 * 1 + 4 * 5",
            .expected = "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            .input = "true",
            .expected = "true",
        },
        {
            .input = "false",
            .expected = "false",
        },
        {
            .input = "3 > 5 == false",
            .expected = "((3 > 5) == false)",
        },
        {
            .input = "3 < 5 == true",
            .expected = "((3 < 5) == true)",
        },
        {
            .input = "1 + (2 + 3) + 4",
            .expected = "((1 + (2 + 3)) + 4)",
        },
        {
            .input = "(5 + 5) * 2",
            .expected = "((5 + 5) * 2)",
        },
        {
            .input = "2 / (5 + 5)",
            .expected = "(2 / (5 + 5))",
        },
        {
            .input = "-(5 + 5)",
            .expected = "(-(5 + 5))",
        },
        {
            .input = "!(true == true)",
            .expected = "(!(true == true))",
        },
        {
            .input = "a + add(b * c) + d",
            .expected = "((a + add((b * c))) + d)",
        },
        {
            .input = "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            .expected = "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
            .input = "add(a + b + c * d / f + g)",
            .expected = "add((((a + b) + ((c * d) / f)) + g))",
        },
    };

    for (const auto &test : tests) {
        Lexer::Lexer l(test.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        std::string actual = program.String();
        ASSERT_EQ(actual, test.expected);
    }
}

TEST(Parser, IfExpressionParsing) {
    std::string input = "if (x < y) { x }";

    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 1)
        << "program.m_statements does not contain 1 statement. got="
        << program.m_statements.size();

    Ast::IStatement *iStmt = program.m_statements[0].get();
    auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
    ASSERT_TRUE(stmt != nullptr)
        << "program.m_statements[0] is not an AST::ExpressionStatement";

    Ast::IExpression *iExp = stmt->m_expression.get();
    auto exp = dynamic_cast<Ast::IfExpression *>(iExp);
    ASSERT_TRUE(exp != nullptr) << "stmt->m_expression is not an IfExpression";

    variant left("x");
    variant right("y");
    testInfixExpression(exp->m_condition.get(), left, "<", right);

    ASSERT_EQ(exp->m_consequence->m_statements.size(), 1)
        << "consequence is not 1 statement. got="
        << exp->m_consequence->m_statements.size();

    Ast::IStatement *cStmt = exp->m_consequence->m_statements[0].get();
    auto cExpStmt = dynamic_cast<Ast::ExpressionStatement *>(cStmt);
    ASSERT_TRUE(cExpStmt != nullptr)
        << "Statements[0] is not an Ast::ExpressionStatement";

    testIdentifier(*cExpStmt->m_expression, "x");

    ASSERT_EQ(exp->m_alternative, nullptr) << "exp->Alternative was not null";
}

TEST(Parser, IfElseExpressionParsing) {
    std::string input = "if (x < y) { x } else { y }";

    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 1)
        << "program.m_statements does not contain 1 statement. got="
        << program.m_statements.size();

    Ast::IStatement *iStmt = program.m_statements[0].get();
    auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
    ASSERT_TRUE(stmt != nullptr)
        << "program.m_statements[0] is not an AST::ExpressionStatement";

    Ast::IExpression *iExp = stmt->m_expression.get();
    auto exp = dynamic_cast<Ast::IfExpression *>(iExp);
    ASSERT_TRUE(exp != nullptr) << "stmt->m_expression is not an IfExpression";

    variant left("x");
    variant right("y");
    testInfixExpression(exp->m_condition.get(), left, "<", right);

    // Test true path
    ASSERT_EQ(exp->m_consequence->m_statements.size(), 1)
        << "consequence is not 1 statement. got="
        << exp->m_consequence->m_statements.size();

    Ast::IStatement *cStmt = exp->m_consequence->m_statements[0].get();
    auto cExpStmt = dynamic_cast<Ast::ExpressionStatement *>(cStmt);
    ASSERT_TRUE(cExpStmt != nullptr)
        << "Statements[0] is not an Ast::ExpressionStatement";

    testIdentifier(*cExpStmt->m_expression, "x");

    // Set Else Path
    ASSERT_FALSE(exp->m_alternative == nullptr)
        << "exp->Alternative was not null";

    ASSERT_EQ(exp->m_alternative->m_statements.size(), 1)
        << "consequence is not 1 statement. got="
        << exp->m_alternative->m_statements.size();

    Ast::IStatement *aStmt = exp->m_alternative->m_statements[0].get();
    auto aExpStmt = dynamic_cast<Ast::ExpressionStatement *>(aStmt);
    ASSERT_TRUE(aExpStmt != nullptr)
        << "Statements[0] is not an Ast::ExpressionStatement";

    testIdentifier(*aExpStmt->m_expression, "y");
}

TEST(Parser, FunctionLiteralParsing) {
    std::string input = "fn(x, y) { x + y;}";

    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program.m_statements.size(), 1)
        << "program.m_statements does not contain 1 statement. got="
        << program.m_statements.size();

    Ast::IStatement *iStmt = program.m_statements[0].get();
    auto stmt = dynamic_cast<Ast::ExpressionStatement *>(iStmt);
    ASSERT_TRUE(stmt != nullptr)
        << "program.m_statements[0] is not an AST::ExpressionStatement";

    Ast::IExpression *iExp = stmt->m_expression.get();
    auto exp = dynamic_cast<Ast::FunctionLiteral *>(iExp);
    ASSERT_TRUE(exp != nullptr)
        << "stmt->m_expression is not an FunctionLiteral";

    ASSERT_EQ(std::ssize(exp->m_parameters), 2)
        << "function literal parameters wrong. want 2. got="
        << std::ssize(exp->m_parameters);

    variant x("x");
    variant y("y");
    testLiteralExpression(*exp->m_parameters[0], x);
    testLiteralExpression(*exp->m_parameters[1], y);

    ASSERT_EQ(std::ssize(exp->m_body->m_statements), 1)
        << "function body statement does not have 1 statement got="
        << std::ssize(exp->m_body->m_statements);

    Ast::IStatement *bodyStatement = exp->m_body->m_statements[0].get();
    auto bodyExp = dynamic_cast<Ast::ExpressionStatement *>(bodyStatement);

    ASSERT_TRUE(bodyExp != nullptr)
        << "function body stmt is not Ast::ExpressionStatement";
    testInfixExpression(bodyExp->m_expression.get(), x, "+", y);
}

TEST(Parser, FunctionParameterParsing) {
    struct test {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<test> tests = {
        {.input = "fn() {};", .expectedParams = {}},
        {.input = "fn(x) {};", .expectedParams = {"x"}},
        {.input = "fn(x, y ,z) {}", .expectedParams = {"x", "y", "z"}}};

    for (auto tst : tests) {
        Lexer::Lexer l(tst.input);
        Parser::Parser p(l);
        Ast::Program program = p.ParseProgram();
        checkParserErrors(p);

        auto stmt = dynamic_cast<Ast::ExpressionStatement *>(
            program.m_statements[0].get());
        ASSERT_TRUE(stmt != nullptr);

        auto function =
            dynamic_cast<Ast::FunctionLiteral *>(stmt->m_expression.get());
        ASSERT_TRUE(function != nullptr);

        ASSERT_EQ(std::ssize(function->m_parameters),
                  std::ssize(tst.expectedParams))
            << "number of parameters wrong. Expected "
            << std::ssize(tst.expectedParams)
            << ", got=" << std::ssize(function->m_parameters);

        for (unsigned int i = 0; i < std::ssize(function->m_parameters); i++) {
            variant expectedParam(tst.expectedParams[i]);

            testLiteralExpression(*function->m_parameters[i], expectedParam);
        }
    }
}

TEST(Parser, CallExpressionParsing) {
    std::string input = "add(1, 2 * 3, 4 + 5)";
    Lexer::Lexer l(input);
    Parser::Parser p(l);
    Ast::Program program = p.ParseProgram();
    checkParserErrors(p);

    ASSERT_EQ(std::ssize(program.m_statements), 1)
        << "program.m_statements does not contain 1 statement. got="
        << std::ssize(program.m_statements);

    auto stmt =
        dynamic_cast<Ast::ExpressionStatement *>(program.m_statements[0].get());
    ASSERT_TRUE(stmt != nullptr) << "Statement is not an ExpressionStatement";

    auto exp = dynamic_cast<Ast::CallExpression *>(stmt->m_expression.get());
    ASSERT_TRUE(exp != nullptr)
        << "ExpressionStatement is not a CallExpression";

    variant expectedIdentifier("add");
    testIdentifier(*exp->m_function, "add");

    ASSERT_EQ(std::ssize(exp->m_arguments), 3)
        << "wrong length of arguments. Expected 3, got="
        << std::ssize(exp->m_arguments);

    testLiteralExpression(*exp->m_arguments[0], 1);
    testInfixExpression(exp->m_arguments[1].get(), 2, "*", 3);
    testInfixExpression(exp->m_arguments[2].get(), 4, "+", 5);
}
