/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMSolver1.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SVMSolver1_h
#define __SVMSolver1_h

#include "itkSVMSolverBase.h"
#include "itkSVMpr_loqo.h"
#include <map>

namespace itk
{
namespace Statistics
{
template<class TXSample, class TYSample>
class SVMSolver1 : public SVMSolverBase<TXSample,TYSample>
{
public:

  typedef SVMSolver1 Self;
  typedef SVMSolverBase<TXSample,TYSample> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  itkTypeMacro(SVMSolver1, SVMSolverBase);
  itkNewMacro(Self) ;

  
  typedef typename Superclass::XVectorType XVectorType;
  typedef typename Superclass::YVectorType YVectorType;
  typedef typename Superclass::InternalVectorType InternalVectorType;
  typedef typename Superclass::VnlVectorType VnlVectorType;
  typedef typename Superclass::SVMType SVMType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;
  typedef SVMPrLoqo<XVectorType,YVectorType> QPSolverType;
      
  void Update();

  bool CheckOptimality();

  void PerformShrinking(){}

  void ComputeLambdaEq();

  void AddSupportVectors();

  
  itkSetMacro(C,double);
  itkGetMacro(C,double);
  
  itkSetMacro(Epsilon,double);
  itkGetMacro(Epsilon,double);

  itkSetMacro(Lambda_eq,double);
  itkGetMacro(Lambda_eq,double);

  itkSetMacro(AlphaThreshold,double);
  itkGetMacro(AlphaThreshold,double);

  itkSetMacro(AlphaThresholdMult,double);
  itkGetMacro(AlphaThresholdMult,double);
  
  void ComputeDefaultC();

  VnlMatrixType SelectWorkingSet();


  itkSetMacro(Iterations,long);
  itkGetMacro(Iterations,long);

  itkSetMacro(Margin,double);
  itkGetMacro(Margin,double);

  itkSetMacro(Precision,double);
  itkGetMacro(Precision,double);

  itkSetMacro(SigDigits,double);
  itkGetMacro(SigDigits,double);

protected:

  SVMSolver1();
  ~SVMSolver1();
    
  /** Method to print the object. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:

  //QP Solver parameters
  long                           m_Iterations;  
  double                         m_Margin;
  double                         m_Precision;
  double                         m_SigDigits;
  
  typename QPSolverType::Pointer m_QPSolver;
  double                         m_C;             //alpha upper bound
  VnlVectorType                  m_Alphas;
  double                         m_AlphaThreshold;
  double                         m_AlphaThresholdMult;
  double                         m_Epsilon; // alphas lowe bound
  double                         m_Lambda_eq;
  int                            m_WorkingSetSize;
  VnlVectorType                  m_S;   
  VnlVectorType                  m_G;   // g(alpha) = -1+Q*alpha
  VnlVectorType                  m_W;   //Wi =  yi * gi(Alpha(t))
};

} // end namespace Statistics
} // end namespace itk






#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkSVMSolver1.txx"
#endif

#endif
