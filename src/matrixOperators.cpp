#include "matrix.hpp"

Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
  if (lhs.rows == rhs.rows && lhs.columns == rhs.columns) {
    Matrix resultMatrix(lhs.rows, lhs.columns);
    for (size_t i = 0; i < lhs.values.size(); i++) {
      resultMatrix.values[i] = lhs.values[i] + rhs.values[i];
    }
    return resultMatrix;
  }
  return Matrix();
}

Matrix operator+(const Matrix& lhs, const int number) {
  Matrix returnMatrix(lhs.rows, lhs.columns);
  for (size_t i = 0; i < lhs.values.size(); i++) {
    returnMatrix.values[i] = lhs.values[i] + number;
  }
  return returnMatrix;
}
Matrix operator+(const int number, const Matrix& lhs) {
  return operator+(lhs, number);
}

Matrix operator+(const Matrix& lhs, double number) {
  Matrix returnMatrix(lhs.rows, lhs.columns);
  for (size_t i = 0; i < lhs.values.size(); i++) {
    returnMatrix.values[i] = lhs.values[i] + number;
  }
  return returnMatrix;
}

Matrix operator+(double number, const Matrix& lhs) {
  return operator+(lhs, number);
}