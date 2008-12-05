#include "vtkProstateNavConfigurationStep.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWCheckButton.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavConfigurationStep);
vtkCxxRevisionMacro(vtkProstateNavConfigurationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavConfigurationStep::vtkProstateNavConfigurationStep()
{
  this->SetName("1/5. Configuration");
  this->SetDescription("Perform system configuration.");

  this->ConfigNTFrame = NULL;
  this->ConnectNTFrame = NULL;
  this->LoadConfigButtonNT = NULL;
  this->ConfigFileEntryNT = NULL;
  this->ConnectCheckButtonNT = NULL;

  this->RobotAddressEntry   = NULL;
  this->RobotPortEntry      = NULL;
  this->ScannerAddressEntry = NULL;
  this->ScannerPortEntry    = NULL;
  this->StartButton         = NULL;

}

//----------------------------------------------------------------------------
vtkProstateNavConfigurationStep::~vtkProstateNavConfigurationStep()
{
  if (this->LoadConfigButtonNT)
    {
    this->LoadConfigButtonNT->GetWidget()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->LoadConfigButtonNT->SetParent(NULL );
    this->LoadConfigButtonNT->Delete();
    }
  if (this->ConfigFileEntryNT)
    {
    this->ConfigFileEntryNT->SetParent(NULL );
    this->ConfigFileEntryNT->Delete();
    }
  if (this->ConnectNTFrame)
    {
    this->ConnectNTFrame->SetParent(NULL);
    this->ConnectNTFrame->Delete();
    }
  if (this->ConfigNTFrame)
    {
    this->ConfigNTFrame->SetParent(NULL);
    this->ConfigNTFrame->Delete();
    }

  if (this->ConnectCheckButtonNT)
    {
    this->ConnectCheckButtonNT->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ConnectCheckButtonNT->SetParent(NULL);
    this->ConnectCheckButtonNT->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavConfigurationStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  if (!this->RobotAddressEntry)
    {
    vtkKWFrame *robotFrame = vtkKWFrame::New();
    robotFrame->SetParent ( parent );
    robotFrame->Create ( );
    this->Script ( "pack %s -side top -fill x",
                  robotFrame->GetWidgetName());

    vtkKWLabel *robotLabel1 = vtkKWLabel::New();
    robotLabel1->SetParent(robotFrame);
    robotLabel1->Create();
    robotLabel1->SetWidth(15);
    robotLabel1->SetText("Robot Addr: ");

    this->RobotAddressEntry = vtkKWEntry::New();
    this->RobotAddressEntry->SetParent(robotFrame);
    this->RobotAddressEntry->Create();
    this->RobotAddressEntry->SetWidth(20);

    vtkKWLabel *robotLabel2 = vtkKWLabel::New();
    robotLabel2->SetParent(robotFrame);
    robotLabel2->Create();
    robotLabel2->SetWidth(3);
    robotLabel2->SetText(":");

    this->RobotPortEntry = vtkKWEntry::New();
    this->RobotPortEntry->SetParent(robotFrame);
    this->RobotPortEntry->Create();
    this->RobotPortEntry->SetWidth(10);

    this->Script("pack %s %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
                 robotLabel1->GetWidgetName(), this->RobotAddressEntry->GetWidgetName(),
                 robotLabel2->GetWidgetName(), this->RobotPortEntry->GetWidgetName());

    robotFrame->Delete();
    robotLabel1->Delete();
    robotLabel2->Delete();
    }

  if (!this->ScannerAddressEntry)
    {
    vtkKWFrame *scannerFrame = vtkKWFrame::New();
    scannerFrame->SetParent ( parent );
    scannerFrame->Create ( );
    this->Script ( "pack %s -side top -fill x",  
                   scannerFrame->GetWidgetName());

    vtkKWLabel *scannerLabel1 = vtkKWLabel::New();
    scannerLabel1->SetParent(scannerFrame);
    scannerLabel1->Create();
    scannerLabel1->SetWidth(15);
    scannerLabel1->SetText("Scanner Addr: ");

    this->ScannerAddressEntry = vtkKWEntry::New();
    this->ScannerAddressEntry->SetParent(scannerFrame);
    this->ScannerAddressEntry->Create();
    this->ScannerAddressEntry->SetWidth(20);

    vtkKWLabel *scannerLabel2 = vtkKWLabel::New();
    scannerLabel2->SetParent(scannerFrame);
    scannerLabel2->Create();
    scannerLabel2->SetWidth(3);
    scannerLabel2->SetText(":");

    this->ScannerPortEntry = vtkKWEntry::New();
    this->ScannerPortEntry->SetParent(scannerFrame);
    this->ScannerPortEntry->Create();
    this->ScannerPortEntry->SetWidth(10);

    this->Script("pack %s %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
                scannerLabel1->GetWidgetName(), this->ScannerAddressEntry->GetWidgetName(),
                scannerLabel2->GetWidgetName(), this->ScannerPortEntry->GetWidgetName());
    
    scannerFrame->Delete();
    scannerLabel1->Delete();
    scannerLabel2->Delete();
    }
  

  /*
  // Create the frame
  if (!this->ConfigNTFrame)
    {
    this->ConfigNTFrame = vtkKWFrame::New();
    this->ConfigNTFrame->SetParent ( parent );
    this->ConfigNTFrame->Create ( );
    }
  
  if (!this->ConnectNTFrame)
    {
    this->ConnectNTFrame = vtkKWFrame::New();
    this->ConnectNTFrame->SetParent ( parent );
    this->ConnectNTFrame->Create ( );
    }

  if (!this->LoadConfigButtonNT)
    {
    this->LoadConfigButtonNT = vtkKWLoadSaveButtonWithLabel::New();
    this->LoadConfigButtonNT->SetParent(this->ConfigNTFrame);
    this->LoadConfigButtonNT->Create();
    //this->LoadConfigButtonNT->SetWidth(30);
    this->LoadConfigButtonNT->SetLabelText("Config. File:");
    this->LoadConfigButtonNT->GetWidget()->SetText ("Browse Config File");
    this->LoadConfigButtonNT->GetWidget()->GetLoadSaveDialog()->SetFileTypes(
      "{ {ProstateNav} {*.xml} }");
    this->LoadConfigButtonNT->GetWidget()->GetLoadSaveDialog()
      ->RetrieveLastPathFromRegistry("OpenPath");

    this->LoadConfigButtonNT->GetWidget()
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  // Create the file entry and load button
  if (!this->ConfigFileEntryNT)
    {
    this->ConfigFileEntryNT = vtkKWEntry::New();
    this->ConfigFileEntryNT->SetParent(this->ConfigNTFrame);
    this->ConfigFileEntryNT->Create();
    this->ConfigFileEntryNT->SetWidth(50);
    this->ConfigFileEntryNT->SetValue ("");
    
    }

  // The connnect button 
  if (!this->ConnectCheckButtonNT)
    {
    this->ConnectCheckButtonNT = vtkKWCheckButton::New();
    this->ConnectCheckButtonNT->SetParent(this->ConnectNTFrame);
    this->ConnectCheckButtonNT->Create();
    this->ConnectCheckButtonNT->SelectedStateOff();
    this->ConnectCheckButtonNT->SetText("Connect");

    this->ConnectCheckButtonNT
      ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

    }


  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->ConfigNTFrame->GetWidgetName());
    
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->ConnectNTFrame->GetWidgetName());


  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->LoadConfigButtonNT->GetWidgetName());

  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
               this->ConnectCheckButtonNT->GetWidgetName());
  */

}

//----------------------------------------------------------------------------
void vtkProstateNavConfigurationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkProstateNavConfigurationStep::ProcessGUIEvents( vtkObject *caller,
                                         unsigned long event, void *callData )
{

  if (this->LoadConfigButtonNT->GetWidget() == vtkKWLoadSaveButton::SafeDownCast(caller) 
           && event == vtkKWPushButton::InvokedEvent )
    {
    const char* filename = this->LoadConfigButtonNT->GetWidget()->GetFileName();
    if (filename)
      {
      const vtksys_stl::string fname(filename);
      this->ConfigFileEntryNT->SetValue(fname.c_str());
      }
    else
      {
      this->ConfigFileEntryNT->SetValue("");
      }
    }

  else if (this->ConnectCheckButtonNT == vtkKWCheckButton::SafeDownCast(caller) 
           && event == vtkKWCheckButton::SelectedStateChangedEvent )
    {

    if (this->ConnectCheckButtonNT->GetSelectedState() && this->Logic)
      {
      // Activate NaviTrack Stream
      const char* filename = this->LoadConfigButtonNT->GetWidget()->GetFileName();
      this->Logic->ConnectTracker(filename);
      }
    else
      {
      // Deactivate NaviTrack Stream
      this->Logic->DisconnectTracker();
      }
    }

}



