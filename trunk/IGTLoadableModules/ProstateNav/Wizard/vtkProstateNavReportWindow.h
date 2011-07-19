/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/

#ifndef __vtkProstateNavReportWindow_h
#define __vtkProstateNavReportWindow_h

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

class vtkKWRadioButtonSet;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWPushButton;
class vtkKWFrame;

class vtkKWText;
class vtkKWTextWithScrollbars;
class vtkMRMLProstateNavManagerNode;
class vtkMRMLTransPerinealProstateTemplateNode;
class vtkProstateNavTargetDescriptor;

class VTK_PROSTATENAV_EXPORT vtkProstateNavReportWindow : public vtkKWTopLevel
{
public:

  //BTX
  enum {
    MODE_RANDOM = 0,
    MODE_FILE = 1,
  };
  //ETX

  static vtkProstateNavReportWindow *New();  
  vtkTypeRevisionMacro(vtkProstateNavReportWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  void ProcessTimerEvents();
  void DisplayOnWindow();

  void GenerateReport(vtkMRMLTransPerinealProstateTemplateNode* robot, vtkProstateNavTargetDescriptor* desc);

protected:

  vtkProstateNavReportWindow();
  ~vtkProstateNavReportWindow();  

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  
  vtkKWRadioButtonSet* ModeButtonSet;
  vtkKWLoadSaveButtonWithLabel* SelectTrackingFileButton;

  vtkKWPushButton* StartTrackingButton;
  vtkKWPushButton* StopTrackingButton;
  vtkKWPushButton* CloseButton;

  vtkKWTextWithScrollbars* ReportText;

  vtkCallbackCommand *GUICallbackCommand;

  int InGUICallbackFlag;
  int IsObserverAddedFlag;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  bool   MultipleMonitorsAvailable; 
  int    WindowPosition[2]; // virtual screen position in pixels
  int    WindowSize[2]; // virtual screen size in pixels
  
 private:
  vtkProstateNavReportWindow(const vtkProstateNavReportWindow&);
  void operator=(const vtkProstateNavReportWindow&);


};

#endif
