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
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkAmoebaOptimizer.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkCommand.h"
#include "itkVTKPolyDataReader.h"


class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;

  itkNewMacro( Self );

protected:
  CommandIterationUpdate()
   {
   iterationCounter = 0;
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
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );

    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << " CommandIterationUpdate new iteration  number   " << ++iterationCounter;
    std::cout << "  cached value " << optimizer->GetCachedValue() << "   ";
    std::cout << "  position " << optimizer->GetCachedCurrentPosition() << std::endl ; 
    }
private:

  unsigned int iterationCounter; 

};




int main( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputFixedMesh inputMovingMesh " << std::endl;
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

  typedef MetricType::TransformType                 TransformBaseType;

  MetricType::Pointer  metric = MetricType::New();
  registration->SetMetric( metric.GetPointer() ); 


  registration->SetFixedMesh( fixedMeshReader->GetOutput() );
  registration->SetMovingMesh( movingMeshReader->GetOutput() );


  typedef itk::VersorTransform< FixedMeshType::CoordRepType >  TransformType;

  TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );


  typedef itk::LinearInterpolateMeshFunction< MovingMeshType > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  registration->SetInterpolator( interpolator );

  const unsigned int numberOfTransformParameters = 
     transform->GetNumberOfParameters();

  typedef TransformBaseType::ParametersType         ParametersType;
  ParametersType parameters( numberOfTransformParameters );

  parameters.Fill( 0.0 );

  registration->SetInitialTransformParameters( parameters );

  // Optimizer Type
  typedef itk::AmoebaOptimizer         OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();

  OptimizerType::ParametersType simplexDelta( numberOfTransformParameters );
  simplexDelta.Fill( 0.1 );

  optimizer->AutomaticInitialSimplexOff();
  optimizer->SetInitialSimplexDelta( simplexDelta );
  optimizer->SetParametersConvergenceTolerance( 0.01 );
  optimizer->SetFunctionConvergenceTolerance(0.001);
  optimizer->SetMaximumNumberOfIterations( 200 );


  registration->SetOptimizer( optimizer );

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

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

