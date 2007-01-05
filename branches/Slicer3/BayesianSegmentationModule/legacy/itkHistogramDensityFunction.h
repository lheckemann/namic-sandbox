/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramDensityFunction.h,v $
  Language:  C++
  Date:      $Date: 2005/07/25 16:53:08 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogramDensityFunction_h
#define __itkHistogramDensityFunction_h

#include "itkDensityFunction.h"
#include "itkArray.h"

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
  typedef typename HistogramType::SizeType       HistogramSizeType;

  /** Standard macros */
  itkTypeMacro(HistogramDensityFunction, DensityFunction);
  itkNewMacro(Self);

  typedef TMeasurementVector MeasurementVectorType;
  typedef THistogram HistogramType;

  /** Method to get probability of an instance. The return value is the
   * value of the density function, not probability. */
  virtual double Evaluate(const MeasurementVectorType &measurement) const;
  virtual void SetHistogram(const HistogramType * histogram);
 
  /**  Set/Get the histogram */
  itkGetConstObjectMacro( Histogram, HistogramType );  
 
  /** Set/Get the class number to use from the multi-dimensional Histogram */
  itkSetMacro( Class, unsigned int );
  itkGetMacro( Class, unsigned int );
  
protected:
  HistogramDensityFunction(void);
  virtual ~HistogramDensityFunction(void);

  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef Array< unsigned long > ArrayType;

  HistogramConstPointer      m_Histogram;
  ArrayType                  m_FrequencyPerLabel;

  unsigned int               m_Class;

} ; // end of class

} // end of namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogramDensityFunction.txx"
#endif

#endif





