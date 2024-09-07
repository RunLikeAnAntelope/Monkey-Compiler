#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <variant>

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

void testIdentifier(std::unique_ptr<Ast::IExpression> &exp, std::string value) {
    auto ident = dynamic_cast<Ast::Identifier *>(exp.get());
    ASSERT_TRUE(ident != nullptr) << "exp not an identifier";

    ASSERT_EQ(value, ident->m_value)
        << "ident->value not " << value << "got=" << ident->m_value;

    ASSERT_EQ(value, ident->TokenLiteral())
        << "ident->TokenLiteral() not " << value
        << "got=" << ident->TokenLiteral();
}

// Test int expression
void testLiteralExpression(std::unique_ptr<Ast::IExpression> &exp,
                           long int expected) {
    testIntegerLiteral(exp, expected);
}

// Test identifier expression
void testLiteralExpression(std::unique_ptr<Ast::IExpression> &exp,
                           std::string expected) {
    testIdentifier(exp, expected);
}

// test int InfixExpression
void testInfixExpression(std::unique_ptr<Ast::IExpression> &exp, long int left,
                         std::string op, long int right) {
    auto opExp = dynamic_cast<Ast::InfixExpression *>(exp.get());
    ASSERT_TRUE(opExp != nullptr) << "exp is not an InfixExpression";

    testLiteralExpression(opExp->m_left, left);
    testLiteralExpression(opExp->m_right, right);
    ASSERT_EQ(opExp->m_op, op)
        << "exp.Operator is not " << op << ". got=" << opExp->m_op;
}

// test string InfixExpession
void testInfixExpression(std::unique_ptr<Ast::IExpression> &exp,
                         std::string left, std::string op, std::string right) {
    auto opExp = dynamic_cast<Ast::InfixExpression *>(exp.get());
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
        std::variant<int, std::string> expectedValue;
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
        // TODO: Figure out what is going on here
        Ast::IStatement *lStmt = stmt.get();
        Ast::LetStatement *letStmt = dynamic_cast<Ast::LetStatement *>(lStmt);
        testLiteralExpression(letStmt->m_expression, test.expectedValue);
        // test.expectedIdentifier);
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

TEST(Parser, ParsingPrefixExpressions) {
    struct PrefixTest {
        std::string input;
        std::string op;
        long int integerValue;
    };

    std::vector<PrefixTest> tests = {{"!5;", "!", 5}, {"-15;", "-", 15}};

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
        testIntegerLiteral(exp->m_right, test.integerValue);
    }
}

TEST(Parser, ParsingInfixExpressions) {
    struct InfixTest {
        std::string input;
        long int leftValue;
        std::string op;
        long int rightValue;
    };

    std::vector<InfixTest> tests = {
        {"5 + 5;", 5, "+", 5},   {"5 - 5;", 5, "-", 5},   {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},   {"5 > 5;", 5, ">", 5},   {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5}, {"5 != 5;", 5, "!=", 5},
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

        testIntegerLiteral(exp->m_left, test.leftValue);
        ASSERT_EQ(exp->m_op, test.op);
        testIntegerLiteral(exp->m_right, test.leftValue);
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
