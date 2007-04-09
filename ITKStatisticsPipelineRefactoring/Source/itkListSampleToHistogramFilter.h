/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSample.h,v $
  Language:  C++
  Date:      $Date: 2006/02/21 18:53:18 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkListSampleToHistogramFilter_h
#define __itkListSampleToHistogramFilter_h

#include "itkMacro.h"
#include "itkProcessObject.h"
#include "itkMeasurementVectorTraits.h"


namespace itk { 
namespace Statistics {

/** \class ListSampleToHistogramFilter 
 *  \brief Computes the Histogram corresponding to a ListSample.
 *
 * This filter produces as output the histogram corresponding to
 * the values of a ListSample.
 *
 * \sa ListSample, Histogram
 *
 */

template < class TSample, class THistogram >
class ITK_EXPORT ListSampleToHistogramFilter : public ProcessObject
{
public:
  /** Standard class typedefs */
  typedef ListSampleToHistogramFilter   Self;  
  typedef ProcessObject                 Superclass;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer<const Self >     ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(ListSampleToHistogramFilter, ProcessObject);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** MeasurementVector typedef support */ 
  typedef TSample                                     SampleType;
  typedef THistogram                                  HistogramType;
  typedef typename SampleType::MeasurementVectorType  MeasurementVectorType;
  typedef typename MeasurementVectorType::ValueType   MeasurementType;

  // Add concept checking here
  
protected:
  ListSampleToHistogramFilter();
  virtual ~ListSampleToHistogramFilter();

  void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  ListSampleToHistogramFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end of namespace StatisticsNew 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkListSampleToHistogramFilter.txx"
#endif
  
#endif
