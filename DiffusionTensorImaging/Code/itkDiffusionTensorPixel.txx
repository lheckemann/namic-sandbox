/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDiffusionTensorPixel.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/20 12:29:59 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkDiffusionTensorPixel_txx
#define _itkDiffusionTensorPixel_txx
#include "itkDiffusionTensorPixel.h"
#include "itkNumericTraits.h"

namespace itk
{

/*
 * Assignment Operator
 */
template<class T>
DiffusionTensorPixel<T>&
DiffusionTensorPixel<T>
::operator= (const Self& r)
{
  BaseArray::operator=(r);
  return *this;
}


/*
 * Assigment from a plain array
 */
template<class T>
DiffusionTensorPixel<T>&
DiffusionTensorPixel<T>
::operator= (const ComponentType r[Dimension])
{
  BaseArray::operator=(r);
  return *this;
}


  
/**
 * Returns a temporary copy of a vector
 */
template<class T>
DiffusionTensorPixel<T> 
DiffusionTensorPixel<T>
::operator+(const Self & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++) 
    {
    result[i] = (*this)[i] + r[i];
    }
  return result;
}




/**
 * Returns a temporary copy of a vector
 */
template<class T>
DiffusionTensorPixel<T> 
DiffusionTensorPixel<T>
::operator-(const Self & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++) 
    {
    result[i] = (*this)[i] - r[i];
    }
  return result;
}


 
/**
 * Returns a temporary copy of a vector
 */
template<class T>
const DiffusionTensorPixel<T> & 
DiffusionTensorPixel<T>
::operator+=(const Self & r) 
{
  for( unsigned int i=0; i<Dimension; i++) 
    {
    (*this)[i] += r[i];
    }
  return *this;
}



 
/**
 * Returns a temporary copy of a vector
 */
template<class T>
const DiffusionTensorPixel<T> & 
DiffusionTensorPixel<T>
::operator-=(const Self & r)
{
  for( unsigned int i=0; i<Dimension; i++) 
    {
    (*this)[i] -= r[i];
    }
  return *this;
}





/**
 * Returns a temporary copy of a vector
 */
template<class T>
DiffusionTensorPixel<T> 
DiffusionTensorPixel<T>
::operator*(const ComponentType & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++) 
    {
    result[i] = (*this)[i] * r;
    }
  return result;
}


/*
 * Matrix notation access to elements
 */
template<class T>
const typename DiffusionTensorPixel<T>::ValueType &
DiffusionTensorPixel<T>
::operator()(unsigned int i, unsigned int j) const
{
  unsigned int k = i * 3 + j;
  if( k >= 5 )
    {
    return  (*this)[0];
    }
  return (*this)[k];
}



/*
 * Matrix notation access to elements
 */
template<class T>
typename DiffusionTensorPixel<T>::ValueType &
DiffusionTensorPixel<T>
::operator()(unsigned int i, unsigned int j)
{
  unsigned int k = i * 3 + j;
  if( k >= 5 )
    {
    return  (*this)[0];
    }
  return (*this)[k];
}





/**
 * Print content to an ostream
 */
template<class TComponent>
std::ostream &
operator<<(std::ostream& os,const DiffusionTensorPixel<TComponent> & c ) 
{
  for(unsigned int i=0; i<c.GetNumberOfComponents(); i++)
    {
    os <<  static_cast<typename NumericTraits<TComponent>::PrintType>(c[i]) << "  ";
    }
  return os;
}


/**
 * Read content from an istream
 */
template<class TComponent>
std::istream &
operator>>(std::istream& is, DiffusionTensorPixel<TComponent> & dt ) 
{
  for(unsigned int i=0; i < dt.GetNumberOfComponents(); i++)
    {
    is >> dt[i];
    }
  return is;
}

} // end namespace itk

#endif
