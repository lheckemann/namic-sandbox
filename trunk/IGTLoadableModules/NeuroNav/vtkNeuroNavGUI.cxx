#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkNeuroNavGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWRenderWidget.h"
#include "vtkKWWidget.h"
#include "vtkKWMenuButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMenu.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWEvent.h"


#include "vtkKWTkUtilities.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCornerAnnotation.h"

#include "vtkIGTDataStream.h"

#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"


//---------------------------------------------------------------------------
// vtkStandardNewMacro (vtkNeuroNavGUI );
vtkCxxRevisionMacro ( vtkNeuroNavGUI, "$Revision: 1.0 $");


/* go redsox */


vtkNeuroNavGUI* vtkNeuroNavGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = 
    vtkObjectFactory::CreateInstance("vtkNeuroNavGUI");
  if (ret)
    {
    return (vtkNeuroNavGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkNeuroNavGUI;
}



//---------------------------------------------------------------------------
vtkNeuroNavGUI::vtkNeuroNavGUI ( )
{
    this->Logic = NULL;

    this->LocatorCheckButton = NULL;
    this->HandleCheckButton = NULL;
    this->GuideCheckButton = NULL;

    this->LocatorModeCheckButton = NULL;
    this->UserModeCheckButton = NULL;

    this->RedSliceMenu = NULL;
    this->YellowSliceMenu = NULL;
    this->GreenSliceMenu = NULL;


    this->PatCoordinatesEntry = NULL;
    this->SlicerCoordinatesEntry = NULL;
    this->GetPatCoordinatesPushButton = NULL;
    this->AddPointPairPushButton = NULL;

    this->PointPairMultiColumnList = NULL;

    // this->LoadPointPairPushButton = NULL;
    // this->SavePointPairPushButton = NULL;
    this->DeletePointPairPushButton = NULL;
    this->DeleteAllPointPairPushButton = NULL;
    this->RegisterPushButton = NULL;
    this->ResetPushButton = NULL;

    this->LocatorMatrix = NULL;

    this->LocatorModelDisplayNode = NULL;

    this->DataManager = vtkIGTDataManager::New();
    this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();

    this->DataCallbackCommand = vtkCallbackCommand::New();
    this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
    this->DataCallbackCommand->SetCallback(vtkNeuroNavGUI::DataCallback);

    this->Logic0 = NULL; 
    this->Logic1 = NULL; 
    this->Logic2 = NULL; 
    this->SliceNode0 = NULL; 
    this->SliceNode1 = NULL; 
    this->SliceNode2 = NULL; 
    this->Control0 = NULL; 
    this->Control1 = NULL; 
    this->Control2 = NULL; 

    this->NeedOrientationUpdate0 = 0;
    this->NeedOrientationUpdate1 = 0;
    this->NeedOrientationUpdate2 = 0;
}


//---------------------------------------------------------------------------
vtkNeuroNavGUI::~vtkNeuroNavGUI ( )
{
    if (this->DataManager)
    {
        // If we don't set the scence to NULL for DataManager,
        // Slicer will report a lot leak when it is closed.
        this->DataManager->SetMRMLScene(NULL);
        this->DataManager->Delete();
    }
    if (this->Pat2ImgReg)
    {
        this->Pat2ImgReg->Delete();
    }
    if (this->DataCallbackCommand)
    {
        this->DataCallbackCommand->Delete();
    }

    this->RemoveGUIObservers();


    if (this->LocatorCheckButton)
    {
        this->LocatorCheckButton->SetParent(NULL );
        this->LocatorCheckButton->Delete ( );
    }
    if (this->HandleCheckButton)
    {
        this->HandleCheckButton->SetParent(NULL );
        this->HandleCheckButton->Delete ( );
    }
    if (this->GuideCheckButton)
    {
        this->GuideCheckButton->SetParent(NULL );
        this->GuideCheckButton->Delete ( );
    }

    if (this->LocatorModeCheckButton)
    {
        this->LocatorModeCheckButton->SetParent(NULL );
        this->LocatorModeCheckButton->Delete ( );
    }
    if (this->UserModeCheckButton)
    {
        this->UserModeCheckButton->SetParent(NULL );
        this->UserModeCheckButton->Delete ( );
    }

    if (this->RedSliceMenu)
    {
        this->RedSliceMenu->SetParent(NULL );
        this->RedSliceMenu->Delete ( );
    }
    if (this->YellowSliceMenu)
    {
        this->YellowSliceMenu->SetParent(NULL );
        this->YellowSliceMenu->Delete ( );
    }
    if (this->GreenSliceMenu)
    {
        this->GreenSliceMenu->SetParent(NULL );
        this->GreenSliceMenu->Delete ( );
    }


    if (this->PatCoordinatesEntry)
    {
        this->PatCoordinatesEntry->SetParent(NULL );
        this->PatCoordinatesEntry->Delete ( );
    }
    if (this->SlicerCoordinatesEntry)
    {
        this->SlicerCoordinatesEntry->SetParent(NULL );
        this->SlicerCoordinatesEntry->Delete ( );
    }
    if (this->GetPatCoordinatesPushButton)
    {
        this->GetPatCoordinatesPushButton->SetParent(NULL );
        this->GetPatCoordinatesPushButton->Delete ( );
    }
    if (this->AddPointPairPushButton)
    {
        this->AddPointPairPushButton->SetParent(NULL );
        this->AddPointPairPushButton->Delete ( );
    }

    if (this->PointPairMultiColumnList)
    {
        this->PointPairMultiColumnList->SetParent(NULL );
        this->PointPairMultiColumnList->Delete ( );
    }

    /*
    if (this->LoadPointPairPushButton)
    {
        this->LoadPointPairPushButton->SetParent(NULL );
        this->LoadPointPairPushButton->Delete ( );
    }
    if (this->SavePointPairPushButton)
    {
        this->SavePointPairPushButton->SetParent(NULL );
        this->SavePointPairPushButton->Delete ( );
    }
    */
    if (this->DeletePointPairPushButton)
    {
        this->DeletePointPairPushButton->SetParent(NULL );
        this->DeletePointPairPushButton->Delete ( );
    }
    if (this->DeleteAllPointPairPushButton)
    {
        this->DeleteAllPointPairPushButton->SetParent(NULL );
        this->DeleteAllPointPairPushButton->Delete ( );
    }
    if (this->RegisterPushButton)
    {
        this->RegisterPushButton->SetParent(NULL );
        this->RegisterPushButton->Delete ( );
    }
    if (this->ResetPushButton)
    {
        this->ResetPushButton->SetParent(NULL );
        this->ResetPushButton->Delete ( );
    }

    this->SetModuleLogic ( NULL );
}



//---------------------------------------------------------------------------
void vtkNeuroNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "NeuroNavGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";
    // print widgets?
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::RemoveGUIObservers ( )
{
    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

    appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

    if (this->GetPatCoordinatesPushButton)
    {
        this->GetPatCoordinatesPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->AddPointPairPushButton)
    {
        this->AddPointPairPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->DeletePointPairPushButton)
    {
        this->DeletePointPairPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->DeleteAllPointPairPushButton)
    {
        this->DeleteAllPointPairPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegisterPushButton)
    {
        this->RegisterPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->ResetPushButton)
    {
        this->ResetPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->LocatorCheckButton)
    {
        this->LocatorCheckButton->RemoveObservers ( vtkKWCheckButton::SelectedStateChangedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->LocatorModeCheckButton)
    {
        this->LocatorModeCheckButton->RemoveObservers ( vtkKWCheckButton::SelectedStateChangedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->UserModeCheckButton)
    {
        this->UserModeCheckButton->RemoveObservers ( vtkKWCheckButton::SelectedStateChangedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::AddGUIObservers ( )
{
    this->RemoveGUIObservers();

    // make a user interactor style to process our events
    // look at the InteractorStyle to get our events

    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

    appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);


    // Fill in
    // observer load volume button
    this->GetPatCoordinatesPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->AddPointPairPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->DeletePointPairPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->DeleteAllPointPairPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegisterPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ResetPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

    this->LocatorCheckButton->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->LocatorModeCheckButton->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->UserModeCheckButton->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
}



void vtkNeuroNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
    vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
    vtkSlicerInteractorStyle *istyle1 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
    vtkSlicerInteractorStyle *istyle2 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());

    
    //
    //
    // So easy, a caveman can do it!
    //
    //



    vtkCornerAnnotation *anno = NULL;
    if (style == istyle0)
    {
        anno = appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
    else if (style == istyle1)
    {
        anno = appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
    else if (style == istyle2)
    {
        anno = appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }

    if (anno)
    {
        const char *rasText = anno->GetText(1);
        if ( rasText != NULL )
          {
          std::string ras = std::string(rasText);

          // remove "R:," "A:," and "S:" from the string
          int loc = ras.find("R:", 0);
          if ( loc != std::string::npos ) 
            {
            ras = ras.replace(loc, 2, "");
            }
          loc = ras.find("A:", 0);
          if ( loc != std::string::npos ) 
            {
            ras = ras.replace(loc, 2, "");
            }
          loc = ras.find("S:", 0);
          if ( loc != std::string::npos ) 
            {
            ras = ras.replace(loc, 2, "");
            }

          // remove "\n" from the string
          int found = ras.find("\n", 0);
          while ( found != std::string::npos )
            {
            ras = ras.replace(found, 1, " ");
            found = ras.find("\n", 0);
            }

          this->SlicerCoordinatesEntry->GetWidget()->SetValue(ras.c_str());
          }
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessGUIEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    const char *eventName = vtkCommand::GetStringFromEventId(event);
    if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
        vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
        HandleMouseEvent(style);
    }
    else
    {

        if (this->GetPatCoordinatesPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
            float position[3];
            position[0] = position[1] = position[2] = 0.0;
            char value[50];
            if (this->LocatorMatrix)
            {
                for (int j = 0; j < 3; j++) 
                {
                    position[j] = this->LocatorMatrix->GetElement(j,0);
                    // position[j] = j / 1.0;
                }
            }
            sprintf(value, "%6.2f  %6.2f  %6.2f", position[0], position[1], position[2]);
            this->PatCoordinatesEntry->GetWidget()->SetValue(value);
        }
        else if (this->AddPointPairPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
            int scSize = 0;
            int pcSize = 0;
            const char *pc = this->PatCoordinatesEntry->GetWidget()->GetValue();
            const char *sc = this->SlicerCoordinatesEntry->GetWidget()->GetValue();

            if (pc) 
            {
                const vtksys_stl::string pcCor(pc);
                pcSize = pcCor.size();
            }
            if (sc) 
            {
                const vtksys_stl::string scCor(sc);
                scSize = scCor.size();
            }

            if (pcSize < 5 || scSize < 5)
            {
                vtkSlicerApplication::GetInstance()->ErrorMessage("Patient or Slicer coordinates are invalid."); 
            }
            else 
            {
                int row = this->PointPairMultiColumnList->GetWidget()->GetNumberOfRows();
                this->PointPairMultiColumnList->GetWidget()->AddRow();
                this->PointPairMultiColumnList->GetWidget()->SetCellText(row, 0, pc);
                this->PointPairMultiColumnList->GetWidget()->SetCellText(row, 1, sc);
            }
        }
        else if (this->DeletePointPairPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
            int numOfRows = this->PointPairMultiColumnList->GetWidget()->GetNumberOfSelectedRows();
            if (numOfRows == 1)
            {
                int index[2];
                this->PointPairMultiColumnList->GetWidget()->GetSelectedRows(index);
                this->PointPairMultiColumnList->GetWidget()->DeleteRow(index[0]);
            }
        }
        else if (this->DeleteAllPointPairPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
            this->PointPairMultiColumnList->GetWidget()->DeleteAllRows();
        }
        else if (this->RegisterPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
            int row = this->PointPairMultiColumnList->GetWidget()->GetNumberOfRows();
            if (row < 2)
            {
                vtkSlicerApplication::GetInstance()->ErrorMessage("At least 2 pairs of landmarks are needed for patient to image registration.");
            }
            else
            {
                this->Pat2ImgReg->SetNumberOfPoints(row);
                float pc1 = 0.0, pc2 = 0.0, pc3 = 0.0, sc1 = 0.0, sc2 = 0.0, sc3 = 0.0;
                for (int r = 0; r < row; r++)
                {
                    for (int c = 0; c < 2; c++)
                    {
                        const char *val = this->PointPairMultiColumnList->GetWidget()->GetCellText(r, c);
                        if (c == 0)
                        {
                            sscanf(val, "%f %f %f", &pc1, &pc2, &pc3);
                        }
                        else
                        {
                            sscanf(val, "%f %f %f", &sc1, &sc2, &sc3);
                        }
                    }
                    this->Pat2ImgReg->AddPoint(r, sc1, sc2, sc3, pc1, pc2, pc3);
                }

                int error = this->Pat2ImgReg->DoRegistration();
                if (error)
                {
                    vtkSlicerApplication::GetInstance()->ErrorMessage("Error registration between patient and image land marks.");
                    return;
                }
            }
        }
        else if (this->ResetPushButton == vtkKWPushButton::SafeDownCast(caller) 
                && event == vtkKWPushButton::InvokedEvent)
        {
        }
        else if (this->LocatorCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
                && event == vtkKWCheckButton::SelectedStateChangedEvent )
        {
            int checked = this->LocatorCheckButton->GetSelectedState(); 

            // vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->LocatorModelID.c_str())); 
            vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLModelNode1")); 
            if (model != NULL)
            {
                vtkMRMLModelDisplayNode *disp = model->GetModelDisplayNode();

                vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
                vtkSlicerColor *color = app->GetSlicerTheme()->GetSlicerColors ( );
                disp->SetColor(color->SliceGUIGreen);
                disp->SetVisibility(checked);
            }

        }
        else if (this->LocatorModeCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
                && event == vtkKWCheckButton::SelectedStateChangedEvent )
        {
            int checked = this->LocatorModeCheckButton->GetSelectedState(); 
            std::string val("Locator");

            if (checked)
            {
                this->UserModeCheckButton->SelectedStateOff();
            }
            else
            {
                this->UserModeCheckButton->SelectedStateOn();
                this->SliceNode0->SetOrientationToAxial();
                this->SliceNode1->SetOrientationToSagittal();
                this->SliceNode2->SetOrientationToCoronal();
                this->NeedOrientationUpdate0 = 0;
                this->NeedOrientationUpdate1 = 0;
                this->NeedOrientationUpdate2 = 0;

                val = "User";
            }
            this->RedSliceMenu->SetValue(val.c_str());
            this->YellowSliceMenu->SetValue(val.c_str());
            this->GreenSliceMenu->SetValue(val.c_str());
        }
        else if (this->UserModeCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
                && event == vtkKWCheckButton::SelectedStateChangedEvent )
        {
            int checked = this->UserModeCheckButton->GetSelectedState(); 
            std::string val("User");

            if (checked)
            {
                this->LocatorModeCheckButton->SelectedStateOff();
                this->SliceNode0->SetOrientationToAxial();
                this->SliceNode1->SetOrientationToSagittal();
                this->SliceNode2->SetOrientationToCoronal();

                this->NeedOrientationUpdate0 = 0;
                this->NeedOrientationUpdate1 = 0;
                this->NeedOrientationUpdate2 = 0;
            }
            else
            {
                this->LocatorModeCheckButton->SelectedStateOn();
                val = "Locator";
            }
            this->RedSliceMenu->SetValue(val.c_str());
            this->YellowSliceMenu->SetValue(val.c_str());
            this->GreenSliceMenu->SetValue(val.c_str());
        }
    }
} 



void vtkNeuroNavGUI::Init()
{
    this->DataManager->SetMRMLScene(this->GetMRMLScene());
    this->LocatorModelID = std::string(this->DataManager->RegisterStream(0));
}



void vtkNeuroNavGUI::DataCallback(vtkObject *caller, 
            unsigned long eid, void *clientData, void *callData)
{
    vtkNeuroNavGUI *self = reinterpret_cast<vtkNeuroNavGUI *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkNeuroNavGUI DataCallback");

    self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessLogicEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    // Fill in
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessMRMLEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    // Fill in
}



//---------------------------------------------------------------------------
void vtkNeuroNavGUI::Enter ( )
{
    // Fill in
    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

    this->Logic0 = appGUI->GetMainSliceGUI0()->GetLogic();
    this->Logic1 = appGUI->GetMainSliceGUI1()->GetLogic();
    this->Logic2 = appGUI->GetMainSliceGUI2()->GetLogic();
    this->SliceNode0 = appGUI->GetMainSliceGUI0()->GetLogic()->GetSliceNode();
    this->SliceNode1 = appGUI->GetMainSliceGUI1()->GetLogic()->GetSliceNode();
    this->SliceNode2 = appGUI->GetMainSliceGUI2()->GetLogic()->GetSliceNode();
    this->Control0 = appGUI->GetMainSliceGUI0()->GetSliceController();
    this->Control1 = appGUI->GetMainSliceGUI1()->GetSliceController();
    this->Control2 = appGUI->GetMainSliceGUI2()->GetSliceController();

}

//---------------------------------------------------------------------------
void vtkNeuroNavGUI::Exit ( )
{
    // Fill in
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::BuildGUI ( )
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    // Define your help text here.
    const char *help = "The **NeuroNav Module** connects Slicer to a IGSTK-supported device that feeds a real-time stream of coordinates from a tracking device. This is useful in the operating room to allow the surgeons to navigate the preloaded volume(s) by pointing to a location with a probe that is tracked."; 

    // ---
    // MODULE GUI FRAME 
    // create a page
    this->UIPanel->AddPage ( "NeuroNav", "NeuroNav", NULL );

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "NeuroNav" );


    // ----------------------------------------------------------------
    // HELP FRAME            
    // ----------------------------------------------------------------
    vtkSlicerModuleCollapsibleFrame *NeuroNavHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    NeuroNavHelpFrame->SetParent ( page );
    NeuroNavHelpFrame->Create ( );
    NeuroNavHelpFrame->CollapseFrame ( );
    NeuroNavHelpFrame->SetLabelText ("Help");
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            NeuroNavHelpFrame->GetWidgetName(), page->GetWidgetName());

    // configure the parent classes help text widget
    this->HelpText->SetParent ( NeuroNavHelpFrame->GetFrame() );
    this->HelpText->Create ( );
    this->HelpText->SetHorizontalScrollbarVisibility ( 0 );
    this->HelpText->SetVerticalScrollbarVisibility ( 1 );
    this->HelpText->GetWidget()->SetText ( help );
    this->HelpText->GetWidget()->SetReliefToFlat ( );
    this->HelpText->GetWidget()->SetWrapToWord ( );
    this->HelpText->GetWidget()->ReadOnlyOn ( );
    this->HelpText->GetWidget()->QuickFormattingOn ( );
    this->HelpText->GetWidget()->SetBalloonHelpString ( "" );
    app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
            this->HelpText->GetWidgetName ( ) );

    NeuroNavHelpFrame->Delete();

    // BuildGUIForDeviceFrame ();
    BuildGUIForRegistrationFrame ();
    BuildGUIForTrackingFrame ();
    // BuildGUIForHandPieceFrame ();
}


void vtkNeuroNavGUI::BuildGUIForRegistrationFrame ()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "NeuroNav" );

    // ----------------------------------------------------------------
    // REGISTRATION FRAME            
    // ----------------------------------------------------------------
    vtkSlicerModuleCollapsibleFrame *regFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    regFrame->SetParent ( page );
    regFrame->Create ( );
    regFrame->SetLabelText ("Registration");
    regFrame->CollapseFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            regFrame->GetWidgetName(), page->GetWidgetName());


    // add a point pair 
    vtkKWFrameWithLabel *addFrame = vtkKWFrameWithLabel::New();
    addFrame->SetParent ( regFrame->GetFrame() );
    addFrame->Create ( );
    addFrame->SetLabelText ("Add a point pair");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                  addFrame->GetWidgetName());

    vtkKWFrame *patFrame = vtkKWFrame::New();
    patFrame->SetParent ( addFrame->GetFrame() );
    patFrame->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  patFrame->GetWidgetName());

    vtkKWFrame *okFrame = vtkKWFrame::New();
    okFrame->SetParent ( addFrame->GetFrame() );
    okFrame->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  okFrame->GetWidgetName());

    this->PatCoordinatesEntry = vtkKWEntryWithLabel::New();
    this->PatCoordinatesEntry->SetParent(patFrame);
    this->PatCoordinatesEntry->Create();
    this->PatCoordinatesEntry->SetWidth(30);
    this->PatCoordinatesEntry->SetLabelWidth(16);
    this->PatCoordinatesEntry->SetLabelText("Patient Coordinates:");
    this->PatCoordinatesEntry->GetWidget()->SetValue ( "" );

    this->GetPatCoordinatesPushButton = vtkKWPushButton::New();
    this->GetPatCoordinatesPushButton->SetParent(patFrame);
    this->GetPatCoordinatesPushButton->Create();
    this->GetPatCoordinatesPushButton->SetText("Get");
    this->GetPatCoordinatesPushButton->SetWidth ( 6 );

    this->Script(
      "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2", 
      this->PatCoordinatesEntry->GetWidgetName(),
      this->GetPatCoordinatesPushButton->GetWidgetName());

    this->SlicerCoordinatesEntry = vtkKWEntryWithLabel::New();
    this->SlicerCoordinatesEntry->SetParent(okFrame);
    this->SlicerCoordinatesEntry->Create();
    this->SlicerCoordinatesEntry->SetWidth(30);
    this->SlicerCoordinatesEntry->SetLabelWidth(16);
    this->SlicerCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->SlicerCoordinatesEntry->GetWidget()->SetValue ( "" );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->SlicerCoordinatesEntry->GetWidgetName());

    this->AddPointPairPushButton = vtkKWPushButton::New();
    this->AddPointPairPushButton->SetParent(okFrame);
    this->AddPointPairPushButton->Create();
    this->AddPointPairPushButton->SetText( "OK" );
    this->AddPointPairPushButton->SetWidth ( 12 );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->AddPointPairPushButton->GetWidgetName());

    // list of defined point pairs 
    vtkKWFrameWithLabel *listFrame = vtkKWFrameWithLabel::New();
    listFrame->SetParent ( regFrame->GetFrame() );
    listFrame->Create ( );
    listFrame->SetLabelText ("Defined point pairs");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                  listFrame->GetWidgetName());


    // add the multicolumn list to show the points
    this->PointPairMultiColumnList = vtkKWMultiColumnListWithScrollbars::New ( );
    this->PointPairMultiColumnList->SetParent ( listFrame->GetFrame() );
    this->PointPairMultiColumnList->Create ( );
    this->PointPairMultiColumnList->SetHeight(1);
    this->PointPairMultiColumnList->GetWidget()->SetSelectionTypeToRow();
    this->PointPairMultiColumnList->GetWidget()->MovableRowsOff();
    this->PointPairMultiColumnList->GetWidget()->MovableColumnsOff();
    // set up the columns of data for each point
    // refer to the header file for order
    this->PointPairMultiColumnList->GetWidget()->AddColumn("Patient Coordinates");
    this->PointPairMultiColumnList->GetWidget()->AddColumn("Image Coordinates");
    
    // make the selected column editable by checkbox
//    this->PointPairMultiColumnList->GetWidget()->SetColumnEditWindowToCheckButton(this->SelectedColumn);
    
    // now set the attributes that are equal across the columns
    for (int col = 0; col < 2; col++)
    {
        this->PointPairMultiColumnList->GetWidget()->SetColumnWidth(col, 22);

        this->PointPairMultiColumnList->GetWidget()->SetColumnAlignmentToLeft(col);
        this->PointPairMultiColumnList->GetWidget()->ColumnEditableOff(col);
    }
        /*
        if (col >= this->XColumn && col <= this->OrZColumn)
        {
            this->PointPairMultiColumnList->GetWidget()->SetColumnEditWindowToSpinBox(col);
        }
        */
    // set the name column width to be higher
    // this->PointPairMultiColumnList->GetWidget()->SetColumnWidth(this->NameColumn, 15);
    // set the selected column width a bit higher
    // this->PointPairMultiColumnList->GetWidget()->SetColumnWidth(this->SelectedColumn, 9);
    
    app->Script ( "pack %s -fill both -expand true",
                  this->PointPairMultiColumnList->GetWidgetName());
//                  listFrame->GetWidgetName());
//    this->PointPairMultiColumnList->GetWidget()->SetCellUpdatedCommand(this, "UpdateElement");

    // button frame
    vtkKWFrame *buttonFrame = vtkKWFrame::New();
    buttonFrame->SetParent ( listFrame->GetFrame() );
    buttonFrame->Create ( );
    app->Script ("pack %s -side top -anchor nw -fill x -pady 0 -in %s",
                 buttonFrame->GetWidgetName(),
                 listFrame->GetFrame()->GetWidgetName());
/*    
     // add an load button
    this->LoadPointPairPushButton = vtkKWPushButton::New ( );
    this->LoadPointPairPushButton->SetParent ( buttonFrame );
    this->LoadPointPairPushButton->Create ( );
    this->LoadPointPairPushButton->SetText ("Load Points");
    this->LoadPointPairPushButton->SetWidth (12);
    this->LoadPointPairPushButton->SetBalloonHelpString("Load point pairs from a file.");
    
    // add a save button
    this->SavePointPairPushButton = vtkKWPushButton::New ( );
    this->SavePointPairPushButton->SetParent ( buttonFrame );
    this->SavePointPairPushButton->Create ( );
    this->SavePointPairPushButton->SetText ("Save Points");
    this->SavePointPairPushButton->SetWidth (12);
    this->SavePointPairPushButton->SetBalloonHelpString("Save all defined point pairs to a file.");
*/

    // add a delete button 
    this->DeletePointPairPushButton = vtkKWPushButton::New ( );
    this->DeletePointPairPushButton->SetParent ( buttonFrame );
    this->DeletePointPairPushButton->Create ( );
    this->DeletePointPairPushButton->SetText ("Delete Points");
    this->DeletePointPairPushButton->SetWidth (12);
    this->DeletePointPairPushButton->SetBalloonHelpString("Delete the selected point pair.");

    // add a delete button 
    this->DeleteAllPointPairPushButton = vtkKWPushButton::New ( );
    this->DeleteAllPointPairPushButton->SetParent ( buttonFrame );
    this->DeleteAllPointPairPushButton->Create ( );
    this->DeleteAllPointPairPushButton->SetText ("Delete All Points");
    this->DeleteAllPointPairPushButton->SetWidth (12);
    this->DeleteAllPointPairPushButton->SetBalloonHelpString("Delete all point pairs.");

    app->Script("pack %s %s -side left -anchor w -padx 2 -pady 2", 
                this->DeletePointPairPushButton->GetWidgetName(),
                this->DeleteAllPointPairPushButton->GetWidgetName());



    // do registration
    vtkKWFrame *actionFrame = vtkKWFrame::New();
    actionFrame->SetParent ( regFrame->GetFrame() );
    actionFrame->Create ( );
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                  actionFrame->GetWidgetName());

     // add a register button 
    this->RegisterPushButton = vtkKWPushButton::New ( );
    this->RegisterPushButton->SetParent ( actionFrame );
    this->RegisterPushButton->Create ( );
    this->RegisterPushButton->SetText ("Register");
    this->RegisterPushButton->SetWidth (12);
    this->RegisterPushButton->SetBalloonHelpString("Perform patient to image registration.");

     // add a reset button 
    this->ResetPushButton = vtkKWPushButton::New ( );
    this->ResetPushButton->SetParent ( actionFrame );
    this->ResetPushButton->Create ( );
    this->ResetPushButton->SetText ("Reset");
    this->ResetPushButton->SetWidth (12);
    this->ResetPushButton->SetBalloonHelpString("Ignore the current registration.");


    app->Script("pack %s %s -side left -anchor w -padx 2 -pady 2", 
                this->RegisterPushButton->GetWidgetName(),
                this->ResetPushButton->GetWidgetName());


    regFrame->Delete ();
    addFrame->Delete ();
    patFrame->Delete ();
    okFrame->Delete ();
    listFrame->Delete ();
    buttonFrame->Delete ();
    actionFrame->Delete ();

}


void vtkNeuroNavGUI::BuildGUIForTrackingFrame ()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "NeuroNav" );


    // ----------------------------------------------------------------
    // Navigation FRAME            
    // ----------------------------------------------------------------
    vtkSlicerModuleCollapsibleFrame *trackingFrame = vtkSlicerModuleCollapsibleFrame::New ( );    
    trackingFrame->SetParent ( page );
    trackingFrame->Create ( );
    trackingFrame->SetLabelText ("Navigation");
    //trackingFrame->ExpandFrame ( );
    trackingFrame->CollapseFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            trackingFrame->GetWidgetName(), page->GetWidgetName());



    // Display frame: Options to locator display 
    // -----------------------------------------
    vtkKWFrameWithLabel *displayFrame = vtkKWFrameWithLabel::New ( );
    displayFrame->SetParent ( trackingFrame->GetFrame() );
    displayFrame->Create ( );
    displayFrame->SetLabelText ("Locator Display");
    this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                   displayFrame->GetWidgetName() );

    this->LocatorCheckButton = vtkKWCheckButton::New();
    this->LocatorCheckButton->SetParent(displayFrame->GetFrame());
    this->LocatorCheckButton->Create();
    this->LocatorCheckButton->SelectedStateOff();
    this->LocatorCheckButton->SetText("Show Locator");

/*
    this->HandleCheckButton = vtkKWCheckButton::New();
    this->HandleCheckButton->SetParent(displayFrame->GetFrame());
    this->HandleCheckButton->Create();
    this->HandleCheckButton->SelectedStateOff();
    this->HandleCheckButton->SetText("Show Handle");

    this->GuideCheckButton = vtkKWCheckButton::New();
    this->GuideCheckButton->SetParent(displayFrame->GetFrame());
    this->GuideCheckButton->Create();
    this->GuideCheckButton->SelectedStateOff();
    this->GuideCheckButton->SetText("Show Guide");


    this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
                this->LocatorCheckButton->GetWidgetName(),
                this->HandleCheckButton->GetWidgetName(),
                this->GuideCheckButton->GetWidgetName());
*/


    this->Script("pack %s -side left -anchor w -padx 2 -pady 2", 
                this->LocatorCheckButton->GetWidgetName());


    // Driver frame: Locator can drive slices 
    // -----------------------------------------
    vtkKWFrameWithLabel *driverFrame = vtkKWFrameWithLabel::New ( );
    driverFrame->SetParent ( trackingFrame->GetFrame() );
    driverFrame->Create ( );
    driverFrame->SetLabelText ("Driver");
    this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                   driverFrame->GetWidgetName() );

    // Mode frame
    vtkKWFrame *modeFrame = vtkKWFrame::New();
    modeFrame->SetParent ( driverFrame->GetFrame() );
    modeFrame->Create ( );
    app->Script ("pack %s -side top -anchor nw -fill x -pady 1 -in %s",
                 modeFrame->GetWidgetName(),
                 driverFrame->GetFrame()->GetWidgetName());


    this->LocatorModeCheckButton = vtkKWCheckButton::New();
    this->LocatorModeCheckButton->SetParent(modeFrame);
    this->LocatorModeCheckButton->Create();
    this->LocatorModeCheckButton->SelectedStateOff();
    this->LocatorModeCheckButton->SetText("Locator");

    this->UserModeCheckButton = vtkKWCheckButton::New();
    this->UserModeCheckButton->SetParent(modeFrame);
    this->UserModeCheckButton->Create();
    this->UserModeCheckButton->SelectedStateOn();
    this->UserModeCheckButton->SetText("User");


    this->Script("pack %s %s -side left -anchor w -padx 2 -pady 2", 
                this->LocatorModeCheckButton->GetWidgetName(),
                this->UserModeCheckButton->GetWidgetName());


    // slice frame
    vtkKWFrame *sliceFrame = vtkKWFrame::New();
    sliceFrame->SetParent ( driverFrame->GetFrame() );
    sliceFrame->Create ( );
    app->Script ("pack %s -side top -anchor nw -fill x -pady 1 -in %s",
                 sliceFrame->GetWidgetName(),
                 driverFrame->GetFrame()->GetWidgetName());


    // Contents in slice frame 
    vtkSlicerColor *color = app->GetSlicerTheme()->GetSlicerColors ( );

    this->RedSliceMenu = vtkKWMenuButton::New();
    this->RedSliceMenu->SetParent(sliceFrame);
    this->RedSliceMenu->Create();
    this->RedSliceMenu->SetWidth(10);
    this->RedSliceMenu->SetBackgroundColor(color->SliceGUIRed);
    this->RedSliceMenu->SetActiveBackgroundColor(color->SliceGUIRed);
    this->RedSliceMenu->GetMenu()->AddRadioButton ("User");
    this->RedSliceMenu->GetMenu()->AddRadioButton ("Locator");
    this->RedSliceMenu->SetValue ("User");

    this->YellowSliceMenu = vtkKWMenuButton::New();
    this->YellowSliceMenu->SetParent(sliceFrame);
    this->YellowSliceMenu->Create();
    this->YellowSliceMenu->SetWidth(10);
    this->YellowSliceMenu->SetBackgroundColor(color->SliceGUIYellow);
    this->YellowSliceMenu->SetActiveBackgroundColor(color->SliceGUIYellow);
    this->YellowSliceMenu->GetMenu()->AddRadioButton ("User");
    this->YellowSliceMenu->GetMenu()->AddRadioButton ("Locator");
    this->YellowSliceMenu->SetValue ("User");

    this->GreenSliceMenu = vtkKWMenuButton::New();
    this->GreenSliceMenu->SetParent(sliceFrame);
    this->GreenSliceMenu->Create();
    this->GreenSliceMenu->SetWidth(10);
    this->GreenSliceMenu->SetBackgroundColor(color->SliceGUIGreen);
    this->GreenSliceMenu->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->GreenSliceMenu->GetMenu()->AddRadioButton ("User");
    this->GreenSliceMenu->GetMenu()->AddRadioButton ("Locator");
    this->GreenSliceMenu->SetValue ("User");

    this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
                this->RedSliceMenu->GetWidgetName(),
                this->YellowSliceMenu->GetWidgetName(),
                this->GreenSliceMenu->GetWidgetName());

    trackingFrame->Delete();
    displayFrame->Delete();
    driverFrame->Delete();
    modeFrame->Delete();
    sliceFrame->Delete();
}



void vtkNeuroNavGUI::UpdateAll()
{
    this->LocatorMatrix = NULL;
    if (this->LocatorMatrix)
    {
        char Val[10];

        float px = this->LocatorMatrix->GetElement(0, 0);
        float py = this->LocatorMatrix->GetElement(1, 0);
        float pz = this->LocatorMatrix->GetElement(2, 0);
        float nx = this->LocatorMatrix->GetElement(0, 1);
        float ny = this->LocatorMatrix->GetElement(1, 1);
        float nz = this->LocatorMatrix->GetElement(2, 1);
        float tx = this->LocatorMatrix->GetElement(0, 2);
        float ty = this->LocatorMatrix->GetElement(1, 2);
        float tz = this->LocatorMatrix->GetElement(2, 2);

        // update the display of locator
        if (this->LocatorModeCheckButton->GetSelectedState()) this->UpdateLocator();

        //this->UpdateSliceDisplay(px, py, pz);  // RSierra 3/9/07: This line is redundant. If you remove it the slice views are still updated.
        this->UpdateSliceDisplay(nx, ny, nz, tx, ty, tz, px, py, pz);
    }
}


void vtkNeuroNavGUI::UpdateLocator()
{
    vtkTransform *transform = NULL;

    //vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->LocatorModelID.c_str())); 
    vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLModelNode1")); 
    if (model != NULL)
    {
        if (transform)
        {
            vtkMRMLLinearTransformNode *lnode = (vtkMRMLLinearTransformNode *)model->GetParentTransformNode();
            lnode->SetAndObserveMatrixTransformToParent(transform->GetMatrix());
            this->GetMRMLScene()->Modified();
        }
    }
}



void vtkNeuroNavGUI::UpdateSliceDisplay(float nx, float ny, float nz, 
                                        float tx, float ty, float tz, 
                                        float px, float py, float pz)
{
    // Axial
    if (strcmp(this->RedSliceMenu->GetValue(), "Locator"))
    {
        if (this->NeedOrientationUpdate0) 
        {
            this->SliceNode0->SetOrientationToAxial();
            this->NeedOrientationUpdate0 = 0;
        }
    }
    else
    {
        this->SliceNode0->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 0);
        this->Control0->GetOffsetScale()->SetValue(pz);
        this->Logic0->SetSliceOffset(pz);
        this->NeedOrientationUpdate0 = 1;
    }

    // Sagittal
    if (strcmp(this->YellowSliceMenu->GetValue(), "Locator"))
    {
        if (this->NeedOrientationUpdate1) 
        {
            this->SliceNode1->SetOrientationToSagittal();
            this->NeedOrientationUpdate1 = 0;
        }
    }
    else
    {
        this->SliceNode1->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 1);
        this->Control1->GetOffsetScale()->SetValue(px);
        this->Logic1->SetSliceOffset(px);
        this->NeedOrientationUpdate1 = 1;
    }

    // Coronal
    if (strcmp(this->GreenSliceMenu->GetValue(), "Locator"))
    {
        if (this->NeedOrientationUpdate2) 
        {
            this->SliceNode2->SetOrientationToCoronal();
            this->NeedOrientationUpdate2 = 0;
        }
    }
    else
    {
        this->SliceNode2->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 2);
        this->Control2->GetOffsetScale()->SetValue(py);
        this->Logic2->SetSliceOffset(py);
        this->NeedOrientationUpdate2 = 1;
    }
}

