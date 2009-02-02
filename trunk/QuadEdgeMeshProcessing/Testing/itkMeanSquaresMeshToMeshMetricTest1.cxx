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

#include <math.h>
#include "itkVersorRigid3DTransform.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGradientDescentOptimizer.h"
#include "itkMeanSquaresImageToImageMetric.h"
//#include "itkCommandIterationUpdate.h"

#include <iostream>

const float TWO_PI= M_PI * 2.0; 

static float linearMapSphericalCoordinatesFunction(float inPhi, float inTheta); 

//Linear mapping between 0 and 1 as a function of phi and theta
static float 
linearMapSphericalCoordinatesFunction(float inPhi, float inTheta) 
{
  float result; 

  float phiFactor= (M_PI - inPhi)/M_PI; //inPhi should be in [0,PI]; peak at North Pole: phi=0.
  float thetaFactor= (inTheta + M_PI)/TWO_PI; //inTheta should be in [-PI,PI]; 

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

  typedef MovingSphereMeshSourceType::PointType   MovingPointType;
  typedef FixedSphereMeshSourceType::PointType   FixedPointType;
  typedef MovingSphereMeshSourceType::VectorType  MovingVectorType;
  typedef FixedSphereMeshSourceType::VectorType  FixedVectorType;

  // Registration Method
  typedef itk::MeshToMeshRegistrationMethod< 
                                    FixedMeshType, 
                                    MovingMeshType >    RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();



// Set up synthetic data. Two spherical meshes, one is rotated theta=pi/4 from the other 

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
  FixedPointType  fixedPt;
  float radius, phi, theta, movingTheta; //spherical coordinates
  float fixedValue, movingValue; 
  
  std::cout << "Testing itk::RegularSphereMeshSource "<< std::endl;

  myFixedMesh->Print( std::cout );

  for( unsigned int i=0; i < myFixedMesh->GetNumberOfPoints(); i++ )
    {
      
    myFixedMesh->GetPoint(i, &fixedPt);

    for ( unsigned int j=0; j<3; j++ ) 
      {
        fixedPt[j]-= fixedCenter[j];   //coordinates relative to center, if center is not origin
      }

    radius= sqrt(fixedPt[0]*fixedPt[0] + fixedPt[1]*fixedPt[1] + fixedPt[2]*fixedPt[2]); //assuming radius is not valued 1
    theta= atan2(fixedPt[1], fixedPt[0]); 
    phi= acos(fixedPt[2]/radius); 

    fixedValue= linearMapSphericalCoordinatesFunction(phi, theta); 

    myFixedMesh->SetPointData(i, fixedValue);

    std::cout << "Point[" << i << "]: " << fixedPt << " radius " << radius << "  theta " << theta << "  phi " << phi  << "  fixedValue " << fixedValue << std::endl;
        
    }

   for( unsigned int i=0; i < myMovingMesh->GetNumberOfPoints(); i++ )
    {
      
    myMovingMesh->GetPoint(i, &movingPt);

    for ( unsigned int j=0; j<3; j++ ) 
      {
        movingPt[j]-= movingCenter[j];   //coordinates relative to center, if center is not origin
      }

    radius= sqrt(movingPt[0]*movingPt[0] + movingPt[1]*movingPt[1] + movingPt[2]*movingPt[2]); //assuming radius is not valued 1
    theta= atan2(movingPt[1], movingPt[0]); 
    phi= acos(movingPt[2]/radius); 

    movingTheta= theta + M_PI_4; 
    if (movingTheta>M_PI) 
      {
      movingTheta-= TWO_PI; 
      }

    movingValue= linearMapSphericalCoordinatesFunction(phi, movingTheta); 

    myMovingMesh->SetPointData(i, movingValue);

    std::cout << "Point[" << i << "]: " << movingPt << " radius " << radius << "  movingTheta " << movingTheta << "  phi " << phi  << "  movingValue " << movingValue << std::endl;
        
    }   


//-----------------------------------------------------------
// Set up  the Metric
//-----------------------------------------------------------
  typedef itk::MeanSquaresMeshToMeshMetric< FixedMeshType, 
                                            MovingMeshType >   
                                            MetricType;

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;

  MetricType::Pointer  metric = MetricType::New();
  registration->SetMetric( metric.GetPointer() ); 


//-----------------------------------------------------------
// Plug the Meshes into the metric
//-----------------------------------------------------------
  registration->SetFixedMesh( myFixedMesh );
  registration->SetMovingMesh( myMovingMesh );


//-----------------------------------------------------------
// Set up a Transform
//-----------------------------------------------------------

  typedef itk::VersorRigid3DTransform<float>  TransformType;

  TransformType::Pointer transform = TransformType::New();

  //metric->SetTransform( transform.GetPointer() );
  registration->SetTransform( transform.GetPointer() );


//------------------------------------------------------------
// Set up an Interpolator
//------------------------------------------------------------
  typedef itk::LinearInterpolateMeshFunction< 
                    MovingMeshType,
                    double > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( movingMesh.GetPointer() );
 
  metric->SetInterpolator( interpolator.GetPointer() );
  
  std::cout << metric << std::endl;

  // Optimizer Type
  typedef itk::GradientDescentOptimizer                  OptimizerType;
  typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
  registration->SetOptimizer(     optimizer     );


//------------------------------------------------------------
// This call is mandatory before start querying the Metric
// This method do all the necesary connections between the 
// internal components: Interpolator, Transform and Meshs
//------------------------------------------------------------
  try {
    metric->Initialize();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Metric initialization failed" << std::endl;
    std::cout << "Reason " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }


//------------------------------------------------------------
// Set up transform parameters
//------------------------------------------------------------
  ParametersType parameters( transform->GetNumberOfParameters() );

  // initialize the offset/vector part
  for( unsigned int k = 0; k < MeshDimension; k++ )
    {
    parameters[k]= 0.0f;
    }


//---------------------------------------------------------
// Print out metric values
// for parameters[1] = {-10,10}  (arbitrary choice...)
//---------------------------------------------------------

  MetricType::MeasureType     measure;

  std::cout << "param[1]   Metric    d(Metric)/d(param[1] " << std::endl;



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
  std::cout << "Interpolator: " << metric->GetInterpolator() << std::endl;
  std::cout << "NumberOfPixelsCounted: " << metric->GetNumberOfPixelsCounted() << std::endl;

  std::cout << "Check case when Target is NULL" << std::endl;
  metric->SetFixedMesh( NULL );
  try 
    {
    std::cout << "Value = " << metric->GetValue( parameters );
    std::cout << "If you are reading this message the Metric " << std::endl;
    std::cout << "is NOT managing exceptions correctly    " << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & e )
    { 
    std::cout << "Exception received (as expected) "    << std::endl;
    std::cout << "Description : " << e.GetDescription() << std::endl;
    std::cout << "Location    : " << e.GetLocation()    << std::endl;
    std::cout << "Test for exception throwing... PASSED ! " << std::endl;
    }
  
 
 bool pass;
#define TEST_INITIALIZATION_ERROR( ComponentName, badComponent, goodComponent ) \
  metric->Set##ComponentName( badComponent ); \
  try \
    { \
    pass = false; \
    metric->Initialize(); \
    } \
  catch( itk::ExceptionObject& err ) \
    { \
    std::cout << "Caught expected ExceptionObject" << std::endl; \
    std::cout << err << std::endl; \
    pass = true; \
    } \
  metric->Set##ComponentName( goodComponent ); \
  \
  if( !pass ) \
    { \
    std::cout << "Test failed." << std::endl; \
    return EXIT_FAILURE; \
    } 

  TEST_INITIALIZATION_ERROR( Transform, NULL, transform );
  TEST_INITIALIZATION_ERROR( FixedMesh, NULL, fixedMesh );
  TEST_INITIALIZATION_ERROR( MovingMesh, NULL, movingMesh );
  TEST_INITIALIZATION_ERROR( Interpolator, NULL, interpolator );
 
  std::cout << "Test passed. " << std::endl;

  return EXIT_SUCCESS;

}

