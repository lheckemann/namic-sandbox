/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkManhattanDistanceMetric.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:55 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkManhattanDistanceMetric_h
#define __itkManhattanDistanceMetric_h

#include "itkNumericTraits.h"
#include "itkDistanceMetric.h"
#include "itkMeasurementVectorTraits.h"

namespace itk  { 
namespace Statistics  {

/** \class ManhattanDistanceMetric
 * \brief Euclidean distance function.
 *
 * \sa DistanceMetric
 * \sa EuclideanSquareDistanceMetric
 * \sa EuclideanDistanceMetric
 *
 */
template< class TVector >
class ITK_EXPORT ManhattanDistanceMetric : 
    public DistanceMetric< TVector >
{
public:
  /** Standard "Self" typedef. */
  typedef ManhattanDistanceMetric       Self;
  typedef DistanceMetric< TVector >     Superclass;
  typedef SmartPointer< Self >          Pointer; 
  typedef SmartPointer<const Self>      ConstPointer;

  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ManhattanDistanceMetric, DistanceMetric);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type of the component of a vector */
  typedef typename TVector::ValueType ValueType;

  /** Gets the distance between the origin and x */
  double Evaluate(const TVector &x) const;

protected:
  ManhattanDistanceMetric() {}
  virtual ~ManhattanDistanceMetric() {} 
}; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManhattanDistanceMetric.txx"
#endif

#endif
