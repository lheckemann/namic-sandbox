/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnalyticalMeshToMeshMetricTest1.cxx,v $
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
#include "itkVersor.h"
#include "itkCovariantVector.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkAnalyticalMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkGradientDescentOptimizer.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "vtkPolyDataReader.h"
#include "RegistrationMonitor.h"
#include "itkTransformMeshFilter.h"
//#include "itkCommandIterationUpdate.h"

#include <iostream>

const float TWO_PI= M_PI * 2.0; 

static float sineMapSphericalCoordinatesFunction(float inPhi, float inTheta); 

//Linear mapping between 0 and 1 as a function of phi and theta
static float 
sineMapSphericalCoordinatesFunction(float inPhi, float inTheta) 
{
  float result; 

  //Sharply peaked function at phi=PI/2, theta=PI/2
  float phiFactor= pow(sin(inPhi),1.0); //inPhi should be in [0,PI]; peak at equator: phi=PI/2.
  float thetaFactor= pow((sin(inTheta)+1.0)/2.0,1.0); //inTheta should be in [-PI,PI]; 

  result= phiFactor * thetaFactor; 

  return result; 
}

#include "itkCommand.h"

static int counterCmdIteration= 0; 

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
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
      //std::cout << optimizer->GetCurrentIteration() << "   ";
      //std::cout << "  value " << optimizer->GetValue() << "   ";
      std::cout << " CommandIterationUpdate new iteration  number   " << ++counterCmdIteration;
      std::cout << "  cached value " << optimizer->GetValue() << "   ";
      std::cout << "  position " << optimizer->GetCurrentPosition() << std::endl ; 
    }
};




int main( int argc, char * argv [] )
{

  typedef itk::QuadEdgeMesh<float, 3>   MovingMeshType;
  typedef itk::QuadEdgeMesh<float, 3>   FixedMeshType;
  typedef itk::QuadEdgeMesh<float, 3>   OutputMeshType;

  typedef itk::RegularSphereMeshSource< MovingMeshType >  MovingSphereMeshSourceType;
  typedef itk::RegularSphereMeshSource< FixedMeshType >  FixedSphereMeshSourceType;
  typedef itk::RegularSphereMeshSource< OutputMeshType >  OutputSphereMeshSourceType;

  MovingSphereMeshSourceType::Pointer  myMovingSphereMeshSource = MovingSphereMeshSourceType::New();
  FixedSphereMeshSourceType::Pointer  myFixedSphereMeshSource = FixedSphereMeshSourceType::New();
  OutputSphereMeshSourceType::Pointer  myOutputSphereMeshSource = OutputSphereMeshSourceType::New();

  typedef MovingSphereMeshSourceType::PointType   MovingPointType;
  typedef FixedSphereMeshSourceType::PointType   FixedPointType;
  typedef MovingSphereMeshSourceType::VectorType  MovingVectorType;
  typedef FixedSphereMeshSourceType::VectorType  FixedVectorType;
  
  //vtkPolyData*                FixedSurface;
  //vtkPolyData*                MovingSurface;
  //vtkPolyData*                OutputSurface;


  // Registration Method
  typedef itk::MeshToMeshRegistrationMethod< 
                                    FixedMeshType, 
                                    MovingMeshType >    RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();

  RegistrationMonitor monitor;


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

#ifdef DEBUG    
    std::cout << "Point[" << i << "]: " << fixedPt << " radius " << radius << "  theta " << theta << "  phi " << phi  << "  fixedValue " << fixedValue << std::endl;
#endif
    
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
  
  // Load the fixed surface
  vtkPolyDataReader* fixedReader = vtkPolyDataReader::New();
  fixedReader->SetFileName(fixedWriter->GetFileName());
  fixedReader->Update();

  if ( fixedReader->GetErrorCode() )
    {
       std::cerr << "Error: Failed reading " <<  fixedWriter->GetFileName() << std::endl;
    fixedReader->Delete();
    return EXIT_FAILURE;
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

    movingTheta= theta /*+ M_PI_4*/; 
    if (movingTheta>M_PI) 
      {
      movingTheta-= TWO_PI; 
      }

    movingValue= sineMapSphericalCoordinatesFunction(phi, movingTheta); 

    myMovingMesh->SetPointData(i, movingValue);

#ifdef DEBUG    
    std::cout << "Point[" << i << "]: " << movingPt << " radius " << radius << "  movingTheta " << movingTheta << "  phi " << phi  << "  movingValue " << movingValue << std::endl;
#endif    
        
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


  // Load the moving surface
  vtkPolyDataReader* movingReader = vtkPolyDataReader::New();
  movingReader->SetFileName( movingWriter->GetFileName() );
  movingReader->Update();

  if ( movingReader->GetErrorCode() )
    {
    std::cerr << "Error: Failed reading " << movingWriter->GetFileName() << std::endl;
    fixedReader->Delete();
    movingReader->Delete();
    return EXIT_FAILURE;
    }

  monitor.SetFixedSurface( fixedReader->GetOutput() );
  monitor.SetMovingSurface( movingReader->GetOutput() );

//-----------------------------------------------------------
// Set up  the Metric
//-----------------------------------------------------------
  //This so-called metric is just a synthetic objective function
  //based on moving mesh alone to test the mesh registration.
  //We would expect the registration
  //to keep nudging the sphere to the global minimum in the function
  typedef itk::AnalyticalMeshToMeshMetric< FixedMeshType, 
                                            MovingMeshType >   
                                            MetricType;

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;

  MetricType::Pointer  metric = MetricType::New();
  registration->SetMetric( metric ); 


//-----------------------------------------------------------
// Plug the Meshes into the metric
//-----------------------------------------------------------
  registration->SetFixedMesh( myFixedMesh );
  registration->SetMovingMesh( myFixedMesh );


//-----------------------------------------------------------
// Set up a Transform
//-----------------------------------------------------------

  typedef itk::VersorRigid3DTransform<double>  TransformType;

  TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );


//------------------------------------------------------------
// Set up an Interpolator
//------------------------------------------------------------
  typedef itk::LinearInterpolateMeshFunction< 
                    MovingMeshType,
                    double > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( myMovingMesh );

  registration->SetInterpolator( interpolator );

  interpolator->Initialize();


//------------------------------------------------------------
// Simple Test of derivative computed by Interpolator
//------------------------------------------------------------

  std::cout << metric << std::endl;

//------------------------------------------------------------
// Set up transform parameters
//------------------------------------------------------------
  const unsigned int numberOfTransformParameters = 
     transform->GetNumberOfParameters();

  ParametersType parameters( numberOfTransformParameters );

  //typedef versorCostFunction::ParametersType    ParametersType;

  // Optimizer Type
  typedef itk::VersorRigid3DTransformOptimizer         OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();

  typedef  OptimizerType::ScalesType            ScalesType;
  typedef OptimizerType::VersorType      VersorType;

  //OptimizerType::ParametersType simplexDelta( numberOfTransformParameters );
  //simplexDelta.Fill( 0.1 );

  // We start with a null rotation
  VersorType::VectorType axis;
  axis[0] =  1.0f;
  axis[1] =  0.0f;
  axis[2] =  0.0f;

  VersorType::ValueType angle = 0.34f;

  VersorType initialRotation;
  
  initialRotation.Set( axis, angle );
  
  ParametersType  initialPosition( numberOfTransformParameters );
  initialPosition[0] = initialRotation.GetX();
  initialPosition[1] = initialRotation.GetY();
  initialPosition[2] = initialRotation.GetZ();

  initialPosition[3] = 0.0;
  initialPosition[4] = 0.0;
  initialPosition[5] = 0.0;

  ScalesType    parametersScale( numberOfTransformParameters );
  parametersScale[0] = 1.0;
  parametersScale[1] = 1.0;
  parametersScale[2] = 1.0;
  
  parametersScale[3] = 1.0;
  parametersScale[4] = 1.0;
  parametersScale[5] = 1.0;

  registration->SetInitialTransformParameters( initialPosition );

  optimizer->MinimizeOn();
  optimizer->SetScales( parametersScale );
  optimizer->SetGradientMagnitudeTolerance( 1e-15 );
  optimizer->SetMaximumStepLength( 0.01 ); // About 20 degrees
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetNumberOfIterations( 200 );
  optimizer->SetRelaxationFactor( 0.9 );

  registration->SetOptimizer( optimizer );


  typedef itk::TransformMeshFilter<MovingMeshType,OutputMeshType,TransformBaseType> MeshTransformFilter;
  MeshTransformFilter::Pointer filter = MeshTransformFilter::New();
  filter->SetInput( myMovingMesh );
  //registration->Update();
  filter->SetTransform( registration->GetTransform()  );
  filter->Update();
  OutputMeshType::Pointer myOutputMesh = filter->GetOutput();
  std::cout << " first transform applied to moving mesh by filter " <<  filter->GetTransform()->GetParameters() << " \n";
  
  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< OutputMeshType >   OutputWriterType;
  OutputWriterType::Pointer outputWriter = OutputWriterType::New();
  outputWriter->SetInput( myOutputMesh );
  outputWriter->SetFileName( "OutputMeshFirst.vtk" );

  try
    {
    outputWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during outputWriter Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
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

    monitor.Observe( 
      optimizer, transform );
    monitor.SetNumberOfIterationPerUpdate( 1 );

//------------------------------------------------------------
// This call is mandatory before start querying the Metric
// This method do all the necesary connections between the 
// internal components: Interpolator, Transform and Meshes
//------------------------------------------------------------
  try
    {
       //optimizer->StartOptimization(); 
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

  //typedef itk::TransformMeshFilter<MovingMeshType,OutputMeshType,TransformBaseType> MeshTransformFilter;
  //MeshTransformFilter::Pointer filter = MeshTransformFilter::New();
  filter->SetInput( myMovingMesh ); 
  filter->SetTransform( transform );
  filter->Update();
  /*OutputMeshType::Pointer */ myOutputMesh = filter->GetOutput(); 
  

  //typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< OutputMeshType >   OutputWriterType;
  //OutputWriterType::Pointer outputWriter = OutputWriterType::New();
  outputWriter->SetInput( myOutputMesh );
  outputWriter->SetFileName( "OutputMeshFinal.vtk" );
  std::cout << " second transform applied to moving mesh by filter " <<  filter->GetTransform()->GetParameters() << " \n";

  try
    {
    outputWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during outputWriter Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


//-------------------------------------------------------
// exercise Print() method
//-------------------------------------------------------
  metric->Print( std::cout );

  
#if 0
//-------------------------------------------------------
// exercise misc member functions
//-------------------------------------------------------
  std::cout << "FixedMesh: " << metric->GetFixedMesh() << std::endl;
  std::cout << "MovingMesh: " << metric->GetMovingMesh() << std::endl;
  std::cout << "Transform: " << metric->GetTransform() << std::endl;

  metric->SetFixedMesh( NULL );
  try 
    {
    std::cout << "Value = " << metric->GetValue( parameters );
    std::cout << "Check case when Target is NULL" << std::endl;
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
  
#endif
  
  std::cout << "Test passed. " << std::endl;

  return EXIT_SUCCESS;

}
