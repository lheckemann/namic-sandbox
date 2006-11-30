/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMSolverBase.txx,v $
  Language:  C++
  Date:      $Date: 2006/09/08 19:19:14 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SVMSolverBase_txx
#define __SVMSolverBase_txx


namespace itk
{
namespace Statistics
{
/** Constructor */
template<class TVector, class TOutput>
SVMSolverBase<TVector,TOutput>
::
SVMSolverBase()
{
  m_NumClass1        = 0;
  m_NumClass2        = 0;
  m_NumSamples       = 0;
  m_ComputeQMatrix   = true;
  m_ComputeKMatrix   = true;
}


template<class TVector, class TOutput>
SVMSolverBase<TVector,TOutput>
::
~SVMSolverBase()
{
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::Initialize()
{
  m_QMatrix.clear();
  m_KMatrix.clear();
  m_NumClass1 = 0;
  m_NumClass2 = 0;
  m_NumSamples = 0;
  m_ComputeQMatrix   = true;
  m_ComputeKMatrix   = true;
  this->m_SupportVectorInstanceIdentifiers.clear();
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::SetQMatrix(const VnlMatrixType& matrix)
{
  m_QMatrix = matrix;
  m_ComputeQMatrix = false;
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::SetKMatrix(const VnlMatrixType& matrix)
{
  m_KMatrix = matrix;
  m_ComputeKMatrix = false;
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::SetClass1TrainingSamples(const TXSample* samples)
{
  m_NumClass1=samples->Size();
  m_NumSamples+=samples->Size();
  m_C1SamplePointer = samples;
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::SetClass2TrainingSamples(const TXSample* samples)
{
  m_NumClass2=samples->Size();
  m_NumSamples+=samples->Size();
  m_C2SamplePointer = samples;
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::GenerateLabels()
{
  m_Labels.set_size(1,m_NumClass1+m_NumClass2);
  for(unsigned int i=0; i<m_NumClass1; i++)
  {
    m_Labels(0,i)=1;
  }
  for(unsigned int j=0; j<m_NumClass2; j++)
  {
    m_Labels(0,m_NumClass1+j)=-1;
  }
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::ComputeQMatrix()
{
  m_QMatrix.set_size(m_NumSamples, m_NumSamples);
  for(unsigned int i = 0; i<m_NumSamples; i++)
  {
    for(unsigned int j = i; j<m_NumSamples; j++)
    {
     m_QMatrix(i,j) = m_Labels(0,i)*m_Labels(0,j)* m_KMatrix(i,j);
     m_QMatrix(j,i) = m_QMatrix(i,j); 
    }
  }
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::ComputeKMatrix()
{ 
  m_KMatrix.set_size(m_NumSamples, m_NumSamples);
  for(unsigned int i = 0; i<m_NumSamples; i++)
  {
    for(unsigned int j = 0; j<m_NumSamples; j++)
    {
     m_KMatrix(i,j)= m_QMatrix(i,j)*m_Labels(0,i)*m_Labels(0,j) ;
    }
  }
}


template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::ComputeQAndKMatrices()
{ 
  this->m_QMatrix.set_size(m_NumSamples, m_NumSamples);
  this->m_KMatrix.set_size(m_NumSamples, m_NumSamples);
  
  //
  // NB: this required a complete rewrite to compile on visual c++
  // 2005 express.  The problem is that the original code required the
  // assignment of an iterator over one vector to another vector.
  // VSExpress did not like this.  The result is not pretty and will
  // be innificent for large sample sizes because of bad cache
  // locality.
  //
  
  // compute upper left block
  typename TXSample::ConstIterator c1IterI = m_C1SamplePointer->Begin();
  typename TXSample::ConstIterator c1IterJ = m_C1SamplePointer->Begin();
  for (unsigned int i = 0; i < this->m_C1SamplePointer->Size(); ++i)
  {
    c1IterJ = c1IterI;
    for (unsigned int j = i; j < this->m_C1SamplePointer->Size(); ++j)
    {
      double kernelProduct = this->m_SVM->GetKernel()->
        Evaluate(c1IterI.GetMeasurementVector(),
                 c1IterJ.GetMeasurementVector());
      double labeledProduct =
        m_Labels(0,i) * m_Labels(0,j) * kernelProduct;
      this->m_QMatrix(i,j) = labeledProduct;
      this->m_QMatrix(j,i) = labeledProduct;
      this->m_KMatrix(i,j) = kernelProduct;
      this->m_KMatrix(j,i) = kernelProduct;
      
      ++c1IterJ;
    }
    ++c1IterI;
  }

  // compute lower right block
  typename TXSample::ConstIterator c2IterI = m_C2SamplePointer->Begin();
  typename TXSample::ConstIterator c2IterJ = m_C2SamplePointer->Begin();
  unsigned int c1Size = this->m_C1SamplePointer->Size();
  for (unsigned int i = 0; i < this->m_C2SamplePointer->Size(); ++i)
  {
    c2IterJ = c2IterI;
    for (unsigned int j = i; j < this->m_C2SamplePointer->Size(); ++j)
    {
      double kernelProduct = this->m_SVM->GetKernel()->
        Evaluate(c2IterI.GetMeasurementVector(),
                 c2IterJ.GetMeasurementVector());
      double labeledProduct =
        m_Labels(0,i+c1Size) * m_Labels(0,j+c1Size) * kernelProduct;
      this->m_QMatrix(i+c1Size,j+c1Size) = labeledProduct;
      this->m_QMatrix(j+c1Size,i+c1Size) = labeledProduct;
      this->m_KMatrix(i+c1Size,j+c1Size) = kernelProduct;
      this->m_KMatrix(j+c1Size,i+c1Size) = kernelProduct;
      
      ++c2IterJ;
    }
    ++c2IterI;
  }
  
  // compute off diagonal blocks
  c1IterI = m_C1SamplePointer->Begin();
  c2IterJ = m_C2SamplePointer->Begin();
  for (unsigned int i = 0; i < this->m_C1SamplePointer->Size(); ++i)
  {
    c2IterJ = m_C2SamplePointer->Begin();
    for (unsigned int j = 0; j < this->m_C2SamplePointer->Size(); ++j)
    {
      double kernelProduct = this->m_SVM->GetKernel()->
        Evaluate(c1IterI.GetMeasurementVector(),
                 c2IterJ.GetMeasurementVector());
      double labeledProduct =
        m_Labels(0,i) * m_Labels(0,j+c1Size) * kernelProduct;
      this->m_QMatrix(i,j+c1Size) = labeledProduct;
      this->m_KMatrix(i,j+c1Size) = kernelProduct;
      this->m_QMatrix(j+c1Size,i) = labeledProduct;
      this->m_KMatrix(j+c1Size,i) = kernelProduct;
      
      ++c2IterJ;
    }
    ++c1IterI;
  }
}

template<class TXSample, class TYSample>
void SVMSolverBase<TXSample,TYSample>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
} 

} // namespace Statistics
} // namespace itk
#endif

