/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryDilateImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSecondRankTensorReconstructionImageFilter_txx
#define __itkSymmetricSecondRankTensorReconstructionImageFilter_txx

#include "itkSymmetricSecondRankTensorReconstructionImageFilter.h"

namespace itk {

template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::SymmetricSecondRankTensorReconstructionImageFilter()
{
  this->SetNumberOfRequiredInputs( 6 ); // At least 6 inputs are necessary

  m_GradientDirectionContainer = GradientDirectionContainerType::New();

  m_NumberOfGradientDirections = 0;

  m_Threshold = NumericTraits< ReferencePixelType >::min();
 
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::~SymmetricSecondRankTensorReconstructionImageFilter()
{
  
}




template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::BeforeThreadedGenerateData()
{
  int numberOfThreads = this->GetNumberOfThreads();
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::AfterThreadedGenerateData()
{

}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::ThreadedGenerateData(const RegionType& outputRegionForThread,
                       int threadId) 
{

  ImageRegionConstIteratorWithIndex< ReferenceImageType > 
                  it(this->GetInput(0), outputRegionForThread);
  it.GoToBegin();
  
  ReferenceImageType::IndexType index;

  typedef Array< ReferenceImageType > ImageArrayType;
  ImageArrayType imageArray( m_NumberOfGradientDirections );

  OutputImageType::Pointer outputImage = this->ProcessObject::GetOutput(0);
  
  vnl_vector< double > b(m_NumberOfGradientDirections);
  
  for( unsigned int i = 0; i< m_NumberOfGradientDirections; i++ )
    {
    imageArray[i] = this->ProcessObject::GetInput(i);
    }
    
  while( !it.IsAtEnd() )
    {
    ReferencePixelType b0 = it.Get();
    index = it.GetIndex();
    
    for( unsigned int i = 0; i< m_NumberOfGradientDirections; i++ )
      {
      b = imageArray[i]->GetPixel( index );
      if( (b == 0) || (b0 == 0) )
        {
        B[i] = 0;
        }
      else
        {
        B[i] = -log( b / b0 );
        }
      }

    vnl_vector< double > D(6);
    vnl_svd< double > pseudoInverseSolver( m_TensorBasis );
    if( m_NumberOfGradientDirections > 6 )
      {
      D = pseudoInverseSolver.solve( m_Coeffs * B );
      }
    else
      {
      D = pseudoInverseSolver.solve( B );
      }

    TensorPixelType tensor;
    tensor(0,0) = D[0];
    tensor(0,1) = D[1]; 
    tensor(0,2) = D[2];
    tensor(1,1) = D[3]; 
    tensor(1,2) = D[4];
    tensor(2,2) = D[5];
    
    outputImage->SetPixel( index, tensor );
    }
    
}



template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::ComputeTensorBasis()
{
  if( m_NumberOfGradientDirections < 6 )
    {
    itkExceptionMacro( << "Not enough gradient directions supplied. Need to supply at least 6" );
    }

  CoefficientMatrixType m_Coeffs(m_NumberOfGradientDirections, 6);
  for (int m = 0; m < m_NumberOfGradientDirections; m++)
    {
    m_Coeffs[m][0] =     m_GradientDirectionContainer->ElementAt(m)[0] * m_GradientDirectionContainer->ElementAt(m)[0];
    m_Coeffs[m][1] = 2 * m_GradientDirectionContainer->ElementAt(m)[0] * m_GradientDirectionContainer->ElementAt(m)[1];
    m_Coeffs[m][2] = 2 * m_GradientDirectionContainer->ElementAt(m)[0] * m_GradientDirectionContainer->ElementAt(m)[2];
    m_Coeffs[m][3] =     m_GradientDirectionContainer->ElementAt(m)[1] * m_GradientDirectionContainer->ElementAt(m)[1];
    m_Coeffs[m][4] = 2 * m_GradientDirectionContainer->ElementAt(m)[1] * m_GradientDirectionContainer->ElementAt(m)[2];
    m_Coeffs[m][5] =     m_GradientDirectionContainer->ElementAt(m)[2] * m_GradientDirectionContainer->ElementAt(m)[2];
    }
 
  if( m_NumberOfGradientDirections > 6 )
    {
    m_TensorBasis = m_Coeffs.transpose() * m_Coeffs;
    }
  else
    {
    m_TensorBasis = m_Coeffs;
    }

  m_Coeffs.inplace_transpose();
    
}



template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::AddGradientImage( const GradientDirectionType &gradientDirection, 
                        const GradientImageType *gradientImage )
{
  m_GradientDirectionContainer->InsertElement( 
              m_NumberOfGradientDirections, gradientDirection );
  ++m_NumberOfGradientDirections;
  this->ProcessObject::SetNthInput( m_NumberOfGradientDirections, 
      const_cast< GradientImageType* >(gradientImage) );
  
}



template <class TImage>
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "TensorBasisMatrix: " << m_TensorBasis << std::endl;
  os << indent << "Coeffs: " << m_Coeffs << std::endl;
  os << indent << "GradientDirectionContainer: "
     << m_GradientDirectionContainer << std::endl;
  os << indent << "NumberOfGradientDirections: " << 
              m_NumberOfGradientDirections << std::endl;
  os << indent << "Threshold for reference B0 image: " << m_Threshold << std::endl;
}









}

#endif
