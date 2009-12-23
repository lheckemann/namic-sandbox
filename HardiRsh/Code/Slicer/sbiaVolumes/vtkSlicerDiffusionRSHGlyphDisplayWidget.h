/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerNodeSelectorWidget.h,v $
  Date:      $Date: 2006/01/08 04:48:05 $
  Version:   $Revision: 1.45 $

=========================================================================auto=*/

// .NAME vtkSlicerDiffusionRSHGlyphDisplayWidget - GUI for setting parameters of a 
// vtkMRMLDiffusionRSHGlyphDisplayNode
// .SECTION Description
//


#ifndef __vtkSlicerDiffusionRSHGlyphDisplayWidget_h
#define __vtkSlicerDiffusionRSHGlyphDisplayWidget_h

#include "vtkSlicerWidget.h"

#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWFrameWithLabel.h"

#include "vtkMRMLDiffusionRSHDisplayPropertiesNode.h"


class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerDiffusionRSHGlyphDisplayWidget : public vtkSlicerWidget
{
  
public:
  static vtkSlicerDiffusionRSHGlyphDisplayWidget* New();
  vtkTypeRevisionMacro(vtkSlicerDiffusionRSHGlyphDisplayWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set DiffusionRSHDisplayPropertiesNode currently active in this GUI.
  // Internally this method sets the DiffusionRSHDisplayPropertiesNodeID 
  // and sets up observers.
  void SetDiffusionRSHDisplayPropertiesNode ( vtkMRMLDiffusionRSHDisplayPropertiesNode *node );

  // Description:
  // Get MRML DiffusionRSHDisplayPropertiesNodeID.
  vtkGetStringMacro ( DiffusionRSHDisplayPropertiesNodeID );
  
  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  void SetGlyphResolution(int value);

  vtkGetObjectMacro(Frame, vtkKWFrameWithLabel);

  void Collapse() 
  {
    this->Frame->CollapseFrame();
  };
 void Expend() 
  {
    this->Frame->ExpandFrame ();
  };


 protected:
  vtkSlicerDiffusionRSHGlyphDisplayWidget();
  virtual ~vtkSlicerDiffusionRSHGlyphDisplayWidget();

  // Description:
  // add observers on display node
  virtual void AddMRMLObservers ( );

  // Description:
  // remove observers on display node
  virtual void RemoveMRMLObservers ( );

  // Description:
  // adds observers on widgets in the class
  virtual void AddWidgetObservers ( );

  // Description:
  // removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );

  // Description:
  // Set MRML DiffusionRSHDisplayPropertiesNodeID.
  vtkSetStringMacro ( DiffusionRSHDisplayPropertiesNodeID );

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
  // ID in the MRML scene of the current display properties node
  char* DiffusionRSHDisplayPropertiesNodeID;

  // Description:
  // All of the widgets used in this widget
  vtkKWMenuButtonWithLabel  *GlyphGeometryMenu;

  vtkKWFrameWithLabel *Frame;

  // advanced frame
  vtkKWScaleWithLabel  *GlyphScale;
  vtkKWScaleWithLabel  *GlyphResolutionScale;
  vtkKWScaleWithLabel  *GlyphAngularResolutionScale;
  vtkKWCheckButton     *GlyphMinMaxNormalizationButton;

  //BTX
  std::map <std::string, int> GlyphGeometryMap;
  //ETX

private:

  vtkSlicerDiffusionRSHGlyphDisplayWidget(const vtkSlicerDiffusionRSHGlyphDisplayWidget&); // Not implemented
  void operator=(const vtkSlicerDiffusionRSHGlyphDisplayWidget&); // Not Implemented
};

#endif

