
#include <ctime>
#include <iostream>
#include <utility>
#include <vector>


#include "vtkMRMLNode.h"
// #include "vtkMRMLStorageNode.h"
#include "vtkMRMLTransformNode.h"

#include "vtkTransformRecorderWin32Header.h"

class vtkMRMLIGTLConnectorNode;


//BTX
/**
 * Struct to store a recorded transform.
 */
class TransformRecord
{
public:
  std::string DeviceName;
  std::string Transform;
  long int TimeStampSec; // UNIX time, rounded down. Seconds from 1970 Jan 1 00:00, UTC.
  int TimeStampNSec;     // Nanoseconds from TimeStampSec to the real timestamp.
};



class MessageRecord
{
public:
  std::string Message;
  long int TimeStampSec;
  int TimeStampNSec;
};
//ETX


class
VTK_TransformRecorder_EXPORT
vtkMRMLTransformRecorderNode
: public vtkMRMLNode
{

public:
  
  //BTX
  // Events.
  enum {
    TransformChangedEvent = 201001,
    RecordingStartEvent   = 200901,
    RecordingStopEvent    = 200902
  };
  //ETX
  
  // Standard MRML node methods
  
  static vtkMRMLTransformRecorderNode *New();
  vtkTypeMacro( vtkMRMLTransformRecorderNode, vtkMRMLNode );
  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "vtkMRMLTransformRecorderNode"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );
  virtual void UpdateScene( vtkMRMLScene * );
  virtual void UpdateReferenceID( const char *oldID, const char *newID );
  void UpdateReferences();
  
  
    // Public interface
  
  vtkGetStringMacro( ObservedConnectorNodeID );
  vtkMRMLIGTLConnectorNode* GetObservedConnectorNode();
  void SetAndObserveObservedConnectorNodeID( const char* ConnectorNodeRef );
  
  unsigned int GetTransformsBufferSize();
  unsigned int GetMessagesBufferSize();
  
  vtkGetMacro( Recording, bool );
  void SetRecording( bool newState );
  
  //BTX
  void SetTransformSelections( std::vector< int > selections );
  void SetLogFileName( std::string fileName );
  void SaveIntoFile( std::string fileName );
  std::string GetLogFileName();
  void CustomMessage( std::string message );
  //ETX
  
  void UpdateFileFromBuffer();
  void ClearBuffer();
  
  
protected:

    // Constructor/desctructor

  vtkMRMLTransformRecorderNode();
  virtual ~vtkMRMLTransformRecorderNode();
  vtkMRMLTransformRecorderNode ( const vtkMRMLTransformRecorderNode& );
  void operator=( const vtkMRMLTransformRecorderNode& );

  void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void RemoveMRMLObservers();
  
  
    // Protected member variables
    
  vtkSetReferenceStringMacro( ObservedTransformNodeID );
  char* ObservedTransformNodeID;
  vtkMRMLTransformNode* ObservedTransformNode;
  
  vtkSetReferenceStringMacro( ObservedConnectorNodeID );
  char* ObservedConnectorNodeID;
  vtkMRMLIGTLConnectorNode* ObservedConnectorNode;
  
  //BTX
  std::vector< vtkMRMLTransformNode* > ObservedTransformNodes;
  // std::vector< char* >                 ObservedTransformNodeIDs;
  //ETX
  
  
  //BTX
  std::vector< int > TransformSelections;
  
  std::string LogFileName;
  std::vector< TransformRecord > TransformsBuffer;
  std::vector< MessageRecord > MessagesBuffer;
  //ETX
  
  bool Recording;
  
  double Time0;  // Seconds (UNIX)
  clock_t Clock0;
};
