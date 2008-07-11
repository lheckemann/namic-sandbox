// .NAME vtkNeuroNavGUI 
// .SECTION Description
// Main Volumes GUI and mediator methods for slicer3. 


#ifndef __vtkNeuroNavGUI_h
#define __vtkNeuroNavGUI_h


#ifdef WIN32
#include "vtkNeuroNavWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkNeuroNavLogic.h"

#include "vtkIGTDataManager.h"
#include "vtkIGTPat2ImgRegistration.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"


#include <string>

class vtkKWPushButton;
class vtkKWEntryWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMenuButton;
class vtkKWCheckButton;
class vtkKWScaleWithEntry;
class vtkKWEntry;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;


// Description:    
// This class implements Slicer's Volumes GUI.
//
class VTK_NEURONAV_EXPORT vtkNeuroNavGUI : public vtkSlicerModuleGUI
{
public:

  // Description:    
  // Usual vtk class functions
  static vtkNeuroNavGUI* New (  );
  vtkTypeRevisionMacro ( vtkNeuroNavGUI, vtkSlicerModuleGUI );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Description:    
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro ( Logic, vtkNeuroNavLogic );

  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  void SetModuleLogic ( vtkSlicerLogic *logic )
    { this->SetLogic ( vtkObjectPointer (&this->Logic), logic ); }
  //void SetAndObserveModuleLogic ( vtkNeuroNavLogic *logic )
  //    { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }

  // Description:    
  // This method builds the IGTDemo module GUI
  virtual void BuildGUI ( );

  // Description:
  // Add/Remove observers on widgets in the GUI
  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );


  // Description:
  // Class's mediator methods for processing events invoked by
  // either the Logic, MRML or GUI.    
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();

  void HandleMouseEvent(vtkSlicerInteractorStyle *style);


  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( );
  virtual void Exit ( );

  void Init();

  //BTX
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);

  //ETX

protected:
  vtkNeuroNavGUI ( );
  virtual ~vtkNeuroNavGUI ( );

  vtkKWCheckButton *LocatorCheckButton;
  vtkKWCheckButton *HandleCheckButton;
  vtkKWCheckButton *GuideCheckButton;

  vtkKWCheckButton *LocatorModeCheckButton;
  vtkKWCheckButton *UserModeCheckButton;
  vtkKWCheckButton *FreezeCheckButton;
  vtkKWCheckButton *ObliqueCheckButton;

  vtkKWMenuButton *RedSliceMenu;
  vtkKWMenuButton *YellowSliceMenu;
  vtkKWMenuButton *GreenSliceMenu;

  vtkKWEntryWithLabel *TransformNodeNameEntry;

  vtkKWEntryWithLabel *PatCoordinatesEntry;
  vtkKWEntryWithLabel *SlicerCoordinatesEntry;
  vtkKWPushButton *GetPatCoordinatesPushButton;
  vtkKWPushButton *AddPointPairPushButton;

  vtkKWMultiColumnListWithScrollbars *PointPairMultiColumnList;

  //    vtkKWPushButton *LoadPointPairPushButton;
  //    vtkKWPushButton *SavePointPairPushButton;
  vtkKWPushButton *DeletePointPairPushButton;
  vtkKWPushButton *DeleteAllPointPairPushButton;    
  vtkKWPushButton *RegisterPushButton;
  vtkKWPushButton *ResetPushButton;


  // Module logic and mrml pointers
  vtkNeuroNavLogic *Logic;

  // int StopTimer;
  vtkMatrix4x4 *LocatorMatrix;

  vtkMRMLModelNode *LocatorModelNode;
  vtkMRMLModelDisplayNode *LocatorModelDisplayNode;

  //BTX
  std::string LocatorModelID;
  //ETX
  vtkIGTDataManager *DataManager;
  vtkIGTPat2ImgRegistration *Pat2ImgReg;

  vtkCallbackCommand *DataCallbackCommand;

  // Access the slice windows
  vtkSlicerSliceLogic *Logic0;
  vtkSlicerSliceLogic *Logic1;
  vtkSlicerSliceLogic *Logic2;
  vtkMRMLSliceNode *SliceNode0;
  vtkMRMLSliceNode *SliceNode1;
  vtkMRMLSliceNode *SliceNode2;
  vtkSlicerSliceControllerWidget *Control0;
  vtkSlicerSliceControllerWidget *Control1;
  vtkSlicerSliceControllerWidget *Control2;

  int NeedOrientationUpdate0;
  int NeedOrientationUpdate1;
  int NeedOrientationUpdate2;

  void UpdateAll();
  void UpdateLocator();
  void UpdateSliceDisplay(float nx, float ny, float nz, 
                          float tx, float ty, float tz, 
                          float px, float py, float pz);

private:
  vtkNeuroNavGUI ( const vtkNeuroNavGUI& ); // Not implemented.
  void operator = ( const vtkNeuroNavGUI& ); //Not implemented.

  void BuildGUIForTrackingFrame ();
  void BuildGUIForRegistrationFrame ();

  void TrackerLoop();

  bool CloseScene;
  int TimerFlag;
  int TimerInterval;

};

#endif
