/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMFileWriter.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkSVMFileWriter_h
#define __itkSVMFileWriter_h

#include <metaTypes.h>
#include <metaUtils.h>
#include <typeinfo>

#include "itkSupportVectorMachine.h"
#include "itkPolynomialSVMKernel.h"

namespace itk
{

template<class TVector, class TOutput>
class SVMFileWriter : public Object
{
public:
  
  /** SmartPointer typedef support */
  typedef SVMFileWriter Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SVMFileWriter,Object);
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);

  typedef Statistics::SupportVectorMachine<TVector>  SVMType;
  typedef typename SVMType::Pointer                  SVMPointer;
 
  typedef typename SVMType::DefaultKernelType        RBFKernelType;
  typedef typename RBFKernelType::Pointer            RBFKernelPointer;
  typedef Statistics::PolynomialSVMKernel<TVector,double> 
                                                     PolynomialKernelType;
  typedef typename PolynomialKernelType::Pointer     PolynomialKernelPointer; 

  struct lines{ std::string name;
                std::string value;
                };
  std::list<lines> m_namevalue;  

  /** Set the filename  */
  itkSetStringMacro(FileName);

  /** Get the filename */
  itkGetStringMacro(FileName);

 /** Set/Get the input transform to write */
  void SetInput(SVMType* svm);
  const SVMType * GetInput() {return *(m_SVM.GetPointer());}

  /** Read SVM */
  void Update(void);
 
 
  SVMType* GetSVM();

  itkSetMacro(WriteASCII, bool);
  itkGetMacro(WriteASCII, bool);
  itkBooleanMacro(WriteASCII);

protected:
  std::ofstream svmOutputfile;
  typedef std::vector<MET_FieldRecordType *> FieldsContainerType;
  FieldsContainerType m_Fields;
  std::string m_FileName;
  SVMFileWriter();
  virtual ~SVMFileWriter();
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:
  SVMPointer    m_SVM;
  bool m_WriteASCII;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkSVMFileWriter.txx"
#endif

#endif 

