/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkSecondaryWindowViwerWindow_h
#define __vtkSecondaryWindowViwerWindow_h



// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include "vtkSecondaryWindowWin32Header.h"


#include "vtkSmartPointer.h"
//#include "vtkSlicerFiducialListWidget.h"
//#include "vtkSlicerROIViewerWidget.h"
//#include "vtkSlicerSecondaryViewerWidget.h"
#include "vtkSlicerViewerWidget.h"

#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

#include "vtkMRMLViewNode.h"

class VTK_VideoImporter_EXPORT vtkSecondaryWindowViwerWindow : 
  public vtkKWTopLevel
{
public:
  static vtkSecondaryWindowViwerWindow *New();  
  vtkTypeRevisionMacro(vtkSecondaryWindowViwerWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DisplayOnSecondaryMonitor();
  void SetCameraNode(vtkMRMLCameraNode* node);
  

  //void SetCurrentTarget(vtkTRProstateBiopsyTargetDescriptor* target);

protected:

  void UpdateSecondaryMonitorPoisition();
  virtual void CreateWidget();

  vtkSecondaryWindowViwerWindow();
  ~vtkSecondaryWindowViwerWindow();  

  vtkKWFrame* MainFrame;

  //vtkSlicerSecondaryViewerWidget* ViewerWidget;
  vtkSlicerViewerWidget* ViewerWidget;
  //vtkSlicerFiducialListWidget* FiducialListWidget;  
  //vtkSlicerROIViewerWidget* ROIViewerWidget;
  //vtkTRProstateBiopsyRobotWidget* RobotViewerWidget;

  bool MultipleMonitorsAvailable; 
  int  SecondaryMonitorPosition[2]; // virtual screen position in pixels
  int  SecondaryMonitorSize[2]; // virtual screen size in pixels

  //BTX
  //vtkTRProstateBiopsyTargetDescriptor* CurrentTarget;
  //ETX

private:
  vtkSecondaryWindowViwerWindow(const vtkSecondaryWindowViwerWindow&);
  void operator=(const vtkSecondaryWindownViwerWindow&);
};

#endif
