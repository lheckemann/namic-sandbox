/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkListSampleToHistogramFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:53 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkListSampleToHistogramFilter_h
#define __itkListSampleToHistogramFilter_h

#include "itkSampleToHistogramFilter.h"

namespace itk {
namespace Statistics {

/** \class ListSampleToHistogramFilter
 *  \brief Imports data from ListSample object to Histogram object
 *
 * Before beginning import process, users should prepare the Histogram
 * object by calling histogram object's Initialize(Size),
 * SetBinMin(dimension, n), and SetBinMax(dimension, n) methods.
 *
 * To do: selective importing for subset of feature vector dimensions
 */
template< class TListSample, class THistogram >
class ITK_EXPORT ListSampleToHistogramFilter :
    public SampleToHistogramFilter<TListSample, THistogram>
{
public:
  /** Standard typedefs */
  typedef ListSampleToHistogramFilter                      Self;
  typedef SampleToHistogramFilter<TListSample, THistogram> Superclass;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;
  
  typedef typename Superclass::HistogramType HistogramType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ListSampleToHistogramFilter, SampleToHistogramFilter);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** plug in the ListSample object */
  void SetListSample(const TListSample* list)
    {
    Superclass::SetInput(list);
    }

  /************ NEEDS MORE WORK */
  void SetHistogram(HistogramType * histogram)
    { m_Histogram = histogram;  }

  void GenerateData()
    {
    this->SetHistogramSize(m_Histogram->GetSize());
    Superclass::GenerateData();
    HistogramType * outputHistogram = 
      static_cast<HistogramType*>(this->ProcessObject::GetOutput(0));
    m_Histogram->Graft(outputHistogram);
    }
protected:
  ListSampleToHistogramFilter(){}
  virtual ~ListSampleToHistogramFilter() {}

private:
  HistogramType *  m_Histogram;

}; // end of class

} // end of namespace Statistics 
} // end of namespace itk 

#endif
