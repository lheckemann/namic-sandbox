/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ITKHeader.h,v $
  Language:  C++
  Date:      $Date: 2006-04-25 23:20:16 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx
#define __itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx

#include "itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"
#include "itkProgressReporter.h"

#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"

#include "itkResampleQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshTraits.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshVectorDataVTKPolyDataWriter.h"
#include "itkDeformationFieldFromTransformMeshFilter.h"
#include "itkResampleDestinationPointsQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshGenerateDeformationFieldFilter.h"
#include "itkIdentityTransform.h"
#include "itkReplaceDestinationPointsQuadEdgeMeshFilter.h"
#include "itkWarpQuadEdgeMeshFilter.h"


namespace itk
{

template< class TMesh >
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
  this->SetNumberOfInputs( 8 ); // four resolution levels, two meshes on each
  this->SetNumberOfOutputs( 1 );

  this->SetNumberOfRequiredInputs( 8 ); // four resolution levels, two meshes on each
  this->SetNumberOfRequiredOutputs( 1 );

  this->SetNthOutput( 0, TMesh::New() );

  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;

  this->m_RigidTransform = TransformType::New();

  this->m_RigidOptimizer = RigidOptimizerType::New();

  this->m_CurrentLevelFixedMesh = MeshType::New();
  this->m_CurrentLevelMovingMesh = MeshType::New();

  this->m_DemonsRegistrationFilter = DemonsRegistrationFilterType::New();

  this->m_CurrentResolutionLevel = 0;
  this->m_NumberOfResolutionLevels = 4;
}


template< class TMesh >
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::~MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
}


template< class TMesh >
DataObject::Pointer
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::MakeOutput(unsigned int idx)
{
  DataObject::Pointer output;
  switch( idx )
    {
    case 0:
      output = (TMesh::New()).GetPointer();
      break;
    }
  return output.GetPointer();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::SetFixedMesh( const MeshType * fixedMesh )
{
  itkDebugMacro("setting Fixed Mesh to " << fixedMesh );

  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, const_cast< MeshType *>( fixedMesh ) );

  this->Modified();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::SetMovingMesh( const MeshType * movingMesh )
{
  itkDebugMacro("setting Moving Mesh to " << movingMesh );

  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1, const_cast< MeshType *>( movingMesh ) );

  this->Modified();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::SetFixedMesh( unsigned int level, const MeshType * fixedMesh )
{
  itkDebugMacro("setting Fixed Mesh to " << fixedMesh );

  const unsigned int inputNumber = 2 * level;
std::cout << "inputNumber = " << inputNumber << " Fixed image with # points = " << fixedMesh->GetNumberOfPoints() << std::endl;
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( inputNumber, const_cast< MeshType *>( fixedMesh ) );

  this->Modified();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::SetMovingMesh( unsigned int level, const MeshType * movingMesh )
{
  itkDebugMacro("setting Moving Mesh to " << movingMesh );

  const unsigned int inputNumber = 2 * level + 1;
std::cout << "inputNumber = " << inputNumber << " Moving image with # points = " << movingMesh->GetNumberOfPoints() << std::endl;
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(inputNumber, const_cast< MeshType *>( movingMesh ) );

  this->Modified();
}



template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::GenerateData()
{
  this->PrepareCoarsestResolutionMeshes();

  while( this->m_CurrentResolutionLevel < this->m_NumberOfResolutionLevels )
    {
    std::cout << "RESOLUTION LEVEL = " << this->m_CurrentResolutionLevel << std::endl;
    this->ComputeRigidRegistration();
    this->RigidlyTransformPointsOfFixedMesh();
    this->ComputeDemonsRegistration();
    this->PrepareNextResolutionLevelMeshes();
    this->DeformNextResolutionLevelFixedMesh();
    this->m_CurrentResolutionLevel++;
    }

}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::PrepareCoarsestResolutionMeshes()
{
  this->m_CurrentResolutionLevel = 0;

  TMesh * fixedMesh  = static_cast<MeshType *>(this->ProcessObject::GetInput( 0 ));
  TMesh * movingMesh = static_cast<MeshType *>(this->ProcessObject::GetInput( 1 ));

  this->m_CurrentLevelFixedMesh = MeshType::New();
  this->m_CurrentLevelMovingMesh = MeshType::New();

  this->CopyMeshToMesh( fixedMesh,  this->m_CurrentLevelFixedMesh  );
  this->CopyMeshToMesh( movingMesh, this->m_CurrentLevelMovingMesh );
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::PrepareNextResolutionLevelMeshes()
{
  const unsigned int fixedInput  = ( this->m_CurrentResolutionLevel + 1 ) * 2;
  const unsigned int movingInput = ( this->m_CurrentResolutionLevel + 1 ) * 2 + 1;

std::cout << "fixedInput = " << fixedInput << std::endl;
std::cout << "movingInput = " << movingInput << std::endl;

  // 
  //   Prepare meshes for next resolution level
  // 
  TMesh * fixedMesh  = static_cast<MeshType *>(this->ProcessObject::GetInput( fixedInput ));
  TMesh * movingMesh = static_cast<MeshType *>(this->ProcessObject::GetInput( movingInput ));

  this->m_NextLevelFixedMesh = MeshType::New();
  this->m_NextLevelMovingMesh = MeshType::New();

std::cout << "fixedInput  # points = " << fixedMesh->GetNumberOfPoints()  << std::endl;
std::cout << "movingInput # points = " << movingMesh->GetNumberOfPoints() << std::endl;

  this->CopyMeshToMesh( fixedMesh,  this->m_NextLevelFixedMesh  );
  this->CopyMeshToMesh( movingMesh, this->m_NextLevelMovingMesh );
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::ComputeRigidRegistration()
{
#ifdef USE_VTK
  this->m_RegistrationMonitor->Observe( this->GetRigidOptimizer() );
  this->m_RegistrationMonitor->ObserveData( this->GetRigidTransform() );
#endif

  typedef MeshToMeshRegistrationMethod< MeshType, MeshType >    RegistrationType;

  typedef MeanSquaresMeshToMeshMetric< MeshType, MeshType >   MetricType;

  typename RegistrationType::Pointer   registration  = RegistrationType::New();

  typename MetricType::Pointer  metric = MetricType::New();

  registration->SetMetric( metric ); 

  std::cout << "Current fixed Mesh points = " << this->m_CurrentLevelFixedMesh->GetNumberOfPoints() << std::endl;
  std::cout << "Current moving Mesh points = " << this->m_CurrentLevelMovingMesh->GetNumberOfPoints() << std::endl;

  registration->SetFixedMesh( this->m_CurrentLevelFixedMesh );
  registration->SetMovingMesh( this->m_CurrentLevelMovingMesh );

  registration->SetTransform( this->m_RigidTransform );

  typedef LinearInterpolateMeshFunction< MeshType > InterpolatorType;

  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

  registration->SetInterpolator( interpolator );

  const unsigned int numberOfTransformParameters =
    this->m_RigidTransform->GetNumberOfParameters();

  typedef typename TransformType::ParametersType         ParametersType;
  ParametersType parameters( numberOfTransformParameters );

  this->m_RigidTransform->SetIdentity();
  
  parameters = this->m_RigidTransform->GetParameters();

  registration->SetInitialTransformParameters( parameters );

  registration->SetOptimizer( this->m_RigidOptimizer );

  typedef RigidOptimizerType::ScalesType             ScalesType;

  ScalesType    parametersScale( numberOfTransformParameters );
  parametersScale[0] = 1.0;
  parametersScale[1] = 1.0;
  parametersScale[2] = 1.0;

  this->m_RigidOptimizer->SetScales( parametersScale );

  this->m_RigidOptimizer->MinimizeOn();
  this->m_RigidOptimizer->SetGradientMagnitudeTolerance( 1e-6 );
  this->m_RigidOptimizer->SetMaximumStepLength( 1e-2 );
  this->m_RigidOptimizer->SetMinimumStepLength( 1e-9 );
  this->m_RigidOptimizer->SetRelaxationFactor( 0.9 );
  this->m_RigidOptimizer->SetNumberOfIterations( 32 );

  try
    {
    registration->StartRegistration();
    }
  catch( ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cerr << "Reason " << e << std::endl;
    throw e;
    }

  ParametersType finalParameters = registration->GetLastTransformParameters();

  this->m_RigidTransform->SetParameters( finalParameters );
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::RigidlyTransformPointsOfFixedMesh()
{
  PointsContainer * fixedPoints = this->m_CurrentLevelFixedMesh->GetPoints();

  PointsContainerIterator fixedPointItr = fixedPoints->Begin();
  PointsContainerIterator fixedPointEnd = fixedPoints->End();

  while( fixedPointItr != fixedPointEnd )
    {
    fixedPointItr.Value().SetPoint(
      this->m_RigidTransform->TransformPoint( fixedPointItr.Value() ) );
    ++fixedPointItr;
    }
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::ComputeDemonsRegistration()
{
#ifdef USE_VTK
  this->m_RegistrationMonitor->Observe( this->GetDemonsRegistrationFilter() );
  this->m_RegistrationMonitor->ObserveData( this->GetCurrentDestinationPoints() );
#endif

  this->m_DemonsRegistrationFilter->SetFixedMesh( this->m_CurrentLevelFixedMesh );
  this->m_DemonsRegistrationFilter->SetMovingMesh( this->m_CurrentLevelMovingMesh );

  this->m_DemonsRegistrationFilter->SetSphereCenter( this->m_SphereCenter );
  this->m_DemonsRegistrationFilter->SetSphereRadius( this->m_SphereRadius );

  this->m_DemonsRegistrationFilter->SetEpsilon( 0.016 );
  this->m_DemonsRegistrationFilter->SetSigmaX( 8.0 );

  this->m_DemonsRegistrationFilter->SelfRegulatedModeOn(); 

  this->m_DemonsRegistrationFilter->SetLambda( 1.0 );

  this->m_DemonsRegistrationFilter->SetMaximumNumberOfIterations( 15 );
  this->m_DemonsRegistrationFilter->SetMaximumNumberOfSmoothingIterations( 2 );
std::cout << "STARTING DEMONS" << std::endl;
  try
    {
    this->m_DemonsRegistrationFilter->Update();
    }
  catch( ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    throw exp;
    }
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::DeformNextResolutionLevelFixedMesh()
{
  //
  //   Deform fixed new mesh using current fixed mesh and destination points
  //
  typedef WarpQuadEdgeMeshFilter< MeshType, MeshType, DestinationPointSetType > WarpFilterType;

  typename WarpFilterType::Pointer warpFilter = WarpFilterType::New();

  warpFilter->SetInput( this->m_NextLevelFixedMesh );
  warpFilter->SetReferenceMesh( this->m_CurrentLevelFixedMesh );

  typename DestinationPointSetType::Pointer currentDestinationPoints =
    this->m_DemonsRegistrationFilter->GetFinalDestinationPoints();

  currentDestinationPoints->DisconnectPipeline();

  // Create the new destination points for the following iteration
  this->m_DemonsRegistrationFilter->MakeOutput(2);

  warpFilter->SetDestinationPoints( currentDestinationPoints );

  warpFilter->SetSphereRadius( this->m_SphereRadius );
  warpFilter->SetSphereCenter( this->m_SphereCenter );

  try
    {
    warpFilter->Update();
    }
  catch( ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    throw excp;
    }

  this->m_CurrentLevelFixedMesh = warpFilter->GetOutput();

  this->m_CurrentLevelFixedMesh->DisconnectPipeline();

  this->m_CurrentLevelMovingMesh = this->m_NextLevelMovingMesh;
}


template< class TMesh >
const typename MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >::DestinationPointSetType *
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::GetCurrentDestinationPoints() const
{
  return this->m_DemonsRegistrationFilter->GetFinalDestinationPoints();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "Sphere center: " << this->m_SphereCenter << std::endl;
  os << "Sphere radius: " << this->m_SphereRadius << std::endl;
}

}

#endif
