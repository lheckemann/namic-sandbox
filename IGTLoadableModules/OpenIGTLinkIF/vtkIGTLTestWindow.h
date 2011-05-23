/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/

#ifndef __vtkIGTLTestWindow_h
#define __vtkIGTLTestWindow_h

// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include "vtkOpenIGTLinkIFWin32Header.h"

#include "vtkSmartPointer.h"
//#include "vtkSlicerFiducialListWidget.h"
//#include "vtkSlicerROIViewerWidget.h"
//#include "vtkSlicerSecondaryViewerWidget.h"
#include "vtkSlicerViewerWidget.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWPushButton.h"
#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLIGTLQueryNode.h"

class vtkOpenIGTLinkIFGUI;
class vtkKWRadioButtonSet;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLTestWindow : public vtkKWTopLevel
{
public:

  //BTX
  enum {
    MODE_RANDOM = 0,
    MODE_FILE = 1,
  };
  //ETX

  vtkGetObjectMacro(MRMLScene, vtkMRMLScene);
  vtkSetObjectMacro(MRMLScene, vtkMRMLScene);

  vtkGetObjectMacro(Connector, vtkMRMLIGTLConnectorNode);
  vtkSetObjectMacro(Connector, vtkMRMLIGTLConnectorNode);

  static vtkIGTLTestWindow *New();  
  vtkTypeRevisionMacro(vtkIGTLTestWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);
  void SetInMRMLCallbackFlag (int flag) {
    this->InMRMLCallbackFlag = flag;
  }
  vtkGetMacro(InMRMLCallbackFlag, int);

  void SetAndObserveMRMLScene ( vtkMRMLScene *mrml )
    {
    vtkMRMLScene *oldValue = this->MRMLScene;
    this->MRMLObserverManager->SetAndObserveObject ( vtkObjectPointer( &this->MRMLScene), (vtkObject*)mrml );
    if ( oldValue != this->MRMLScene )
      {
      this->InvokeEvent (vtkCommand::ModifiedEvent);
      }
    }

  void SetAndObserveMRMLSceneEvents ( vtkMRMLScene *mrml, vtkIntArray *events )
    {
    vtkObject *oldValue = this->MRMLScene;
    this->MRMLObserverManager->SetAndObserveObjectEvents ( vtkObjectPointer( &this->MRMLScene), mrml, events );
    if ( oldValue != this->MRMLScene )
      {
      this->InvokeEvent (vtkCommand::ModifiedEvent);
      }
    }

  void DisplayOnWindow();
  void SetOpenIGTLinkIFGUI(vtkOpenIGTLinkIFGUI* moduleGUI)
  {
    this->ModuleGUI = moduleGUI;
  }

protected:

  vtkIGTLTestWindow();
  ~vtkIGTLTestWindow();  

  static void MRMLCallback(vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData );

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();

  void    SwitchMode(int mode);
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  vtkSlicerViewerWidget* ViewerWidget;
  
  vtkKWRadioButtonSet* ModeButtonSet;

  vtkKWMultiColumnListWithScrollbars* RemoteDataList;

  vtkKWPushButton* StartTrackingButton;
  vtkKWPushButton* StopTrackingButton;
  vtkKWPushButton* CloseButton;

  vtkCallbackCommand *MRMLCallbackCommand;  
  vtkCallbackCommand *GUICallbackCommand;
  vtkObserverManager *MRMLObserverManager;
  int InGUICallbackFlag;
  int InMRMLCallbackFlag;
  int IsObserverAddedFlag;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  bool MultipleMonitorsAvailable; 
  int  WindowPosition[2]; // virtual screen position in pixels
  int  WindowSize[2]; // virtual screen size in pixels
  
  int  Mode;
  

  vtkOpenIGTLinkIFGUI* ModuleGUI;
  vtkMRMLScene* MRMLScene;
  
  vtkMRMLIGTLConnectorNode* Connector;
  
  vtkMRMLIGTLQueryNode* TrackingDataQueryNode;
  //BTX
  std::list<vtkMRMLIGTLQueryNode*> ImageQueryNodeList;
  //ETX

 private:
  vtkIGTLTestWindow(const vtkIGTLTestWindow&);
  void operator=(const vtkIGTLTestWindow&);


};

#endif
