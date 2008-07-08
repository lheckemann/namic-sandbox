
#ifndef __sbiaOdfImageFilter_txx
#define __sbiaOdfImageFilter_txx

#include "sbiaOdfReconImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkArray.h"
#include "vnl/vnl_vector.h"
#include <math.h>

namespace itk {

template< class TReferenceImagePixelType, 
          class TGradientImagePixelType,
          class TOdfPixelType,
    unsigned int NOrder>
OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder >
::OdfReconImageFilter()
{
  // At least 1 inputs is necessary for a vector image.
  // For images added one at a time we need at least six
  this->SetNumberOfRequiredInputs( 1 ); 
  m_NumberOfGradientDirections = 0;
  m_NumberOfBaselineImages = 1;
  m_Threshold = NumericTraits< ReferencePixelType >::min();
  m_GradientImageTypeEnumeration = Else;
  m_GradientDirectionContainer = NULL;
  m_RshBasis.set_identity();
  m_Normalize = true;
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType, unsigned int NOrder >
void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType, NOrder >
::BeforeThreadedGenerateData()
{
  // If we have more than 2 inputs, then each input, except the first is a 
  // gradient image. The number of gradient images must match the number of
  // gradient directions.
  const unsigned int numberOfInputs = this->GetNumberOfInputs();

  // There need to be at least 6 gradient directions to be able to compute the 
  // tensor basis
  if( m_NumberOfGradientDirections < 6 )
  {
    itkExceptionMacro( << "At least 6 gradient directions are required" );
  }
    
  // If there is only 1 gradient image, it must be an itk::VectorImage. Otherwise 
  // we must have a container of (numberOfInputs-1) itk::Image. Check to make sure
  if ( numberOfInputs == 1
       && m_GradientImageTypeEnumeration != GradientIsInASingleImage )
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
    ///Setup things needed to make this work...
  this->ComputeRshBasis();
}


// POTENTIAL WARNING:
//
// Until we fix netlib svd routines, we will need to set the number of thread
// to 1.
template< class TReferenceImagePixelType, 
          class TGradientImagePixelType,
          class TOdfPixelType,
    unsigned int NOrder >
void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int ) 
{

  typename OutputImageType::Pointer outputImage = 
      static_cast< OutputImageType * >(this->ProcessObject::GetOutput(0));
  
  ImageRegionIterator< OutputImageType > oit(outputImage, outputRegionForThread);
  oit.GoToBegin();

  //Vecotor to hold S(g) and then to hold S(g) / S0
  vnl_vector<double> Sig(m_NumberOfGradientDirections);
  
  //Setup the psuedoInverseSolver.
  //vnl_svd< double > pseudoInverseSolver( m_RshBasis_Sqr );
  vnl_svd< double > pseudoInverseSolver( m_RshBasis );
  
  vnl_vector< double > dwiRshCoeffs;
  
  // Two cases here .
  // 1. If the Gradients have been specified in multiple images, we will create
  // 'n' iterators for each of the gradient images and solve the Stejskal-Tanner
  // equations for every pixel. 
  // 2. If the Gradients have been specified in a single multi-component image,
  // one iterator will suffice to do the same.

  if( m_GradientImageTypeEnumeration == GradientIsInManyImages )
  {

    ImageRegionConstIterator< ReferenceImageType > 
        it(static_cast< ReferenceImageType * >(this->ProcessObject::GetInput(0)), 
           outputRegionForThread);
    it.GoToBegin();

    typedef ImageRegionConstIterator< GradientImageType > GradientIteratorType;
    std::vector< GradientIteratorType * > gradientItContainer;
    
    for( unsigned int i = 1; i<= m_NumberOfGradientDirections; i++ )
    {
      typename GradientImageType::Pointer gradientImagePointer = NULL;
      
      // Would have liked a dynamic_cast here, but seems SGI doesn't like it
      // The enum will ensure that an inappropriate cast is not done
      gradientImagePointer = static_cast< GradientImageType * >( 
          this->ProcessObject::GetInput(i) );
      
      GradientIteratorType *git = new GradientIteratorType( 
          gradientImagePointer, outputRegionForThread );
      git->GoToBegin();
      gradientItContainer.push_back(git);
    }
    
    // Iterate over the reference and gradient images and solve the steskal
    // equations to reconstruct the Diffusion tensor.
    // See splweb.bwh.harvard.edu:8000/pages/papers/westin/ISMRM2002.pdf
    // "A Dual Tensor Basis Solution to the Stejskal-Tanner Equations for DT-MRI"

    while( !it.IsAtEnd() )
    {

      ReferencePixelType b0 = it.Get();
      OdfPixelType Odf(-0.0);
      if( (b0 != 0) && (b0 >= m_Threshold) )
      {
        for( unsigned int i = 0; i< m_NumberOfGradientDirections; i++ )
        {
          GradientPixelType b = gradientItContainer[i]->Get();
          
          if( b == 0 )
          {
            Sig[i] = 0;
          }
          else
          {
            Sig[i] = static_cast<double>(b) / static_cast<double>(b0);
          }
        
          ++(*gradientItContainer[i]);  
        }
        
        dwiRshCoeffs = pseudoInverseSolver.solve( Sig );
        Odf.InitFromDwiCoeffs(dwiRshCoeffs);
        
        if (m_Normalize)
          Odf.Normalize();

      }
      else
      {
        for( unsigned int i = 0; i< m_NumberOfGradientDirections; i++ )
        {
          ++(*gradientItContainer[i]);  
        }
      }
        
      oit.Set( Odf );
      ++oit;
      ++it;
    }

    for( unsigned int i = 0; i< gradientItContainer.size(); i++ )
    {
      delete gradientItContainer[i];
    }
  
  }
  // The gradients are specified in a single multi-component image
  else if( m_GradientImageTypeEnumeration == GradientIsInASingleImage )
  {
    typedef ImageRegionConstIterator< GradientImagesType > GradientIteratorType;
    typedef typename GradientImagesType::PixelType         GradientVectorType;
    typename GradientImagesType::Pointer gradientImagePointer = NULL;
    
    // Would have liked a dynamic_cast here, but seems SGI doesn't like it
    // The enum will ensure that an inappropriate cast is not done
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

      typename NumericTraits<ReferencePixelType>::AccumulateType b0 = NumericTraits<ReferencePixelType>::Zero;

      // Average the baseline image pixels
      for(unsigned int i = 0; i < baselineind.size(); ++i)
      {
        b0 += b[baselineind[i]];
      }
      b0 /= this->m_NumberOfBaselineImages;

      //Make an ODF to hold the results
      OdfPixelType Odf(0.0);

      if( (b0 != 0) && (b0 >= m_Threshold) )
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
        }
        
        dwiRshCoeffs = pseudoInverseSolver.solve( Sig );
        Odf.InitFromDwiCoeffs(dwiRshCoeffs);

        if ( m_Normalize )
          Odf.Normalize();
      }
      
      oit.Set( Odf );
      ++oit; // Output (reconstructed tensor image) iterator
      ++git; // Gradient  image iterator
    }
  }

}

//
// Initialize everything
//
template< class TReferenceImagePixelType, 
  class TGradientImagePixelType,
  class TOdfPixelType,
  unsigned int NOrder >
  void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder>
  ::ComputeRshBasis()
{
  ///We need at least as many gradient Directions as coeffs.
  if( m_NumberOfGradientDirections < NumberOfCoeffients )
  {
    itkExceptionMacro( << "Not enough gradient directions supplied. Need to supply at least " << NumberOfCoeffients );
  }

  // This is only important if we are using a vector image.  For
  // images added one at a time, this is not needed but doesn't hurt.
  std::vector<unsigned int> gradientind;
  for(GradientDirectionContainerType::ConstIterator gdcit = this->m_GradientDirectionContainer->Begin();
      gdcit != this->m_GradientDirectionContainer->End(); ++gdcit)
  {
    if(gdcit.Value().one_norm() > 0.0)
    {
      gradientind.push_back(gdcit.Index());
    }
  }
  
  m_RshBasis.set_size( m_NumberOfGradientDirections, NumberOfCoeffients );

  for (unsigned int m = 0; m < m_NumberOfGradientDirections; m++)
  {
    /*** Grad directions relation to theta phi
    * x = sin(theta) * cos(phi)
    * y = sin(theta) * sin(phi)
    * z = cos(theta)
    */
    
    double theta, phi;
    
    if ( m_GradientDirectionContainer->ElementAt(gradientind[m])[2] == 1) // z = 1
    {
      theta =0.0;
      phi   =0.0;
    }
    else
    {
      theta = acos(m_GradientDirectionContainer->ElementAt(gradientind[m])[2]);
      phi   = atan2(m_GradientDirectionContainer->ElementAt(gradientind[m])[1],
                    m_GradientDirectionContainer->ElementAt(gradientind[m])[0]); // atan2(y,x) = atan(y/x);
    }
    
    for (unsigned int c = 0; c < NumberOfCoeffients; c++)
      m_RshBasis[m][c]  = OdfPixelType::Y(c+1,theta,phi);
  
  }
  std::cerr << "{" << std::endl;
  for (unsigned int m = 0; m < m_NumberOfGradientDirections; m++)
  {
    std::cerr << "{ ";
    for (unsigned int c = 0; c < NumberOfCoeffients-1; c++)
      std::cerr <<  m_RshBasis[m][c] <<", ";

    std::cerr <<  m_RshBasis[m][NumberOfCoeffients-1] << "}, "<<std::endl;

  }
  std::cerr << " };" << std::endl;
}

//
// Methods for adding images to the filter....
//
template< class TReferenceImagePixelType, 
          class TGradientImagePixelType,
          class TOdfPixelType,
    unsigned int NOrder >
void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder>
::AddGradientImage( const GradientDirectionType &gradientDirection, 
                        const GradientImageType *gradientImage )
{
  // Make sure crazy users did not call both AddGradientImage and 
  // SetGradientImage
  if( m_GradientImageTypeEnumeration == GradientIsInASingleImage)
    {
    itkExceptionMacro( << "Cannot call both methods:" 
    << "AddGradientImage and SetGradientImage. Please call only one of them.");
    }

  // If the container to hold the gradient directions hasn't been allocated
  // yet, allocate it.
  if( !this->m_GradientDirectionContainer )
    {
    this->m_GradientDirectionContainer = GradientDirectionContainerType::New();
    }
    
  m_GradientDirectionContainer->InsertElement( 
              m_NumberOfGradientDirections, gradientDirection / gradientDirection.two_norm() );
  ++m_NumberOfGradientDirections;
  this->ProcessObject::SetNthInput( m_NumberOfGradientDirections, 
      const_cast< GradientImageType* >(gradientImage) );
  m_GradientImageTypeEnumeration = GradientIsInManyImages;
}

template< class TReferenceImagePixelType, 
          class TGradientImagePixelType,
          class TOdfPixelType,
    unsigned int NOrder >
void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder>
::SetGradientImage( GradientDirectionContainerType *gradientDirection, 
                        const GradientImagesType *gradientImage )
{
  // Make sure crazy users did not call both AddGradientImage and 
  // SetGradientImage
  if( m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    itkExceptionMacro( << "Cannot call both methods:" 
    << "AddGradientImage and SetGradientImage. Please call only one of them.");
    }

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
  m_GradientImageTypeEnumeration = GradientIsInASingleImage;
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType,
          class TOdfPixelType,
    unsigned int NOrder >
void OdfReconImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TOdfPixelType, NOrder>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
///TODO FIX ME!!!!!!
/*  os << indent << "TensorBasisMatrix: " << m_TensorBasis << std::endl;
  os << indent << "Coeffs: " << m_BMatrix << std::endl;
  if ( m_GradientDirectionContainer )
    {
    os << indent << "GradientDirectionContainer: "
       << m_GradientDirectionContainer << std::endl;
    }
  else
    {
    os << indent << 
    "GradientDirectionContainer: (Gradient directions not set)" << std::endl;
    }
  os << indent << "NumberOfGradientDirections: " << 
              m_NumberOfGradientDirections << std::endl;
  os << indent << "NumberOfBaselineImages: " << 
              m_NumberOfBaselineImages << std::endl;
  os << indent << "Threshold for reference B0 image: " << m_Threshold << std::endl;
  os << indent << "BValue: " << m_BValue << std::endl;
  if ( this->m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    os << indent << "Gradient images haven been supplied " << std::endl;
    }
  else if ( this->m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    os << indent << "A multicomponent gradient image has been supplied" << std::endl;
    }
*/
}

}

#endif
