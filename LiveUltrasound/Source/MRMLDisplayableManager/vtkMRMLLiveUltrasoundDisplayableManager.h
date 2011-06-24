#ifndef __vtkMRMLLiveUltrasoundDisplayableManager_h
#define __vtkMRMLLiveUltrasoundDisplayableManager_h

#include "vtkSlicerModuleLogic.h"
#include "vtkVolumeMapper.h"
#include "vtkVolume.h"

#include <vtkMRMLAbstractThreeDViewDisplayableManager.h>

#include "vtkMRMLLiveUltrasoundNode.h"
#include "vtkSlicerLiveUltrasoundLogic.h"
#include "qSlicerLiveUltrasoundModuleExport.h"

class vtkTimerLog;
class vtkMatrix4x4;
class vtkImageActor; 

/// \ingroup Slicer_QtModules_VolumeRendering
class Q_SLICER_QTMODULES_LIVEULTRASOUND_EXPORT vtkMRMLLiveUltrasoundDisplayableManager  :
  public vtkMRMLAbstractThreeDViewDisplayableManager
{
public:
  static vtkMRMLLiveUltrasoundDisplayableManager *New();
  vtkTypeRevisionMacro(vtkMRMLLiveUltrasoundDisplayableManager, vtkMRMLAbstractThreeDViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetGUICallbackCommand(vtkCommand* callback);

  virtual void Create();
 
  // Description:
  // setup image actor based on current node
  // return values:
  //  0: invalid input parameter
  //  1: success
  int SetupImageActorFromNode(vtkMRMLLiveUltrasoundNode* vspNode);

  // Description:
  // Update MRML events
  virtual void ProcessMRMLEvents(vtkObject * caller,
                                 unsigned long event,
                                 void * callData);
  virtual void ProcessLogicEvents(vtkObject * caller,
                                  unsigned long event,
                                  void * callData);

  virtual void UpdateFromMRML();

  void TransformModified(vtkMRMLLiveUltrasoundNode* vspNode);

  virtual void OnMRMLSceneAboutToBeClosedEvent();
  virtual void OnMRMLSceneClosedEvent();
  virtual void OnMRMLSceneImportedEvent();
  virtual void OnMRMLSceneRestoredEvent();
  virtual void OnMRMLSceneNodeAddedEvent(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemovedEvent(vtkMRMLNode* node);

  void UpdateDisplayNodeList();

  void AddDisplayNode(vtkMRMLLiveUltrasoundNode* dnode);
  void RemoveDisplayNode(vtkMRMLLiveUltrasoundNode* dnode);
  void RemoveDisplayNodes();

protected:
  vtkMRMLLiveUltrasoundDisplayableManager();
  ~vtkMRMLLiveUltrasoundDisplayableManager();
  vtkMRMLLiveUltrasoundDisplayableManager(const vtkMRMLLiveUltrasoundDisplayableManager&);
  void operator=(const vtkMRMLLiveUltrasoundDisplayableManager&);

  void OnCreate();

  static bool First;

  vtkSlicerLiveUltrasoundLogic *LiveUltrasoundLogic;
  vtkMRMLLiveUltrasoundNode*    DisplayedNode;

  int SceneIsLoadingFlag;
  int ProcessingMRMLFlag;
  int UpdatingFromMRML;

  typedef std::map<std::string, vtkMRMLLiveUltrasoundNode *> DisplayNodesType;
  DisplayNodesType      DisplayNodes;


protected:
  void OnLiveUltrasoundNodeModified(vtkMRMLLiveUltrasoundNode* dnode);

  void InitializePipelineFromDisplayNode(vtkMRMLLiveUltrasoundNode* vspNode);

};

#endif
