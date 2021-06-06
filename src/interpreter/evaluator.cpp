#include "interpreter/evaluator.hpp"

#include "parser/statementNode.hpp"

std::optional<VariableInfo> Evaluator::searchVariable(std::string identifier,
                                                      int currentDepth) const {
  auto iter = variableMap.find(std::make_pair(identifier, currentDepth));
  if (iter != variableMap.end()) {
    return iter->second;
  } else {
    while (currentDepth > 0) {
      currentDepth--;
      iter = variableMap.find(std::make_pair(identifier, currentDepth));
    }
    if (iter != variableMap.end()) return iter->second;
  }
  return {};
}

void Evaluator::updateVariable(std::string identifier, int currentDepth,
                               Value newValue) {
  auto iter = variableMap.find(std::make_pair(identifier, currentDepth));
  if (iter != variableMap.end()) {
    iter->second.value = newValue;
  } else {
    while (currentDepth > 0) {
      currentDepth--;
      iter = variableMap.find(std::make_pair(identifier, currentDepth));
    }
    iter->second.value = newValue;
  }
}

std::optional<const FunctionStatementNode*> Evaluator::searchFunction(
    std::string identifier) const {
  auto iter = functionsMap.find(identifier);
  if (iter != functionsMap.end()) return iter->second;
  return {};
}

void Evaluator::enterBlock() {
  currentDepth++;
  scopeStack.emplace(currentDepth, std::set<std::string>());
}
void Evaluator::closeBlock() {
  std::set<std::string> identifiers = scopeStack.top().second;
  scopeStack.pop();
  for (const auto& identifier : identifiers) {
    variableMap.erase(std::make_pair(identifier, currentDepth));
  }
  currentDepth--;
}

void Evaluator::enterVariable(std::string identifier, std::string value) {
  VariableInfo info = {Type::String, value};
  try {
    scopeStack.top().second.insert(identifier);
  } catch (...) {
    throw SemanticError("Variable " + identifier + "already exists!");
  }
  variableMap.emplace(
      std::make_pair(std::make_pair(identifier, currentDepth), info));
}
void Evaluator::enterVariable(std::string identifier, double value) {
  VariableInfo info = {Type::Double, value};
  try {
    scopeStack.top().second.insert(identifier);
  } catch (...) {
    throw SemanticError("Variable " + identifier + "already exists!");
  }
  variableMap.emplace(
      std::make_pair(std::make_pair(identifier, currentDepth), info));
}
void Evaluator::enterVariable(std::string identifier, Matrix value) {
  VariableInfo info = {Type::Matrix, value};
  try {
    scopeStack.top().second.insert(identifier);
  } catch (...) {
    throw SemanticError("Variable " + identifier + "already exists!");
  }
  variableMap.emplace(
      std::make_pair(std::make_pair(identifier, currentDepth), info));
}
void Evaluator::enterVariable(std::string identifier, int64_t value) {
  VariableInfo info = {Type::Integer, value};
  try {
    scopeStack.top().second.insert(identifier);
  } catch (...) {
    throw SemanticError("Variable " + identifier + "already exists!");
  }
  variableMap.emplace(
      std::make_pair(std::make_pair(identifier, currentDepth), info));
}

void Evaluator::enterFunction(std::string identifier,
                              FunctionStatementNode* node) {
  functionsMap.emplace(std::make_pair(identifier, node));
}

void Evaluator::enterVariable(std::string identifier, TokenVariant value) {
  if (std::holds_alternative<int64_t>(value))
    enterVariable(identifier, std::get<int64_t>(value));
  else if (std::holds_alternative<Matrix>(value))
    enterVariable(identifier, std::get<Matrix>(value));
  else if (std::holds_alternative<double>(value))
    enterVariable(identifier, std::get<double>(value));
  else if (std::holds_alternative<std::string>(value))
    enterVariable(identifier, std::get<std::string>(value));
}

Value Evaluator::evaluate(const ValueNode* node) { return node->getValue(); }

Value Evaluator::evaluate(const IdentifierNode* node) {
  std::string identifier = std::get<std::string>(node->getValue());
  std::optional<VariableInfo> var = searchVariable(identifier, currentDepth);
  if (var) return var->value;
  throw SemanticError("Unknown variable: " + identifier +
                      " at:" + node->getToken().getLinePositionString());
}

Value Evaluator::evaluate(const AdditiveOperatorNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  bool isPlus = (node->getType() == AdditiveOperatorNode::NodeType::Plus);
  Value leftValue = left->accept(*this);
  if (right) {
    Value rightValue = right->accept(*this);
    int signature =
        std::variant_size_v<Value> * leftValue.index() + rightValue.index();
    Matrix returnMatrix;
    switch (static_cast<OperatorSignatures>(signature)) {
      case OperatorSignatures::INTEGER_INTEGER:
        if (isPlus)
          return std::get<int64_t>(leftValue) + std::get<int64_t>(rightValue);
        else
          return std::get<int64_t>(leftValue) - std::get<int64_t>(rightValue);
      case OperatorSignatures::INTEGER_DOUBLE:
        if (isPlus)
          return std::get<int64_t>(leftValue) + std::get<double>(rightValue);
        else
          return std::get<int64_t>(leftValue) - std::get<double>(rightValue);
      case OperatorSignatures::DOUBLE_INTEGER:
        if (isPlus)
          return std::get<double>(leftValue) + std::get<int64_t>(rightValue);
        else
          return std::get<double>(leftValue) - std::get<int64_t>(rightValue);
      case OperatorSignatures::DOUBLE_DOUBLE:
        if (isPlus)
          return std::get<double>(leftValue) + std::get<double>(rightValue);
        else
          return std::get<double>(leftValue) - std::get<double>(rightValue);
      case OperatorSignatures::STRING_STRING:
        if (isPlus)
          return std::get<std::string>(leftValue) +
                 std::get<std::string>(rightValue);
        else
          throw SemanticError("Invalid operation in AdditiveOperator at " +
                              node->getToken().getLinePositionString() +
                              ". Operator-(const std::string&, "
                              "std::string& ) is undefinded.");
      case OperatorSignatures::MATRIX_INTEGER:
        if (isPlus)
          return std::get<Matrix>(leftValue) + std::get<int64_t>(rightValue);
        else
          return std::get<Matrix>(leftValue) - std::get<int64_t>(rightValue);
      case OperatorSignatures::INTEGER_MATRIX:
        if (isPlus)
          return std::get<int64_t>(leftValue) + std::get<Matrix>(rightValue);
        else
          return std::get<int64_t>(leftValue) - std::get<Matrix>(rightValue);
      case OperatorSignatures::MATRIX_DOUBLE:
        if (isPlus)
          return std::get<Matrix>(leftValue) + std::get<double>(rightValue);
        else
          return std::get<Matrix>(leftValue) - std::get<double>(rightValue);
      case OperatorSignatures::DOUBLE_MATRIX:
        if (isPlus)
          return std::get<double>(leftValue) + std::get<Matrix>(rightValue);
        else
          return std::get<double>(leftValue) - std::get<Matrix>(rightValue);
      case OperatorSignatures::MATRIX_MATRIX:
        if (isPlus)
          returnMatrix =
              std::get<Matrix>(leftValue) + std::get<Matrix>(rightValue);
        else
          returnMatrix =
              std::get<Matrix>(leftValue) - std::get<Matrix>(rightValue);
        if (!returnMatrix.empty()) return returnMatrix;
        throw SemanticError("Invalid types in AdditiveOperator at " +
                            node->getToken().getLinePositionString() +
                            ". Matrix's dimensions are not compatible.");
      default:
        throw SemanticError(
            "Wrong types in AdditiveOperator at " +
            node->getToken().getLinePositionString() +
            ". Correct types are matrix:matrix, matrix:double, double:matrix, "
            "matrix:integer, integer:matrix, "
            "string:string, integer:integer, integer:double, double:integer, "
            "double:double.");
    }
  } else {
    if (!isPlus) {
      switch (leftValue.index()) {
        case 0:
          return -(std::get<int64_t>(leftValue));
        case 1:
          return -(std::get<double>(leftValue));
        case 2:
          throw SemanticError("Invalid unary operator in AdditiveOperator at " +
                              node->getToken().getLinePositionString() +
                              ". Operator-(const std::string&) is undefinded.");
        case 3:
          return -(std::get<Matrix>(leftValue));
      }
    }
  }
  return leftValue;
}

Value Evaluator::evaluate(const MultiplicativeOperatorNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  bool isMulti =
      (node->getType() == MultiplicativeOperatorNode::NodeType::Multiplication);
  Value leftValue = left->accept(*this);
  Value rightValue = right->accept(*this);
  if (!isMulti && checkZeroDivision(rightValue))
    throw SemanticError("Division per zero!!! At: " +
                        node->getToken().getLinePositionString());
  int signature =
      std::variant_size_v<Value> * leftValue.index() + rightValue.index();
  Matrix returnMatrix;
  switch (static_cast<OperatorSignatures>(signature)) {
    case OperatorSignatures::INTEGER_INTEGER:
      if (isMulti)
        return std::get<int64_t>(leftValue) * std::get<int64_t>(rightValue);
      else
        return std::get<int64_t>(leftValue) / std::get<int64_t>(rightValue);
    case OperatorSignatures::INTEGER_DOUBLE:
      if (isMulti)
        return std::get<int64_t>(leftValue) * std::get<double>(rightValue);
      else
        return std::get<int64_t>(leftValue) / std::get<double>(rightValue);
    case OperatorSignatures::DOUBLE_INTEGER:
      if (isMulti)
        return std::get<double>(leftValue) * std::get<int64_t>(rightValue);
      else
        return std::get<double>(leftValue) / std::get<int64_t>(rightValue);
    case OperatorSignatures::DOUBLE_DOUBLE:
      if (isMulti)
        return std::get<double>(leftValue) * std::get<double>(rightValue);
      else
        return std::get<double>(leftValue) / std::get<double>(rightValue);
    case OperatorSignatures::MATRIX_INTEGER:
      if (isMulti)
        return std::get<Matrix>(leftValue) * std::get<int64_t>(rightValue);
      else
        return std::get<Matrix>(leftValue) / std::get<int64_t>(rightValue);
    case OperatorSignatures::INTEGER_MATRIX:
      if (isMulti)
        return std::get<int64_t>(leftValue) * std::get<Matrix>(rightValue);
      else
        throw SemanticError("Invalid operation in MultiplicativeOperator at " +
                            node->getToken().getLinePositionString() +
                            ". Integer Matrix division is illegal.");
    case OperatorSignatures::MATRIX_DOUBLE:
      if (isMulti)
        return std::get<Matrix>(leftValue) * std::get<double>(rightValue);
      else
        return std::get<Matrix>(leftValue) / std::get<double>(rightValue);
    case OperatorSignatures::DOUBLE_MATRIX:
      if (isMulti)
        return std::get<double>(leftValue) * std::get<Matrix>(rightValue);
      else
        throw SemanticError("Invalid operation in MultiplicativeOperator at " +
                            node->getToken().getLinePositionString() +
                            ". Double Matrix division is illegal.");
    case OperatorSignatures::MATRIX_MATRIX:
      if (isMulti)
        return std::get<Matrix>(leftValue) * std::get<Matrix>(rightValue);
      else
        return std::get<Matrix>(leftValue) / std::get<Matrix>(rightValue);
    default:
      throw SemanticError("Wrong types in MultiplicativeOperator at " +
                          node->getToken().getLinePositionString() +
                          ". Correct types are matrix:matrix, " +
                          "matrix:double, " + "matrix:integer," +
                          "integer:integer, integer:double, double:integer, " +
                          "double:double.");
  }
}
Value Evaluator::evaluate(const ExponentiationOperatorNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  Value leftValue = left->accept(*this);
  Value rightValue = right->accept(*this);
  int signature =
      std::variant_size_v<Value> * leftValue.index() + rightValue.index();
  Matrix returnMatrix;
  switch (static_cast<OperatorSignatures>(signature)) {
    case OperatorSignatures::INTEGER_INTEGER:
      return std::pow(std::get<int64_t>(leftValue),
                      std::get<int64_t>(rightValue));
    case OperatorSignatures::INTEGER_DOUBLE:
      return std::pow(std::get<int64_t>(leftValue),
                      std::get<double>(rightValue));
    case OperatorSignatures::DOUBLE_INTEGER:
      return std::pow(std::get<double>(leftValue),
                      std::get<int64_t>(rightValue));
    case OperatorSignatures::DOUBLE_DOUBLE:
      return std::pow(std::get<double>(leftValue),
                      std::get<double>(rightValue));
    default:
      throw SemanticError("Wrong types in ExponentiationOperator at " +
                          node->getToken().getLinePositionString() +
                          ". Correct types are" +
                          "integer:integer, integer:double, double:integer, " +
                          "double:double.");
  }
}
Value Evaluator::evaluate(const MatrixOperatorNode* node) {
  auto left = node->getLeft();
  Value leftValue = left->accept(*this);
  bool isDet = node->getType() == MatrixOperatorNode::NodeType::Det;
  bool isInv = node->getType() == MatrixOperatorNode::NodeType::Inverse;
  bool isTrans = node->getType() == MatrixOperatorNode::NodeType::Transpose;

  switch (leftValue.index()) {
    case 3:
      if (isDet) {
        return std::get<Matrix>(leftValue).det();
      } else if (isInv) {
        return std::get<Matrix>(leftValue).inverse();
      } else if (isTrans) {
        return std::get<Matrix>(leftValue).transpose();
      } else {
        throw SemanticError("Invalid MatrixOperator at " +
                            node->getToken().getLinePositionString());
      }
    default:
      throw SemanticError("Wrong type in MatrixOperatorNode at " +
                          node->getToken().getLinePositionString() +
                          ". You can only use this operators with matrices!");
  }
}

Value Evaluator::evaluate(const LogicalOperatorNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  Value leftValue = left->accept(*this);
  if (right) {
    Value rightValue = right->accept(*this);
    int signature =
        std::variant_size_v<Value> * leftValue.index() + rightValue.index();
    Matrix returnMatrix;
    switch (static_cast<OperatorSignatures>(signature)) {
      case OperatorSignatures::INTEGER_INTEGER:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::Greater:
            return std::get<int64_t>(leftValue) > std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::GreaterEqual:
            return std::get<int64_t>(leftValue) >=
                   std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Less:
            return std::get<int64_t>(leftValue) < std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::LessEqual:
            return std::get<int64_t>(leftValue) <=
                   std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<int64_t>(leftValue) !=
                   std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::And:
            return std::get<int64_t>(leftValue) &&
                   std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Or:
            return std::get<int64_t>(leftValue) ||
                   std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<int64_t>(leftValue) ==
                   std::get<int64_t>(rightValue);
          default:
            throw SemanticError("Invalid use of LogicalOperator at " +
                                node->getToken().getLinePositionString() +
                                " Not operator is unary!");
        }
      case OperatorSignatures::DOUBLE_INTEGER:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::Greater:
            return std::get<double>(leftValue) > std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::GreaterEqual:
            return std::get<double>(leftValue) >= std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Less:
            return std::get<double>(leftValue) < std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::LessEqual:
            return std::get<double>(leftValue) <= std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<double>(leftValue) != std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::And:
            return std::get<double>(leftValue) && std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Or:
            return std::get<double>(leftValue) || std::get<int64_t>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<double>(leftValue) == std::get<int64_t>(rightValue);
          default:
            throw SemanticError("Invalid use of LogicalOperator at " +
                                node->getToken().getLinePositionString() +
                                " Not operator is unary!");
        }
      case OperatorSignatures::INTEGER_DOUBLE:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::Greater:
            return std::get<int64_t>(leftValue) > std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::GreaterEqual:
            return std::get<int64_t>(leftValue) >= std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Less:
            return std::get<int64_t>(leftValue) < std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::LessEqual:
            return std::get<int64_t>(leftValue) <= std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<int64_t>(leftValue) != std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::And:
            return std::get<int64_t>(leftValue) && std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Or:
            return std::get<int64_t>(leftValue) || std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<int64_t>(leftValue) == std::get<double>(rightValue);
          default:
            throw SemanticError("Invalid use of LogicalOperator at " +
                                node->getToken().getLinePositionString() +
                                " Not operator is unary!");
        }
      case OperatorSignatures::DOUBLE_DOUBLE:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::Greater:
            return std::get<double>(leftValue) > std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::GreaterEqual:
            return std::get<double>(leftValue) >= std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Less:
            return std::get<double>(leftValue) < std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::LessEqual:
            return std::get<double>(leftValue) <= std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<double>(leftValue) != std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::And:
            return std::get<double>(leftValue) && std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Or:
            return std::get<double>(leftValue) || std::get<double>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<double>(leftValue) == std::get<double>(rightValue);
          default:
            throw SemanticError("Invalid use of LogicalOperator at " +
                                node->getToken().getLinePositionString() +
                                " Not operator is unary!");
        }
      case OperatorSignatures::STRING_STRING:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::Greater:
            return std::get<std::string>(leftValue) >
                   std::get<std::string>(rightValue);
          case LogicalOperatorNode::NodeType::GreaterEqual:
            return std::get<std::string>(leftValue) >=
                   std::get<std::string>(rightValue);
          case LogicalOperatorNode::NodeType::Less:
            return std::get<std::string>(leftValue) <
                   std::get<std::string>(rightValue);
          case LogicalOperatorNode::NodeType::LessEqual:
            return std::get<std::string>(leftValue) >=
                   std::get<std::string>(rightValue);
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<std::string>(leftValue) !=
                   std::get<std::string>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<std::string>(leftValue) ==
                   std::get<std::string>(rightValue);
          default:
            throw SemanticError("Invalid use of LogicalOperator at " +
                                node->getToken().getLinePositionString() +
                                "Available opertaors for string variables are: "
                                "<, <=, >=, >, ==, !=");
        }
      case OperatorSignatures::MATRIX_MATRIX:
        switch (node->getType()) {
          case LogicalOperatorNode::NodeType::NotEqual:
            return std::get<Matrix>(leftValue) != std::get<Matrix>(rightValue);
          case LogicalOperatorNode::NodeType::Equal:
            return std::get<Matrix>(leftValue) == std::get<Matrix>(rightValue);
          default:
            throw SemanticError(
                "Invalid use of LogicalOperator at " +
                node->getToken().getLinePositionString() +
                " Logical operator available for matrices are: !=, ==");
        }
      default:
        throw SemanticError(
            "Wrong types in LogicOperator at " +
            node->getToken().getLinePositionString() +
            ". Correct types are matrix:matrix," +
            "string:string, integer:integer, integer:double, double:integer, "
            "double:double.");
    }
  } else {
    switch (leftValue.index()) {
      case 0:
        if (node->getType() == LogicalOperatorNode::NodeType::Not) {
          return !std::get<int64_t>(leftValue);
        } else
          throw SemanticError("Invalid use of LogicalOperator at " +
                              node->getToken().getLinePositionString() +
                              " Only Not operator is unary!");
      case 1:
        if (node->getType() == LogicalOperatorNode::NodeType::Not) {
          return !std::get<double>(leftValue);
        } else
          throw SemanticError("Invalid use of LogicalOperator at " +
                              node->getToken().getLinePositionString() +
                              "Only Not operator is unary!");
      case 2:
        if (node->getType() == LogicalOperatorNode::NodeType::Not) {
          return std::get<std::string>(leftValue).empty();
        } else
          throw SemanticError("Invalid use of LogicalOperator at " +
                              node->getToken().getLinePositionString() +
                              "Only Not operator is unary!");
      case 3:
        if (node->getType() == LogicalOperatorNode::NodeType::Not) {
          return std::get<Matrix>(leftValue).empty();
        } else
          throw SemanticError("Invalid use of LogicalOperator at " +
                              node->getToken().getLinePositionString() +
                              "Only Not operator is unary!");
      default:
        throw SemanticError(
            "Invalid use of LogicalOperator at " +
            node->getToken().getLinePositionString() +
            "You can use not operator only with double or integer.");
    }
  }
}

Value Evaluator::evaluate(const AssignmentNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  Value leftValue = left->accept(*this);
  Value rightValue = right->accept(*this);
  switch (rightValue.index()) {
    case 0:
      if (left->getToken() == Token::TokenType::IntegerToken) {
        enterVariable(std::get<std::string>(leftValue),
                      std::get<int64_t>(rightValue));
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Type of assign expression is integer, but type "
                            "of variable is not!");
      }
    case 1:
      if (left->getToken() == Token::TokenType::DoubleToken) {
        enterVariable(std::get<std::string>(leftValue),
                      std::get<double>(rightValue));
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Type of assign expression is double, but type "
                            "of variable is not!");
      }
    case 2:
      if (left->getToken() == Token::TokenType::TextToken) {
        enterVariable(std::get<std::string>(leftValue),
                      std::get<std::string>(rightValue));
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Type of assign expression is text, but type "
                            "of variable is not!");
      }
    case 3:
      if (left->getToken() == Token::TokenType::MatrixToken) {
        Matrix valueMatrix = std::get<Matrix>(rightValue);
        Matrix sizeMatrix = std::get<Matrix>(leftValue);
        enterVariable(static_cast<const MatrixVariable*>(left)->getIdentifier(),
                      combineSizeValues(node, sizeMatrix, valueMatrix));
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Type of assign expression is matrix, but type "
                            "of variable is not!");
      }
  }
  return {};
}

Value Evaluator::evaluate(const AssignNewValueNode* node) {
  auto left = node->getLeft();
  auto right = node->getRight();
  Value leftValue = left->accept(*this);
  std::string identifier = std::get<std::string>(leftValue);
  Value rightValue = right->accept(*this);
  std::optional<VariableInfo> info = searchVariable(identifier, currentDepth);
  switch (rightValue.index()) {
    case 0:
      if (info && info->type == Type::Integer) {
        updateVariable(identifier, currentDepth, rightValue);
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Mismatched types or variable doesn't exist.");
      }
    case 1:
      if (info && info->type == Type::Double) {
        updateVariable(identifier, currentDepth, rightValue);
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Mismatched types or variable doesn't exist.");
      }
    case 2:
      if (info && info->type == Type::String) {
        updateVariable(identifier, currentDepth, rightValue);
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Mismatched types or variable doesn't exist.");
      }
    case 3:
      if (info && info->type == Type::Matrix) {
        updateVariable(identifier, currentDepth, rightValue);
        break;
      } else {
        throw SemanticError("Illegal assignment at " +
                            node->getToken().getLinePositionString() +
                            " Mismatched types or variable doesn't exist.");
      }
  }
  return {};
}

Value Evaluator::evaluate(const MatrixSizeNode* node) {
  const std::vector<ExpressionNodeUptr>& values = node->getValues();
  std::vector<int64_t> evaluatedValues;
  try {
    for (const auto& value : values) {
      evaluatedValues.push_back(std::get<int64_t>(value->accept(*this)));
    }
  } catch (...) {
    throw SemanticError("Invalid values in Matrix size at " +
                        node->getToken().getLinePositionString() +
                        " matrix size shoud be of type numeric: integer.");
  }
  return Matrix(evaluatedValues[0], evaluatedValues[1]);
}

Value Evaluator::evaluate(const MatrixValueNode* node) {
  const std::vector<ExpressionNodeUptr>& values = node->getValues();
  std::vector<double> evaluatedValues;
  for (const auto& value : values) {
    Value evalValue = value->accept(*this);
    switch (evalValue.index()) {
      case 0:
        evaluatedValues.push_back(std::get<int64_t>(evalValue));
        break;
      case 1:
        evaluatedValues.push_back(std::get<double>(evalValue));
        break;
      default:
        throw SemanticError(
            "Invalid values in Matrix at " +
            node->getToken().getLinePositionString() +
            " values shoud be of type numeric: integer/double.");
    }
  }

  return Matrix(evaluatedValues);
}

Matrix Evaluator::combineSizeValues(const AssignmentNode* node,
                                    const Matrix& size,
                                    const Matrix& values) const {
  const std::vector<double>& matrixValues = values.getValues();
  std::vector<double> finalValues;
  int64_t rows = size.getRows();
  int64_t columns = size.getColumns();
  int64_t diff = rows * columns - matrixValues.size();
  if (diff < 0) {
    throw SemanticError("More values in matrix than declared at: " +
                        node->getToken().getLinePositionString());
  }
  for (const auto& value : matrixValues) {
    finalValues.push_back(value);
  }
  for (int64_t i = 0; i < diff; i++) {
    finalValues.push_back(0.0);
  }

  return Matrix(rows, columns, finalValues);
}

Value Evaluator::evaluate(const VariableNode* node) {
  return node->getIdentifier();
}
Value Evaluator::evaluate(const MatrixVariable* node) {
  return node->getMatrixSizeNode()->accept(*this);
}
Value Evaluator::evaluate(const IfStatementNode* node) {
  Value conditionValue = node->getIfExpression()->accept(*this);
  const std::vector<StatementNodeUptr>& children = node->getChildren();
  switch (conditionValue.index()) {
    case 0:
      if (std::get<int64_t>(conditionValue) != 0) {
        enterBlock();
        for (const auto& child : children) {
          child->accept(*this);
        }
        closeBlock();
      } else {
        const StatementNode* otherwiseNode = node->getOtherwiseExpression();
        if (otherwiseNode) otherwiseNode->accept(*this);
      }
      break;
    default:
      throw SemanticError(
          "Condition expression in if statement is not evaluable to bool at:" +
          node->getToken().getLinePositionString());
      break;
  }
  return {};
}

Value Evaluator::evaluate(const OtherwiseStatementNode* node) {
  const std::vector<StatementNodeUptr>& children = node->getChildren();
  enterBlock();
  for (const auto& child : children) {
    child->accept(*this);
  }
  closeBlock();
  return {};
}

Value Evaluator::evaluate(const ArgumentNode* node) {
  throw SemanticError("ArgumentNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}

Value Evaluator::evaluate(const LoopStatementNode* node) {
  throw SemanticError("LoopStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const AslasStatementNode* node) {
  throw SemanticError("AslasStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const FunctionStatementNode* node) {
  throw SemanticError("FunctionStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const FunctionCallNode* node) {
  throw SemanticError("FunctionCallNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const ConditionStatementNode* node) {
  throw SemanticError("ConditionStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const CaseStatementNode* node) {
  throw SemanticError("CaseStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}
Value Evaluator::evaluate(const DefaultStatementNode* node) {
  throw SemanticError("DefaultStatementNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}

Value Evaluator::evaluate(const ExpressionNode* node) {
  throw SemanticError("ExpressionNode Not implemented! At: " +
                      node->getToken().getLinePositionString());
}

bool Evaluator::checkZeroDivision(const Value value) const {
  switch (value.index()) {
    case 0:
      if (std::get<int64_t>(value) == 0) return true;
      return false;
    case 1:
      if (std::get<double>(value) == 0) return true;
      return false;
    default:
      return false;
  }
}