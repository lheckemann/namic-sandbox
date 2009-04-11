/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVariableDimensionHistogram.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 20:26:56 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVariableDimensionHistogram_h
#define __itkVariableDimensionHistogram_h

#include "itkHistogram.h"

namespace itk {
namespace Statistics {

/** \class VariableDimensionHistogram 
 *  \brief  This class is similar to the Histogram class. It 
 *  however allows you to specify the histogram dimension at run time.
 *  (and is therefore not templated over the size of a measurement
 *  vector). Users who know that the length of a measurement vector 
 *  will be fixed, for instance joint statistics on pixel values of
 *  2 images, (where the dimension will be 2), etc should use 
 *  the Histogram class instead.
 *  
 * This class stores measurement vectors in the context of 
 * n-dimensional histogram.
 *
 * VariableDimensionHistogram represents an ND histogram.  Histogram bins can be
 * regularly or irregularly spaced. The storage for the histogram is
 * managed via the FrequencyContainer specified by the template
 * argument.  The default frequency container is a
 * DenseFrequencyContainer. A SparseFrequencyContainer can be used as
 * an alternative.
 *
 * Frequencies of a bin (SetFrequency(), IncreaseFrequency()) can be
 * specified by measurement, index, or instance identifier.
 *
 * Measurements can be queried by bin index or instance
 * identifier. In this case, the measurement returned in the centroid
 * of the histogram bin.
 *
 * The Initialize() method is used to specified the number of bins for
 * each dimension of the histogram. An overloaded version also allows
 * for regularly spaced bins to defined.  To define irregularly sized
 * bins, use the SetBinMin()/SetBinMax() methods.
 *
 * \sa Histogram, Sample, DenseFrequencyContainer, SparseFrequencyContainer
 */
  
template < class TMeasurement = float,
           class TFrequencyContainer = DenseFrequencyContainer2 >
class ITK_EXPORT VariableDimensionHistogram 
  : public Histogram < TMeasurement, TFrequencyContainer >
{
public:

  /** Standard typedefs */
  typedef VariableDimensionHistogram                     Self;
  typedef Histogram< TMeasurement , TFrequencyContainer> Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VariableDimensionHistogram, Histogram);

  /** standard New() method support */
  itkNewMacro(Self);

protected:
  VariableDimensionHistogram() {}
  virtual ~VariableDimensionHistogram() {}

private:
  VariableDimensionHistogram(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end of namespace Statistics 
} // end of namespace itk 

#endif
