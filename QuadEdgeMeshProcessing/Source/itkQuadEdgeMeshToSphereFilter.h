#ifndef __itkQuadEdgeMeshToSphereFilter_h
#define __itkQuadEdgeMeshToSphereFilter_h


#include <itkQuadEdgeMeshToQuadEdgeMeshFilter.h>
#include <itkQuadEdgeMeshSplitFilter.h>
#include <itkQuadEdgeMeshBorderTransform.h>
#include <itkQuadEdgeMeshParamMatrixCoefficients.h>
#include <itkQuadEdgeMeshParam.h>


namespace itk
{
/**
 * \class QuadEdgeMeshToSphereFilter
 * \brief
*/
template< class TInputMesh, class TOutputMesh, class TSolverTraits >
class QuadEdgeMeshToSphereFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshToSphereFilter Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh > Superclass;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( QuadEdgeMeshToSphereFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

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

  typedef TSolverTraits SolverTraits;

  typedef QuadEdgeMeshSplitFilter< OutputMeshType,
    OutputMeshType >                                SplitFilterType;
  typedef typename SplitFilterType::Pointer         SplitFilterPointer;

  typedef QuadEdgeMeshBorderTransform< OutputMeshType, OutputMeshType >
    BorderTransformType;
  typedef typename BorderTransformType::Pointer
    BorderTransformPointer;

  typedef QuadEdgeMeshParam< OutputMeshType, OutputMeshType, SolverTraits >
    ParametrizationType;
  typedef typename ParametrizationType::Pointer
    ParametrizationPointer;

  typedef MatrixCoefficients< OutputMeshType >       CoefficientsComputation;

  void SetCoefficientsMethod( CoefficientsComputation* iMethod )
    {
    this->m_CoefficientsMethod = iMethod;
    }

protected:
  QuadEdgeMeshToSphereFilter() : Superclass() {}
  ~QuadEdgeMeshToSphereFilter() {}

  CoefficientsComputation * m_CoefficientsMethod;

  void GenerateData( )
  {
    Superclass::GenerateData();
    OutputMeshPointer output = this->GetOutput();

    SplitFilterPointer split_filter = SplitFilterType::New();
    split_filter->SetInput( output );
    split_filter->Update();

    BorderTransformPointer border_transform = BorderTransformType::New();
    border_transform->SetInput( split_filter->GetOutput( 0 ) );
    border_transform->SetTransformType(
      BorderTransformType::DISK_BORDER_TRANSFORM );
    border_transform->SetRadius( 1. );

    ParametrizationPointer param0 = ParametrizationType::New();
    param0->SetInput( split_filter->GetOutput( 0 ) );
    param0->SetBorderTransform( border_transform );
    param0->SetCoefficientsMethod( m_CoefficientsMethod );
    param0->Update();

    ParametrizationPointer param1 = ParametrizationType::New();
    param1->SetInput( split_filter->GetOutput( 1 ) );
    param1->SetBorderTransform( border_transform );
    param1->SetCoefficientsMethod( m_CoefficientsMethod );
    param1->Update();

    OutputPointsContainerPointer points = param0->GetOutput()->GetPoints();
    OutputPointType p, q;
    OutputCoordRepType den, r2;
    OutputPointsContainerIterator p_it;

    for( p_it = points->Begin(); p_it != points->End(); ++p_it )
      {
      p = p_it->Value();
      r2 = p[0] * p[0] + p[1] * p[1];
      den = 1. / ( 1. + r2 );
      p[0] = 2. * p[0] * den;
      p[1] = 2. * p[1] * den;
      p[2] = 2. * r2 * den  - 1.;
      q.CastFrom( p );
      output->SetPoint( p_it->Index(), q );
      }

    points = param1->GetOutput()->GetPoints();

    for( p_it = points->Begin(); p_it != points->End(); ++p_it )
      {
      p = p_it->Value();
      r2 = p[0] * p[0] + p[1] * p[1];
      den = 1. / ( 1. + r2 );
      p[0] = 2. * p[0] * den;
      p[1] = 2. * p[1] * den;
      p[2] = -2. * r2 * den + 1.;
      q = output->GetPoint( p_it->Index() );
      q.CastFrom( p );
      output->SetPoint( p_it->Index(), q );
      }


  }

private:
  QuadEdgeMeshToSphereFilter( const Self& );
  void operator = ( const Self& );
};
}
#endif
