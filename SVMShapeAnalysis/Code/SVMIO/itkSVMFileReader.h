/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMFileReader.h,v $
  Language:  C++
  Date:      $Date: 2006/09/07 20:14:55 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkSVMFileReader_h
#define __itkSVMFileReader_h

#include <metaTypes.h>
#include <metaUtils.h>

#include "itkSupportVectorMachine.h"
#include "itkPolynomialSVMKernel.h"
#include "itkLinearSVMKernel.h"

namespace itk
{

template<class TVector, class TOutput>
class SVMFileReader : public Object
{
public:
  
  /** SmartPointer typedef support */
  typedef SVMFileReader Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SVMFileReader,Object);
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);

  typedef Statistics::SupportVectorMachine<TVector>  SVMType;
  typedef typename SVMType::Pointer                  SVMPointer;

  /** Set the filename  */
  itkSetStringMacro(FileName);

  /** Get the filename */
  itkGetStringMacro(FileName);

  /** Read SVM */
  void Update(void);

  /** return true if measurement vector is not fixed array */
  bool MeasurementVectorResizable();
 
  SVMType* GetSVM();

  void  M_SetupReadFields(void);

  itkNotUsed(itkSetMacro(ReadWeightValues, unsigned int););
  itkNotUsed(itkGetMacro(ReadWeightValues, unsigned int););
   
  // Read binary of ASCII weights
  // Binary =1
  // ASCII = 2
  void ReadValues(int type){};

protected:

  typedef std::vector<MET_FieldRecordType *> FieldsContainerType;
  FieldsContainerType m_Fields;
  std::string m_FileName;
  SVMFileReader();
  virtual ~SVMFileReader();
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;
  
private:
  SVMPointer    m_SVM;
  itkNotUsed(int m_ReadWeightValues;);
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkSVMFileReader.txx"
#endif

#endif 

