#include <gtest/gtest.h>

#include "helpers/exception.hpp"
#include "interpreter/evaluator.hpp"
#include "parser/parser.hpp"

TEST(EvaluatorTest, EvaluateVariableTest) {
  try {
    Interpreter interpret;
    IdentifierNode node(Token(Token::TokenType::IdentifierToken, "zmienna"));
    interpret.getEvaluator().evaluate(&node);
    FAIL() << "Expected throw!";
  } catch (const SemanticError& err) {
    EXPECT_EQ(err.what(), std::string("Unknown variable: zmienna at:1:1"));
  } catch (...) {
    FAIL() << "Invalid throw!";
  }
}

TEST(EvaluatorTest, EvaluateVariableTest2) {
  Interpreter interpret;
  interpret.enterVariable("zmienna", TokenVariant(4));
  EXPECT_EQ(interpret.getVariableMap().size(), 1);
  IdentifierNode node(Token(Token::TokenType::IdentifierToken, "zmienna"));
  EXPECT_EQ(std::get<int64_t>(interpret.getEvaluator().evaluate(&node)), 4);
}

TEST(EvaluatorTest, EvaluateVariableTest3) {
  Interpreter interpret;
  ValueNode node(Token(Token::TokenType::IntegerLiteralToken, 1));
  EXPECT_EQ(std::get<int64_t>(interpret.getEvaluator().evaluate(&node)), 1);
}

TEST(EvaluatorTest, EvaluateVariableTest4) {
  Interpreter interpret;
  ValueNode node(Token(Token::TokenType::StringLiteralToken, "zmienna"));
  EXPECT_EQ(std::get<std::string>(interpret.getEvaluator().evaluate(&node)),
            "zmienna");
}

TEST(EvaluatorTest, EvaluateVariableTest5) {
  Interpreter interpret;
  ValueNode node(Token(Token::TokenType::DoubleLiteralToken, 3.14));
  EXPECT_EQ(std::get<double>(interpret.getEvaluator().evaluate(&node)), 3.14);
}

TEST(EvaluatorTest, INT_INT_Test) {
  std::string test = R"(2+2)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  Interpreter interpret;
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<int64_t>(node->accept(interpret.getEvaluator())), 4);
}

TEST(EvaluatorTest, DOUBLE_DOUBLE_TEST) {
  std::string test = R"(3.0+0.14)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  Interpreter interpret;
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<double>(node->accept(interpret.getEvaluator())), 3.14);
}

TEST(EvaluatorTest, INT_DOUBLE_TEST) {
  std::string test = R"(3 +0.14)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  Interpreter interpret;
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<double>(node->accept(interpret.getEvaluator())), 3.14);
}

TEST(EvaluatorTest, DOUBLE_INT_TEST) {
  std::string test = R"(3.14 + 1)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  Interpreter interpret;
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<double>(node->accept(interpret.getEvaluator())), 3.14 + 1);
}

TEST(EvaluatorTest, STRING_STRING_TEST) {
  std::string test = R"("mama" + "tata")";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  Interpreter interpret;
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<std::string>(node->accept(interpret.getEvaluator())),
            std::string("mamatata"));
}

TEST(EvaluatorTest, STRING_STRING_TEST2) {
  try {
    std::string test = R"("mama" - "tata")";
    StringSource src(test);
    LexicalAnalyzer lexicAna(&src);
    Parser parser(lexicAna);
    Interpreter interpret;
    ExpressionNodeUptr node = parser.parseExpression();
    node->accept(interpret.getEvaluator());
  } catch (const SemanticError& err) {
    SUCCEED();
  } catch (...) {
    FAIL();
  }
}

TEST(EvaluatorTest, MATRIX_INT_TEST) {
  Interpreter interpret;
  std::vector<double> values = {1, 1, 1, 1};
  std::vector<double> values2 = {2, 2, 2, 2};
  Matrix resultMatrix(2, 2, values2);
  Matrix matrix(2, 2, values);
  interpret.enterVariable("zmienna", matrix);
  std::string test = R"(zmienna + 1)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<Matrix>(node->accept(interpret.getEvaluator())),
            resultMatrix);
}

TEST(EvaluatorTest, INT_MATRIX_TEST) {
  Interpreter interpret;
  std::vector<double> values = {1, 1, 1, 1};
  std::vector<double> values2 = {2, 2, 2, 2};
  Matrix resultMatrix(2, 2, values2);
  Matrix matrix(2, 2, values);
  interpret.enterVariable("zmienna", matrix);
  std::string test = R"(1+zmienna)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<Matrix>(node->accept(interpret.getEvaluator())),
            resultMatrix);
}

TEST(EvaluatorTest, DOUBLE_MATRIX_TEST) {
  Interpreter interpret;
  std::vector<double> values = {1, 1, 1, 1};
  std::vector<double> values2 = {2.5, 2.5, 2.5, 2.5};
  Matrix resultMatrix(2, 2, values2);
  Matrix matrix(2, 2, values);
  interpret.enterVariable("zmienna", matrix);
  std::string test = R"(1.5+zmienna)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<Matrix>(node->accept(interpret.getEvaluator())),
            resultMatrix);
}

TEST(EvaluatorTest, MATRIX_DOUBLE_TEST) {
  Interpreter interpret;
  std::vector<double> values = {1, 1, 1, 1};
  std::vector<double> values2 = {2.5, 2.5, 2.5, 2.5};
  Matrix resultMatrix(2, 2, values2);
  Matrix matrix(2, 2, values);
  interpret.enterVariable("zmienna", matrix);
  std::string test = R"(zmienna+1.5)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<Matrix>(node->accept(interpret.getEvaluator())),
            resultMatrix);
}

TEST(EvaluatorTest, MATRIX_MATRIX_TEST) {
  Interpreter interpret;
  std::vector<double> values = {1, 1, 1, 1};
  std::vector<double> values2 = {2, 2, 2, 2};
  Matrix resultMatrix(2, 2, values2);
  Matrix matrix(2, 2, values);
  Matrix matrix2(2, 2, values);

  interpret.enterVariable("matrix1", matrix);
  interpret.enterVariable("matrix2", matrix2);
  std::string test = R"(matrix1 + matrix2)";
  StringSource src(test);
  LexicalAnalyzer lexicAna(&src);
  Parser parser(lexicAna);
  ExpressionNodeUptr node = parser.parseExpression();
  EXPECT_EQ(std::get<Matrix>(node->accept(interpret.getEvaluator())), values2);
}