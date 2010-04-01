/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRBFSVMKernel.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRBFSVMKernel_h
#define __itkRBFSVMKernel_h

#include "itkSVMKernelBase.h"
#include "itkNumericTraits.h"

#include "itkMeasurementVectorTraits.h"

namespace itk{ 
namespace Statistics{


template<class TVector, class TOutput>
class RBFSVMKernel : public SVMKernelBase<TVector, TOutput>
{
public:
  /** Standard "Self" typedef. */
  typedef RBFSVMKernel Self;
  typedef SVMKernelBase<TVector, TOutput> Superclass;
  typedef SmartPointer< Self > Pointer ; 
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(RBFSVMKernel, SVMKernelBase);

  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;

  /** Evaluates K(x1,x2) */
  TOutput Evaluate(const TVector &x1, const TVector &x2) const ;
  
  itkSetMacro(Gamma,double);
  itkGetConstMacro(Gamma,double);

protected:
  RBFSVMKernel();           // not implemented
  virtual ~RBFSVMKernel();  // not implemented
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:
 double m_Gamma;

} ; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRBFSVMKernel.txx"
#endif

#endif






