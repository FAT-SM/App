#pragma once

#include <vector>

/* Represents a matrix (i.e., a 2D array of numeric values). */
template<typename T>
class Matrix {

private:
    int _size;
    int _rowCount;
    int _columnCount;
    std::vector<T> _data;

public:

    /* Creates an empty matrix. */
    Matrix();

    /* Creates a m-by-n matrix of initial or default values. */
    Matrix(int m, int n, const T& x = T());

    /* Copy constructor. */
    Matrix(const Matrix& other);

    /* Copy assignment operator. */
    Matrix& operator=(const Matrix& other);

    /* Equality operator. */
    bool operator==(const Matrix& other) const;

    /* Access to the k-th matrix value (row-major indexing). */
    T& at(int k);
    const T& at(int k) const;

    /* Access to the matrix value at the i-th row and j-th column. */
    T& at(int i, int j);
    const T& at(int i, int j) const;

    /* The number of rows. */
    int rowCount() const;

    /* The number of columns. */
    int columnCount() const;

};
