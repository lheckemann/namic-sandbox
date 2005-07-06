/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCannySegmentationLevelSetImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/02 17:30:21 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSecondRankTensorReconstructionImageFilter_h_
#define __itkSymmetricSecondRankTensorReconstructionImageFilter_h_

#include "itkImageToImageFilter.h"
#include "vnl/vnl_matrix.h"

namespace itk{


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType=double >
class ITK_EXPORT SymmetricSecondRankTensorReconstructionImageFilter :
  public ImageToImageFilter< Image< TReferenceImagePixelType, 3>, 
                             Image< TTensorPixelType, 3> >
{

public:

  typedef SymmetricSecondRankTensorReconstructionImageFilter Self;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;
  typedef ImageToImageFilter< Image< TReferenceImagePixelType, 3>, 
                         Image< TTensorPixelType, 3> > Superclass;
 
  itkStaticConstMacro( ImageDimension, unsigned int, 3 );

  typedef TReferenceImagePixelType                 ReferencePixelType;

  typedef TGradientImagePixelType                  GradientPixelType;

  typedef SymmetricSecondRankTensor< 
          TTensorPixelType, ImageDimension >       TensorPixelType;

  /** Reference image data, S_0. This image is aquired in teh absence 
   * of a diffusion sensitizing field gradient */
  typedef typename Superclass::InputImageType      ReferenceImageType;
  
  typedef Image< TensorPixelType, ImageDimension > TensorImageType;
  
  typedef TensorImageType                          OutputImageType;

  typedef Image< GradientPixelType,ImageDimension> GradientImageType;

  /** Holds the tensor basis coefficients G_k */
  typedef vnl_matrix_fixed< double, 6, 6 >         TensorBasisMatrixType;
  
  typedef vnl_matrix< double >                     CoefficientMatrixType;

  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >            GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int, 
          GradientDirectionType >                  GradientDirectionContainerType;
  

  /** Set method to add a gradient direction and its corresponding image. */
  void AddGradientImage( const GradientDirectionType &, const GradientImageType *image);

  /** Set method to set the reference image. */
  void SetReferenceImage( const ReferenceImageType *referenceImage )
    {
    this->ProcessObject::SetNthInput( 0, referenceImage );
    }
    
  /** Get reference image */
  virtual ReferenceImageType * GetReferenceImage() const 
  { return ( static_cast< ReferenceImageType *>(this->ProcessObject::GetInput(0)) ); }

  /** Return the gradient image. idx is 0 based */
  virtual GradientImageType * GetGradientImage( unsigned int idx ) const
  {
    if( idx >= m_NumberOfGradientDirections )
      {
      return NULL;
      }
    return ( static_cast< GradientImageType *>(this->ProcessObject::GetInput(idx+1)) ); 
  }

  /** Return the gradient direction. idx is 0 based */
  virtual GradientDirectionType * GetGradientDirection( unsigned int idx) const
    {
    if( idx >= m_NumberOfGradientDirections )
      {
      return NULL;
      }
    return m_GradientDirectionContainer->ElementAt( idx+1 );
    }



  void BeforeThreadedGenerateData();
  void AfterThreadedGenerateData();
  void ThreadedGenerateData( const RegionType &outputRegionForThread, int threadId )
  
protected:
  SymmetricSecondRankTensorReconstructionImageFilter();
  ~SymmetricSecondRankTensorReconstructionImageFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  
private:
  
  /* Tensor basis coeffs */
  TensorBasisMatrixType                             m_TensorBasis;
  
  CoefficientMatrixType                             m_Coeffs;

  

  GradientDirectionContainerType::Pointer           m_GradientDirectionContainer;

  /** Number of gradient measurements */
  unsigned int                                      m_NumberOfGradientDirections;

  /** Threshold on the baseline image data, S_0 */
  ReferencePixelType                                m_Threshold;
  
  

  
  

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymmetricSecondRankTensorReconstructionImageFilter.txx"
#endif

#endif

