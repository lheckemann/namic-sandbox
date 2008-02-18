/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/22 00:30:16 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"

#include "itkTimeProbesCollectorBase.h"

#include "itkBSplineDeformableTransform.h"
#include "itkBSplineDeformableTransformInverseCalculator.h"


int main( int argc, char *argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " gridSize";
    return EXIT_FAILURE;
    }
  
  const unsigned int SpaceDimension = 3;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;

  typedef itk::BSplineDeformableTransformInverseCalculator<
                            TransformType, 
                            TransformType >   CalculatorType;


  TransformType::Pointer  directTransform  = TransformType::New();
  TransformType::Pointer  inverseTransform = TransformType::New();

  CalculatorType::Pointer inverseCalculator = CalculatorType::New();

  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;

  RegionType::SizeType   gridSizeOnImage;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;

  gridSizeOnImage[0] = 32;
  gridSizeOnImage[1] = 32;
  gridSizeOnImage[2] = 16;

  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef TransformType::SpacingType SpacingType;
  SpacingType spacing;

  spacing[0] = 1.5;
  spacing[1] = 1.5;
  spacing[2] = 1.5;

  typedef TransformType::OriginType OriginType;
  OriginType origin;

  origin[0] = 17.0;
  origin[1] = 19.0;
  origin[2] = 23.0;


  typedef TransformType::ParametersType     ParametersType;

  itk::TimeProbesCollectorBase collector;

  std::cout << std::endl << "Starting Benchmark" << std::endl;

  TransformType::ParametersType parameters;

  directTransform->SetGridSpacing( spacing );
  directTransform->SetGridOrigin( origin );
  directTransform->SetGridRegion( bsplineRegion );

  const unsigned int numberOfParameters = directTransform->GetNumberOfParameters();

  std::cout << "numberOfParameters " << numberOfParameters << std::endl;

  parameters.SetSize( numberOfParameters );
  parameters.Fill( 0.0 );
  directTransform->SetParameters( parameters );

  inverseCalculator->SetDirectTransform( directTransform );
  inverseCalculator->SetInverseTransform( inverseTransform );

  try
    {
    collector.Start("Initialize");
    inverseCalculator->Compute(); 
    collector.Stop("Initialize");
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  catch( std::bad_alloc )
    {
    std::cerr << "Problem allocating memory" << std::endl;
    return EXIT_FAILURE;
    }
  
  collector.Report( std::cout );

  // FIXME  add here, numerical test that doing a round-trip that composes the
  // two transform direct and inverse the points land in very close proximity
  // (define tolerance).

  return EXIT_SUCCESS;
}

