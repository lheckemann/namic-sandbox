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


namespace itk
{

template< class TMesh >
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
  this->SetNumberOfInputs( 2 );
  this->SetNumberOfOutputs( 1 );

  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );

  this->SetNthOutput( 0, TMesh::New() );

  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;

  this->m_RigidTransform = TransformType::New();

  this->m_RigidOptimizer = RigidOptimizerType::New();

  this->m_CurrentLevelFixedMesh = MeshType::New();
  this->m_CurrentLevelMovingMesh = MeshType::New();

  this->m_DemonsRegistrationFilter = DemonsRegistrationFilterType::New();
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
::GenerateData()
{
  this->PrepareCoarsestResolutionMeshes();
  this->ComputeRigidRegistration();
  this->RigidlyTransformPointsOfFixedMesh();
  this->ComputeDemonsRegistration();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::PrepareCoarsestResolutionMeshes()
{
  TMesh * fixedMesh  = static_cast<MeshType *>(this->ProcessObject::GetInput( 0 ));
  TMesh * movingMesh = static_cast<MeshType *>(this->ProcessObject::GetInput( 1 ));

  this->m_CurrentLevelFixedMesh = MeshType::New();
  this->m_CurrentLevelMovingMesh = MeshType::New();

  this->CopyMeshToMesh( fixedMesh,  this->m_CurrentLevelFixedMesh  );
  this->CopyMeshToMesh( movingMesh, this->m_CurrentLevelMovingMesh );

  this->m_CurrentLevelFixedMesh = fixedMesh;
  this->m_CurrentLevelMovingMesh = movingMesh;
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

  typedef itk::MeshToMeshRegistrationMethod< MeshType, MeshType >    RegistrationType;

  typedef itk::MeanSquaresMeshToMeshMetric< MeshType, MeshType >   MetricType;

  typename RegistrationType::Pointer   registration  = RegistrationType::New();

  typename MetricType::Pointer  metric = MetricType::New();

  registration->SetMetric( metric ); 

  registration->SetFixedMesh( this->m_CurrentLevelFixedMesh );
  registration->SetMovingMesh( this->m_CurrentLevelMovingMesh );

  registration->SetTransform( this->m_RigidTransform );

  typedef itk::LinearInterpolateMeshFunction< MeshType > InterpolatorType;

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
  catch( itk::ExceptionObject & e )
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
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    throw exp;
    }
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
