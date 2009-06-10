/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkThresholdingFilterGUI.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkThresholdingFilterGUI_h
#define __vtkThresholdingFilterGUI_h

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkThresholdingFilterLogic.h"


class vtkSlicerSliceWidget;
class vtkKWFrame;
class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWFrameWithLabel;
class vtkKWNotebook;
class vtkKWSpinBoxWithLabel;
class vtkSlicerApplicationLogic;

class VTK_ThresholdingFilter_EXPORT vtkThresholdingFilterGUI : public vtkSlicerModuleGUI
{
  public:
  static vtkThresholdingFilterGUI *New();
  vtkTypeMacro(vtkThresholdingFilterGUI,vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: Get/Set MRML node
  vtkGetObjectMacro (Logic, vtkThresholdingFilterLogic);
  vtkSetObjectMacro (Logic, vtkThresholdingFilterLogic);

  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic(reinterpret_cast<vtkThresholdingFilterLogic*> (logic)); 
  }

  // Description: Get/Set MRML node
  vtkGetObjectMacro (ThresholdingFilterNode, vtkMRMLThresholdingFilterNode);

  // Description:
  // Create widgets
  virtual void BuildGUI ( );

  // Description:
  // Add obsereves to GUI widgets
  virtual void AddGUIObservers ( );
  
  // Description:
  // Remove obsereves to GUI widgets
  virtual void RemoveGUIObservers ( );
  
  // Description:
  // Pprocess events generated by Logic
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event,
                                  void *callData ){};

  // Description:
  // Pprocess events generated by GUI widgets
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event,
                                  void *callData );

  // Description:
  // Pprocess events generated by MRML
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, 
                                  void *callData);
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( ){};
  virtual void Exit ( ){};

  // Description:
  // Get the categorization of the module.  The category is used for
  // grouping modules together into menus.
  const char *GetCategory() const {return "Filtering";}

protected:
  vtkThresholdingFilterGUI();
  virtual ~vtkThresholdingFilterGUI();
  vtkThresholdingFilterGUI(const vtkThresholdingFilterGUI&);
  void operator=(const vtkThresholdingFilterGUI&);

  // Description:
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  // Description:
  // Updates parameters values in MRML node based on GUI widgets 
  void UpdateMRML();
  
  vtkKWScaleWithEntry* ConductanceScale;
  vtkKWScaleWithEntry* TimeStepScale;
  vtkKWScaleWithEntry* NumberOfIterationsScale;
  
  vtkKWScaleWithEntry* Sagittal1Scale;
  vtkKWScaleWithEntry* Sagittal2Scale;
  
  vtkKWScaleWithEntry* Coronal1Scale;
  vtkKWScaleWithEntry* Coronal2Scale;
  
  vtkSlicerNodeSelectorWidget* VolumeSelector;
  vtkSlicerNodeSelectorWidget* OutVolumeSelector;
  vtkSlicerNodeSelectorWidget* GADNodeSelector;
  vtkSlicerNodeSelectorWidget* StorageVolumeSelector;
  vtkKWPushButton* ApplyButton;
  vtkKWPushButton* CatchButton;
  
  vtkThresholdingFilterLogic *Logic;
  vtkMRMLThresholdingFilterNode* ThresholdingFilterNode;
  vtkKWFrameWithLabel* ThresholdingFrame;
  vtkKWFrameWithLabel* VolumeSelectionFrame;
  
  vtkKWFrameWithLabel* ParametersFrame;
  vtkKWFrameWithLabel* AdvancedParametersFrame;
  
  vtkKWNotebook* Notebook;
  
  vtkKWScaleWithEntry* ShrinkFactor;
  vtkKWScaleWithEntry* MaxNumOfIt;
  vtkKWScaleWithEntry* NumOfFitLev;
  vtkKWScaleWithEntry* WienFilNoi;
  vtkKWScaleWithEntry* BiasField;
  vtkKWScaleWithEntry* Conv;
  
  vtkKWScaleWithEntry* SlidePositionScale;
  vtkKWScaleWithEntry* BiasIntensityScale;
  vtkKWScaleWithEntry* SlidePositionScale2;
  
};

#endif

