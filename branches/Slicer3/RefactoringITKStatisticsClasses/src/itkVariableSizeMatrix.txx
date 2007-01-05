/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVariableSizeMatrix.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/21 17:48:24 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkVariableSizeMatrix_txx
#define _itkVariableSizeMatrix_txx

#include "itkVariableSizeMatrix.h" 
#include "itkNumericTraits.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_transpose.h"
#include "vnl/vnl_matrix.h"

namespace itk
{



 
/**
 *  Product by a Vector
 */
template<class T>
Array<T> 
VariableSizeMatrix<T>
::operator*( const Array<T> & vect ) const
{
  if( vect.Size() != m_Rows )
    {
    itkExceptionMacro( << "Matrix with " << m_Cols << " columns cannot be "
        << "multiplied with array of length: " << vect.Size() );
    }
  
  Array<T> result(m_Rows);
  for( unsigned int r=0; r<this->m_Rows; r++) 
    {
    T sum = NumericTraits<T>::Zero;   
    for( unsigned int c=0; c< this->m_Cols; c++ ) 
      {
      sum += m_Matrix(r,c) * vect[c];
      }
    result[r] = sum;
    }
  return result;
}



 
/**
 *  Product by a matrix
 */
template<class T >
VariableSizeMatrix<T>
VariableSizeMatrix<T>
::operator*( const Self & matrix ) const
{
  Self result;
  result = m_Matrix * matrix.m_Matrix;
  return result;
}

  
/**
 *  Matrix Addition 
 */
template<class T >
VariableSizeMatrix<T>
VariableSizeMatrix<T>
::operator+( const Self & matrix ) const
{
  if( (matrix.rows() != this->m_Rows) ||
      (matrix.cols() != this->m_Cols))
    { 
    itkExceptionMacro( << "Matrix with size (" << matrix.rows() << "," << 
      matrix.cols() << ") cannot be subtracted from matrix with size (" <<
      this->m_Rows << "," << this->m_Cols );
    }
  
  Self result;
  for( unsigned int r=0; r< this->m_Rows; r++) 
    {
    for( unsigned int c=0; c< this->m_Cols; c++ ) 
      {
      result.m_Matrix(r,c) = m_Matrix(r,c) + matrix.m_Matrix(r,c);
      }
    }
  return result;
}

  

/**
 *  Matrix Addition in-place
 */
template<class T>
const VariableSizeMatrix<T> &
VariableSizeMatrix<T>
::operator+=( const Self & matrix ) 
{
  if( (matrix.rows() != this->m_Rows) ||
      (matrix.cols() != this->m_Cols))
    { 
    itkExceptionMacro( << "Matrix with size (" << matrix.rows() << "," << 
      matrix.cols() << ") cannot be subtracted from matrix with size (" <<
      this->m_Rows << "," << this->m_Cols );
    }
  
  for( unsigned int r=0; r<this->m_Rows; r++) 
    {
    for( unsigned int c=0; c<this->m_Cols; c++ ) 
      {
      m_Matrix(r,c) += matrix.m_Matrix(r,c);
      }
    }
  return *this;
}



  
/**
 *  Matrix Subtraction
 */
template<class T>
VariableSizeMatrix<T>
VariableSizeMatrix<T>
::operator-( const Self & matrix ) const
{
  if( (matrix.rows() != this->m_Rows) ||
      (matrix.cols() != this->m_Cols))
    { 
    itkExceptionMacro( << "Matrix with size (" << matrix.rows() << "," << 
      matrix.cols() << ") cannot be subtracted from matrix with size (" <<
      this->m_Rows << "," << this->m_Cols );
    }
  
  Self result;
  for( unsigned int r=0; r<this->m_Rows; r++) 
    {
    for( unsigned int c=0; c<this->m_Cols; c++ ) 
      {
      result.m_Matrix(r,c) = m_Matrix(r,c) - matrix.m_Matrix(r,c);
      }
    }
  return result;
}



/**
 *  Matrix subtraction in-place 
 */
template<class T>
const VariableSizeMatrix<T> &
VariableSizeMatrix<T>
::operator-=( const Self & matrix ) 
{
  if( (matrix.rows() != this->m_Rows) ||
      (matrix.cols() != this->m_Cols))
    { 
    itkExceptionMacro( << "Matrix with size (" << matrix.rows() << "," << 
      matrix.cols() << ") cannot be subtracted from matrix with size (" <<
      this->m_Rows << "," << this->m_Cols );
    }
  
  for( unsigned int r=0; r<this->m_Rows; r++) 
    {
    for( unsigned int c=0; c<this->m_Cols; c++ ) 
      {
      m_Matrix(r,c) -= matrix.m_Matrix(r,c);
      }
    }
  return *this;
}





/**
 *  Product by a vnl_matrix
 */
template<class T>
vnl_matrix<T> 
VariableSizeMatrix<T>
::operator*( const vnl_matrix<T> & matrix ) const
{
  return m_Matrix * matrix;
}


 
/**
 *  Product by a matrix
 */
template<class T>
void
VariableSizeMatrix<T>
::operator*=( const Self & matrix ) 
{
  m_Matrix *= matrix.m_Matrix;
}

 
/**
 *  Product by a vnl_matrix
 */
template<class T>
void
VariableSizeMatrix<T>
::operator*=( const vnl_matrix<T> & matrix ) 
{
  m_Matrix *= matrix;
}



/**
 *  Product by a vnl_vector
 */
template<class T>
vnl_vector<T>
VariableSizeMatrix<T>
::operator*( const vnl_vector<T> & vc ) const
{
  return m_Matrix * vc;
}



/**
 *  Assignment
 */
template<class T>
const VariableSizeMatrix<T> &
VariableSizeMatrix<T>
::operator=( const Self  & matrix )
{
  m_Matrix = matrix.m_Matrix;
  return *this;
}

template<class T>
const VariableSizeMatrix<T> &
VariableSizeMatrix<T>
::operator=( const vnl_matrix<T>  & matrix )
{
  m_Matrix = matrix;
  return *this;
}

/**
 *  Comparison
 */
template<class T>
bool
VariableSizeMatrix<T>
::operator==( const Self & matrix )
{
  if( (matrix.rows() != this->m_Rows) ||
      (matrix.cols() != this->m_Cols))
    { 
    return false;
    }
  bool equal = true;
  
  for( unsigned int r=0; r<this->m_Rows; r++) 
    {
    for( unsigned int c=0; c<this->m_Cols; c++ ) 
      {
      if (m_Matrix(r,c) != matrix.m_Matrix(r,c))
        {
        equal = false;
        break;
        }
      }
    }
  return equal;
}

template<class T>
bool
VariableSizeMatrix<T>
::operator!=( const Self & matrix )
{
  return !this->operator==(matrix);
}


/**
 *  Returns the inverse matrix
 */
template<class T>
vnl_matrix<T>
VariableSizeMatrix<T>
::GetInverse( void ) const
{
  vnl_matrix<T> temp = vnl_matrix_inverse<T>( m_Matrix );
  return temp;
}


/**
 *  Returns the transposed matrix
 */
template<class T>
vnl_matrix<T>
VariableSizeMatrix<T>
::GetTranspose( void ) const
{
  return m_Matrix.transpose();
}

 

} // end namespace itk


#endif
