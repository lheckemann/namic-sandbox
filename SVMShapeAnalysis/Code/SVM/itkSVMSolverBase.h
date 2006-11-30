/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMSolverBase.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SVMSolverBase_h
#define __SVMSolverBase_h

#include "itkLightProcessObject.h"
#include <vector>

//#define ITK_SVM_DEBUG_OUTPUT

namespace itk
{
namespace Statistics
{
template<class TXSample, class TYSample>
class SVMSolverBase : public LightProcessObject
{
public:

  typedef SVMSolverBase Self;
  typedef LightProcessObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  itkTypeMacro(SVMSolverBase, LightProcessObject);

  typedef typename TXSample::MeasurementVectorType XVectorType;
  typedef typename TYSample::MeasurementVectorType YVectorType;
  typedef typename TXSample::ConstPointer InputSamplePointerType;

  typedef Array<double> InternalVectorType;
  typedef std::vector<XVectorType> InputSampleVectorType;
  typedef std::vector<YVectorType> OutputSampleVectorType;
  typedef SupportVectorMachine<XVectorType> SVMType;

  typedef vnl_vector<double> VnlVectorType;
  typedef vnl_matrix<double> VnlMatrixType;
  
  void SetClass1TrainingSamples(const TXSample* samples);
  void SetClass2TrainingSamples(const TXSample* samples);
  void GenerateLabels();

  void Initialize();

  virtual void Update() = 0;

  virtual bool CheckOptimality() = 0;

  itkSetMacro(NumSamples,long);
  itkGetMacro(NumSamples,long);

  itkSetObjectMacro(SVM, SVMType);
  itkGetObjectMacro(SVM, SVMType);
 
  void SetQMatrix(const VnlMatrixType& matrix);

  void SetKMatrix(const VnlMatrixType& matrix);
  
  unsigned int GetSupportVectorInstanceIdentifier(unsigned int svIndex)
  {
    return m_SupportVectorInstanceIdentifiers[svIndex];
  }

protected:

  SVMSolverBase();
  
  ~SVMSolverBase();
  
  /** Method to print the object. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

  /** compute Q and K matrices if they were not set by the user */
  void ComputeQAndKMatrices();
  void ComputeQMatrix();
  void ComputeKMatrix();

  /** allow subclasses access to flags */
  itkGetMacro(ComputeKMatrix, bool);
  itkGetMacro(ComputeQMatrix, bool);

  InputSamplePointerType    m_C1SamplePointer;
  InputSamplePointerType    m_C2SamplePointer;
 
  InputSampleVectorType     m_InputSamples;   
  OutputSampleVectorType    m_Targets;
    
  vnl_matrix<double>        m_Labels;
  long m_NumSamples;
  VnlMatrixType             m_QMatrix;  //hessian matrix
  VnlMatrixType             m_KMatrix;  //inner products

  unsigned int                             m_NumClass1;
  unsigned int                             m_NumClass2;

  typename SVMType::Pointer                m_SVM;
  bool                                     m_ComputeKMatrix;
  bool                                     m_ComputeQMatrix;

  std::vector<unsigned int>                m_SupportVectorInstanceIdentifiers;

};

} // end namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkSVMSolverBase.txx"
#endif

#endif
