/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyRobotWidget - ...
// .SECTION Description
// vtkTRProstateBiopsyRobotWidget ... TODO: to be completed

#ifndef __vtkTRProstateBiopsyRobotWidget_h
#define __vtkTRProstateBiopsyRobotWidget_h

#include "vtkObject.h"
#include "vtkTRProstateBiopsyWin32Header.h"

#include "vtkSlicerWidget.h"
#include "vtkKWRenderWidget.h"

#include "vtkSmartPointer.h"

class vtkSlicerViewerWidget;
class vtkKWRenderWidget;


class vtkMRMLTRProstateBiopsyModuleNode;
class vtkLineSource;
class vtkActor;


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyRobotWidget : public vtkSlicerWidget
{

//BTX
class RobotWidget
{
public:
  RobotWidget();
  virtual ~RobotWidget();
  void SetRenderer(vtkKWRenderWidget* renderer); // add to renderer  
  void UpdatePosition(vtkMRMLTRProstateBiopsyModuleNode *robot);
protected:
  void UpdateNeedleTrajectoryPosition(vtkMRMLTRProstateBiopsyModuleNode *robot);
  void UpdateRobotManipulatorPosition(vtkMRMLTRProstateBiopsyModuleNode *robot);
  void SetupNeedleTrajectoryActor();
  void SetupRobotManipulatorActor();

  vtkKWRenderWidget* Renderer;
  // Needle trajectory
  vtkSmartPointer<vtkActor> NeedleTrajectoryActor;
  vtkSmartPointer<vtkLineSource> NeedleTrajectoryLine;  
  // Robot manipulator
  vtkSmartPointer<vtkActor> RobotManipulatorActor;
  vtkSmartPointer<vtkLineSource> RobotManipulatorSheathLine;
};
//ETX

public:
  static vtkTRProstateBiopsyRobotWidget* New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyRobotWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  //virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // add mrml scene observers 
  virtual void AddMRMLObservers();
  
  // Description:
  // removes observers on widgets in the class
  virtual void RemoveMRMLObservers ( );

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  
  // Description:
  // Set/Get the main viewer, called by vtkSlicerApplicationGUI
  void SetMainViewerWidget(vtkSlicerViewerWidget *viewer)
    {
    this->MainViewerWidget = viewer;
    };

  vtkGetObjectMacro(MainViewerWidget, vtkSlicerViewerWidget);
  
  // Description:
  // Updates all robot's based on mrml nodes
  void UpdateFromMRML();

  // Description:
  // Updates an robot's based on mrml nodes
  void UpdateRobotFromMRML(vtkMRMLTRProstateBiopsyModuleNode *robot);

  // Description: 
  // Post a request for a render -- won't be done until the system is
  // idle, and then only once....
  void RequestRender();

  // Description: 
  // Actually do a render (don't wait for idle)
  void Render();


  // Description:
  // return the current robot widget corresponding to a given MRML ID
  RobotWidget* GetRobotWidgetByID (const char *id);

  // Description:
  // Remove all the box widgets from the scene and memory
  void RemoveRobotWidgets();

  // Description:
  // Remove the box widget associated with this id
  void RemoveRobotWidget(const char *pointID);
  
  // Description:
  // Remove the box widget
  void RemoveRobotWidget(RobotWidget* boxWidget);
  
  void SetProcessingWidgetEvent(int flag)
  {
    this->ProcessingWidgetEvent = flag;
  };

protected:
  vtkTRProstateBiopsyRobotWidget();
  virtual ~vtkTRProstateBiopsyRobotWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  // Description:
  // Add observers for this MRML node
  void AddMRMLRobotObservers(vtkMRMLTRProstateBiopsyModuleNode *robot);

  // Description:
  // Remove observers for all MRML nodes
  void RemoveMRMLRobotObservers();

  // Description:
  // Remove observers for this MRML node
  void RemoveMRMLRobotNodeObservers(vtkMRMLTRProstateBiopsyModuleNode *robot);
  
  
  // Description:
  // Removes observers that this widget placed on the Robot node in the
  // mrml tree
  void RemoveRobotObservers();

  // Description:
  // Goes through the MRML scene and for each Robot
  // node, calls UpdateRobotFromMRML. Once done, requests a render.
  void UpdateRobotsFromMRML();

  
  //BTX
  std::map<std::string, RobotWidget*> DisplayedRobotWidgets; 
  //ETX
  
  // Description:
  // Flag set to 1 when processing mrml events
  int ProcessingMRMLEvent;
  int ProcessingWidgetEvent;
  

  // Description:
  // A pointer back to the main viewer, so that can render when update
  vtkSlicerViewerWidget *MainViewerWidget;
 
    
private:
  
  vtkTRProstateBiopsyRobotWidget(const vtkTRProstateBiopsyRobotWidget&); // Not implemented
  void operator=(const vtkTRProstateBiopsyRobotWidget&); // Not Implemented

};

#endif // __vtkTRProstateBiopsyRobotWidget_h
