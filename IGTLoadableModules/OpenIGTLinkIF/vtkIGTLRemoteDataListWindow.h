/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/

#ifndef __vtkIGTLRemoteDataListWindow_h
#define __vtkIGTLRemoteDataListWindow_h

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

class vtkOpenIGTLinkIFGUI;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLRemoteDataListWindow : public vtkKWTopLevel
{
public:

  vtkGetObjectMacro(MRMLScene, vtkMRMLScene);
  vtkSetObjectMacro(MRMLScene, vtkMRMLScene);

  vtkGetObjectMacro(Connector, vtkMRMLIGTLConnectorNode);
  vtkSetObjectMacro(Connector, vtkMRMLIGTLConnectorNode);

  static vtkIGTLRemoteDataListWindow *New();  
  vtkTypeRevisionMacro(vtkIGTLRemoteDataListWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  void DisplayOnWindow();
  void SetOpenIGTLinkIFGUI(vtkOpenIGTLinkIFGUI* moduleGUI)
  {
    this->ModuleGUI = moduleGUI;
  }

protected:

  vtkIGTLRemoteDataListWindow();
  ~vtkIGTLRemoteDataListWindow();  
  
  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  void UpdateWindowPoisition();
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  vtkSlicerViewerWidget* ViewerWidget;
  
  vtkKWMultiColumnListWithScrollbars* RemoteDataList;
  vtkKWPushButton* GetButton;
  vtkKWPushButton* CloseButton;
  
  vtkCallbackCommand *GUICallbackCommand;
  int InGUICallbackFlag;
  int IsObserverAddedFlag;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  bool MultipleMonitorsAvailable; 
  int  WindowPosition[2]; // virtual screen position in pixels
  int  WindowSize[2]; // virtual screen size in pixels
  
  vtkOpenIGTLinkIFGUI* ModuleGUI;
  vtkMRMLScene* MRMLScene;
  
  vtkMRMLIGTLConnectorNode* Connector;

 private:
  vtkIGTLRemoteDataListWindow(const vtkIGTLRemoteDataListWindow&);
  void operator=(const vtkIGTLRemoteDataListWindow&);


};

#endif
