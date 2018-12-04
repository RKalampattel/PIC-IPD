//
//  matrix.h
//  OP2A
//
//  Created by Kim M.K. on 25/08/2016.
//  Last edited by Rahul Kalampattel, March 2018
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef matrix_h
#define matrix_h


#include <vector>
#include <cmath>


template <class T>
T min (const T& a, const T& b)
{
    if (a < b) return a;
    else       return b;
};

template <class T>
T max (const T& a, const T& b)
{
    if (a > b) return a;
    else       return b;
};



class matrix{
    
    // Consructor and Destructor
public:
    matrix();
    matrix(unsigned int i);
    matrix(const unsigned int i, const unsigned int j);
    matrix(const std::vector<std::vector<double> > &data);
  
    ~matrix();
    
    // Basic operators
public:
    double& operator() (const unsigned int i);
    double& operator() (const unsigned int i, const unsigned int j);
    matrix&	operator= (const std::vector< std::vector<double> >& A);
    
    
    
    // Basic functions
public:
    void resize(unsigned int I, unsigned int J);
    unsigned int sizeRow() const;
    unsigned int sizeCol() const;
    double element(const unsigned int i, const unsigned int j) const;
    void removeRow(unsigned int I);
    void removeColumn(unsigned int J);
    void normalize();
    double sum();
    double max();
    double min();
    

    std::vector <std::vector <double> > exportData();
    void importData(std::vector < std::vector <double> >& data);
    
    double* extractToPtr() const;
    void assignFromPtr(double* elements, const unsigned int m, const unsigned int n);
    
    
    // Pre-defined Matrix functions
public:
    void ones();
    void ones(unsigned int i, unsigned int j);
    
    void zeros();
    void zeros(unsigned int i, unsigned int j);
    
    void diag();
    void diag(unsigned int I);
    void diag(unsigned int I, double value);
    
    
    // Linear Algebra functions
public:
    matrix& minorMatrix(const int r, const int c);
    void trans();
    
    // Debugging tools
public:
    void showData();
    
    
    
// DATA set
protected:
    unsigned int m_I;
    unsigned int m_J;
    std::vector < std::vector<double> > m_data;
};


// Operators
matrix operator+ (const matrix& a, const matrix& b);
matrix operator+ (const matrix& a, const double b);
matrix operator+ (const double a, const matrix& b);

matrix operator- (const matrix& a, const matrix& b);
matrix operator- (const matrix& a, const double b);
matrix operator- (const double a, const matrix& b);

matrix operator* (const matrix& a, const matrix& b);
matrix operator* (const matrix& a, const double b);
matrix operator* (const double a, const matrix& b);

matrix operator*= (matrix& a, double b);

// Linear algebra functions
matrix minorMatrix(const matrix&a, const int r, const int c);
double det(const matrix& a);
matrix confactor(const matrix& a);
matrix adjoint(const matrix& a);
matrix inv(const matrix& a);
matrix inv2(const matrix& a);

void block_tri_diagonal_decomp(matrix *A, matrix *B, matrix *C, int NB);
void block_tri_diagonal_solver(matrix *A, matrix *B, matrix *C, matrix *R, matrix *X, int NB);

void rotx(const double ang, matrix& res);
void roty(const double ang, matrix& res);
void rotz(const double ang, matrix& res);
void rot2D(const double ang, matrix& res);
void rotx(const double ang, bool is_degree, matrix& res);
void roty(const double ang, bool is_degree, matrix& res);
void rotz(const double ang, bool is_degree, matrix& res);
void rot2D(const double ang, bool is_degree, matrix& res);

double dot(const matrix& a, const matrix& b);
matrix cross(const matrix& a, const matrix& b);
matrix normalFromThreePoints(const matrix& a, const matrix&b, const matrix&c);

void svdMatrix(matrix& a, matrix& res);
void sumMatrix(matrix& a, matrix& res);
void absMatrix(matrix& a, matrix& res);

double norm(matrix&a, const int flag);
int   matrix_type(const matrix&a);

matrix vectorStartEnd(const matrix& s, const matrix& e);





#endif /* matrix_h */
