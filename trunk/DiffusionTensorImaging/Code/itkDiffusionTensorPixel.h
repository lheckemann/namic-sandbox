/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDiffusionTensorPixel.h,v $
  Language:  C++
  Date:      $Date: 2004/12/04 13:20:17 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDiffusionTensorPixel_h
#define __itkDiffusionTensorPixel_h

// Undefine an eventual DiffusionTensorPixel macro
#ifdef DiffusionTensorPixel
#undef DiffusionTensorPixel
#endif

#include <itkIndent.h>
#include <itkFixedArray.h>
#include <itkMatrix.h>
#include "vnl/vnl_math.h"

namespace itk
{

/** \class DiffusionTensorPixel
 * \brief Represent a diffusion tensor as used in DTI images.
 *
 * This class implements a 3D symmetric tensor as it is used for representing
 * diffusion of water molecules in Diffusion Tensor Images.
 *
 * Since DiffusionTensorPixel is a subclass of Array, you can access its components as:
 *
 * typedef itk::DiffusionTensorPixel< float >    TensorPixelType;
 * TensorPixelType tensor;
 * 
 *   tensor[0] = 1.233;
 *   tensor[1] = 1.456;
 *
 * for convenience the indexed access is also available as
 *
 *   tensor(0,0) = 1.233;
 *   tensor(2,0) = 1.233;
 *
 * The Tensor in principle represents a 3x3 matrix, but given that it is
 * always symmetric the representation can be compacted into a 6-elements
 * array that derives from the itk::FixedArray<T>
 *
 * \ingroup ImageObjects
 */

template < typename TComponent = float >
class DiffusionTensorPixel: public FixedArray<TComponent,6>
{
public:
  /** Standard class typedefs. */
  typedef DiffusionTensorPixel  Self;
  typedef FixedArray<TComponent, 6> SuperClass;
  
  /** Dimension of the vector space. */
  itkStaticConstMacro(Dimension, unsigned int, 6);
  itkStaticConstMacro(Length, unsigned int, 6);

  /** Convenience typedefs. */
  typedef FixedArray<TComponent, 6> BaseArray;
  
  /** Array of eigen-values. */
  typedef FixedArray<TComponent, 3> EigenValuesArray;
  
  /** Matrix of eigen-vectors. */
  typedef Matrix<TComponent, 3, 3> EigenVectorsMatrix;
  
  /**  Define the component type. */
  typedef TComponent ComponentType;
  typedef typename SuperClass::ValueType ValueType;
  
  /** Default constructor has nothing to do. */
  DiffusionTensorPixel() {this->Fill(0);}
  DiffusionTensorPixel (const ComponentType& r) { this->Fill(r); }
  
  /** Pass-through constructor for the Array base class. */
  DiffusionTensorPixel(const Self& r): BaseArray(r) {}
  DiffusionTensorPixel(const ComponentType  r[6]): BaseArray(r) {}  
    
  /** Pass-through assignment operator for the Array base class. */
  Self& operator= (const Self& r);
  Self& operator= (const ComponentType r[6]);

  /** Aritmetic operations between pixels. Return a new DiffusionTensorPixel. */
  Self operator+(const Self &vec) const;
  Self operator-(const Self &vec) const;
  const Self & operator+=(const Self &vec);
  const Self & operator-=(const Self &vec);
  Self operator*(const ComponentType &f) const;

 
  /** Return the number of components. */
  static int GetNumberOfComponents(){ return 6;}

  /** Return the value for the Nth component. */
  ComponentType GetNthComponent(int c) const
    { return this->operator[](c); }

  /** Set the Nth component to v. */
  void SetNthComponent(int c, const ComponentType& v)  
    {  this->operator[](c) = v; }

  /** Matrix notation, in const and non-const forms. */
  ValueType & operator()( unsigned int i, unsigned int j);
  const ValueType & operator()( unsigned int i, unsigned int j) const;

};


template< typename TComponent  >  
ITK_EXPORT std::ostream& operator<<(std::ostream& os, 
                                    const DiffusionTensorPixel<TComponent> & c); 
template< typename TComponent  >  
ITK_EXPORT std::istream& operator>>(std::istream& is, 
                                          DiffusionTensorPixel<TComponent> & c); 

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionTensorPixel.txx"
#endif

#endif
