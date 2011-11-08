/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/

#ifndef __vtkProstateNavNeedleOrientationWindow_h
#define __vtkProstateNavNeedleOrientationWindow_h

// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include <queue>
#include <vector>

#include "vtkSmartPointer.h"
#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

#include "igtlMath.h"

#include "vtkProstateNav.h"
#include "vtkProstateNavTargetDescriptor.h"

class vtkKWRadioButtonSet;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWPushButton;
class vtkKWFrame;

class vtkKWText;
class vtkKWTextWithScrollbars;
class vtkMRMLProstateNavManagerNode;
class vtkMRMLTransPerinealProstateTemplateNode;
class vtkSlicerNodeSelectorWidget;
class vtkKWMultiColumnListWithScrollbars;
class vtkMRMLScene;
class vtkIntArray;


struct NeedleDescriptorStruct;

class VTK_PROSTATENAV_EXPORT vtkProstateNavNeedleOrientationWindow : public vtkKWTopLevel
{
public:

  //BTX
  enum {
    MODE_RANDOM = 0,
    MODE_FILE = 1,
  };
  //ETX

  static vtkProstateNavNeedleOrientationWindow *New();  
  vtkTypeRevisionMacro(vtkProstateNavNeedleOrientationWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  vtkSetStringMacro( TargetFiducialID );
  vtkGetStringMacro( TargetFiducialID );

  vtkSetMacro( SelectedTarget, int );
  vtkGetMacro( SelectedTarget, int );

  void ProcessTimerEvents();
  void DisplayOnWindow();

protected:

  vtkProstateNavNeedleOrientationWindow();
  ~vtkProstateNavNeedleOrientationWindow();  

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();

  virtual void UpdateEntryList();

  void OnMultiColumnListUpdate(int row, int col, char * str);
  void OnMultiColumnListSelectionChanged();
  
  void SetOrientation();
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  
  vtkKWPushButton* SetButton;
  vtkKWPushButton* ResetButton;
  vtkKWPushButton* CloseButton;

  vtkKWTextWithScrollbars* ReportText;

  vtkCallbackCommand *GUICallbackCommand;

  int InGUICallbackFlag;
  int IsObserverAddedFlag;

  // Orientation Window
  vtkSlicerNodeSelectorWidget*        EntryListSelectorWidget;
  vtkKWMultiColumnListWithScrollbars* EntryList;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  char * TargetFiducialID;
  int    SelectedTarget;

  bool   MultipleMonitorsAvailable; 
  int    WindowPosition[2]; // virtual screen position in pixels
  int    WindowSize[2]; // virtual screen size in pixels
  
 private:
  vtkProstateNavNeedleOrientationWindow(const vtkProstateNavNeedleOrientationWindow&);
  void operator=(const vtkProstateNavNeedleOrientationWindow&);


};

#endif
