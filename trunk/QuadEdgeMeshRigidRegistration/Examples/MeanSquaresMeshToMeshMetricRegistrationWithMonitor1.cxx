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

#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkVersorTransformOptimizer.h"
#include "itkVersorTransform.h"
#include "itkQuadEdgeMesh.h"

#include "itkCommand.h"
#include "itkVTKPolyDataReader.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

#include "RegistrationMonitor.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"


int main( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputFixedMesh inputMovingMesh ";
    std::cerr << "axisX axisY axisZ angle " << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::QuadEdgeMesh< float, 3 >   FixedMeshType;
  typedef itk::QuadEdgeMesh< float, 3 >   MovingMeshType;

  typedef itk::VTKPolyDataReader< FixedMeshType >     FixedReaderType;
  typedef itk::VTKPolyDataReader< MovingMeshType >    MovingReaderType;

  FixedReaderType::Pointer fixedMeshReader = FixedReaderType::New();
  fixedMeshReader->SetFileName( argv[1] );

  MovingReaderType::Pointer movingMeshReader = MovingReaderType::New();
  movingMeshReader->SetFileName( argv[2] );

  try
    {
    fixedMeshReader->Update( );
    movingMeshReader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  FixedMeshType::ConstPointer  meshFixed  = fixedMeshReader->GetOutput();
  MovingMeshType::ConstPointer meshMoving = movingMeshReader->GetOutput();

  typedef itk::MeshToMeshRegistrationMethod< 
                                    FixedMeshType, 
                                    MovingMeshType >    RegistrationType;

  RegistrationType::Pointer   registration  = RegistrationType::New();

  typedef itk::MeanSquaresMeshToMeshMetric< FixedMeshType, 
                                            MovingMeshType >   
                                            MetricType;

  MetricType::Pointer  metric = MetricType::New();

  registration->SetMetric( metric ); 

  registration->SetFixedMesh( fixedMeshReader->GetOutput() );
  registration->SetMovingMesh( movingMeshReader->GetOutput() );


  typedef itk::VersorTransform< MetricType::TransformComputationType >  TransformType;

  TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );


  typedef itk::LinearInterpolateMeshFunction< MovingMeshType > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  registration->SetInterpolator( interpolator );

  const unsigned int numberOfTransformParameters = 
     transform->GetNumberOfParameters();

  typedef TransformType::ParametersType         ParametersType;
  ParametersType parameters( numberOfTransformParameters );

  TransformType::AxisType  axis;
  TransformType::AngleType angle;

  axis[0] = atof( argv[3] );
  axis[1] = atof( argv[4] );
  axis[2] = atof( argv[5] );

  angle = atof( argv[6] );

  transform->SetRotation( axis, angle );
  
  parameters = transform->GetParameters();

  registration->SetInitialTransformParameters( parameters );


  typedef itk::VersorTransformOptimizer     OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();

  typedef OptimizerType::ScalesType             ScalesType;

  ScalesType    parametersScale( numberOfTransformParameters );
  parametersScale[0] = 1.0;
  parametersScale[1] = 1.0;
  parametersScale[2] = 1.0;

  optimizer->MinimizeOn();
  optimizer->SetScales( parametersScale );
  optimizer->SetGradientMagnitudeTolerance( 1e-6 );
  optimizer->SetMaximumStepLength( 0.05 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.9 );
  optimizer->SetNumberOfIterations( 100 );


  registration->SetOptimizer( optimizer );


  //
  // Configuration of Visual Registration Monitor
  //
  RegistrationMonitor visualMonitor;

  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader->SetFileName( argv[1] );
  vtkMovingMeshReader->SetFileName( argv[2] );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );
  
  visualMonitor.SetNumberOfIterationsPerUpdate( 1 );

  visualMonitor.Observe( optimizer.GetPointer(), transform.GetPointer() );


  //
  // Now trigger the registration process...
  //
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


  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  const double bestValue = optimizer->GetValue();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << bestValue << std::endl;

  return EXIT_SUCCESS;

}
