/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkEuclideanDistance.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:46 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkEuclideanDistance_h
#define __itkEuclideanDistance_h

#include "itkEuclideanDistanceMetric.h"

namespace itk { 
namespace Statistics {

/** \class EuclideanDistance
 * \brief Euclidean distance function.
 *
 * This class is derived from DistanceMetric class. In addition to the
 * two Evaluate methods in the base class, this class has a method
 * (Evaluate) to compute the coordinate distance between two vector
 * component (not vectors), and a method to tell if a measurement
 * vector is whithin the range (defined by a radius value) from the
 * origin (set by SetOrigin mehtod).
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
class ITK_EXPORT EuclideanDistance : 
    public EuclideanDistanceMetric< TVector >
{
public:
  /** Standard "Self" typedef. */
  typedef EuclideanDistance                  Self;
  typedef EuclideanDistanceMetric< TVector > Superclass;
  typedef SmartPointer< Self >               Pointer; 
  typedef SmartPointer<const Self>           ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(EuclideanDistance, EuclideanDistanceMetric);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
protected:
  EuclideanDistance() {}
  virtual ~EuclideanDistance() {} 
}; // end of class

} // end of namespace Statistics 
} // end of namespace itk
#endif
