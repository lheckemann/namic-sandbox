/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerNodeSelectorWidget.h,v $
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


#ifndef __vtkSlicerDiffusionRSHVolumeDisplayWidget_h
#define __vtkSlicerDiffusionRSHVolumeDisplayWidget_h

#include "vtkVolumes.h"
#include "vtkSlicerVolumeDisplayWidget.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWWindowLevelThresholdEditor.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMenuButtonWithSpinButtonsWithLabel.h"
#include "vtkKWCheckButton.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLVolumeDisplayNode.h"

class vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget;

class vtkDiffusionRSHMathematicsSimple;
class vtkImageExtractComponents;
class vtkAssignAttribute;

class VTK_VOLUMES_EXPORT vtkSlicerDiffusionRSHVolumeDisplayWidget : public vtkSlicerVolumeDisplayWidget
{
  
public:
  static vtkSlicerDiffusionRSHVolumeDisplayWidget* New();
  vtkTypeRevisionMacro(vtkSlicerDiffusionRSHVolumeDisplayWidget,vtkSlicerVolumeDisplayWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // add observers on widgets in the class
  virtual void AddWidgetObservers ( );
  
  // Description:
  // removes observers on widgets in the class
  virtual void RemoveWidgetObservers();

  virtual void UpdateWidgetFromMRML();

protected:
  vtkSlicerDiffusionRSHVolumeDisplayWidget();
  virtual ~vtkSlicerDiffusionRSHVolumeDisplayWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  vtkKWMenuButtonWithSpinButtonsWithLabel* ScalarModeMenu;
  vtkSlicerModuleCollapsibleFrame* ScalarOptionsFrame; 
  vtkSlicerNodeSelectorWidget* ColorSelectorWidget;
  vtkKWWindowLevelThresholdEditor* WindowLevelThresholdEditor;
  vtkKWCheckButton* InterpolateButton;

  vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget *GlyphDisplayWidget;
  vtkDiffusionRSHMathematicsSimple *RSHMathematics;
  vtkImageExtractComponents *ExtractComponent;
  
  vtkAssignAttribute* AssignAttributeTensorsFromScalars;
  vtkAssignAttribute* AssignAttributeScalarsFromTensors;

  int UpdatingMRML;
  int UpdatingWidget;

  //BTX
  std::map <std::string, int> ScalarModeMap;
  std::map <std::string, int> GlyphModeMap;
  //ETX
private:

  vtkSlicerDiffusionRSHVolumeDisplayWidget(const vtkSlicerDiffusionRSHVolumeDisplayWidget&); // Not implemented
  void operator=(const vtkSlicerDiffusionRSHVolumeDisplayWidget&); // Not Implemented
};

#endif
