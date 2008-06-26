#include "vtkPerkStationEvaluateStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkMRMLPerkStationModuleNode.h"


#include "vtkKWWizardStep.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationEvaluateStep);
vtkCxxRevisionMacro(vtkPerkStationEvaluateStep, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkPerkStationEvaluateStep::vtkPerkStationEvaluateStep()
{
  this->SetName("5/5. Evaluation");
  this->SetDescription("Performance evaluation");

 
}

//----------------------------------------------------------------------------
vtkPerkStationEvaluateStep::~vtkPerkStationEvaluateStep()
{
  
}

//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  
  // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();

  
}

//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();
 // this->RemoveRunRegistrationOutputGUIObservers();
}



//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::AddRunRegistrationOutputGUIObservers() 
{
  this->RunSegmentationOutVolumeSelector->AddObserver(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent, 
    this->GetGUI()->GetGUICallbackCommand()); 
}
*/
//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RemoveRunRegistrationOutputGUIObservers()
{
  this->RunSegmentationOutVolumeSelector->RemoveObservers(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent, 
    this->GetGUI()->GetGUICallbackCommand()); 
}*/

//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::ProcessRunRegistrationOutputGUIEvents(
  vtkObject *caller,
  unsigned long event,
  void *callData) 
{
  if (caller == this->RunSegmentationOutVolumeSelector && 
      event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
      this->RunSegmentationOutVolumeSelector->GetSelected() != NULL) 
    { 
    vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
    if (mrmlManager)
      {
      mrmlManager->SetOutputVolumeMRMLID(
        this->RunSegmentationOutVolumeSelector->GetSelected()->GetID());
      }
    }

}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SelectTemplateFileCallback()
{
  // The template file has changed because of user interaction

  if (this->RunSegmentationSaveTemplateButton && 
      this->RunSegmentationSaveTemplateButton->IsCreated())
    {
    if (this->RunSegmentationSaveTemplateButton->GetLoadSaveDialog()->
        GetStatus() == vtkKWDialog::StatusOK)
      {
      this->RunSegmentationSaveTemplateButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry("OpenPath");
      vtksys_stl::string filename = 
        this->RunSegmentationSaveTemplateButton->GetFileName();
      vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
      vtkPerkStationModuleLogic *logic = this->GetGUI()->GetLogic();
      if (mrmlManager)
        {
        mrmlManager->SetSaveTemplateFilename(filename.c_str());
        }
      if (logic)
        {
        logic->SaveTemplateNow();
        }
      }
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SelectDirectoryCallback()
{
  // The template file has changed because of user interaction

  if (this->RunSegmentationDirectoryButton && 
      this->RunSegmentationDirectoryButton->IsCreated())
    {
    if (this->RunSegmentationDirectoryButton->GetLoadSaveDialog()->
        GetStatus() == vtkKWDialog::StatusOK)
      {
      this->RunSegmentationDirectoryButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry("OpenPath");
      vtksys_stl::string filename = 
        this->RunSegmentationDirectoryButton->GetFileName();
      if(!vtksys::SystemTools::FileExists(filename.c_str()) ||
        !vtksys::SystemTools::FileIsDirectory(filename.c_str()))
        {
        if(!vtksys::SystemTools::MakeDirectory(filename.c_str()))
          {
          vtkErrorMacro("Can not create directory: " << filename.c_str());
          return;
          }
        }

      vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
      if (mrmlManager)
        {
        mrmlManager->SetSaveWorkingDirectory(filename.c_str());
        }
      }
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SaveAfterSegmentationCallback(
  int state)
{
  // The save template checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveTemplateAfterSegmentation(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SaveIntermediateCallback(int state)
{
  // The save intermediate checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveIntermediateResults(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::GenerateSurfaceModelsCallback(
  int state)
{
  // The save surface checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveSurfaceModels(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RunSegmentationROIMaxChangedCallback(
    int row, int col, const char *value)
{
  int ijk[3] = {0, 0, 0};
  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  mrmlManager->GetSegmentationBoundaryMax(ijk);
  ijk[col] = atoi(value);
  if (mrmlManager->HasGlobalParametersNode())
    {
    mrmlManager->SetSegmentationBoundaryMax(ijk);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RunSegmentationROIMinChangedCallback(
    int row, int col, const char *value)
{
  int ijk[3] = {0, 0, 0};
  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  mrmlManager->GetSegmentationBoundaryMin(ijk);
  ijk[col] = atoi(value);
  if (mrmlManager->HasGlobalParametersNode())
    {
    mrmlManager->SetSegmentationBoundaryMin(ijk);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::MultiThreadingCallback(int state)
{
  // The multithreading checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetEnableMultithreading(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::StartSegmentationCallback()
{
  vtkPerkStationModuleLogic *logic = this->GetGUI()->GetLogic();
  vtkMRMLPerkStationModuleNode* mrmlManager = this->GetGUI()->GetMRMLManager();

  if (!mrmlManager || !logic)
    {
    return;
    }
  
  // make sure that data types are the same
  if (!mrmlManager->DoTargetAndAtlasDataTypesMatch())
    {
    // popup an error message
    std::string errorMessage = 
      "Scalar type mismatch for input images; all image scalar types must be "
      "the same (including input channels and atlas images).";

    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                     NULL,
                                     "Input Image Error",
                                     errorMessage.c_str(),
                                     vtkKWMessageDialog::ErrorIcon | 
                                     vtkKWMessageDialog::InvokeAtPointer);    
    return;
    }

  // start the segmentation
  logic->StartSegmentation();
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::CancelSegmentationCallback()
{
  //vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  //if (mrmlManager)
  //  {
  //    mrmlManager->CancelSegmentation();
  //  }
}
*/
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::InstallCallbacks()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::PopulateControls()
{
}
