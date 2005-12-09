/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SimpleAppOutput.txx,v $
  Language:  C++
  Date:      $Date: 2004/02/06 22:38:15 $
  Version:   $Revision: 1.5 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _SimpleAppOutput_txx
#define _SimpleAppOutput_txx

#include "SimpleAppOutput.h"

#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkExceptionObject.h"

#include "itkImageFileWriter.h"

#include <iostream>
#include <fstream>

namespace itk
{

template <typename TImage>
SimpleAppOutput<TImage>
::SimpleAppOutput()
{
  m_FixedImage = NULL;
  m_MovingImage = NULL;
  m_ResampledImage = NULL;

  m_Transform = NULL;

  m_OutputFileName = "";
  m_TransformOutputFileName = "";
}


template <typename TImage>
void
SimpleAppOutput<TImage>
::Execute()
{

  if ( !m_MovingImage || !m_FixedImage || !m_Transform ||
    m_OutputFileName == "" )
    {
    ExceptionObject err(__FILE__, __LINE__);
    err.SetLocation( "Execute()" );
    err.SetDescription( "Not all the inputs are valid." );
    throw err;
    }

  // set up the resampler
  typedef typename AffineTransformType::ScalarType CoordRepType;
  typedef itk::LinearInterpolateImageFunction<ImageType,CoordRepType> 
    InterpolatorType;
  typedef itk::ResampleImageFilter<ImageType,ImageType> ResamplerType;

  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

  typename ResamplerType::Pointer resampler = ResamplerType::New();
  resampler->SetInput( m_MovingImage );

  resampler->SetTransform( m_Transform.GetPointer() );
  resampler->SetInterpolator( interpolator.GetPointer() );
  resampler->SetSize( m_FixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin( m_FixedImage->GetOrigin() );
  resampler->SetOutputSpacing( m_FixedImage->GetSpacing() );
  resampler->SetDefaultPixelValue( 0 );

  // Write out a copy of the transform to disk.
  std::ofstream ofile(m_TransformOutputFileName.c_str());
  ofile << "parameters : ";
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ofile << (m_Transform->GetMatrix().GetVnlMatrix()(i,j)) << " " ;
    }
  }
  for (int i = 0; i < 3; i++) {
    ofile << m_Transform->GetOffset()[i] << " " ;
  }
  ofile << std::endl;
  ofile.close();


  // resample the moving image
  resampler->Update();

  m_ResampledImage = resampler->GetOutput();


  // write out resampled image
  typedef ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer resampleMovingwriter = WriterType::New();

  try
    {
    resampleMovingwriter->SetInput( m_ResampledImage );
    resampleMovingwriter->SetFileName( m_OutputFileName.c_str() );
    resampleMovingwriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
     std::cerr << "Error occured while writing output files." << std::endl;
     std::cerr << excp << std::endl;              
     throw;
    }

}


} // namespace itk

#endif
