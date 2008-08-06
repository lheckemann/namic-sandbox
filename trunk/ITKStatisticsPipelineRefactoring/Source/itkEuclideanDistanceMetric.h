/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkEuclideanDistanceMetric.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:55 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkEuclideanDistanceMetric_h
#define __itkEuclideanDistanceMetric_h

#include "itkNumericTraits.h"
#include "itkDistanceMetric.h"
#include "itkMeasurementVectorTraits.h"

namespace itk  { 
namespace Statistics  {

/** \class EuclideanDistanceMetric
 * \brief Euclidean distance function.
 *
 * The class can be templated over any container that holds data elements. The 
 * containter is expected to provide access to its elements with the [] operator.
 * It must also implement a Size() that returns the length of the container.
 * It must also contain a typedef "ValueType" that defines the data-type held
 * by the container.
 * (In other words it will support itk::Vector, FixedArray, Array ).
 * 
 * <b>Recent API changes:</b>
 * The static const macro to get the length of a measurement vector,
 * \c VectorLength  has been removed to allow the length of a measurement
 * vector to be specified at run time. Please use the function 
 * GetMeasurementVectorSize() instead.
 *
 */
template< class TVector >
class ITK_EXPORT EuclideanDistanceMetric : 
    public DistanceMetric< TVector >
{
public:
  /** Standard "Self" typedef. */
  typedef EuclideanDistanceMetric       Self;
  typedef DistanceMetric< TVector >     Superclass;
  typedef SmartPointer< Self >          Pointer; 
  typedef SmartPointer<const Self>      ConstPointer;

  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;

  typedef typename Superclass::OriginType OriginType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(EuclideanDistanceMetric, DistanceMetric);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type of the component of a vector */
  typedef typename TVector::ValueType ValueType;

  /** Gets the distance between the origin and x */
  double Evaluate(const TVector &x) const;

protected:
  EuclideanDistanceMetric() {}
  virtual ~EuclideanDistanceMetric() {} 
}; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkEuclideanDistanceMetric.txx"
#endif

#endif
