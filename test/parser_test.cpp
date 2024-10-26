#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <ranges>
#include <variant>

typedef std::variant<long int, std::string, bool> variant;
void testLetStatement(std::unique_ptr<Ast::IStatement> &s, std::string name) {
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

void checkParserErrors(Parser::Parser p) {
    for (std::string error : p.m_errors) {
        ASSERT_TRUE(false) << "ParserError:" << error;
    }
}

void testIntegerLiteral(std::unique_ptr<Ast::IExpression> &il, long int value) {
    auto integ = dynamic_cast<Ast::IntegerLiteral *>(il.get());
    ASSERT_TRUE(integ != nullptr) << "Not an IntegerLiteral";

    ASSERT_EQ(integ->m_value, value)
        << "integ->m_value not " << value << ". got=" << integ->m_value;

    ASSERT_EQ(integ->TokenLiteral(), std::to_string(value))
        << "integ->TokenLiteral() not " << value
        << ". got=" << integ->TokenLiteral();
}

void testBooleanLiteral(std::unique_ptr<Ast::IExpression> &exp, bool value) {
    auto boolean = dynamic_cast<Ast::Boolean *>(exp.get());
    ASSERT_TRUE(boolean != nullptr) << "exp not a boolean value";

    ASSERT_EQ(boolean->m_value, value)
        << "boolean.value not " << value << ". got=" << boolean->m_value;

    std::string txtValue = value ? "true" : "false";
    ASSERT_EQ(boolean->TokenLiteral(), txtValue)
        << "boolean.TokenLiteral not " << boolean->TokenLiteral()
        << ". got=" << txtValue;
}

void testIdentifier(std::unique_ptr<Ast::IExpression> &exp, std::string value) {
    auto ident = dynamic_cast<Ast::Identifier *>(exp.get());
    ASSERT_TRUE(ident != nullptr) << "exp not an identifier";

    ASSERT_EQ(value, ident->m_value)
        << "ident->value not " << value << "got=" << ident->m_value;

    ASSERT_EQ(value, ident->TokenLiteral())
        << "ident->TokenLiteral() not " << value
        << "got=" << ident->TokenLiteral();
}

void testLiteralExpression(std::unique_ptr<Ast::IExpression> &exp,
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

void testInfixExpression(Ast::IExpression *exp, variant left, std::string op,
                         variant right) {
    auto opExp = dynamic_cast<Ast::InfixExpression *>(exp);
    ASSERT_TRUE(opExp != nullptr) << "exp is not an InfixExpression";

    testLiteralExpression(opExp->m_left, left);
    testLiteralExpression(opExp->m_right, right);
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
        {"let x = 5;", "x", 5},
        {"let y = true;", "y", true},
        {"let foobar = y;", "foobar", "y"},
    };

    for (LetStmtTest test : tests) {
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
        testLiteralExpression(letStmt->m_expression, test.expectedValue);
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

    for (unsigned int i = 0; auto test : tests) {
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

    std::vector<booleanTest> tests = {{"true;", true}, {"false;", false}};
    for (booleanTest btest : tests) {
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
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!foobar;", "!", "foobar"},
        {"-foobar;", "-", "foobar"},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    for (auto test : tests) {
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
        testLiteralExpression(exp->m_right, test.value);
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
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
        {"true == true;", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };
    for (auto test : tests) {
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

        testLiteralExpression(exp->m_left, test.leftValue);
        ASSERT_EQ(exp->m_op, test.op);
        testLiteralExpression(exp->m_right, test.rightValue);
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
            "-a * b",
            "((-a) * b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "true",
            "true",
        },
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {
            "!(true == true)",
            "(!(true == true))",
        },
    };

    for (auto test : tests) {
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

    testIdentifier(cExpStmt->m_expression, "x");

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

    testIdentifier(cExpStmt->m_expression, "x");

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

    testIdentifier(aExpStmt->m_expression, "y");
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
    std::unique_ptr<Ast::IExpression> xIEepr = std::move(exp->m_parameters[0]);
    std::unique_ptr<Ast::IExpression> yIExpr = std::move(exp->m_parameters[1]);
    testLiteralExpression(xIEepr, x);
    testLiteralExpression(yIExpr, y);

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

    std::vector<test> tests = {{"fn() {};", {}},
                               {"fn(x) {};", {"x"}},
                               {"fn(x, y ,z) {}", {"x", "y", "z"}}};

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
            auto parameter = dynamic_cast<Ast::IExpression *>(
                function->m_parameters[i].get());

            testLiteralExpression(std::make_unique(parameter), expectedParam);
        }
    }
}
