/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMKernelBase.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _itkSVMKernelBase_h
#define _itkSVMKernelBase_h

#include "itkFunctionBase.h"

namespace itk
{
namespace Statistics
{

template<class TVector, class TOutput>
class SVMKernelBase : public FunctionBase<TVector, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef SVMKernelBase Self;
  typedef FunctionBase<TVector, TOutput> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SVMKernelBase, FunctionBase);

  /** Output type */
  typedef TOutput OutputType;

  /** get the value of K(x1,x2) */
  virtual OutputType Evaluate(const TVector& x1,const TVector& x2 ) const = 0;
  
protected:
  SVMKernelBase(){};
  virtual ~SVMKernelBase(){}; 

  /** hack to make this class derive from FunctionBase.  That is, it
   *   does not make any sense to say K(x).
   */
  OutputType Evaluate(const TVector& x1) const { return 0; }

private:
  SVMKernelBase(const Self&);  //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk
} // end namespace Statistics
#endif
