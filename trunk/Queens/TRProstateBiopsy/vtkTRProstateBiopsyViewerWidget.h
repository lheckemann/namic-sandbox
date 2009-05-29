/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTRProstateBiopsyViewerWidget.h,v $
  Date:      $Date: 2006/01/08 04:48:05 $
  Version:   $Revision: 1.45 $

=========================================================================auto=*/

// .NAME vtkSlicerNodeSelectorWidget - menu to select volumes from current mrml scene
// .SECTION Description
// Inherits most behavior from kw widget, but is specialized to observe
// the current mrml scene and update the entries of the pop up menu to correspond
// to the currently available volumes.  This widget also has a notion of the current selection
// that can be observed or set externally
//


#ifndef __vtkTRProstateBiopsyViewerWidget_h
#define __vtkTRProstateBiopsyViewerWidget_h

#include "vtkTRProstateBiopsyWin32Header.h"

#include "vtkSlicerWidget.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerModelHierarchyLogic.h"

#include "vtkSlicerRenderWidget.h"
#include "vtkKWFrame.h"
#include "vtkMRMLClipModelsNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLCameraNode.h"
#include "vtkMRMLViewNode.h"

#include "vtkSlicerViewerWidget.h"

class vtkMRMLDisplayableNode;
class vtkMRMLDisplayNode;
class vtkMRMLModelHierarchyNode;
class vtkSlicerModelHierarchyLogic;
class vtkPolyData;
class vtkCellArray;
class vtkProp3D;
class vtkActor;
class vtkActorText;
class vtkFollower;
class vtkImplicitBoolean;
class vtkPlane;
class vtkWorldPointPicker;
class vtkPropPicker;
class vtkCellPicker;
class vtkPointPicker;
class vtkSlicerBoxWidget2;
class vtkSlicerBoxRepresentation;


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyViewerWidget : public vtkSlicerViewerWidget
{

public:
  static vtkTRProstateBiopsyViewerWidget* New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyViewerWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObject *o) { Superclass::UnRegister(o); };

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // Updates Actors based on models in the scene
  void UpdateFromMRML();

    // Description: 
  // Actually do a render (don't wait for idle)
  void Render();

protected:

  vtkTRProstateBiopsyViewerWidget();
  virtual ~vtkTRProstateBiopsyViewerWidget();

  void UpdateCameraNode();
  void UpdateViewNode();

  void UpdateModelsFromMRML();

  int GetDisplayedModelsVisibility(vtkMRMLDisplayNode *model);

  // Description:
  // Create the widget.
  virtual void CreateWidget();



private:
  
  vtkTRProstateBiopsyViewerWidget(const vtkTRProstateBiopsyViewerWidget&); // Not implemented
  void operator=(const vtkTRProstateBiopsyViewerWidget&); // Not Implemented
};

#endif

