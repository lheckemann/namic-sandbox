#include "vtkPerkStationPlanStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkCylinderSource.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkIdentityTransform.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMath.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationPlanStep);
vtkCxxRevisionMacro(vtkPerkStationPlanStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkPerkStationPlanStep::vtkPerkStationPlanStep()
{
  this->SetName("2/5. Plan");
  this->SetDescription("Plan the needle insertion");

  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationPlanStep::WizardGUICallback);

  this->ResetFrame = NULL;
  this->ResetPlanButton = NULL;
  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionAngle = NULL;
  this->InsertionDepth = NULL;
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;

  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
  this->NeedleActor = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationPlanStep::~vtkPerkStationPlanStep()
{
  if (this->ResetFrame)
    {
    this->ResetFrame->Delete();
    this->ResetFrame = NULL;
    }
  if (this->ResetPlanButton)
    {
    this->ResetPlanButton->Delete();
    this->ResetPlanButton = NULL;
    }

  if (this->EntryPointFrame)
    {
    this->EntryPointFrame->Delete();
    this->EntryPointFrame = NULL;
    }
  if (this->EntryPointLabel)
    {
    this->EntryPointLabel->Delete();
    this->EntryPointLabel = NULL;
    }

  if (this->EntryPoint)
    {
    this->EntryPoint->DeleteAllWidgets();
    this->EntryPoint = NULL;
    }

  if (this->TargetPointFrame)
    {
    this->TargetPointFrame->Delete();
    this->TargetPointFrame = NULL;
    }
  if (this->TargetPointLabel)
    {
    this->TargetPointLabel->Delete();
    this->TargetPointLabel = NULL;
    }
  if (this->TargetPoint)
    {
    this->TargetPoint->DeleteAllWidgets();
    this->TargetPoint = NULL;
    }

  if (this->InsertionAngle)
    {
    this->InsertionAngle->Delete();
    this->InsertionAngle = NULL;
    }

  if (this->InsertionDepth)
    {
    this->InsertionDepth->Delete();
    this->InsertionDepth = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();

  // clear controls
  if (this->ResetFrame)
    {
    this->Script("pack forget %s", 
                    this->ResetFrame->GetWidgetName());
    }

  if (this->ResetPlanButton)
    {
    this->Script("pack forget %s", 
                    this->ResetPlanButton->GetWidgetName());
    }


  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::ModeId::Training:

      this->SetName("2/5. Plan");
      this->GetGUI()->GetWizardWidget()->Update();

       
      break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:
       
      // in clinical mode
      this->SetName("2/4. Plan");
      this->GetGUI()->GetWizardWidget()->Update();

      // additional reset button

      // frame for reset button

      if (!this->ResetFrame)
        {
        this->ResetFrame = vtkKWFrame::New();
        }
      if (!this->ResetFrame->IsCreated())
        {
        this->ResetFrame->SetParent(parent);
        this->ResetFrame->Create();     
        }
      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->ResetFrame->GetWidgetName());     
      
      

      if (!this->ResetPlanButton)
        {
        this->ResetPlanButton = vtkKWPushButton::New();
        }
      if(!this->ResetPlanButton->IsCreated())
        {
        this->ResetPlanButton->SetParent(this->ResetFrame);
        this->ResetPlanButton->SetText("Reset plan");
        this->ResetPlanButton->SetBorderWidth(2);
        this->ResetPlanButton->SetReliefToRaised();      
        this->ResetPlanButton->SetHighlightThickness(2);
        this->ResetPlanButton->SetBackgroundColor(0.85,0.85,0.85);
        this->ResetPlanButton->SetActiveBackgroundColor(1,1,1);      
        this->ResetPlanButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
        this->ResetPlanButton->Create();
        }
      
      this->Script("pack %s -side top -padx 2 -pady 4", 
                        this->ResetPlanButton->GetWidgetName());
      
      

      break;
    }
  
  this->SetDescription("Plan the needle insertion");

  /*vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  
  if (!mrmlManager || !wizard_widget)
    {
    return;
    }*/

 

  // Create the individual components

  //frame
  if (!this->EntryPointFrame)
    {
    this->EntryPointFrame = vtkKWFrame::New();
    }
  if (!this->EntryPointFrame->IsCreated())
    {
    this->EntryPointFrame->SetParent(parent);
    this->EntryPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->EntryPointFrame->GetWidgetName());

  // label.
  if (!this->EntryPointLabel)
    { 
    this->EntryPointLabel = vtkKWLabel::New();
    }
  if (!this->EntryPointLabel->IsCreated())
    {
    this->EntryPointLabel->SetParent(this->EntryPointFrame);
    this->EntryPointLabel->Create();
    this->EntryPointLabel->SetText("Entry point:   ");
    this->EntryPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    } 
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPointLabel->GetWidgetName());

  // Entry point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->EntryPoint)
    {
    this->EntryPoint =  vtkKWEntrySet::New();   
    }
  if (!this->EntryPoint->IsCreated())
    {
    this->EntryPoint->SetParent(this->EntryPointFrame);
    this->EntryPoint->Create();
    this->EntryPoint->SetBorderWidth(2);
    this->EntryPoint->SetReliefToGroove();
    this->EntryPoint->SetPackHorizontally(1);
    this->EntryPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->EntryPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPoint->GetWidgetName());

  //frame
  if (!this->TargetPointFrame)
    {
    this->TargetPointFrame = vtkKWFrame::New();
    }
  if (!this->TargetPointFrame->IsCreated())
    {
    this->TargetPointFrame->SetParent(parent);
    this->TargetPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TargetPointFrame->GetWidgetName());

  // label
  if (!this->TargetPointLabel)
    { 
    this->TargetPointLabel = vtkKWLabel::New();
    }
  if (!this->TargetPointLabel->IsCreated())
    {
    this->TargetPointLabel->SetParent(this->TargetPointFrame);
    this->TargetPointLabel->Create();
    this->TargetPointLabel->SetText("Target point: ");
    this->TargetPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPointLabel->GetWidgetName());
 
  // Target point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->TargetPoint)
    {
    this->TargetPoint =  vtkKWEntrySet::New();  
    }
  if (!this->TargetPoint->IsCreated())
    {
    this->TargetPoint->SetParent(this->TargetPointFrame);
    this->TargetPoint->Create();
    this->TargetPoint->SetBorderWidth(2);
    this->TargetPoint->SetReliefToGroove();
    this->TargetPoint->SetPackHorizontally(1);
    this->TargetPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->TargetPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPoint->GetWidgetName());


  // insertion angle  
  if (!this->InsertionAngle)
    {
    this->InsertionAngle =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionAngle->IsCreated())
    {
    this->InsertionAngle->SetParent(parent);
    this->InsertionAngle->Create();
    this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngle->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionAngle->SetLabelText("Insertion angle (in degrees):");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionAngle->GetWidgetName());


  // insertion depth  
  if (!this->InsertionDepth)
    {
    this->InsertionDepth =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionDepth->IsCreated())
    {
    this->InsertionDepth->SetParent(parent);
    this->InsertionDepth->Create();
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionDepth->SetLabelText("Insertion depth (in mm):   ");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionDepth->GetWidgetName());

  // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();


  wizard_widget->SetErrorText(
    "Please note that the order of the clicks on image is important.");
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->ResetPlanButton)
    {
    this->ResetPlanButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if(this->InsertionDepth)
    {
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->GetWidget()->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  if(this->InsertionAngle)
    {
    this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngle->GetWidget()->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::RemoveGUIObservers()
{
  if (this->ResetPlanButton)
    {
    this->ResetPlanButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if(this->InsertionDepth)
    {
    this->InsertionDepth->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  if(this->InsertionAngle)
    {
    this->InsertionAngle->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InstallCallbacks()
{
  /*
  // callback on insertion depth
  this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
  this->InsertionDepth->GetWidget()->SetCommand(this, "InsertionDepthEntryCallback");
  this->AddCallbackCommandObserver(this->InsertionDepth->GetWidget(), vtkKWEntry::EntryValueChangedEvent);

  // callback on insertion angle
  this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
  this->InsertionAngle->GetWidget()->SetCommand(this, "InsertionAngleEntryCallback");
  this->AddCallbackCommandObserver(this->InsertionAngle->GetWidget(), vtkKWEntry::EntryValueChangedEvent);
*/

  this->AddGUIObservers();

}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::PopulateControls()
{
    

}
//---------------------------------------------------------------------------
void vtkPerkStationPlanStep::PopulateControlsOnLoadPlanning()
{
    if (!this->GetGUI()->GetMRMLNode())
      return;

    if (!this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
      return;

    double rasEntry[3];
    this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);

    // entry point
    this->EntryPoint->GetWidget(0)->SetValueAsDouble(rasEntry[0]);
    this->EntryPoint->GetWidget(1)->SetValueAsDouble(rasEntry[1]);
    this->EntryPoint->GetWidget(2)->SetValueAsDouble(rasEntry[2]);

    double rasTarget[3];
    this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);

    // target point
    this->TargetPoint->GetWidget(0)->SetValueAsDouble(rasTarget[0]);
    this->TargetPoint->GetWidget(1)->SetValueAsDouble(rasTarget[1]);
    this->TargetPoint->GetWidget(2)->SetValueAsDouble(rasTarget[2]);

    // insertion angle  
    double insAngle = this->GetGUI()->GetMRMLNode()->GetActualPlanInsertionAngle();
    this->InsertionAngle->GetWidget()->SetValueAsDouble(insAngle);
    // insertion depth
    double insDepth = this->GetGUI()->GetMRMLNode()->GetActualPlanInsertionDepth();
    this->InsertionDepth->GetWidget()->SetValueAsDouble(insDepth);

}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step
  if (!wizard_widget ||   wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this)
    {
    return;
    }

  if (!this->GetGUI()->GetMRMLNode() || !this->GetGUI()->GetMRMLNode()->GetPlanningVolumeNode() || strcmp(this->GetGUI()->GetMRMLNode()->GetVolumeInUse(), "Planning")!=0)
    {
    // TO DO: what to do on failure
    return;
    }


  // see if the entry and target have already been acquired
  if(this->EntryTargetAcquired)
    {
    return;
    }
  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  
  vtkRenderWindowInteractor *rwi;
  vtkMatrix4x4 *matrix;

  // planning has to happen on slicer laptop, cannot be done from secondary monitor, so don't listen to clicks in secondary monitor
  if ((s == istyle0)&& (event == vtkCommand::LeftButtonPressEvent))
    {
    ++this->ClickNumber;
    // mouse click happened in the axial slice view
    vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
    rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();    
    matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();

    
    int point[2];
    rwi->GetLastEventPosition(point);
    double inPt[4] = {point[0], point[1], 0, 1};
    double outPt[4];    
    matrix->MultiplyPoint(inPt, outPt); 
    double ras[3] = {outPt[0], outPt[1], outPt[2]};

    // depending on click number, it is either Entry point or target point
    if (this->ClickNumber ==1)
      {
      // entry point specification by user
      this->EntryPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->EntryPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->EntryPoint->GetWidget(2)->SetValueAsDouble(ras[2]);

      // record value in mrml node
      this->GetGUI()->GetMRMLNode()->SetPlanEntryPoint(ras);

      // record value in mrml fiducial list node      
      int index = this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ(ras[0], ras[1], ras[2], false);
      this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText(index, "Entry");
      

      }
    else if (this->ClickNumber == 2)
      {
      this->TargetPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->TargetPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->TargetPoint->GetWidget(2)->SetValueAsDouble(ras[2]);
      
      // record value in the MRML node
      this->GetGUI()->GetMRMLNode()->SetPlanTargetPoint(ras);
    
      // record value in mrml fiducial list node      
      int index = this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ(ras[0], ras[1], ras[2], false);
      this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText(index, "Target");
      
      // calculate insertion angle and insertion depth
      this->CalculatePlanInsertionAngleAndDepth();
      // record those values too in the mrml node
    
      // do an image overlay of a cylinder!!
      this->OverlayNeedleGuide();
      this->GetGUI()->GetSecondaryMonitor()->OverlayNeedleGuide();  
      
      this->ClickNumber = 0;

      this->EntryTargetAcquired = true;
      }


    }
}

//----------------------------------------------------------------------------
bool vtkPerkStationPlanStep::DoubleEqual(double val1, double val2)
{
  bool result = false;
    
  if(fabs(val2-val1) < 0.0001)
      result = true;

  return result;
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InsertionDepthEntryCallback()
{
    if (!this->GetGUI()->GetMRMLNode())
        return;

    // TO DO: check if its a valid input i.e. non-negative  
    double mrmlVal = this->GetGUI()->GetMRMLNode()->GetUserPlanInsertionDepth();
    
    if (strcmpi(this->InsertionDepth->GetWidget()->GetValue(), "")!=0)
      {
      // counter the case when the user enters the same value in entry, which is equal to the initial value set in MRML node
      // in this case above code, won't detect change, but actually user has entered a change
      // for insertion depth, initial value is 0
      if(     ( (mrmlVal == 0) && (this->InsertionDepth->GetWidget()->GetValueAsDouble() == 0))
        || (!this->DoubleEqual(mrmlVal,this->InsertionDepth->GetWidget()->GetValueAsDouble()))
        )
        {
        // set the value in the mrml node
        this->GetGUI()->GetMRMLNode()->SetUserPlanInsertionDepth(this->InsertionDepth->GetWidget()->GetValueAsDouble()); 
        this->GetGUI()->GetMRMLNode()->CalculatePlanInsertionDepthError();  
        }
      }
    
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InsertionAngleEntryCallback()
{
    // TO DO: check if its a valid input i.e. 0-360 
    if (!this->GetGUI()->GetMRMLNode())
        return;

    // TO DO: check if its a valid input i.e. non-negative  
    double mrmlVal = this->GetGUI()->GetMRMLNode()->GetUserPlanInsertionAngle();
    
    if (strcmpi(this->InsertionAngle->GetWidget()->GetValue(), "")!=0)
      {
      // counter the case when the user enters the same value in entry, which is equal to the initial value set in MRML node
      // in this case above code, won't detect change, but actually user has entered a change
      // for insertion angle, initial value is 0
      if(     ( (mrmlVal == 0) && (this->InsertionAngle->GetWidget()->GetValueAsDouble() == 0))
        || (!this->DoubleEqual(mrmlVal,this->InsertionAngle->GetWidget()->GetValueAsDouble()))
        )
        {
        // set the value in the mrml node
        this->GetGUI()->GetMRMLNode()->SetUserPlanInsertionAngle(this->InsertionAngle->GetWidget()->GetValueAsDouble());  
        this->GetGUI()->GetMRMLNode()->CalculatePlanInsertionAngleError();
        }
      }
    
}

//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::OverlayNeedleGuide()
{
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();

  // get the world coordinates
  int point[2];
  double worldCoordinate[4];
  vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(xyToRAS, rasToXY);

  // entry point
  double rasEntry[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
  double inPt[4] = {rasEntry[0], rasEntry[1], rasEntry[2], 1};
  double outPt[4];  
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];

  renderer->SetDisplayPoint(point[0],point[1], 0);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(worldCoordinate);
  this->WCEntryPoint[0] = worldCoordinate[0];
  this->WCEntryPoint[1] = worldCoordinate[1];
  this->WCEntryPoint[2] = worldCoordinate[2];

  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
  inPt[0] = rasTarget[0];
  inPt[1] = rasTarget[1];
  inPt[2] = rasTarget[2];
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];
  renderer->SetDisplayPoint(point[0],point[1], 0);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(worldCoordinate);
  this->WCTargetPoint[0] = worldCoordinate[0];
  this->WCTargetPoint[1] = worldCoordinate[1];
  this->WCTargetPoint[2] = worldCoordinate[2];


  // steps
  // get the cylinder source, create the cylinder, whose height is equal to calculated insertion depth
  // apply transform on the cylinder to world coordinates, using the information of entry and target point
  // i.e. using the insertion angle
  // add it to slice viewer's renderer

  vtkCylinderSource *needleGuide = vtkCylinderSource::New();
  // TO DO: how to relate this to actual depth???
  double halfNeedleLength = sqrt( (this->WCTargetPoint[0]-this->WCEntryPoint[0])*(this->WCTargetPoint[0]-this->WCEntryPoint[0]) + (this->WCTargetPoint[1]-this->WCEntryPoint[1])*(this->WCTargetPoint[1]-this->WCEntryPoint[1]));
  needleGuide->SetHeight(2*halfNeedleLength);
  //needleGuide->SetHeight(0.75 );
  needleGuide->SetRadius( 0.02 );
  //needleGuide->SetCenter((this->XYTargetPoint[0]+this->XYPlanPoint[0])/2, (this->XYTargetPoint[1]+this->XYPlanPoint[1])/2, 0);
  needleGuide->SetResolution( 10 );

  //int *windowSize = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindow()->GetSize();
  // because cylinder is positioned at the window center
  double needleCenter[3];  
  //needleCenter[0] = (this->WCTargetPoint[0]+this->WCEntryPoint[0])/2;// - windowSize[0]/2;
  //needleCenter[1] = (this->WCTargetPoint[1]+this->WCEntryPoint[1])/2;// - windowSize[1]/2;
  needleCenter[0] = this->WCEntryPoint[0];// - windowSize[0]/2;
  needleCenter[1] = this->WCEntryPoint[1];// - windowSize[1]/2;
  //needleGuide->SetCenter(needleCenter[0], needleCenter[1], needleCenter[2]);
  
  // TO DO: transfrom needle mapper using vtkTransformPolyData
  vtkMatrix4x4 *transformMatrix = vtkMatrix4x4::New();
  transformMatrix->Identity();
  //insertion angle is the angle with x-axis of the line which has entry and target as its end points;
  double angle = double(180/vtkMath::Pi()) * atan(double((rasEntry[1] - rasTarget[1])/(rasEntry[0] - rasTarget[0])));
  double insAngleRad = vtkMath::Pi()/2 - double(vtkMath::Pi()/180)*angle;
  transformMatrix->SetElement(0,0, cos(insAngleRad));
  transformMatrix->SetElement(0,1, -sin(insAngleRad));
  transformMatrix->SetElement(0,2, 0);
  transformMatrix->SetElement(0,3, 0);
  transformMatrix->SetElement(0,3, needleCenter[0]);

  transformMatrix->SetElement(1,0, sin(insAngleRad));
  transformMatrix->SetElement(1,1, cos(insAngleRad));
  transformMatrix->SetElement(1,2, 0);
  transformMatrix->SetElement(1,3, 0);
  transformMatrix->SetElement(1,3, needleCenter[1]);

  transformMatrix->SetElement(2,0, 0);
  transformMatrix->SetElement(2,1, 0);
  transformMatrix->SetElement(2,2, 1);
  transformMatrix->SetElement(2,3, 0);

  transformMatrix->SetElement(3,0, 0);
  transformMatrix->SetElement(3,1, 0);
  transformMatrix->SetElement(3,2, 0);
  transformMatrix->SetElement(3,3, 1);

  vtkMatrixToHomogeneousTransform *transform = vtkMatrixToHomogeneousTransform::New();
  transform->SetInput(transformMatrix);
  vtkTransformPolyDataFilter *filter = vtkTransformPolyDataFilter::New();
  filter->SetInputConnection(needleGuide->GetOutputPort()); 
  filter->SetTransform(transform);

  // map
  vtkPolyDataMapper *needleMapper = vtkPolyDataMapper::New();
  needleMapper->SetInputConnection( filter->GetOutputPort() );
  //needleMapper->SetInputConnection(needleGuide->GetOutputPort());

  // after transfrom, set up actor
  this->NeedleActor = vtkActor::New(); 
  this->NeedleActor->SetMapper(needleMapper );  
  this->NeedleActor->GetProperty()->SetOpacity(0.3);
  //needleActor->SetPosition(needleCenter[0], needleCenter[1],0);  
  //needleActor->RotateZ(90-angle);
  
 
  // add to renderer of the Axial slice viewer
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->AddActor(this->NeedleActor);
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender(); 
 
  
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::CalculatePlanInsertionAngleAndDepth()
{
  // insertion depth is the distance between two points (entry and target) in mm
  double rasEntry[3];
  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
  double insDepth = sqrt( (rasTarget[0] - rasEntry[0])*(rasTarget[0] - rasEntry[0]) + (rasTarget[1] - rasEntry[1])*(rasTarget[1] - rasEntry[1]) + (rasTarget[2] - rasEntry[2])*(rasTarget[2] - rasEntry[2]) );
  this->GetGUI()->GetMRMLNode()->SetActualPlanInsertionDepth(insDepth);
  
  
  //insertion angle is the angle with x-axis of the line which has entry and target as its end points;
  double insAngle = double(180/vtkMath::Pi()) * atan(double((rasEntry[1] - rasTarget[1])/(rasEntry[0] - rasTarget[0])));
  this->GetGUI()->GetMRMLNode()->SetActualPlanInsertionAngle(insAngle);
  
  if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    // in clinical mode, the insertion angle reported is the angle with the vertical
    // also if there was any rotation while calibration, that should be taken into account
    // in effect, the angle should be calculated from xy space of the secondary monitor

    vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
    vtkMatrix4x4 *XYToRAS = this->GetGUI()->GetSecondaryMonitor()->GetXYToRAS();
    vtkMatrix4x4::Invert(XYToRAS, rasToXY);

    int point[2];
    double xyEntry[2];
    double xyTarget[2];

    // entry point
    double rasEntry[3];
    this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
    double inPt[4] = {rasEntry[0], rasEntry[1], rasEntry[2], 1};
    double outPt[4];  
    rasToXY->MultiplyPoint(inPt, outPt);
    point[0] = outPt[0];
    point[1] = outPt[1];
    xyEntry[0] = point[0];
    xyEntry[1] = point[1];

    double rasTarget[3];
    this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
    inPt[0] = rasTarget[0];
    inPt[1] = rasTarget[1];
    inPt[2] = rasTarget[2];
    rasToXY->MultiplyPoint(inPt, outPt);
    point[0] = outPt[0];
    point[1] = outPt[1];
    xyTarget[0] = point[0];
    xyTarget[1] = point[1];

    double denom = xyEntry[0]-xyTarget[0];
    double numer = xyEntry[1]-xyTarget[1];

    double insAngleXY = double(180/vtkMath::Pi()) * atan(double(numer/denom));

    if( denom == 0)
      {
      insAngleXY = 0.0;
      }
    else if (numer == 0)
      {
      insAngleXY = 90.0;
      }
    else if (denom > 0 && numer > 0)
      {
      // first quadrant
      // report angle w.r.t vertical
      insAngleXY = 90 - insAngleXY;
      }
    else if (denom < 0 && numer > 0)
      {
      // second quadrant
      // report angle w.r.t vertical
      insAngleXY = 90 + insAngleXY;
      }
    else if (denom < 0 && numer < 0)
      {
      // third quadrant
      // report angle w.r.t vertical
      insAngleXY = 90 - insAngleXY;
      }
    else if (denom > 0 && numer < 0)
      {
      // fourth quadrant
      // report angle w.r.t vertical
      insAngleXY = 90 + insAngleXY;
      }
    this->GetGUI()->GetMRMLNode()->SetActualPlanInsertionAngle(insAngleXY);
    this->InsertionDepth->GetWidget()->SetValueAsDouble(insDepth);
    this->InsertionAngle->GetWidget()->SetValueAsDouble(insAngleXY);
    }
}

//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::Reset()
{
  // reset the overlay needle guide both in sliceviewer and in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayNeedleGuide();

  if (this->GetGUI()->GetSecondaryMonitor()->GetDepthLinesInitialized())
    {
    // although the depth perception lines are actually set and rendered in Insert step
    // we do reset here, because we don't want to have reset button in 'Insert' step
    // thats why we first check if the Depth lines were actually initialized
    // if in case, we do decide to have reset button in 'Insert' step, then we should
    // move the remove code to Reset of Insert itself
    this->GetGUI()->GetSecondaryMonitor()->RemoveDepthPerceptionLines();
    }
  this->RemoveOverlayNeedleGuide();

  // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  double ras[3] = {0.0,0.0,0.0};
  mrmlNode->SetPlanEntryPoint(ras);
  mrmlNode->SetPlanTargetPoint(ras);
  mrmlNode->SetUserPlanInsertionAngle(0.0);
  mrmlNode->SetActualPlanInsertionAngle(0.0);
  mrmlNode->SetUserPlanInsertionDepth(0.0);
  mrmlNode->SetActualPlanInsertionDepth(0.0);
  mrmlNode->CalculatePlanInsertionAngleError();
  mrmlNode->CalculatePlanInsertionDepthError();
  // the fiducial list node
  mrmlNode->GetPlanMRMLFiducialListNode()->RemoveAllFiducials();


  // reset local member variables to defaults
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
  // reset gui controls
  this->ResetControls();
  
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::ResetControls()
{
  if(this->EntryPoint)
    {
    this->EntryPoint->GetWidget(0)->SetValue("");
    this->EntryPoint->GetWidget(1)->SetValue("");
    this->EntryPoint->GetWidget(2)->SetValue("");
    }
  if (this->TargetPoint)
    {
    this->TargetPoint->GetWidget(0)->SetValue("");
    this->TargetPoint->GetWidget(1)->SetValue("");
    this->TargetPoint->GetWidget(2)->SetValue("");
    }

  if (this->InsertionAngle)
    {
    this->InsertionAngle->GetWidget()->SetValue("");
    }
  
  if (this->InsertionDepth)
    {
    this->InsertionDepth->GetWidget()->SetValue("");
    }
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::RemoveOverlayNeedleGuide()
{
  // should remove the overlay needle guide
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->GetActors();
  if (collection->IsItemPresent(this->NeedleActor))
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->RemoveActor(this->NeedleActor);
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender();     
    }
}

//-----------------------------------------------------------------------------
void vtkPerkStationPlanStep::LoadPlanning(istream &file)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

  char currentLine[256];  
  char* attName = "";
  char* attValue = "";
  char* pdest;
  int nCharCount = 0;
  unsigned int indexEndOfAttribute = 0;
  unsigned int indexStartOfValue = 0;
  unsigned int indexEndOfValue = 0;

  int paramSetCount = 0;
  while(!file.eof())
    {
    // first get each line,
    // then parse each line on basis of attName, and attValue
    // this can be done as delimiters '='[]' is used to separate out name from value
    file.getline(&currentLine[0], 256, '\n');   
    nCharCount = strlen(currentLine);
    indexEndOfAttribute = strcspn(currentLine,"=");
    if(indexEndOfAttribute >0)
      {
      attName = new char[indexEndOfAttribute+1];
      strncpy(attName, currentLine,indexEndOfAttribute);
      attName[indexEndOfAttribute] = '\0';
      pdest = strchr(currentLine, '"');   
      indexStartOfValue = (int)(pdest - currentLine + 1);
      pdest = strrchr(currentLine, '"');
      indexEndOfValue = (int)(pdest - currentLine + 1);
      attValue = new char[indexEndOfValue-indexStartOfValue+1];
      strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
      attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

      // at this point, we have line separated into, attributeName, and attributeValue
      // now we need to do string matching on attributeName, and further parse attributeValue as it may have more than one value
      if (!strcmp(attName, " PlanInsertionAngle"))
        {
        std::stringstream ss;
        ss << attValue;
        double val;
        ss >> val;
        mrmlNode->SetActualPlanInsertionAngle(val);
        paramSetCount++;
        }     
      else if (!strcmp(attName, " PlanInsertionDepth"))
        {
        std::stringstream ss;
        ss << attValue;
        double val;
        ss >> val;
        mrmlNode->SetActualPlanInsertionDepth(val);     
        paramSetCount++;
        }     
      else if (!strcmp(attName, " PlanEntryPoint"))
        {
        // read data into a temporary vector
        std::stringstream ss;
        ss << attValue;
        double d;
        std::vector<double> tmpVec;
        while (ss >> d)
          {
          tmpVec.push_back(d);
          }
        if (tmpVec.size()==3)
          {
          double point[3];
          for (unsigned int i = 0; i < tmpVec.size(); i++)
            point[i] = tmpVec[i];
          mrmlNode->SetPlanEntryPoint(point[0], point[1], point[2]);
          paramSetCount++;
          }
        else
          {
          // error in file?
          }     
        }
      else if (!strcmp(attName, " PlanTargetPoint"))
        {
        // read data into a temporary vector
        std::stringstream ss;
        ss << attValue;
        double d;
        std::vector<double> tmpVec;
        while (ss >> d)
          {
          tmpVec.push_back(d);
          }
        if (tmpVec.size()==3)
          {
          double point[3];
          for (unsigned int i = 0; i < tmpVec.size(); i++)
            point[i] = tmpVec[i];
          mrmlNode->SetPlanTargetPoint(point[0], point[1], point[2]);
          paramSetCount++;
          }
        else
          {
          // error in file?
          }     
        }
      
      }// end if testing for it is a valid attName

    } // end while going through the file
  
  if (paramSetCount == 4)
    {
    // all params correctly read from file
    
    // reflect the values of params in GUI controls
    this->PopulateControlsOnLoadPlanning();
    // set any state variables required to be set

    // overlay needle guide in slicer gui & secondary monitor
    this->OverlayNeedleGuide();
    this->GetGUI()->GetSecondaryMonitor()->OverlayNeedleGuide();  
    
    this->ClickNumber = 0;

    this->EntryTargetAcquired = true;
    }
  else
    {
    // error reading file, not all values set
    int error = -1;
    }
}

//-----------------------------------------------------------------------------
void vtkPerkStationPlanStep::SavePlanning(ostream& of)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

  
  // entry point
  of << " PlanEntryPoint=\"" ;
  double entryPoint[3];
  mrmlNode->GetPlanEntryPoint(entryPoint);
  for(int i = 0; i < 3; i++)
      of << entryPoint[i] << " ";
  of << "\" \n";
     
  // target point
  of << " PlanTargetPoint=\""; 
  double targetPoint[3];
  mrmlNode->GetPlanTargetPoint(targetPoint);
  for(int i = 0; i < 3; i++)
      of << targetPoint[i] << " ";
  of << "\" \n";

  // insertion depth
  of << " PlanInsertionDepth=\"";
  double depth = mrmlNode->GetActualPlanInsertionDepth();  
  of << depth << " ";
  of << "\" \n";

  
  // insertion angle
  of << " PlanInsertionAngle=\"";
  double angle = mrmlNode->GetActualPlanInsertionAngle();  
  of << angle << " ";
  of << "\" \n";

  

  

}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationPlanStep *self = reinterpret_cast<vtkPerkStationPlanStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;
  
  // reset plan button
  if (this->ResetPlanButton && this->ResetPlanButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->Reset();
    }

  // insertion depth
  if (this->InsertionDepth && this->InsertionDepth->GetWidget()== vtkKWEntry::SafeDownCast(caller) && (event == vtkKWEntry::EntryValueChangedEvent))
    {
    this->InsertionDepthEntryCallback();    
    }

    // insertion angle
  if (this->InsertionAngle && this->InsertionAngle->GetWidget()== vtkKWEntry::SafeDownCast(caller) && (event == vtkKWEntry::EntryValueChangedEvent))
    {
    this->InsertionAngleEntryCallback();    
    }

  this->ProcessingCallback = false;
}
