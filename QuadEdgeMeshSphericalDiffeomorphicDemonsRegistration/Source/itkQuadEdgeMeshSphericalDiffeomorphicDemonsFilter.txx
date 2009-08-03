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

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkProgressReporter.h"

namespace itk
{

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );
  this->SetNthOutput( 0, OutputMeshType::New() );

  this->m_BasisSystemAtNode = BasisSystemContainerType::New();
  this->m_DestinationPoints = DestinationPointContainerType::New();
  this->m_DestinationPointsSwap = DestinationPointContainerType::New();

  this->m_TriangleListBasisSystemCalculator = TriangleListBasisSystemCalculatorType::New(); 
  
  this->m_NodeScalarGradientCalculator = NodeScalarGradientCalculatorType::New(); 

  this->m_NodeVectorJacobianCalculator = NodeVectorJacobianCalculatorType::New(); 

  this->m_ResampledMovingValuesContainer = ResampledMovingValuesContainerType::New();

  this->m_ScalarInterpolator = ScalarInterpolatorType::New();

  this->m_DeformationInterpolator = DeformationInterpolatorType::New();

  this->m_MaximumNumberOfIterations = 50;

  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;
  this->m_Gamma = 1.0;

  this->m_SigmaX = 1.0;

  this->m_Lambda = 1.0;
  this->m_MaximumNumberOfSmoothingIterations = 1;

  this->m_ShortestEdgeLength = 1.0;
  this->m_ScalingAndSquaringNumberOfIterations = 2;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::SetFixedMesh( const FixedMeshType * fixedMesh )
{
  itkDebugMacro("setting Fixed Mesh to " << fixedMesh ); 

  if (this->m_FixedMesh.GetPointer() != fixedMesh ) 
    { 
    this->m_FixedMesh = fixedMesh;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0, 
                                   const_cast< FixedMeshType *>( fixedMesh ) );
    
    this->Modified(); 
    } 
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::SetMovingMesh( const MovingMeshType * movingMesh )
{
  itkDebugMacro("setting Moving Mesh to " << movingMesh ); 

  if (this->m_MovingMesh.GetPointer() != movingMesh ) 
    { 
    this->m_MovingMesh = movingMesh;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(1, const_cast< MovingMeshType *>( movingMesh ) );
    
    this->Modified(); 
    } 
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
GenerateData()
{
  this->CopyInputMeshToOutputMesh();
  this->AllocateInternalArrays();
  this->InitializeFixedNodesSigmas();
  this->ComputeBasisSystemAtEveryNode();
  this->ComputeShortestEdgeLength();
  this->ComputeInitialArrayOfDestinationPoints();
  this->InitializeInterpolators();
  this->RunIterations();
  this->PrintOutDeformationVectors();
  this->ComputeMappedMovingValueAtEveryNode();
  this->AssignResampledMovingValuesToOutputMesh();
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
AllocateInternalArrays()
{
  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  //
  // create new containers and allocate memory for them, in case the filter has
  // been run previously with a mesh having a larger number of nodes than the
  // current mesh.
  //

  this->m_BasisSystemAtNode = BasisSystemContainerType::New();
  this->m_BasisSystemAtNode->Reserve( numberOfNodes );

  this->m_DestinationPoints = DestinationPointContainerType::New();
  this->m_DestinationPoints->Reserve( numberOfNodes );

  this->m_DestinationPointsSwap = DestinationPointContainerType::New();
  this->m_DestinationPointsSwap->Reserve( numberOfNodes );

  this->m_DisplacementField = DestinationPointContainerType::New();
  this->m_DisplacementField->Reserve( numberOfNodes );

  this->m_DisplacementFieldSwap = DestinationPointContainerType::New();
  this->m_DisplacementFieldSwap->Reserve( numberOfNodes );

  this->m_ResampledMovingValuesContainer = ResampledMovingValuesContainerType::New();
  this->m_ResampledMovingValuesContainer->Reserve( numberOfNodes );

  this->m_VelocityField = VelocityVectorContainer::New();
  this->m_VelocityField->Reserve( numberOfNodes );

  this->m_TangentVectorField = TangentVectorContainer::New();
  this->m_TangentVectorField->Reserve( numberOfNodes );

  this->m_TangentVectorFieldSwap = TangentVectorContainer::New();
  this->m_TangentVectorFieldSwap->Reserve( numberOfNodes );
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
InitializeFixedNodesSigmas()
{
  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  if( this->m_FixedNodesSigmas.IsNull() || this->m_FixedNodesSigmas->Size() != numberOfNodes )
    {
    NodeSigmaContainerPointer sigmas = NodeSigmaContainerType::New();
    sigmas->Reserve( numberOfNodes );
    NodeSigmaContainerIterator sigmaItr = sigmas->Begin();
    NodeSigmaContainerIterator sigmaEnd = sigmas->End();
    while( sigmaItr != sigmaEnd )
      {
      sigmaItr.Value() = 1.0;
      ++sigmaItr;
      }
    this->SetFixedNodesSigmas( sigmas );
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeBasisSystemAtEveryNode()
{
  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  typedef typename TFixedMesh::PointsContainer    PointsContainer;
  const PointsContainer * points = this->m_FixedMesh->GetPoints();

  typedef typename TFixedMesh::QEPrimal           EdgeType;
  typedef typename TFixedMesh::PointType          PointType;
  typedef typename PointType::VectorType          VectorType;

  for( PointIdentifier pointId1 = 0; pointId1 < numberOfNodes; pointId1++ )
    {
    const EdgeType * edge = this->m_FixedMesh->FindEdge( pointId1 );
    PointIdentifier pointId2 = edge->GetDestination();

    const PointType point1 = points->GetElement( pointId1 );
    const PointType point2 = points->GetElement( pointId2 );

    const VectorType v12    = point1 - point2;

    // v12 is not necessarily tangent to the sphere, therefore we must use
    // cross products in order to find an orthogonal system.

    const VectorType radial = point1.GetVectorFromOrigin();

    VectorType u12 = CrossProduct( v12, radial );
    VectorType w12 = CrossProduct( radial, u12 );

    w12.Normalize();
    u12.Normalize();

    BasisSystemType basis;
    basis.SetVector( 0, w12 );
    basis.SetVector( 1, u12 );

    this->m_BasisSystemAtNode->SetElement( pointId1, basis );
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeInitialArrayOfDestinationPoints()
{
  //
  // We start with a deformation field defining an identity transform.
  // This should be modified if we ever take an initial deformation field
  // as an input.
  //
  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator srcPointItr = points->Begin();

  DestinationPointIterator dstPointItr = this->m_DestinationPoints->Begin();
  DestinationPointIterator dstPointEnd = this->m_DestinationPoints->End();

  while( dstPointItr != dstPointEnd )
    {
    dstPointItr.Value() = srcPointItr.Value();
    ++srcPointItr;
    ++dstPointItr;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
InitializeInterpolators()
{
  this->m_ScalarInterpolator->SetInputMesh( this->m_MovingMesh );
  this->m_ScalarInterpolator->Initialize();

  this->m_DeformationInterpolator->SetInputMesh( this->m_FixedMesh );
  this->m_DeformationInterpolator->Initialize();
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
RunIterations()
{
  // Report the progress
  ProgressReporter progress( this, 0, this->m_MaximumNumberOfIterations );
  
  for( unsigned int i = 0; i < this->m_MaximumNumberOfIterations; i++ )
    {
    this->ComputeMappedMovingValueAtEveryNode();
    this->ComputeGradientsOfMappedMovingValueAtEveryNode();
    this->ComputeVelocityField();
    this->ComputeScalingAndSquaringNumberOfIterations();
    this->ComputeDeformationByScalingAndSquaring();
    this->ComposeDeformationUpdateWithPreviousDeformation();
    this->SmoothDeformationField();

    // Report progress via Events
    progress.CompletedPixel();
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeGradientsOfMappedMovingValueAtEveryNode()
{
  this->m_TriangleListBasisSystemCalculator->SetInputMesh( this->m_FixedMesh );
  this->m_TriangleListBasisSystemCalculator->Calculate();

  this->m_NodeScalarGradientCalculator->SetInputMesh( this->m_FixedMesh );
  this->m_NodeScalarGradientCalculator->SetDataContainer( this->m_ResampledMovingValuesContainer );

  this->m_NodeScalarGradientCalculator->SetBasisSystemList( 
    this->m_TriangleListBasisSystemCalculator->GetBasisSystemList() );

  this->m_NodeScalarGradientCalculator->SetSphereCenter( this->m_SphereCenter );
  this->m_NodeScalarGradientCalculator->SetSphereRadius( this->m_SphereRadius );

  this->m_NodeScalarGradientCalculator->Initialize();
  this->m_NodeScalarGradientCalculator->Compute();

  this->m_NodeVectorJacobianCalculator->SetInputMesh( this->m_FixedMesh );
  this->m_NodeVectorJacobianCalculator->SetVectorContainer( this->m_DestinationPoints );

  this->m_NodeVectorJacobianCalculator->SetBasisSystemList( 
    this->m_TriangleListBasisSystemCalculator->GetBasisSystemList() );

  this->m_NodeVectorJacobianCalculator->SetSphereCenter( this->m_SphereCenter );
  this->m_NodeVectorJacobianCalculator->SetSphereRadius( this->m_SphereRadius );

  this->m_NodeVectorJacobianCalculator->Initialize();
  this->m_NodeVectorJacobianCalculator->Compute();
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeMappedMovingValueAtEveryNode()
{
  typedef typename DestinationPointContainerType::ConstIterator  DestinationPointIterator;

  DestinationPointIterator pointItr = this->m_DestinationPoints->Begin();
  DestinationPointIterator pointEnd = this->m_DestinationPoints->End();

  ResampledMovingValuesContainerIterator  resampledArrayItr = this->m_ResampledMovingValuesContainer->Begin();

  while( pointItr != pointEnd )
    {
    resampledArrayItr.Value() = this->m_ScalarInterpolator->Evaluate( pointItr.Value() );

    ++pointItr;
    ++resampledArrayItr;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeVelocityField()
{
  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator pointItr = points->Begin();

  const FixedPointDataContainer * pointData = this->m_FixedMesh->GetPointData();

  FixedPointDataConstIterator fixedPointDataItr = pointData->Begin();

  BasisSystemContainerIterator basisItr = this->m_BasisSystemAtNode->Begin();

  NodeSigmaContainerConstIterator sigmaItr = this->m_FixedNodesSigmas->Begin();

  VelocityVectorIterator velocityItr = this->m_VelocityField->Begin();

  ResampledMovingValuesContainerIterator  resampledArrayItr =
    this->m_ResampledMovingValuesContainer->Begin();

  typedef vnl_matrix_fixed<double,3,3>  VnlMatrix33Type;
  typedef vnl_vector_fixed<double,2>    VnlVector2Type;
  typedef vnl_vector_fixed<double,3>    VnlVector3Type;
  typedef vnl_matrix_fixed<double,3,2>  VnlMatrix32Type;
  typedef vnl_matrix_fixed<double,2,3>  VnlMatrix23Type;
  typedef vnl_matrix_fixed<double,2,2>  VnlMatrix22Type;
  typedef Vector<double, 3>             DoubleVectorType;

  VnlMatrix33Type Gn;
  VnlMatrix33Type Gn2;
  VnlMatrix33Type mn2;
  VnlMatrix32Type Qn;
  VnlMatrix23Type QnT;
  VnlMatrix22Type GammaI22;
  VnlMatrix33Type Gn2Bn2;
  VnlMatrix33Type Gn2Bn2m2;
  VnlMatrix22Type QnTGn2Bn2m2Qn;
  VnlMatrix22Type QnTGn2Bn2m2QnGI22;
  VnlMatrix22Type QnTGn2Bn2m2QnGI22I;
  VnlMatrix33Type BnT;
  VnlMatrix33Type Gn2Bn;

  VnlVector3Type mn;
  VnlVector2Type QnTmn;
  VnlVector3Type IntensitySlope;
  VelocityVectorType Vn;

  VectorType vectorToCenter;

  typedef typename NodeVectorJacobianCalculatorType::OutputType   JacobianType;

  JacobianType destinationJacobian;

  GammaI22.set_identity();
  GammaI22 *= this->m_Gamma;

  const double sigmaX2 = this->m_SigmaX * this->m_SigmaX;

  for( PointIdentifier pointId = 0; pointId < numberOfNodes; pointId++ )
    {
    vectorToCenter = pointItr.Value() - this->m_SphereCenter;

    vectorToCenter.Normalize();

    Gn(0,0) = 0.0;
    Gn(1,1) = 0.0;
    Gn(2,2) = 0.0;

    Gn(0,1) = -vectorToCenter[2];
    Gn(0,2) =  vectorToCenter[1];
    Gn(1,2) = -vectorToCenter[0];

    Gn(1,0) =  vectorToCenter[2];
    Gn(2,0) = -vectorToCenter[1];
    Gn(2,1) =  vectorToCenter[0];

    Gn2 = Gn * Gn;

    typedef typename NodeScalarGradientCalculatorType::DerivativeType  DerivativeType; 
    DerivativeType derivative = this->m_NodeScalarGradientCalculator->Evaluate( pointId );

    destinationJacobian = this->m_NodeVectorJacobianCalculator->Evaluate( pointId );

    const BasisSystemType & basis = basisItr.Value();
    const VectorType & v0 = basis.GetVector(0);
    const VectorType & v1 = basis.GetVector(1);
    const MovingPixelRealType Mv = resampledArrayItr.Value();
    const FixedPixelRealType Fv = fixedPointDataItr.Value();
    
    for( unsigned int i = 0; i < 3; i++ )
      {
      Qn(i,0) = v0[i];
      Qn(i,1) = v1[i];
      QnT(0,i) = v0[i];
      QnT(1,i) = v1[i];
      mn[i] = derivative[i];
      }

    for( unsigned int r = 0; r < 3; r++ )
      {
      for( unsigned int c = 0; c < 3; c++ )
        {
        mn2(r,c) = mn[r] * mn[c];
        BnT(r,c) = destinationJacobian(c,r);  // FIXME : Check for potential transposition here...
        }
      }
    
    Gn2Bn = Gn2 * BnT; 

    Gn2Bn2 = Gn2Bn.transpose() * Gn2Bn;

    //
    // The general form of this addition would involve two weights,
    // representing the variance of each term at this node.
    //
    const double sigmaN2 = sigmaItr.Value() * sigmaItr.Value();

    Gn2Bn2m2 = mn2 / sigmaN2 + Gn2Bn2 / sigmaX2;

    QnTGn2Bn2m2Qn = QnT * Gn2Bn2m2 * Qn;
    
    QnTGn2Bn2m2QnGI22 = QnTGn2Bn2m2Qn + GammaI22;

    QnTGn2Bn2m2QnGI22I = vnl_matrix_inverse< double >( QnTGn2Bn2m2QnGI22 );

    QnTmn = QnT * mn;

    IntensitySlope = Qn * QnTGn2Bn2m2QnGI22I * QnTmn;

    Vn.SetVnlVector( IntensitySlope * ( Fv - Mv ) );

    velocityItr.Value() = Vn;

    ++velocityItr;
    ++sigmaItr;
    ++basisItr;
    ++resampledArrayItr;
    ++fixedPointDataItr;
    ++pointItr;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeScalingAndSquaringNumberOfIterations()
{
  //
  // Largest velocity vector Vn  magnitude  / 2^(N-2) < 1/2 Vertex distance
  //
  const double largestVelocityMagnitude = this->ComputeLargestVelocityMagnitude();

  const double ratio = largestVelocityMagnitude / ( this->m_ShortestEdgeLength / 2.0 );

  if( ratio < 1.0 )
    {
    this->m_ScalingAndSquaringNumberOfIterations = 10;
    }
  else
    {
    int iterations = static_cast< int >( vcl_log( ratio ) / vcl_log( 2.0 ) ) + 2;
    
    if( iterations < 10 )
      {
      iterations = 10;
      }

    this->m_ScalingAndSquaringNumberOfIterations = iterations;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeShortestEdgeLength()
{
  double shortestLength = NumericTraits< double >::max();

  typedef typename FixedMeshType::QEPrimal    EdgeType;

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator pointItr = points->Begin();
  FixedPointsConstIterator pointEnd = points->End();

  while( pointItr != pointEnd )
    {
    EdgeType * edge1 = this->m_FixedMesh->FindEdge( pointItr.Index() );

    EdgeType * temp1 = NULL;
    EdgeType * temp2 = edge1;

    const PointType & point = pointItr.Value();

    do
      {
      temp1 = temp2;
      temp2 = temp1->GetOnext();

      const PointIdentifier neighborPointId = temp1->GetDestination();

      const PointType & neighborPoint = points->GetElement( neighborPointId );

      const double distance = point.EuclideanDistanceTo( neighborPoint );

      if( distance < shortestLength )
        {
        shortestLength = distance;
        }

      }
    while( temp2 != edge1 );

    ++pointItr;
    }

  this->m_ShortestEdgeLength = shortestLength;
}

 
template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
double
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeLargestVelocityMagnitude() const
{
  double largestVelocityMagnitude = NumericTraits< double >::Zero;

  VelocityVectorConstIterator velocityItr = this->m_VelocityField->Begin();
  VelocityVectorConstIterator velocityEnd = this->m_VelocityField->End();

  while( velocityItr != velocityEnd )
    { 
    const double velocityMagnitude = velocityItr.Value().GetNorm();

    if( velocityMagnitude > largestVelocityMagnitude )
      {
      largestVelocityMagnitude = velocityMagnitude;
      }

    ++velocityItr;
    }

  return largestVelocityMagnitude;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeDeformationByScalingAndSquaring()
{
  unsigned long powerOfTwo = 1;
  powerOfTwo <<= this->m_ScalingAndSquaringNumberOfIterations;

  const double scalingFactor = 1.0 / powerOfTwo;

  DestinationPointIterator displacementItr = this->m_DisplacementField->Begin();
  DestinationPointIterator displacementEnd = this->m_DisplacementField->End();

  VelocityVectorConstIterator velocityItr = this->m_VelocityField->Begin();

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator pointItr = points->Begin();

  while( displacementItr != displacementEnd )
    {
    displacementItr.Value() = pointItr.Value() +  velocityItr.Value() * scalingFactor;

    ++displacementItr;
    ++pointItr;
    ++velocityItr;
    }

  for( unsigned int i = 0; i < this->m_ScalingAndSquaringNumberOfIterations; i++ )
    {
    DestinationPointConstIterator oldDisplacementItr = this->m_DisplacementField->Begin();
    DestinationPointConstIterator oldDisplacementEnd = this->m_DisplacementField->End();

    DestinationPointIterator newDisplacementItr = this->m_DisplacementFieldSwap->Begin();

    while( oldDisplacementItr != oldDisplacementEnd )
      { 
      newDisplacementItr.Value() = 
        this->InterpolateDestinationFieldAtPoint( 
          this->m_DisplacementField, oldDisplacementItr.Value() );

      ++newDisplacementItr;
      ++oldDisplacementItr;
      }

    this->SwapOldAndNewDisplacementFieldContainers();
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComposeDeformationUpdateWithPreviousDeformation()
{
  DestinationPointConstIterator displacementItr = this->m_DisplacementField->Begin();
  DestinationPointConstIterator displacementEnd = this->m_DisplacementField->End();

  DestinationPointIterator newDestinationPointItr = this->m_DestinationPointsSwap->Begin();

  while( displacementItr != displacementEnd )
    { 
    newDestinationPointItr.Value() =
      this->InterpolateDestinationFieldAtPoint( 
        this->m_DestinationPoints, displacementItr.Value() );


    ++newDestinationPointItr;
    ++displacementItr;
    }

  this->SwapOldAndNewDestinationPointContainers();
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
typename QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::PointType
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
InterpolateDestinationFieldAtPoint( const DestinationPointContainerType * destinationField, 
  const PointType & point )
{
  PointType interpolatedDestinationPoint;
  this->m_DeformationInterpolator->Evaluate( destinationField, point, interpolatedDestinationPoint );
  this->ProjectPointToSphereSurface( interpolatedDestinationPoint );
  return interpolatedDestinationPoint;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ProjectPointToSphereSurface( PointType & point ) const
{
  VectorType vectorToCenter( point - this->m_SphereCenter );
  vectorToCenter *= this->m_SphereRadius / vectorToCenter.GetNorm();
  point = this->m_SphereCenter + vectorToCenter;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
SwapOldAndNewDisplacementFieldContainers()
{
  DestinationPointContainerPointer temp = this->m_DisplacementField;
  this->m_DisplacementField = this->m_DisplacementFieldSwap;
  this->m_DisplacementFieldSwap = temp;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
SwapOldAndNewDestinationPointContainers()
{
  DestinationPointContainerPointer temp = this->m_DestinationPoints;
  this->m_DestinationPoints = this->m_DestinationPointsSwap;
  this->m_DestinationPointsSwap = temp;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
SwapOldAndNewTangetFieldContainers()
{
  TangentVectorPointer temp = this->m_TangentVectorField;
  this->m_TangentVectorField = this->m_TangentVectorFieldSwap;
  this->m_TangentVectorFieldSwap = temp;
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
SmoothDeformationField()
{
  this->ConvertDeformationFieldToTangentVectorField();
  this->SmoothTangentVectorField();
  this->ConvertTangentVectorFieldToDeformationField();
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ConvertDeformationFieldToTangentVectorField()
{
  DestinationPointIterator dstPointItr = this->m_DestinationPoints->Begin();
  DestinationPointIterator dstPointEnd = this->m_DestinationPoints->End();

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator pointItr = points->Begin();

  TangentVectorIterator tangentItr = this->m_TangentVectorField->Begin();

  const double factor = -1.0 / this->m_SphereRadius;

  while( dstPointItr != dstPointEnd )
    {
    VectorType vectorToCenter( pointItr.Value() - this->m_SphereCenter );

    vectorToCenter.Normalize();

    tangentItr.Value() =
      CrossProduct( vectorToCenter, 
        CrossProduct( vectorToCenter, dstPointItr.Value().GetVectorFromOrigin() ) );

    tangentItr.Value() *= factor;

    ++dstPointItr;
    ++tangentItr;
    ++pointItr;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
SmoothTangentVectorField()
{
  const double weightFactor = vcl_exp( - 1.0 / ( 2.0 * this->m_Lambda ) );

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  TangentVectorType smoothedVector;
  TangentVectorType transportedTangentVector;

  const unsigned int PointDimension = PointType::Dimension;

  for( unsigned int iter = 0; iter < this->m_MaximumNumberOfIterations; ++iter )
    {
    std::cout << " Smoothing Iteration " << iter << std::endl;
    
    typedef typename OutputMeshType::QEPrimal    EdgeType;

    TangentVectorIterator tangentItr = this->m_TangentVectorField->Begin();
    TangentVectorIterator tangentEnd = this->m_TangentVectorField->End();

    TangentVectorIterator smoothedTangentItr = this->m_TangentVectorFieldSwap->Begin();

    FixedPointsConstIterator pointItr = points->Begin();

    typedef typename NumericTraits< TangentVectorType >::AccumulateType AccumulatePixelType;

    while( tangentItr != tangentEnd )
      {
      const TangentVectorType & centralTangentVector = tangentItr.Value();

      const EdgeType * edgeToFirstNeighborPoint = this->m_FixedMesh->FindEdge( tangentItr.Index() );
      const EdgeType * edgeToNeighborPoint = edgeToFirstNeighborPoint;

      AccumulatePixelType tangentVectorSum;
  
      for( unsigned int k = 0; k < PointDimension; k++ )
        {
        tangentVectorSum[k] = centralTangentVector[k];
        }

      unsigned int numberOfNeighbors = 0;

      do
        {
        const PointIdentifier neighborPointId = edgeToNeighborPoint->GetDestination();
        const PointType & neighborPoint = points->GetElement( neighborPointId );
        const TangentVectorType & neighborTangentVector = 
          this->m_TangentVectorField->GetElement( neighborPointId );

        this->ParalelTransport( neighborPoint, pointItr.Value(), 
          neighborTangentVector, transportedTangentVector );

        for( unsigned int k = 0; k < PointDimension; k++ )
          {
          tangentVectorSum[k] += weightFactor * transportedTangentVector[k];
          }

        numberOfNeighbors++;

        edgeToNeighborPoint = edgeToNeighborPoint->GetOnext();
        }
      while( edgeToNeighborPoint != edgeToFirstNeighborPoint );

      const double normalizationFactor = 1.0 / ( 1.0 + numberOfNeighbors * weightFactor );


      for( unsigned int k = 0; k < PointDimension; k++ )
        {
        smoothedVector[k] = tangentVectorSum[k] * normalizationFactor;
        }

      smoothedTangentItr.Value() = smoothedVector;

      ++tangentItr;
      ++smoothedTangentItr;
      ++pointItr;
      }

    this->SwapOldAndNewTangetFieldContainers();
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ParalelTransport( const PointType sourcePoint, const PointType destinationPoint,
  const TangentVectorType & inputVector, TangentVectorType & transportedVector ) const
{
  VectorType vsrc = sourcePoint - this->m_SphereCenter;
  VectorType vdst = destinationPoint - this->m_SphereCenter;

  VectorType axis = CrossProduct( vsrc, vdst );

  const double scaledSinus   = axis.GetNorm();
  const double scaledCosinus = vsrc * vdst;

  double angle = vcl_atan2( scaledSinus, scaledCosinus );
  
  typedef Versor< double > VersorType;

  VersorType versor;
  versor.Set( axis, angle );

  transportedVector = versor.Transform( inputVector );
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ConvertTangentVectorFieldToDeformationField()
{
  TangentVectorIterator tangentItr = this->m_TangentVectorField->Begin();
  TangentVectorIterator tangentEnd = this->m_TangentVectorField->End();

  DestinationPointIterator dstPointItr = this->m_DestinationPoints->Begin();

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();

  FixedPointsConstIterator pointItr = points->Begin();

  typedef Versor< double >   VersorType;
  VersorType versor;

  while( tangentItr != tangentEnd )
    {
    VectorType vectorToCenter( pointItr.Value() - this->m_SphereCenter );

    vectorToCenter.Normalize();

    const VectorType & tangent = tangentItr.Value();

    const double sinTheta = tangent.GetNorm();
    const double theta = vcl_asin( sinTheta );

    const VectorType axis = CrossProduct( vectorToCenter, tangent );

    versor.Set( axis, theta );

    dstPointItr.Value() = versor.Transform( pointItr.Value() );

    ++dstPointItr;
    ++tangentItr;
    ++pointItr;
    }
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
AssignResampledMovingValuesToOutputMesh()
{
  OutputMeshPointer out = this->GetOutput();

  OutputPointDataContainerPointer outputPointData = out->GetPointData();

  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  outputPointData->Reserve( numberOfNodes );

  typedef typename ResampledMovingValuesContainerType::ConstIterator  ResampledMovingValuesContainerIterator;

  ResampledMovingValuesContainerIterator  resampledArrayItr = this->m_ResampledMovingValuesContainer->Begin();
  ResampledMovingValuesContainerIterator  resampledArrayEnd = this->m_ResampledMovingValuesContainer->End();

  while( resampledArrayItr != resampledArrayEnd )
    {
    outputPointData->SetElement( resampledArrayItr.Index(), resampledArrayItr.Value() );
    resampledArrayItr++;
    } 
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
PrintOutDeformationVectors()
{
  std::cout << std::endl;
  std::cout << "Deformation Vectors at every node " <<  std::endl;
  DestinationPointIterator dstPointItr = this->m_DestinationPoints->Begin();
  DestinationPointIterator dstPointEnd = this->m_DestinationPoints->End();

  const FixedPointsContainer * points = this->m_FixedMesh->GetPoints();
  FixedPointsConstIterator srcPointItr = points->Begin();

  while( dstPointItr != dstPointEnd )
    {
    std::cout <<  dstPointItr.Value() - srcPointItr.Value() << std::endl;

    ++dstPointItr;
    ++srcPointItr;
    }
  std::cout << std::endl;
}

}

#endif
