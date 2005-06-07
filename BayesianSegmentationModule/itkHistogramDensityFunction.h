/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramDensityFunction.h,v $
  Language:  C++
  Date:      $Date: 2003/12/09 16:53:08 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogramDensityFunction_h
#define __itkHistogramDensityFunction_h

#include "itkDensityFunction.h"

namespace itk{ 
namespace Statistics{

/** \class HistogramDensityFunction
 * \brief HistogramDensityFunction class estimate a probability density
 * function from the data of a histogram.
 *
 * The Evaluate method returns density value for the input measurement vector
 */

template< class TMeasurementVector, class THistogram >
class ITK_EXPORT HistogramDensityFunction :
    public DensityFunction< TMeasurementVector >
{
public:
  /** Standard class typedefs */
  typedef HistogramDensityFunction Self;
  typedef DensityFunction< TMeasurementVector > Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Types related to the Histogram */
  typedef THistogram                             HistogramType;
  typedef typename HistogramType::ConstPointer   HistogramConstPointer;

  /** Standard macros */
  itkTypeMacro(HistogramDensityFunction, DensityFunction);
  itkNewMacro(Self);

  typedef TMeasurementVector MeasurementVectorType;

  /** Method to get probability of an instance. The return value is the
   * value of the density function, not probability. */
  virtual double Evaluate(const TMeasurementVector &measurement) const;
 
  /**  Set/Get the historgram */
  itkSetObjectMacro( Histogram, HistogramType );  
  itkGetConstObjectMacro( Histogram, HistogramType );  
 
protected:
  HistogramDensityFunction(void);
  virtual ~HistogramDensityFunction(void);

  void PrintSelf(std::ostream& os, Indent indent) const;

  HistogramConstPointer      m_Histogram;

} ; // end of class

} // end of namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogramDensityFunction.txx"
#endif

#endif





