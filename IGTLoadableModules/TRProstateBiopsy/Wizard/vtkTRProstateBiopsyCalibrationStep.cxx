#include "vtkTRProstateBiopsyCalibrationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerColorLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkKWLabel.h"
#include "vtkKWPushButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWSpinBoxWithLabel.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMessageDialog.h"

#include "vtkImageChangeInformation.h"

// for DICOM read
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

#ifdef _WIN32
#  define slicerCerr(x) \
  do { \
    cout << x; \
    vtkstd::ostringstream str; \
    str << x; \
    MessageBox(NULL, str.str().c_str(), "Slicer Error", MB_OK);\
  } while (0)
#else
#  define slicerCerr(x) \
  cout << x
#endif

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyCalibrationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyCalibrationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::vtkTRProstateBiopsyCalibrationStep()
{
  this->SetName("1/3. Calibration");
  this->SetDescription("Perform robot calibration.");

  this->ButtonFrame  = NULL;
  this->LoadCalibrationImageButton = NULL;
  this->ResetCalibrationButton = NULL; 

  this->FiducialDimensionsFrame = NULL;
  this->FiducialWidthSpinBox = NULL;
  this->FiducialHeightSpinBox = NULL;
  this->FiducialDepthSpinBox = NULL;

  this->FiducialThresholdFrame = NULL;
  this->FiducialThresholdScale1 = NULL;
  this->FiducialThresholdScale2 = NULL;
  this->FiducialThresholdScale3 = NULL;
  this->FiducialThresholdScale4 = NULL;

  this->FiducialRadiusFrame = NULL;
  this->FiducialRadiusSpinBox = NULL;

  this->InitialAngleSpinBox = NULL;
}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::~vtkTRProstateBiopsyCalibrationStep()
{
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  if (!this->ButtonFrame)
    {
    this->ButtonFrame = vtkKWFrame::New();
    this->ButtonFrame->SetParent(parent);
    this->ButtonFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ButtonFrame->GetWidgetName());

  if (!this->LoadCalibrationImageButton)
    {
    this->LoadCalibrationImageButton = vtkKWLoadSaveButtonWithLabel::New();
    this->LoadCalibrationImageButton->SetParent(this->ButtonFrame);
    this->LoadCalibrationImageButton->Create();
    this->LoadCalibrationImageButton->GetWidget()->SetWidth(240);
    this->LoadCalibrationImageButton->GetWidget()->SetText(
                    "Browse For DICOM Series");
    this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
            ->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
      ->RetrieveLastPathFromRegistry("OpenPath");
    }
  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->LoadCalibrationImageButton->GetWidgetName());

  if (!this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton = vtkKWPushButton::New();
    this->ResetCalibrationButton->SetParent(this->ButtonFrame);
    this->ResetCalibrationButton->Create();
    this->ResetCalibrationButton->SetText("Reset Calibration");
    this->ResetCalibrationButton->SetBalloonHelpString("Reset the calibration");
    }

  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
               this->ResetCalibrationButton->GetWidgetName());

  if (!this->FiducialDimensionsFrame)
    {
    this->FiducialDimensionsFrame = vtkKWFrameWithLabel::New();
    this->FiducialDimensionsFrame->SetParent(parent);
    this->FiducialDimensionsFrame->Create();
    this->FiducialDimensionsFrame->SetLabelText(
                    "Approximate fiducial dimensions");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->FiducialDimensionsFrame->GetWidgetName());

  if (!this->FiducialWidthSpinBox)
    {
    this->FiducialWidthSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialWidthSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialWidthSpinBox->Create();
    this->FiducialWidthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialWidthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialWidthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialWidthSpinBox->GetWidget()->SetValue(8);
    this->FiducialWidthSpinBox->SetLabelText("W");
    }

  if (!this->FiducialHeightSpinBox)
    {
    this->FiducialHeightSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialHeightSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialHeightSpinBox->Create();
    this->FiducialHeightSpinBox->GetWidget()->SetWidth(11);
    this->FiducialHeightSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialHeightSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialHeightSpinBox->GetWidget()->SetValue(5);
    this->FiducialHeightSpinBox->SetLabelText("H");
    }

  if (!this->FiducialDepthSpinBox)
    {
    this->FiducialDepthSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialDepthSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialDepthSpinBox->Create();
    this->FiducialDepthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialDepthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialDepthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialDepthSpinBox->GetWidget()->SetValue(5);
    this->FiducialDepthSpinBox->SetLabelText("D");
    }

  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->FiducialWidthSpinBox->GetWidgetName(),
               this->FiducialHeightSpinBox->GetWidgetName(),
               this->FiducialDepthSpinBox->GetWidgetName());

  if (!this->FiducialThresholdFrame)
    {
    this->FiducialThresholdFrame = vtkKWFrameWithLabel::New();
    this->FiducialThresholdFrame->SetParent(parent);
    this->FiducialThresholdFrame->Create();
    this->FiducialThresholdFrame->SetLabelText("Fiducial thresholds");
    this->FiducialThresholdFrame->CollapseFrame();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->FiducialThresholdFrame->GetWidgetName());

  if (!this->FiducialThresholdScale1)
    {
    this->FiducialThresholdScale1 = vtkKWScaleWithEntry::New();
    this->FiducialThresholdScale1->SetParent(
                    this->FiducialThresholdFrame->GetFrame());
    this->FiducialThresholdScale1->Create();
    this->FiducialThresholdScale1->GetWidget()->SetLength(200);
    this->FiducialThresholdScale1->GetWidget()->SetRange(0,100);
    this->FiducialThresholdScale1->GetWidget()->SetResolution(1);
    this->FiducialThresholdScale1->GetWidget()->SetValue(9);
    this->FiducialThresholdScale1->SetValue(9);
    this->FiducialThresholdScale1->SetLabelText("1");
    this->FiducialThresholdScale1->SetWidth(3);
    }

  if (!this->FiducialThresholdScale2)
    {
    this->FiducialThresholdScale2 = vtkKWScaleWithEntry::New();
    this->FiducialThresholdScale2->SetParent(
                    this->FiducialThresholdFrame->GetFrame());
    this->FiducialThresholdScale2->Create();
    this->FiducialThresholdScale2->GetWidget()->SetLength(200);
    this->FiducialThresholdScale2->GetWidget()->SetRange(0,100);
    this->FiducialThresholdScale2->GetWidget()->SetResolution(1);
    this->FiducialThresholdScale2->GetWidget()->SetValue(9);
    this->FiducialThresholdScale2->SetValue(9);
    this->FiducialThresholdScale2->SetLabelText("2");
    this->FiducialThresholdScale2->SetWidth(3);
    }

  if (!this->FiducialThresholdScale3)
    {
    this->FiducialThresholdScale3 = vtkKWScaleWithEntry::New();
    this->FiducialThresholdScale3->SetParent(
                    this->FiducialThresholdFrame->GetFrame());
    this->FiducialThresholdScale3->Create();
    this->FiducialThresholdScale3->GetWidget()->SetLength(200);
    this->FiducialThresholdScale3->GetWidget()->SetRange(0,100);
    this->FiducialThresholdScale3->GetWidget()->SetResolution(1);
    this->FiducialThresholdScale3->GetWidget()->SetValue(9);
    this->FiducialThresholdScale3->SetValue(9);
    this->FiducialThresholdScale3->SetLabelText("3");
    this->FiducialThresholdScale3->SetWidth(3);
    }

  if (!this->FiducialThresholdScale4)
    {
    this->FiducialThresholdScale4 = vtkKWScaleWithEntry::New();
    this->FiducialThresholdScale4->SetParent(
                    this->FiducialThresholdFrame->GetFrame());
    this->FiducialThresholdScale4->Create();
    this->FiducialThresholdScale4->GetWidget()->SetLength(200);
    this->FiducialThresholdScale4->GetWidget()->SetRange(0,100);
    this->FiducialThresholdScale4->GetWidget()->SetResolution(1);
    this->FiducialThresholdScale4->GetWidget()->SetValue(9);
    this->FiducialThresholdScale4->SetValue(9);
    this->FiducialThresholdScale4->SetLabelText("4");
    this->FiducialThresholdScale4->SetWidth(3);
    }

  this->Script("pack %s %s %s %s -side top -anchor w -padx 2 -pady 2", 
               this->FiducialThresholdScale1->GetWidgetName(),
               this->FiducialThresholdScale2->GetWidgetName(),
               this->FiducialThresholdScale3->GetWidgetName(),
               this->FiducialThresholdScale4->GetWidgetName() );




}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents(vtkObject *caller,
        unsigned long event, void *callData)
{
  slicerCerr("vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents");

  const char *eventName = vtkCommand::GetStringFromEventId(event);
  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style =
              vtkSlicerInteractorStyle::SafeDownCast(caller);
    return;
    }

  if (vtkKWLoadSaveButtonWithLabel::SafeDownCast(caller) ==
            this->LoadCalibrationImageButton
       && event == vtkKWPushButton::InvokedEvent)
    {
    const char *fileName =
        this->LoadCalibrationImageButton->GetWidget()->GetFileName();

    slicerCerr("vtkKWLoadSaveButtonWithLabel");
  
    if ( fileName )
      {
      slicerCerr("fileName");
     
      // don't center volume, or treat as label map
      int centered = 0;
      int labelMap = 0;

      std::string fileString(fileName);
      for (unsigned int i = 0; i < fileString.length(); i++)
        {
        if (fileString[i] == '\\')
          {
          fileString[i] = '/';
          }
        }
      vtkSlicerApplication *app =
              (vtkSlicerApplication *)this->GetApplication();
      vtkSlicerVolumesGUI *volGui =
              (vtkSlicerVolumesGUI*)app->GetModuleGUIByName("Volumes");
      vtkSlicerVolumesLogic *volumeLogic =
              (vtkSlicerVolumesLogic*)(volGui->GetLogic());

      //volumeLogic->AddObserver(vtkCommand::ProgressEvent,
        //              this->LogicCallbackCommand);

      vtkMRMLVolumeNode *volumeNode = NULL;
      std::string archetype( fileName );
      volumeNode = volumeLogic->AddArchetypeVolume( fileString.c_str(),
                                                    archetype.c_str(),
                                                    (labelMap << 0) |
                                                    (centered << 1));

      slicerCerr("Added Volume");

      if ( volumeNode == NULL )
        {
        vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
        dialog->SetParent (this->ButtonFrame);
        dialog->SetStyleToMessage();
        std::string msg = std::string("Unable to read volume file ")
                + std::string(fileName);
        dialog->SetText(msg.c_str());
        dialog->Create();
        dialog->Invoke();
        dialog->Delete();
        }
      this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
              ->SaveLastPathToRegistry("OpenPath");
      if (volumeNode)
        {
        volGui->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID(
                        volumeNode->GetID());
        volGui->GetApplicationLogic()->PropagateVolumeSelection();
        }
      //volumeLogic->RemoveObservers(vtkCommand::ProgressEvent,  this->GUICallbackCommand);
      }
    }
}

vtkMRMLVolumeNode* vtkTRProstateBiopsyCalibrationStep::AddVolumeNode(
                vtkSlicerVolumesLogic* volLogic, const char* volumeNodeName)
{

  std::cerr << "AddVolumeNode(): called." << std::endl;

  vtkMRMLVolumeNode *volumeNode = NULL;

  if (volumeNode == NULL)  // if real-time volume node has not been created
    {

      vtkMRMLVolumeDisplayNode *displayNode = NULL;
      vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();

      vtkImageData* image = vtkImageData::New();

      //image->SetDimensions(RealtimeXsize, RealtimeYsize, 1);
      image->SetDimensions(256, 256, 1);
      //image->SetExtent( xmin, xmax, ymin, ymax, zmin, zmax );
      image->SetExtent(0, 255, 0, 255, 0, 0 );
      image->SetNumberOfScalarComponents( 1 );
      image->SetOrigin( 0, 0, 0 );
      image->SetSpacing( 1, 1, 0 );
      image->SetScalarTypeToShort();
      image->AllocateScalars();

      short* dest = (short*) image->GetScalarPointer();
      if (dest) {
       memset(dest, 0x01, 256*100*sizeof(short));
       image->Update();
      
      }
  
      /*
      vtkMRMLSliceNode *sliceorient = vtkMRMLSliceNode::New();
      sliceorient->SetSliceToRAS(LocatorMatrix);
      sliceorient->UpdateMatrices();
      */

      vtkSlicerSliceLayerLogic *reslice = vtkSlicerSliceLayerLogic::New();
      reslice->SetUseReslice(0);

      vtkImageChangeInformation *ici = vtkImageChangeInformation::New();
      ici->SetInput (image);
      ici->SetOutputSpacing( 1, 1, 1 );
      ici->SetOutputOrigin( 0, 0, 0 );
      ici->Update();
      scalarNode->SetAndObserveImageData (ici->GetOutput());

      vtkMatrix4x4* mat = vtkMatrix4x4::New();
      double space[3];
      int dim[3];
      space[0] = 1;
      space[1] = 1;
      space[2] = 10;
      dim[0]   = 256;
      dim[1]   = 256;
      dim[2]   = 1;
      scalarNode->ComputeIJKToRASFromScanOrder("IS",
                                               // possible is IS, PA, LR
                                              //image->GetSpacing(),
                                              space,
                                              //image->GetDimensions(),
                                              dim,
                                              true, mat);
      scalarNode->SetIJKToRASMatrix(mat);
      mat->Delete();
      image->Delete();

      ici->Delete();


      /* Based on the code in vtkSlicerVolumeLogic::AddHeaderVolume() */

      displayNode = vtkMRMLVolumeDisplayNode::New();
      scalarNode->SetLabelMap(0);
      volumeNode = scalarNode;

      if (volumeNode != NULL)
        {
          volumeNode->SetName(volumeNodeName);
          volLogic->GetMRMLScene()->SaveStateForUndo();

          vtkDebugMacro("Setting scene info");
          volumeNode->SetScene(volLogic->GetMRMLScene());
          displayNode->SetScene(volLogic->GetMRMLScene());

          //should we give the user the chance to modify this?.
          double range[2];
          vtkDebugMacro("Set basic display info");
          volumeNode->GetImageData()->GetScalarRange(range);
           range[0] = 0.0;
           range[1] = 256.0;
          // These methods aren't defined
          //displayNode->SetLowerThreshold(range[0]);
          //displayNode->SetUpperThreshold(range[1]);
          //displayNode->SetWindow(range[1] - range[0]);
          //displayNode->SetLevel(0.5 * (range[1] - range[0]) );

          vtkDebugMacro("Adding node..");
          volLogic->GetMRMLScene()->AddNode(displayNode);

          //displayNode->SetDefaultColorMap();
          vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
          displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
          colorLogic->Delete();

          volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());

          vtkDebugMacro("Name vol node "<<volumeNode->GetClassName());
          vtkDebugMacro("Display node "<<displayNode->GetClassName());

          volLogic->GetMRMLScene()->AddNode(volumeNode);
          vtkDebugMacro("Node added to scene");

          volLogic->SetActiveVolumeNode(volumeNode);
          volLogic->Modified();
        }

      scalarNode->Delete();

      if (displayNode)
        {
          displayNode->Delete();
        }
    }

  return volumeNode;

}

