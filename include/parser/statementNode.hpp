#pragma once
#include "parser/expressionNode.hpp"
#include "parser/node.hpp"

class ChildrenStatementNode;
class FunctionStatementNode;
class AslasStatementNode;
class IfStatementNode;
class OtherwiseStatementNode;
class FunctionCallNode;

using StatementNodeUptr = std::unique_ptr<StatementNode>;
using ChildrenStatementNodeUptr = std::unique_ptr<ChildrenStatementNode>;
using FunctionStatementNodeUptr = std::unique_ptr<FunctionStatementNode>;
using AslasStatementNodeUptr = std::unique_ptr<AslasStatementNode>;
using IfStatementNodeUptr = std::unique_ptr<IfStatementNode>;
using OtherwiseStatementNodeUptr = std::unique_ptr<OtherwiseStatementNode>;
using FunctionCallNodeUptr = std::unique_ptr<FunctionCallNode>;

class ChildrenStatementNode : public StatementNode {
 public:
  ChildrenStatementNode(Token token) : StatementNode(token){};
  ChildrenStatementNode() : StatementNode(Token(Token::TokenType::RootToken)){};

  void add(StatementNodeUptr newNode);
  void add(std::vector<StatementNodeUptr>& newNodes);
  const std::vector<StatementNodeUptr>& getChildren() const { return children; }
  void buildTreeStringStream(int64_t depth,
                             std::stringstream& tree) const override;
  void remove(const StatementNode* node);

 protected:
  std::vector<StatementNodeUptr> children;
};

class IfStatementNode : public ChildrenStatementNode {
 public:
  IfStatementNode(Token token) : ChildrenStatementNode(token){};
  void setIfExpression(ExpressionNodeUptr expression) {
    this->ifExpression = std::move(expression);
  }
  void buildTreeStringStream(int64_t depth,
                             std::stringstream& tree) const override;

 private:
  ExpressionNodeUptr ifExpression;
};

class OtherwiseStatementNode : public ChildrenStatementNode {
 public:
  OtherwiseStatementNode(Token token) : ChildrenStatementNode(token){};

 private:
};

class LoopStatementNode : public ChildrenStatementNode {
 public:
  LoopStatementNode(Token token) : ChildrenStatementNode(token) {}

  void setStep(TokenVariant newStep);
  void setEnd(TokenVariant newEnd);
  void setStart(TokenVariant newStart);

 private:
  std::variant<std::string, int64_t> start = 0;
  std::variant<std::string, int64_t> end = 0;
  std::variant<std::string, int64_t> step = 1;
};

class AslasStatementNode : public ChildrenStatementNode {
 public:
  AslasStatementNode(Token token) : ChildrenStatementNode(token){};
  void setAsLAsExpression(ExpressionNodeUptr expression) {
    this->aslasExpression = std::move(expression);
  }
  void buildTreeStringStream(int64_t depth,
                             std::stringstream& tree) const override;

 private:
  ExpressionNodeUptr aslasExpression;
};

class FunctionStatementNode : public ChildrenStatementNode {
 public:
  FunctionStatementNode(Token functionToken, Token returnTypeToken)
      : ChildrenStatementNode(functionToken), returnType(returnTypeToken){};

  void setReturnType(Token& returnType) { this->returnType = returnType; }
  void setIdentifier(std::string identifier) { this->identifier = identifier; }
  void setArgumentsNodes(std::vector<ArgumentNodeUptr>& arguments) {
    std::ranges::move(arguments, std::back_inserter(this->arguments));
  }
  void buildTreeStringStream(int64_t depth,
                             std::stringstream& tree) const override;

 protected:
  Token returnType;
  std::string identifier;
  std::vector<ArgumentNodeUptr> arguments;
};

class FunctionCallNode : public ChildrenStatementNode {
 public:
  FunctionCallNode(Token token) : ChildrenStatementNode(token){};
  void setIdentifier(std::string identifier) { this->identifier = identifier; }
  void setArgumentsNodes(std::vector<ExpressionNodeUptr>& arguments) {
    std::ranges::move(arguments, std::back_inserter(this->arguments));
  }
  void buildTreeStringStream(int64_t depth,
                             std::stringstream& tree) const override;

 private:
  std::string identifier;
  std::vector<ExpressionNodeUptr> arguments;
};