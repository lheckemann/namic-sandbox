/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerNodeSelectorWidget.h,v $
  Date:      $Date: 2006/01/08 04:48:05 $
  Version:   $Revision: 1.45 $

=========================================================================auto=*/

// .NAME vtkSlicerFiberBundleDisplayWidget - GUI for setting parameters of a 
// vtkMRMLFiberBundleDisplayNode
// .SECTION Description
//


#ifndef __vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget_h
#define __vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget_h

#include "vtkVolumes.h"
#include "vtkSmartPointer.h"

#include "vtkSlicerWidget.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerDiffusionRSHGlyphDisplayWidget.h"

#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWRange.h"
#include "vtkKWChangeColorButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWEntry.h"

#include "vtkMRMLDiffusionRSHVolumeNode.h"
#include "vtkMRMLDiffusionRSHVolumeSliceDisplayNode.h"


class VTK_VOLUMES_EXPORT vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget : public vtkSlicerWidget
{
  
public:
  static vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget* New();
  vtkTypeRevisionMacro(vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set FiberBundleDisplayNode currently active in this GUI.
  // Internally this method sets the FiberBundleNodeID and FiberBundleDisplayNodeID,
  // and sets up observers.
  void SetDiffusionRSHVolumeNode ( vtkMRMLDiffusionRSHVolumeNode *node );

  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
protected:
  vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget();
  virtual ~vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget();

  // Description:
  // add observers on display node
  virtual void AddMRMLObservers ( );

  // Description:
  // remove observers on display node
  virtual void RemoveMRMLObservers ( );
  
  // Description:
  // removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  // Description:
  // Update the widget's values to correspond to the MRML display node.
  void UpdateWidget();

  // Description:
  // Update the display node's values to correspond to the widget
  void UpdateMRML();
  
  // Description:
  // All of the widgets used in this widget
  vtkKWCheckButtonWithLabel    *VisibilityButton[3];
  vtkKWMenuButtonWithLabel     *GeometryColorMenu;
  vtkSlicerNodeSelectorWidget  *ColorSelectorWidget;
  vtkKWScaleWithLabel          *OpacityScale;

  vtkKWMenuButtonWithLabel     *AutoScalarRangeMenu;
  vtkKWRange                   *ScalarRange;
  vtkKWEntry                   *MinRangeEntry;
  vtkKWEntry                   *MaxRangeEntry;

  vtkSlicerDiffusionRSHGlyphDisplayWidget *GlyphDisplayWidget;

  vtkKWFrameWithLabel           *DisplayFrame;


  int UpdatingMRML;
  int UpdatingWidget;
  

  vtkMRMLDiffusionRSHVolumeNode *DiffusionRSHVolumeNode;

  vtkMRMLDiffusionRSHDisplayPropertiesNode* GetCurrentDiffusionRSHDisplayPropertyNode();

//BTX
  std::map <std::string, int> GeometryColorMap;
  std::vector< vtkSmartPointer< vtkMRMLDiffusionRSHVolumeSliceDisplayNode > > GlyphDisplayNodes;
//ETX
  
private:


  vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget(const vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget&); // Not implemented
  void operator=(const vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget&); // Not Implemented
};

#endif

