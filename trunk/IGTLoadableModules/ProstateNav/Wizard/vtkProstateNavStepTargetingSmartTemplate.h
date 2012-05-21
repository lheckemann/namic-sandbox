/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkProstateNavStepTargetingSmartTemplate_h
#define __vtkProstateNavStepTargetingSmartTemplate_h

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

class VTK_PROSTATENAV_EXPORT vtkProstateNavStepTargetingSmartTemplate : public vtkProstateNavStep
{
public:
  static vtkProstateNavStepTargetingSmartTemplate *New();
  vtkTypeRevisionMacro(vtkProstateNavStepTargetingSmartTemplate,vtkProstateNavStep);
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
  vtkProstateNavStepTargetingSmartTemplate();
  ~vtkProstateNavStepTargetingSmartTemplate();


  void ShowTargetPlanningFrame();
  void ShowTargetListFrame();
  void ShowTargetControlFrame();

  void ShowTargetNeedle(bool show);
  void ShowCurrentNeedle(bool show);

  void EnableAddTargetsOnClickButton(bool enable);

  unsigned int PopulateListWithTargetDetails(unsigned int targetDescIndex);

  void AddGUIObservers();
  void RemoveGUIObservers();  
  
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
  vtkKWCheckButton *ShowWorkspaceButton;
  vtkKWCheckButton *AddTargetsOnClickButton;
  vtkKWCheckButton *ShowTargetNeedleButton;
  vtkKWCheckButton *ShowCurrentNeedleButton;
  vtkKWMenuButtonWithLabel *NeedleTypeMenuList;
  vtkMRMLFiducialListNode *TargetPlanListNode;
  vtkKWFrame *OptionFrame;

  // TargetList frame
  vtkKWFrame *TargetListFrame;
  vtkKWMultiColumnListWithScrollbars* TargetList;
  vtkKWPushButton *DeleteButton;

  // TargetControl frame
  vtkKWEntryWithLabel * TargetNameEntry;
  vtkKWFrame *TargetControlFrame;
  vtkKWMatrixWidgetWithLabel* NeedlePositionMatrix;
  vtkKWMatrixWidgetWithLabel* NeedleOffsetMatrix;
  //vtkKWMatrixWidgetWithLabel* NeedleOrientationMatrix;

  //vtkKWPushButton *MoveButton;
  //vtkKWPushButton *StopButton;
  vtkKWPushButton *GenerateListButton;
  vtkKWPushButton *MoveButton;

  vtkKWText *Message;

  // Taret list report window
  vtkProstateNavReportWindow* ReportWindow;

private:
  vtkProstateNavStepTargetingSmartTemplate(const vtkProstateNavStepTargetingSmartTemplate&);
  void operator=(const vtkProstateNavStepTargetingSmartTemplate&);

  float NeedleOffset[3];

};

#endif
