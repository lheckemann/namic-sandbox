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

  typedef TTensorPixelType                         TensorPixelType;

  /** Reference image data, S_0. This image is aquired in teh absence 
   * of a diffusion sensitizing field gradient */
  typedef typename Superclass::InputImageType      ReferenceImageType;
  
  typedef Image< TensorPixelType, ImageDimension > TensorImageType;
  
  typedef TensorImageType                          OutputImageType;

  typedef Image< GradientPixelType,ImageDimension> GradientImageType;

  /** Holds the tensor basis coefficients G_k */
  typedef vnl_matrix< double >                     CoefficientMatrixType;

  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >            GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int, 
          GradientDirectionType >                  GradientDirectionContainerType;
  
  
protected:
  SymmetricSecondRankTensorReconstructionImageFilter();
  ~SymmetricSecondRankTensorReconstructionImageFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  
private:
  
  /* Tensor basis coeffs */
  CoefficientMatrixType                             m_TensorBasisCoeffs;

  GradientDirectionContainerType                    m_GradientDirectionContainer;

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

