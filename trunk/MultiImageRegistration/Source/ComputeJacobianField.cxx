/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ResampleImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/14 12:12:52 $
  Version:   $Revision: 1.32 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImage.h"
#include "itkCompose3DVectorImageFilter.h"
//#include "itkImageRegionIterator.h"
//#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#include "itkBSplineDeformableTransform.h"
#include "itkTransformFactory.h"

#include <string>
#include <sstream>
#include <fstream>

#include <itkDeformationFieldJacobianDeterminantFilter.h>

//Define the global types for image type
//#define PixelType unsigned short // could that be detremined online? 
#define InputPixelType double
#define OutputPixelType double
#define Dimension 3

#include "itkTransformFileReader.h"   
#include "ComputeJacobianFieldCLP.h" 
    
int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  // typedef for transformation types
  typedef itk::Transform< InputPixelType, Dimension >  TransformType;
  typedef itk::BSplineDeformableTransform< InputPixelType, Dimension, 3 >     BSplineTransformType;
                                             
  itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
  BSplineTransformType::Pointer  bsplineTransform = BSplineTransformType::New();
  //std::cout << bsplineTransform->GetTransformTypeAsString().c_str() << std::endl;
  f->RegisterTransform(bsplineTransform->GetTransformTypeAsString().c_str(),
                       bsplineTransform->GetTransformTypeAsString().c_str(),
                       bsplineTransform->GetTransformTypeAsString().c_str(),
                       1,
                       itk::CreateObjectFunction<BSplineTransformType>::New());
  


  typedef BSplineTransformType::ParametersType BSplineParametersType;
  BSplineParametersType bsplineParameters;

  // Typedef for reader
  typedef itk::TransformFileReader    TransformFileReader;

  TransformFileReader::Pointer      transformFileReader = TransformFileReader::New();
  //std::cout << argv[1] << std::endl;
  transformFileReader->SetFileName(transformParametersFile);
  try
  {
     transformFileReader->Update();
  }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while reading files " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef TransformFileReader::TransformListType   TransformListType;
  TransformListType*   transformList = transformFileReader->GetTransformList();
  //  TransformListType::iterator it=transformList->begin();
  TransformType::Pointer   transform;
  
  //BSplineTransformType::Pointer  bsplineTransform = BSplineTransformType::New();
  bsplineTransform->SetFixedParameters(transformList->front()->GetFixedParameters());
  bsplineParameters.set_size(bsplineTransform->GetNumberOfParameters());
  bsplineParameters = transformList->front()->GetParameters();
  bsplineTransform->SetParameters(bsplineParameters);
  bsplineTransform->SetParametersByValue(transformList->front()->GetParameters());
  //    transform = bsplineTransform;
  //bsplineTransform->Update();
  

  //
  //
  //
  //  Compute the Jacobian determinant of the transformation field
  //
  //
  //
  std::cout << "Begin computation ... \n" << std::endl;
  typedef itk::Vector<double, 3> VectorType;
  typedef itk::Image< VectorType, 3> VectorImageType;
  //VectorImageType::Pointer vectorimage = VectorImageType::New();
  //vectorimage[0].SetInputImage(bsplineTransform->GetCoefficientImage()[0]);

  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::Compose3DVectorImageFilter< InputImageType >  ThreeDVecFilterType;
  ThreeDVecFilterType::Pointer ThreeDvecfilter = ThreeDVecFilterType::New();
  
  //cout << bsplineTransform->GetParameters();
  //bsplineTransform->GetCoefficientImage()[0];
  ThreeDvecfilter->SetInput1( bsplineTransform->GetCoefficientImage()[0] );
  ThreeDvecfilter->SetInput2( bsplineTransform->GetCoefficientImage()[1] );
  ThreeDvecfilter->SetInput3( bsplineTransform->GetCoefficientImage()[2] );

  //std::cout << "Begin 3D vec try ... \n" << std::endl;
  try
    {
      ThreeDvecfilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
    
  typedef itk::DeformationFieldJacobianDeterminantFilter< VectorImageType >  DefJacDetFilterType;
  typedef DefJacDetFilterType::OutputImageType  OutputImageType;

  // Connect deformation-to-Jacobian filter
  DefJacDetFilterType::Pointer jacdetfilter = DefJacDetFilterType::New();
  jacdetfilter->SetInput( ThreeDvecfilter->GetOutput());
  //  jacdetfilter->Update();

  // Write the Jacobian detrminant
  //  typedef itk::Image< OutputPixelType, 3 >   OutputType;
  //std::cout << "Prepare writing ... \n" << std::endl;
  typedef itk::Image< float, 3 >   OutputType;
  typedef itk::ImageFileWriter< OutputType >  WriterType;

  // Write Jacobian determinant image.
  WriterType::Pointer writer = WriterType::New();
  std::cout << "Writing into: " << JacobianDeterminantOutputFile << std::endl;
  writer->SetFileName( JacobianDeterminantOutputFile );
  writer->SetInput( jacdetfilter->GetOutput() );
  
  //std::cout << "Before image write try ... \n" << std::endl;
  try
    {
      writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
    }
  
  return EXIT_SUCCESS;
}


