#pragma once
#include <initializer_list>

#include "helpers/exception.hpp"
#include "lexicalAnalyzer/lexicalAnalyzer.hpp"
#include "lexicalAnalyzer/lexicalTable.hpp"
#include "parser/node.hpp"
#include "parser/statementNode.hpp"
#include "parser/expressionNode.hpp"


class Parser {
 public:
  void parseProgram();
  ExpressionNodeUptr parseExpression();
  void shiftToken();
  Parser(LexicalAnalyzer& newLexer)
      : programNode(std::make_unique<ChildrenStatementNode>()),
        lexer(newLexer),
        currentToken(*lexer.getToken()) {}

  const ChildrenStatementNode* getProgramNode() const {
    return programNode.get();
  }

  std::string getProgramString();

 private:
  Token getToken();
  StatementNodeUptr parseStatement();
  StatementNodeUptr parseIfStatement();
  StatementNodeUptr parseOtherwiseStatement();
  StatementNodeUptr parseConditionStatement();
  StatementNodeUptr parseLoopStatement();
  StatementNodeUptr parseAsLAsStatement();
  StatementNodeUptr parseFunctionStatement(FunctionStatementNodeUptr root);
  StatementNodeUptr parseCaseStatement();
  StatementNodeUptr parseDefaultStatement();
  StatementNodeUptr parseReturnStatement();
  StatementNodeUptr parseFunCall(NodeUptr root);

  std::vector<ArgumentNodeUptr> parseArguments();
  std::vector<ArgumentNodeUptr> parseFunCallArguments();
  ExpressionNodeUptr parseDefaultArgument();

  StatementNodeUptr parseFunCallOrAssignment();
  StatementNodeUptr parseFunStatOrAssignment();

  ExpressionNodeUptr parseFactor();
  ExpressionNodeUptr parseTerm();
  ExpressionNodeUptr parseAssignment(ExpressionNodeUptr root);
  ExpressionNodeUptr parseAssignExpression(ExpressionNodeUptr root);
  ExpressionNodeUptr parseParenthesesExpression();
  ExpressionNodeUptr parseTestExpression();
  ExpressionNodeUptr parseMultipleTestExpressions();
  ExpressionNodeUptr parseMatrixAssignment(MatrixVariableUptr matrixNode);

  MatrixValueNodeUptr parseMatrixValue();
  MatrixSizeNodeUptr parseMatrixSize();
  
  bool parseEndOfFile();
  bool accept(const Token::TokenType token);
  bool accept(std::initializer_list<Token::TokenType> list);
  bool expect(std::initializer_list<Token::TokenType> list);
  bool expect(const Token::TokenType token);

  ChildrenStatementNodeUptr programNode;
  LexicalAnalyzer lexer;
  Token currentToken;
};