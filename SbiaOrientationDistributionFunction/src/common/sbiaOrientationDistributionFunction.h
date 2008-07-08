/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDiffusionTensor3D.h,v $
  Language:  C++
  Date:      $Date: 2007-01-30 20:56:07 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __sbiaOrientationDistributionFunction_h
#define __sbiaOrientationDistributionFunction_h

// Undefine an eventual OrientationDistributionFunction macro
#ifdef OrientationDistributionFunction
#undef OrientationDistributionFunction
#endif

#include <sbiaSymRealSphericalHarmonicRep.h>


namespace itk
{


template < typename TComponent , unsigned int NOrder=4 >
class OrientationDistributionFunction: public SymRealSphericalHarmonicRep<TComponent,NOrder>
{
public:
  /** Standard class typedefs. */
  typedef OrientationDistributionFunction  Self;
  typedef SymRealSphericalHarmonicRep<TComponent, NOrder> Superclass;
  
  /** Propagating some typedef from the superclass */
  typedef typename Superclass::ValueType             ValueType;
  typedef typename Superclass::ComponentType         ComponentType;
  typedef typename Superclass::ComponentArrayType    ComponentArrayType;
  typedef typename Superclass::AccumulateValueType   AccumulateValueType;
  typedef typename Superclass::RealValueType         RealValueType;

  /** Runtime information support. */
  itkTypeMacro(OrientationDistributionFunction, SymRealSphericalHarmonicRep);

  /** Default Constructor. */
  OrientationDistributionFunction();

  /** Constructor with initialization. */
  OrientationDistributionFunction(const Self& r);
  OrientationDistributionFunction(const Superclass& r);
  OrientationDistributionFunction(const ComponentType& r);
  OrientationDistributionFunction(const ComponentArrayType r);

  /** Pass-through assignment operator for the Array base class. */
  Self& operator= (const Self& r);
  Self& operator= (const Superclass & r);
  Self& operator= (const ComponentType& r);
  Self& operator= (const ComponentArrayType r);
  Self& operator= (const vnl_vector<ComponentType> r);

  /** Normalize the distribution */

  void Normalize();
  void InitFromDwiCoeffs( const vnl_vector<double> rshCoeffs );

  FixedArray<TComponent, itkGetStaticConstMacro(InternalDimension)>
    GetFixedArray()
  {
    return (*this);
  }
};

} // end namespace itk
# include "sbiaOrientationDistributionFunction.txx"

#endif
