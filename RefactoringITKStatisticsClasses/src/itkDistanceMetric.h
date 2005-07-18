/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDistanceMetric.h,v $
  Language:  C++
  Date:      $Date: 2004/11/04 20:40:41 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDistanceMetric_h
#define __itkDistanceMetric_h

#include "itkMembershipFunctionBase.h"
#include "itkArray.h"
#include "itkMeasurementVectorTraits.h"

namespace itk{ 
namespace Statistics{

/** \class DistanceMetric
 * \brief this class declares common interfaces 
 * for distance functions.
 *
 * As a function derived from MembershipFunctionBase, 
 * users use Evaluate method to get result.
 * 
 * To use this function in the context of MembershipFunction,
 * users should first set the origin by calling SetOrigin() function,
 * then call Evaluate() method with a point to get the distance between
 * the origin point and the evaluation point.
 * 
 * If users want to the distance between two points without setting
 * the origin point. Use two argument version of Evaluate() function.
 * 
 * The class can be templated over any container that holds data elements. The 
 * containter is expected to provide access to its elements with the [] operator.
 */

template< class TVector >
class ITK_EXPORT DistanceMetric : public MembershipFunctionBase< TVector >
{
public:
  /** Standard typedefs */
  typedef DistanceMetric Self;
  typedef MembershipFunctionBase< TVector > Superclass;

  /** Typedef for the length of each measurement vector */
  typedef unsigned int  MeasurementVectorSizeType;

  /** Typedef for the measurement vector */
  typedef TVector       MeasurementVectorType;

  /** DEPRECATED: The static const macro will be deprecated in a future version.
   * Please use GetMeasurementVectorSize() instead. This constant returns the 
   * length of a measurement vector for FixedArrays, Vectors and other fixed 
   * containers and zero for dynamically resizable containers. The true value for 
   * dynamically resizable containers will be obtained from the 
   * GetMeasurementVectorSize() call. 
   */
  itkStaticConstMacro(VectorLength, unsigned int,
     MeasurementVectorTraits< MeasurementVectorType >::MeasurementVectorLength);

  /** Set/Get Macro to set the length of each measurement vector. */
  virtual void SetMeasurementVectorSize( MeasurementVectorSizeType );
  itkGetConstMacro( MeasurementVectorSize, MeasurementVectorSizeType ); 
  
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(DistanceMetric, MembershipFunctionBase);

  /** OriginType typedef */
  typedef typename MeasurementVectorTraits< 
           MeasurementVectorType >::OriginType OriginType ;

  /** Sets the origin point that will be used for the single point 
   * version Evaluate() function. This function is necessary part of
   * implementing MembershipFunctionBase's Evaluate() interface */ 
  void SetOrigin(const OriginType& x) ;
  itkGetConstReferenceMacro(Origin, OriginType) ;

  /** Gets the distance between the origin point and x. This function
   * work with SetOrigin() function*/
  virtual double Evaluate(const TVector &x) const = 0 ;
  
  /** Gets the distance between x1 and x2 points */
  virtual double Evaluate(const TVector &x1, const TVector &x2) const = 0 ;
  
protected:
  DistanceMetric() 
    {
    m_MeasurementVectorSize = MeasurementVectorTraits< 
                               MeasurementVectorType >::GetSize();
    }

  virtual ~DistanceMetric() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  OriginType m_Origin ;

  MeasurementVectorSizeType m_MeasurementVectorSize;
  
} ; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDistanceMetric.txx"
#endif

#endif







