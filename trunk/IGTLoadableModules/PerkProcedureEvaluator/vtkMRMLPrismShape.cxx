
#include "vtkMRMLPrismShape.h"


#include <fstream>
#include <string>
#include <sstream>

#include "vtkCell.h"
#include "vtkCubeSource.h"
#include "vtkDataSetMapper.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkQuad.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkVertex.h"

#include "vtkMRMLFiducial.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLModelNode.h"


// #define DEBUG_PRISMSHAPE
int DebugPrismIndex = 0;


vtkMRMLPrismShape*
vtkMRMLPrismShape
::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPrismShape" );
  if ( ret )
    {
    return ( vtkMRMLPrismShape* )ret;
    }
  return new vtkMRMLPrismShape;
}



void
vtkMRMLPrismShape
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
  
  os << indent << "Shape: Prism" << std::endl;
}



vtkMRMLNode*
vtkMRMLPrismShape
::CreateNodeInstance()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPrismShape" );
  if ( ret )
    {
    return ( vtkMRMLPrismShape* )ret;
    }
  return new vtkMRMLPrismShape;
}


void
vtkMRMLPrismShape
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes( atts );
  
  const char* attName;
  const char* attValue;
  
  int numPointsFound = 0;
  
  while( *atts != NULL )
    {
    attName = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "P0" ) )
      {
      StringToDoubleVector( std::string( attValue ), this->P0, 3 );
      ++ numPointsFound;
      }
    else if ( ! strcmp( attName, "P1" ) )
      {
      StringToDoubleVector( std::string( attValue ), this->P1, 3 );
      ++ numPointsFound;
      }
    else if ( ! strcmp( attName, "P2" ) )
      {
      StringToDoubleVector( std::string( attValue ), this->P2, 3 );
      ++ numPointsFound;
      }
    else if ( ! strcmp( attName, "P3" ) )
      {
      StringToDoubleVector( std::string( attValue ), this->P3, 3 );
      ++ numPointsFound;
      }
    }
  
  if ( numPointsFound == 4 )
    {
    this->ComputePlanes();
    }
}



void
vtkMRMLPrismShape
::WriteXML( ostream& of, int indent )
{
  Superclass::WriteXML( of, indent );
  
  vtkIndent vindent( indent );
  
  of << indent << " P0=\"" << P0[ 0 ] << " " << P0[ 1 ] << " " << P0[ 2 ] << "\"" << std::endl;
  of << indent << " P1=\"" << P1[ 0 ] << " " << P1[ 1 ] << " " << P1[ 2 ] << "\"" << std::endl;
  of << indent << " P2=\"" << P2[ 0 ] << " " << P2[ 1 ] << " " << P2[ 2 ] << "\"" << std::endl;
  of << indent << " P3=\"" << P3[ 0 ] << " " << P3[ 1 ] << " " << P3[ 2 ] << "\"" << std::endl;
  
  /*
  if ( this->ObservedTransformNodeID != NULL )
    {
    of << vindent << " ObservedTransformNodeID=\"" << this->ObservedTransformNodeID << "\"";
    }
  */
}



void
vtkMRMLPrismShape
::Copy( vtkMRMLNode* node )
{
  Superclass::Copy( node );
  vtkMRMLPrismShape* prismNode = ( vtkMRMLPrismShape* )node;
  
  for ( int i = 0; i < 3; ++ i )
    {
    this->P0[ i ] = prismNode->P0[ i ];
    this->P1[ i ] = prismNode->P1[ i ];
    this->P2[ i ] = prismNode->P2[ i ];
    this->P3[ i ] = prismNode->P3[ i ];
    }
}



bool
vtkMRMLPrismShape
::IsInside( double r, double a, double s )
{
  if ( ! this->Initialized ) return false;
  
  double S[ 6 ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  
  for ( int i = 0; i < 6; ++ i )
    {
    S[ i ] = A[ i % 3 ] * r + B[ i % 3 ] * a + C[ i % 3 ] * s + D[ i ];
    }
  
  
#ifdef DEBUG_PRISMSHAPE
  std::ofstream out( "_PrismShape.txt", std::ios_base::app );
  out << "Is inside? : " << r << "  " << a << "  " << s << std::endl;
  out << "S: ";
  for ( int i = 0; i < 6; ++ i ) out << S[ i ] << "  ";
  out << endl;
  out.close();
#endif
  
  
    // Check if the given Point(r,a,s) is between the primary and secondary planes.
    // Return false if not.
  
  if ( S[ 0 ] < 0.0 ) return false;
  if ( S[ 3 ] > 0.0 ) return false;
  if ( S[ 1 ] < 0.0 ) return false;
  if ( S[ 4 ] > 0.0 ) return false;
  if ( S[ 2 ] < 0.0 ) return false;
  if ( S[ 5 ] > 0.0 ) return false;

  return true;
}



/**
 * @returns true if 'point' is inside this box.
 */
bool
vtkMRMLPrismShape
::IsInside( const double* point )
{
  return this->IsInside( point[ 0 ], point[ 1 ], point[ 2 ] );
}



/**
 * Computes the intersection of the needle line, described by transform 'tr'
 * with the wall of this box. Returns result in 'entry', 3 doubles (R,A,S).
 * @returns true if entry point exists, false otherwise.
 */
bool
vtkMRMLPrismShape
::GetEntryPoint( vtkTransform* tNeedleToWorld, double* entry )
{
  if ( tNeedleToWorld == NULL )
    {
    vtkErrorMacro( "Calculation with NULL transform." );
    return false;
    }
  
  double pOrigin[ 4 ] = { 0, 0, 0, 1 };
  double P[ 4 ] = { 0, 0, 0, 1 };  // Position of needle tip.
  
  double vInit[ 4 ] = { 0, 0, -1, 0 };
  double V[ 4 ] = { 0, 0, 0, 0 };  // Direction of needle.
  
  tNeedleToWorld->MultiplyPoint( pOrigin, P );
  tNeedleToWorld->MultiplyPoint( vInit, V );
  
  
  double t[ 6 ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  
  for ( int i = 0; i < 6; ++ i )
    {
    t[ i ] = - ( D[ i ] + A[ i ] * P[ 0 ] + B[ i ] * P[ 1 ] + C[ i ] * P[ 2 ] )
             / (          A[ i ] * V[ 0 ] + B[ i ] * V[ 1 ] + C[ i ] * V[ 2 ] );
    }
  
  int minIndex = -1;
  int minT = 10000;
  
  for ( int i = 0; i < 6; ++ i )
    {
    if ( ( minT > t[ i ] ) && ( t[ i ] >= 0 ) )
      {
      minT = t[ i ];
      minIndex = i;
      }
    }
  
  
    // Compute the entry point position.
  
  for ( int i = 0; i < 3; ++ i )
    {
    entry[ i ] = P[ i ] + t[ minIndex ] * V[ i ]; 
    }
  
  
#ifdef DEBUG_PRISMSHAPE
  std::ofstream out( "_PrismShape.txt", std::ios_base::app );
  out << DebugPrismIndex << std::endl;
  ++ DebugPrismIndex;
  out << "tNeedleToWorld: " << std::endl;
  for ( int row = 0; row < 4; ++ row )
    {
    for ( int col = 0; col < 4; ++ col )
      {
      out << tNeedleToWorld->GetMatrix()->GetElement( row, col ) << "   ";
      }
    out << std::endl;
    }
  out << std::endl;
  out << "V: "; for ( int i = 0; i < 3; ++ i ) out << V[ i ] << "  "; out << std::endl;
  out << "t: "; for ( int i = 0; i < 6; ++ i ) out << t[ i ] << "  "; out << std::endl;
  out << "Entry: " << entry[ 0 ] << "  " << entry[ 1 ] << "  " << entry[ 2 ] << std::endl;
  out.close();
#endif
  
  
  return true;
}



void
vtkMRMLPrismShape
::AddModelNode( const char* nodeName, double r, double g, double b )
{
  if ( ! this->Initialized ) return;
  
  
    // Compute all 8 corners of the prism. First 4 is already given.
  
  double P1mP0[ 3 ] = { 0.0, 0.0, 0.0 };
  double P3mP0[ 3 ] = { 0.0, 0.0, 0.0 };
  
  vtkMath::Subtract( P1, P0, P1mP0 );
  vtkMath::Subtract( P3, P0, P3mP0 );
  
  double P4[ 3 ] = { 0, 0, 0 };
  double P5[ 3 ] = { 0, 0, 0 };
  double P6[ 3 ] = { 0, 0, 0 };
  double P7[ 3 ] = { 0, 0, 0 };
  
  vtkMath::Add( P2, P1mP0, P4 );
  vtkMath::Add( P3, P1mP0, P5 );
  vtkMath::Add( P2, P3mP0, P6 );
  vtkMath::Add( P4, P3mP0, P7 );
  
  
    // Create a polydata as the shape of the prism.
  
  vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
  points->InsertNextPoint( P0 );
  points->InsertNextPoint( P1 );
  points->InsertNextPoint( P2 );
  points->InsertNextPoint( P3 );
  points->InsertNextPoint( P4 );
  points->InsertNextPoint( P5 );
  points->InsertNextPoint( P6 );
  points->InsertNextPoint( P7 );
  
  vtkSmartPointer< vtkQuad > Q1 = vtkSmartPointer< vtkQuad >::New();
  Q1->GetPointIds()->SetId( 0, 0 );
  Q1->GetPointIds()->SetId( 1, 1 );
  Q1->GetPointIds()->SetId( 2, 4 );
  Q1->GetPointIds()->SetId( 3, 2 );
  
  vtkSmartPointer< vtkQuad > Q2 = vtkSmartPointer< vtkQuad >::New();
  Q2->GetPointIds()->SetId( 0, 0 );
  Q2->GetPointIds()->SetId( 1, 2 );
  Q2->GetPointIds()->SetId( 2, 6 );
  Q2->GetPointIds()->SetId( 3, 3 );
  
  vtkSmartPointer< vtkQuad > Q3 = vtkSmartPointer< vtkQuad >::New();
  Q3->GetPointIds()->SetId( 0, 0 );
  Q3->GetPointIds()->SetId( 1, 3 );
  Q3->GetPointIds()->SetId( 2, 5 );
  Q3->GetPointIds()->SetId( 3, 1 );
  
  vtkSmartPointer< vtkQuad > Q4 = vtkSmartPointer< vtkQuad >::New();
  Q4->GetPointIds()->SetId( 0, 2 );
  Q4->GetPointIds()->SetId( 1, 4 );
  Q4->GetPointIds()->SetId( 2, 7 );
  Q4->GetPointIds()->SetId( 3, 6 );
  
  vtkSmartPointer< vtkQuad > Q5 = vtkSmartPointer< vtkQuad >::New();
  Q5->GetPointIds()->SetId( 0, 3 );
  Q5->GetPointIds()->SetId( 1, 6 );
  Q5->GetPointIds()->SetId( 2, 7 );
  Q5->GetPointIds()->SetId( 3, 5 );
  
  vtkSmartPointer< vtkQuad > Q6 = vtkSmartPointer< vtkQuad >::New();
  Q6->GetPointIds()->SetId( 0, 1 );
  Q6->GetPointIds()->SetId( 1, 5 );
  Q6->GetPointIds()->SetId( 2, 7 );
  Q6->GetPointIds()->SetId( 3, 4 );
  
  vtkSmartPointer< vtkPolyData > uGrid = vtkSmartPointer< vtkPolyData >::New();
  uGrid->Allocate( 6 );
  uGrid->InsertNextCell( Q1->GetCellType(), Q1->GetPointIds() );
  uGrid->InsertNextCell( Q2->GetCellType(), Q2->GetPointIds() );
  uGrid->InsertNextCell( Q3->GetCellType(), Q3->GetPointIds() );
  uGrid->InsertNextCell( Q4->GetCellType(), Q4->GetPointIds() );
  uGrid->InsertNextCell( Q5->GetCellType(), Q5->GetPointIds() );
  uGrid->InsertNextCell( Q6->GetCellType(), Q6->GetPointIds() );
  uGrid->SetPoints( points );
  
  
    // Create the MRML Model Node.
  
  vtkSmartPointer< vtkMRMLModelDisplayNode > dispNode = vtkSmartPointer< vtkMRMLModelDisplayNode >::New();
    dispNode->SetScene( this->Scene );
    dispNode->SetColor( r, g, b );
    dispNode->SetOpacity( 0.5 );
    dispNode->VisibilityOn();
    dispNode->SliceIntersectionVisibilityOn();
  this->Scene->AddNode( dispNode );
  
  vtkSmartPointer< vtkMRMLModelNode > modelNode = vtkSmartPointer< vtkMRMLModelNode >::New();
  modelNode->SetName( nodeName );
  modelNode->SetScene( this->Scene );
  modelNode->SetHideFromEditors( 0 );
  modelNode->SaveWithSceneOff();
  modelNode->SetAndObserveDisplayNodeID( dispNode->GetID() );
  modelNode->SetAndObservePolyData( uGrid );
  
  this->Scene->AddNode( modelNode );
  
  this->ModelNodeRef = modelNode->GetID();
  
}



void
vtkMRMLPrismShape
::Initialize( vtkMRMLFiducialListNode* fiducials )
{
  if ( ! fiducials ) return;
  int num = fiducials->GetNumberOfFiducials();
  if ( num < 4 ) return;
  
  
    // Initial values.
  
  float* p0 = fiducials->GetNthFiducialXYZ( 0 );
  float* p1 = fiducials->GetNthFiducialXYZ( 1 );
  float* p2 = fiducials->GetNthFiducialXYZ( 2 );
  float* p3 = fiducials->GetNthFiducialXYZ( 3 );
  
  for ( int i = 0; i < 3; ++ i )
    {
    this->P0[ i ] = p0[ i ];
    this->P1[ i ] = p1[ i ];
    this->P2[ i ] = p2[ i ];
    this->P3[ i ] = p3[ i ];
    }
  
  this->ComputePlanes();
  
  this->Initialized = true;
  
  this->AddModelNode( "PrismShapeModel", 1.0, 0.8, 0.2 );
  
}



/**
 * Constructor.
 */
vtkMRMLPrismShape
::vtkMRMLPrismShape()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  for ( int i = 0; i < 3; ++ i )
    {
    this->P0[ i ] = 0.0;
    this->P1[ i ] = 0.0;
    this->P2[ i ] = 0.0;
    this->P3[ i ] = 0.0;
    
    this->A[ i ] = 0.0;
    this->B[ i ] = 0.0;
    this->C[ i ] = 0.0;
    }
  
  for ( int i = 0; i < 6; ++ i )
    {
    this->D[ i ] = 0.0;
    }
  
  this->Initialized = false;
}



void
vtkMRMLPrismShape
::ComputePlanes()
{
    // Determine the normals of the three main planes.
  
  double P1mP0[ 3 ] = { 0.0, 0.0, 0.0 };
  double P2mP0[ 3 ] = { 0.0, 0.0, 0.0 };
  double P3mP0[ 3 ] = { 0.0, 0.0, 0.0 };
  
  vtkMath::Subtract( P1, P0, P1mP0 );
  vtkMath::Subtract( P2, P0, P2mP0 );
  vtkMath::Subtract( P3, P0, P3mP0 );
  
  double N1[ 3 ] = { 0.0, 0.0, 0.0 };
  double N2[ 3 ] = { 0.0, 0.0, 0.0 };
  double N3[ 3 ] = { 0.0, 0.0, 0.0 };
  
  vtkMath::Cross( P2mP0, P3mP0, N1 );
  vtkMath::Cross( P3mP0, P1mP0, N2 );
  vtkMath::Cross( P1mP0, P2mP0, N3 );
  
  vtkMath::Normalize( N1 );
  vtkMath::Normalize( N2 );
  vtkMath::Normalize( N3 );
  
  A[ 0 ] = N1[ 0 ];  B[ 0 ] = N1[ 1 ];  C[ 0 ] = N1[ 2 ];
  A[ 1 ] = N2[ 0 ];  B[ 1 ] = N2[ 1 ];  C[ 1 ] = N2[ 2 ];
  A[ 2 ] = N3[ 0 ];  B[ 2 ] = N3[ 1 ];  C[ 2 ] = N3[ 2 ];
  
  
    // Determine the last plane parameters.
    // Plan is defined by the equation: Ax + By + Cz + D = 0
    // Note: The normal to the plane is the vector (A,B,C)
  
    // D = - Ax - By - Cz
    // D[ 0 - 2 ] : For planes that contain P0. Name these primary planes.
    
  D[ 0 ] = - A[ 0 ] * P0[ 0 ] - B[ 0 ] * P0[ 1 ] - C[ 0 ] * P0[ 2 ];
  D[ 1 ] = - A[ 1 ] * P0[ 0 ] - B[ 1 ] * P0[ 1 ] - C[ 1 ] * P0[ 2 ];
  D[ 2 ] = - A[ 2 ] * P0[ 0 ] - B[ 2 ] * P0[ 1 ] - C[ 2 ] * P0[ 2 ];
  
    // D[ 3 - 5 ] : For planes that do NOT contain P0. Name these secondary planes.
  
  D[ 3 ] = - A[ 0 ] * P1[ 0 ] - B[ 0 ] * P1[ 1 ] - C[ 0 ] * P1[ 2 ];
  D[ 4 ] = - A[ 1 ] * P2[ 0 ] - B[ 1 ] * P2[ 1 ] - C[ 1 ] * P2[ 2 ];
  D[ 5 ] = - A[ 2 ] * P3[ 0 ] - B[ 2 ] * P3[ 1 ] - C[ 2 ] * P3[ 2 ];
  
  
    // The normals of primary planes should point in the direction where the prism lies.
    // If the fourth given P point is in the opposite direction, invert the normals.
    
  if ( ( A[ 0 ] * P1[ 0 ] + B[ 0 ] * P1[ 1 ] + C[ 0 ] * P1[ 2 ] + D[ 0 ] ) < 0.0 )
    {
    for ( int i = 0; i < 3; ++ i )
      {
      A[ i ] = - A[ i ];
      B[ i ] = - B[ i ];
      C[ i ] = - C[ i ];
      D[ i ] = - D[ i ];
      D[ i + 3 ] = - D[ i + 3 ];
      }
    }
  
  
#ifdef DEBUG_PRISMSHAPE
  std::ofstream out( "_PrismShape.txt", std::ios_base::app );
  out << "-------------------------------------------------------" << std::endl;
  out << "P0: " << P0[ 0 ] << "  " << P0[ 1 ] << "  " << P0[ 2 ] << std::endl;
  out << "P1: " << P1[ 0 ] << "  " << P1[ 1 ] << "  " << P1[ 2 ] << std::endl;
  out << "P2: " << P2[ 0 ] << "  " << P2[ 1 ] << "  " << P2[ 2 ] << std::endl;
  out << "P3: " << P3[ 0 ] << "  " << P3[ 1 ] << "  " << P3[ 2 ] << std::endl;
  out << "ABCD: " << std::endl;
  for ( int i = 0; i < 6; ++ i )
    {
    out << A[i%3] << "  " << B[i%3] << "  " << C[i%3] << "  " << D[ i ] << std::endl;
    }
  out.close();
#endif

}
