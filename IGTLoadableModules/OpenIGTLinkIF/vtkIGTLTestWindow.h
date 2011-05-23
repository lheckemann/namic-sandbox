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
#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

class vtkOpenIGTLinkIFGUI;
class vtkKWRadioButtonSet;
class vtkKWPushButton;
class vtkKWEntry;
class vtkKWFrame;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLTestWindow : public vtkKWTopLevel
{
public:

  //BTX
  enum {
    MODE_RANDOM = 0,
    MODE_FILE = 1,
  };
  //ETX

  static vtkIGTLTestWindow *New();  
  vtkTypeRevisionMacro(vtkIGTLTestWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  void DisplayOnWindow();

protected:

  vtkIGTLTestWindow();
  ~vtkIGTLTestWindow();  

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  
  void SwitchMode(int mode);
  void StartServer(int port, float rate);
  void StopServer();
  
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  
  vtkKWRadioButtonSet* ModeButtonSet;
  vtkKWEntry*          PortEntry;
  vtkKWEntry*          FrameRateEntry;

  vtkKWPushButton* StartTrackingButton;
  vtkKWPushButton* StopTrackingButton;
  vtkKWPushButton* CloseButton;

  vtkCallbackCommand *GUICallbackCommand;

  int InGUICallbackFlag;
  int IsObserverAddedFlag;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  bool   MultipleMonitorsAvailable; 
  int    WindowPosition[2]; // virtual screen position in pixels
  int    WindowSize[2]; // virtual screen size in pixels
  
  int    Mode;
  int    DefaultPort;
  double DefaultFrameRate;

 private:
  vtkIGTLTestWindow(const vtkIGTLTestWindow&);
  void operator=(const vtkIGTLTestWindow&);


};

#endif
