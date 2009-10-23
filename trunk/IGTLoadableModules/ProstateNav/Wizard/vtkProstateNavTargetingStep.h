/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkProstateNavTargetingStep_h
#define __vtkProstateNavTargetingStep_h

#include "vtkProstateNavStep.h"

class vtkKWEntry;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWLabel;
class vtkKWLoadSaveButton;
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

class VTK_PROSTATENAV_EXPORT vtkProstateNavTargetingStep : public vtkProstateNavStep
{
public:
  static vtkProstateNavTargetingStep *New();
  vtkTypeRevisionMacro(vtkProstateNavTargetingStep,vtkProstateNavStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);

  virtual void UpdateGUI();

  void UpdateMRMLObserver();
  void OnMultiColumnListUpdate(int row, int col, char * str);
  void OnMultiColumnListSelectionChanged();
  void UpdateTargetListGUI();

protected:
  vtkProstateNavTargetingStep();
  ~vtkProstateNavTargetingStep();


  void ShowTargetPlanningFrame();
  void ShowTargetListFrame();
  void ShowTargetControlFrame();

  void ShowCoverage(bool show);

  unsigned int PopulateListWithTargetDetails(unsigned int targetDescIndex);

  // Description
  // Callback on the load targeting volume button
  void LoadTargetingVolumeButtonCallback(const char *fileName); 

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
  vtkKWLoadSaveButton *LoadTargetingVolumeButton;
  vtkKWFrame *LoadVolumeDialogFrame;
  vtkKWCheckButton *ShowCoverageButton;  
  vtkKWCheckButton *AddTargetsOnClickButton;
  vtkKWMenuButtonWithLabel *NeedleTypeMenuList;  

  // TargetList frame
  vtkKWFrame *TargetListFrame;
  vtkKWMultiColumnListWithScrollbars* TargetList;
  vtkKWPushButton *DeleteButton;

  // TargetControl frame
  vtkKWFrame *TargetControlFrame;
  vtkKWMatrixWidgetWithLabel* NeedlePositionMatrix;
  vtkKWMatrixWidgetWithLabel* NeedleOrientationMatrix;
  vtkKWPushButton *MoveButton;
  vtkKWPushButton *StopButton;

  vtkKWText *Message;

private:
  vtkProstateNavTargetingStep(const vtkProstateNavTargetingStep&);
  void operator=(const vtkProstateNavTargetingStep&);
};

#endif
