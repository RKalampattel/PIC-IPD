//
//  matrix.cpp
//  OP2A
//
//  Created by Kim M.K. on 25/08/2016.
//  Last edited by Rahul Kalampattel, March 2018
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <omp.h>
#include "matrix.hpp"
#include "../COMM/error_check.hpp"
#include "../COMM/MultiVector.hpp"
#include "mkl.h"


// Constructor and Desctructor
matrix::matrix()
:m_I(0), m_J(0)
{
    
}

matrix::matrix(const unsigned int i)
:m_I(i), m_J(1), m_data(i, std::vector<double>(1, 0.0))
{
    
}

matrix::matrix(const unsigned int i, const unsigned int j)
:m_I(i), m_J(j), m_data(i, std::vector<double>(j, 0.0))
{
    
}


matrix::matrix(const vector2D& data)
{
    m_I = data.size();
    m_J = data[0].size();
    m_data = data;
}


matrix::~matrix()
{
    
}



// Operators
double& matrix::operator() (const unsigned int i)
{
    if (m_J == 1)
    {
        return(operator()(i,0));
    }
    
    if (m_I == 1)
    {
        return (operator()(0,i));
    }
    
    
    Common::ExceptionError(FromHere(), "Need two indeces for matrix", Common::ErrorCodes::ExceedLimit());
    return (operator()(0,0));
}



double& matrix::operator() (const unsigned int i, const unsigned int j)
{
    if (i >= m_I)
    {
        Common::ExceptionError(FromHere(), "Exceed the row size of a Matrix", Common::ErrorCodes::ExceedLimit());
    }
    
    if (j >= m_J)
    {
        Common::ExceptionError(FromHere(), "Exceed the column size of a Matrix", Common::ErrorCodes::ExceedLimit());
    }
        
    return (m_data[i][j]);
}

matrix& matrix::operator= (const std::vector< std::vector<double> >& A)
{
    if (m_I !=  A.size() || m_J != A[0].size())	resize(A.size(), A[0].size());
    
    m_data	= A;
    return *this;
}


// Basic Functions
void matrix::resize(unsigned int i, unsigned int j)
{
    m_I = i;
    m_J = j;
    
    m_data.resize(i);
    for (int ii = 0; ii <= m_I; ii++) m_data[ii].resize(j);
}

unsigned int matrix::sizeRow() const
{
    return (m_I);
}

unsigned int matrix::sizeCol() const
{
    return (m_J);
}


double matrix::element(const unsigned int i, const unsigned int j) const
{
    return (m_data[i][j]);
}


void matrix::removeRow(unsigned int I)
{
    if (I >= m_I)	Common::ExceptionError(FromHere(), "Exceed the row size of a Matrix", Common::ErrorCodes::ExceedLimit());
    
    int inew = 0;
    std::vector <std::vector <double> >  data_temp(m_I-1, std::vector<double>(m_J)) ;
    
    for (int i = 0; i <= m_I-1; i++)
    {
        if (i != I)
        {
            if (i < I)	inew	 = i;
            if (i > I)	inew	 = i - 1;
            
            for (int j = 0; j <= m_J-1; j++)
            {
                data_temp[inew][j] = m_data[i][j];
            }
        }
    }
    
    resize(m_I-1, m_J);
    m_data = data_temp;
}


void matrix::removeColumn(unsigned int J)
{
    if (J >= m_J) Common::ExceptionError(FromHere(), "Exceed the column size of a Matrix", Common::ErrorCodes::ExceedLimit());
    
    int jnew = 0;
    std::vector <std::vector <double> >  data_temp(m_I, std::vector<double>(m_J-1)) ;
    
    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            if (j != J)
            {
                if (j < J)	jnew	 = j;
                if (j > J)	jnew	 = j - 1;
                
                data_temp[i][jnew] = m_data[i][j];
            }
        }
    }
    
    resize(m_I, m_J-1);
    m_data = data_temp;
}


void matrix::normalize()
{
    double res = norm(*this, 2);
    
    for (int i = 0; i < m_I; i++)
        for (int j = 0; j < m_J; j++)
            m_data[i][j] = m_data[i][j] / res;
}


double matrix::sum()
{
    std::vector<double> o_sumMP(m_I, 0.0);
    
#pragma omp parallel for
    for (int i = 0; i <= m_I-1; i++)
    {
        double sum_temp = 0.0;
        for (int j = 0; j <= m_J-1; j++)
        {
            sum_temp += m_data[i][j];
        }
        
        o_sumMP[i] = sum_temp;
    }
    
    double o_sum = 0.0;
#pragma omp parallel for reduction(+: o_sum)
    for (int i = 0; i <= m_I-1; i++)
    {
        o_sum += o_sumMP[i];
    }
    
    return (o_sum);
}



double matrix::max()
{
    std::vector<double> o_maxMP(m_I, 0.0);
    
#pragma omp parallel for shared (o_maxMP)
    for (int ii = 0; ii < m_I; ii++)
    {
        o_maxMP[ii] = m_data[ii][0];
        for (int jj = 1; jj < m_J; jj++)
        {
            if (m_data[ii][jj] > o_maxMP[ii]) o_maxMP[ii] = m_data[ii][jj];
        }
    }
    
#pragma omp barrier
    double res = o_maxMP[0];
    for (int ii = 1; ii < m_I; ii++)
    {
        if (o_maxMP[ii] > res)  res = o_maxMP[ii];
    }
    
    return res;
}


double matrix::min()
{
    std::vector<double> o_minMP(m_I, 0.0);
    
#pragma omp parallel for
    for (int ii = 0; ii < m_I; ii++)
    {
        o_minMP[ii] = m_data[ii][0];
        for (int jj = 1; jj < m_J; jj++)
        {
            if (m_data[ii][jj] < o_minMP[ii]) o_minMP[ii] = m_data[ii][jj];
        }
    }
    
    double res = o_minMP[0];
    for (int ii = 1; ii < m_I; ii++)
    {
        if (o_minMP[ii] < res)  res = o_minMP[ii];
    }
    
    return (res);
}


std::vector < std::vector <double> > matrix::exportData()
{
    return(m_data);
}

void matrix::importData(std::vector < std::vector <double> >& data)
{
    m_data = data;
    m_I = data.size();
    m_J = data[0].size();
}


double* matrix::extractToPtr() const
{
    //std::vector<double> res(m_I * m_J);
    double* res = new double [m_I*m_J];
    
    int k = 0;
    for (int i = 0; i < m_I; i++)
    {
        for (int j = 0; j < m_J; j++)
        {
            res[k] = m_data[i][j];
            k++;
        }
    }
    
    return res;
}

void matrix::assignFromPtr(double* elements, const unsigned int m, const unsigned int n)
{
    resize(m, n);
    
    int k = 0;
    for (int i = 0; i < m_I; i++)
    {
        for (int j = 0; j < m_J; j++)
        {
            m_data[i][j] = elements[k];
            k++;
        }
    }
}




// Pre-defined matrix
void matrix::ones()
{
    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            m_data[i][j] = 1.0;
        }
    }
}

void matrix::ones(unsigned int i, unsigned int j)
{
    resize(i, j);
    ones();
}

void matrix::zeros()
{
    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            m_data[i][j] = 0.0;
        }
    }
}

void matrix::zeros(unsigned int i, unsigned int j)
{
    resize(i, j);
    zeros();
}

void matrix::diag()
{
    if (m_I != m_J)
        Common::ExceptionError(FromHere(), "You need a square matrix to create a diagonal matrix", Common::ErrorCodes::ExceedLimit());
    

    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            m_data[i][j] = 0.0;
        }
        
        m_data[i][i] = 1.0;
    }
}

void matrix::diag(unsigned int I)
{
    resize(I, I);
    
    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            m_data[i][j] = 0.0;
        }
        m_data[i][i] = 1.0;
    }
}

void matrix::diag(unsigned int I, double value)
{
    resize(I, I);

    for (int i = 0; i <= m_I-1; i++)
    {
        for (int j = 0; j <= m_J-1; j++)
        {
            m_data[i][j] = 0.0;
        }
        m_data[i][i] = value;
    }
}


// Linear Algebra functions
matrix& matrix::minorMatrix(const int r, const int c)
{
    removeRow(r);
    removeColumn(c);
    
    return *this;
}


void matrix::trans()
{
    std::vector< std::vector<double> > data_temp(m_J, std::vector<double>(m_I, 0.0));
    
    for (int ii = 0; ii < m_I ; ii++)
    {
        for (int jj = 0; jj < m_J; jj++)
        {
            data_temp[jj][ii] = m_data[ii][jj];
        }
    }
    
    
    m_data = data_temp;
    
    int tempI = m_I;
    m_I = m_J;
    m_J = tempI;
}



// Debuggin tools
void matrix::showData()
{
    std::cout << "Row size    : " << m_I << std::endl;
    std::cout << "Coloumn size: " << m_J << std::endl;
    std::cout << "Matrix data:"   << std::endl;
    
    for (int i= 0; i < m_data.size(); i++)
    {
        for (int j = 0; j < m_data[i].size(); j++)
        {
            std::cout << m_data[i][j] << "  ";
        }
        std::cout << std::endl;
    }
    std::cout<<std::endl;
}



//Operators
matrix operator +(const matrix& a, const matrix& b)
{
    int i, j;
    matrix res;
    
    if (a.sizeRow() == b.sizeRow() && a.sizeCol() == b.sizeCol())
    {
        res = a;
        for (i = 0; i < a.sizeRow(); i++)
        {
            for (j = 0;  j < a.sizeCol(); j++)
            {
                res(i,j) = res(i,j) + b.element(i,j);
            }
        }
    }
    else
    {
        Common::ExceptionError(FromHere(), "Dimensions do not match", Common::ErrorCodes::MismatchDimension());
    }
    
    return res;
}

matrix operator +(const matrix& a, const double b)
{
    int i, j;
    matrix res = a;
    
    for (i = 0; i < a.sizeRow(); i++)
    {
        for (j = 0;  j < a.sizeCol(); j++)
        {
            res(i,j) = res(i,j) + b;
        }
    }
    
    return res;
}

matrix operator+ (const double a, const matrix& b)
{
    int i, j;
    matrix res = b;
    
    for (i = 0; i < b.sizeRow(); i++)
    {
        for (j = 0;  j < b.sizeCol(); j++)
        {
            res(i,j) = res(i,j) + a;
        }
    }
    
    return res;
}


matrix operator -(const matrix& a, const matrix& b)
{
    int i, j;
    matrix res;
    
    if (a.sizeRow() == b.sizeRow() && a.sizeCol() == b.sizeCol())
    {
        res = a;
        for (i = 0; i < a.sizeRow(); i++)
        {
            for (j = 0;  j < a.sizeCol(); j++)
            {
                res(i,j) = res(i,j) - b.element(i,j);
            }
        }
    }
    else
    {
        Common::ExceptionError(FromHere(), "Dimensions do not match", Common::ErrorCodes::MismatchDimension());
    }
    
    return res;
}


matrix operator -(const matrix& a, const double b)
{
    int i, j;
    matrix res = a;
    
    for (i = 0; i < a.sizeRow(); i++)
    {
        for (j = 0;  j < a.sizeCol(); j++)
        {
            res(i,j) = res(i,j) - b;
        }
    }
    
    return res;
}

matrix operator- (const double a, const matrix& b)
{
    int i, j;
    matrix res = b;
    
    for (i = 0; i < b.sizeRow(); i++)
    {
        for (j = 0;  j < b.sizeCol(); j++)
        {
            res(i,j) = a - res(i,j);
        }
    }
    
    return res;
}



#ifdef USE_MKL
matrix operator* (const matrix& a, const matrix& b)
{
    int m, n, k;
    double alpha, beta;
    
    alpha	= 1.0;
    beta	= 0.0;
    
    double* res_elements;
    double* a_elements;
    double* b_elements;
    
    a_elements = a.extractToPtr();
    b_elements = b.extractToPtr();
    
    if (a.sizeCol() == b.sizeRow())
    {
        m	= a.sizeRow();
        k	= a.sizeCol();
        n	= b.sizeCol();
        
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, a_elements, k, b_elements, n, beta, res_elements, n);
    }
    else
    {
        Common::ExceptionError(FromHere(), "Dimensions do not match", Common::ErrorCodes::MismatchDimension());
    }
    
    matrix res;
    res.assignFromPtr(res_elements, m, n);
    return res;
}

#else
matrix operator* (const matrix& a, const matrix& b)
{
    int m, n, k;
    matrix res;

    if (a.sizeCol() == b.sizeRow())
    {
        m	= a.sizeRow();
        k	= a.sizeCol();
        n	= b.sizeCol();
        
        res.resize(m, n);
        
//#pragma omp parallel for
        for (int i = 0; i < m; i++)
        {
//#pragma ivdep
        for (int j = 0; j < n; j++)
            {
                double temp1 = 0.0;
                for (int ii = 0; ii < k;ii++) temp1 += a.element(i,ii)*b.element(ii, j);
                
                res(i,j) = temp1;
            }
        }
        
    }
    else
    {
        Common::ExceptionError(FromHere(), "Dimensions do not match", Common::ErrorCodes::MismatchDimension());
    }
    
    return res;
}
#endif


matrix operator* (const matrix& a, const double b)
{
    matrix res = a;
    int m = res.sizeRow();
    int n = res.sizeCol();
    for (int i = 0; i < m; i++)
    {
        for (int  j= 0; j < n; j++)
        {
            res(i,j) = res.element(i,j) * b;
        }
    }
    
    return res;
}


matrix operator* (const double a, const matrix& b)
{
    matrix res = b;
    int m = res.sizeRow();
    int n = res.sizeCol();
    
    for (int i = 0; i < m; i++)
    {
        for (int  j= 0; j < n; j++)
        {
            res(i,j) = a * res.element(i,j);
        }
    }
    
    return res;
}


matrix operator*= (matrix& a, double b)
{
	int m = a.sizeRow();
	int n = a.sizeCol();
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a(i, j) = a.element(i, j) * b;
		}
	}

	return a;
}


// Linear algebra functions
matrix  minorMatrix(const matrix&a, const int r, const int c)
{
    matrix C = a;
    C.minorMatrix(r, c);
    return (C);
}




double det(const matrix& a)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    
    if (m != n) Common::ExceptionError(FromHere(), "Square matrix required", Common::ErrorCodes::MismatchDimension());

    
    double res = 0.0;
    
    if (m == 1)
    {
        res = a.element(0,0);
    }
    else if (m == 2)
    {
        res = a.element(0,0)*a.element(1,1) - a.element(0,1)*a.element(1,0);
    }
    else if (m <= 5)
    {
        for (int c = 0; c < n; c++)
        {
            matrix tempM = minorMatrix(a, 0, c);

            double temp1 = pow(-1.0, c);
            double temp2 = det(tempM);
            
            res += temp1 * a.element(0, c) * temp2;  // faster than with pow()
        }
    }
    else
    {
#pragma omp parallel for reduction(+: res)
        for (int i = 0; i < m-1; i++)
        {
            matrix tempMatrix = minorMatrix(a, 0, i);
    
            double temp1 = pow(-1.0, i);
            double temp2 = det(tempMatrix);
            
            res += temp1*a.element(0, i) * temp2;
        }
    }

    Common::ErrorCheck::NumericValue(FromHere(), "Determinant of a matric:", res);
    return res;
}



matrix confactor(const matrix& a)
{
    int m = a.sizeRow();
    
    if (m != a.sizeCol())
        Common::ExceptionError(FromHere(), "Matrix should be a square matrix to calculate confactor matrix", Common::ErrorCodes::ExceedLimit());
    
    if (m == 0)
        Common::ExceptionError(FromHere(), "Input matrix has Zero-dimension", Common::ErrorCodes::ExceedLimit());
    
    matrix res(m, m);
    
#pragma omp parallel
    {
        for (int i = 0; i < m; i++)
        {
#pragma ivdep
            for (int j = 0; j < m; j++)
            {
                matrix a_minor = minorMatrix(a, i, j);
                double d = det(a_minor);
                
                res(i,j) = pow(-1.0, i+j) * d;
            }
        }
    }
    
    return res;
}


matrix adjoint(const matrix& a)
{
    int m = a.sizeRow();

    matrix res(m , m);
    matrix ap = confactor(a);
    
#pragma omp parallel
    {
        for (int i = 0; i < m; i++)
        {
#pragma ivdep
            for (int j = 0; j < m; j++)
            {
                res(i, j) = ap(j, i);
            }
        }
    }
    
    return res;
}


matrix inv(const matrix &a)
{
    int m = a.sizeRow();
    
    matrix res(m, m);
    double 	d = det(a);
    
    
    if (m == 1)		// 1 X 1 MATRIX
    {
        res(0,0) = 1/d;
    }
    else if (m == 2)	// 2 X 2 MATRIX
    {
        res(0,0) =  a.element(1,1) / d;
        res(0,1) = -a.element(0,1) / d;
        
        res(1,0) = -a.element(1,0) / d;
        res(1,1) =  a.element(0,0) / d;
    }
    else if (m <= 5)
    {
        // IT USES ADJOINT METHOD
        matrix ai = adjoint(a);
        
        for (int i = 0; i <= m-1; i++)
        {
            for (int j = 0; j <= m-1; j++)
            {
                res(i,j)	= ai(i,j) / d;
            }
        }
    }
    else
    {
        // IT USES ADJOINT METHOD
        matrix ai = adjoint(a);
        
#pragma omp parallel
        {
            for (int i = 0; i <= m-1; i++)
            {
#pragma ivdep
                for (int j = 0; j <= m-1; j++)
                {
                    res(i,j)	= ai(i,j) / d;
                }
            }
        }
    }
    
    return res;
}

matrix inv2(const matrix& a)
{
    int m = a.sizeRow();
    if (m != a.sizeCol()) Common::ExceptionError(FromHere(), "Square matrix required for Inv", Common::ErrorCodes::MismatchDimension());

    matrix res(m,m);
    
    int i, j;
    
    int info  = 0;
    int *ipiv = new int [m+1];
    int lwork = m * m;
    double *work	= new double [lwork];
    double *element;
    
    element = a.extractToPtr();
    dgetrf(&m, &m, element, &m, ipiv, &info);
    dgetri(&m, element, &m, ipiv, work, &lwork, &info);
    res.assignFromPtr(element, m, m);
    
    delete[] ipiv;
    delete[] work;
    delete[] element;
    
    return (res);
}


void block_tri_diagonal_decomp(matrix *A, matrix *B, matrix *C, int NB)
{
    for (int  i = 1; i <= NB; i++)
    {
        int m_a = A[i].sizeRow();
        int m_b = B[i].sizeRow();
        int m_c = C[i].sizeRow();
    
        if (m_a != A[i].sizeCol()) Common::ExceptionError(FromHere(), "The Block matrices[A] should be square", Common::ErrorCodes::MismatchDimension());
        if (m_b != B[i].sizeCol()) Common::ExceptionError(FromHere(), "The Block matrices[B] should be square", Common::ErrorCodes::MismatchDimension());
        if (m_c != C[i].sizeCol()) Common::ExceptionError(FromHere(), "The Block matrices[C] should be square", Common::ErrorCodes::MismatchDimension());
    }
    
    // LU Decomposition for Block-tridiagonal matrix
    // L = |B1 0   0 ... 0|
    //   = |A2 B2  0 ... 0|
    //   = |   A3 B3 ... 0|
    //
    // U = |I C1  0  ... 0|
    //   = |0  I  C2 ... 0|
    //   = |0  0   I  C3 0|
    
    // Step 2: Get C_1
    matrix B1_inv = inv(B[1]);
    C[1] = B1_inv * C[1];
    
    // Step 3: for other boundaries
#pragma ivdep
    for (int i = 2; i <= NB; i++)
    {
        matrix AC = A[i]*C[i-1];
        B[i] = B[i] - AC;
        
        matrix Binv = inv(B[i]);
        C[i] = Binv * C[i];
    }
}


void block_tri_diagonal_solver(matrix *A, matrix *B, matrix *C, matrix *R, matrix *X, int NB)
{
    int m = A[1].sizeRow();
    
    for (int  i = 1; i <= NB; i++)
    {
        int m_r = R[i].sizeRow();
        int m_x = X[i].sizeRow();
        
        if (m_r != m) Common::ExceptionError(FromHere(), "The size of vector R should be same as A", Common::ErrorCodes::MismatchDimension());
        if (m_x != m) Common::ExceptionError(FromHere(), "The size of vector X should be same as A", Common::ErrorCodes::MismatchDimension());
        
        if (R[i].sizeCol() != 1) Common::ExceptionError(FromHere(), "R should be vector (not matrix)", Common::ErrorCodes::MismatchDimension());
        if (X[i].sizeCol() != 1) Common::ExceptionError(FromHere(), "X should be vector (not matrix)", Common::ErrorCodes::MismatchDimension());
    }
    
    block_tri_diagonal_decomp(A, B, C, NB);
    
    // Step 3:: Forward substitution
    matrix *Y;
    Y	= new matrix [NB+1];
    for (int i = 0; i <= NB; i++)	Y[i].zeros(m, 1);
    
    matrix Binv;
    
    Binv = inv(B[1]);
    Y[1] = Binv * R[1];
    
    for (int i = 2;  i <= NB; i++)
    {
        Binv = inv(B[i]);
        matrix aux1 = A[i]*Y[i-1];
        matrix aux2 = R[i] - aux1;
        Y[i] = Binv * aux2;
    }
    
    // Step 4:: Backward substitution
    X[NB]	= Y[NB];
    for (int i = NB-1; i >= 1; i--)
    {
        matrix aux3;
        aux3 = C[i] * X[i+1];
        X[i]= Y[i] - aux3;
    }
}


void rotx(const double ang, matrix& res)
{
    res.diag(3);
    
    double sine_theta = sin(ang);
    double cosine_theta = cos(ang);
 
    res(1,1) = cosine_theta;
    res(1,2) = -sine_theta;
    
    res(2,1) = sine_theta;
    res(2,2) = cosine_theta;
}


void roty(const double ang, matrix& res)
{
    res.diag(3);
    
    double sine_theta = sin(ang);
    double cosine_theta = cos(ang);
    
    res(0,0) = cosine_theta;
    res(0,2) = sine_theta;
    
    res(2,0) = -sine_theta;
    res(2,2) = cosine_theta;
}

void rotz(const double ang, matrix& res)
{
    res.diag(3);
    
    double sine_theta = sin(ang);
    double cosine_theta = cos(ang);
    
    res(0,0) = cosine_theta;
    res(0,1) = -sine_theta;
    
    res(1,0) = sine_theta;
    res(1,1) = cosine_theta;
}

void rot2D(const double ang, matrix& res)
{
    res.resize(2, 2);
    
    double sine_theta = sin(ang);
    double cosine_theta = cos(ang);
    
    res(0,0) = cosine_theta;
    res(0,1) = -sine_theta;
    
    res(1,0) = sine_theta;
    res(1,1) = cosine_theta;
}


void rotx(const double ang, bool is_degree, matrix& res)
{
    double rad;
    
    if(is_degree == true)   rad = ang * M_PI/180.0;
    rotx(rad, res);
}


void roty(const double ang, bool is_degree, matrix& res)
{
    double rad;
    if(is_degree == true)   rad = ang * M_PI/180.0;
    roty(rad, res);
}

void rotz(const double ang, bool is_degree, matrix& res)
{
    double rad;
    if(is_degree == true)   rad = ang * M_PI/180.0;
    rotz(rad, res);
}

void rot2D(const double ang, bool is_degree, matrix& res)
{
    double rad;
    if(is_degree == true)   rad = ang * M_PI/180.0;
    
    rot2D(rad, res);
}

double dot(const matrix& a, const matrix& b)
{
    double res = 0.0;
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    if (m != b.sizeRow())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());
    
    if (n != b.sizeCol())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());

    for (int ii = 0; ii < m; ii++)
    {
        for (int jj = 0; jj < n; jj++)
        {
            res += a.element(ii, jj) * b.element(ii, jj);
        }
    }
    
    return (res);
}

matrix cross(const matrix& a, const matrix& b)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    if (m != b.sizeRow())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());
    
    if (n != b.sizeCol())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());

    matrix res(m, n);
    
    if (m == 3)
    {
#pragma ivdep
        for (int jj = 0; jj < n; jj++)
        {
            res(0, jj) = a.element(1, jj)*b.element(2, jj) - a.element(2, jj)*b.element(1,jj);
            res(1, jj) = a.element(2, jj)*b.element(0, jj) - a.element(0, jj)*b.element(2,jj);
            res(2, jj) = a.element(0, jj)*b.element(1, jj) - a.element(1, jj)*b.element(0,jj);
        }
        
        return (res);
    }
    
    if (n == 3)
    {
#pragma ivdep
        for (int ii = 0; ii < m; ii++)
        {
            res(ii, 0) = a.element(ii, 1)*b.element(ii, 2) - a.element(ii, 2)*b.element(ii, 1);
            res(ii, 1) = a.element(ii, 2)*b.element(ii, 0) - a.element(ii, 0)*b.element(ii, 2);
            res(ii, 2) = a.element(ii, 0)*b.element(ii, 1) - a.element(ii, 1)*b.element(ii, 0);
        }
        
        return (res);
    }
    
    
    Common::ExceptionError(FromHere(), "A and B must be 3xM or Mx3 matrix", Common::ErrorCodes::MismatchDimension());
    
    return (res);
}



matrix normalFromThreePoints(const matrix& a, const matrix&b, const matrix&c)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    if (m != b.sizeRow())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());
    
    if (n != b.sizeCol())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());

    if (m != c.sizeRow())
        Common::ExceptionError(FromHere(), "A and C must be same size", Common::ErrorCodes::MismatchDimension());
    
    if (n != c.sizeCol())
        Common::ExceptionError(FromHere(), "A and C must be same size", Common::ErrorCodes::MismatchDimension());

    
    matrix res(m,n);
    matrix ab = b - a;
    matrix ac = c - a;
    
    if (m == 3 && n ==1)
    {
        res = cross(ab, ac);
    }
    else if (m ==1 && n == 3)
    {
        res = cross(ab, ac);
    }
    else
    {
        Common::ExceptionError(FromHere(), "Matrix should be a 3D vector", Common::ErrorCodes::MismatchDimension());
    }
    
    return (res);
}


void svdMatrix(matrix& a, matrix& res)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    int lda = n;
    int ldu = m;
    int ldvt = n;
    int lds = min<int>(m,n);
    
    // Setup buffers to hold the matrices U, sigma and Vt:
    std::vector<double> superb(lds);
    std::vector<double>  s(lds);
    std::vector<double>  u(m*m);
    std::vector<double> vt(n*n);
    
    double*  A = a.extractToPtr();
    
    
    // Compute SVD
    int info;
    info = LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'A', 'A', m, n, A, lda, s.data(), u.data(), ldu, vt.data(), ldvt, superb.data());
   
    
    // Check for convergence
    if( info > 0 )
    {
        printf( "The algorithm computing SVD failed to converge.\n" );
        exit( 1 );
    }
    
    
    res.resize(1, lds);
    for (int ii = 0; ii < lds; ii++) res(ii) = s[ii];
    
}


void sumMatrix(matrix& a, matrix& sumA)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    matrix res(1,n);
    
    if (m == 1)
    {
        double temp = 0.0;
        for (int j = 0; j < n; j++) temp = temp + a.element(0,j);
        
        res(0,0) = temp;
    }
    else if (n == 1)
    {
        double temp = 0.0;
        for (int i = 0; i < m; i++) temp = temp + a.element(i,0);
        
        res(0,0) = temp;
    }
    else
    {
        for (int j = 0; j < n; j++)
        {
            double temp = 0.0;
            for (int i = 0; i < m; i++) temp = temp + a.element(i,j);
        
            res(0, j) = temp;
        }
    }
    
    sumA = res;
}


void absMatrix(matrix& a, matrix& res)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    res.resize(m, n);
    for (int ii = 0; ii < m; ii++)
    {
        for (int jj = 0; jj < n; jj++)
        {
            res(ii, jj) = fabs(a.element(ii,jj));
        }
    }
}


int   matrix_type(const matrix&a)
{
    int m = a.sizeRow();
    int n = a.sizeCol();
   
    int flag = 5; // Matrix
    
    if (m == 1 && n == 1)
    {
        flag = 0;   // Scalar
    }
    else if (n == 1)
    {
        if (m >= 2 && m <= 3)   flag = 1;   // vector
        else                    flag = 3;   // [m x 1] matrix
    }
    else if (m == 1)
    {
        if (n >= 2 && n <= 3)   flag = 2;   // transverse vector
        else                    flag = 4;   // [1 x n] matrix
        
    }

    return (flag);
}





double norm(matrix&a, const int flag)
{
    double res = 0.0;
    int m = a.sizeRow();
    int n = a.sizeCol();
    
    if (m == 1)
    {
        res = 0.0;
        
        if (flag == 1)
        {
            for (int j = 0; j < n; j++)
            {
                res = res + fabs(a(0, j));
            }
        }
        else if (flag == 2)
        {
            for (int j = 0; j < n; j++)
            {
                res = res + pow(a(0, j), 2);
            }
            
            res = pow(res, 0.5);
        }
        else if (flag > 0)
        {
            for (int j = 0; j < n; j++)
            {
                res = res + pow(fabs(a(0, j)), flag);
            }
            
            res = pow(res, 1.0/flag);
        }
        else
        {
            Common::ExceptionError(FromHere(), "MAtrix norm: it is not supported flag", Common::ErrorCodes::NotSupportedType());
        }
    }
    else if (n == 1)
    {
        res = 0.0;
        
        if (flag == 1)
        {
            for (int i = 0; i < m; i++)
            {
                res = res + fabs(a(i, 0));
            }
        }
        else if (flag == 2)
        {
            for (int i = 0; i < m; i++)
            {
                res = res + pow(a(i, 0), 2);
            }
            
            res = pow(res, 0.5);
        }
        else if (flag > 0)
        {
            for (int i = 0; i < m; i++)
            {
                res = res + pow(fabs(a(i, 0)), flag);
            }
            
            res = pow(res, 1.0/flag);
        }
        else
        {
            Common::ExceptionError(FromHere(), "MAtrix norm: it is not supported flag", Common::ErrorCodes::NotSupportedType());
        }
    }
    else
    {
        if (flag == 1)
        {
            matrix absA;
            absMatrix(a, absA);
            
            matrix sumA;
            sumMatrix(absA, sumA);
            res = sumA.max();
        }
        else if (flag == 2)
        {
            matrix svdA;
            svdMatrix(a, svdA);
            res = svdA.max();
        }
        else
        {
            Common::ExceptionError(FromHere(), "MAtrix norm: it is not supported flag", Common::ErrorCodes::NotSupportedType());
        }
    }
    
    return (res);
}





matrix vectorStartEnd(const matrix& s, const matrix& e)
{
    int m = s.sizeRow();
    int n = s.sizeCol();
    
    if (m != e.sizeRow())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());
    
    if (n != e.sizeCol())
        Common::ExceptionError(FromHere(), "A and B must be same size", Common::ErrorCodes::MismatchDimension());
    
    matrix res(m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            res(i,j) = e.element(i,j) - s.element(i,j);
        }
    }
    return (res);
}








