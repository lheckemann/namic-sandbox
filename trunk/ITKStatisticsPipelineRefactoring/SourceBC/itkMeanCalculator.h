/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:58 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMeanCalculator_h
#define __itkMeanCalculator_h

#include "itkMeanFilter.h"
#include "itkArray.h"

namespace itk { 
namespace Statistics {
  
/** \class MeanCalculator
 * \brief calculates sample mean
 *
 * You plug in the target sample data using SetSample method. Then call
 * the GenerateData method to run the alogithm.
 *
 * The return value that the GetOutput method 
 * \f$ = \frac{1}{n}\sum^{n}_{i=1}x_{i}\f$ where \f$n\f$ is the
 * number of measurement vectors in the target 
 * 
 * Recent API changes:
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained from the input sample.
 * Please use the function GetMeasurementVectorSize() to obtain the length. 
 * The mean output is an Array rather than a Vector.
 */

template< class TSample >
class MeanCalculator :
      public MeanFilter< TSample >
{
public:
  /**Standard class typedefs. */
  typedef MeanCalculator                  Self;
  typedef MeanFilter < TSample >          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Typedef for the mean output */
  typedef typename Superclass::MeasurementVectorType   OutputType;

  /** Returns the mean vector */
  OutputType* GetOutput()
    {
    m_MeanInternal = Superclass::GetMean();
    return &m_MeanInternal;
    }

  /**Standard Macros */
  itkTypeMacro(MeanCalculator, MeanFilter);
  itkNewMacro(Self);

  void SetInputSample( const TSample * sample ) 
    {
    Superclass::SetInput(sample);
    }
protected:
  MeanCalculator(){}
  virtual ~MeanCalculator() {}

private:
  OutputType m_MeanInternal;
}; // end of class
  
} // end of namespace Statistics 
} // end of namespace itk 

#endif
