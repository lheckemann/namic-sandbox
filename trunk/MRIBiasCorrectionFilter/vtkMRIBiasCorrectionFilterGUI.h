/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRIBiasCorrectionFilterGUI.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRIBiasCorrectionFilterGUI_h
#define __vtkMRIBiasCorrectionFilterGUI_h

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkMRIBiasCorrectionFilterLogic.h"


class vtkSlicerSliceWidget;
class vtkKWFrame;
class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWFrameWithLabel;
class vtkKWSpinBoxWithLabel;
class vtkSlicerApplicationLogic;

class VTK_MRIBiasCorrectionFilter_EXPORT vtkMRIBiasCorrectionFilterGUI : public vtkSlicerModuleGUI
{
  public:
  static vtkMRIBiasCorrectionFilterGUI *New();
  vtkTypeMacro(vtkMRIBiasCorrectionFilterGUI,vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: Get/Set MRML node
  vtkGetObjectMacro (Logic, vtkMRIBiasCorrectionFilterLogic);
  vtkSetObjectMacro (Logic, vtkMRIBiasCorrectionFilterLogic);

  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic(reinterpret_cast<vtkMRIBiasCorrectionFilterLogic*> (logic)); 
  }

  // Description: Get/Set MRML node
  vtkGetObjectMacro (MRIBiasCorrectionFilterNode, vtkMRMLMRIBiasCorrectionFilterNode);

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
  vtkMRIBiasCorrectionFilterGUI();
  virtual ~vtkMRIBiasCorrectionFilterGUI();
  vtkMRIBiasCorrectionFilterGUI(const vtkMRIBiasCorrectionFilterGUI&);
  void operator=(const vtkMRIBiasCorrectionFilterGUI&);

  // Description:
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  // Description:
  // Updates parameters values in MRML node based on GUI widgets 
  void UpdateMRML();

  vtkKWScaleWithEntry* TimeStepScale;
  vtkKWScaleWithEntry* NumberOfIterationsScale;

  vtkKWScaleWithEntry* Sagittal2Scale;
  
  vtkKWScaleWithEntry* Coronal1Scale;
  vtkKWScaleWithEntry* Coronal2Scale;
  
  vtkSlicerNodeSelectorWidget* VolumeSelector;
  vtkSlicerNodeSelectorWidget* OutVolumeSelector;
  vtkSlicerNodeSelectorWidget* GADNodeSelector;
  vtkSlicerNodeSelectorWidget* StorageVolumeSelector;
  vtkSlicerNodeSelectorWidget* MaskVolumeSelector;
  
  vtkKWPushButton* ApplyButton;
  vtkKWPushButton* CatchButton;
  
  vtkMRIBiasCorrectionFilterLogic *Logic;
  vtkMRMLMRIBiasCorrectionFilterNode* MRIBiasCorrectionFilterNode;
  vtkKWFrameWithLabel* VolumeSelectionFrame;
  
  vtkKWFrameWithLabel* ParametersFrame;
  vtkKWFrameWithLabel* AdvancedParametersFrame;
  
  vtkKWScaleWithEntry* ShrinkFactor;
  vtkKWScaleWithEntry* MaxNumOfIt;
  vtkKWScaleWithEntry* NumOfFitLev;
  vtkKWScaleWithEntry* WienFilNoi;
  vtkKWScaleWithEntry* BiasField;
  vtkKWScaleWithEntry* Conv;
  
  vtkKWScaleWithEntry* SlidePositionScale;
  vtkKWScaleWithEntry* BiasIntensityScale;
  
};

#endif

