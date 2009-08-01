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

  this->m_ResampledMovingValuesContainer = ResampledMovingValuesContainerType::New();

  this->m_ScalarInterpolator = ScalarInterpolatorType::New();

  this->m_DeformationInterpolator = DeformationInterpolatorType::New();

  this->m_MaximumNumberOfIterations = 50;

  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;
  this->m_Gamma = 1.0;
  this->m_ScalingAndSquaringNumberOfIterations = 6;
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
  this->ComputeBasisSystemAtEveryNode();
  this->ComputeInitialArrayOfDestinationPoints();
  this->InitializeInterpolators();
  this->ComputeScalingAndSquaringNumberOfIterations();
  this->RunIterations();
  this->ComputeMappedMovingValueAtEveryNode();
  this->AssignResampledMovingValuesToOutputMesh();
  this->PrintOutDeformationVectors();
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

  DestinationPointConstIterator dstPointItr = this->m_DestinationPoints->Begin();

  DestinationPointIterator displacementItr = this->m_DisplacementField->Begin();

  BasisSystemContainerIterator basisItr = this->m_BasisSystemAtNode->Begin();

  ResampledMovingValuesContainerIterator  resampledArrayItr =
    this->m_ResampledMovingValuesContainer->Begin();

  const double scalingFactor = 1.0 / ( 1 << this->m_ScalingAndSquaringNumberOfIterations );

  typedef vnl_matrix_fixed<double,3,3>  VnlMatrix33Type;
  typedef vnl_vector_fixed<double,2>    VnlVector2Type;
  typedef vnl_vector_fixed<double,3>    VnlVector3Type;
  typedef vnl_matrix_fixed<double,3,2>  VnlMatrix32Type;
  typedef vnl_matrix_fixed<double,2,3>  VnlMatrix23Type;
  typedef vnl_matrix_fixed<double,2,2>  VnlMatrix22Type;
  typedef Vector<double, 3>             DoubleVectorType;

  VnlMatrix33Type Gn;
  VnlMatrix33Type Gn2;
  VnlMatrix33Type m2;
  VnlMatrix32Type Qn;
  VnlMatrix23Type QnT;
  VnlMatrix22Type GI22;
  VnlMatrix33Type Gn2Bn2;
  VnlMatrix33Type Gn2Bn2m2;
  VnlMatrix22Type QnTGn2Bn2m2Qn;
  VnlMatrix22Type QnTGn2Bn2m2QnGI22;
  VnlMatrix22Type QnTGn2Bn2m2QnGI22I;

  VnlVector3Type Bn;
  VnlVector3Type mn;
  VnlVector3Type Gn2Bn;
  VnlVector2Type QnTmn;
  VnlVector3Type IntensitySlope;
  DoubleVectorType Vn;


  GI22.set_identity();
  GI22 *= this->m_Gamma;

  for( PointIdentifier pointId = 0; pointId < numberOfNodes; pointId++ )
    {
    const PointType & point = pointItr.Value();

    Gn(0,0) = 0.0;     // NOTE: FIXME Here we must take the m_SphereRadius into account.
    Gn(1,1) = 0.0;
    Gn(2,2) = 0.0;

    Gn(0,1) = -point[2];
    Gn(0,2) =  point[1];
    Gn(1,2) = -point[0];

    Gn(1,0) =  point[2];
    Gn(2,0) = -point[1];
    Gn(2,1) =  point[0];

    Gn2 = Gn * Gn;

    typedef typename NodeScalarGradientCalculatorType::DerivativeType  DerivativeType; 
    DerivativeType derivative = this->m_NodeScalarGradientCalculator->Evaluate( pointId );

    const PointType & destinationPoint = dstPointItr.Value();

    const BasisSystemType & basis = basisItr.Value();
    const VectorType & v0 = basis.GetVector(0);
    const VectorType & v1 = basis.GetVector(1);
    const MovingPixelRealType Mv = resampledArrayItr.Value();
    const FixedPixelRealType Fv = fixedPointDataItr.Value();
    
    for( unsigned int i = 0; i < 3; i++ )
      {
      Bn[i] = destinationPoint[i]; // FIXME This should be the Jacobian of the Destination points.
      Qn(0,i) = QnT(i,0) = v0[i];
      Qn(1,i) = QnT(i,1) = v1[i];
      mn[i] = derivative[i];
      }

    for( unsigned int r = 0; r < 3; r++ )
      {
      for( unsigned int c = 0; c < 3; c++ )
        {
        m2(r,c) = mn[r] * mn[c];
        }
      }
    
    Gn2Bn = Gn2 * Bn; 

    Gn2Bn2 = outer_product( Gn2Bn, Gn2Bn );

    //
    // The general form of this addition would involve two weights,
    // representing the variance of each term at this node.
    //
    Gn2Bn2m2 = m2 + Gn2Bn2; 

    QnTGn2Bn2m2Qn = QnT * Gn2Bn2m2 * Qn;
    
    QnTGn2Bn2m2QnGI22 = QnTGn2Bn2m2Qn + GI22;

    QnTGn2Bn2m2QnGI22I = vnl_matrix_inverse< double >( QnTGn2Bn2m2QnGI22 );

    QnTmn = QnT * mn;

    IntensitySlope = Qn * QnTGn2Bn2m2QnGI22I * QnTmn;

    Vn.SetVnlVector( IntensitySlope * ( Fv - Mv ) );

    displacementItr.Value() = point +  Vn * scalingFactor;

    ++dstPointItr;
    ++displacementItr;
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
  // How to compute N:  Largest velocity vector Vn  magnitude  / 2^(N-2) < 1/2 vertex distance
  // Take smallest 
  //
  this->m_ScalingAndSquaringNumberOfIterations = 6; // Temporarily...
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeDeformationByScalingAndSquaring()
{
  for( unsigned int i = 0; i < this->m_ScalingAndSquaringNumberOfIterations; i++ )
    {
    DestinationPointConstIterator oldDisplacementItr = this->m_DisplacementField->Begin();
    DestinationPointConstIterator oldDisplacementEnd = this->m_DisplacementField->End();

    DestinationPointIterator newDisplacementItr = this->m_DisplacementFieldSwap->Begin();

    while( oldDisplacementItr != oldDisplacementEnd )
      { 
      this->InterpolateDestinationFieldAtPoint( 
        this->m_DisplacementField, oldDisplacementItr.Value(), newDisplacementItr.Value() );

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
    this->InterpolateDestinationFieldAtPoint( 
      this->m_DestinationPoints, displacementItr.Value(), newDestinationPointItr.Value() ); 

    ++newDestinationPointItr;
    ++displacementItr;
    }

  this->SwapOldAndNewDestinationPointContainers();
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
InterpolateDestinationFieldAtPoint( const DestinationPointContainerType * destinationField, 
  const PointType & point, PointType & interpolatedDestinationPoint )
{
  this->m_DeformationInterpolator->Evaluate( destinationField, point, interpolatedDestinationPoint );
  this->ProjectPointToSphereSurface( interpolatedDestinationPoint );
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
SmoothDeformationField()
{
  //
  // FIXME: Introduce here the code from the Smoothing filter
  //
  //   Convert C field into tangent vectors using Gn2,
  //
  //   Smooth all the vectors....
  //
  //   Convert vectors to destination points. --->  this is S.
  //
  //
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
