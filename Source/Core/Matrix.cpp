#include <string>
#include <QString>
#include <stdexcept>
#include "Core/Matrix.hpp"

template<typename T>
Matrix<T>::Matrix() : _size(0), _rowCount(0), _columnCount(0), _data(0) {}

template<typename T>
Matrix<T>::Matrix(int m, int n, const T& x) : _size(m*n), _rowCount(m), _columnCount(n), _data(_size, x) {}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& other) :
    _size(other._size), _rowCount(other._rowCount), _columnCount(other._columnCount), _data(other._data) {}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
    if (this != &other) {
        _size = other._size;
        _rowCount = other._rowCount;
        _columnCount = other._columnCount;
        _data = other._data;
    }
    return *this;
}

template<typename T>
bool Matrix<T>::operator==(const Matrix<T>& other) const {
    return _rowCount == other._rowCount && _columnCount == other._columnCount && _data == other._data;
}

template<typename T>
T& Matrix<T>::at(int k) {
    if (k < 0 || k >= _size) throw std::out_of_range("Index out of range.");
    return _data[k];
}

template<typename T>
const T& Matrix<T>::at(int k) const {
    if (k < 0 || k >= _size) throw std::out_of_range("Index out of range.");
    return _data[k];
}

template<typename T>
T& Matrix<T>::at(int i, int j) {
    if (i < 0 || i >= _rowCount) throw std::out_of_range("Row index out of range.");
    if (j < 0 || j >= _columnCount) throw std::out_of_range("Column index out of range.");
    return _data[i*_columnCount + j];
}

template<typename T>
const T& Matrix<T>::at(int i, int j) const {
    if (i < 0 || i >= _rowCount) throw std::out_of_range("Row index out of range.");
    if (j < 0 || j >= _columnCount) throw std::out_of_range("Column index out of range.");
    return _data[i*_columnCount + j];
}

template<typename T>
int Matrix<T>::rowCount() const { return _rowCount; }

template<typename T>
int Matrix<T>::columnCount() const { return _columnCount; }

template class Matrix<int>;
template class Matrix<double>;
template class Matrix<QString>;
template class Matrix<std::string>;
