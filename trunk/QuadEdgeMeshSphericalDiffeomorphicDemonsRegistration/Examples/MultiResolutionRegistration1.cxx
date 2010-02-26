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

#include "itkResampleQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshTraits.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshVectorDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"
#include "itkDeformationFieldFromTransformMeshFilter.h"
#include "itkResampleDestinationPointsQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshGenerateDeformationFieldFilter.h"
#include "itkIdentityTransform.h"

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsRegistrationConfigure.h"

#ifdef USE_VTK
#include "AffineRegistrationMonitor.h"
#include "DeformableRegistrationMonitor.h"
#include "DeformableAndAffineRegistrationMonitor.h"
#include "vtkSmartPointer.h"
#include "vtkPolyDataReader.h"
#endif


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
  typedef itk::VersorTransformOptimizer  OptimizerType;
  typedef   const OptimizerType   *      OptimizerPointer;

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

    std::cout << " Iteration " << ++iterationCounter;
    std::cout << "  Value " << optimizer->GetValue() << "   ";
    std::cout << "  Position " << optimizer->GetCurrentPosition() << std::endl ; 
    }
private:

  unsigned int iterationCounter; 

};




int main( int argc, char * argv [] )
{

  if( argc < 16 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputFixedMeshRes1 inputMovingMeshRes1 ";
    std::cerr << "outputResampledMeshRes1 ";
    std::cerr << "inputFixedMeshRes2 inputMovingMeshRes2 ";
    std::cerr << "outputResampledMeshRes2 ";
    std::cerr << "inputFixedMeshRes3 inputMovingMeshRes3 ";
    std::cerr << "outputResampledMeshRes3 ";
    std::cerr << "inputFixedMeshRes4 inputMovingMeshRes4 ";
    std::cerr << "outputResampledMeshRes4 ";
    std::cerr << "inputFixedMeshRes5 inputMovingMeshRes5 ";
    std::cerr << "outputResampledMeshRes5 ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef float      MeshPixelType;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   FixedMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   MovingMeshType;

  typedef itk::VTKPolyDataReader< FixedMeshType >     FixedReaderType;
  typedef itk::VTKPolyDataReader< MovingMeshType >    MovingReaderType;

  FixedReaderType::Pointer fixedMeshReader1 = FixedReaderType::New();
  fixedMeshReader1->SetFileName( argv[1] );

  MovingReaderType::Pointer movingMeshReader1 = MovingReaderType::New();
  movingMeshReader1->SetFileName( argv[2] );

  try
    {
    fixedMeshReader1->Update();
    movingMeshReader1->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  FixedMeshType::ConstPointer  meshFixed  = fixedMeshReader1->GetOutput();
  MovingMeshType::ConstPointer meshMoving = movingMeshReader1->GetOutput();

  typedef itk::MeshToMeshRegistrationMethod< 
                                    FixedMeshType, 
                                    MovingMeshType >    RegistrationType;

  RegistrationType::Pointer   registration  = RegistrationType::New();

  typedef itk::MeanSquaresMeshToMeshMetric< FixedMeshType, 
                                            MovingMeshType >   
                                            MetricType;

  MetricType::Pointer  metric = MetricType::New();

  registration->SetMetric( metric ); 


  registration->SetFixedMesh( meshFixed );
  registration->SetMovingMesh( meshMoving );


  typedef itk::VersorTransform< MetricType::TransformComputationType >  TransformType;

  TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );


  typedef itk::LinearInterpolateMeshFunction< MovingMeshType > InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  registration->SetInterpolator( interpolator );


  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   RegisteredMeshType;

  typedef itk::QuadEdgeMeshSphericalDiffeomorphicDemonsFilter<
    FixedMeshType, MovingMeshType, RegisteredMeshType >   DemonsFilterType;

  typedef DemonsFilterType::DestinationPointSetType    DestinationPointSetType;


  const unsigned int numberOfTransformParameters = transform->GetNumberOfParameters();

  typedef TransformType::ParametersType         ParametersType;
  ParametersType parameters( numberOfTransformParameters );

  transform->SetIdentity();
  
  parameters = transform->GetParameters();

  registration->SetInitialTransformParameters( parameters );


  typedef itk::VersorTransformOptimizer     OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();

  registration->SetOptimizer( optimizer );


  typedef OptimizerType::ScalesType             ScalesType;

  ScalesType    parametersScale( numberOfTransformParameters );
  parametersScale[0] = 1.0;
  parametersScale[1] = 1.0;
  parametersScale[2] = 1.0;

  optimizer->SetScales( parametersScale );

  optimizer->MinimizeOn();
  optimizer->SetGradientMagnitudeTolerance( 1e-6 );
  optimizer->SetMaximumStepLength( 0.05 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.9 );
  optimizer->SetNumberOfIterations( 64 );


  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

#ifdef USE_VTK
  typedef DeformableAndAffineRegistrationMonitor< 
    DestinationPointSetType > RegistrationMonitorType;

  RegistrationMonitorType  visualMonitor;
  visualMonitor.SetNumberOfIterationsPerUpdate( 1 );

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 1");

  visualMonitor.Observe( optimizer.GetPointer() );
  visualMonitor.ObserveData( transform.GetPointer() );

  visualMonitor.SetVerbose( false );
  visualMonitor.SetScreenShotsBaseFileName("registrationLog");

  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader->SetFileName( fixedMeshReader1->GetFileName() );
  vtkMovingMeshReader->SetFileName( movingMeshReader1->GetFileName() );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );
#endif


  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cout << "Reason " << e << std::endl;
    return EXIT_FAILURE;
    }


  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << optimizer->GetValue() << std::endl;


  transform->SetParameters( finalParameters );

  typedef FixedMeshType::Traits   MeshTraits;
  typedef itk::PointSet< MeshPixelType, Dimension, MeshTraits >   PointSetType;

  typedef itk::DeformationFieldFromTransformMeshFilter<
    FixedMeshType, PointSetType >  DeformationFieldFromTransformFilterType;

  DeformationFieldFromTransformFilterType::Pointer deformationFieldFromTransform =
    DeformationFieldFromTransformFilterType::New();

  deformationFieldFromTransform->SetInput( fixedMeshReader1->GetOutput() );
  deformationFieldFromTransform->SetTransform( transform );

  try
    {
    deformationFieldFromTransform->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Transform Parameters = " << transform->GetParameters() << std::endl;

  PointSetType::ConstPointer destinationPoints = deformationFieldFromTransform->GetOutput();

  typedef FixedMeshType::PointType  PointType;
  typedef PointType::VectorType     VectorType;

  typedef itk::QuadEdgeMeshTraits< VectorType, Dimension, bool, bool > VectorPointSetTraits;

  typedef itk::QuadEdgeMesh< VectorType, Dimension, VectorPointSetTraits > MeshWithVectorsType;

  typedef itk::QuadEdgeMeshGenerateDeformationFieldFilter<
    FixedMeshType, PointSetType, MeshWithVectorsType >   DeformationFilterType;

  DeformationFilterType::Pointer deformationFilter = DeformationFilterType::New();

  deformationFilter->SetInputMesh( fixedMeshReader1->GetOutput() );
  deformationFilter->SetDestinationPoints( destinationPoints );
  deformationFilter->Update();

  typedef itk::QuadEdgeMeshVectorDataVTKPolyDataWriter< MeshWithVectorsType >  VectorMeshWriterType;
  VectorMeshWriterType::Pointer vectorMeshWriter = VectorMeshWriterType::New();
  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh0.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh0.vtk  Saved" << std::endl;

  DemonsFilterType::Pointer demonsFilter = DemonsFilterType::New();

  demonsFilter->SetFixedMesh( fixedMeshReader1->GetOutput() );
  demonsFilter->SetMovingMesh( movingMeshReader1->GetOutput() );

  DemonsFilterType::PointType center;
  center.Fill( 0.0 );

  const double radius = 100.0;

  demonsFilter->SetSphereCenter( center );
  demonsFilter->SetSphereRadius( radius );

  const double epsilon = 1.0;
  const double sigmaX = 1.0;
  const double lambda = 1.0;
  const unsigned int maximumNumberOfSmoothingIterations = 10;
  const unsigned int maximumNumberOfIterations = 20;

  demonsFilter->SetEpsilon( epsilon );
  demonsFilter->SetSigmaX( sigmaX );
  demonsFilter->SetMaximumNumberOfIterations( maximumNumberOfIterations );

  demonsFilter->SetLambda( lambda );
  demonsFilter->SetMaximumNumberOfSmoothingIterations( maximumNumberOfSmoothingIterations );

  //
  // Initialize the deformable registration stage with 
  // the results of the Rigid Registration.
  //
  demonsFilter->SetInitialDestinationPoints( destinationPoints );

#ifdef USE_VTK
  visualMonitor.SetBaseAnnotationText("Demons Registration Level 1");
  visualMonitor.Observe( demonsFilter.GetPointer() );
  visualMonitor.ObserveData( demonsFilter->GetFinalDestinationPoints() );
#endif

  try
    {
    demonsFilter->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< FixedMeshType >   WriterType;
  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( argv[3] );
  writer->SetInput( demonsFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  DestinationPointSetType::Pointer finalDestinationPoints = demonsFilter->GetFinalDestinationPoints();
  
  finalDestinationPoints->DisconnectPipeline();

  deformationFilter->SetInputMesh( fixedMeshReader1->GetOutput() );
  deformationFilter->SetDestinationPoints( finalDestinationPoints );
std::cout << "BEFORE Computing deformation field " << std::endl;
  deformationFilter->Update();
std::cout << "AFTER Computing deformation field " << std::endl;

  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh1.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh1.vtk  Saved" << std::endl;

  writer->SetInput( demonsFilter->GetDeformedFixedMesh() );
  writer->SetFileName("DeformedMesh1.vtk");
  writer->Update(); 
  std::cout << "Deformation DeformedMesh1.vtk  Saved" << std::endl;
 

  //
  //  Starting process for the second resolution level (IC5).
  // 
  FixedReaderType::Pointer fixedMeshReader2 = FixedReaderType::New();
  fixedMeshReader2->SetFileName( argv[4] );

  MovingReaderType::Pointer movingMeshReader2 = MovingReaderType::New();
  movingMeshReader2->SetFileName( argv[5] );

std::cout << "Fixed  mesh second level = " << argv[4] << std::endl;
std::cout << "Moving mesh second level = " << argv[5] << std::endl;
  try
    {
    fixedMeshReader2->Update();
    movingMeshReader2->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Supersample the list of destination points using the mesh at the next resolution level.
  //
  typedef itk::ResampleDestinationPointsQuadEdgeMeshFilter< 
    PointSetType, FixedMeshType, FixedMeshType, PointSetType > UpsampleDestinationPointsFilterType;

  UpsampleDestinationPointsFilterType::Pointer upsampleDestinationPoints = 
    UpsampleDestinationPointsFilterType::New();

  upsampleDestinationPoints->SetSphereCenter( center );
  upsampleDestinationPoints->SetSphereRadius( radius );
  upsampleDestinationPoints->SetInput( finalDestinationPoints );
  upsampleDestinationPoints->SetFixedMesh( fixedMeshReader1->GetOutput() );
  upsampleDestinationPoints->SetReferenceMesh( fixedMeshReader2->GetOutput() );
  upsampleDestinationPoints->SetTransform( itk::IdentityTransform<double>::New() );

  try
    {
std::cout << "BEFORE upsampleDestinationPoints Update()" << std::endl;
    upsampleDestinationPoints->Update();
std::cout << "AFTER upsampleDestinationPoints Update()" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Here build a Mesh using the upsampled destination points and
  // the scalar values of the fixed IC5 mesh.

  FixedMeshType::Pointer fixedMesh2 = fixedMeshReader2->GetOutput();
  fixedMesh2->DisconnectPipeline();


  PointSetType::ConstPointer upsampledPointSet = upsampleDestinationPoints->GetOutput();

  const PointSetType::PointsContainer * upsampledPoints = upsampledPointSet->GetPoints();

  PointSetType::PointsContainerConstIterator upsampledPointsItr = upsampledPoints->Begin();
  PointSetType::PointsContainerConstIterator upsampledPointsEnd = upsampledPoints->End();

  FixedMeshType::PointsContainer::Pointer fixedPoints2 = fixedMesh2->GetPoints();

  FixedMeshType::PointsContainerIterator fixedPoint2Itr = fixedPoints2->Begin();

  while( upsampledPointsItr != upsampledPointsEnd )
    {
    // Point in the QuadEdgeMesh must also keep their pointer to Edge
    fixedPoint2Itr.Value().SetPoint( upsampledPointsItr.Value() );
    ++fixedPoint2Itr;
    ++upsampledPointsItr;
    }


  // 
  // Now feed this mesh into the Rigid registration of the second resolution level.
  //

  registration->SetFixedMesh( fixedMesh2 );
  registration->SetMovingMesh( movingMeshReader2->GetOutput() );

  transform->SetIdentity();
  parameters = transform->GetParameters();

  registration->SetInitialTransformParameters( parameters );

  //
  //   Save deformed fixed mesh, before initiating rigid registration
  //
  writer->SetInput( fixedMesh2 );
  writer->SetFileName("DeformedMesh2BeforeRigidRegistration.vtk");
  writer->Update(); 

#ifdef USE_VTK
  // 
  //  Reconnecting Registration monitor
  //
  vtkFixedMeshReader->SetFileName("DeformedMesh2BeforeRigidRegistration.vtk");
  vtkMovingMeshReader->SetFileName( movingMeshReader2->GetFileName() );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 2");
  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );
  visualMonitor.Observe( optimizer.GetPointer() );
  visualMonitor.ObserveData( transform.GetPointer() );
#endif

  // 
  //  Running Second Resolution Level Rigid Registration.
  //

  std::cout << "Running Second Resolution Level Rigid Registration." << std::endl;

  optimizer->SetMaximumStepLength( 0.02 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.8 );
  optimizer->SetNumberOfIterations( 32 );

  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cout << "Reason " << e << std::endl;
    return EXIT_FAILURE;
    }

  finalParameters = registration->GetLastTransformParameters();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << optimizer->GetValue() << std::endl;

  transform->SetParameters( finalParameters );

  deformationFieldFromTransform->SetInput( fixedMesh2 );
  deformationFieldFromTransform->SetTransform( transform );

  try
    {
    deformationFieldFromTransform->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }

  demonsFilter->SetInitialDestinationPoints( deformationFieldFromTransform->GetOutput() );

  demonsFilter->SetFixedMesh( fixedMesh2 );
  demonsFilter->SetMovingMesh( movingMeshReader2->GetOutput() );


#ifdef USE_VTK
  demonsFilter->MakeOutput(2);
  visualMonitor.SetBaseAnnotationText("Demons Registration Level 2");
  visualMonitor.Observe( demonsFilter.GetPointer() );
  visualMonitor.ObserveData( demonsFilter->GetFinalDestinationPoints() );
#endif

  // 
  //  Running Second Resolution Level Demons Registration.
  //
  std::cout << "Running Second Resolution Level Demons Registration." << std::endl;

  try
    {
    demonsFilter->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  writer->SetFileName( argv[6] );
  writer->SetInput( demonsFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  finalDestinationPoints = demonsFilter->GetFinalDestinationPoints();
  finalDestinationPoints->DisconnectPipeline();

  deformationFilter->SetInputMesh( fixedMeshReader2->GetOutput() );
  deformationFilter->SetDestinationPoints( finalDestinationPoints );
std::cout << "BEFORE Computing deformation field " << std::endl;
  deformationFilter->Update();
std::cout << "AFTER Computing deformation field " << std::endl;

  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh2.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh2.vtk  Saved" << std::endl;

  writer->SetInput( demonsFilter->GetDeformedFixedMesh() );
  writer->SetFileName("DeformedMesh2.vtk");
  writer->Update(); 
  std::cout << "Deformation DeformedMesh2.vtk  Saved" << std::endl;
 
  //
  //  Starting process for the Third resolution level (IC6).
  // 
  FixedReaderType::Pointer fixedMeshReader3 = FixedReaderType::New();
  fixedMeshReader3->SetFileName( argv[7] );

  MovingReaderType::Pointer movingMeshReader3 = MovingReaderType::New();
  movingMeshReader3->SetFileName( argv[8] );

std::cout << "Fixed  mesh third level = " << argv[7] << std::endl;
std::cout << "Moving mesh third level = " << argv[8] << std::endl;
  try
    {
    fixedMeshReader3->Update();
    movingMeshReader3->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Supersample the list of destination points using the mesh at the next resolution level.
  //
  upsampleDestinationPoints->SetInput( finalDestinationPoints );
  upsampleDestinationPoints->SetFixedMesh( fixedMesh2 );
  upsampleDestinationPoints->SetReferenceMesh( fixedMeshReader3->GetOutput() );
  upsampleDestinationPoints->SetTransform( itk::IdentityTransform<double>::New() );

  try
    {
std::cout << "BEFORE upsampleDestinationPoints Update()" << std::endl;
    upsampleDestinationPoints->Update();
std::cout << "AFTER upsampleDestinationPoints Update()" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Here build a Mesh using the upsampled destination points and
  // the scalar values of the fixed IC6 mesh.

  FixedMeshType::Pointer fixedMesh3 = fixedMeshReader3->GetOutput();
  fixedMesh3->DisconnectPipeline();

  upsampledPointSet = upsampleDestinationPoints->GetOutput();
  
  upsampledPoints = upsampledPointSet->GetPoints();

  upsampledPointsItr = upsampledPoints->Begin();
  upsampledPointsEnd = upsampledPoints->End();

  FixedMeshType::PointsContainer::Pointer fixedPoints3 = fixedMesh3->GetPoints();

  FixedMeshType::PointsContainerIterator fixedPoint3Itr = fixedPoints3->Begin();

  while( upsampledPointsItr != upsampledPointsEnd )
    {
    fixedPoint3Itr.Value().SetPoint( upsampledPointsItr.Value() );
    ++fixedPoint3Itr;
    ++upsampledPointsItr;
    }

  // 
  // Now feed this mesh into the Rigid registration of the third resolution level.
  //

  registration->SetFixedMesh( fixedMesh3 );
  registration->SetMovingMesh( movingMeshReader3->GetOutput() );

  transform->SetIdentity();
  parameters = transform->GetParameters();

  registration->SetInitialTransformParameters( parameters );

  //
  //   Save deformed fixed mesh, before initiating rigid registration
  //
  writer->SetInput( fixedMesh3 );
  writer->SetFileName("DeformedMesh3BeforeRigidRegistration.vtk");
  writer->Update(); 

#ifdef USE_VTK
  // 
  //  Reconnecting Registration monitor
  //
  vtkFixedMeshReader->SetFileName("DeformedMesh3BeforeRigidRegistration.vtk");
  vtkMovingMeshReader->SetFileName( movingMeshReader3->GetFileName() );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 3");
  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );
  visualMonitor.Observe( optimizer.GetPointer() );
  visualMonitor.ObserveData( transform.GetPointer() );
#endif

  // 
  //  Running Third Resolution Level Rigid Registration.
  //

  std::cout << "Running Third Resolution Level Rigid Registration." << std::endl;

  optimizer->SetMaximumStepLength( 0.01 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.6 );
  optimizer->SetNumberOfIterations( 16 );

  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cout << "Reason " << e << std::endl;
    return EXIT_FAILURE;
    }

  finalParameters = registration->GetLastTransformParameters();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << optimizer->GetValue() << std::endl;

  transform->SetParameters( finalParameters );

  deformationFieldFromTransform->SetInput( fixedMesh3 );
  deformationFieldFromTransform->SetTransform( transform );

  try
    {
    deformationFieldFromTransform->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }

  demonsFilter->SetInitialDestinationPoints( deformationFieldFromTransform->GetOutput() );

  demonsFilter->SetFixedMesh( fixedMesh3 );
  demonsFilter->SetMovingMesh( movingMeshReader3->GetOutput() );


#ifdef USE_VTK
  demonsFilter->MakeOutput(2);
  visualMonitor.SetBaseAnnotationText("Demons Registration Level 3");
  visualMonitor.Observe( demonsFilter.GetPointer() );
  visualMonitor.ObserveData( demonsFilter->GetFinalDestinationPoints() );
#endif

  // 
  //  Running Third Resolution Level Demons Registration.
  //
  std::cout << "Running Third Resolution Level Demons Registration." << std::endl;

  try
    {
    demonsFilter->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  writer->SetFileName( argv[9] );
  writer->SetInput( demonsFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  finalDestinationPoints = demonsFilter->GetFinalDestinationPoints();
  finalDestinationPoints->DisconnectPipeline();

  deformationFilter->SetInputMesh( fixedMeshReader3->GetOutput() );
  deformationFilter->SetDestinationPoints( finalDestinationPoints );
std::cout << "BEFORE Computing deformation field " << std::endl;
  deformationFilter->Update();
std::cout << "AFTER Computing deformation field " << std::endl;

  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh3.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh3.vtk  Saved" << std::endl;

  writer->SetInput( demonsFilter->GetDeformedFixedMesh() );
  writer->SetFileName("DeformedMesh3.vtk");
  writer->Update(); 
  std::cout << "Deformation DeformedMesh3.vtk  Saved" << std::endl;
 

  //
  //  Starting process for the Fourth resolution level (IC7).
  // 
  FixedReaderType::Pointer fixedMeshReader4 = FixedReaderType::New();
  fixedMeshReader4->SetFileName( argv[10] );

  MovingReaderType::Pointer movingMeshReader4 = MovingReaderType::New();
  movingMeshReader4->SetFileName( argv[11] );

std::cout << "Fixed  mesh fourth level = " << argv[10] << std::endl;
std::cout << "Moving mesh fourth level = " << argv[11] << std::endl;
  try
    {
    fixedMeshReader4->Update();
    movingMeshReader4->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Supersample the list of destination points using the mesh at the next resolution level.
  //
  upsampleDestinationPoints->SetInput( finalDestinationPoints );
  upsampleDestinationPoints->SetFixedMesh( fixedMesh3 );
  upsampleDestinationPoints->SetReferenceMesh( fixedMeshReader4->GetOutput() );
  upsampleDestinationPoints->SetTransform( itk::IdentityTransform<double>::New() );

  try
    {
std::cout << "BEFORE upsampleDestinationPoints Update()" << std::endl;
    upsampleDestinationPoints->Update();
std::cout << "AFTER upsampleDestinationPoints Update()" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Here build a Mesh using the upsampled destination points and
  // the scalar values of the fixed IC6 mesh.

  FixedMeshType::Pointer fixedMesh4 = fixedMeshReader4->GetOutput();
  fixedMesh4->DisconnectPipeline();

  upsampledPointSet = upsampleDestinationPoints->GetOutput();
  
  upsampledPoints = upsampledPointSet->GetPoints();

  upsampledPointsItr = upsampledPoints->Begin();
  upsampledPointsEnd = upsampledPoints->End();

  FixedMeshType::PointsContainer::Pointer fixedPoints4 = fixedMesh4->GetPoints();

  FixedMeshType::PointsContainerIterator fixedPoint4Itr = fixedPoints4->Begin();

  while( upsampledPointsItr != upsampledPointsEnd )
    {
    fixedPoint4Itr.Value().SetPoint( upsampledPointsItr.Value() );
    ++fixedPoint4Itr;
    ++upsampledPointsItr;
    }

  // 
  // Now feed this mesh into the Rigid registration of the third resolution level.
  //

  registration->SetFixedMesh( fixedMesh4 );
  registration->SetMovingMesh( movingMeshReader4->GetOutput() );

  transform->SetIdentity();
  parameters = transform->GetParameters();

  registration->SetInitialTransformParameters( parameters );


  //
  //   Save deformed fixed mesh, before initiating rigid registration
  //
  writer->SetInput( fixedMesh4 );
  writer->SetFileName("DeformedMesh4BeforeRigidRegistration.vtk");
  writer->Update(); 


#ifdef USE_VTK
  // 
  //  Reconnecting Registration monitor
  //
  vtkFixedMeshReader->SetFileName("DeformedMesh4BeforeRigidRegistration.vtk");
  vtkMovingMeshReader->SetFileName( movingMeshReader4->GetFileName() );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 4");
  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );
  visualMonitor.Observe( optimizer.GetPointer() );
  visualMonitor.ObserveData( transform.GetPointer() );
#endif

  // 
  //  Running Fourth Resolution Level Rigid Registration.
  //

  std::cout << "Running Fourth Resolution Level Rigid Registration." << std::endl;

  optimizer->SetMaximumStepLength( 0.005 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.5 );
  optimizer->SetNumberOfIterations( 8 );

  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cout << "Reason " << e << std::endl;
    return EXIT_FAILURE;
    }

  finalParameters = registration->GetLastTransformParameters();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << optimizer->GetValue() << std::endl;

  transform->SetParameters( finalParameters );

  deformationFieldFromTransform->SetInput( fixedMesh4 );
  deformationFieldFromTransform->SetTransform( transform );

  try
    {
    deformationFieldFromTransform->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }

  demonsFilter->SetInitialDestinationPoints( deformationFieldFromTransform->GetOutput() );

  demonsFilter->SetFixedMesh( fixedMesh4 );
  demonsFilter->SetMovingMesh( movingMeshReader4->GetOutput() );

#ifdef USE_VTK
  demonsFilter->MakeOutput(2);
  visualMonitor.SetBaseAnnotationText("Demons Registration Level 4");
  visualMonitor.Observe( demonsFilter.GetPointer() );
  visualMonitor.ObserveData( demonsFilter->GetFinalDestinationPoints() );
#endif

  // 
  //  Running Fourth Resolution Level Demons Registration.
  //
  std::cout << "Running Fourth Resolution Level Demons Registration." << std::endl;

  try
    {
    demonsFilter->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  writer->SetFileName( argv[12] );
  writer->SetInput( demonsFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  finalDestinationPoints = demonsFilter->GetFinalDestinationPoints();
  finalDestinationPoints->DisconnectPipeline();
  
  deformationFilter->SetInputMesh( fixedMeshReader4->GetOutput() );
  deformationFilter->SetDestinationPoints( finalDestinationPoints );
std::cout << "BEFORE Computing deformation field " << std::endl;
  deformationFilter->Update();
std::cout << "AFTER Computing deformation field " << std::endl;

  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh4.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh4.vtk  Saved" << std::endl;

  writer->SetFileName("DeformedMesh4.vtk");
  writer->SetInput( demonsFilter->GetDeformedFixedMesh() );
  writer->Update();
  std::cout << "Deformation DeformedMesh4.vtk  Saved" << std::endl;



  //
  //  Starting process for the Fifth resolution level (FINAL).
  // 
  FixedReaderType::Pointer fixedMeshReader5 = FixedReaderType::New();
  fixedMeshReader5->SetFileName( argv[13] );

  MovingReaderType::Pointer movingMeshReader5 = MovingReaderType::New();
  movingMeshReader5->SetFileName( argv[14] );

std::cout << "Fixed  mesh fifth level = " << argv[13] << std::endl;
std::cout << "Moving mesh fifth level = " << argv[14] << std::endl;
  try
    {
    fixedMeshReader5->Update();
    movingMeshReader5->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Supersample the list of destination points using the mesh at the next resolution level.
  //
  upsampleDestinationPoints->SetInput( finalDestinationPoints );
  upsampleDestinationPoints->SetFixedMesh( fixedMesh4 );
  upsampleDestinationPoints->SetReferenceMesh( fixedMeshReader5->GetOutput() );
  upsampleDestinationPoints->SetTransform( itk::IdentityTransform<double>::New() );

  try
    {
std::cout << "BEFORE upsampleDestinationPoints Update()" << std::endl;
    upsampleDestinationPoints->Update();
std::cout << "AFTER upsampleDestinationPoints Update()" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Here build a Mesh using the upsampled destination points and
  // the scalar values of the fixed FINAL mesh.

  FixedMeshType::Pointer fixedMesh5 = fixedMeshReader5->GetOutput();
  fixedMesh5->DisconnectPipeline();

  upsampledPointSet = upsampleDestinationPoints->GetOutput();
  
  deformationFilter->SetInputMesh( fixedMeshReader5->GetOutput() );
  deformationFilter->SetDestinationPoints( upsampleDestinationPoints->GetOutput() );
std::cout << "BEFORE Computing deformation field " << std::endl;
  deformationFilter->Update();
std::cout << "AFTER Computing deformation field " << std::endl;

  vectorMeshWriter->SetInput( deformationFilter->GetOutput() );
  vectorMeshWriter->SetFileName("VectorMesh5.vtk");
  vectorMeshWriter->Update(); 
  std::cout << "Deformation VectorMesh5.vtk  Saved" << std::endl;

  upsampledPoints = upsampledPointSet->GetPoints();

  upsampledPointsItr = upsampledPoints->Begin();
  upsampledPointsEnd = upsampledPoints->End();

  FixedMeshType::PointsContainer::Pointer fixedPoints5 = fixedMesh5->GetPoints();

  FixedMeshType::PointsContainerIterator fixedPoint5Itr = fixedPoints5->Begin();

  while( upsampledPointsItr != upsampledPointsEnd )
    {
    fixedPoint5Itr.Value().SetPoint( upsampledPointsItr.Value() );
    ++fixedPoint5Itr;
    ++upsampledPointsItr;
    }

  writer->SetFileName( argv[15] );
  writer->SetInput( fixedMesh5 );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  writer->SetFileName("DeformedMesh5.vtk");
  writer->SetInput( fixedMesh5 );
  writer->Update();
  std::cout << "Deformation DeformedMesh5.vtk  Saved" << std::endl;

  return EXIT_SUCCESS;
}
