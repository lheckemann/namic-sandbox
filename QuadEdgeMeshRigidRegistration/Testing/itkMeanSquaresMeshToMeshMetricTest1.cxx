/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanSquaresMeshToMeshMetricTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2007-09-06 17:44:24 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkVersorTransform.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkNearestNeighborInterpolateMeshFunction.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkCommand.h"
#include "itkTestingMacros.h"



//Linear mapping between 0 and 1 as a function of phi and theta
static float 
sineMapSphericalCoordinatesFunction(float inPhi, float inTheta) 
{
  float result; 

  const float pi = atan(1.0) * 4.0;

  float phiFactor= (pi - inPhi)/pi; //inPhi should be in [0,PI]; peak at North Pole: phi=0.
  float thetaFactor= (sin(inTheta)+1.0)/2.0; //inTheta should be in [-PI,PI]; 

  result= phiFactor * thetaFactor; 

  return result; 
}


int main( int argc, char * argv [] )
{

  typedef itk::QuadEdgeMesh<float, 3>   MovingMeshType;
  typedef itk::QuadEdgeMesh<float, 3>   FixedMeshType;

  typedef itk::RegularSphereMeshSource< MovingMeshType >  MovingSphereMeshSourceType;
  typedef itk::RegularSphereMeshSource< FixedMeshType >  FixedSphereMeshSourceType;

  MovingSphereMeshSourceType::Pointer  myMovingSphereMeshSource = MovingSphereMeshSourceType::New();
  FixedSphereMeshSourceType::Pointer  myFixedSphereMeshSource = FixedSphereMeshSourceType::New();

  typedef MovingSphereMeshSourceType::PointType     MovingPointType;
  typedef FixedSphereMeshSourceType::PointType      FixedPointType;
  typedef MovingSphereMeshSourceType::VectorType    MovingVectorType;
  typedef FixedSphereMeshSourceType::VectorType     FixedVectorType;


  // Set up synthetic data. Two spherical meshes, one is rotated theta=pi/4
  // from the other 

  MovingPointType movingCenter; 
  movingCenter.Fill( 0.0 );
  MovingPointType fixedCenter; 
  fixedCenter.Fill( 0.0 );

  MovingVectorType movingScale;
  movingScale.Fill( 1.0 );
  FixedVectorType fixedScale;
  fixedScale.Fill( 1.0 );
  
  myMovingSphereMeshSource->SetCenter( movingCenter );
  myMovingSphereMeshSource->SetResolution( 4.0 );
  myMovingSphereMeshSource->SetScale( movingScale );
  myMovingSphereMeshSource->Modified();

  myFixedSphereMeshSource->SetCenter( fixedCenter );
  myFixedSphereMeshSource->SetResolution( 4.0 );
  myFixedSphereMeshSource->SetScale( fixedScale );
  myFixedSphereMeshSource->Modified();

  try
    {
    myMovingSphereMeshSource->Update();
    myFixedSphereMeshSource->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during source Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "myMovingSphereMeshSource: " << myMovingSphereMeshSource;
  std::cout << "myFixedSphereMeshSource: " << myFixedSphereMeshSource;
  
  FixedMeshType::Pointer myFixedMesh = myFixedSphereMeshSource->GetOutput();
  MovingMeshType::Pointer myMovingMesh = myMovingSphereMeshSource->GetOutput();

  MovingPointType  movingPt;
  FixedPointType   fixedPt;

  typedef MovingPointType::VectorType  MovingVectorType;
  typedef FixedPointType::VectorType   FixedVectorType;

  MovingVectorType  movingVtr;
  FixedVectorType   fixedVtr;
  
  std::cout << "Testing itk::RegularSphereMeshSource "<< std::endl;

  myFixedMesh->Print( std::cout );

  for( unsigned int i=0; i < myFixedMesh->GetNumberOfPoints(); i++ )
    {
    myFixedMesh->GetPoint(i, &fixedPt);

    fixedVtr = fixedPt - fixedCenter;

    const float radius = fixedVtr.GetNorm();
    const float theta  = atan2(fixedVtr[1], fixedVtr[0]); 
    const float phi    = acos(fixedVtr[2]/radius); 

    const float fixedValue= sineMapSphericalCoordinatesFunction(phi, theta); 

    myFixedMesh->SetPointData(i, fixedValue);
    }

    const double pi = 4.0 * atan( 1.0 );

    for( unsigned int i=0; i < myMovingMesh->GetNumberOfPoints(); i++ )
      {
      myMovingMesh->GetPoint(i, &movingPt);

      movingVtr = movingPt - movingCenter;

      const float radius = movingVtr.GetNorm();
      const float theta  = atan2(movingVtr[1], movingVtr[0]); 
      const float phi    = acos(movingVtr[2]/radius); 

      float movingTheta = theta + pi / 4.0; 
      if ( movingTheta > pi ) 
        {
        movingTheta-= 2.0 * pi; 
        }

      const float movingValue= sineMapSphericalCoordinatesFunction(phi, movingTheta);

      myMovingMesh->SetPointData(i, movingValue);
      }   

  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< FixedMeshType >   FixedWriterType;
  FixedWriterType::Pointer fixedWriter = FixedWriterType::New();
  fixedWriter->SetInput( myFixedMesh );
  fixedWriter->SetFileName( "FixedMesh.vtk" );

  try
    {
    fixedWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during fixedWriter Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MovingMeshType >   MovingWriterType;
  MovingWriterType::Pointer movingWriter = MovingWriterType::New();
  movingWriter->SetInput( myMovingMesh );
  movingWriter->SetFileName( "MovingMesh.vtk" );

  try
    {
    movingWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during movingWriter Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  //-----------------------------------------------------------
  // Set up  the Metric
  //-----------------------------------------------------------
  typedef itk::MeanSquaresMeshToMeshMetric< 
    FixedMeshType, MovingMeshType >   MetricType;

  MetricType::Pointer  metric = MetricType::New();

  TRY_EXPECT_EXCEPTION( metric->Initialize() );

  //-----------------------------------------------------------
  // Plug the Meshes into the metric
  //-----------------------------------------------------------
  metric->SetFixedMesh( myFixedMesh );

  TRY_EXPECT_EXCEPTION( metric->Initialize() );

  metric->SetMovingMesh( myMovingMesh );

  TRY_EXPECT_EXCEPTION( metric->Initialize() );

  //-----------------------------------------------------------
  // Set up a Transform
  //-----------------------------------------------------------
  typedef itk::VersorTransform<double>  TransformType;

  TransformType::Pointer transform = TransformType::New();

  TRY_EXPECT_EXCEPTION( metric->Initialize() );

  metric->SetTransform( transform );


//------------------------------------------------------------
// Set up an Interpolator
//------------------------------------------------------------
  typedef itk::NearestNeighborInterpolateMeshFunction< 
                    MovingMeshType,
                    double > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( myMovingMesh );

  metric->SetInterpolator( interpolator );

  interpolator->Initialize();

  std::cout << metric << std::endl;

//------------------------------------------------------------
// Set up transform parameters
//------------------------------------------------------------
  const unsigned int numberOfTransformParameters = 
     transform->GetNumberOfParameters();

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;

  ParametersType parameters( numberOfTransformParameters );

  const unsigned int numberOfSampleValues = 10;

  const float toRadians = 8.0 * atan( 1.0 );

  typedef itk::Versor< double > VersorType;
  VersorType versor;

  VersorType::VectorType  axis;
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  MetricType::DerivativeType derivative1;
  MetricType::DerivativeType derivative2;

  MetricType::MeasureType value1;
  MetricType::MeasureType value2;

  const double tolerance = 1e-5;

  for( unsigned int p = 0; p < numberOfSampleValues; p++ )
    { 
    const float angle = p * toRadians / numberOfSampleValues;

    versor.Set( axis, angle );

    parameters[0] = versor.GetX();
    parameters[1] = versor.GetY();
    parameters[2] = versor.GetZ();
    
    value1 = metric->GetValue( parameters );
    metric->GetDerivative( parameters, derivative1 );
    metric->GetValueAndDerivative( parameters, value2, derivative2 );

    if ( vnl_math_abs( value2 - value1 ) > tolerance )
      {
      std::cerr << "GetValue() does not match GetValueAndDerivative()" << std::endl;
      return EXIT_FAILURE;
      }
    }
  
  //-------------------------------------------------------
  // exercise Print() method
  //-------------------------------------------------------
  metric->Print( std::cout );


  //-------------------------------------------------------
  // exercise misc member functions
  //-------------------------------------------------------
  std::cout << "FixedMesh: " << metric->GetFixedMesh() << std::endl;
  std::cout << "MovingMesh: " << metric->GetMovingMesh() << std::endl;
  std::cout << "Transform: " << metric->GetTransform() << std::endl;

  std::cout << "Check case when Target is NULL" << std::endl;
  metric->SetFixedMesh( NULL );
        
  TRY_EXPECT_EXCEPTION( metric->GetValue( parameters ) );
  TRY_EXPECT_EXCEPTION( metric->GetDerivative( parameters, derivative1 ) );
  TRY_EXPECT_EXCEPTION( metric->GetValueAndDerivative( parameters, value1, derivative1  ) );

  std::cout << "Test passed. " << std::endl;

  return EXIT_SUCCESS;

}
