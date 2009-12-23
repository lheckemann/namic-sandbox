/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkSbiaVolumesGUI_h
#define __vtkSbiaVolumesGUI_h

#include "vtkVolumes.h"

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkSbiaVolumesLogic.h"

#include "vtkSlicerVolumeDisplayWidget.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkSlicerDiffusionWeightedVolumeDisplayWidget.h"
#include "vtkSlicerDiffusionTensorVolumeDisplayWidget.h"
#include "vtkSlicerScalarVolumeDisplayWidget.h"
#include "vtkSlicerVectorVolumeDisplayWidget.h"
#include "vtkSlicerLabelMapVolumeDisplayWidget.h"
#include "vtkSlicerDiffusionEditorWidget.h"

#include "vtkSlicerVolumeHeaderWidget.h"
#include "vtkITKArchetypeImageSeriesReader.h"

#include "vtkSlicerDiffusionRSHVolumeDisplayWidget.h"

class vtkKWPushButton;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWLoadSaveButton;
class vtkKWEntryWithLabel;
class vtkSlicerModuleCollapsibleFrame;
class vtkSlicerVolumeFileHeaderWidget;
class vtkSlicerVolumeFileHeaderWidget;

class VTK_VOLUMES_EXPORT vtkSbiaVolumesGUI : public vtkSlicerModuleGUI
{
 public:
  // Description:
  // Usual vtk class functions
  static vtkSbiaVolumesGUI* New (  );
  vtkTypeRevisionMacro ( vtkSbiaVolumesGUI, vtkSlicerModuleGUI );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Description:
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro ( LoadVolumeButton, vtkKWLoadSaveButtonWithLabel );

  vtkGetObjectMacro ( Logic, vtkSbiaVolumesLogic );
  vtkGetObjectMacro ( VolumeNode, vtkMRMLVolumeNode );

  vtkGetStringMacro ( SelectedVolumeID);
  vtkSetStringMacro ( SelectedVolumeID );

  vtkGetObjectMacro (HelpFrame, vtkSlicerModuleCollapsibleFrame );
  vtkGetObjectMacro (LoadFrame, vtkSlicerModuleCollapsibleFrame );
  vtkGetObjectMacro (DisplayFrame, vtkSlicerModuleCollapsibleFrame );
  vtkGetObjectMacro (OptionFrame, vtkSlicerModuleCollapsibleFrame );

  vtkGetObjectMacro ( VolumeDisplayWidget, vtkSlicerVolumeDisplayWidget );
  vtkGetObjectMacro ( VolumeSelectorWidget, vtkSlicerNodeSelectorWidget );

  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  virtual void SetModuleLogic ( vtkSbiaVolumesLogic *logic )
      { this->SetLogic ( vtkObjectPointer (&this->Logic), logic ); }
  virtual void SetAndObserveModuleLogic ( vtkSbiaVolumesLogic *logic )
      { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }

  vtkGetObjectMacro(DiffusionEditorWidget, vtkSlicerDiffusionEditorWidget);

  // Description
  // Overloaded method for this modules' logic type
  virtual void SetModuleLogic( vtkSlicerLogic *logic )
      { this->SetModuleLogic (reinterpret_cast<vtkSbiaVolumesLogic*> (logic)); }

  // Description:
  // This method builds the Volumes module GUI
  virtual void BuildGUI ( );

     // Description:
  // This method releases references and key-bindings,
  // and optionally removes observers.
  virtual void TearDownGUI ( );

      // Description:
  // Methods for adding module-specific key bindings and
  // removing them.
  virtual void CreateModuleEventBindings ( );
  virtual void ReleaseModuleEventBindings ( );
  
  // Description:
  // Add/Remove observers on widgets in the GUI
  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );


  // Description:
  // Class's mediator methods for processing events invoked by
  // either the Logic, MRML or GUI.
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( vtkMRMLNode *node );
  virtual void Enter ( ) { this->Enter(NULL); };
  virtual void Exit ( );

  void CreateScalarDisplayWidget ( );
  void CreateVectorDisplayWidget ( );
  void CreateLabelMapDisplayWidget ( );
  void CreateDWIDisplayWidget ( );
  void CreateDTIDisplayWidget ( );
  void CreateRSHDisplayWidget ( );

  //BTX
  // Description:
  // Return the MetaDataDictionary from the ITK layer
  std::vector<std::string> Tags;
  std::vector<std::string> TagValues;
  void CopyTagAndValues( vtkITKArchetypeImageSeriesReader* reader );
  //ETX

  unsigned int GetNumberOfItemsInDictionary(); 
  bool HasKey( char* tag );
  const char* GetNthKey( unsigned int n );
  const char* GetNthValue( unsigned int n );
  const char* GetTagValue( char* tag );

protected:
  vtkSbiaVolumesGUI ( );
  virtual ~vtkSbiaVolumesGUI ( );

  char *SelectedVolumeID;

  // Module logic and mrml pointers
  vtkSbiaVolumesLogic *Logic;
  vtkMRMLVolumeNode *VolumeNode;

  virtual void UpdateFramesFromMRML();


  // Widgets for the Volumes module
  vtkKWLoadSaveButtonWithLabel *LoadVolumeButton;
  vtkKWEntryWithLabel  *NameEntry;
  vtkKWMenuButtonWithLabel *CenterImageMenu;
  vtkKWMenuButtonWithLabel *OrientImageMenu;
  vtkKWCheckButton *LabelMapCheckButton;
  vtkKWCheckButton *SingleFileCheckButton;
  vtkKWPushButton *ApplyButton;

  vtkSlicerModuleCollapsibleFrame *HelpFrame;
  vtkSlicerModuleCollapsibleFrame *LoadFrame;
  vtkSlicerModuleCollapsibleFrame *DisplayFrame;
  vtkSlicerModuleCollapsibleFrame *InfoFrame;
  vtkSlicerModuleCollapsibleFrame *OptionFrame;
  vtkSlicerModuleCollapsibleFrame *GradientFrame;

  vtkKWFrame *LabelMapDisplayFrame;
  vtkKWFrame *ScalarDisplayFrame;
  vtkKWFrame *VectorDisplayFrame;
  vtkKWFrame *DWIDisplayFrame;
  vtkKWFrame *DTIDisplayFrame;
  vtkKWFrame *RSHDisplayFrame;
  vtkKWFrame *VolumeDisplayFrame;

  vtkSlicerNodeSelectorWidget* VolumeSelectorWidget;
  vtkSlicerVolumeDisplayWidget *VolumeDisplayWidget;
  vtkSlicerLabelMapVolumeDisplayWidget *labelVDW;
  vtkSlicerScalarVolumeDisplayWidget *scalarVDW;
  vtkSlicerVectorVolumeDisplayWidget *vectorVDW;
  vtkSlicerDiffusionWeightedVolumeDisplayWidget *dwiVDW;
  vtkSlicerDiffusionTensorVolumeDisplayWidget *dtiVDW;
  vtkSlicerVolumeHeaderWidget *VolumeHeaderWidget;
  vtkSlicerVolumeFileHeaderWidget *VolumeFileHeaderWidget;    
  vtkSlicerDiffusionEditorWidget *DiffusionEditorWidget;

  vtkSlicerDiffusionRSHVolumeDisplayWidget *RSHVDW;

  vtkKWLabel *NACLabel;
  vtkKWLabel *NAMICLabel;
  vtkKWLabel *NCIGTLabel;
  vtkKWLabel *BIRNLabel;

private:
  vtkSbiaVolumesGUI ( const vtkSbiaVolumesGUI& ); // Not implemented.
  void operator = ( const vtkSbiaVolumesGUI& ); //Not implemented.
};

#endif
