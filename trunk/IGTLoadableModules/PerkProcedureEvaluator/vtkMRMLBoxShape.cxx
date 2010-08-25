
#include "vtkMRMLBoxShape.h"


#include <string>
#include <sstream>

#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"

#include "vtkMRMLFiducial.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLModelNode.h"



// ===============================================================

void
StringToBool( std::string str, bool& var );
/*
{
  std::stringstream ss( str );
  ss >> var;
}
*/

void
StringToDouble( std::string str, double& var );
/*
{
  std::stringstream ss( str );
  ss >> var;
}
*/

// ===============================================================



vtkMRMLBoxShape*
vtkMRMLBoxShape
::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLBoxShape" );
  if ( ret )
    {
    return ( vtkMRMLBoxShape* )ret;
    }
  return new vtkMRMLBoxShape;
}



void
vtkMRMLBoxShape
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}



vtkMRMLNode*
vtkMRMLBoxShape
::CreateNodeInstance()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLBoxShape" );
  if ( ret )
    {
    return ( vtkMRMLBoxShape* )ret;
    }
  return new vtkMRMLBoxShape;
}


void
vtkMRMLBoxShape
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes( atts );
  
  const char* attName;
  const char* attValue;
  
  while( *atts != NULL )
    {
    attName = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "ObservedTransformNodeID" ) )
      {
      }
    
    else if ( ! strcmp( attName, "MinR" ) ) StringToDouble( std::string( attValue ), this->MinR );
    else if ( ! strcmp( attName, "MaxR" ) ) StringToDouble( std::string( attValue ), this->MaxR );
    else if ( ! strcmp( attName, "MinA" ) ) StringToDouble( std::string( attValue ), this->MinA );
    else if ( ! strcmp( attName, "MaxA" ) ) StringToDouble( std::string( attValue ), this->MaxA );
    else if ( ! strcmp( attName, "MinS" ) ) StringToDouble( std::string( attValue ), this->MinS );
    else if ( ! strcmp( attName, "MaxS" ) ) StringToDouble( std::string( attValue ), this->MaxS );
    else if ( ! strcmp( attName, "Initialized" ) ) StringToBool( std::string( attValue ), this->Initialized );
    }
}



void
vtkMRMLBoxShape
::WriteXML( ostream& of, int indent )
{
  Superclass::WriteXML( of, indent );
  
  vtkIndent vindent( indent );
  
  of << " MinR=\"" << this->MinR << "\"";
  of << " MaxR=\"" << this->MaxR << "\"";
  of << " MinA=\"" << this->MinA << "\"";
  of << " MaxA=\"" << this->MaxA << "\"";
  of << " MinS=\"" << this->MinS << "\"";
  of << " MaxS=\"" << this->MaxS << "\"";
  of << " Initialized=\"" << this->Initialized << "\"" << std::endl;
  
  /*
  if ( this->ObservedTransformNodeID != NULL )
    {
    of << vindent << " ObservedTransformNodeID=\"" << this->ObservedTransformNodeID << "\"";
    }
  */
}



void
vtkMRMLBoxShape
::Copy( vtkMRMLNode* node )
{
  Superclass::Copy( node );
  vtkMRMLBoxShape* boxNode = ( vtkMRMLBoxShape* )node;
}



bool
vtkMRMLBoxShape
::IsInside( double r, double a, double s )
{
  if ( ! this->Initialized ) return false;
  
  if ( r < this->MinR  || r > this->MaxR ) return false;
  if ( a < this->MinA  || a > this->MaxA ) return false;
  if ( s < this->MinS  || s > this->MaxS ) return false;
  
  return true;
}



bool
vtkMRMLBoxShape
::IsInside( const double* point )
{
  return this->IsInside( point[ 0 ], point[ 1 ], point[ 2 ] );
}



void
vtkMRMLBoxShape
::AddModelNode( const char* nodeName, double r, double g, double b )
{
    // Create and set the polydata.
  
  vtkSmartPointer< vtkCubeSource > cubeSource = vtkSmartPointer< vtkCubeSource >::New();
    cubeSource->SetXLength( this->MaxR - this->MinR );
    cubeSource->SetYLength( this->MaxA - this->MinA );
    cubeSource->SetZLength( this->MaxS - this->MinS );
    cubeSource->SetCenter( ( MaxR + MinR ) / 2.0, ( MaxA + MinA ) / 2.0, ( MaxS + MinS ) / 2.0 );
    cubeSource->Update();
  
  this->ModelPolyData->DeepCopy( cubeSource->GetOutput() );
  
  
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
    modelNode->SetAndObserveDisplayNodeID( dispNode->GetID() );
    modelNode->SetAndObservePolyData( this->ModelPolyData );
  this->Scene->AddNode( modelNode ); // _*_ 
  
  this->ModelNodeRef = modelNode->GetID();
}



void
vtkMRMLBoxShape
::Initialize( vtkMRMLFiducialListNode* fiducials )
{
  if ( ! fiducials ) return;
  int num = fiducials->GetNumberOfFiducials();
  if ( num < 2 ) return;
  
  
    // Initial values.
  
  float* p = fiducials->GetNthFiducialXYZ( 0 );
  this->MinR = p[ 0 ]; this->MaxR = p[ 0 ];
  this->MinA = p[ 1 ]; this->MaxA = p[ 1 ];
  this->MinS = p[ 2 ]; this->MaxS = p[ 2 ];
  
  
    // Determine min and max values.
  
  for ( int i = 1; i < num; ++ i )
    {
    p = fiducials->GetNthFiducialXYZ( i );
    if ( p[ 0 ] < MinR ) MinR = p[ 0 ]; else if ( p[ 0 ] > MaxR ) MaxR = p[ 0 ];
    if ( p[ 1 ] < MinA ) MinA = p[ 1 ]; else if ( p[ 1 ] > MaxA ) MaxA = p[ 1 ];
    if ( p[ 2 ] < MinS ) MinS = p[ 2 ]; else if ( p[ 2 ] > MaxS ) MaxS = p[ 2 ];
    }
  
  this->AddModelNode( "BoxShapeModel", 1.0, 0.8, 0.1 );
  
  this->Initialized = true;
}



/**
 * Constructor.
 */
vtkMRMLBoxShape
::vtkMRMLBoxShape()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  this->MinR = 0.0;
  this->MaxR = 0.0;
  this->MinA = 0.0;
  this->MaxA = 0.0;
  this->MinS = 0.0;
  this->MaxS = 0.0;
  
  this->Initialized = false;
}
