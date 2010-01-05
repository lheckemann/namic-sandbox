
#ifndef __itkSymRshReconImageFilter_txx
#define __itkSymRshReconImageFilter_txx

#include "itkSymRshReconImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkArray.h"
#include "vnl/vnl_vector.h"
#include "itkProgressReporter.h"

#include <cmath>

namespace itk {

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::SymRshReconImageFilter()
{
  // At least 1 inputs is necessary for a vector image.
  // For images added one at a time we need at least six
  this->SetNumberOfRequiredInputs( 1 );
  m_NumberOfGradientDirections = 0;
  m_NumberOfBaselineImages = 1;
  m_GradientDirectionContainer = NULL;
  m_RshBasis.set_identity();
  m_RshBasisPseudoInverse.set_identity();
  m_ImageMask = 0; //Must be suplied by the user
  
  m_ResidualImage = 0;
  m_CalculateResidualImage = false;
  
  m_BeltramiLambda = 0.0; //By default use no Regularization.
  this->AllowSuperResolutionOff();
}


template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
void
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::BeforeThreadedGenerateData()
{

  itkDebugMacro( "SymRshReconImageFilter::BeforeThreadedGenerateData ")

  // If we have more than 2 inputs, then each input, except the first is a
  // gradient image. The number of gradient images must match the number of
  // gradient directions.
  const unsigned int numberOfInputs = this->GetNumberOfInputs();

  //the number of gradient directions needs to be at least as many as the number of unknowns
  if( not this->GetAllowSuperResolution() && m_NumberOfGradientDirections < NumberOfCoefficients )
  {
    itkExceptionMacro( << "At least " << NumberOfCoefficients
      << " gradient directions are required to estimate " << NumberOfCoefficients
      << " coefficients" );
  }

  // If there is only 1 gradient image, it must be an itk::VectorImage. Otherwise
  // we must have a container of (numberOfInputs-1) itk::Image. Check to make sure
  if ( numberOfInputs == 1 )
  {
    std::string gradientImageClassName(
                                       this->ProcessObject::GetInput(0)->GetNameOfClass());
    if ( strcmp(gradientImageClassName.c_str(),"VectorImage") != 0 )
    {
      itkExceptionMacro( <<
          "There is only one Gradient image. I expect that to be a VectorImage. "
          << "But its of type: " << gradientImageClassName );
    }
  }

  //Make sure the mask image was specified
  if ( ! m_ImageMask )
  {
   itkExceptionMacro( << "You must specifiy a mask Image!");
  }

  /** Setup both fscores Image and residuals Image */
  //Initialize the residualImage if we are going to calculate it.
  if (m_CalculateResidualImage)
  {
    m_ResidualImage = ResidualImageType::New();
    m_ResidualImage->CopyInformation(this->ProcessObject::GetInput(0));
    m_ResidualImage->SetRegions(m_ResidualImage->GetLargestPossibleRegion() );
    m_ResidualImage->SetVectorLength( m_NumberOfGradientDirections );
    
    m_ResidualImage->Allocate();
  }

  ///Setup things needed to make this work...
  this->ComputeRshBasis();

  itkDebugMacro( "SymRshReconImageFilter::BeforeThreadedGenerateData done")

}

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
void
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

  itkDebugMacro( "SymRshReconImageFilter::ThreadedGenerateData Begin")
  
  typename OutputImageType::Pointer outputImage =
      static_cast< OutputImageType * >(this->ProcessObject::GetOutput(0));

  ImageRegionIterator< OutputImageType > oit(outputImage, outputRegionForThread);
  oit.GoToBegin();

  ImageRegionIterator< ResidualImageType > res_iter;
  
  //if we are calculateing the Residual set up an iterator...
  if (m_CalculateResidualImage)
  {
    res_iter = ImageRegionIterator< ResidualImageType >( m_ResidualImage, outputRegionForThread);
    res_iter.GoToBegin();
  }

  //Vecotor to hold S(g) and then to hold S(g) / S0
  vnl_vector<double> Sig(m_NumberOfGradientDirections);

  // Support for progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

  // the Gradients have been specified in a single multi-component image,
  // one iterator will suffice to do the same.

  typedef ImageRegionConstIterator< GradientImagesType > GradientIteratorType;
  typedef typename GradientImagesType::PixelType         GradientVectorType;
  typename GradientImagesType::Pointer gradientImagePointer = NULL;

  // Would have liked a dynamic_cast here, but seems SGI doesn't like it
  gradientImagePointer = static_cast< GradientImagesType * >(
      this->ProcessObject::GetInput(0) );

  GradientIteratorType git(gradientImagePointer, outputRegionForThread );
  git.GoToBegin();

  // Compute the indicies of the baseline images and gradient images
  std::vector<unsigned int> baselineind; // contains the indicies of
                                         // the baseline images
  std::vector<unsigned int> gradientind; // contains the indicies of
                                         // the gradient images

  for(GradientDirectionContainerType::ConstIterator gdcit = this->m_GradientDirectionContainer->Begin();
      gdcit != this->m_GradientDirectionContainer->End(); ++gdcit)
  {
    if(gdcit.Value().one_norm() <= 0.0)
    {
      baselineind.push_back(gdcit.Index());
    }
    else
    {
      gradientind.push_back(gdcit.Index());
    }
  }

  while( !git.IsAtEnd() )
  {

    //grab the whole vector, at this pixel, out of the GradientImage.
    GradientVectorType b = git.Get();

    typename NumericTraits<GradientPixelType>::AccumulateType b0 = NumericTraits<GradientPixelType>::Zero;
    // Average the baseline image pixels
    for(unsigned int i = 0; i < baselineind.size(); ++i)
    {
      b0 += b[baselineind[i]];
    }
    b0 /= this->m_NumberOfBaselineImages;

    //make an empty pixel to hold the results
    OutputPixelType pixValue(0.0);

    typename GradientImagesType::IndexType index = git.GetIndex();
    typename GradientImagesType::PointType inputPoint;
    gradientImagePointer->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_ImageMask->IsInside( inputPoint ) &&
        b0 > NumericTraits<GradientPixelType>::Zero )
    {
      for( unsigned int i = 0; i< m_NumberOfGradientDirections; i++ )
      {
        if( b[gradientind[i]] == 0 )
        {
          Sig[i] = 0;
        }
        else
        {
          Sig[i] = static_cast<double>(b[gradientind[i]]) / static_cast<double>(b0);
        }
        
        if ( vnl_math_isnan(Sig[i]) || vnl_math_isinf(Sig[i]) )
        {
          std::cerr << "gInd    :" << gradientind[i] << std::endl;
          std::cerr << "sig     :" << Sig[i] << std::endl;
          std::cerr << "b       :" << b[i] << std::endl;
          std::cerr << "b[g[i]] :" << static_cast<double>(b[gradientind[i]]) << std::endl;
          std::cerr << "b0      :" << static_cast<double>(b0) << std::endl;
          std::cerr << "        :" << static_cast<double>(b[gradientind[i]]) / static_cast<double>(b0) << std::endl;
          itkExceptionMacro( << "Error in Pixel "<< index);
        }
      }

      ResidualPixelType residual( m_NumberOfGradientDirections );
      pixValue = this->ComputeCoeffsFromSignal( Sig, residual );
      
      if (m_CalculateResidualImage) res_iter.Set(residual);

    }
    progress.CompletedPixel();
    oit.Set( pixValue );
    

    //if we are calculating the Residual set up an iterator...
    if (m_CalculateResidualImage)   ++res_iter;

    ++oit; // Output (reconstructed tensor image) iterator
    ++git; // Gradient  image iterator
  }
  
  itkDebugMacro( "SymRshReconImageFilter::ThreadedGenerateData done")

}


template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
void
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::SetGradientImage( GradientDirectionContainerType *gradientDirection,
                        const GradientImagesType *gradientImage )
{

  this->m_GradientDirectionContainer = gradientDirection;

  unsigned int numImages = gradientDirection->Size();
  this->m_NumberOfBaselineImages = 0;
  for(GradientDirectionContainerType::Iterator it = this->m_GradientDirectionContainer->Begin();
      it != this->m_GradientDirectionContainer->End(); it++)
    {
    if(it.Value().one_norm() <= 0.0)
      {
      this->m_NumberOfBaselineImages++;
      }
    else // Normalize non-zero gradient directions
      {
      it.Value() = it.Value() / it.Value().two_norm();
      }
    }

  if ( this->m_NumberOfBaselineImages == 0 )
    {
    itkExceptionMacro( << "No Baseline Images found!!!" );
    }
  this->m_NumberOfGradientDirections = numImages - this->m_NumberOfBaselineImages;

  // ensure that the gradient image we received has as many components as
  // the number of gradient directions
  if( gradientImage->GetVectorLength() != this->m_NumberOfBaselineImages + this->m_NumberOfGradientDirections )
    {
    itkExceptionMacro( << this->m_NumberOfGradientDirections << " gradients + " << this->m_NumberOfBaselineImages
                       << "baselines = " << this->m_NumberOfGradientDirections + this->m_NumberOfBaselineImages
                       << " directions specified but image has " << gradientImage->GetVectorLength()
      << " components.");
    }

  this->ProcessObject::SetNthInput( 0,
      const_cast< GradientImagesType* >(gradientImage) );
}

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
void
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "m_BeltramiLambda: " << m_BeltramiLambda << std::endl;
  
  os << indent << "m_CalculateResidualImage: " << m_CalculateResidualImage << std::endl;
  if (m_CalculateResidualImage)
  {
    os << indent << "m_ResidualImage: " << std::endl << indent << indent << m_ResidualImage << std::endl;
  }
  
/*
  if (m_RshBasisPseudoInverse)
    os << m_RshBasisPseudoInverse
*/
    

}


//
// Initialize everything
//
template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
          >
void
SymRshReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension, TResidualPercisionType >
::ComputeRshBasis()
{
  ///We need at least as many gradient Directions as coeffs.
  if( not this->GetAllowSuperResolution() && m_NumberOfGradientDirections < NumberOfCoefficients )
  {
    itkExceptionMacro( << "Not enough gradient directions supplied. Need to supply at least " << NumberOfCoefficients );
  }

  m_RshBasis = OutputPixelType::ComputeRshBasis(this->m_GradientDirectionContainer);
  
  //compute the psuedo inverse once!
  if (m_BeltramiLambda == 0)
  {
    vnl_svd<double> solver(m_RshBasis);
    m_RshBasisPseudoInverse = solver.inverse();
  }
  else
  {
    RshBasisMatrixType L2, basis2;
    typename OutputPixelType::LmVector lmVec;

    //Build lambda * L^2 matrix
    L2.set_size( NumberOfCoefficients, NumberOfCoefficients );
    L2.set_identity();

    for(unsigned int j=0;j<NumberOfCoefficients;j++)
    {
      lmVec = OutputPixelType::GetLM(j+1);
      int l = lmVec(0);
      L2[j][j] = m_BeltramiLambda * (l*l) * ( (l+1) * (l+1) );
    }
    basis2 = m_RshBasis.transpose() * m_RshBasis;
    vnl_svd<double> solver( basis2 + L2 );
    m_RshBasisPseudoInverse = solver.inverse() * m_RshBasis.transpose();
  }

}

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
vnl_vector< double >
SymRshReconImageFilter<  TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::ComputeCoeffsFromSignal( vnl_vector< double > signal, ResidualPixelType& residual )
{
  vnl_vector< double > coeffs;
  vnl_vector< double > res;
  coeffs  = m_RshBasisPseudoInverse * signal;
  
  if ( vnl_math_isnan(coeffs[1]) )
    std::cerr << signal << std::endl;
    
  if ( m_CalculateResidualImage )
  { 
    res     = signal - (m_RshBasis * coeffs) ;
  }
  
  if (m_CalculateResidualImage) 
  {
    for (unsigned int i=0;i<m_NumberOfGradientDirections;++i)
    {
      residual[i] = static_cast<typename ResidualPixelType::ValueType>
        (res[i]);
    }
  }  
  
  return coeffs;
}

} //end Itk namespace

#endif
