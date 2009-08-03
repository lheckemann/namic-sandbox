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

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkNodeScalarGradientCalculator.h"
#include "itkNodeVectorJacobianCalculator.h"
#include "itkLinearInterpolateDeformationFieldMeshFunction.h"
#include "itkTriangleListBasisSystemCalculator.h"
#include "itkTriangleBasisSystem.h"
#include "itkVectorContainer.h"
#include "itkVector.h"


namespace itk
{
template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
class QuadEdgeMeshSphericalDiffeomorphicDemonsFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshSphericalDiffeomorphicDemonsFilter                Self;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >   Superclass;
  
  /** Method that instantiates a new object */
  itkNewMacro( Self );

  /** Method that provides the name of the class as a string as well as the
   * name of the parent class. */
  itkTypeMacro( QuadEdgeMeshSphericalDiffeomorphicDemonsFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  
  /** Input types. */
  typedef TFixedMesh                                        FixedMeshType;
  typedef TMovingMesh                                       MovingMeshType;
  typedef typename  FixedMeshType::ConstPointer             FixedMeshConstPointer;
  typedef typename  FixedMeshType::PointType                PointType;
  typedef typename  FixedMeshType::PointsContainer          FixedPointsContainer;
  typedef typename  FixedPointsContainer::ConstIterator     FixedPointsConstIterator;
  typedef typename  FixedMeshType::PointDataContainer       FixedPointDataContainer;
  typedef typename  FixedPointDataContainer::ConstIterator  FixedPointDataConstIterator;
  typedef typename  MovingMeshType::ConstPointer            MovingMeshConstPointer;

  /** Output types. */
  typedef TOutputMesh                                       OutputMeshType;
  typedef typename  OutputMeshType::Pointer                 OutputMeshPointer;
  typedef typename  Superclass::OutputPointDataContainer    OutputPointDataContainer;
  typedef typename OutputPointDataContainer::Pointer        OutputPointDataContainerPointer;
 
  /** Declaration of internal types, some of which are exposed for monitoring purposes */
  typedef typename PointType::VectorType                        VectorType;
  typedef TriangleBasisSystem< VectorType, 2 >                  BasisSystemType;
  typedef typename FixedMeshType::PointIdentifier               PointIdentifier;
  typedef VectorContainer< PointIdentifier, BasisSystemType >   BasisSystemContainerType;
  typedef typename BasisSystemContainerType::Pointer            BasisSystemContainerPointer;
  typedef typename BasisSystemContainerType::Iterator           BasisSystemContainerIterator;
  typedef VectorContainer< PointIdentifier, PointType >         DestinationPointContainerType;
  typedef typename DestinationPointContainerType::Pointer       DestinationPointContainerPointer;
  typedef typename DestinationPointContainerType::Iterator      DestinationPointIterator;
  typedef typename DestinationPointContainerType::ConstIterator DestinationPointConstIterator;
  typedef VectorContainer< PointIdentifier, double >            NodeSigmaContainerType;
  typedef typename NodeSigmaContainerType::Pointer              NodeSigmaContainerPointer;
  typedef typename NodeSigmaContainerType::Iterator             NodeSigmaContainerIterator;
  typedef typename NodeSigmaContainerType::ConstPointer         NodeSigmaContainerConstPointer;
  typedef typename NodeSigmaContainerType::ConstIterator        NodeSigmaContainerConstIterator;

  typedef Vector<double, 3>                                       VelocityVectorType;
  typedef VectorContainer< PointIdentifier, VelocityVectorType >  VelocityVectorContainer;
  typedef typename VelocityVectorContainer::Pointer               VelocityVectorPointer;
  typedef typename VelocityVectorContainer::Iterator              VelocityVectorIterator;
  typedef typename VelocityVectorContainer::ConstPointer          VelocityVectorConstPointer;
  typedef typename VelocityVectorContainer::ConstIterator         VelocityVectorConstIterator;

  typedef Vector<double, 3>                                       TangentVectorType;
  typedef VectorContainer< PointIdentifier, TangentVectorType >   TangentVectorContainer;
  typedef typename TangentVectorContainer::Pointer                TangentVectorPointer;
  typedef typename TangentVectorContainer::Iterator               TangentVectorIterator;
  typedef typename TangentVectorContainer::ConstPointer           TangentVectorConstPointer;
  typedef typename TangentVectorContainer::ConstIterator          TangentVectorConstIterator;

  /** Set/Get the Fixed mesh. */
  void SetFixedMesh( const FixedMeshType * fixedMesh );
  itkGetConstObjectMacro( FixedMesh, FixedMeshType ); 

  /** Set/Get the Moving mesh. */
  void SetMovingMesh( const MovingMeshType * movingMesh );
  itkGetConstObjectMacro( MovingMesh, MovingMeshType );

  /** Returns the array of local coordinates systems at every node of the fixed
   * mesh. This array is only valid after a call to Update() has completed
   * successfully. */
  itkGetConstObjectMacro( BasisSystemAtNode, BasisSystemContainerType );

  /** Returns the array of destination points resulting from applying the
   * deformation field to all nodes of the Fixed Mesh. The content of this array
   * is only valid after the first iteration of the filter execution has been
   * completed. It can be used for tracking the progress of the filter. */
  itkGetConstObjectMacro( DestinationPoints, DestinationPointContainerType );

  /** Set/Get the maximum number of iterations that the filter will be
   * allowed to run.  The default is set to 50. */
  itkSetMacro( MaximumNumberOfIterations, unsigned int );
  itkGetMacro( MaximumNumberOfIterations, unsigned int );

  /** Set Sphere Center.  The implementation of this filter assumes that the
   * Mesh surface has a spherical geometry (not only spherical topology). With
   * this method you can specify the coordinates of the center of the sphere
   * represented by the Mesh. This will be used in the computation of parallel
   * transport for vector values associated with nodes.
   */
  itkSetMacro( SphereCenter, PointType );
  itkGetConstMacro( SphereCenter, PointType );

  /** Set Sphere Radius.  The implementation of this filter assumes that the
   * Mesh surface has a spherical geometry (not only spherical topology). With
   * this method you can specify the radius of the sphere. This will be used in
   * the computation of parallel transport for vector values associated
   * with nodes.
   */
  itkSetMacro( SphereRadius, double );
  itkGetConstMacro( SphereRadius, double );

  /** Set/Get the value of the regularization constant used in the computation
   * of the deformation field update. */
  itkSetMacro( Gamma, double );
  itkGetConstMacro( Gamma, double );

  /** Set/Get the value of the weight used in the contribution of the Jacobian
   * to the Levenberg Marquardt term during the computation of the velocity
   * field. */
  itkSetMacro( SigmaX, double );
  itkGetConstMacro( SigmaX, double );

  /** Set/Get the container of sigma values to be associated with each node of
   * the fixed mesh. This sigma value represents the expected variability of
   * scalar values at this node of the mesh. */
  itkSetConstObjectMacro( FixedNodesSigmas, NodeSigmaContainerType );
  itkGetConstObjectMacro( FixedNodesSigmas, NodeSigmaContainerType );

  /** The smoothing filter will run iteratively until reaching this maximum
   * number of iterations. Emprical observartions indicate that ten iterations
   * are enough for typical deformation fields, but of course this would depend
   * on the process that you used for generating your deformation field. 
   */
  itkSetMacro( MaximumNumberOfSmoothingIterations, unsigned int );
  itkGetMacro( MaximumNumberOfSmoothingIterations, unsigned int );

  /** Factor that controls the degree of Smoothing. Large values of Lambda
   * result is stronger smoothing.  The Lambda factor is used to compute the
   * weights of the smoothing kernel as
   *
   * \f$
   * \frac{ \exp( \frac{-1}{2 \lambda} }{ 1 + \abs{ N_i } \exp( \frac{-1}{2 \lambda} }
   * \f$
   *
   * where \f$ N_i \f$ is the number of neighbor nodes around node \f$ i \f$.
   *
   * The default value of Lambda is 1.0.
   *
   * The filter assumes that the neighbor nodes of any given nodes are located
   * at similar distances, and therefore uses the same weight for each one of
   * the neighbor values when computing their weighted average.
   *
   */
  itkSetMacro( Lambda, double );
  itkGetMacro( Lambda, double );


protected:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  
  virtual void GenerateData( );
  
private:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );

  void AllocateInternalArrays();
  void InitializeFixedNodesSigmas();
  void ComputeBasisSystemAtEveryNode();
  void ComputeInitialArrayOfDestinationPoints();
  void InitializeInterpolators();

  void RunIterations();
  void ComputeMappedMovingValueAtEveryNode();
  void ComputeGradientsOfMappedMovingValueAtEveryNode();
  void ComputeVelocityField();
  void SmoothDeformationField();
  void ConvertDeformationFieldToTangentVectorField();
  void SmoothTangentVectorField();
  void ConvertTangentVectorFieldToDeformationField();
  void AssignResampledMovingValuesToOutputMesh();
  void ComputeScalingAndSquaringNumberOfIterations();
  void ComputeShortestEdgeLength();
  double ComputeLargestVelocityMagnitude() const;
  void ComputeDeformationByScalingAndSquaring();
  void ComposeDeformationUpdateWithPreviousDeformation();
  void SwapOldAndNewDestinationPointContainers();
  void SwapOldAndNewDisplacementFieldContainers();

  void ParalelTransport( 
    const PointType sourcePoint, const PointType destinationPoint,
    const TangentVectorType & inputVector, 
    TangentVectorType & transportedVector ) const;

  void PrintOutDeformationVectors();

  virtual PointType InterpolateDestinationFieldAtPoint( 
    const DestinationPointContainerType * destinationField, const PointType & point );

  virtual void ProjectPointToSphereSurface( PointType & point ) const;

  MovingMeshConstPointer                m_MovingMesh;
  FixedMeshConstPointer                 m_FixedMesh;

  /** This is the Array of "Qn" matrices 
   *  presented in equation 3.14 in the paper. */
  BasisSystemContainerPointer           m_BasisSystemAtNode; 

  /** Array containing the destination coordinates of every node in the Fixed
   * Mesh.  This array represents both the deformation field c(xn) and its
   * smoothed version, the field s(xn) as defined in.  */
  DestinationPointContainerPointer      m_DestinationPoints;
  DestinationPointContainerPointer      m_DestinationPointsSwap;

  /** Auxiliary array for computing the Exponential of the velocity field
   * via the Scaling and Squaring method.  */
  DestinationPointContainerPointer      m_DisplacementField;
  DestinationPointContainerPointer      m_DisplacementFieldSwap;

  /** Maximum number of iterations that the filter will be allowed to run. */
  unsigned int                          m_MaximumNumberOfIterations;

  /** Maximum number of iterations that will be used for smoothing the tangent field. */
  unsigned int                         m_MaximumNumberOfSmoothingIterations;

  /** Coefficient that controls the degree of smoothing applied to the tangent field. */
  double                                m_Lambda;

  typedef TriangleListBasisSystemCalculator< 
    FixedMeshType, BasisSystemType > TriangleListBasisSystemCalculatorType;

  /** Helper class that will compute basis systems at every triangle of the Fixed Mesh. */
  typename TriangleListBasisSystemCalculatorType::Pointer m_TriangleListBasisSystemCalculator;

  /** Types definitions for the container of values resampled from the Moving
   * mesh into the coordinates of the Fixed mesh nodes. */ 
  typedef typename MovingMeshType::PixelType                      MovingPixelType;
  typedef typename NumericTraits< MovingPixelType >::RealType     MovingPixelRealType;
  typedef VectorContainer< PointIdentifier, MovingPixelRealType > ResampledMovingValuesContainerType;
  typedef typename ResampledMovingValuesContainerType::Iterator   ResampledMovingValuesContainerIterator;

  typedef typename FixedMeshType::PixelType                     FixedPixelType;
  typedef typename NumericTraits< FixedPixelType >::RealType    FixedPixelRealType;

  /** Container that stores values resampled from the Moving mesh field at the
   * coordinates resulting from mapping the fixed mesh nodes through the current
   * deformation field. */
  typename ResampledMovingValuesContainerType::Pointer          m_ResampledMovingValuesContainer;


  /** Interpolator type for bringing scalar values from the Moving Mesh into the Fixed Mesh. */
  typedef LinearInterpolateMeshFunction< MovingMeshType >       ScalarInterpolatorType;

  /** Interpolator object that will bring scalar values from the Moving Mesh into the Fixed Mesh. */
  typename ScalarInterpolatorType::Pointer                      m_ScalarInterpolator; 

  /** Interpolator for the deformation field values on the grid of the Fixed mesh. */
  typedef LinearInterpolateDeformationFieldMeshFunction< FixedMeshType > DeformationInterpolatorType;

  /** Interpolator object that will compute deformation destination points on the fixed mesh grid. */
  typename DeformationInterpolatorType::Pointer                 m_DeformationInterpolator; 

  /** Helper class that will compute the gradient of resampled Moving mesh
   * values at every node of the Fixed mesh with respect to the coordinate system
   * of that node in the fixed mesh. */
  typedef NodeScalarGradientCalculator< 
    FixedMeshType, ResampledMovingValuesContainerType >         NodeScalarGradientCalculatorType;
  typename NodeScalarGradientCalculatorType::Pointer            m_NodeScalarGradientCalculator; 

  /** Helper class that will compute the Jacobian of destination points 
   * at every node of the Fixed mesh with respect to the coordinate system
   * of that node in the fixed mesh. */
  typedef NodeVectorJacobianCalculator< 
    FixedMeshType, DestinationPointContainerType >              NodeVectorJacobianCalculatorType;
  typename NodeVectorJacobianCalculatorType::Pointer            m_NodeVectorJacobianCalculator; 


  /** Center of spherical mesh. We assume that both the Fixed and
   * Moving meshes have spherical geometry and that they share the same
   * center and radius. */
  PointType       m_SphereCenter;

  /** Radius of spherical mesh. We assume that both the Fixed and
   * Moving meshes have spherical geometry and that they share the same
   * center and radius. */
  double          m_SphereRadius;

  /** Regularization constant used during the update of the deformation field. */
  double          m_Gamma;

  /** This term controls the contribution of the Jacobian in the Levenberg
   * Marquardt term that computes the velocity field. Large values of SigmaX
   * will result in large deformations. */
  double          m_SigmaX;

  /** Shortest edge length in the input fixed mesh. This is used for computing
   * the number of iterations required in the squaring and scaling method. */
  double          m_ShortestEdgeLength;

  /** Number of iterations to be applied in the Scaling and Squaring method. */
  unsigned int    m_ScalingAndSquaringNumberOfIterations;

  /** Container of Sigma values associated to each node of the Fixed mesh. */
  NodeSigmaContainerConstPointer    m_FixedNodesSigmas;

  /** Container of velocity field vectors. */
  VelocityVectorPointer             m_VelocityField;

  /** Container of tangent vectors used to smooth the deformation field. */
  TangentVectorPointer              m_TangentVectorField;

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif

#endif
