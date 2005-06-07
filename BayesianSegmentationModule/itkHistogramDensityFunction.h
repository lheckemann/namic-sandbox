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

#include "itkMembershipFunction.h"

namespace itk{ 
namespace Statistics{

/** \class HistogramDensityFunction
 * \brief HistogramDensityFunction class defines common interfaces for 
 * density functions.
 *
 * The Evaluate method returns density value for the input measurement vector
 */

template< class TMeasurementVector >
class ITK_EXPORT HistogramDensityFunction :
    public MembershipFunction< TMeasurementVector >
{
public:
  /** Standard class typedefs */
  typedef HistogramDensityFunction Self;
  typedef MembershipFunctionBase< TMeasurementVector > Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Strandard macros */
  itkTypeMacro(HistogramDensityFunction, MembershipFunctionBase);

  /** Method to get probability of an instance. The return value is the
   * value of the density function, not probability. */
  virtual double Evaluate(const TMeasurementVector &measurement) const;
  
protected:
  HistogramDensityFunction(void) {}
  virtual ~HistogramDensityFunction(void) {}

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent) ; }
} ; // end of class

} // end of namespace Statistics
} // end namespace itk

#endif





