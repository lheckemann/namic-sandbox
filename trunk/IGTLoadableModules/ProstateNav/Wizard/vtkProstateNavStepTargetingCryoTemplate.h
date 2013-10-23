/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkProstateNavStepTargetingCryoTemplate_h
#define __vtkProstateNavStepTargetingCryoTemplate_h

#include "vtkProstateNavStep.h"

class vtkKWEntry;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWLabel;
class vtkSlicerNodeSelectorWidget;
class vtkKWMatrixWidgetWithLabel;
class vtkKWMenuButton;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWPushButton;
class vtkKWText;
class vtkImageData;
class vtkMRMLScalarVolumeNode;
class vtkMRMLSelectionNode;
class vtkMRMLFiducialListNode;
class vtkKWCheckButton;

class vtkProstateNavReportWindow;

class VTK_PROSTATENAV_EXPORT vtkProstateNavStepTargetingCryoTemplate : public vtkProstateNavStep
{
 public:
  static vtkProstateNavStepTargetingCryoTemplate *New();
  vtkTypeRevisionMacro(vtkProstateNavStepTargetingCryoTemplate,vtkProstateNavStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);

  virtual void UpdateGUI();

  void AddMRMLObservers();
  void RemoveMRMLObservers();

  void OnMultiColumnListUpdate(int row, int col, char * str);
  void OnMultiColumnListSelectionChanged();
  void UpdateTargetListGUI();

 protected:
  vtkProstateNavStepTargetingCryoTemplate();
  ~vtkProstateNavStepTargetingCryoTemplate();


  void ShowTargetPlanningFrame();
  void ShowTargetListFrame();
  void ShowTargetControlFrame();

  void ShowTemplate(bool show);

  void EnableAddTargetsOnClickButton(bool enable);

  unsigned int PopulateListWithTargetDetails(unsigned int targetDescIndex);

  void AddGUIObservers();
  void RemoveGUIObservers();

  void NewFiducialAdded();

  //BTX
  // Description:
  // The column orders in the list box
  enum
  {
    TargetNumberColumn = 0,
    NeedleTypeColumn = 1,
    RASLocationColumn = 2,
    ReachableColumn = 3,
    RotationColumn = 4,
    NeedleAngleColumn = 5,
    DepthColumn = 6,
    NumberOfColumns = 7,
  };

  //ETX

  bool ProcessingCallback;

  vtkKWFrame *MainFrame;

  // TargetPlanning
  vtkKWFrame *TargetPlanningFrame;
  //vtkKWPushButton* LoadTargetingVolumeButton;
  vtkSlicerNodeSelectorWidget* VolumeSelectorWidget;
  vtkSlicerNodeSelectorWidget* TargetListSelectorWidget;
  vtkKWFrame *LoadVolumeDialogFrame;
  vtkKWCheckButton *AddTargetsOnClickButton;
  vtkKWCheckButton *ShowTemplateButton;
  vtkKWMenuButtonWithLabel *NeedleTypeMenuList;
  vtkMRMLFiducialListNode *TargetPlanListNode;
  vtkKWFrame *OptionFrame;

  // TargetList frame
  vtkKWFrame *TargetListFrame;
  vtkKWMultiColumnListWithScrollbars* TargetList;
  vtkKWPushButton *DeleteButton;

  // TargetControl frame
  vtkKWFrame *TargetControlFrame;
  vtkKWMatrixWidgetWithLabel* NeedlePositionMatrix;
  vtkKWMatrixWidgetWithLabel* NeedleOrientationMatrix;

  //vtkKWPushButton *MoveButton;
  //vtkKWPushButton *StopButton;
  vtkKWPushButton *GenerateListButton;
  vtkKWPushButton *MoveButton;

  vtkKWText *Message;

  // Taret list report window
  vtkProstateNavReportWindow* ReportWindow;

 private:
  vtkProstateNavStepTargetingCryoTemplate(const vtkProstateNavStepTargetingCryoTemplate&);
  void operator=(const vtkProstateNavStepTargetingCryoTemplate&);
};

#endif
