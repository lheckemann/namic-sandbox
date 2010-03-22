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
#include "itkVersorTransformOptimizer.h"
#include "itkVersorTransform.h"

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
  this->ComputeRigidRegistration();
}


template< class TMesh >
void
MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TMesh >
::ComputeRigidRegistration()
{
  typedef itk::MeshToMeshRegistrationMethod< MeshType, MeshType >    RegistrationType;

  typedef itk::MeanSquaresMeshToMeshMetric< MeshType, MeshType >   MetricType;

  typename RegistrationType::Pointer   registration  = RegistrationType::New();

  typename MetricType::Pointer  metric = MetricType::New();

  registration->SetMetric( metric ); 

  const TMesh * fixedMesh = 
    static_cast<MeshType *>(this->ProcessObject::GetInput( 0 ));

  const TMesh * movingMesh = 
    static_cast<MeshType *>(this->ProcessObject::GetInput( 1 ));

  registration->SetFixedMesh( fixedMesh );
  registration->SetMovingMesh( movingMesh );

  typedef itk::VersorTransform< typename MetricType::TransformComputationType >  TransformType;

  typename TransformType::Pointer transform = TransformType::New();

  registration->SetTransform( transform );

  typedef itk::LinearInterpolateMeshFunction< MeshType > InterpolatorType;

  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

  registration->SetInterpolator( interpolator );

  const unsigned int numberOfTransformParameters = transform->GetNumberOfParameters();

  typedef typename TransformType::ParametersType         ParametersType;
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
  optimizer->SetMaximumStepLength( 1e-2 );
  optimizer->SetMinimumStepLength( 1e-9 );
  optimizer->SetRelaxationFactor( 0.9 );
  optimizer->SetNumberOfIterations( 32 );

  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Registration failed" << std::endl;
    std::cout << "Reason " << e << std::endl;
    throw e;
    }

  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  std::cout << "final parameters = " << finalParameters << std::endl;
  std::cout << "final value      = " << optimizer->GetValue() << std::endl;
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
