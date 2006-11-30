/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSupportVectorMachine.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SupportVectorMachine_h
#define __SupportVectorMachine_h

#include "itkDataObject.h"
#include "itkSVMKernelBase.h"
#include "itkRBFSVMKernel.h"
#include <vector>

namespace itk
{
namespace Statistics
{
template<class TMeasurementVector>
class SupportVectorMachine : 
    public DataObject
{
public:
  /** standard ITK typedefs */
  typedef SupportVectorMachine                          Self;
  typedef DataObject                                    Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** standard ITK new and type macros */
  itkTypeMacro(SupportVectorMachine, DataObject);
  itkNewMacro(Self) ;

  /** the type of vector that this svm operates with */
  typedef TMeasurementVector                            MeasurementVectorType;

  /** the kernel associated with this svm */
  typedef SVMKernelBase<MeasurementVectorType,double>   KernelType;

  /** the default kernel for this svm */
  typedef RBFSVMKernel<MeasurementVectorType,double>    DefaultKernelType;

  /** label type for the support vector machine */
  typedef double                                        LabelType;

  /** access the number of support vectors associated with this svm */
  void SetNumberOfSupportVectors(unsigned int n);
  unsigned int GetNumberOfSupportVectors() const;

  /** access the support vectors associated with this svm */
  void SetSupportVector(unsigned int index, const MeasurementVectorType& v);
  const MeasurementVectorType& GetSupportVector(unsigned int index) const;

  /** access the alpha value associated with each support vector */
  void SetAlpha(unsigned int index, double alpha);
  double GetAlpha(unsigned int index) const;

  /** access the label associated with each support vector */
  void SetLabel(unsigned int index, LabelType label);
  LabelType GetLabel(unsigned int index) const;

  /** access the value of B for this linear classifier */
  itkSetMacro(B, double);
  itkGetMacro(B, double);

  /** access MeasurementVectorSize for the SVM    */
  itkGetMacro(MeasurementVectorSize, unsigned int);
   
  /** access the kernel used by this support vector machine */
  itkSetObjectMacro(Kernel, KernelType);
  itkGetObjectMacro(Kernel, KernelType);
  itkGetConstObjectMacro(Kernel, KernelType);
 
  /** use this support vector machine to classify a vector */
  double Classify(const MeasurementVectorType& inputVector) const;

  /** clear support vectors as well as associated parameters */
  void Initialize();

protected:
  SupportVectorMachine();
  virtual ~SupportVectorMachine();

  unsigned int                                     m_MeasurementVectorSize;
  double                                           m_B;
  std::vector<MeasurementVectorType>               m_SupportVectors;
  std::vector<LabelType>                           m_Labels;
  std::vector<double>                              m_Alphas;
  typename KernelType::Pointer                     m_Kernel;

private:
  // purposely not implemented
  SupportVectorMachine(const Self&); 
  // purposely not implemented
  void operator=(const Self&);                    
};

} // end namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkSupportVectorMachine.txx"
#endif

#endif
