/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSupportVectorMachine.txx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SupportVectorMachine_txx
#define __SupportVectorMachine_txx

namespace itk
{
namespace Statistics
{
template<class TMeasurementVector>
SupportVectorMachine<TMeasurementVector>
::SupportVectorMachine()
{
  m_B      = 0.0;
  m_Kernel = DefaultKernelType::New();
  m_MeasurementVectorSize = 0;
}

template<class TMeasurementVector>
SupportVectorMachine<TMeasurementVector>
::~SupportVectorMachine()
{ }

template<class TMeasurementVector>
void
SupportVectorMachine<TMeasurementVector>
::Initialize()
{
  this->m_SupportVectors.clear();
  this->m_Labels.clear();
  this->m_Alphas.clear();
  this->m_B = 0.0;
  this->m_MeasurementVectorSize = 0;
}

template<class TMeasurementVector>
typename SupportVectorMachine<TMeasurementVector>::LabelType
SupportVectorMachine<TMeasurementVector>
::GetLabel(unsigned int index) const
{
  return m_Labels[index];
}

template<class TMeasurementVector>
void
SupportVectorMachine<TMeasurementVector>::
SetLabel(unsigned int index, LabelType label)
{
  m_Labels[index] = label;
  this->Modified();
}

template<class TMeasurementVector>
double
SupportVectorMachine<TMeasurementVector>::
GetAlpha(unsigned int index) const
{
  return m_Alphas[index];
}

template<class TMeasurementVector>
void
SupportVectorMachine<TMeasurementVector>::
SetAlpha(unsigned int index, double alpha)
{
  m_Alphas[index] = alpha;
  this->Modified();
}

template<class TMeasurementVector>
void
SupportVectorMachine<TMeasurementVector>::
SetNumberOfSupportVectors(unsigned int n)
{
  m_SupportVectors.resize(n);
  m_Alphas.resize(n);
  m_Labels.resize(n);
  this->Modified();
}

template<class TMeasurementVector>
unsigned int
SupportVectorMachine<TMeasurementVector>::
GetNumberOfSupportVectors() const
{
  return this->m_SupportVectors.size();
}

template<class TMeasurementVector>
void
SupportVectorMachine<TMeasurementVector>::
SetSupportVector(unsigned int index, 
                 const typename SupportVectorMachine<TMeasurementVector>::
                 MeasurementVectorType & v)
{
  // if this is the first vector then set the measurment vector size
  // to the size of this vector
  if (this->m_MeasurementVectorSize == 0)
  {
    this->m_MeasurementVectorSize = v.Size();
  }
  // don't allow measurement vectors of different sizes
  else if (this->m_MeasurementVectorSize != v.Size())
  {
    itkExceptionMacro(<< "Invalid measurement vector size: " 
                      << "found " << v.Size() << ", expected " 
                      << this->m_MeasurementVectorSize);
  }
  this->m_SupportVectors[index] = v;
}

template<class TMeasurementVector>
const typename SupportVectorMachine<TMeasurementVector>::MeasurementVectorType&
SupportVectorMachine<TMeasurementVector>::
GetSupportVector(unsigned int index) const
{
  return this->m_SupportVectors[index];
}

template<class TMeasurementVector>
double
SupportVectorMachine<TMeasurementVector>
::Classify(const typename SupportVectorMachine<TMeasurementVector>::
           MeasurementVectorType& inputVector) const
{
  double f = 0;

  for(unsigned int i = 0; i < this->m_SupportVectors.size(); ++i) 
  {  
    f += this->m_Alphas[i] * this->m_Labels[i] * this->m_Kernel->
      Evaluate(this->m_SupportVectors[i], inputVector);
  }
  return f - this->m_B;
}

} // namespace Statistics
} // namespace itk
#endif

