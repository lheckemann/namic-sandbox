
#ifndef __vtkMRMLLiveUltrasoundNode_h
#define __vtkMRMLLiveUltrasoundNode_h 

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"
#include "vtkTransform.h"

#include "vtkSlicerLiveUltrasoundModuleMRMLExport.h"
#include "vtkImageActor.h"

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

    virtual const char* GetNodeTagName() { return "LiveUltrasound"; };
    void PrintSelf( ostream& os, vtkIndent indent );

    virtual void ReadXMLAttributes( const char** atts );
    virtual void WriteXML( ostream& of, int indent );

    virtual void Copy( vtkMRMLNode *node );
    virtual void UpdateScene( vtkMRMLScene * );
    virtual void UpdateReferenceID( const char *oldID, const char *newID );
    void UpdateReferences();

    virtual void CheckIncomingOpenIGTLinkData(); 

    virtual void StartOpenIGTLinkIFServer(); 
    virtual void StopOpenIGTLinkIFServer(); 

    vtkSetObjectMacro(ProbeToTrackerTransform, vtkTransform); 
    vtkSetObjectMacro(ProbeModelToTrackerTransform, vtkTransform); 
    vtkSetObjectMacro(ModelToProbeTransform, vtkTransform); 
    vtkSetObjectMacro(ImageToProbeTransform, vtkTransform); 
    vtkSetObjectMacro(ImageToTrackerTransform, vtkTransform); 
    
    vtkGetObjectMacro(LiveImageActor, vtkImageActor); 

    enum {
        NewFrameArrivedEvent  = 228944
    };


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



    // Reference to probe model to tracker transform that needs to be updated while tracking.
public:
    vtkGetStringMacro( ProbeModelToTrackerNodeID );
    vtkMRMLLinearTransformNode* GetProbeModelToTrackerTransformNode();
    vtkSetReferenceStringMacro( ProbeModelToTrackerNodeID );
    void SetAndObserveProbeModelToTrackerTransformNodeID( const char* transformNodeRef );
protected:
    char* ProbeModelToTrackerNodeID;
    vtkMRMLLinearTransformNode* ProbeModelToTrackerTransformNode;


    // Reference to image to probe transform node.

public:
    vtkGetStringMacro( ImageToProbeTransformNodeID );
    vtkMRMLLinearTransformNode* GetImageToProbeTransformNode();
    void SetAndObserveImageToProbeTransformNodeID( const char* transformNodeRef );
protected:
    vtkSetReferenceStringMacro( ImageToProbeTransformNodeID );
    char* ImageToProbeTransformNodeID;
    vtkMRMLLinearTransformNode* ImageToProbeTransformNode;

    // Reference to model to probe transform node.
public:
    vtkGetStringMacro( ModelToProbeTransformNodeID );
    vtkMRMLLinearTransformNode* GetModelToProbeTransformNode();
    void SetAndObserveModelToProbeTransformNodeID( const char* transformNodeRef );
protected:
    vtkSetReferenceStringMacro( ModelToProbeTransformNodeID );
    char* ModelToProbeTransformNodeID;
    vtkMRMLLinearTransformNode* ModelToProbeTransformNode;


protected:
    int LastTimeSec;
    int LastTimeNSec;

    vtkImageData*  LastImageData;
    vtkImageActor* LiveImageActor;
    vtkTransform*  ProbeToTrackerTransform;
    vtkTransform*  ModelToProbeTransform;
    vtkTransform*  ImageToProbeTransform;
    vtkTransform*  ImageToTrackerTransform;
    vtkTransform*  ProbeModelToTrackerTransform; 
};

#endif 
