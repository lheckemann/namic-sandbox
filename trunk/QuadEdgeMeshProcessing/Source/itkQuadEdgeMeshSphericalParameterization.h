#ifndef __itkQuadEdgeMeshSphericalParameterization_h
#define __itkQuadEdgeMeshSphericalParameterization_h

#include <itkQuadEdgeMeshToQuadEdgeMeshFilter.h>
#include <itkQuadEdgeMeshParamMatrixCoefficients.h>

namespace itk
{
  /**
    * \class QuadEdgeMeshSphericalParameterization
    * \brief Compute the spherical parameterization of the input mesh. The input
    * mesh must be homeomorph to a sphere.
    * \note No test is perform on the topology of the input mesh.
  */
  template< class TInputMesh,
            class TOutputMesh,
            class TInitializationFilter >
  class QuadEdgeMeshSphericalParameterization :
    public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
  {
  public:
    typedef QuadEdgeMeshSphericalParameterization Self;
    typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh,
        TOutputMesh > Superclass;
    typedef SmartPointer< Self >          Pointer;
    typedef SmartPointer< const Self >    ConstPointer;

    /** Input types. */
    typedef TInputMesh                              InputMeshType;
    typedef typename InputMeshType::Pointer         InputMeshPointer;
    typedef typename InputMeshType::ConstPointer    InputMeshConstPointer;
    typedef typename InputMeshType::CoordRepType    InputCoordRepType;
    typedef typename InputMeshType::PointType       InputPointType;
    typedef typename InputPointType::VectorType     InputPointVectorType;
    typedef typename InputMeshType::PointIdentifier InputPointIdentifier;
    typedef typename InputMeshType::QEType          InputQEType;
    typedef typename InputMeshType::VectorType      InputVectorType;
    typedef typename InputMeshType::EdgeListType    InputEdgeListType;
    typedef typename InputMeshType::PixelType       InputPixelType;
    typedef typename InputMeshType::Traits          InputTraits;

    typedef typename InputMeshType::PointsContainer InputPointsContainer;
    typedef typename InputMeshType::PointsContainerConstIterator
      InputPointsContainerConstIterator;

    typedef typename InputMeshType::CellsContainerConstIterator
      InputCellsContainerConstIterator;
    typedef typename InputMeshType::EdgeCellType    InputEdgeCellType;
    typedef typename InputMeshType::PolygonCellType InputPolygonCellType;
    typedef typename InputMeshType::PointIdList     InputPointIdList;

    /** Output types. */
    typedef TOutputMesh                               OutputMeshType;
    typedef typename OutputMeshType::Pointer          OutputMeshPointer;
    typedef typename OutputMeshType::ConstPointer     OutputMeshConstPointer;
    typedef typename OutputMeshType::CoordRepType     OutputCoordRepType;
    typedef typename OutputMeshType::PointType        OutputPointType;
    typedef typename OutputMeshType::PointIdentifier  OutputPointIdentifier;
    typedef typename OutputMeshType::QEType           OutputQEType;
    typedef typename OutputMeshType::VectorType       OutputVectorType;
    typedef typename OutputQEType::IteratorGeom       OutputQEIterator;
    typedef typename OutputMeshType::PointsContainerPointer
      OutputPointsContainerPointer;
    typedef typename OutputMeshType::PointsContainerIterator
      OutputPointsContainerIterator;

    itkStaticConstMacro( PointDimension, unsigned int,
                         OutputMeshType::PointDimension );

    typedef MatrixCoefficients< OutputMeshType > CoefficientsComputation;

    typedef TInitializationFilter InitializationFilterType;
    typedef typename InitializationFilterType::Pointer
      InitializationFilterPointer;

  public:
    void SetCoefficientsMethod( CoefficientsComputation* iMethod )
    { m_CoefficientsMethod = iMethod; }

    itkNewMacro( Self );
    itkTypeMacro( QuadEdgeMeshSphericalParameterization,
      QuadEdgeMeshToQuadEdgeMeshFilter );

    itkSetMacro( IterationSTOP, unsigned int );
    itkSetMacro( TimeStep, OutputCoordRepType );
    itkSetMacro( Threshold, OutputCoordRepType );

  protected:
    QuadEdgeMeshSphericalParameterization() : Superclass(), m_Iteration( 0 ),
      m_IterationSTOP( 100 ), m_OldEnergy( 1e12 ), m_NewEnergy( 1e12 ),
      m_Threshold( 0.001 ), m_TimeStep( 0.1 )
    {
      m_InitFilter = InitializationFilterType::New();
    }
    ~QuadEdgeMeshSphericalParameterization() {}

    unsigned int m_Iteration;
    unsigned int m_IterationSTOP;
    OutputCoordRepType m_OldEnergy;
    OutputCoordRepType m_NewEnergy;

    std::map< OutputQEType*, OutputCoordRepType > m_CoefficientMap;

    OutputCoordRepType m_Threshold;
    OutputCoordRepType m_TimeStep;

    InitializationFilterPointer m_InitFilter;

    CoefficientsComputation* m_CoefficientsMethod;

    void ComputeCoefficientMap()
    {
      OutputMeshPointer output = this->GetOutput();

      OutputPointsContainerPointer points = output->GetPoints();
      OutputPointIdentifier p_id( 0 );
      OutputQEType* qe( 0 );
      OutputQEType* qe_it( 0 );
      OutputPointType p;
      OutputCoordRepType coeff( 0. );

      for( OutputPointsContainerIterator it = points->Begin();
           it != points->End();
           ++it )
        {
        p_id = it->Index();
        p = it->Value();

        qe = p.GetEdge();
        if( qe != 0 )
          {
          qe_it = qe;
          do
            {
            if( p_id < qe_it->GetDestination() )
              {
              coeff = ( *m_CoefficientsMethod )( output, qe_it );
              m_CoefficientMap[qe_it] = coeff;
              m_CoefficientMap[qe_it->GetSym()] = coeff;
              }
            qe_it = qe_it->GetOnext();
            } while( qe_it != qe );
          }
        }
    }

    void GenerateData()
    {
      assert( m_CoefficientsMethod != 0 );
      Superclass::GenerateData();

      OutputMeshPointer output = this->GetOutput();
      m_Iteration = 0;

      ComputeCoefficientMap();

      m_InitFilter->SetInput( output );
      m_InitFilter->SetCoefficientsMethod( m_CoefficientsMethod );
      m_InitFilter->Update();

      output = m_InitFilter->GetOutput();

      do
      {
        m_OldEnergy = m_NewEnergy;
        ComputeEnergyAndRelaxVertexLocation();
        //if( m_Iteration % 10 == 0 )
        std::cout <<m_Iteration <<" " <<m_NewEnergy <<" "
          <<vnl_math_abs( m_OldEnergy - m_NewEnergy )
          <<std::endl;
        ++m_Iteration;
      } while( vnl_math_abs( m_OldEnergy - m_NewEnergy ) > m_Threshold
        && m_Iteration < m_IterationSTOP );
       std::cout <<m_Iteration <<std::endl;
    }

    inline OutputCoordRepType ComputeQuadEdgeEnergy(
      const OutputCoordRepType& iCoeff,
      const OutputPointType& iOrg,
      const OutputPointType& iDest )
    {
      return iCoeff * iOrg.SquaredEuclideanDistanceTo( iDest );
    }

    inline OutputVectorType ComputeQuadEdgeAbsoluteDerivative(
      const OutputCoordRepType& iCoeff,
      const OutputPointType& iOrg,
      const OutputPointType& iDest )
    {
      return iCoeff * ( iDest - iOrg );
    }

    /** E = \sum_K k_{uv} \| n_u - n_v \|^2 */
    OutputVectorType ComputeEnergyAndAbsoluteDerivative(
      const OutputPointIdentifier& iId )
    {
      OutputMeshPointer output = this->GetOutput();
      OutputQEType* qe = output->FindEdge( iId );
      OutputVectorType oVector;
      oVector.Fill( 0. );

      if( qe != 0 )
      {
        OutputQEType* qe_it = qe;

        OutputPointType p_org = output->GetPoint( iId );
        OutputPointType p_dest;

        OutputVectorType delta;
        delta.Fill( 0. );

        OutputVectorType n = p_org.GetVectorFromOrigin();
        OutputCoordRepType coeff( 0. );

        do
        {
          p_dest = output->GetPoint( qe_it->GetDestination() );
          coeff = m_CoefficientMap[ qe_it ];
          m_NewEnergy += ComputeQuadEdgeEnergy( coeff, p_org, p_dest );
          delta += ComputeQuadEdgeAbsoluteDerivative( coeff, p_org, p_dest );
          qe_it = qe_it->GetOnext();
        } while( qe_it != qe );

        oVector = delta - ( delta * n ) * n;
      }

      return oVector;
    }

    void ComputeEnergyAndRelaxVertexLocation()
    {
      m_NewEnergy = static_cast< OutputCoordRepType >( 0. );
      OutputMeshPointer output = this->GetOutput();

      OutputPointsContainerPointer points = output->GetPoints();
      OutputVectorType v;
      OutputPointType p;
      OutputPointIdentifier id( 0 );
      unsigned int dim( 0 );
      OutputCoordRepType norm( 0. );

      for( OutputPointsContainerIterator it = points->Begin();
           it != points->End();
           ++it )
      {
        id = it->Index();
        p = it->Value();

        for( dim = 0; dim < PointDimension; ++dim )
          p[dim] *= ( 1. - m_TimeStep );

        if( p.GetEdge() != 0 )
        {
          v = ComputeEnergyAndAbsoluteDerivative( id );
          norm = 0.;

          for( dim = 0; dim < PointDimension; ++dim )
          {
            p[dim] += m_TimeStep * v[dim];
            norm += p[dim] * p[dim];
          }

          norm = 1. / vcl_sqrt( norm );
          for( dim = 0; dim < PointDimension; ++dim )
            p[dim] *= norm;

          output->SetPoint( id, p );
        }
      }
    }


  private:
    QuadEdgeMeshSphericalParameterization( const Self& );
    void operator = ( const Self& );
  };
}
#endif
