
#ifndef __vtkMRMLLiveUltrasoundNode_h
#define __vtkMRMLLiveUltrasoundNode_h 

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"
#include "vtkSlicerLiveUltrasoundModuleMRMLExport.h"
#include "vtkTransform.h"

class vtkActor;
class vtkImageActor;
class vtkMatrix4x4;
class vtkPolyData;
class vtkRenderer;

class vtkSlicerApplication;

class vtkMRMLIGTLConnectorNode;
class vtkImageData;
class vtkMRMLLinearTransformNode;
class vtkMRMLModelNode;
class vtkMRMLVolumeNode;

class VTK_SLICER_LIVEULTRASOUND_MODULE_MRML_EXPORT vtkMRMLLiveUltrasoundNode : public vtkMRMLNode
{
public:

    // Standard MRML node methods

    static vtkMRMLLiveUltrasoundNode *New();
    vtkTypeMacro( vtkMRMLLiveUltrasoundNode, vtkMRMLNode );
    virtual vtkMRMLNode* CreateNodeInstance();

    void Init();
    void StartReceiveServer();
    void StopReceiveServer();

    virtual const char* GetNodeTagName() { return "LiveUltrasound"; };
    void PrintSelf( ostream& os, vtkIndent indent );

    virtual void ReadXMLAttributes( const char** atts );
    virtual void WriteXML( ostream& of, int indent );

    virtual void Copy( vtkMRMLNode *node );
    virtual void UpdateScene( vtkMRMLScene * );
    virtual void UpdateReferenceID( const char *oldID, const char *newID );
    void UpdateReferences();


    vtkSetObjectMacro(ProbeToTrackerTransform, vtkTransform); 
    vtkSetObjectMacro(ModelToProbeTransform, vtkTransform); 
    vtkSetObjectMacro(ImageToProbeTransform, vtkTransform); 



protected:


    // Constructor/desctructor
    vtkMRMLLiveUltrasoundNode();
    virtual ~vtkMRMLLiveUltrasoundNode();
    vtkMRMLLiveUltrasoundNode ( const vtkMRMLLiveUltrasoundNode& );
    void operator=( const vtkMRMLLiveUltrasoundNode& );

    void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
    void RemoveMRMLObservers();


    // Reference to the OpenIGTLink connection node.
public:
    vtkGetStringMacro( ConnectorNodeID );
    vtkMRMLIGTLConnectorNode* GetConnectorNode();
    void SetAndObserveConnectorNodeID( const char* ConnectorNodeRef );
protected:
    vtkSetReferenceStringMacro( ConnectorNodeID );
    char* ConnectorNodeID;
    vtkMRMLIGTLConnectorNode* ConnectorNode;


    // Reference to the probe model node.

public:
    vtkGetStringMacro( ProbeModelNodeID );
    vtkSetReferenceStringMacro( ProbeModelNodeID );
    vtkMRMLModelNode* GetProbeModelNode();  
protected:
    char* ProbeModelNodeID;


    // Reference to probe-to-tracker transform that needs to be updated while tracking.

public:
    vtkGetStringMacro( ProbeToTrackerNodeID );
    vtkMRMLLinearTransformNode* GetProbeToTrackerTransformNode();
    vtkSetReferenceStringMacro( ProbeToTrackerNodeID );
    void SetAndObserveProbeToTrackerTransformNodeID( const char* transformNodeRef );
protected:
    char* ProbeToTrackerNodeID;
    vtkMRMLLinearTransformNode* ProbeToTrackerTransformNode;


    // References to calibration transform nodes.

public:
    vtkGetStringMacro( ImageToProbeTransformNodeID );
    vtkMRMLLinearTransformNode* GetImageToProbeTransformNode();
    void SetAndObserveImageToProbeTransformNodeID( const char* transformNodeRef );
protected:
    vtkSetReferenceStringMacro( ImageToProbeTransformNodeID );
    char* ImageToProbeTransformNodeID;
    vtkMRMLLinearTransformNode* ImageToProbeTransformNode;

public:
    vtkGetStringMacro( ModelToProbeTransformNodeID );
    vtkMRMLLinearTransformNode* GetModelToProbeTransformNode();
    void SetAndObserveModelToProbeTransformNodeID( const char* transformNodeRef );
protected:
    vtkSetReferenceStringMacro( ModelToProbeTransformNodeID );
    char* ModelToProbeTransformNodeID;
    vtkMRMLLinearTransformNode* ModelToProbeTransformNode;


private:

    void UpdateLiveModels( vtkMRMLVolumeNode* volumeNode, vtkMatrix4x4* mat );


    vtkSlicerApplication* SlicerApplication;

    int LastTimeSec;
    int LastTimeNSec;


    // Objects for the 3D renderer.
    // Tracker gives: TrackerToProbeTransform.

    vtkRenderer*   ViewerRenderer;

    vtkTransform*  LastTrackerToProbeTransform;
    vtkImageData*  LastImageData;

    vtkTransform*  ProbeToTrackerTransform;

    vtkTransform*  ModelToProbeTransform;
    vtkPolyData*   ProbeModel;
    vtkActor*      ProbeActor;

    vtkTransform*  ImageToProbeTransform;
    vtkImageActor* LiveImageActor;
};

#endif 
