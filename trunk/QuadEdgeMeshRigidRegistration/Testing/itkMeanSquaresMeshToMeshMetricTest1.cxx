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

#include "itkVersorRigid3DTransform.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkNearestNeighborInterpolateMeshFunction.h"
#include "itkAmoebaOptimizer.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkCommand.h"

#include <iostream>


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



class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate()
    {
    this->IterationCounter = 0;
    }
public:
  typedef itk::AmoebaOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << "  Iteration " << IterationCounter++ << "   ";
      std::cout << "  cached value " << optimizer->GetCachedValue() << "   ";
      std::cout << "  position " << optimizer->GetCachedCurrentPosition() << std::endl ; 
    }

private:
   unsigned int   IterationCounter;

};




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

    fixedValue= sineMapSphericalCoordinatesFunction(phi, theta); 

    myFixedMesh->SetPointData(i, fixedValue);

    std::cout << "Point[" << i << "]: " << fixedPt << " radius " << radius << "  theta " << theta << "  phi " << phi  << "  fixedValue " << fixedValue << std::endl;
        
    }

   const double pi = 4.0 * atan( 1.0 );

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

    movingTheta= theta + pi / 4.0; 
    if ( movingTheta > pi ) 
      {
      movingTheta-= 2.0 * pi; 
      }

    movingValue= sineMapSphericalCoordinatesFunction(phi, movingTheta); 

    myMovingMesh->SetPointData(i, movingValue);

    std::cout << "Point[" << i << "]: " << movingPt << " radius " << radius << "  movingTheta " << movingTheta << "  phi " << phi  << "  movingValue " << movingValue << std::endl;
        
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

  typedef itk::VersorTransform<double>  TransformType;

  TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );


//------------------------------------------------------------
// Set up an Interpolator
//------------------------------------------------------------
  typedef itk::NearestNeighborInterpolateMeshFunction< 
                    MovingMeshType,
                    double > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( myMovingMesh );

  registration->SetInterpolator( interpolator );

  interpolator->Initialize();

  std::cout << metric << std::endl;

//------------------------------------------------------------
// Set up transform parameters
//------------------------------------------------------------
  const unsigned int numberOfTransformParameters = 
     transform->GetNumberOfParameters();

  ParametersType parameters( numberOfTransformParameters );

  // initialize the offset/vector part
  for( unsigned int k = 0; k < numberOfTransformParameters; k++ )
    {
    parameters[k]= 0.0f;
    }

  registration->SetInitialTransformParameters( parameters );

  // Optimizer Type
  typedef itk::AmoebaOptimizer         OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();

  OptimizerType::ParametersType simplexDelta( numberOfTransformParameters );
  simplexDelta.Fill( 0.1 );

  optimizer->AutomaticInitialSimplexOff();
  optimizer->SetInitialSimplexDelta( simplexDelta );
  optimizer->SetParametersConvergenceTolerance( 0.01 ); // radiands ?
  optimizer->SetFunctionConvergenceTolerance(0.001); // 0.1%
  optimizer->SetMaximumNumberOfIterations( 200 );



  registration->SetOptimizer( optimizer );

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  //typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  //CommandType::Pointer command = CommandType::New();
  //registration->AddObserver( itk::IterationEvent(), command );

#ifdef MICHELOUT
  //Callback to monitor results at each iteration
  typedef IterationCallback< OptimizerType >   IterationCallbackType;
  IterationCallbackType::Pointer callback = IterationCallbackType::New();
  callback->SetOptimizer( optimizer );
#endif


//------------------------------------------------------------
// This call is mandatory before start querying the Metric
// This method do all the necesary connections between the 
// internal components: Interpolator, Transform and Meshes
//------------------------------------------------------------
  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Metric initialization failed" << std::endl;
    std::cout << "Reason " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }


//---------------------------------------------------------
// Print out metric values
// for parameters[1] = {-10,10}  (arbitrary choice...)
//---------------------------------------------------------

  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  const double bestValue = optimizer->GetValue();

  std::cout << "final params " << finalParameters << "  final value " << bestValue << std::endl;



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
  

  std::cout << "Test passed. " << std::endl;

  return EXIT_SUCCESS;

}

