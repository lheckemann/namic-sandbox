
#include "vtkMRMLPerkProcedureNode.h"


#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "vtkMath.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"

#include "vtkMRMLLinearTransformNode.h"


// =============================================================


// #define DEBUG_PERKPROCEDURE


#define DISTANCE( p1, p2 ) \
  std::sqrt( \
      ( p1[ 0 ] - p2[ 0 ] ) * ( p1[ 0 ] - p2[ 0 ] ) \
    + ( p1[ 1 ] - p2[ 1 ] ) * ( p1[ 1 ] - p2[ 1 ] ) \
    + ( p1[ 2 ] - p2[ 2 ] ) * ( p1[ 2 ] - p2[ 2 ] ) \
    )




// ==============================================================



vtkTransform*
StrToTransform( std::string str, vtkTransform* tr )
{
  std::stringstream ss( str );
  
  double e00; ss >> e00; double e01; ss >> e01; double e02; ss >> e02; double e03; ss >> e03;
  double e10; ss >> e10; double e11; ss >> e11; double e12; ss >> e12; double e13; ss >> e13;
  double e20; ss >> e20; double e21; ss >> e21; double e22; ss >> e22; double e23; ss >> e23;
  double e30; ss >> e30; double e31; ss >> e31; double e32; ss >> e32; double e33; ss >> e33;
  
  tr->Identity();
  
  tr->GetMatrix()->SetElement( 0, 0, e00 );
  tr->GetMatrix()->SetElement( 0, 1, e01 );
  tr->GetMatrix()->SetElement( 0, 2, e02 );
  tr->GetMatrix()->SetElement( 0, 3, e03 );
  
  tr->GetMatrix()->SetElement( 1, 0, e10 );
  tr->GetMatrix()->SetElement( 1, 1, e11 );
  tr->GetMatrix()->SetElement( 1, 2, e12 );
  tr->GetMatrix()->SetElement( 1, 3, e13 );
  
  tr->GetMatrix()->SetElement( 2, 0, e20 );
  tr->GetMatrix()->SetElement( 2, 1, e21 );
  tr->GetMatrix()->SetElement( 2, 2, e22 );
  tr->GetMatrix()->SetElement( 2, 3, e23 );
  
  tr->GetMatrix()->SetElement( 3, 0, e30 );
  tr->GetMatrix()->SetElement( 3, 1, e31 );
  tr->GetMatrix()->SetElement( 3, 2, e32 );
  tr->GetMatrix()->SetElement( 3, 3, e33 );
  
  return tr;
}


// =============================================================


vtkMRMLPerkProcedureNode*
vtkMRMLPerkProcedureNode
::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureNode" );
  if ( ret )
    {
    return ( vtkMRMLPerkProcedureNode* )ret;
    }
  return new vtkMRMLPerkProcedureNode;
}



void
vtkMRMLPerkProcedureNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
  
  os << indent << "NeedleCalibrationTransformNodeID: " <<
    ( this->NeedleCalibrationTransformNodeID ? this->NeedleCalibrationTransformNodeID : "(none)" )
    << "\n";
}



vtkMRMLNode*
vtkMRMLPerkProcedureNode
::CreateNodeInstance()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureNode" );
  if ( ret )
    {
    return ( vtkMRMLPerkProcedureNode* )ret;
    }
  return new vtkMRMLPerkProcedureNode;
}



void
vtkMRMLPerkProcedureNode
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes( atts );
  
  const char* attName;
  const char* attValue;
  
  while( *atts != NULL )
    {
    attName = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "FileName" ) )
      {
      this->SetFileName( attValue );
      }
    
    if ( ! strcmp( attName, "NeedleCalibrationTransformNodeID" ) )
      {
      this->SetAndObserveNeedleCalibrationTransformNodeID( NULL );
      this->SetNeedleCalibrationTransformNodeID( attValue );
      }
    
    if ( ! strcmp( attName, "NeedleTransformNodeID" ) )
      {
      this->SetAndObserveNeedleTransformNodeID( NULL );
      this->SetAndObserveNeedleTransformNodeID( attValue );
      }
    /*
    if ( ! strcmp( attName, "PrismShapeID" ) )
      {
      this->SetAndObservePrismShapeID( NULL );
      this->SetAndObservePrismShapeID( attValue );
      }
    */
    if ( ! strcmp( attName, "PrismShapeID" ) )
      {
      this->SetAndObservePrismShapeID( NULL );
      this->SetAndObservePrismShapeID( attValue );
      }
    
    if ( ! strcmp( attName, "IndexBegin" ) )
      {
      this->IndexBegin = StringToInt( std::string( attValue ) );
      }
    
    if ( ! strcmp( attName, "IndexEnd" ) )
      {
      this->IndexEnd = StringToInt( std::string( attValue ) );
      }
    
    if ( ! strcmp( attName, "PlanEntryPoint" ) )
      {
      double point[ 4 ];
      StringToDoubleVector( std::string( attValue ), point, 4 );
      for ( int i = 0; i < 4; ++ i ) this->PlanEntryPoint[ i ] = point[ i ];
      }
    
    if ( ! strcmp( attName, "PlanTargetPoint" ) )
      {
      double point[ 4 ];
      StringToDoubleVector( std::string( attValue ), point, 4 );
      for ( int i = 0; i < 4; ++ i ) this->PlanTargetPoint[ i ] = point[ i ];
      }
    }
}



void
vtkMRMLPerkProcedureNode
::WriteXML( ostream& of, int indent )
{
  Superclass::WriteXML( of, indent );
  
  vtkIndent vindent( indent );
  
  
  if ( this->FileName != NULL )
    {
    of << vindent << " FileName=\"" << this->FileName << "\"" << std::endl;
    }
  
  if ( this->NeedleCalibrationTransformNodeID != NULL )
    {
    of << vindent << " NeedleCalibrationTransformNodeID=\"" << this->NeedleCalibrationTransformNodeID << "\"" << std::endl;
    }
  
  if ( this->NeedleTransformNodeID != NULL )
    {
    of << vindent << " NeedleTransformNodeID=\"" << this->NeedleTransformNodeID << "\"" << std::endl;
    }
  /*
  if ( this->PrismShapeID != NULL )
    {
    of << vindent << " PrismShapeID=\"" << this->PrismShapeID << "\"" << std::endl;
    }
  */
  if ( this->PrismShapeID != NULL )
    {
    of << vindent << " PrismShapeID=\"" << this->PrismShapeID << "\"" << std::endl;
    }
  
  of << vindent << " IndexBegin=\"" << this->IndexBegin << "\"" << std::endl;
  of << vindent << " IndexEnd=\"" << this->IndexEnd << "\"" << std::endl;
  
  
  of << vindent << " PlanEntryPoint=\"";
  for ( int i = 0; i < 4; ++ i )
    {
    of << this->PlanEntryPoint[ i ] << " ";
    }
  of << vindent << "\"" << std::endl;
  
  
  of << vindent << " PlanTargetPoint=\"";
  for ( int i = 0; i < 4; ++ i )
    {
    of << this->PlanTargetPoint[ i ] << " ";
    }
  of << vindent << "\"" << std::endl;
  
}



void
vtkMRMLPerkProcedureNode
::ImportFromFile( const char* fileName )
{
  std::ifstream input( fileName );
  if ( ! input.is_open() ) return;
  input.close();
  
  
  this->SetFileName( fileName );
  
  
  this->ClearData();
  
  
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName );
  parser->Parse();
  
  vtkXMLDataElement* element = parser->GetRootElement();
  if ( ! element ) return;
  
  int num = element->GetNumberOfNestedElements();
  
  for ( int i = 0; i < num; ++ i )
    {
    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    if ( strcmp( noteElement->GetName(), "log" ) != 0 ) continue;
    const char* type = noteElement->GetAttribute( "type" );
    
    
      // Check if time was coded according to the old or new fashion.
    
    int numAttribs = noteElement->GetNumberOfAttributes();
    bool old_time_style = false;
    for ( int j = 0; j < numAttribs; ++ j )
      {
      const char* attName = noteElement->GetAttributeName( j );
      if ( !strcmp( attName, "time" ) )
        {
        old_time_style = true;
        break;
        }
      }
    
    
    double time = 0.0;
    if ( old_time_style )
      {
      double time = atof( noteElement->GetAttribute( "time" ) );
      }
    else
      {
      double time_sec = atof( noteElement->GetAttribute( "TimeStampSec" ) );
      double time_nsec = atof( noteElement->GetAttribute( "TimeStampNSec" ) );
      time = time_sec + time_nsec * 1e-9;
      }
    
    
    if ( strcmp( type, "message" ) == 0 )
      {
      PerkNote* note = new PerkNote; // Deleted in NoteList destructor.
      note->Time = time;
      note->Message = std::string( noteElement->GetAttribute( "message" ) );
      this->NoteList.push_back( note );
      }
    
    else if ( strcmp( type, "transform" ) == 0 )
      {
      const char* traC = noteElement->GetAttribute( "transform" );
      vtkSmartPointer< vtkTransform > tr = vtkSmartPointer< vtkTransform >::New();
      StrToTransform( std::string( traC ), tr );
      this->TransformTimeSeries->AddRecord( time, tr );
      }
    }
}



void
vtkMRMLPerkProcedureNode
::Copy( vtkMRMLNode* node )
{
  Superclass::Copy( node );
  vtkMRMLPerkProcedureNode* perkNode = ( vtkMRMLPerkProcedureNode* )node;
  
  this->IndexBegin = perkNode->GetIndexBegin();
  this->IndexEnd = perkNode->GetIndexEnd();
  
  this->FileName = perkNode->GetFileName();
  
  this->SetAndObserveNeedleCalibrationTransformNodeID( NULL );
  this->SetNeedleCalibrationTransformNodeID( perkNode->NeedleCalibrationTransformNodeID );
  
  this->SetAndObserveNeedleTransformNodeID( NULL );
  this->SetNeedleTransformNodeID( perkNode->NeedleTransformNodeID );
  
  // this->SetAndObservePrismShapeID( NULL );
  this->SetAndObservePrismShapeID( NULL );
  // this->SetPrismShapeID( perkNode->PrismShapeID );
  this->SetPrismShapeID( perkNode->PrismShapeID );
}



void
vtkMRMLPerkProcedureNode
::UpdateScene( vtkMRMLScene* scene )
{
  Superclass::UpdateScene( scene );
  
  this->SetAndObservePrismShapeID( this->PrismShapeID );
  this->SetAndObserveNeedleTransformNodeID( this->NeedleTransformNodeID );
  this->SetAndObserveNeedleCalibrationTransformNodeID( this->NeedleCalibrationTransformNodeID );
}



void
vtkMRMLPerkProcedureNode
::UpdateReferenceID( const char* oldID, const char* newID )
{
  Superclass::UpdateReferenceID( oldID, newID );
  
  if ( this->PrismShapeID && ! strcmp( oldID, this->PrismShapeID ) )
    {
    this->SetAndObservePrismShapeID( newID );
    }
  
  if ( this->NeedleTransformNodeID && ! strcmp( oldID, this->NeedleTransformNodeID ) )
    {
    this->SetAndObserveNeedleTransformNodeID( newID );
    }
  
  if ( this->NeedleCalibrationTransformNodeID && ! strcmp( oldID, this->NeedleCalibrationTransformNodeID ) )
    {
    this->SetAndObserveNeedleCalibrationTransformNodeID( newID );
    }
}



void
vtkMRMLPerkProcedureNode
::UpdateReferences()
{
  Superclass::UpdateReferences();
  
  if ( this->PrismShapeID != NULL && this->Scene->GetNodeByID( this->PrismShapeID ) == NULL )
    {
    this->SetAndObservePrismShapeID( NULL );
    }
  
  if ( this->NeedleTransformNodeID != NULL && this->Scene->GetNodeByID( this->NeedleTransformNodeID ) == NULL )
    {
    this->SetAndObserveNeedleTransformNodeID( NULL );
    }
  
  if ( this->NeedleCalibrationTransformNodeID != NULL && this->Scene->GetNodeByID( this->NeedleCalibrationTransformNodeID ) == NULL )
    {
    this->SetAndObserveNeedleCalibrationTransformNodeID( NULL );
    }
}



/**
 * @returns NULL if index not exists.
 */
PerkNote*
vtkMRMLPerkProcedureNode
::GetNoteAtIndex( int index )
{
  if ( index >= 0  &&  (unsigned int)index < this->NoteList.size() )
    {
    return this->NoteList[ index ];
    }
  else
  
  return NULL;
}



/**
 * @returns -1.0 if index not exists.
 */
double
vtkMRMLPerkProcedureNode
::GetRelativeTimeAtNoteIndex( int index )
{
  if ( index >= 0  &&  (unsigned int)index < this->NoteList.size() )
    {
    return ( this->NoteList[ index ]->Time - this->TransformTimeSeries->GetMinTime() );
    }
  else
    {
    return -1.0;
    }
}



double
vtkMRMLPerkProcedureNode
::GetTimeAtTransformIndex( int index )
{
    // Index validity checked in TransformTimeSeries.
  
  return this->TransformTimeSeries->GetTimeAtIndex( index );
}



double
vtkMRMLPerkProcedureNode
::GetRelativeTimeAtTransformIndex( int index )
{
  return ( this->TransformTimeSeries->GetTimeAtIndex( index ) - this->TransformTimeSeries->GetMinTime() );
}



vtkTransform*
vtkMRMLPerkProcedureNode
::GetTransformAtTransformIndex( int index )
{
  return this->TransformTimeSeries->GetTransformAtIndex( index );
}



vtkMRMLStorageNode*
vtkMRMLPerkProcedureNode
::CreateDefaultStorageNode()
{
  return vtkMRMLPerkProcedureStorageNode::New();
};



vtkMRMLLinearTransformNode*
vtkMRMLPerkProcedureNode
::GetNeedleCalibrationTransformNode()
{
  vtkMRMLLinearTransformNode* node = NULL;
  if ( this->GetScene() && this->NeedleCalibrationTransformNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->NeedleCalibrationTransformNodeID );
    node = vtkMRMLLinearTransformNode::SafeDownCast( snode );
    }
  return node;
}



void
vtkMRMLPerkProcedureNode
::SetAndObserveNeedleCalibrationTransformNodeID( const char *nodeID )
{
  vtkSetAndObserveMRMLObjectMacro( this->NeedleCalibrationTransformNode, NULL );
  this->SetNeedleCalibrationTransformNodeID( nodeID );
  vtkMRMLLinearTransformNode *tnode = this->GetNeedleCalibrationTransformNode();
  
  vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
  events->InsertNextValue( vtkMRMLTransformNode::TransformModifiedEvent );
  vtkSetAndObserveMRMLObjectEventsMacro( this->NeedleCalibrationTransformNode, tnode, events );
}



vtkMRMLLinearTransformNode*
vtkMRMLPerkProcedureNode
::GetNeedleTransformNode()
{
  vtkMRMLLinearTransformNode* node = NULL;
  if ( this->GetScene() && this->NeedleTransformNodeID != NULL )
    {
    vtkMRMLNode* cnode = this->GetScene()->GetNodeByID( this->NeedleTransformNodeID );
    node = vtkMRMLLinearTransformNode::SafeDownCast( cnode );
    }
  return node;
}



void
vtkMRMLPerkProcedureNode
::SetAndObserveNeedleTransformNodeID( const char *TransformNodeRef )
{
  vtkSetAndObserveMRMLObjectMacro( this->NeedleTransformNode, NULL );
  this->SetNeedleTransformNodeID( TransformNodeRef );
  vtkMRMLLinearTransformNode* cnode = this->GetNeedleTransformNode();
  vtkSetAndObserveMRMLObjectMacro( this->NeedleTransformNode, cnode );
  if ( cnode )
    {
    cnode->AddObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent, (vtkCommand*)(this->MRMLCallbackCommand) );
    }
}



vtkMRMLPrismShape*
vtkMRMLPerkProcedureNode
::GetPrismShapeNode()
{
  vtkMRMLPrismShape* node = NULL;
  if ( this->GetScene() && this->NeedleTransformNodeID != NULL )
    {
    vtkMRMLNode* cnode = this->GetScene()->GetNodeByID( this->PrismShapeID );
    node = vtkMRMLPrismShape::SafeDownCast( cnode );
    }
  return node;
}



bool
vtkMRMLPerkProcedureNode
::IsNeedleInsideBody()
{
  vtkSmartPointer< vtkTransform > ctr = vtkSmartPointer< vtkTransform >::New();
  this->NeedleTransformNode->GetMatrixTransformToWorld( ctr->GetMatrix() );
  
  
  double cpos[ 3 ] = { ctr->GetMatrix()->GetElement( 0, 3 ),
                       ctr->GetMatrix()->GetElement( 1, 3 ),
                       ctr->GetMatrix()->GetElement( 2, 3 ) };
  
  if ( this->PrismShape != NULL )
    {
    return this->PrismShape->IsInside( cpos[ 0 ], cpos[ 1 ], cpos[ 2 ] );
    }
  else
    {
    return false;
    }
}



void
vtkMRMLPerkProcedureNode
::SetAndObservePrismShapeID( const char* PrismShapeRef )
{
  vtkSetAndObserveMRMLObjectMacro( this->PrismShape, NULL );
  this->SetPrismShapeID( PrismShapeRef );
  vtkMRMLPrismShape* node = this->GetPrismShapeNode();
  vtkSetAndObserveMRMLObjectMacro( this->PrismShape, node );
}



void
vtkMRMLPerkProcedureNode
::PrismShapeFromFiducials( vtkMRMLFiducialListNode* fiducials )
{
  if ( ! this->PrismShape )
    {
    this->PrismShape = vtkMRMLPrismShape::New();
    this->PrismShape->SetScene( this->GetScene() );
    this->PrismShape->SetHideFromEditors( 0 );
    this->PrismShape->SetSaveWithScene( 1 );
    this->SetPrismShapeID( this->PrismShape->GetID() );
    this->GetScene()->AddNode( this->PrismShape );
    }
  
  this->PrismShape->Initialize( fiducials );
}



void
vtkMRMLPerkProcedureNode
::SetNoteIndex( int ind )
{
  if ( ind < 0 ) return;
  else if ( ind >= (int)( this->NoteList.size() ) ) return;
  
  this->NoteIndex = ind;
  
  this->UpdateTransformIndex();
}



void
vtkMRMLPerkProcedureNode
::SetTransformIndex( int ind )
{
  if ( ind >= 0 && ind < this->TransformTimeSeries->GetNumberOfRecords() )
    {
    this->TransformIndex = ind;
    }
  
  this->UpdateTransform();
}



void
vtkMRMLPerkProcedureNode
::MarkIndexBegin()
{
  this->IndexBegin = this->TransformIndex;
}



void
vtkMRMLPerkProcedureNode
::MarkIndexEnd()
{
  this->IndexEnd = this->TransformIndex;
}
  


void
vtkMRMLPerkProcedureNode
::SetPlan( vtkMRMLFiducialListNode* fiducials )
{
  if (    ! fiducials
       || ( fiducials->GetNumberOfFiducials() < 2 ) )
    {
    this->PlanEntryPoint[ 3 ] = 0;
    this->PlanTargetPoint[ 3 ] = 0;
    return;
    }
  
  float* c1 = fiducials->GetNthFiducialXYZ( 0 );
  float* c2 = fiducials->GetNthFiducialXYZ( 1 );
  
  for ( int i = 0; i < 3; ++ i )
    {
    this->PlanEntryPoint[ i ] = c1[ i ];
    this->PlanTargetPoint[ i ] = c2[ i ];
    }
  this->PlanEntryPoint[ 3 ] = 1;
  this->PlanTargetPoint[ 3 ] = 1;
  
  this->PlanReady = true;
}



void
vtkMRMLPerkProcedureNode
::UpdateTransformIndex()
{
  PerkNote* note = this->GetNoteAtIndex( this->NoteIndex );
  double time = note->Time;
  
  int numRecords = this->TransformTimeSeries->GetNumberOfRecords();
  if ( numRecords < 1 ) return;
  
  int index = 0;
  
  while ( this->TransformTimeSeries->GetTimeAtIndex( index ) < time )
    {
    ++ index;
    if ( index == numRecords ) break;
    }
  
  if ( index < numRecords ) this->TransformIndex = index;
  else this->TransformIndex = numRecords - 1;
  
  this->UpdateTransform();
}



void
vtkMRMLPerkProcedureNode
::UpdateMeasurements()
{
    // Check prerequisites for measurement.
  
  if ( this->IndexBegin < 0  ||  this->IndexEnd < 0 ) return;
  if ( this->NeedleTransformNode == NULL ) return;
  
  
    // Save current needle transform.
  
  vtkSmartPointer< vtkMatrix4x4 > mNeedleOriginal = vtkSmartPointer< vtkMatrix4x4 >::New();
  mNeedleOriginal->DeepCopy( this->NeedleTransformNode->GetMatrixTransformToParent() );
  
  
  this->TotalTime =   this->GetTimeAtTransformIndex( this->IndexEnd )
                    - this->GetTimeAtTransformIndex( this->IndexBegin );
  
  
    // Metrics to be measured from user difined begin to end.
  
  double pathInside = 0.0;
  double timeInside = 0.0;
  double surfInside = 0.0; // Surface touched by needle inside.
  
  
  vtkSmartPointer< vtkTransform > tNeedleToWorld = vtkSmartPointer< vtkTransform >::New();
  GetNeedleToWorldTransformAtIndex( this->IndexBegin, tNeedleToWorld );
  
  
    // Parameters needed to calculate metrics.
  
  double lastTime      = this->TransformTimeSeries->GetTimeAtIndex( this->IndexBegin );
  double lastEpos[ 4 ] = { 0, 0, 0, 1 };  // Last entry point of needle into the phantom.
  double currEpos[ 4 ] = { 0, 0, 0, 1 };  // Current entry point of needle.
  double lastTpos[ 4 ] = { 0, 0, 0, 1 };  // Last position of the needle tip.
  double currTpos[ 4 ] = { 0, 0, 0, 1 };  // Current position of the needle tip.
  
  
    // Make the current Target position the last one.
  
  lastTpos[ 0 ] = tNeedleToWorld->GetMatrix()->GetElement( 0, 3 );
  lastTpos[ 1 ] = tNeedleToWorld->GetMatrix()->GetElement( 1, 3 );
  lastTpos[ 2 ] = tNeedleToWorld->GetMatrix()->GetElement( 2, 3 );
    
  
    // Loop over recorded transforms.
  
  for ( int index = this->IndexBegin; index <= this->IndexEnd; ++ index )
    {
    
      // Update parameters for current state.
    
    double ctime = this->TransformTimeSeries->GetTimeAtIndex( index );
    GetNeedleToWorldTransformAtIndex( index, tNeedleToWorld );
    
    for ( int i = 0; i < 3; ++ i ) currTpos[ i ] = tNeedleToWorld->GetMatrix()->GetElement( i, 3 );
    
    
    bool inside = this->PrismShape->IsInside( currTpos[ 0 ], currTpos[ 1 ], currTpos[ 2 ] );
    double d = DISTANCE( lastTpos, currTpos );
    double dt = ctime - lastTime;
    
    
      // Metrics only computed if needle is inside the target body.
    
    if ( inside )
      {
      pathInside += d;
      timeInside += dt;
      
      bool valid = this->PrismShape->GetEntryPoint( tNeedleToWorld, currEpos );
      
        // If "inside surface covered" can be computed.
        
      if ( valid  &&  index > this->IndexBegin )
        {
        double dSurface = this->ApproximateSurface( lastEpos, currEpos, lastTpos, currTpos );
        surfInside += dSurface;
        for ( int i = 0; i < 4; ++ i ) lastEpos[ i ] = currEpos[ i ];
        }
      }
    
    
      // Current parameters become last parameters for the next iteration.
    
    lastTime = ctime;
    for ( int i = 0; i < 4; ++ i ) lastTpos[ i ] = currTpos[ i ];
    
    
#ifdef DEBUG_PERKPROCEDURE
    std::ofstream dout ( "_DebugPerkProcedureEvaluator.txt", std::ios_base::app );
    dout << index << " " << std::endl;
    dout << "Time: " << ctime << std::endl;
    dout << "NeedleToWorld " << std::endl;
    for ( int row = 0; row < 4; ++ row )
      {
      for ( int col = 0; col < 4; ++ col )
        {
        dout << tNeedleToWorld->GetMatrix()->GetElement( row, col ) << " ";
        }
      dout << std::endl;
      }
    dout << std::endl;
    
    dout.close();
#endif
    }
  
  this->TimeInside = timeInside;
  this->PathInside = pathInside;
  this->SurfaceInside = surfInside;
  
  
    // Create a superior oriented unit vector, transform by tr, compare angle with axial.
  
  double inferiorDirection[ 4 ] = { 0, 0, - 1, 1 };
  double needleDirection[ 4 ] = { 0, 0, 0, 1 };
  tNeedleToWorld->MultiplyPoint( inferiorDirection, needleDirection );
  
  double cosinus = lastTpos[ 1 ] - needleDirection[ 1 ];
  double sinus = lastTpos[ 2 ] - needleDirection[ 2 ];
  this->AngleFromAxial = std::atan( sinus / cosinus ) * 180 / 3.141592;
  
  
    // Compute plan angle in axial plane.
  
  this->AngleInAxial = -1; // In case there is no plan.
  
  if ( this->PlanReady )
    {
    
      // Errors compared to the planned needle position.
    
    this->AngleError = this->ComputeAngleError( PlanEntryPoint, PlanTargetPoint, currEpos, currTpos );
    this->EntryError = DISTANCE( PlanEntryPoint, currEpos );
    this->TargetError = DISTANCE( PlanTargetPoint, currTpos );
    
    
      // Old metrics that shouldn't be used anymore...
    
    sinus = this->PlanTargetPoint[ 0 ] - this->PlanEntryPoint[ 0 ];
    cosinus = this->PlanTargetPoint[ 1 ] - this->PlanEntryPoint[ 1 ];
    double planAngleInAxial = 361.0;
    if ( cosinus != 0.0 && sinus != 0.0 )
      {
      planAngleInAxial = std::atan( sinus / cosinus ) * 180 / 3.141592;
      }
    
      // Compute needle angle in axial plane.
    
    sinus = lastTpos[ 0 ] - needleDirection[ 0 ];
    cosinus = lastTpos[ 1 ] - needleDirection[ 1 ];
    double angleInAxial = 361.0;
    if ( cosinus != 0.0 && sinus != 0.0 )
      {
      angleInAxial = std::atan( sinus / cosinus ) * 180 / 3.141592;
      }
    
      // Angle deviation in the axial plane.
    
    if ( planAngleInAxial < 360 && angleInAxial < 360 )
      {
      this->AngleInAxial = planAngleInAxial - angleInAxial;
      if ( this->AngleInAxial < 0.0 ) this->AngleInAxial *= -1;
      }
    else
      {
      this->AngleInAxial = -1.0;
      }
    }
  
  
    // Restore original needle transform.
  
  this->NeedleTransformNode->GetMatrixTransformToParent()->DeepCopy( mNeedleOriginal );
}



void
vtkMRMLPerkProcedureNode
::UpdateTransform()
{
  if ( ! this->NeedleTransformNode ) return;
  
  vtkTransform* transform = this->TransformTimeSeries->GetTransformAtIndex( this->TransformIndex );
  if ( transform == NULL ) return;
  
  
  //debug
  double t1 = transform->GetMatrix()->GetElement( 0, 3 );
  double t2 = transform->GetMatrix()->GetElement( 1, 3 );
  double t3 = transform->GetMatrix()->GetElement( 2, 3 );
  
  
  vtkSmartPointer< vtkMatrix4x4 > mCalibration = vtkSmartPointer< vtkMatrix4x4 >::New();
  mCalibration->Identity();
  
  if ( this->NeedleCalibrationTransformNode != NULL )
    {
    mCalibration->DeepCopy( this->NeedleCalibrationTransformNode->GetMatrixTransformToParent() );
    }
  
  vtkSmartPointer< vtkMatrix4x4 > mNeedle = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  vtkMatrix4x4::Multiply4x4( transform->GetMatrix(), mCalibration, mNeedle );
  
  
  //debug
  double n1 = mNeedle->GetElement( 0, 3 );
  double n2 = mNeedle->GetElement( 1, 3 );
  double n3 = mNeedle->GetElement( 2, 3 );
  
  
  this->NeedleTransformNode->GetMatrixTransformToParent()->DeepCopy( mNeedle );
  
  
  //debug
  vtkSmartPointer< vtkMatrix4x4 > md = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->NeedleTransformNode->GetMatrixTransformToWorld( md );
  double a1 = md->GetElement( 0, 3 );
  double a2 = md->GetElement( 1, 3 );
  double a3 = md->GetElement( 2, 3 );
}



vtkMRMLPerkProcedureNode
::vtkMRMLPerkProcedureNode()
{
  this->TransformTimeSeries = vtkTransformTimeSeries::New();
  
  this->NeedleCalibrationTransformNode = NULL;
  this->NeedleCalibrationTransformNodeID = NULL;
  
  this->NeedleTransformNode = NULL;
  this->NeedleTransformNodeID = NULL;
  this->PrismShape = NULL;
  this->PrismShapeID = NULL;
  
  
  this->FileName = NULL;
  this->NoteIndex = -1;
  this->TransformIndex = -1;
  
  
    // For measurements.
  
  this->IndexBegin = -1;
  this->IndexEnd = -1;
  
  for ( int i = 0; i < 4; ++ i )
    {
    this->PlanEntryPoint[ i ] = 0;
    this->PlanTargetPoint[ i ] = 0;
    }
  this->PlanReady = false;
  
  
    // Measurements.
  
  this->TotalTime = 0.0;
  this->PathInside = 0.0;
  this->TimeInside = 0.0;
  this->SurfaceInside = 0.0;
  this->AngleFromAxial = 0.0;
  this->AngleInAxial = 0.0;
  this->AngleError = 0.0;
  this->EntryError = 0.0;
  this->TargetError = 0.0;
  
  
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
}



vtkMRMLPerkProcedureNode
::~vtkMRMLPerkProcedureNode()
{
  this->ClearData();
  
  this->TransformTimeSeries->Delete();
  this->TransformTimeSeries = NULL;
  
  this->RemoveMRMLObservers();
  
  this->SetAndObserveNeedleCalibrationTransformNodeID( NULL );
  
  this->SetAndObserveNeedleTransformNodeID( NULL );
  // this->SetAndObservePrismShapeID( NULL );
  this->SetAndObservePrismShapeID( NULL );
}



void
vtkMRMLPerkProcedureNode
::RemoveMRMLObservers()
{
  if (    this->NeedleTransformNode
       && this->NeedleTransformNode->HasObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent ) )
    {
    this->NeedleTransformNode->RemoveObservers( vtkMRMLLinearTransformNode::TransformModifiedEvent );
    }
  
  if (    this->NeedleCalibrationTransformNode
       && this->NeedleCalibrationTransformNode->HasObserver( vtkMRMLTransformNode::TransformModifiedEvent ) )
    {
    this->NeedleCalibrationTransformNode->RemoveObservers( vtkMRMLTransformNode::TransformModifiedEvent );
    }
  
  if (    this->NeedleTransformNode
       && this->NeedleTransformNode->HasObserver( vtkMRMLTransformNode::TransformModifiedEvent ) )
    {
    this->NeedleTransformNode->RemoveObservers( vtkMRMLTransformNode::TransformModifiedEvent );
    }
}



void
vtkMRMLPerkProcedureNode
::ClearData()
{
  for ( unsigned int index = 0; index < this->NoteList.size(); ++ index )
    {
    delete this->NoteList[ index ];
    this->NoteList[ index ] = NULL;
    }
  
  this->NoteList.clear();
  this->NoteIndex = -1;
  
  this->TransformTimeSeries->Clear();
  this->TransformIndex = -1;
  
  // this->IndexBegin = -1;
  // this->IndexEnd = -1;
  
  
    // Measurements.
  
  this->TotalTime = 0.0;
  this->PathInside = 0.0;
  this->TimeInside = 0.0;
  this->SurfaceInside = 0.0;
}



/**
 * Calculate the approximate surface touched by a needle, that moves from
 * tip pont t1 to t2 and entry point e1 to e2.
 */
double
vtkMRMLPerkProcedureNode
::ApproximateSurface( double* e1, double* e2, double* t1, double* t2 )
{
    // Approximation: summed area of two triangles, (e1,t1,e2) and (t1,e2,t2).
    // Area of a triangle: cross-product of two edge vectors.  
  
    // First triangle: v = e2 - e1; w = t1 = e1;
  
  double v[ 3 ] = { 0, 0, 0 };
  double w[ 3 ] = { 0, 0, 0 };
  
  for ( int i = 0; i < 3; ++ i )
    {
    v[ i ] = e2[ i ] - e1[ i ];
    w[ i ] = t1[ i ] - e1[ i ];
    }
  
  double cprod[ 3 ] = { 0, 0, 0 };
  vtkMath::Cross( v, w, cprod );
  double area1 = std::sqrt( cprod[ 0 ] * cprod[ 0 ] + cprod[ 1 ] * cprod[ 1 ]
    + cprod[ 2 ] * cprod[ 2 ] );
  
    // Second triangle: v = e2 - t1; w = t2 - t1;
  
  for ( int i = 0; i < 3; ++ i )
    {
    v[ i ] = e2[ i ] - t1[ i ];
    w[ i ] = t2[ i ] - t1[ i ];
    }
  
  vtkMath::Cross( v, w, cprod );
  double area2 = std::sqrt( cprod[ 0 ] * cprod[ 0 ] + cprod[ 1 ] * cprod[ 1 ]
    + cprod[ 2 ] * cprod[ 2 ] );
  
  return ( area1 + area2 );
}



void
vtkMRMLPerkProcedureNode
::GetNeedleToWorldTransformAtIndex( int index, vtkTransform* tNeedleToWorld )
{
  vtkSmartPointer< vtkTransform > tNeedletipToParent = vtkSmartPointer< vtkTransform >::New();
  tNeedletipToParent->GetMatrix()->DeepCopy( this->TransformTimeSeries->GetTransformAtIndex( index )->GetMatrix() );
  
  
    // Create a needle calibration (rotation) transform.
  
  vtkSmartPointer< vtkTransform > tNeedleToNeedletip = vtkSmartPointer< vtkTransform >::New();
  tNeedleToNeedletip->Identity();
  if ( this->NeedleCalibrationTransformNode != NULL )
    {
    tNeedleToNeedletip->GetMatrix()->DeepCopy( this->NeedleCalibrationTransformNode->GetMatrixTransformToParent() );
    tNeedleToNeedletip->Update();
    }
  
  
    // Apply needle calibration.
  
  vtkSmartPointer< vtkTransform > tNeedleToParent = vtkSmartPointer< vtkTransform >::New();
  tNeedleToParent->Identity();
  tNeedleToParent->Update();
  tNeedleToParent->Concatenate( tNeedletipToParent );
  tNeedleToParent->Concatenate( tNeedleToNeedletip );
  tNeedleToParent->Update();
  
  
    // Apply fiducial registration transform to the NeedleToParent transform.
  
  vtkSmartPointer< vtkMatrix4x4 > mWorld = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->NeedleTransformNode->GetMatrixTransformToParent()->DeepCopy( tNeedleToParent->GetMatrix() );
  this->NeedleTransformNode->GetMatrixTransformToWorld( mWorld );
  tNeedleToWorld->GetMatrix()->DeepCopy( mWorld );
  tNeedleToWorld->Update();
}



/**
 * @param ep Planned Entry point.
 * @param tp Planned Target point.
 * @param ea Actual Entry point.
 * @param ta Actual Target point.
 */
double
vtkMRMLPerkProcedureNode
::ComputeAngleError( double* ep, double* tp, double* ea, double* ta )
{
  double planEtoT[ 3 ] = { 0, 0, 0 };
  double actualEtoT[ 3 ] = { 0, 0, 0 };
  
  vtkMath::Subtract( tp, ep, planEtoT );
  vtkMath::Subtract( ta, ea, actualEtoT );
  
  vtkMath::Normalize( planEtoT );
  vtkMath::Normalize( actualEtoT );
  
  double cosAngle = vtkMath::Dot( planEtoT, actualEtoT );
  double angle = std::acos( cosAngle ) / 3.141592 * 180.0;
  
  return angle;
}
