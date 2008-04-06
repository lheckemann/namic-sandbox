#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkLegoGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkCornerAnnotation.h"

#include "vtkMRMLLinearTransformNode.h"

#include "vtkKWRenderWidget.h"
#include "vtkKWWidget.h"
#include "vtkKWCheckButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWEvent.h"
#include "vtkKWCanvas.h"
#include "vtkKWRadioButton.h"

#include <math.h> // for standard math functions
#include "vtkMath.h" // for conversion between radians and degrees

#include "vtkIGTPat2ImgRegistration.h" // for landmark registration between image and robot coordinate systems

// in and out ports
const int IN_1 = 0;
const int IN_2 = 1;
const int IN_3 = 2;
const int IN_4 = 3;
const int OUT_A = 0;
const int OUT_B = 1;
const int OUT_C = 2;

// maximum and minumum number of rotations for each motor
const int MIN_MOTOR_A = -1800;
const int MAX_MOTOR_A = 1800;
const int MIN_MOTOR_B = -1170;
const int MAX_MOTOR_B = 0;
const int MIN_MOTOR_C = 0;
const int MAX_MOTOR_C = 1260;

// values for the radio buttons
const char * BASIC_TUTORIAL_RADIO = "basicTutorial";
const char * ADVANCED_TUTORIAL_RADIO = "advancedTutorial";
const char * REGISTRATION_RADIO_0 = "registration0";
const char * REGISTRATION_RADIO_1 = "registration1";
const char * REGISTRATION_RADIO_2 = "registration2";
const char * REGISTRATION_RADIO_3 = "registration3";
const char * REGISTRATION_RADIO_4 = "registration4";
const char * REGISTRATION_RADIO_5 = "registration5";
const char * REGISTRATION_RADIO_6 = "registration6";
const char * REGISTRATION_RADIO_7 = "registration7";
const char * TARGET_RADIO = "target";

// parameters for the graphs
const int CANVAS_WIDTH = 350;
const int CANVAS_HEIGHT = 230;
const int RECTANGLE_WIDTH = 250; // used to be 260
const int RECTANGLE_HEIGHT = 177;
const int RECTANGLE_START_X = 75;
const int RECTANGLE_START_Y = 10;
const int NUM_TICK_SECTIONS = 5;
const int MIN_US_DISTANCE_DISPLAY = 0;
const int MAX_US_DISTANCE_DISPLAY = 50;

// number of registration coordinates used
const int NUM_REGISTRATION_POINTS = 8;
const int NUM_SCANNED_REGISTRATION_POINTS = 4;

// number of rotations for each motor in the registration swipe
const int REGISTRATION_ROTATIONS_MOTOR_A = 1500;
const int NUM_REGISTRATION_POINTS_MOTOR_A = (REGISTRATION_ROTATIONS_MOTOR_A * 2) + 1;
const int REGISTRATION_ROTATIONS_MOTOR_B_TOP =200;
const int REGISTRATION_ROTATIONS_MOTOR_B_BOTTOM =800;

// minimum and maximum valid values for ultrasound distances
const int MIN_US_DISTANCE = 1;
const int MAX_US_DISTANCE_TOP = 35;
const int MAX_US_DISTANCE_BOTTOM = 20;

// parameters for the median filters of ultrasound distances
const int FILTER_WIDTH_BOTTOM_SWIPE = 35; // determined empirically;
const int FILTER_WIDTH_TOP_SWIPE = 71; // determined empirically
const int AVERAGE_WIDTH_BOTTOM_SWIPE = 10;

// values used in forward and inverse kinematics
const double Y_PRIME = 2.0;
const double TOP_BEAM_LENGTH = 18.0;
const double I1 = 15.0;
const double BASE_N = 11.0;
const double MAX_N = BASE_N + 4.125;
const double INVERSE_KINEMATICS_PHI_ERROR = 1.5;
const double INVERSE_KINEMATICS_N_STEP = 0.001;

// conversion between degrees and radians
const double DOUBLE_DEGREES_TO_RADIANS = vtkMath::DoubleDegreesToRadians();
const double DOUBLE_RADIANS_TO_DEGREES = vtkMath::DoubleRadiansToDegrees();

// distances and angles for the pillar registration coordinates, used in the advanced tutorial
const double SIDEWAYS_OFFSET = 2.0;
const double DISTANCE_TO_PHANTOM = 23.1545; // calculated as the mean of eleven points
const double PILLAR_X_DISTANCE_TOP = 22.0;
const double PILLAR_X_DISTANCE_BOTTOM = 16.0;
const double PILLAR_Y_DISTANCE_TOP = 11.0;
const double PILLAR_Y_DISTANCE_BOTTOM = 2.0;
const double PILLAR_D1_TOP_LEFT = sqrt(pow((PILLAR_X_DISTANCE_TOP + SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_TOP), 2));
const double PILLAR_D2_TOP_RIGHT = sqrt(pow((PILLAR_X_DISTANCE_TOP - SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_TOP), 2));
const double PILLAR_D3_BOTTOM_LEFT = sqrt(pow((PILLAR_X_DISTANCE_BOTTOM + SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_BOTTOM), 2));
const double PILLAR_D4_BOTTOM_RIGHT = sqrt(pow((PILLAR_X_DISTANCE_BOTTOM - SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_BOTTOM), 2));
const double PILLAR_DET_ANGLE_1 = DOUBLE_RADIANS_TO_DEGREES * (atan2((PILLAR_X_DISTANCE_TOP + SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_TOP)));
const double PILLAR_DET_ANGLE_2 = DOUBLE_RADIANS_TO_DEGREES * (atan2((PILLAR_X_DISTANCE_TOP - SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_TOP)));
const double PILLAR_DET_ANGLE_3 = DOUBLE_RADIANS_TO_DEGREES * (atan2((PILLAR_X_DISTANCE_BOTTOM + SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_BOTTOM)));
const double PILLAR_DET_ANGLE_4 = DOUBLE_RADIANS_TO_DEGREES * (atan2((PILLAR_X_DISTANCE_BOTTOM - SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + PILLAR_Y_DISTANCE_BOTTOM)));
const double PILLAR_HEIGHT_TOP = 19.0;
const double PILLAR_HEIGHT_BOTTOM = 8.25;
const double PILLAR_RADIUS = 2.0;

// distances and angles for the central box registration coordinates, used in the advanced tutorial
const double CENTRAL_BOX_X_DISTANCE_TOP = 8.0;
const double CENTRAL_BOX_X_DISTANCE_BOTTOM = 8.0;
const double CENTRAL_BOX_Y_DISTANCE_TOP = 12.0;
const double CENTRAL_BOX_Y_DISTANCE_BOTTOM = 0.0;
const double CENTRAL_BOX_D1_TOP_LEFT = sqrt(pow((CENTRAL_BOX_X_DISTANCE_TOP + SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_TOP), 2));
const double CENTRAL_BOX_D2_TOP_RIGHT = sqrt(pow((CENTRAL_BOX_X_DISTANCE_TOP - SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_TOP), 2));
const double CENTRAL_BOX_D3_BOTTOM_LEFT = sqrt(pow((CENTRAL_BOX_X_DISTANCE_BOTTOM + SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_BOTTOM), 2));
const double CENTRAL_BOX_D4_BOTTOM_RIGHT = sqrt(pow((CENTRAL_BOX_X_DISTANCE_BOTTOM - SIDEWAYS_OFFSET), 2) + pow((DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_BOTTOM), 2));
const double CENTRAL_BOX_DET_ANGLE_1 = DOUBLE_RADIANS_TO_DEGREES * (atan2((CENTRAL_BOX_X_DISTANCE_TOP + SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_TOP)));
const double CENTRAL_BOX_DET_ANGLE_2 = DOUBLE_RADIANS_TO_DEGREES * (atan2((CENTRAL_BOX_X_DISTANCE_TOP - SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_TOP)));
const double CENTRAL_BOX_DET_ANGLE_3 = DOUBLE_RADIANS_TO_DEGREES * (atan2((CENTRAL_BOX_X_DISTANCE_BOTTOM + SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_BOTTOM)));
const double CENTRAL_BOX_DET_ANGLE_4 = DOUBLE_RADIANS_TO_DEGREES * (atan2((CENTRAL_BOX_X_DISTANCE_BOTTOM - SIDEWAYS_OFFSET), (DISTANCE_TO_PHANTOM + CENTRAL_BOX_Y_DISTANCE_BOTTOM)));
const double CENTRAL_BOX_HEIGHT = -2.5;

// Scale between the image and robot coordinate systems
const double SCALE = 23.3 / 3.0;


vtkStandardNewMacro (vtkLegoGUI );
vtkCxxRevisionMacro ( vtkLegoGUI, "$Revision: 1.0 $");

/** Usual VTK print function */
void vtkLegoGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "LegoGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}

/** Builds the GUI */
void vtkLegoGUI::BuildGUI ( )
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    
    // Define your help text here.
    const char *help = "The **Lego Module** uses a Lego Mindstorms NXT robot to provide a hands-on, accessible tutorial on image-guided therapy and medical robotics";

    // create a page
    this->UIPanel->AddPage ( "Lego", "Lego", NULL );

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );

    vtkSlicerModuleCollapsibleFrame *LegoHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    LegoHelpFrame->SetParent ( page );
    LegoHelpFrame->Create ( );
    LegoHelpFrame->CollapseFrame ( );
    LegoHelpFrame->SetLabelText ("Help");
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            LegoHelpFrame->GetWidgetName(), page->GetWidgetName());

    // configure the parent classes help text widget
    this->HelpText->SetParent ( LegoHelpFrame->GetFrame() );
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

    LegoHelpFrame->Delete();

    this->BuildGUIForLegoDeviceFrame();
    this->BuildGUIForBasicTutorialFrame();
    this->BuildGUIForAdvancedTutorialFrame();
    this->BuildGUIForPlanningFrame();
    this->BuildGUIForBiopsyFrame();

} // end BuildGUI

/** Add observers on widgets in the GUI */
void vtkLegoGUI::AddGUIObservers ( )
{
    this->RemoveGUIObservers();

    // make a user interactor style to process our events
    // look at the InteractorStyle to get our events

    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

    appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);

    // add observers for our GUI components
    this->LegoConnectCheckButton->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->BasicTutorialRadioButton->AddObserver ( vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->AdvancedTutorialRadioButton->AddObserver ( vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationScanButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->RegistrationPhantomOKButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->RegistrationPhantomResetButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->RegistrationImageCoordinatesOKButton0->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton1->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton2->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton3->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton4->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton5->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton6->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RegistrationImageCoordinatesOKButton7->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this-AddTargetPushButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ExecuteBiopsyPushButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

} // end AddGUIObservers

/** Remove GUI observers */
void vtkLegoGUI::RemoveGUIObservers ( )
{
    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

    appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

    if (this->LegoConnectCheckButton)
    {
        this->LegoConnectCheckButton->RemoveObservers ( vtkKWCheckButton::SelectedStateChangedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->BasicTutorialRadioButton)
    {
        this->BasicTutorialRadioButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->AdvancedTutorialRadioButton)
    {
        this->AdvancedTutorialRadioButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }



    if (this->RegistrationScanButton)
    {
        this->RegistrationScanButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationPhantomOKButton)
    {
        this->RegistrationPhantomOKButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationPhantomResetButton)
    {
        this->RegistrationPhantomResetButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton0)
    {
        this->RegistrationImageCoordinatesOKButton0->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton1)
    {
        this->RegistrationImageCoordinatesOKButton1->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton2)
    {
        this->RegistrationImageCoordinatesOKButton2->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton3)
    {
        this->RegistrationImageCoordinatesOKButton3->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton4)
    {
        this->RegistrationImageCoordinatesOKButton4->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton5)
    {
        this->RegistrationImageCoordinatesOKButton5->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton6)
    {
        this->RegistrationImageCoordinatesOKButton6->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    if (this->RegistrationImageCoordinatesOKButton7)
    {
        this->RegistrationImageCoordinatesOKButton7->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }

    if (this->AddTargetPushButton)
    {
        this->AddTargetPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }

    if (this->ExecuteBiopsyPushButton)
    {
        this->ExecuteBiopsyPushButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }

} // end RemoveGUIObservers


/** Process events invoked by the logic */
void vtkLegoGUI::ProcessLogicEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    // Fill in
} // end ProcessLogicEvents


/** Process events invoked by the GUI */
void vtkLegoGUI::ProcessGUIEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    const char *eventName = vtkCommand::GetStringFromEventId(event);
    
    // ********Left button press event *************
    if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
        vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
        HandleMouseEvent(style);
    }

    else
    {
    // User wants to connect to the lego device
    if (this->LegoConnectCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
        int checked = this->LegoConnectCheckButton->GetSelectedState();
        if (checked)
        {
            ConnectToLegoDevice();
            SetupLegoDevice();
        }

    }
    // User wants to scan for registration coordinates in the robot coordinate system
    else if (this->RegistrationScanButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        this->GetRobotRegistrationCoordinatesForAdvancedTutorial();
    }

    // User says that registration targets found are ok
    else if (this->RegistrationPhantomOKButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char * rasText;
        for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
        {


            char coordinates[256];
            for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
            {
                sprintf(coordinates, " %.2f %.2f %.2f", this->RobotRegistrationCoordinates[i][0], this->RobotRegistrationCoordinates[i][1], this->RobotRegistrationCoordinates[i][2]);        
                this->SummaryList->InsertCellText(i, 1, coordinates);
            }
        }

    }

    // User wants to reset the robot registration coordinates in order to rescan
    else if (this->RegistrationPhantomResetButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {    
        for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                this->RobotRegistrationCoordinates[i][j] = 0;
            }
            this->RegistrationRobotCoordinatesList->SetCellText(i, 1, NULL);
            this->SummaryList->SetCellText(i, 1, NULL);
        }
        CreateBasicGraph(this->RegistrationCanvasTop);
        CreateBasicGraph(this->RegistrationCanvasBottom);
    }

    // User says that image coordinate 0 is ok
    else if (this->RegistrationImageCoordinatesOKButton0 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry0->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(0, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[0]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[0][i] = this->ImageRegistrationCoordinates[0][i] / SCALE;
        }
    }

    // User says that image coordinate 1 is ok
    else if (this->RegistrationImageCoordinatesOKButton1 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry1->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(1, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[1]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[1][i] = this->ImageRegistrationCoordinates[1][i] / SCALE;
        }
    }

    // User says that image coordinate 2 is ok
    else if (this->RegistrationImageCoordinatesOKButton2 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry2->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(2, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[2]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[2][i] = this->ImageRegistrationCoordinates[2][i] / SCALE;
        }
    }

    // User says that image coordinate 3 is ok
    else if (this->RegistrationImageCoordinatesOKButton3 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry3->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(3, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[3]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[3][i] = this->ImageRegistrationCoordinates[3][i] / SCALE;
        }
    }

    // User says that image coordinate 4 is ok
    else if (this->RegistrationImageCoordinatesOKButton4 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry4->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(4, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[4]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[4][i] = this->ImageRegistrationCoordinates[4][i] / SCALE;
        }
    }

    // User says that image coordinate 5 is ok
    else if (this->RegistrationImageCoordinatesOKButton5 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry5->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(5, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[5]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[5][i] = this->ImageRegistrationCoordinates[5][i] / SCALE;
        }
    }

    // User says that image coordinate 6 is ok
    else if (this->RegistrationImageCoordinatesOKButton6 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry6->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(6, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[6]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[6][i] = this->ImageRegistrationCoordinates[6][i] / SCALE;
        }
    }

    // User says that image coordinate 7 is ok
    else if (this->RegistrationImageCoordinatesOKButton7 == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->RegistrationImageCoordinatesEntry7->GetWidget()->GetValue();
        this->SummaryList->InsertCellText(7, 2, rasText);
        this->StripCoordinatesFromString(rasText, this->ImageRegistrationCoordinates[7]);
        for (int i = 0; i < 3; i++)
        {
            this->ImageRegistrationCoordinates[7][i] = this->ImageRegistrationCoordinates[7][i] / SCALE;
        }
    }

    // User says that target coordinate is ok
    else if (this->AddTargetPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        const char* rasText = this->TargetCoordinatesEntry->GetWidget()->GetValue();
        this->SummaryTargetImageCoordinateText->SetText(rasText);
        this->StripCoordinatesFromString(rasText, this->TargetInImageCoordinates);

        // scale target
        for (int i = 0; i < 3; i++)
        {
            this->TargetInImageCoordinates[i] = this->TargetInImageCoordinates[i] / SCALE;
        }

    }

        // User wants to execute the biopsy
    else if (this->ExecuteBiopsyPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        // get the target in the robot coord system
        //TODO ensure that we have image and robot coords

        
        ExecuteBiopsy();
    }

    // User wants to do the advanced tutorial
    else if (this->AdvancedTutorialRadioButton->GetSelectedState() == 1)
    {
        this->BasicTutorial = false;

        for (int i = 0; i < 3; i++)
        {
            this->TargetInRobotCoordinates[i] = 0;
            this->CurrentActuatorPositionInImageCoordinates[i] = 0;
            this->CurrentActuatorPositionInRobotCoordinates[i] = 0;
            this->CurrentNormalToActuator[i] = 0;
            this->FinalActuatorPositionInRobotCoordinates[i] = 0;
            this->CurrentMotorRotations[i] = 0;
        }

        for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                this->ImageRegistrationCoordinates[i][j] = 0;
                this->RobotRegistrationCoordinates[i][j] = 0;
            }
            this->RegistrationRobotCoordinatesList->SetCellText(i, 1, NULL);
            this->SummaryList->SetCellText(i, 1, NULL);
            this->SummaryList->SetCellText(i, 2, NULL);
        }
        //CreateBasicGraph(this->RegistrationCanvasTop);
        //CreateBasicGraph(this->RegistrationCanvasBottom);

        for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
            {
            for (int i = 0; i < 3; i++)
            {
            MotorAValuesForRobotRegistrationCoordinates[i] = 0;
            }
            }
        this->ImageToRobotRegistrationMatrix->Zero();
        this->RobotToImageRegistrationMatrix->Zero();

        this->RegistrationImageCoordinatesEntry0->SetLabelText("");
        this->RegistrationImageCoordinatesEntry1->SetLabelText("");
        this->RegistrationImageCoordinatesEntry2->SetLabelText("");
        this->RegistrationImageCoordinatesEntry3->SetLabelText("");
        this->RegistrationImageCoordinatesEntry4->SetLabelText("");
        this->RegistrationImageCoordinatesEntry5->SetLabelText("");
        this->RegistrationImageCoordinatesEntry6->SetLabelText("");
        this->RegistrationImageCoordinatesEntry7->SetLabelText("");

        this->RegistrationScanButton->SetStateToNormal();
        this->RegistrationPhantomOKButton->SetStateToNormal();
        this->RegistrationPhantomResetButton->SetStateToNormal();
        
        this->RegistrationImageCoordinatesOKButton0->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton1->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton2->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton3->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton4->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton5->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton6->SetStateToNormal();
        this->RegistrationImageCoordinatesOKButton7->SetStateToNormal();

        this->RegistrationImageCoordinatesRadio0->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio1->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio2->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio3->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio4->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio5->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio6->SetStateToNormal();
        this->RegistrationImageCoordinatesRadio7->SetStateToNormal();

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                this->SummaryRegistrationMatrixList->InsertCellText(i, j, "");
            }
        }

        this->SummaryTargetRobotCoordinateText->SetText("");
        this->SummaryFinalRobotCoordinateText->SetText("");

    }
    // User wants to do the basic tutorial
    else if (this->BasicTutorialRadioButton->GetSelectedState() == 1)
    {
        this->BasicTutorial = true;

        // clear stuff in the advanced panes
        
        for (int i = 0; i < 3; i++)
        {
            this->TargetInRobotCoordinates[i] = 0;
            this->CurrentActuatorPositionInImageCoordinates[i] = 0;
            this->CurrentActuatorPositionInRobotCoordinates[i] = 0;
            this->CurrentNormalToActuator[i] = 0;
            this->FinalActuatorPositionInRobotCoordinates[i] = 0;
            this->CurrentMotorRotations[i] = 0;
        }

        for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                this->ImageRegistrationCoordinates[i][j] = 0;
                this->RobotRegistrationCoordinates[i][j] = 0;
            }
            this->RegistrationRobotCoordinatesList->SetCellText(i, 1, NULL);
            this->SummaryList->SetCellText(i, 1, NULL);
            this->SummaryList->SetCellText(i, 2, NULL);
        }


        CreateBasicGraph(this->RegistrationCanvasTop);
        CreateBasicGraph(this->RegistrationCanvasBottom);

        for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
            {
            for (int i = 0; i < 3; i++)
            {
            this->MotorAValuesForRobotRegistrationCoordinates[i] = 0;
            }
            }

        this->RegistrationImageCoordinatesEntry0->SetLabelText("");
        this->RegistrationImageCoordinatesEntry1->SetLabelText("");
        this->RegistrationImageCoordinatesEntry2->SetLabelText("");
        this->RegistrationImageCoordinatesEntry3->SetLabelText("");
        this->RegistrationImageCoordinatesEntry4->SetLabelText("");
        this->RegistrationImageCoordinatesEntry5->SetLabelText("");
        this->RegistrationImageCoordinatesEntry6->SetLabelText("");
        this->RegistrationImageCoordinatesEntry7->SetLabelText("");

        this->RegistrationScanButton->SetStateToDisabled();
        this->RegistrationPhantomOKButton->SetStateToDisabled();
        this->RegistrationPhantomResetButton->SetStateToDisabled();

    
        this->RegistrationImageCoordinatesOKButton0->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton1->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton2->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton3->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton4->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton5->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton6->SetStateToDisabled();
        this->RegistrationImageCoordinatesOKButton7->SetStateToDisabled();

        this->RegistrationImageCoordinatesRadio0->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio1->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio2->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio3->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio4->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio5->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio6->SetStateToDisabled();
        this->RegistrationImageCoordinatesRadio7->SetStateToDisabled();

        this->SummaryTargetRobotCoordinateText->SetText("");
        this->SummaryFinalRobotCoordinateText->SetText("");

        this->CalculateRegistrationMatrixForBasicTutorial();    
    }

    }
} // end ProcessGUIEvents


/** Process MRML events */
void vtkLegoGUI::ProcessMRMLEvents ( vtkObject *caller,
        unsigned long event, void *callData )
{
    // Fill in
} // End ProcessMRMLEvents

/** Handles mouse events */
void vtkLegoGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
    vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
    vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
    vtkSlicerInteractorStyle *istyle1 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
    vtkSlicerInteractorStyle *istyle2 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());

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


    if (this->TargetCoordinatesRadio->GetSelectedState() == 1)
    {
              this->TargetCoordinatesEntry->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio0->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry0->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio1->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry1->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio2->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry2->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio3->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry3->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio4->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry4->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio5->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry5->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio6->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry6->GetWidget()->SetValue(ras.c_str());
    }
    else if (this->RegistrationImageCoordinatesRadio7->GetSelectedState() == 1)
    {
              this->RegistrationImageCoordinatesEntry7->GetWidget()->SetValue(ras.c_str());
    }

       }
    }
} // end HandleMouseEvent

/** Behavior at module startup */
void vtkLegoGUI::Enter ( )
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
} // end Enter

/** Behavior at module exit */
void vtkLegoGUI::Exit ( )
{
    // Fill in
} // end Exit

/** Initialization */
void vtkLegoGUI::Init()
{
    this->DataManager->SetMRMLScene(this->GetMRMLScene());
    this->LocatorModelID = std::string(this->DataManager->RegisterStream(0));
}

/** BTX */
void vtkLegoGUI::DataCallback(vtkObject *caller, 
            unsigned long eid, void *clientData, void *callData)
{
    vtkLegoGUI *self = reinterpret_cast<vtkLegoGUI *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkLegoGUI DataCallback");

    self->UpdateAll();
}

/** Constructor */
vtkLegoGUI::vtkLegoGUI ( )
{

    this->LegoConnectCheckButton = NULL;
    this->LegoConnectLabel = NULL;
    this->BasicTutorialRadioButton = NULL;
    this->AdvancedTutorialRadioButton = NULL;
    this->RegistrationScanButton = NULL;
    this->RegistrationScanStatusLabel = NULL;
    this->RegistrationCanvasTop = NULL;
    this->RegistrationCanvasBottom = NULL;
    this->RegistrationRobotCoordinatesList = NULL;
    this->RegistrationPhantomOKButton = NULL;
    this->RegistrationPhantomResetButton = NULL;
    this->RegistrationImageCoordinatesRadio0 = NULL;
    this->RegistrationImageCoordinatesEntry0 = NULL;
    this->RegistrationImageCoordinatesOKButton0 = NULL;
    this->RegistrationImageCoordinatesRadio1 = NULL;
    this->RegistrationImageCoordinatesEntry1 = NULL;
    this->RegistrationImageCoordinatesOKButton1 = NULL;
    this->RegistrationImageCoordinatesRadio2 = NULL;
    this->RegistrationImageCoordinatesEntry2 = NULL;
    this->RegistrationImageCoordinatesOKButton2 = NULL;
    this->RegistrationImageCoordinatesRadio3 = NULL;
    this->RegistrationImageCoordinatesEntry3 = NULL;
    this->RegistrationImageCoordinatesOKButton3 = NULL;
    this->RegistrationImageCoordinatesRadio4 = NULL;
    this->RegistrationImageCoordinatesEntry4 = NULL;
    this->RegistrationImageCoordinatesOKButton4 = NULL;
    this->RegistrationImageCoordinatesRadio5 = NULL;
    this->RegistrationImageCoordinatesEntry5 = NULL;
    this->RegistrationImageCoordinatesOKButton5 = NULL;
    this->RegistrationImageCoordinatesRadio6 = NULL;
    this->RegistrationImageCoordinatesEntry6 = NULL;
    this->RegistrationImageCoordinatesOKButton6 = NULL;
    this->RegistrationImageCoordinatesRadio7 = NULL;
    this->RegistrationImageCoordinatesEntry7 = NULL;
    this->RegistrationImageCoordinatesOKButton7 = NULL;
    this->TargetCoordinatesRadio = NULL;
    this->TargetCoordinatesEntry = NULL;
    this->AddTargetPushButton = NULL;
    this->SummaryRegistrationMatrixList = NULL;
    this->SummaryList = NULL;
    this->SummaryTargetImageCoordinateText = NULL;
    this->ExecuteBiopsyPushButton = NULL;
    this->SummaryTargetRobotCoordinateText = NULL;
    this->SummaryFinalRobotCoordinateText = NULL;

    this->LegoUSB = new NXT_USB();
    this->Connected = false;
    this->BasicTutorial = true;

    this->Logic = NULL;

    this->LocatorMatrix = NULL;
    this->LocatorModelDisplayNode = NULL;

    this->DataManager = vtkIGTDataManager::New();
    this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();

    this->DataCallbackCommand = vtkCallbackCommand::New();
    this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
    this->DataCallbackCommand->SetCallback(vtkLegoGUI::DataCallback);

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

    this->ImageRegistrationCoordinates = new double*[NUM_REGISTRATION_POINTS];
    this->RobotRegistrationCoordinates = new double*[NUM_REGISTRATION_POINTS];
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        *(this->ImageRegistrationCoordinates+i) = new double[3];
        *(this->RobotRegistrationCoordinates+i) = new double[3];

    for (int j = 0; j < 3; j++)
    {
        this->ImageRegistrationCoordinates[i][j] = 0;
        this->RobotRegistrationCoordinates[i][j] = 0;
    }
    }

    this->ImageToRobotRegistrationMatrix = vtkMatrix4x4::New();
    this->RobotToImageRegistrationMatrix = vtkMatrix4x4::New();

    this->MotorAValuesForRobotRegistrationCoordinates = new double[NUM_SCANNED_REGISTRATION_POINTS];
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
    for (int i = 0; i < 3; i++)
    {
        MotorAValuesForRobotRegistrationCoordinates[i] = 0;
    }
    }


} // end Constructor

/** Destructor */
vtkLegoGUI::~vtkLegoGUI ( )
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

    if (this->LegoConnectCheckButton)
    {
        this->LegoConnectCheckButton->SetParent(NULL );
        this->LegoConnectCheckButton->Delete ( );
    }
    if (this->LegoConnectLabel)
    {
        this->LegoConnectLabel->SetParent(NULL );
        this->LegoConnectLabel->Delete ( );
    }
    if (this->BasicTutorialRadioButton)
    {
        this->BasicTutorialRadioButton->SetParent(NULL );
        this->BasicTutorialRadioButton->Delete ( );
    }

    if (this->AdvancedTutorialRadioButton)
    {
        this->AdvancedTutorialRadioButton->SetParent(NULL );
        this->AdvancedTutorialRadioButton->Delete ( );
    }

    if (this->RegistrationScanButton)
    {
        this->RegistrationScanButton->SetParent(NULL );
        this->RegistrationScanButton->Delete ( );
    }
    if (this->RegistrationScanStatusLabel)
    {
        this->RegistrationScanStatusLabel->SetParent(NULL );
        this->RegistrationScanStatusLabel->Delete ( );
    }
    if (this->RegistrationCanvasTop)
    {
        this->RegistrationCanvasTop->SetParent(NULL );
        this->RegistrationCanvasTop->Delete ( );
    }
    if (this->RegistrationCanvasBottom)
    {
        this->RegistrationCanvasBottom->SetParent(NULL );
        this->RegistrationCanvasBottom->Delete ( );
    }

    if (this->RegistrationRobotCoordinatesList)
    {
        this->RegistrationRobotCoordinatesList->SetParent(NULL );
        this->RegistrationRobotCoordinatesList->Delete ( );
    }
    if (this->RegistrationPhantomOKButton)
    {
        this->RegistrationPhantomOKButton->SetParent(NULL );
        this->RegistrationPhantomOKButton->Delete ( );
    }
    if (this->RegistrationPhantomResetButton)
    {
        this->RegistrationPhantomResetButton->SetParent(NULL );
        this->RegistrationPhantomResetButton->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio0)
    {
        this->RegistrationImageCoordinatesRadio0->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio0->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry0)
    {
        this->RegistrationImageCoordinatesEntry0->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry0->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton0)
    {
        this->RegistrationImageCoordinatesOKButton0->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton0->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio1)
    {
        this->RegistrationImageCoordinatesRadio1->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio1->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry1)
    {
        this->RegistrationImageCoordinatesEntry1->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry1->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton1)
    {
        this->RegistrationImageCoordinatesOKButton1->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton1->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio2)
    {
        this->RegistrationImageCoordinatesRadio2->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio2->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry2)
    {
        this->RegistrationImageCoordinatesEntry2->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry2->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton2)
    {
        this->RegistrationImageCoordinatesOKButton2->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton2->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio3)
    {
        this->RegistrationImageCoordinatesRadio3->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio3->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry3)
    {
        this->RegistrationImageCoordinatesEntry3->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry3->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton3)
    {
        this->RegistrationImageCoordinatesOKButton3->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton3->Delete ( );
    }

 if (this->RegistrationImageCoordinatesRadio4)
    {
        this->RegistrationImageCoordinatesRadio4->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio4->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry4)
    {
        this->RegistrationImageCoordinatesEntry4->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry4->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton4)
    {
        this->RegistrationImageCoordinatesOKButton4->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton4->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio5)
    {
        this->RegistrationImageCoordinatesRadio5->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio5->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry5)
    {
        this->RegistrationImageCoordinatesEntry5->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry5->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton5)
    {
        this->RegistrationImageCoordinatesOKButton5->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton5->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio6)
    {
        this->RegistrationImageCoordinatesRadio6->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio6->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry6)
    {
        this->RegistrationImageCoordinatesEntry6->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry6->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton6)
    {
        this->RegistrationImageCoordinatesOKButton6->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton6->Delete ( );
    }

    if (this->RegistrationImageCoordinatesRadio7)
    {
        this->RegistrationImageCoordinatesRadio7->SetParent(NULL );
        this->RegistrationImageCoordinatesRadio7->Delete ( );
    }
    if (this->RegistrationImageCoordinatesEntry7)
    {
        this->RegistrationImageCoordinatesEntry7->SetParent(NULL );
        this->RegistrationImageCoordinatesEntry7->Delete ( );
    }
    if (this->RegistrationImageCoordinatesOKButton7)
    {
        this->RegistrationImageCoordinatesOKButton7->SetParent(NULL );
        this->RegistrationImageCoordinatesOKButton7->Delete ( );
    }
    if (this->SummaryList)
    {
        this->SummaryList->SetParent(NULL );
        this->SummaryList->Delete ( );
    }
    if (this->TargetCoordinatesEntry)
    {
        this->TargetCoordinatesEntry->SetParent(NULL );
        this->TargetCoordinatesEntry->Delete ( );
    }
    if (this->AddTargetPushButton)
    {
        this->AddTargetPushButton->SetParent(NULL );
        this->AddTargetPushButton->Delete ( );
    }
    if (this->SummaryRegistrationMatrixList)
    {
        this->SummaryRegistrationMatrixList->SetParent(NULL );
        this->SummaryRegistrationMatrixList->Delete ( );
    }

    if (this->SummaryTargetImageCoordinateText)
    {
        this->SummaryTargetImageCoordinateText->SetParent(NULL );
        this->SummaryTargetImageCoordinateText->Delete ( );
    }

    if (this->ExecuteBiopsyPushButton)
    {
        this->ExecuteBiopsyPushButton->SetParent(NULL );
        this->ExecuteBiopsyPushButton->Delete ( );
    }

    if (this->SummaryTargetRobotCoordinateText)
    {
        this->SummaryTargetRobotCoordinateText->SetParent(NULL );
        this->SummaryTargetRobotCoordinateText->Delete ( );
    }
    if (this->SummaryFinalRobotCoordinateText)
    {
        this->SummaryFinalRobotCoordinateText->SetParent(NULL );
        this->SummaryFinalRobotCoordinateText->Delete ( );
    }

    if (this->Connected)
    {
        this->LegoUSB->CloseLegoUSB();
    }

    delete this->LegoUSB;

    this->SetModuleLogic ( NULL );

    this->ImageToRobotRegistrationMatrix->Delete();
    this->RobotToImageRegistrationMatrix->Delete();
}













//---------------------------------------------------------------------------
void vtkLegoGUI::BuildGUIForLegoDeviceFrame()
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );

    vtkSlicerModuleCollapsibleFrame *LegoDeviceFrame = vtkSlicerModuleCollapsibleFrame::New();
    LegoDeviceFrame->SetParent(page);
    LegoDeviceFrame->Create();
    LegoDeviceFrame->SetLabelText ("Lego Device");
    LegoDeviceFrame->CollapseFrame();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
           LegoDeviceFrame->GetWidgetName(), page->GetWidgetName());

    vtkKWFrameWithLabel *LegoDeviceFrameLabel = vtkKWFrameWithLabel::New();
    LegoDeviceFrameLabel->SetParent (LegoDeviceFrame->GetFrame());
    LegoDeviceFrameLabel->Create();
    LegoDeviceFrameLabel->SetLabelText ("Connect to the Lego device");
        this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 LegoDeviceFrameLabel->GetWidgetName());

    vtkKWFrame *LegoDeviceCheckFrame = vtkKWFrame::New();
    LegoDeviceCheckFrame->SetParent (LegoDeviceFrameLabel->GetFrame());
    LegoDeviceCheckFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 LegoDeviceCheckFrame->GetWidgetName());

    vtkKWFrame *LegoDeviceLabelFrame = vtkKWFrame::New();
    LegoDeviceLabelFrame->SetParent (LegoDeviceFrameLabel->GetFrame());
    LegoDeviceLabelFrame->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 LegoDeviceLabelFrame->GetWidgetName());

    this->LegoConnectCheckButton = vtkKWCheckButton::New();
    this->LegoConnectCheckButton->SetParent(LegoDeviceCheckFrame);
    this->LegoConnectCheckButton->Create();
    this->LegoConnectCheckButton->SelectedStateOff();
    this->LegoConnectCheckButton->SetText("Connect");
    this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", 
    this->LegoConnectCheckButton->GetWidgetName());

    this->LegoConnectLabel = vtkKWLabel::New();
    this->LegoConnectLabel->SetParent(LegoDeviceLabelFrame);
    this->LegoConnectLabel->Create();
    this->LegoConnectLabel->SetText("Not Connected\nDevice Found:");
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->LegoConnectLabel->GetWidgetName());
}


void vtkLegoGUI::BuildGUIForBasicTutorialFrame()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );

    vtkSlicerModuleCollapsibleFrame *BasicTutorialFrame = vtkSlicerModuleCollapsibleFrame::New();
    BasicTutorialFrame->SetParent(page);
    BasicTutorialFrame->Create();
    BasicTutorialFrame->SetLabelText ("Basic Tutorial");
    BasicTutorialFrame->CollapseFrame();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
           BasicTutorialFrame->GetWidgetName(), page->GetWidgetName());

    /*vtkKWFrame *BasicTutorialMatrixFrame = vtkKWFrame::New();
    BasicTutorialMatrixFrame->SetParent (BasicTutorialFrame->GetFrame());
    BasicTutorialMatrixFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", BasicTutorialMatrixFrame->GetWidgetName());*/

    this->BasicTutorialRadioButton = vtkKWRadioButton::New();
    this->BasicTutorialRadioButton->SetParent(BasicTutorialFrame->GetFrame());
    this->BasicTutorialRadioButton->Create();
    this->BasicTutorialRadioButton->SetText ("Basic Tutorial");
    this->BasicTutorialRadioButton->SetCompoundModeToLeft();
    this->BasicTutorialRadioButton->IndicatorVisibilityOff();
    this->BasicTutorialRadioButton->SetValue(BASIC_TUTORIAL_RADIO);
    this->BasicTutorialRadioButton->SetSelectedState(0);
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", BasicTutorialRadioButton->GetWidgetName());






}




//---------------------------------------------------------------------------
void vtkLegoGUI::BuildGUIForAdvancedTutorialFrame ()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );

    vtkSlicerModuleCollapsibleFrame *RegistrationFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    RegistrationFrame->SetParent ( page );
    RegistrationFrame->Create ( );
    RegistrationFrame->SetLabelText ("Advanced Tutorial");
    RegistrationFrame->CollapseFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            RegistrationFrame->GetWidgetName(), page->GetWidgetName());

    vtkKWFrame *AdvancedTutorialRadioFrame = vtkKWFrame::New();
    AdvancedTutorialRadioFrame->SetParent (RegistrationFrame->GetFrame());
    AdvancedTutorialRadioFrame->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", AdvancedTutorialRadioFrame->GetWidgetName());

    this->AdvancedTutorialRadioButton = vtkKWRadioButton::New();
    this->AdvancedTutorialRadioButton->SetParent(AdvancedTutorialRadioFrame);
    this->AdvancedTutorialRadioButton->Create();
    this->AdvancedTutorialRadioButton->SetText ("Advanced Tutorial");
    this->AdvancedTutorialRadioButton->SetCompoundModeToLeft();
    this->AdvancedTutorialRadioButton->IndicatorVisibilityOff();
    this->AdvancedTutorialRadioButton->SetValue(ADVANCED_TUTORIAL_RADIO);
    this->AdvancedTutorialRadioButton->SetSelectedState(0);
    this->AdvancedTutorialRadioButton->SetVariableName(this->BasicTutorialRadioButton->GetVariableName());
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", AdvancedTutorialRadioButton->GetWidgetName());

    vtkKWFrameWithLabel *RegistrationPhantomFrameLabel = vtkKWFrameWithLabel::New();
    RegistrationPhantomFrameLabel->SetParent ( RegistrationFrame->GetFrame() );
    RegistrationPhantomFrameLabel->Create ();
    RegistrationPhantomFrameLabel->SetLabelText ("Get phantom coordinates");
        this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                  RegistrationPhantomFrameLabel->GetWidgetName());
    
    vtkKWFrame *RegistrationScanFrame = vtkKWFrame::New();
    RegistrationScanFrame->SetParent (RegistrationPhantomFrameLabel->GetFrame());
    RegistrationScanFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationScanFrame->GetWidgetName());

    vtkKWFrame *RegistrationTitleFrameTop = vtkKWFrame::New();
    RegistrationTitleFrameTop->SetParent (RegistrationPhantomFrameLabel->GetFrame());
    RegistrationTitleFrameTop->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationTitleFrameTop->GetWidgetName());

    vtkKWFrame *RegistrationCanvasFrameTop = vtkKWFrame::New();
    RegistrationCanvasFrameTop->SetParent (RegistrationPhantomFrameLabel->GetFrame());
    RegistrationCanvasFrameTop->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationCanvasFrameTop->GetWidgetName());

    vtkKWFrame *RegistrationTitleFrameBottom = vtkKWFrame::New();
    RegistrationTitleFrameBottom->SetParent (RegistrationPhantomFrameLabel->GetFrame());
    RegistrationTitleFrameBottom->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationTitleFrameBottom->GetWidgetName());

    

    vtkKWFrame *RegistrationCanvasFrameBottom = vtkKWFrame::New();
    RegistrationCanvasFrameBottom->SetParent (RegistrationPhantomFrameLabel->GetFrame());
    RegistrationCanvasFrameBottom->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationCanvasFrameBottom->GetWidgetName());


    vtkKWFrame *RegistrationListAndButtonsFrame = vtkKWFrame::New();
    RegistrationListAndButtonsFrame->SetParent(RegistrationPhantomFrameLabel->GetFrame());
    RegistrationListAndButtonsFrame->Create();
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationListAndButtonsFrame->GetWidgetName());
    




    this->RegistrationScanButton = vtkKWPushButton::New();
    this->RegistrationScanButton->SetParent (RegistrationScanFrame);
    this->RegistrationScanButton->Create();
    this->RegistrationScanButton->SetText ("Scan for phantom coordinates");
    this->RegistrationScanButton->SetWidth (30);
    this->RegistrationScanButton->SetStateToDisabled();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationScanButton->GetWidgetName());

    vtkKWLabel *RegistrationCanvasTitleTop = vtkKWLabel::New();
    RegistrationCanvasTitleTop->SetParent (RegistrationTitleFrameTop);
    RegistrationCanvasTitleTop->Create();
    RegistrationCanvasTitleTop->SetText("Top Distances:");
        this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", RegistrationCanvasTitleTop->GetWidgetName());

    this->RegistrationCanvasTop = vtkKWCanvas::New();
    this->RegistrationCanvasTop->SetParent (RegistrationCanvasFrameTop);
    this->RegistrationCanvasTop->Create();
    this->RegistrationCanvasTop->SetWidth(CANVAS_WIDTH);
    this->RegistrationCanvasTop->SetHeight(CANVAS_HEIGHT);
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->RegistrationCanvasTop->GetWidgetName());

    vtkKWLabel *RegistrationCanvasTitleBottom = vtkKWLabel::New();
    RegistrationCanvasTitleBottom->SetParent (RegistrationTitleFrameBottom);
    RegistrationCanvasTitleBottom->Create();
    RegistrationCanvasTitleBottom->SetText("Bottom Distances:");
        this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", RegistrationCanvasTitleBottom->GetWidgetName());

    this->RegistrationCanvasBottom = vtkKWCanvas::New();
    this->RegistrationCanvasBottom->SetParent (RegistrationCanvasFrameBottom);
    this->RegistrationCanvasBottom->Create();
    this->RegistrationCanvasBottom->SetWidth(CANVAS_WIDTH);
    this->RegistrationCanvasBottom->SetHeight(CANVAS_HEIGHT);
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->RegistrationCanvasBottom->GetWidgetName());

    CreateBasicGraph(this->RegistrationCanvasTop);
    CreateBasicGraph(this->RegistrationCanvasBottom);

    vtkKWLabel *RegistrationListTitle = vtkKWLabel::New();
    RegistrationListTitle->SetParent(RegistrationListAndButtonsFrame);
    RegistrationListTitle->Create();
    RegistrationListTitle->SetText("Robot Coordinates Detected:");
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationListTitle->GetWidgetName());


    this->RegistrationRobotCoordinatesList = vtkKWMultiColumnList::New();
    this->RegistrationRobotCoordinatesList->SetParent(RegistrationListAndButtonsFrame);
    this->RegistrationRobotCoordinatesList->Create();
    this->RegistrationRobotCoordinatesList->SetHeight(NUM_REGISTRATION_POINTS);
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        this->RegistrationRobotCoordinatesList->InsertRow(i);
    }
    this->RegistrationRobotCoordinatesList->AddColumn("Point");
    this->RegistrationRobotCoordinatesList->AddColumn("Robot Coordinates");
    this->RegistrationRobotCoordinatesList->SetColumnWidth(0, 6);
    this->RegistrationRobotCoordinatesList->SetColumnWidth(1, 20);
    for (int i = 0; i < 2; i++)
    {
        this->RegistrationRobotCoordinatesList->SetColumnAlignmentToLeft(i);
        this->RegistrationRobotCoordinatesList->ColumnEditableOff(i);
        this->RegistrationRobotCoordinatesList->ColumnResizableOff(i);
        this->RegistrationRobotCoordinatesList->ColumnStretchableOff(i);
    }
    char iChar[1];
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        sprintf(iChar, "%d", (i+1));
        this->RegistrationRobotCoordinatesList->InsertCellText(i, 0, iChar);
    }
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->RegistrationRobotCoordinatesList->GetWidgetName());

    std::cout << "RegistrationPhantomOKButton" << std::endl;
    this->RegistrationPhantomOKButton = vtkKWPushButton::New();
    this->RegistrationPhantomOKButton->SetParent(RegistrationListAndButtonsFrame);
    this->RegistrationPhantomOKButton->Create();
    this->RegistrationPhantomOKButton->SetText ("OK");
    this->RegistrationPhantomOKButton->SetStateToDisabled();
    this->RegistrationPhantomOKButton->SetWidth (12);

    this->RegistrationPhantomResetButton = vtkKWPushButton::New();
    this->RegistrationPhantomResetButton->SetParent(RegistrationListAndButtonsFrame);
    this->RegistrationPhantomResetButton->Create();
    this->RegistrationPhantomResetButton->SetText ("Reset");
    this->RegistrationPhantomResetButton->SetWidth (12);
    this->RegistrationPhantomResetButton->SetStateToDisabled();
        this->Script("pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationPhantomOKButton->GetWidgetName(), this->RegistrationPhantomResetButton->GetWidgetName());

    // add a point pair 
    vtkKWFrameWithLabel *RegistrationGetImageCoordinatesFrameLabel = vtkKWFrameWithLabel::New();
    RegistrationGetImageCoordinatesFrameLabel->SetParent ( RegistrationFrame->GetFrame() );
    RegistrationGetImageCoordinatesFrameLabel->Create ( );
    RegistrationGetImageCoordinatesFrameLabel->SetLabelText ("Get Image Coordinates");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 RegistrationGetImageCoordinatesFrameLabel->GetWidgetName());

    // registration image coordinates 0
    vtkKWFrame *RegistrationGetImageCoordinatesFrame0 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame0->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame0->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame0->GetWidgetName());

    this->RegistrationImageCoordinatesRadio0 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio0->SetParent(RegistrationGetImageCoordinatesFrame0);
    this->RegistrationImageCoordinatesRadio0->Create();
    this->RegistrationImageCoordinatesRadio0->SetText ("Image Coordinates 1");
    this->RegistrationImageCoordinatesRadio0->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio0->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio0->SetValue(REGISTRATION_RADIO_0);
    this->RegistrationImageCoordinatesRadio0->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio0->SetStateToDisabled();
    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio0->GetWidgetName());

    this->RegistrationImageCoordinatesEntry0 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry0->SetParent(RegistrationGetImageCoordinatesFrame0);
    this->RegistrationImageCoordinatesEntry0->Create();
    this->RegistrationImageCoordinatesEntry0->SetWidth(30);
    this->RegistrationImageCoordinatesEntry0->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry0->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry0->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton0 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton0->SetParent(RegistrationGetImageCoordinatesFrame0);
    this->RegistrationImageCoordinatesOKButton0->Create();
    this->RegistrationImageCoordinatesOKButton0->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton0->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton0->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio0->GetWidgetName(), this->RegistrationImageCoordinatesEntry0->GetWidgetName(), this->RegistrationImageCoordinatesOKButton0->GetWidgetName());

    // registration image coordinates 1
    vtkKWFrame *RegistrationGetImageCoordinatesFrame1 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame1->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame1->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame1->GetWidgetName());

    this->RegistrationImageCoordinatesRadio1 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio1->SetParent(RegistrationGetImageCoordinatesFrame1);
    this->RegistrationImageCoordinatesRadio1->Create();
    this->RegistrationImageCoordinatesRadio1->SetText ("Image Coordinates 2");
    this->RegistrationImageCoordinatesRadio1->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio1->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio1->SetValue(REGISTRATION_RADIO_1);
    this->RegistrationImageCoordinatesRadio1->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio1->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio1->SetStateToDisabled();
    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio1->GetWidgetName());

    this->RegistrationImageCoordinatesEntry1 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry1->SetParent(RegistrationGetImageCoordinatesFrame1);
    this->RegistrationImageCoordinatesEntry1->Create();
    this->RegistrationImageCoordinatesEntry1->SetWidth(30);
    this->RegistrationImageCoordinatesEntry1->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry1->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry1->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton1 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton1->SetParent(RegistrationGetImageCoordinatesFrame1);
    this->RegistrationImageCoordinatesOKButton1->Create();
    this->RegistrationImageCoordinatesOKButton1->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton1->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton1->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio1->GetWidgetName(), this->RegistrationImageCoordinatesEntry1->GetWidgetName(), this->RegistrationImageCoordinatesOKButton1->GetWidgetName());





    // registration image coordinates 2
    vtkKWFrame *RegistrationGetImageCoordinatesFrame2 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame2->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame2->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame2->GetWidgetName());

    this->RegistrationImageCoordinatesRadio2 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio2->SetParent(RegistrationGetImageCoordinatesFrame2);
    this->RegistrationImageCoordinatesRadio2->Create();
    this->RegistrationImageCoordinatesRadio2->SetText ("Image Coordinates 3");
    this->RegistrationImageCoordinatesRadio2->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio2->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio2->SetValue(REGISTRATION_RADIO_2);
    this->RegistrationImageCoordinatesRadio2->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio2->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio2->SetStateToDisabled();

    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio2->GetWidgetName());

    this->RegistrationImageCoordinatesEntry2 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry2->SetParent(RegistrationGetImageCoordinatesFrame2);
    this->RegistrationImageCoordinatesEntry2->Create();
    this->RegistrationImageCoordinatesEntry2->SetWidth(30);
    this->RegistrationImageCoordinatesEntry2->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry2->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry2->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton2 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton2->SetParent(RegistrationGetImageCoordinatesFrame2);
    this->RegistrationImageCoordinatesOKButton2->Create();
    this->RegistrationImageCoordinatesOKButton2->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton2->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton2->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio2->GetWidgetName(), this->RegistrationImageCoordinatesEntry2->GetWidgetName(), this->RegistrationImageCoordinatesOKButton2->GetWidgetName());




    // registration image coordinates 3
    vtkKWFrame *RegistrationGetImageCoordinatesFrame3 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame3->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame3->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame3->GetWidgetName());

    this->RegistrationImageCoordinatesRadio3 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio3->SetParent(RegistrationGetImageCoordinatesFrame3);
    this->RegistrationImageCoordinatesRadio3->Create();
    this->RegistrationImageCoordinatesRadio3->SetText ("Image Coordinates 4");
    this->RegistrationImageCoordinatesRadio3->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio3->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio3->SetValue(REGISTRATION_RADIO_3);
    this->RegistrationImageCoordinatesRadio3->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio3->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio3->SetStateToDisabled();
    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio3->GetWidgetName());

    this->RegistrationImageCoordinatesEntry3 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry3->SetParent(RegistrationGetImageCoordinatesFrame3);
    this->RegistrationImageCoordinatesEntry3->Create();
    this->RegistrationImageCoordinatesEntry3->SetWidth(30);
    this->RegistrationImageCoordinatesEntry3->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry3->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry3->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton3 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton3->SetParent(RegistrationGetImageCoordinatesFrame3);
    this->RegistrationImageCoordinatesOKButton3->Create();
    this->RegistrationImageCoordinatesOKButton3->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton3->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton3->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio3->GetWidgetName(), this->RegistrationImageCoordinatesEntry3->GetWidgetName(), this->RegistrationImageCoordinatesOKButton3->GetWidgetName());








    // registration image coordinates 4
    vtkKWFrame *RegistrationGetImageCoordinatesFrame4 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame4->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame4->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame4->GetWidgetName());

    this->RegistrationImageCoordinatesRadio4 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio4->SetParent(RegistrationGetImageCoordinatesFrame4);
    this->RegistrationImageCoordinatesRadio4->Create();
    this->RegistrationImageCoordinatesRadio4->SetText ("Image Coordinates 5");
    this->RegistrationImageCoordinatesRadio4->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio4->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio4->SetValue(REGISTRATION_RADIO_4);
    this->RegistrationImageCoordinatesRadio4->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio4->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio4->SetStateToDisabled();
    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio4->GetWidgetName());

    this->RegistrationImageCoordinatesEntry4 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry4->SetParent(RegistrationGetImageCoordinatesFrame4);
    this->RegistrationImageCoordinatesEntry4->Create();
    this->RegistrationImageCoordinatesEntry4->SetWidth(30);
    this->RegistrationImageCoordinatesEntry4->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry4->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry4->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton4 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton4->SetParent(RegistrationGetImageCoordinatesFrame4);
    this->RegistrationImageCoordinatesOKButton4->Create();
    this->RegistrationImageCoordinatesOKButton4->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton4->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton4->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio4->GetWidgetName(), this->RegistrationImageCoordinatesEntry4->GetWidgetName(), this->RegistrationImageCoordinatesOKButton4->GetWidgetName());








    // registration image coordinates 5
    vtkKWFrame *RegistrationGetImageCoordinatesFrame5 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame5->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame5->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame5->GetWidgetName());

    this->RegistrationImageCoordinatesRadio5 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio5->SetParent(RegistrationGetImageCoordinatesFrame5);
    this->RegistrationImageCoordinatesRadio5->Create();
    this->RegistrationImageCoordinatesRadio5->SetText ("Image Coordinates 6");
    this->RegistrationImageCoordinatesRadio5->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio5->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio5->SetValue(REGISTRATION_RADIO_5);
    this->RegistrationImageCoordinatesRadio5->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio5->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio5->SetStateToDisabled();

    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio5->GetWidgetName());

    this->RegistrationImageCoordinatesEntry5 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry5->SetParent(RegistrationGetImageCoordinatesFrame5);
    this->RegistrationImageCoordinatesEntry5->Create();
    this->RegistrationImageCoordinatesEntry5->SetWidth(30);
    this->RegistrationImageCoordinatesEntry5->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry5->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry5->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton5 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton5->SetParent(RegistrationGetImageCoordinatesFrame5);
    this->RegistrationImageCoordinatesOKButton5->Create();
    this->RegistrationImageCoordinatesOKButton5->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton5->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton5->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio5->GetWidgetName(), this->RegistrationImageCoordinatesEntry5->GetWidgetName(), this->RegistrationImageCoordinatesOKButton5->GetWidgetName());


    // registration image coordinates 6
    vtkKWFrame *RegistrationGetImageCoordinatesFrame6 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame6->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame6->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame6->GetWidgetName());

    this->RegistrationImageCoordinatesRadio6 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio6->SetParent(RegistrationGetImageCoordinatesFrame6);
    this->RegistrationImageCoordinatesRadio6->Create();
    this->RegistrationImageCoordinatesRadio6->SetText ("Image Coordinates 7");
    this->RegistrationImageCoordinatesRadio6->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio6->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio6->SetValue(REGISTRATION_RADIO_6);
    this->RegistrationImageCoordinatesRadio6->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio6->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio6->SetStateToDisabled();

    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio6->GetWidgetName());

    this->RegistrationImageCoordinatesEntry6 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry6->SetParent(RegistrationGetImageCoordinatesFrame6);
    this->RegistrationImageCoordinatesEntry6->Create();
    this->RegistrationImageCoordinatesEntry6->SetWidth(30);
    this->RegistrationImageCoordinatesEntry6->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry6->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry6->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton6 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton6->SetParent(RegistrationGetImageCoordinatesFrame6);
    this->RegistrationImageCoordinatesOKButton6->Create();
    this->RegistrationImageCoordinatesOKButton6->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton6->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton6->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio6->GetWidgetName(), this->RegistrationImageCoordinatesEntry6->GetWidgetName(), this->RegistrationImageCoordinatesOKButton6->GetWidgetName());





    // registration image coordinates 7
    vtkKWFrame *RegistrationGetImageCoordinatesFrame7 = vtkKWFrame::New();
    RegistrationGetImageCoordinatesFrame7->SetParent ( RegistrationGetImageCoordinatesFrameLabel->GetFrame() );
    RegistrationGetImageCoordinatesFrame7->Create ( );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               RegistrationGetImageCoordinatesFrame7->GetWidgetName());

    this->RegistrationImageCoordinatesRadio7 = vtkKWRadioButton::New();
    this->RegistrationImageCoordinatesRadio7->SetParent(RegistrationGetImageCoordinatesFrame7);
    this->RegistrationImageCoordinatesRadio7->Create();
    this->RegistrationImageCoordinatesRadio7->SetText ("Image Coordinates 8");
    this->RegistrationImageCoordinatesRadio7->SetCompoundModeToLeft();
    this->RegistrationImageCoordinatesRadio7->IndicatorVisibilityOff();
    this->RegistrationImageCoordinatesRadio7->SetValue(REGISTRATION_RADIO_7);
    this->RegistrationImageCoordinatesRadio7->SetSelectedState(0);
    this->RegistrationImageCoordinatesRadio7->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    this->RegistrationImageCoordinatesRadio7->SetStateToDisabled();

    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", RegistrationImageCoordinatesRadio7->GetWidgetName());

    this->RegistrationImageCoordinatesEntry7 = vtkKWEntryWithLabel::New();
    this->RegistrationImageCoordinatesEntry7->SetParent(RegistrationGetImageCoordinatesFrame7);
    this->RegistrationImageCoordinatesEntry7->Create();
    this->RegistrationImageCoordinatesEntry7->SetWidth(30);
    this->RegistrationImageCoordinatesEntry7->SetLabelWidth(0);
    //this->RegistrationImageCoordinatesEntry->SetLabelText("Image Coordinates:");
    this->RegistrationImageCoordinatesEntry7->GetWidget()->SetValue ( "" );
    //this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->RegistrationImageCoordinatesEntry7->GetWidgetName());

    this->RegistrationImageCoordinatesOKButton7 = vtkKWPushButton::New();
    this->RegistrationImageCoordinatesOKButton7->SetParent(RegistrationGetImageCoordinatesFrame7);
    this->RegistrationImageCoordinatesOKButton7->Create();
    this->RegistrationImageCoordinatesOKButton7->SetText( "OK" );
    this->RegistrationImageCoordinatesOKButton7->SetWidth ( 12 );
    this->RegistrationImageCoordinatesOKButton7->SetStateToDisabled();
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->RegistrationImageCoordinatesRadio7->GetWidgetName(), this->RegistrationImageCoordinatesEntry7->GetWidgetName(), this->RegistrationImageCoordinatesOKButton7->GetWidgetName());


    // add a point pair 
    vtkKWFrameWithLabel *SummaryListFrame = vtkKWFrameWithLabel::New();
    SummaryListFrame->SetParent ( RegistrationFrame->GetFrame() );
    SummaryListFrame->Create ( );
    SummaryListFrame->SetLabelText ("Summary");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 SummaryListFrame->GetWidgetName());



    this->SummaryList = vtkKWMultiColumnList::New();
    this->SummaryList->SetParent(SummaryListFrame->GetFrame());
    this->SummaryList->Create();
    this->SummaryList->SetHeight(NUM_REGISTRATION_POINTS);
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        this->SummaryList->InsertRow(i);
    }
    this->SummaryList->AddColumn("Point");
    this->SummaryList->AddColumn("Robot Coordinates");
    this->SummaryList->AddColumn("Image Coordinates");
    this->SummaryList->SetColumnWidth(0, 6);
    this->SummaryList->SetColumnWidth(1, 20);
    this->SummaryList->SetColumnWidth(2, 20);
    for (int i = 0; i < 3; i++)
    {
        this->SummaryList->SetColumnAlignmentToLeft(i);
        this->SummaryList->ColumnEditableOff(i);
        this->SummaryList->ColumnResizableOff(i);
        this->SummaryList->ColumnStretchableOff(i);
    }
//    char iChar[1];
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        sprintf(iChar, "%d", (i+1));
        this->SummaryList->InsertCellText(i, 0, iChar);
    }

        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 SummaryList->GetWidgetName());





}

void vtkLegoGUI::CreateBasicGraph(vtkKWCanvas* registrationCanvas)
{
    // clear the canvas, in case this is the second time that this was called.
    this->Script("%s delete all", registrationCanvas->GetWidgetName());


    this->Script("%s create rectangle %d %d %d %d -outline #000000 -width 1", registrationCanvas->GetWidgetName(), RECTANGLE_START_X, RECTANGLE_START_Y, (RECTANGLE_START_X + RECTANGLE_WIDTH), (RECTANGLE_START_Y + RECTANGLE_HEIGHT));

    //ticks

    for (int i = 1; i < NUM_TICK_SECTIONS; i++)
    {
        this->Script("%s create line %d %d %d %d -width 1 -fill #000000", registrationCanvas->GetWidgetName(), RECTANGLE_START_X, (RECTANGLE_START_Y + ((RECTANGLE_HEIGHT / NUM_TICK_SECTIONS) * i)), (RECTANGLE_START_X + 7), (RECTANGLE_START_Y + (RECTANGLE_HEIGHT / NUM_TICK_SECTIONS) * i));
        
        this->Script("%s create line %d %d %d %d -width 1 -fill #000000", registrationCanvas->GetWidgetName(), (RECTANGLE_START_X + ((RECTANGLE_WIDTH / NUM_TICK_SECTIONS) * i)), (RECTANGLE_START_Y + RECTANGLE_HEIGHT - 7), (RECTANGLE_START_X + ((RECTANGLE_WIDTH / NUM_TICK_SECTIONS) * i)), (RECTANGLE_START_Y + RECTANGLE_HEIGHT));
    }


    this->Script("%s create text %d %d -anchor nw -fill black -text %s -tags st", registrationCanvas->GetWidgetName(), 2, 90, "distance");

    this->Script("%s create text %d %d -anchor nw -fill black -text %s -tags st", registrationCanvas->GetWidgetName(), (RECTANGLE_START_X + 102), (RECTANGLE_START_Y + RECTANGLE_HEIGHT + 30), "motorA");


    // numbers
    // y axis
    int count = 0;
    for (int i = MIN_US_DISTANCE_DISPLAY; i <= MAX_US_DISTANCE_DISPLAY; i = i + ((MAX_US_DISTANCE_DISPLAY - MIN_US_DISTANCE_DISPLAY) / NUM_TICK_SECTIONS))
    {

        this->Script("%s create text %d %d -anchor nw -fill black -text %d -tags st", registrationCanvas->GetWidgetName(), (RECTANGLE_START_X - 20), (RECTANGLE_START_Y + ((RECTANGLE_HEIGHT / NUM_TICK_SECTIONS) * (5-count)) - 7), i);
        count++;
    }


    // x axis
    count = 0;
    for (int i = (-1 * REGISTRATION_ROTATIONS_MOTOR_A); i <= REGISTRATION_ROTATIONS_MOTOR_A; i = i + (REGISTRATION_ROTATIONS_MOTOR_A * 2 / NUM_TICK_SECTIONS))
    {
        this->Script("%s create text %d %d -anchor nw -fill black -text %d -tags st", registrationCanvas->GetWidgetName(), (RECTANGLE_START_X + ((RECTANGLE_WIDTH / NUM_TICK_SECTIONS) * count)-10), (RECTANGLE_START_Y + RECTANGLE_HEIGHT + 7), i);
        count++;
    }



    

}


// assumes sorted by time
// n x 3:  [x, y, special?]
// special is 1 if special, 0 otherwise
//---------------------------------------------------------------------------
void vtkLegoGUI::DisplayUltrasoundDistancesOnGraph(int n, int** pts, vtkKWCanvas* registrationCanvas)
{
    //TODO: error checking here

    std::cout << "displaying" << std::endl;


    CreateBasicGraph(registrationCanvas);



    // find minY, maxY
    //int minY = pts[0][1];
    //int maxY = pts[0][1];
    int minY = MIN_US_DISTANCE_DISPLAY;
    int maxY = MAX_US_DISTANCE_DISPLAY;

    /*for (int i = 1; i < n; i++)
    {
        if (pts[i][1] < minY)
        {
            minY = pts[i][1];
        }
        if (pts[i][1] > maxY)
        {
            maxY = pts[i][1];
        }
    }
*/


    //int dataWidth = ((pts[n-1][0] - pts[0][0]) / NUM_TICK_SECTIONS + 1)*NUM_TICK_SECTIONS;
    //int dataHeight = ((maxY - minY) / NUM_TICK_SECTIONS + 1)*NUM_TICK_SECTIONS;
    //int xTickInterval = dataWidth / NUM_TICK_SECTIONS;
    //int yTickInterval = dataHeight / NUM_TICK_SECTIONS;

    int x, y, special;

    for (int i = n-1; i >=0 ; i--)
    {

        x = RECTANGLE_START_X + (((pts[i][0] + REGISTRATION_ROTATIONS_MOTOR_A) * RECTANGLE_WIDTH) / (REGISTRATION_ROTATIONS_MOTOR_A * 2));


        y = (((MIN_US_DISTANCE_DISPLAY - pts[i][1]) * RECTANGLE_HEIGHT) / (MAX_US_DISTANCE_DISPLAY - MIN_US_DISTANCE_DISPLAY)) + RECTANGLE_START_Y + RECTANGLE_HEIGHT;


        //x = RECTANGLE_START_X + (pts[i][0] *RECTANGLE_WIDTH / (REGISTRATION_ROTATIONS_MOTOR_A * 2));
        //y = RECTANGLE_START_Y + RECTANGLE_HEIGHT - (pts[i][1] * RECTANGLE_HEIGHT / MAX_US_DISTANCE);
        special = pts[i][2];

        // draw lines
        /*if (i != 0)
        {

            prev_x = RECTANGLE_START_X + (pts[i-1][0] * RECTANGLE_WIDTH / dataWidth);
            prev_y = RECTANGLE_START_Y + RECTANGLE_HEIGHT - (pts[i-1][1] * RECTANGLE_HEIGHT / dataHeight);
            
            this->Script("%s create line %d %d %d %d -width 1 -fill #000000", this->registrationCanvas->GetWidgetName(), prev_x, prev_y, x, y);
        }*/

        // draw points
        if ((pts[i][1] >= MIN_US_DISTANCE_DISPLAY) && (pts[i][1] <= MAX_US_DISTANCE_DISPLAY)) // ignore error points
        {
            if (special)
            {
                this->Script("%s create oval %d %d %d %d -outline #000000 -fill red -width 1", registrationCanvas->GetWidgetName(), (x-4), (y-4), (x+4), (y+4));
            }
            else
            {
                this->Script("%s create oval %d %d %d %d -outline #000000 -fill black -width 1", registrationCanvas->GetWidgetName(), (x-1), (y-1), (x+1), (y+1));
            }
        }
    }
}

//---------------------------------------------------------------------------
void vtkLegoGUI::BuildGUIForPlanningFrame ()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );

    vtkSlicerModuleCollapsibleFrame *PlanningFrame = vtkSlicerModuleCollapsibleFrame::New();
    PlanningFrame->SetParent(page);
    PlanningFrame->Create();
    PlanningFrame->SetLabelText ("Planning");
    PlanningFrame->CollapseFrame();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            PlanningFrame->GetWidgetName(), page->GetWidgetName());

    vtkKWFrameWithLabel *PlanningFrameLabel = vtkKWFrameWithLabel::New();
    PlanningFrameLabel->SetParent(PlanningFrame->GetFrame());
    PlanningFrameLabel->Create();
    PlanningFrameLabel->SetLabelText ("Get Target Coordinates");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 PlanningFrameLabel->GetWidgetName());

    vtkKWFrame *PlanningRegistrationPointPairListButtonFrame = vtkKWFrame::New();
    PlanningRegistrationPointPairListButtonFrame->SetParent (PlanningFrameLabel->GetFrame());
    PlanningRegistrationPointPairListButtonFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 PlanningRegistrationPointPairListButtonFrame->GetWidgetName());

    this->TargetCoordinatesRadio = vtkKWRadioButton::New();
    this->TargetCoordinatesRadio->SetParent(PlanningRegistrationPointPairListButtonFrame);
    this->TargetCoordinatesRadio->Create();
    this->TargetCoordinatesRadio->SetText("Target coordinates");
    this->TargetCoordinatesRadio->SetCompoundModeToLeft();
    this->TargetCoordinatesRadio->IndicatorVisibilityOff();
    this->TargetCoordinatesRadio->SetValue(TARGET_RADIO);
    this->TargetCoordinatesRadio->SetSelectedState(0);
    this->TargetCoordinatesRadio->SetVariableName(this->RegistrationImageCoordinatesRadio0->GetVariableName());
    //this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", TargetCoordinatesRadio->GetWidgetName());

    this->TargetCoordinatesEntry = vtkKWEntryWithLabel::New();
    this->TargetCoordinatesEntry->SetParent (PlanningRegistrationPointPairListButtonFrame);
    this->TargetCoordinatesEntry->Create();
    this->TargetCoordinatesEntry->SetWidth(30);
    this->TargetCoordinatesEntry->SetLabelWidth(16);
    this->TargetCoordinatesEntry->SetLabelText("Target Coordinates");

    this->AddTargetPushButton = vtkKWPushButton::New();
    this->AddTargetPushButton->SetParent(PlanningRegistrationPointPairListButtonFrame);
    this->AddTargetPushButton->Create();
    this->AddTargetPushButton->SetText ("OK");
    this->AddTargetPushButton->SetWidth(12);
    this->Script( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                 this->TargetCoordinatesRadio->GetWidgetName(), this->TargetCoordinatesEntry->GetWidgetName(), this->AddTargetPushButton->GetWidgetName());
}

//---------------------------------------------------------------------------
void vtkLegoGUI::BuildGUIForBiopsyFrame()
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Lego" );





    vtkSlicerModuleCollapsibleFrame *ExecuteBiopsyFrame = vtkSlicerModuleCollapsibleFrame::New();
    ExecuteBiopsyFrame->SetParent(page);
    ExecuteBiopsyFrame->Create();
    ExecuteBiopsyFrame->SetLabelText ("Execute Biopsy");
    ExecuteBiopsyFrame->CollapseFrame();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            ExecuteBiopsyFrame->GetWidgetName(), page->GetWidgetName());


    vtkKWFrameWithLabel *SummaryFrameWithLabel = vtkKWFrameWithLabel::New();
    SummaryFrameWithLabel->SetParent(ExecuteBiopsyFrame->GetFrame());
    SummaryFrameWithLabel->Create();
    SummaryFrameWithLabel->SetLabelText ("Summary");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 SummaryFrameWithLabel->GetWidgetName());

    vtkKWFrameWithLabel *ExecuteBiopsyResultsFrame = vtkKWFrameWithLabel::New();
    ExecuteBiopsyResultsFrame->SetParent(ExecuteBiopsyFrame->GetFrame());
    ExecuteBiopsyResultsFrame->Create();
    ExecuteBiopsyResultsFrame->SetLabelText ("Execute the Biopsy");
    this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 ExecuteBiopsyResultsFrame->GetWidgetName());





    vtkKWFrame *ExecuteBiopsyListFrame = vtkKWFrame::New();
    ExecuteBiopsyListFrame->SetParent (SummaryFrameWithLabel->GetFrame());
    ExecuteBiopsyListFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 ExecuteBiopsyListFrame->GetWidgetName());


    vtkKWFrame *SummaryImageTargetCoordinatesFrame = vtkKWFrame::New();
    SummaryImageTargetCoordinatesFrame->SetParent (SummaryFrameWithLabel->GetFrame());
    SummaryImageTargetCoordinatesFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 SummaryImageTargetCoordinatesFrame->GetWidgetName());



    vtkKWFrame *ExecuteBiopsyRegistrationPointPairListButtonFrame = vtkKWFrame::New();
    ExecuteBiopsyRegistrationPointPairListButtonFrame->SetParent (ExecuteBiopsyResultsFrame->GetFrame());
    ExecuteBiopsyRegistrationPointPairListButtonFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 ExecuteBiopsyRegistrationPointPairListButtonFrame->GetWidgetName());

    vtkKWFrame *SummaryRobotTargetCoordinatesFrame = vtkKWFrame::New();
    SummaryRobotTargetCoordinatesFrame->SetParent (ExecuteBiopsyResultsFrame->GetFrame());
    SummaryRobotTargetCoordinatesFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 SummaryRobotTargetCoordinatesFrame->GetWidgetName());

    vtkKWFrame *SummaryRobotFinalCoordinatesFrame = vtkKWFrame::New();
    SummaryRobotFinalCoordinatesFrame->SetParent (ExecuteBiopsyResultsFrame->GetFrame());
    SummaryRobotFinalCoordinatesFrame->Create();
        this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 SummaryRobotFinalCoordinatesFrame->GetWidgetName());


    vtkKWLabel* SummaryRegistrationMatrixLabel = vtkKWLabel::New();
    SummaryRegistrationMatrixLabel->SetParent(ExecuteBiopsyListFrame);
    SummaryRegistrationMatrixLabel->Create();
    SummaryRegistrationMatrixLabel->SetText("Registration Matrix:");
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",  SummaryRegistrationMatrixLabel->GetWidgetName());


    this->SummaryRegistrationMatrixList = vtkKWMultiColumnList::New();
    this->SummaryRegistrationMatrixList->SetParent(ExecuteBiopsyListFrame);
    this->SummaryRegistrationMatrixList->Create();
    this->SummaryRegistrationMatrixList->SetHeight(3);
    for (int i = 0; i < 4; i++)
    {
        this->SummaryRegistrationMatrixList->InsertRow(i);
    }
    for (int i = 0; i < 4; i++)
    {
        this->SummaryRegistrationMatrixList->AddColumn("");
        this->SummaryRegistrationMatrixList->SetColumnWidth(i, 10);
        this->SummaryRegistrationMatrixList->SetColumnAlignmentToLeft(i);
        this->SummaryRegistrationMatrixList->ColumnEditableOff(i);
        this->SummaryRegistrationMatrixList->ColumnResizableOff(i);
        this->SummaryRegistrationMatrixList->ColumnStretchableOff(i);
    }
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  this->SummaryRegistrationMatrixList->GetWidgetName());








    vtkKWLabel* SummaryTargetImageCoordinateTextLabel = vtkKWLabel::New();
    SummaryTargetImageCoordinateTextLabel->SetParent(SummaryImageTargetCoordinatesFrame);
    SummaryTargetImageCoordinateTextLabel->Create();
    SummaryTargetImageCoordinateTextLabel->SetText("Target in Image Coordinates:");

    this->SummaryTargetImageCoordinateText = vtkKWText::New();
    this->SummaryTargetImageCoordinateText->SetParent(SummaryImageTargetCoordinatesFrame);
    this->SummaryTargetImageCoordinateText->Create();
    this->SummaryTargetImageCoordinateText->SetWidth(20);
    this->SummaryTargetImageCoordinateText->SetHeight(1);
    this->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",  SummaryTargetImageCoordinateTextLabel->GetWidgetName(), this->SummaryTargetImageCoordinateText->GetWidgetName());








    this->ExecuteBiopsyPushButton = vtkKWPushButton::New();
    this->ExecuteBiopsyPushButton->SetParent(ExecuteBiopsyRegistrationPointPairListButtonFrame);
    this->ExecuteBiopsyPushButton->Create();
    this->ExecuteBiopsyPushButton->SetText ("Execute the Biopsy");
    this->ExecuteBiopsyPushButton->SetWidth(20);
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                 this->ExecuteBiopsyPushButton->GetWidgetName());

    
    vtkKWLabel* SummaryTargetRobotCoordinateTextLabel = vtkKWLabel::New();
    SummaryTargetRobotCoordinateTextLabel->SetParent(SummaryRobotTargetCoordinatesFrame);
    SummaryTargetRobotCoordinateTextLabel->Create();
    SummaryTargetRobotCoordinateTextLabel->SetText("Target in Robot Coordinates:");

    this->SummaryTargetRobotCoordinateText = vtkKWText::New();
    this->SummaryTargetRobotCoordinateText->SetParent(SummaryRobotTargetCoordinatesFrame);
    this->SummaryTargetRobotCoordinateText->Create();
    this->SummaryTargetRobotCoordinateText->SetWidth(20);
    this->SummaryTargetRobotCoordinateText->SetHeight(1);
    this->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",  SummaryTargetRobotCoordinateTextLabel->GetWidgetName(), this->SummaryTargetRobotCoordinateText->GetWidgetName());


    vtkKWLabel* SummaryFinalRobotCoordinateTextLabel = vtkKWLabel::New();
    SummaryFinalRobotCoordinateTextLabel->SetParent(SummaryRobotFinalCoordinatesFrame);
    SummaryFinalRobotCoordinateTextLabel->Create();
    SummaryFinalRobotCoordinateTextLabel->SetText("Result in Robot Coordinates:");

    this->SummaryFinalRobotCoordinateText = vtkKWText::New();
    this->SummaryFinalRobotCoordinateText->SetParent(SummaryRobotFinalCoordinatesFrame);
    this->SummaryFinalRobotCoordinateText->Create();
    this->SummaryFinalRobotCoordinateText->SetWidth(20);
    this->SummaryFinalRobotCoordinateText->SetHeight(1);
    this->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",  SummaryFinalRobotCoordinateTextLabel->GetWidgetName(), this->SummaryFinalRobotCoordinateText->GetWidgetName());





}

int vtkLegoGUI::ConnectToLegoDevice()
{
    if (this->Connected)
    {
        return 0; 
    }

    // attempt to connect
    int open = this->LegoUSB->OpenLegoUSB();

    char text[256];
    // connection failed
    if (open == 0) 
    {
        sprintf(text, "Could not connect: %s\nDevice Found:", this->LegoUSB->GetStatus());
        this->LegoConnectLabel->SetText(text);
        this->LegoConnectCheckButton->SelectedStateOff();

        std::cout << open << std::endl;
        return open;
    }

    // connection succeeded
    this->Connected = true;

    sprintf(text, "Connection successful!\nDevice Found: %s: ID: %x:%x", this->LegoUSB->GetDeviceFilename(), this->LegoUSB->GetIDVendor(), this->LegoUSB->GetIDProduct());
    std::cout << std::endl;
    for (int i = 0; i < 256; i++)
    {
        if (text[i] == '\0') {break;}
        std::cout << text[i];
    }
    std::cout << std::endl;
    this->LegoConnectLabel->SetText(text);
    return open;
}

int vtkLegoGUI::SetupLegoDevice()
{

    if (!this->Connected)
    {
        return 0;
    }

    this->LegoUSB->SetSensorTouch(IN_1);
    this->LegoUSB->SetSensorUS(IN_4);
    this->LegoUSB->ResetMotorPosition(OUT_A, true);
    this->LegoUSB->ResetMotorPosition(OUT_A, false);
    this->LegoUSB->ResetMotorPosition(OUT_B, true);
    this->LegoUSB->ResetMotorPosition(OUT_B, false);
    this->LegoUSB->ResetMotorPosition(OUT_C, true);
    this->LegoUSB->ResetMotorPosition(OUT_C, false);


    return 1;
}











// returns if error
// assumes starting at 0,0,0
int vtkLegoGUI::SwipeForRegistrationPoints(int motorBDistance, int maxUSDistance, int ** swipeDistances)
{

    // error checking
    if (!this->Connected)
    {
        return -1;
    }

    // move arm forward and to the right
    int powerB = GetPower(motorBDistance);
    int powerA = GetPower(REGISTRATION_ROTATIONS_MOTOR_A);
    this->LegoUSB->MoveMotor(OUT_B, (-1 * powerB), motorBDistance);
    //this->LegoUSB->MoveMotor(OUT_A, powerA, REGISTRATION_ROTATIONS_MOTOR_A);
    this->LegoUSB->MoveMotor(OUT_A, (-1 * powerA), REGISTRATION_ROTATIONS_MOTOR_A);

    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        swipeDistances[i][0] = i - REGISTRATION_ROTATIONS_MOTOR_A; // motorA position
        swipeDistances[i][1] = -1; // default distance = unknown / error
        swipeDistances[i][2] = 0; // point is not special
    }

    int motorA, /*motorB,*/ distance;
    motorA = this->LegoUSB->GetMotorRotation(OUT_A, false);
    std::cout << motorA << std::endl;
    //this->LegoUSB->SetMotorOn(OUT_A, -20);
    this->LegoUSB->SetMotorOn(OUT_A, 20);
    
    //while (motorA > (REGISTRATION_ROTATIONS_MOTOR_A * -1))
    while (motorA < REGISTRATION_ROTATIONS_MOTOR_A)
    {
        motorA = this->LegoUSB->GetMotorRotation(OUT_A, false);
        distance = this->LegoUSB->GetUSSensor(IN_4);

        //if (motorA > (REGISTRATION_ROTATIONS_MOTOR_A * -1))
        if ((motorA < REGISTRATION_ROTATIONS_MOTOR_A) && (motorA > (REGISTRATION_ROTATIONS_MOTOR_A * -1)))
        {
            swipeDistances[(motorA + REGISTRATION_ROTATIONS_MOTOR_A)][1] = distance;
            //std::cout << (motorA + REGISTRATION_ROTATIONS_MOTOR_A) << " " << motorA << " " << distance << std::endl;

        }
    }
    this->LegoUSB->StopMotor(OUT_A, true);

    //this->DisplayUltrasoundDistancesOnGraph(NUM_REGISTRATION_POINTS_MOTOR_A, swipeDistances);


    //this->LegoUSB->MoveMotor(OUT_A, powerA, REGISTRATION_ROTATIONS_MOTOR_A);
    this->LegoUSB->MoveMotor(OUT_A, (-1 * powerA), REGISTRATION_ROTATIONS_MOTOR_A);
    int motorBDistanceUp = (int) this->DownToUpMotorB((double)motorBDistance);
    powerB = this->GetPower(motorBDistance);
    this->LegoUSB->MoveMotor(OUT_B, powerB, motorBDistanceUp);


    int numNotError = 0;
    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        if (swipeDistances[i][1] >= MIN_US_DISTANCE && swipeDistances[i][1] <= maxUSDistance)
        {
            numNotError++;
        }
    }



    return numNotError;
}

double vtkLegoGUI::DownToUpMotorB (double motorBDown)
{
    double motorBUp;
    if (fabs(motorBDown) > 700)
    {
        motorBUp = motorBDown * 1.1;
    }
    else
    {
        motorBUp = motorBDown;
    }
    return motorBUp;
}    


// TODO error checking
// returns numNotError
void vtkLegoGUI::PreprocessDistances (int filterWidth, int maxUSDistance, int ** origDistances, int ** goodDistances, int ** filteredDistances)
{

    int numNotError = 0;
    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        if (origDistances[i][1] >= MIN_US_DISTANCE && origDistances[i][1] <= maxUSDistance)
        {
            numNotError++;
        }
    }


    // extract points with good values (not -1, not too large or zero either)

    //goodDistances = new int*[numNotError];
    for (int i = 0; i < numNotError; i++)
    {
        //*(goodDistances+i) = new int[3];
        goodDistances[i][2] = 0; // not special
        
    }
    int counter = 0;
    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        if (origDistances[i][1] >= MIN_US_DISTANCE && origDistances[i][1] <= maxUSDistance)
        {
            goodDistances[counter][0] = origDistances[i][0];
            goodDistances[counter][1] = origDistances[i][1];
            counter++;
        }
    }

    // median filter
    //filteredDistances = new int*[numNotError];
    for (int i = 0; i < numNotError; i++)
    {
        //*(filteredDistances + i) = new int[3];
        filteredDistances[i][0] = goodDistances[i][0];
        filteredDistances[i][2] = 0; // not special
    }

    MedianFilter(goodDistances, filteredDistances, numNotError, filterWidth);


}

//TODO error checking for size of pts, etc
// returns -1 on error
void vtkLegoGUI::FindMotorAValuesForRobotRegistrationCoordinatesTopSwipe(int** distances, int numNotError)
{

    std::cout << "at beginning " << std::endl;
    int ** goodPoints;
    goodPoints = new int*[numNotError];
    int ** filteredPoints;
    filteredPoints = new int*[numNotError];
    for (int i = 0; i < numNotError; i++)
    {
        *(goodPoints + i) = new int[3];
        *(filteredPoints+i) = new int[3];
    }

    std::cout << "preprocess distances" << std::endl;
    this->PreprocessDistances(FILTER_WIDTH_TOP_SWIPE, MAX_US_DISTANCE_TOP, distances, goodPoints, filteredPoints);




    //this->PreprocessDistances(31, MAX_US_DISTANCE_TOP, distances, goodPoints, filteredPoints);



    int numSections = 0;
    for (int i = 0; i < numNotError-1; i++)
    {
        if (filteredPoints[i][1] != filteredPoints[i+1][1])
        {
            numSections++;
        }
    }
    // check to see if the last one is unique
    //if (filteredPoints[numNotError-2][1] != filteredPoints[numNotError-1][1])
    //{
        numSections++;
    //}

    std::cout << "numsections"  << numSections << std::endl;

    int ** sections;
    sections = new int*[numSections];
    for (int i = 0; i < numSections; i++)
    {
        *(sections+i) = new int[3];
    }
    int counter = 0;
    int firstIndex = 0;
    for (int i = 0; i < numNotError-1; i++)
    {
        if (filteredPoints[i][1] != filteredPoints[i+1][1])
        {
            sections[counter][0] = (filteredPoints[firstIndex][0] + filteredPoints[i][0]) / 2;
            sections[counter][1] = (filteredPoints[firstIndex][1] + filteredPoints[i][1]) / 2;
            sections[counter][2] = (firstIndex + i) / 2;

            firstIndex = i+1;
            counter++;
            
        }
    }
    // check for the last one
    // last one is unique
    /*if (filteredPoints[numNotError-2][1] != filteredPoints[numNotError-1][1])
    {
        sections[numSections-1][0] = filteredPoints[numNotError-1][0];
        sections[numSections-1][1] = filteredPoints[numNotError-1][1];
    }
    // last one is part of a section
    else
    {*/
        sections[numSections-1][0] = (filteredPoints[firstIndex][0] + filteredPoints[numNotError-1][0]) / 2;
        sections[numSections-1][1] = (filteredPoints[firstIndex][1] + filteredPoints[numNotError-1][1]) / 2;
    //}

    int numLocalMinima = 0;
    for (int i = 1; i < numSections-1; i++)
    {
    

        if (((sections[i][1] - sections[i-1][1]) < 0) && ((sections[i+1][1] - sections[i][1]) > 0))
        {
            numLocalMinima++;
            //sections[i][2] = 1;
            //goodPoints[(sections[i][2])][2] = 1;
            
        }
    }

    int thisNumLocalMinima = 0;
    int middleIndex, sectionIndex;
    double motorA;
    double * registrationCoordinates;
    registrationCoordinates = new double[3];

    std::cout << "numLocalMinima " << numLocalMinima << std::endl;

    for (int i = 1; i < numSections-1; i++)
    {
        if (((sections[i][1] - sections[i-1][1]) < 0) && ((sections[i+1][1] - sections[i][1]) > 0))
        {
    
            

            if (thisNumLocalMinima == 0 || thisNumLocalMinima == (numLocalMinima -1))
            {

                sectionIndex = i; // save for if we overshot a boundary
                middleIndex = sections[i][2];
                motorA = (double) goodPoints[middleIndex][0];

                if (thisNumLocalMinima == 0)
                {
                    this->MotorAValuesForRobotRegistrationCoordinates[0] = motorA;
                }
                else
                {
                    this->MotorAValuesForRobotRegistrationCoordinates[1] = motorA;
                }
                goodPoints[middleIndex][2] = 1;

                std::cout << thisNumLocalMinima << " " << motorA << std::endl;

            }    
            thisNumLocalMinima++;                    
        }
    }


    // overshot the boundary - need to get the second local minima
    if (numLocalMinima == 1)
    {

        int tip; // the index
        double motorAtip;
        double newMotorA;
        double distance;

        // found left pillar before - now we have to find the right one
        if (motorA < 0)
        {

            // find the tip
            for (int j = sectionIndex; j < numSections -1; j++)
            {    
                if (sections[j+1][1] < sections[j][1])
                {
                    tip = sections[j][2];
                    break;
                }
            }

            std::cout << "found left pillar" << std::endl;
            distance = ((double) goodPoints[tip][0]) - ((double) goodPoints[0][0]);
            if (distance < MIN_MOTOR_A)
            {
                distance = MIN_MOTOR_A;
            }
            newMotorA = motorA + distance;
        }
        // found right pillar
        else
        {

            // find the tip
            for (int j = sectionIndex; j > 0; j--)
            {    
                if (sections[j-1][1] < sections[j][1])
                {
                    tip = sections[j][2];
                    break;
                }
            }


            std::cout << "found right pillar" << std::endl;
            distance = ((double) goodPoints[numNotError-1][0]) -  ((double) goodPoints[tip][0]);
            if (distance > MAX_MOTOR_A)
            {
                distance = MAX_MOTOR_A;
            }
            
            newMotorA = motorA - distance;
        }

        std::cout << "distance = " << distance << std::endl;

        // find the point in good points that corresponds to this motor
        double minDifference = 100000; // something huge to start off
        int goodIndex = 0;
        for (int j = 0; j < numNotError; j++)
        {
            if (fabs(goodPoints[j][0] - newMotorA) < minDifference)
            {
                minDifference = fabs(goodPoints[j][0] - newMotorA);
                goodIndex = j;
            }
        }

        std::cout << "----> motorA = " << newMotorA << std::endl;
        std::cout << "goodIndex = " << goodIndex << std::endl;

        goodPoints[goodIndex][2] = 1;

        // found left pillar - want to put this as the right pillar
        if (motorA < 1)
        {
            this->MotorAValuesForRobotRegistrationCoordinates[1] = newMotorA;
        }
        // found right pillar - want to put this as the left pillar
        else
        {
            this->MotorAValuesForRobotRegistrationCoordinates[1] = motorA; // would have put this in the wrong spot
            this->MotorAValuesForRobotRegistrationCoordinates[0] = newMotorA;
        }
        
        numLocalMinima++;
    }

    this->DisplayUltrasoundDistancesOnGraph(numNotError, goodPoints, this->RegistrationCanvasTop);






}

// TODO return value for error
int vtkLegoGUI::GetRobotRegistrationCoordinatesForAdvancedTutorial ()
{
    // must reset first if we've already scanned
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        if (strlen(this->RegistrationRobotCoordinatesList->GetCellText(i, 1)) != 0)
        {
            // TODO popup
            std:cout << "must restart before scanning again" << std::endl;
            return -1;
        }
    }

    
    // top swipe
    int ** topSwipeDistances;
    topSwipeDistances = new int*[NUM_REGISTRATION_POINTS_MOTOR_A];
    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        *(topSwipeDistances+i) = new int[3];
    }
    int numNotError = this->SwipeForRegistrationPoints(REGISTRATION_ROTATIONS_MOTOR_B_TOP, MAX_US_DISTANCE_TOP, topSwipeDistances);

    // error in swipe
    if (numNotError == -1)
    {
        return -1;
    }


    this->FindMotorAValuesForRobotRegistrationCoordinatesTopSwipe(topSwipeDistances, numNotError);

    
    // bottom swipe
    int ** bottomSwipeDistances;
    bottomSwipeDistances = new int*[NUM_REGISTRATION_POINTS_MOTOR_A];
    for (int i = 0; i < NUM_REGISTRATION_POINTS_MOTOR_A; i++)
    {
        *(bottomSwipeDistances+i) = new int[3];
    }

    numNotError = this->SwipeForRegistrationPoints(REGISTRATION_ROTATIONS_MOTOR_B_BOTTOM, MAX_US_DISTANCE_BOTTOM, bottomSwipeDistances);

    // error in swipe
    if (numNotError == -1)
    {
        return -1;
    }
    
    FindMotorAValuesForRobotRegistrationCoordinatesBottomSwipe(bottomSwipeDistances, numNotError);

    for (int i = 0; i < 4; i++)
    {
        std::cout << this->MotorAValuesForRobotRegistrationCoordinates[i] << std::endl;
    }


    // get the coordinates from the motorA values
    this->CalculateRobotRegistrationCoordinates(this->MotorAValuesForRobotRegistrationCoordinates, this->RobotRegistrationCoordinates);

    char coordinates[256];
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        sprintf(coordinates, " %.2f %.2f %.2f", this->RobotRegistrationCoordinates[i][0], this->RobotRegistrationCoordinates[i][1], this->RobotRegistrationCoordinates[i][2]);        
        this->RegistrationRobotCoordinatesList->InsertCellText(i, 1, coordinates);
    }




    return 0;

}




//TODO error checking for size of pt, etc
void vtkLegoGUI::FindMotorAValuesForRobotRegistrationCoordinatesBottomSwipe(int** distances, int numNotError)
{

    if (!this->Connected)
    {
        return;
    }

    int ** goodPoints;
    goodPoints = new int*[numNotError];
    int ** filteredPoints;
    filteredPoints = new int*[numNotError];
    for (int i = 0; i < numNotError; i++)
    {
        *(goodPoints + i) = new int[3];
        *(filteredPoints+i) = new int[3];
    }
    this->PreprocessDistances(FILTER_WIDTH_BOTTOM_SWIPE, MAX_US_DISTANCE_BOTTOM, distances, goodPoints, filteredPoints);

    int maxDistanceBetweenPoints = 0;
    int indexOfMaxDistance = 0;
    for (int i = 0; i < numNotError-1; i++)
    {
        if ((filteredPoints[i+1][0] - filteredPoints[i][0]) > maxDistanceBetweenPoints)
        {
            maxDistanceBetweenPoints = filteredPoints[i+1][0] - filteredPoints[i][0];
            indexOfMaxDistance = i;
        }
    }

    int leftMinIndex = 0;
    int leftMaxIndex = indexOfMaxDistance;
    int rightMinIndex = indexOfMaxDistance + 1;
    int rightMaxIndex = numNotError - 1;

    if (leftMinIndex < 0 || leftMinIndex > numNotError-1 || leftMaxIndex < 0 || leftMaxIndex > numNotError-1 || rightMinIndex < 0 || rightMinIndex > numNotError -1 || rightMaxIndex < 0 || rightMaxIndex > numNotError -1)
    {
        return;
    }





    // LEFT PARABOLA

    int middleIndex = (leftMinIndex + leftMaxIndex) / 2;
    int minDistance = 0;
    int start = middleIndex - AVERAGE_WIDTH_BOTTOM_SWIPE;
    if (start < 0)
    {
        start = 0;
    }
    int end = middleIndex + AVERAGE_WIDTH_BOTTOM_SWIPE;
    if (end > numNotError)
    {
        end = numNotError;
    }

    for (int i = start; i <= end; i++)
    {
        minDistance = minDistance + filteredPoints[i][1];
    }
    minDistance = minDistance / (AVERAGE_WIDTH_BOTTOM_SWIPE * 2);    

    // fudge for display
    goodPoints[middleIndex][1] = minDistance;
    goodPoints[middleIndex][2] = 1; // special

    // update registration targets matrix
    double motorA = (double) goodPoints[middleIndex][0];
    double motorB = (double) this->LegoUSB->GetMotorRotation(OUT_B, true);
    //double motorB = REGISTRATION_ROTATIONS_MOTOR_B_BOTTOM;

    std::cout << motorA << " " << minDistance << std::endl;

    this->MotorAValuesForRobotRegistrationCoordinates[2] = motorA;

    // RIGHT PARABOLA


    middleIndex = (rightMinIndex + rightMaxIndex) / 2;
    minDistance = 0;

    start = middleIndex - AVERAGE_WIDTH_BOTTOM_SWIPE;
    if (start < 0)
    {
        start = 0;
    }
    end = middleIndex + AVERAGE_WIDTH_BOTTOM_SWIPE;
    if (end > numNotError)
    {
        end = numNotError;
    }


    for (int i =start; i <= end; i++)
    {
        minDistance = minDistance + filteredPoints[i][1];
    }
    minDistance = minDistance / (AVERAGE_WIDTH_BOTTOM_SWIPE * 2);

    //fudge for display
    goodPoints[middleIndex][1] = minDistance;
    goodPoints[middleIndex][2] = 1; // special

    motorA = (double) goodPoints[middleIndex][0];
    motorB = (double) this->LegoUSB->GetMotorRotation(OUT_B, true);

    std::cout << motorA << " " << minDistance << std::endl;

    this->MotorAValuesForRobotRegistrationCoordinates[3] = motorA;



    this->DisplayUltrasoundDistancesOnGraph(numNotError, goodPoints, this->RegistrationCanvasBottom);

}

// returns 0 if can do biopsy, -1 otherwise

// final motor position stored in result

int vtkLegoGUI::ExecuteBiopsy()
{

    if (!this->Connected)
    {
        return -1;
    }

    char coordinates[256];
    if (!this->BasicTutorial)
    {
        this->CalculateRegistrationMatrixForAdvancedTutorial();
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                sprintf(coordinates, "%.4f",     this->ImageToRobotRegistrationMatrix->GetElement(i,j));
                this->SummaryRegistrationMatrixList->InsertCellText(i, j, coordinates); 
            }
        }
    }



    this->ImageToRobot(this->TargetInImageCoordinates, this->TargetInRobotCoordinates);

    //TODO check that TargetInRobotCoordinates is valid
    //TODO: break out if the error in the registration is too large
    std::cout << "************* EXECUTE BIOPSY ******************" << std::endl;
    std::cout << "target = " << this->TargetInRobotCoordinates[0] << " " << this->TargetInRobotCoordinates[1] << " " << this->TargetInRobotCoordinates[2] << std::endl;

    sprintf(coordinates, " %.2f %.2f %.2f", this->TargetInRobotCoordinates[0], this->TargetInRobotCoordinates[1], this->TargetInRobotCoordinates[2]);
    this->SummaryTargetRobotCoordinateText->SetText(coordinates);




    double motorMovements[3];
    int motorA, motorB, motorC;
    int powerA, powerB, powerC;
    int resultA, resultB, resultC;

    std::cout << "resetting motor" << std::endl;
    this->LegoUSB->ResetMotorPosition(OUT_A, true);
    this->LegoUSB->ResetMotorPosition(OUT_B, true);
    this->LegoUSB->ResetMotorPosition(OUT_C, true);
    std::cout << "inverse kinematics" << std::endl;
    
    InverseKinematics(this->TargetInRobotCoordinates[0], this->TargetInRobotCoordinates[1], this->TargetInRobotCoordinates[2], false, motorMovements);


    motorA = (int)floor(motorMovements[0]);
    motorB = (int)floor(motorMovements[1]);
    motorC = (int)floor(motorMovements[2]);

    powerA = GetPower(motorA);
    powerB = GetPower(motorB);
    powerC = GetPower(motorC);

    std::cout << "theoretical motor movement = " << motorA << " " << motorB << " " << motorC << std::endl;

    // make sure that this is within range
    if (motorA < MIN_MOTOR_A || motorA > MAX_MOTOR_A || motorB < MIN_MOTOR_B || motorB > MAX_MOTOR_B || motorC < MIN_MOTOR_C || motorC > MAX_MOTOR_C)
    {
        std::cout << "target is not within range of motion of the robot" << std::endl;
        return -1; // TODO: deal with this later
    }

    // may have to change the motor speed because of the delay to update needle location

    // so that we can have positive / negative power and absolute value for motor
    motorA = abs(motorA);
    motorB = abs(motorB);
    motorC = abs(motorC);

    this->MoveMotorWithUpdate(OUT_A, powerA, motorA);
    resultA = this->LegoUSB->GetMotorRotation(OUT_A, true);

    this->MoveMotorWithUpdate(OUT_B, powerB, motorB);
    resultB = this->LegoUSB->GetMotorRotation(OUT_B, true);

    this->MoveMotorWithUpdate(OUT_C, powerC, motorC);
    resultC = this->LegoUSB->GetMotorRotation(OUT_C, true);

    std::cout << "actual motor movement = " << resultA << " " << resultB << " " << resultC << std::endl;

    ForwardKinematics(resultA, resultB, resultC, false, this->FinalActuatorPositionInRobotCoordinates);

    std::cout << "resulting position = " << this->FinalActuatorPositionInRobotCoordinates[0] << " " << this->FinalActuatorPositionInRobotCoordinates[1] << " " << this->FinalActuatorPositionInRobotCoordinates[2] << std::endl;


    sprintf(coordinates, " %.2f %.2f %.2f", this->FinalActuatorPositionInRobotCoordinates[0], this->FinalActuatorPositionInRobotCoordinates[1], this->FinalActuatorPositionInRobotCoordinates[2]);
    this->SummaryFinalRobotCoordinateText->SetText(coordinates);




    sleep(2);
    this->MoveMotorWithUpdate(OUT_C, (-1*powerC), motorC);
    motorB = (int) this->DownToUpMotorB(motorB);
    powerB = this->GetPower(motorB);

    std::cout << motorB << " " << powerB << std::endl;
    this->MoveMotorWithUpdate(OUT_B, powerB, motorB);
    this->MoveMotorWithUpdate(OUT_A, (-1*powerA), motorA);
    
    std::cout << "*************************************************" << std::endl << std::endl;


}

// MODIFIED FROM MOVE MOTOR OF NXT_USB
void vtkLegoGUI::MoveMotorWithUpdate(int port, int power, int tachoCount)
{
    if (power < -100 || power > 100 || tachoCount <= 0)
    {
        return;
      }

    if (!this->Connected)
    {
        return;
    }

        // figure out where we want to be
      int goalDegrees;
      if (power > 0)
      {
            goalDegrees = this->LegoUSB->GetMotorRotation(port, false) + tachoCount;
      }
      else
      {
            goalDegrees = this->LegoUSB->GetMotorRotation(port, false) - tachoCount;
      }

      //peform the movement
      this->LegoUSB->SetMotorOn(port, power);
      int motorVal= this->LegoUSB->GetMotorRotation(port, false);

    while ( (power > 0 && motorVal < goalDegrees) || (power < 0 && motorVal > goalDegrees))
    {
        CurrentMotorRotations[port] = motorVal;
        ForwardKinematics(CurrentMotorRotations[0], CurrentMotorRotations[1], CurrentMotorRotations[2], false, this->CurrentActuatorPositionInRobotCoordinates);
        // registration
        this->RobotToImage(this->CurrentActuatorPositionInRobotCoordinates, this->CurrentActuatorPositionInImageCoordinates);
        //TODO update display with Philip's code!
        //this->UpdateAll();


        // get the normal for the needle in robot coord system
        double nIsZero[3]; // n is zero
        ForwardKinematics(CurrentMotorRotations[OUT_A], CurrentMotorRotations[OUT_B], CurrentMotorRotations[OUT_C], false, 0, nIsZero);
        double normal[3];
        normal[0] = this->CurrentMotorRotations[0] - nIsZero[0];
        normal[1] = this->CurrentMotorRotations[1] - nIsZero[1];
        normal[2] = this->CurrentMotorRotations[2] - nIsZero[2];
        // normal should be in the image coordinate system
        RobotToImage(normal, this->CurrentNormalToActuator);

        // normalize
        double normalLength = sqrt(pow(this->CurrentNormalToActuator[0], 2) + pow(this->CurrentNormalToActuator[1], 2) + pow(this->CurrentNormalToActuator[2], 2));
        this->CurrentNormalToActuator[0] = this->CurrentNormalToActuator[0] / normalLength;
        this->CurrentNormalToActuator[1] = this->CurrentNormalToActuator[1] / normalLength;
        this->CurrentNormalToActuator[2] = this->CurrentNormalToActuator[2] / normalLength;
    
        //std::cout << "normal = " << this->CurrentNormalToActuator[0] << " " << this->CurrentNormalToActuator[1] << " " << this->CurrentNormalToActuator[2] << std::endl;

        motorVal = this->LegoUSB->GetMotorRotation(port, false);
    }    

     this->LegoUSB->StopMotor(port, true); // for quick braking
      this->LegoUSB->StopMotor(port, false); // so that it doesn't lock

}



//---------------------------------------------------------------------------
void vtkLegoGUI::UpdateAll()
{

    this->LocatorMatrix = NULL;

   /*  // don't need this stuff anymoe because nx, ny, nz, tx, ty, tz, px, py, pz don't come from a locator matrix, they come from my own stuff
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
    }*/
    this->UpdateLocator();
    //this->UpdateSliceDisplay(nx, ny, nz, tx, ty, tz, px, py, pz);
    this->UpdateSliceDisplay(this->CurrentNormalToActuator[0], this->CurrentNormalToActuator[1], this->CurrentNormalToActuator[2], 0, 0, 0, this->CurrentActuatorPositionInImageCoordinates[0], this->CurrentActuatorPositionInImageCoordinates[1], this->CurrentActuatorPositionInImageCoordinates[2]);

    
}

//---------------------------------------------------------------------------
void vtkLegoGUI::UpdateLocator()
{
    vtkTransform *transform = NULL;

    vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->LocatorModelID.c_str())); 
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

// note:
// locator - image moves with locator
// user - image is set in place, locator moves
void vtkLegoGUI::UpdateSliceDisplay(float nx, float ny, float nz, 
                                        float tx, float ty, float tz, 
                                        float px, float py, float pz)
{
  // Axial
//    if (strcmp(this->RedSliceMenu->GetValue(), "Locator"))
//    {
        if (this->NeedOrientationUpdate0) 
        {
            this->SliceNode0->SetOrientationToAxial();
            this->NeedOrientationUpdate0 = 0;
        }
//     }
//     else
//     {
//         this->SliceNode0->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 0);
//         this->Control0->GetOffsetScale()->SetValue(pz);
//         this->Logic0->SetSliceOffset(pz);
//         this->NeedOrientationUpdate0 = 1;
//     }



    // Sagittal
//    if (strcmp(this->YellowSliceMenu->GetValue(), "Locator"))
//    {
        if (this->NeedOrientationUpdate1) 
        {
            this->SliceNode1->SetOrientationToSagittal();
            this->NeedOrientationUpdate1 = 0;
        }
//    }
//    else
//    {
//         this->SliceNode1->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 1);
//         this->Control1->GetOffsetScale()->SetValue(px);
//         this->Logic1->SetSliceOffset(px);
//         this->NeedOrientationUpdate1 = 1;
//     }

    // Coronal
//    if (strcmp(this->GreenSliceMenu->GetValue(), "Locator"))
//    {
        if (this->NeedOrientationUpdate2) 
        {
            this->SliceNode2->SetOrientationToCoronal();
            this->NeedOrientationUpdate2 = 0;
        }
//     }
//     else
//     {
//         this->SliceNode2->SetSliceToRASByNTP( nx, ny, nz, tx, ty, tz, px, py, pz, 2);
//         this->Control2->GetOffsetScale()->SetValue(py);
//         this->Logic2->SetSliceOffset(py);
//         this->NeedOrientationUpdate2 = 1;
//     }

}

// return array should be of size 3
void vtkLegoGUI::ForwardKinematics (double motorA, double motorB, double motorC, bool up, double * returnArray)
{
    double n = GetN(motorC);
    this->ForwardKinematics (motorA, motorB, motorC, up, n, returnArray);

}


// return Array should be of size 3
void vtkLegoGUI::ForwardKinematics (double motorA, double motorB, double motorC, bool up, double n, double * returnArray)
{
    double alpha, beta, phi;
    alpha = GetAlpha (motorA); // degrees
    if (up)
    {
        beta = GetBetaUp(motorB); // degrees
        phi = GetPhiUp(motorB); // degrees
    }
    else
    {
        beta = GetBetaDown(motorB); // degrees
        phi = GetPhiDown(motorB); // degrees
    }
    //double n = GetN(motorC);

    double I2 = sqrt (pow(n, 2) + pow(TOP_BEAM_LENGTH, 2));
    double x = atan2(n, TOP_BEAM_LENGTH); // radians
    x = RadiansToDegrees(x); // degrees

    double omega = 90.0 - x - beta; // degrees
    omega = DegreesToRadians(omega); // radians
    double B = sqrt(pow(I2, 2) + pow(I1, 2) - (2.0 * I2 * I1 * cos(omega)));
    double q2 = acos((pow(B, 2) + pow(I1, 2) - pow(I2, 2)) / (2.0 * B * I1)); // radians
    q2 = RadiansToDegrees(q2); // degrees
    double q1 = 90.0 - q2 - phi; // degrees
    q1 = DegreesToRadians(q1); // radians
    
    double Y_doublePrime = B * cos(q1);
    double Z = B * sin(q1);

    Y_doublePrime = Y_doublePrime + Y_PRIME;

    alpha = DegreesToRadians(alpha); // radians
    double X = Y_doublePrime * sin(alpha);
    double Y = Y_doublePrime * cos(alpha);
    
    returnArray[0] = X;
    returnArray[1] = Y;
    returnArray[2] = Z;

} // end ForwardKinematics






//tODO error checking, motorA should be of length four (NUM_SCANNED_REGISTRATION_POINTS), in correct order, return array should be 8.3
// gives registration coordinates in robot coordinate system
void vtkLegoGUI::CalculateRobotRegistrationCoordinates(double * motorA, double ** returnArray)
{

    // must have four values in motorA
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
        // perhaps change this to being out of range
        if (motorA[i] == 0)
        {
            std::cout << "error in CalculateRobotRegistrationCoordinates - do not have four motorA values";
            return;
        }
    }

    //TODO take out
    std::cout << "motorAs" << std::endl;
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
        std::cout << motorA[i] << std::endl;
    }


    // get alphas from motorA values - the last four will be calculated later
    double alpha[NUM_REGISTRATION_POINTS];
    alpha[0] = this->GetAlphaRegistrationTopSwipe(motorA[0]);
    alpha[1] = this->GetAlphaRegistrationTopSwipe(motorA[1]);
    alpha[2] = this->GetAlphaRegistrationBottomSwipe(motorA[2]);
    alpha[3] = this->GetAlphaRegistrationBottomSwipe(motorA[3]);
    
    //TODO take out
    //double avgAngle = 0; // straight on
    double avgAngle = RadiansToDegrees(atan2(-5.75, 22.25)); // point 8
    //double avgAngle = RadiansToDegrees(atan2(9.75, 20.875)); // point 5
    //double avgAngle = RadiansToDegrees(atan2(-16.625, 16.25)); // point 10
    /*alpha[0] = avgAngle - PILLAR_DET_ANGLE_1;
    alpha[1] = avgAngle + PILLAR_DET_ANGLE_2;
    alpha[2] = avgAngle - PILLAR_DET_ANGLE_3;
    alpha[3] = avgAngle + PILLAR_DET_ANGLE_4;*/

    // TODO take out
    std::cout << "alphas should be: " << std::endl;
    std::cout << avgAngle - PILLAR_DET_ANGLE_1 << std::endl;
    std::cout << avgAngle + PILLAR_DET_ANGLE_2 << std::endl;
    std::cout << avgAngle - PILLAR_DET_ANGLE_3 << std::endl;
    std::cout << avgAngle + PILLAR_DET_ANGLE_4 << std::endl;
    std::cout << "perpendicular angle should be " << avgAngle << std::endl;    



    // TODO take out
    std::cout << "alphas" << std::endl;
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
        std::cout << alpha[i] << std::endl;
    }

    // get perpendicular angle
    double perpendicularAngles[4];
    perpendicularAngles[0] = alpha[0] + PILLAR_DET_ANGLE_1;
    perpendicularAngles[1] = alpha[1] - PILLAR_DET_ANGLE_2;
    perpendicularAngles[2] = alpha[2] + PILLAR_DET_ANGLE_3;
    perpendicularAngles[3] = alpha[3] - PILLAR_DET_ANGLE_4;
    double avgPerpendicularAngle = 0.0;
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
        avgPerpendicularAngle = avgPerpendicularAngle + perpendicularAngles[i];
    }
    avgPerpendicularAngle = avgPerpendicularAngle / NUM_SCANNED_REGISTRATION_POINTS;

    // TODO take out
    std::cout << "perpendicular angles" << std::endl;
    for (int i = 0; i < NUM_SCANNED_REGISTRATION_POINTS; i++)
    {
        std::cout << perpendicularAngles[i] << std::endl;
    }


    std::cout << "average perpendicular angle = " << avgPerpendicularAngle << std::endl;

    // adjust alphas so that they have the correct internal angles
    alpha[0] = avgPerpendicularAngle - PILLAR_DET_ANGLE_1;
    alpha[1] = avgPerpendicularAngle + PILLAR_DET_ANGLE_2;
    alpha[2] = avgPerpendicularAngle - PILLAR_DET_ANGLE_3;
    alpha[3] = avgPerpendicularAngle + PILLAR_DET_ANGLE_4;
    alpha[4] = avgPerpendicularAngle - CENTRAL_BOX_DET_ANGLE_1;
    alpha[5] = avgPerpendicularAngle + CENTRAL_BOX_DET_ANGLE_2;
    alpha[6] = avgPerpendicularAngle - CENTRAL_BOX_DET_ANGLE_3;
    alpha[7] = avgPerpendicularAngle + CENTRAL_BOX_DET_ANGLE_4;






    // TODO take out
    std::cout << "new alphas" << std::endl;
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        std::cout << alpha[i] << std::endl;
    }

    // adjustment to get front of pillar instead of center
    double adjustX = PILLAR_RADIUS * cos(DegreesToRadians(90.0 - avgPerpendicularAngle));
    double adjustY = PILLAR_RADIUS * sin(DegreesToRadians(90.0 - avgPerpendicularAngle));

    // calculate the coordinates
    double distances[8];
    distances[0] = PILLAR_D1_TOP_LEFT; // incorporates pillar radius, so don't have to add it in in the next step
    distances[1] = PILLAR_D2_TOP_RIGHT;
    distances[2] = PILLAR_D3_BOTTOM_LEFT;
    distances[3] = PILLAR_D4_BOTTOM_RIGHT;
    distances[4] = CENTRAL_BOX_D1_TOP_LEFT;
    distances[5] = CENTRAL_BOX_D2_TOP_RIGHT;
    distances[6] = CENTRAL_BOX_D3_BOTTOM_LEFT;
    distances[7] = CENTRAL_BOX_D4_BOTTOM_RIGHT;

    //TODO take out
    std::cout << "distances" << std::endl;
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        std::cout << distances[i] << std::endl;
    }







    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        returnArray[i][0] = ((distances[i]) * sin(DegreesToRadians(alpha[i])));
        returnArray[i][1] = ((distances[i]) * cos(DegreesToRadians(alpha[i])));
    }


    // have to adjust for pillars
    for (int i = 0; i < 4; i++)
    {
        returnArray[i][0] = returnArray[i][0] - adjustX;
        returnArray[i][1] = returnArray[i][1] - adjustY;
    }


    returnArray[0][2] = PILLAR_HEIGHT_TOP;
    returnArray[1][2] = PILLAR_HEIGHT_TOP;
    returnArray[2][2] = PILLAR_HEIGHT_BOTTOM;
    returnArray[3][2] = PILLAR_HEIGHT_BOTTOM;
    for (int i = 4; i < 8; i++)
    {
        returnArray[i][2] = CENTRAL_BOX_HEIGHT;
    }

    
    // adjust because bottom right pillar is weird
    returnArray[3][1] = returnArray[3][1] + 1.0;    

    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        std::cout << "registration points = " << returnArray[i][0] << " " << returnArray[i][1] << " " << returnArray[i][2] << std::endl;
    }

    // calculate where the green and red targets are - using avgAngle = correct
    // green target
    double green_a = RadiansToDegrees(atan2(3.5, 12.5));
    double green_l = sqrt(pow(3.5, 2) + pow(12.5, 2));
    double green_w = DegreesToRadians((-1.0*avgAngle) - green_a);
    double green_tx = green_l * cos(green_w);
    double green_ty = green_l * sin(green_w);
    double greenX = returnArray[3][0] + adjustX - green_tx;
    double greenY = returnArray[3][1] + adjustY - green_ty;
    std::cout << "green target is really at " << greenX << " " << greenY << " -5.25" << std::endl;
    // red target
    double red_a = RadiansToDegrees(atan2(2.5, 20.5));
    double red_l = sqrt(pow(2.5, 2) + pow(20.5, 2));
    double red_w = DegreesToRadians((-1.0*avgAngle) - red_a);
    double red_tx = red_l * cos(red_w);
    double red_ty = red_l * sin(red_w);
    double redX = returnArray[3][0] + adjustX - red_tx;
    double redY = returnArray[3][1] + adjustY - red_ty;
    std::cout << "red target is really at " << redX << " " << redY << " -4.00" << std::endl;



    // calculate where the green and red targets are - using avgPerpendicularAngle
    // green target
     green_a = RadiansToDegrees(atan2(3.5, 12.5));
     green_l = sqrt(pow(3.5, 2) + pow(12.5, 2));
     green_w = DegreesToRadians((-1.0*avgPerpendicularAngle) - green_a);
     green_tx = green_l * cos(green_w);
     green_ty = green_l * sin(green_w);
     greenX = returnArray[3][0] + adjustX - green_tx;
     greenY = returnArray[3][1] + adjustY - green_ty;
    std::cout << "we say that green target is at " << greenX << " " << greenY << " -5.25" << std::endl;
    // red target
     red_a = RadiansToDegrees(atan2(2.5, 20.5));
     red_l = sqrt(pow(2.5, 2) + pow(20.5, 2));
     red_w = DegreesToRadians((-1.0*avgPerpendicularAngle) - red_a);
     red_tx = red_l * cos(red_w);
     red_ty = red_l * sin(red_w);
     redX = returnArray[3][0] + adjustX - red_tx;
     redY = returnArray[3][1] + adjustY - red_ty;
    std::cout << "we say that red target is at " << redX << " " << redY << " -4.00" << std::endl;


    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            this->RobotRegistrationCoordinates[i][j] = returnArray[i][j];
        }
    }






}





// return array should be of size three
// returns -1 on error, 0 if ok //TODO for all of these functions that return -1 on error, check result when you call them!
//TODO return int, for error, instead of best diff
double vtkLegoGUI::InverseKinematics (double X, double Y, double Z, bool up, double * returnArray)
{

    double alpha = atan2(X, Y); // radians
    alpha = RadiansToDegrees(alpha); // degrees
    double motorA = GetMotorA(alpha);


    //std::cout << "alpha = " << alpha << std::endl;
    //std::cout << "motorA = " << motorA << std::endl;

    
    double Y_doublePrime = sqrt(pow(X, 2) + pow(Y, 2));
    Y_doublePrime = Y_doublePrime - Y_PRIME;

    double q1 = atan2(Z, Y_doublePrime); // radians
    q1 = RadiansToDegrees (q1); // degrees
    
    double B = sqrt (pow(Y_doublePrime, 2) + pow(Z, 2));

    double x, I2, q2, phi, omega, beta, motorBBeta, motorBPhi, diff, bestDiff, bestMotorB, bestN;

    double n = BASE_N;
    for (n = BASE_N; n <= MAX_N; n = n + INVERSE_KINEMATICS_N_STEP)
    {

        x = atan2(n, TOP_BEAM_LENGTH); // radians
        x = RadiansToDegrees(x); // degrees
        I2 = sqrt (pow(n, 2) + pow(TOP_BEAM_LENGTH, 2));
        
        q2 = acos ((pow(I1, 2) + pow(B, 2) - pow(I2, 2)) / (2.0*I1*B)); // radians
        q2 = RadiansToDegrees(q2); // degrees
        phi = 90.0 - q1 - q2;
        omega = acos ((pow(I1, 2) + pow(I2, 2) - pow(B, 2)) / (2.0*I1*I2)); // radians
        omega = RadiansToDegrees(omega); // degrees
        beta = 90.0 - x - omega;

        if (up)
        {
            motorBBeta = GetMotorBBetaUp(beta);
            motorBPhi = GetMotorBPhiUp(phi);
        }
        else
        {
            motorBBeta = GetMotorBBetaDown(beta);
            motorBPhi = GetMotorBPhiDown(phi);
        }
        
        // method 1 - original
        diff = fabs(motorBBeta - motorBPhi);
        if (n == BASE_N)
        {
            bestDiff = diff;
            bestMotorB = (motorBBeta + motorBPhi) / 2.0;
            bestN = n;
        }
        else
        {
            if (diff < bestDiff)
            {
                bestDiff = diff;
                bestMotorB = (motorBBeta + motorBPhi) / 2.0;
                bestN = n;
            }
        }

        // method 2 - use forward kinematics
        /*double motorB = (motorBBeta + motorBPhi) / 2.0;
        double motorC = GetMotorC(n);

        double ForwardResults[3];
        ForwardKinematics(motorA, motorB, motorC, up, ForwardResults);
        double newX = ForwardResults[0];
        double newY = ForwardResults[1];
        double newZ = ForwardResults[2];

        double dist = sqrt(pow((newX - X), 2) + pow((newY - Y), 2) + pow((newZ - Z), 2));

        if (n == BASE_N)
        {
            bestDiff = dist;
            bestMotorB = motorB;
            bestN = n;
        }
        else
        {
            if (dist < bestDiff)
            {
                bestDiff = dist;
                bestMotorB = motorB;
                bestN = n;
            }
        }*/





    } // end for
    
    // it is possible to reach that point given the range of motion of the robot
    double motorB = bestMotorB;
    double motorC = GetMotorC(bestN);

    //std::cout << "motorC = " << motorC << std::endl;

    returnArray[0] = motorA;
    returnArray[1] = motorB;
    returnArray[2] = motorC;

    /*std::cout << "\talpha = " << alpha << std::endl;
    std::cout << "\tbeta = " << beta << std::endl;
    std::cout << "\tphi = " << phi << std::endl;
    std::cout << "\tbestN = " << bestN << std::endl;
    std::cout << "\t\tbestDiff = " << bestDiff << std::endl;*/



    return bestDiff;

    // TODO put back?
    //return 0;

} // end Inverse Kinematics





// returns in degrees
double vtkLegoGUI::GetAlpha (double motorA)
{
    double alpha;
    double fabsMotorA = fabs(motorA);

    // calculate
    if (fabsMotorA < 70.0)
    {
        alpha = 0.0;
    }
    else if (fabsMotorA < 200.0)
    {
        alpha = (0.0581 * fabsMotorA) -3.2806;
    }
    else if (fabsMotorA < 520.0)
    {
        alpha = (0.0495 * fabsMotorA) -1.8980;
    }
    else
    {
        alpha = (0.0474 * fabsMotorA) -0.4741;
    }

    // negative stuff again
    if (motorA < 0)
    {
        alpha = alpha * -1.0;
    }

    return alpha;
}

double vtkLegoGUI::GetAlphaRegistrationTopSwipe (double motorA)
{
    double alpha = (0.0451 * motorA) -2.1377;
    return alpha;
}

double vtkLegoGUI::GetMotorARegistrationTopSwipe(double alpha)
{
    double motorA = (22.1503 * alpha) + 47.2195;
    return motorA;
}

double vtkLegoGUI::GetAlphaRegistrationBottomSwipe (double motorA)
{
      double alpha = (-0.00000000346570 * pow(motorA, 3)) + (0.00000129474660 * pow(motorA, 2)) + (0.05043163906092 * motorA) -3.98448970770030;
    return alpha;
}

double vtkLegoGUI::GetMotorARegistrationBottomSwipe (double alpha)
{
    double motorA = (0.00098507783650 * pow(alpha, 3)) + (-0.01028916341507 * pow(alpha, 2)) + (19.20971018871304 * alpha) +  83.41975692335431;
    return motorA;

}




// returns in degrees
double vtkLegoGUI::GetBetaDown (double motorB)
{
    double beta = (-0.00000003308719 * pow(motorB, 3)) - (0.00005762455591 * pow(motorB, 2)) + (0.01827383455325 * motorB) + 5.00327731092438;
    return beta;
}

// returns in degrees
double vtkLegoGUI::GetBetaUp (double motorB)
{
       double beta = (-0.00000002413574 * pow(motorB, 3)) - (0.00003452007325 * pow(motorB, 2)) + (0.03052308203779 * motorB) + 1.77592436974790;
    return beta;
}

// returns in degrees
double vtkLegoGUI::GetPhiDown (double motorB)
{
        double phi = (0.00000004004888 * pow(motorB, 3)) + (0.00006667111030 * pow(motorB, 2)) - (0.03820015414868 * motorB) - 23.784264705888236;
    return phi;
}

// returns in degrees
double vtkLegoGUI::GetPhiUp (double motorB)
{
        double phi = (0.00000002390368 * pow(motorB, 3)) + (0.00002728792631 * pow(motorB, 2)) - (0.05840783862107 * motorB) - 18.81804621848739;
    return phi;
}





double vtkLegoGUI::GetN (double motorC)
{
    double n = BASE_N + (0.0031 * motorC) + 0.5706;
    return n;
}





// returns in degrees
double vtkLegoGUI::GetMotorA (double alpha)
{
    double motorA;
    double fabsAlpha = fabs(alpha);

    // calculate
    if (fabsAlpha == 0)
    {
        motorA = 0.0;
    }
    else if (alpha < 8.14)
    {
        motorA = (17.1146 * fabsAlpha) + 56.8467;
    }
    else if (alpha < 24.0)
    {
        motorA = (20.1744 * fabsAlpha) + 38.7501;
    }
    else
    {
        motorA = (21.0408 * fabsAlpha) + 12.3574;
    }

    // negative stuff again
    if (alpha < 0)
    {
        motorA = motorA * -1.0;
    }

    return motorA;
}


// argument in degrees
double vtkLegoGUI::GetMotorBBetaDown (double beta)
{
        double motorB = (0.011104339223 * pow(beta, 3)) +  (0.615557013735 * pow(beta, 2)) + (29.488907230758 * beta) -174.114741896755;
    return motorB;
}
// argument in degrees
double vtkLegoGUI::GetMotorBBetaUp (double beta)
{
     double motorB = (0.01109625573188 * pow(beta, 3)) + (0.57469172871044 * pow(beta, 2)) + (30.05314588321274 * beta) -54.08429004041755;
    return motorB;
}

// argument in degrees
double vtkLegoGUI::GetMotorBPhiDown (double phi)
{
    double motorB = (-0.002556602616 * pow(phi, 3)) + (0.081161149356 * pow(phi, 2)) + (-13.627661211963 * phi) -408.915010835473;
    return motorB;
}

// argument in degrees
double vtkLegoGUI::GetMotorBPhiUp (double phi)
{
    double motorB = (-0.002363478676 * pow(phi, 3)) + (0.045400562740 * pow(phi, 2)) + (-14.222625483279 * phi) -295.623191831991;
    return motorB;
}



double vtkLegoGUI::GetMotorC(double n)
{
    double motorC = (326.8925 * (n - BASE_N)) - 185.9533;
    return motorC;
}






 // convert radians to degrees
double vtkLegoGUI::RadiansToDegrees(double rad)
{
    return rad * DOUBLE_RADIANS_TO_DEGREES;
}

// convert degrees to radians
double vtkLegoGUI::DegreesToRadians(double deg)
{
    return deg * DOUBLE_DEGREES_TO_RADIANS;
}



//TODO coordinates should be of length 3
// exit gracefully on error, instead of crashing and displaying nasty error message
void vtkLegoGUI::StripCoordinatesFromString(const char* rasText, double * coordinates)
{
    std::string ras = std::string(rasText);

    if (ras.empty())
    {
        std::cout << "ras is empty" << std::endl;
        return;
    }

    // strip the first space
    ras = ras.substr(ras.find(" ")+1);     //5.67^^7.46^^3.64

    // find x
    std::string x = ras.substr(0, (ras.find(" ")));    //5.67

    coordinates[0] = atof(x.c_str());
    ras = ras.substr(ras.find(" "));    //^^7.46^^3.64
    ras = ras.substr(ras.find_first_not_of(" "));    //7.46^^3.64

    // find y
    std::string y = ras.substr(0, (ras.find(" ")));
    //this->TargetInImageCoordinates[1] = atof(y.c_str());
    coordinates[1] = atof(y.c_str());
    ras = ras.substr(ras.find(" "));
    ras = ras.substr(ras.find_first_not_of(" "));

    // find z
    std::string z = ras;
    //this->TargetInImageCoordinates[2] = atof(z.c_str());
    coordinates[2] = atof(z.c_str());
    

}


int vtkLegoGUI::GetPower(int motor)
{
    int power = 0;
    if (abs(motor) < 50)
    {
        power = 2;
    }
    else if (abs(motor) < 100)
    {
        power = 5;
    }
    else if (abs(motor) < 200)
    {
        power = 10;
    }
    else if (abs(motor) < 520)
    {
        power = 20;
    }
    else
    {
        power = 50;
    }
    
    if (motor < 0)
    {
        power = power * -1;
    }
    return power;
}




// median filter on column 1 (second column) - must be formatted properly
// both arrays should be length x 3
// n should be odd
void vtkLegoGUI::MedianFilter(int** original, int** filtered, int arrayLength, int filterWidth)
{
    int halfN = (filterWidth-1) / 2;
    int min;
    int max;
    int count;

    for (int i = 0; i < arrayLength; i++)
    {
    
        // find boundaries of the mask
        min = i-halfN;
        if (min < 0)
        {
            min = 0;
        }

        max = i+halfN;
        if (max >= arrayLength)
        {
            max = arrayLength - 1;
        }

        //std::cout << halfN << " " << min << " " << max << std::endl;
        
        // the mask values
        int window[max - min + 1];
        count = 0;
        for (int j = min; j <= max; j++)
        {
            window[count] = original[j][1];
            count++;
        }

        // find the median
        filtered[i][1] = this->Median(window, (max-min+1));

        //std::cout << "returned " << filtered[i][1] << std::endl;
    }    
}



// window should be a 1D array of length length
int vtkLegoGUI::Median (int* window, int windowLength)
{


    int temp, k;
    for (int j = 1; j < windowLength; j++)
    {
        temp = window[j];
        k = j-1;
        while (k >= 0 && temp < window[k])
        {
            window[k+1] = window[k];
            k--;
        }
        window[k+1] = temp;
    }

    // even - average middle two numbers
    if ((windowLength % 2) == 0)
    {
        return (window[windowLength / 2] + window[(windowLength / 2)-1]) / 2;
    }
    //odd - return the middle number
    else
    {
        return window[windowLength / 2]; // integer division will work out
    }


}

int vtkLegoGUI::CalculateRegistrationMatrixForAdvancedTutorial()
{
    vtkIGTPat2ImgRegistration* regAlg = vtkIGTPat2ImgRegistration::New();
    regAlg->SetNumberOfPoints(NUM_REGISTRATION_POINTS);





    
    //TODO error checking here to makes sure that the two matrices are filled
    
    for (int i = 0; i < NUM_REGISTRATION_POINTS; i++)
    {

        // gives {1, 2, 3} - transformation from image to robot
        regAlg->AddPoint(i, RobotRegistrationCoordinates[i][0], RobotRegistrationCoordinates[i][1], RobotRegistrationCoordinates[i][2], ImageRegistrationCoordinates[i][0], ImageRegistrationCoordinates[i][1], ImageRegistrationCoordinates[i][2]);

        // gives {-1, -2, -3} - transformation from robot to image
        //regAlg->AddPoint(i, ImageRegistrationCoordinates[i][0], ImageRegistrationCoordinates[i][1], ImageRegistrationCoordinates[i][2], RobotRegistrationCoordinates[i][0], RobotRegistrationCoordinates[i][1], RobotRegistrationCoordinates[i][2]);
    }    

    int error = regAlg->DoRegistration();
    if (error)
    {
            vtkSlicerApplication::GetInstance()->ErrorMessage("Error registration between robot and image land marks.");
                return error;
    }
    
    this->ImageToRobotRegistrationMatrix->DeepCopy(regAlg->GetLandmarkTransformMatrix());

    this->RobotToImageRegistrationMatrix->DeepCopy(this->ImageToRobotRegistrationMatrix);

    this->RobotToImageRegistrationMatrix->Invert();


    regAlg->Delete();




    return 0;
}

// should be of length 3
// transforms image coords to robot coords
//TODO error checking

void vtkLegoGUI::ImageToRobot(double * imageCoordinates, double * robotCoordinates)
{

    double imageCoordinatesLong[4];
    imageCoordinatesLong[0] = imageCoordinates[0];
    imageCoordinatesLong[1] = imageCoordinates[1];
    imageCoordinatesLong[2] = imageCoordinates[2];
    imageCoordinatesLong[3] = 1;

    double robotCoordinatesLong[4];
    robotCoordinatesLong[0] = robotCoordinates[0];
    robotCoordinatesLong[1] = robotCoordinates[1];
    robotCoordinatesLong[2] = robotCoordinates[2];
    robotCoordinatesLong[3] = 1;


    // MultiplyPoint(in, out)
    this->ImageToRobotRegistrationMatrix->MultiplyPoint(imageCoordinatesLong, robotCoordinatesLong);

    robotCoordinates[0] = robotCoordinatesLong[0];
    robotCoordinates[1] = robotCoordinatesLong[1];
    robotCoordinates[2] = robotCoordinatesLong[2];
}


// should be of length 3
// transforms image coords to robot coords
//TODO error checking

void vtkLegoGUI::RobotToImage(double * robotCoordinates, double * imageCoordinates)
{

    double robotCoordinatesLong[4];
    robotCoordinatesLong[0] = robotCoordinates[0];
    robotCoordinatesLong[1] = robotCoordinates[1];
    robotCoordinatesLong[2] = robotCoordinates[2];
    robotCoordinatesLong[3] = 1;

    double imageCoordinatesLong[4];
    imageCoordinatesLong[0] = imageCoordinates[0];
    imageCoordinatesLong[1] = imageCoordinates[1];
    imageCoordinatesLong[2] = imageCoordinates[2];
    imageCoordinatesLong[3] = 1;


    // MultiplyPoint(in, out)
    this->RobotToImageRegistrationMatrix->MultiplyPoint(robotCoordinatesLong, imageCoordinatesLong);

    imageCoordinates[0] = imageCoordinatesLong[0];
    imageCoordinates[1] = imageCoordinatesLong[1];
    imageCoordinates[2] = imageCoordinatesLong[2];
}




void vtkLegoGUI::CalculateRegistrationMatrixForBasicTutorial()
{

    int numCoords = 72; // used to be 72
    
    // base - robot coordinates: x centered on robot, y at bottom edge, z at floor
    double base[3] = {-2.0, 5.5, -10.0};


    double robotCoordinates[72][3] =
    {
        {-8.0, 30.0, 7.125},
        {8.0, 30.0, 7.125},
        {-8.0, 18.0, 7.125},
        {8.0, 18.0, 7.125},
        {-7.0, 29.0, 3.625},
        {7.0, 29.0, 3.625},
        {-7.0, 19.0, 3.625},
        {7.0, 19.0, 3.625},
        {-7.0, 25.0, 7.125},
        {-2.0, 25.0, 7.125},
        {-7.0, 20.0, 7.125},
        {-2.0, 20.0, 7.125},
        {-6.0, 24.0, 4.75},
        {-3.0, 24.0, 4.75},
        {-6.0, 21.0, 4.75},
        {-3.0, 21.0, 4.75},
        {1.0, 26.0, 6.0},
        {6.0, 26.0, 6.0},
        {1.0, 21.0, 6.0},
        {6.0, 21.0, 6.0},
        {2.0, 25.0, 3.625},
        {5.0, 25.0, 3.625},
        {2.0, 22.0, 3.625},
        {5.0, 22.0, 3.625},
        {-25.0, 32.0, 14.375},
        {-19.0, 32.0, 14.375},
        {-25.0, 28.0, 16.75},
        {-19.0, 28.0, 16.75},
        {-24.0, 30.0, 22.75},
        {-20.0, 30.0, 22.75},
        {-24.0, 28.0, 22.75},
        {-20.0, 28.0, 22.75},
        {-23.0, 31.0, 28.75},
        {-21.0, 31.0, 28.75},
        {-23.0, 27.0, 28.75},
        {-21.0, 27.0, 28.75},
        {19.0, 26.0, 7.125},
        {25.0, 26.0, 7.125},
        {19.0, 30.0, 16.75},
        {25.0, 30.0, 16.75},
        {20.0, 30.0, 22.75},
        {24.0, 30.0, 22.75},
        {20.0, 28.0, 22.75},
        {24.0, 28.0, 22.75},
        {21.0, 31.0, 28.75},
        {23.0, 31.0, 28.75},
        {21.0, 27.0, 28.75},
        {23.0, 27.0, 28.75},
        {-17.0, 17.0, 3.625},
        {-15.0, 17.0, 3.625},
        {-19.0, 21.0, 6.0},
        {-13.0, 21.0, 6.0},
        {-18.0, 21.0, 12.0},
        {-14.0, 21.0, 12.0},
        {-18.0, 19.0, 12.0},
        {-14.0, 19.0, 12.0},
        {-17.0, 22.0, 17.875},
        {-15.0, 22.0, 17.875},
        {-17.0, 18.0, 17.875},
        {-15.0, 18.0, 17.875},
        {15.0, 23.0, 3.625},
        {17.0, 23.0, 3.625},
        {13.0, 19.0, 6.0},
        {19.0, 19.0, 6.0},
        {14.0, 21.0, 13.125},
        {18.0, 21.0, 13.125},
        {14.0, 19.0, 13.125},
        {18.0, 19.0, 13.125},
        {14.0, 21.0, 17.875},
        {18.0, 21.0, 17.875},
        {14.0, 19.0, 17.875},
        {18.0, 19.0, 17.875}
    };

    double imageCoordinates[72][3] =
    {
        { -119.3, -35.0, 65.5},
        { -114.7, -36.0, -61.4},
        { -25.6, -37.0, 69.0 },
        { -20.4, -39.0, -59.1},
        { -112.9, -63.0, 59.7},
        { -108.9, -64.0, -53.8},
        { -33.2, -65.0, 62.6},
        { -29.7, -65.0, -52.7},
        { -80.3, -36.0, 59.7},
        { -78.6, -37.0, 18.3},
        { -40.8, -37.0, 60.3},
        { -40.8, -37.0, 21.2},
        { -72.2, -55.0, 52.7},
        { -71.6, -55.0, 28.2},
        { -48.9, -55.0, 53.3},
        { -48.3, -55.0, 28.2},
        { -85.0, -46.0, -4.9 },
        { -83.8, -47.0, -45.1},
        { -47.2, -47.0, -3.2},
        { -46.0, -47.0, -43.4},
        { -78.0, -65.0, -12.5},
        { -77.4, -65.0, -35.8},
        { -54.7, -65.0, -11.9},
        { -54.1, -65.0, -35.2},
        { -136.8, 25.0, 200.6},
        { -135.1, 25.0, 152.8},
        { -106.0, 43.0, 200.6},
        { -106.5, 43.0, 152.8},
        { -118.2, 91.0, 191.8},
        { -116.4, 91.0, 159.2},
        { -103.6, 91.0, 192.4},
        { -102.5, 91.0, 160.4},
        { -124.0, 138.0, 182.5},
        { -123.4, 138.0, 166.8},
        { -92.6, 137.0, 184.2},
        { -92.6, 137.0, 167.9},
        { -81.5, -38.0, -148.2},
        { -79.2, -38.0, -195.3},
        { -106.5, 41.0, -145.8},
        { -106.0, 41.0, -194.1},
        { -103.6, 88.0, -154.0},
        { -103.0, 88.0, -185.4},
        { -90.8, 88.0, -154.6},
        { -90.8, 88.0, -184.2},
        { -109.4, 136.0, -162.1},
        { -109.4, 136.0, -176.7},
        { -79.2, 135.0, -161.5},
        { -78.6, 135.0, -174.9},
        { -21.0, -67.0, 142.3},
        { -20.4, -67.0, 125.5},
        { -52.4, -47.0, 156.3},
        { -50.6, -47.0, 109.2},
        { -51.8, 0.0, 148.7},
        { -50.6, 0.0, 115.6},
        { -36.1, 0.0, 148.7},
        { -34.9, 0.0, 116.1},
        { -58.8, 48.0, 139.4},
        { -58.2, 48.0, 121.4},
        { -27.4, 48.0, 140.6},
        { -27.4, 48.0, 123.7},
        { -57.6, -68.0, -115.0},
        { -58.8, -68.0, -130.7},
        { -27.4, -49.0, -99.3},
        { -26.2, -49.0, -145.8},
        { -43.1, 9.0, -105.1},
        { -44.2, 9.0, -137.1},
        { -27.4, 9.0, -105.1},
        { -27.9, 9.0, -138.3},
        { -43.1, 48.0, -105.7},
        { -43.7, 48.0, -136.5},
        { -27.9, 48.0, -105.7},
        { -27.9, 48.0, -137.1}
    };

    for (int i = 0; i < numCoords; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            robotCoordinates[i][j] = robotCoordinates[i][j] + base[j];
        }
    }


    // scale the image data
    for (int i = 0; i < numCoords; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            imageCoordinates[i][j] = imageCoordinates[i][j] / SCALE;
        }
    }


    // robot coordinates originally measured from a point on the phantom - now convert to the robot coordinate system
    vtkIGTPat2ImgRegistration* regAlg = vtkIGTPat2ImgRegistration::New();
    regAlg->SetNumberOfPoints(numCoords);

    for (int i = 0; i < numCoords; i++)
    {    
        // transformation from robot to image
        regAlg->AddPoint(i, robotCoordinates[i][0], robotCoordinates[i][1], robotCoordinates[i][2], imageCoordinates[i][0], imageCoordinates[i][1], imageCoordinates[i][2]);

    }    

    int error = regAlg->DoRegistration();
    if (error)
    {
            vtkSlicerApplication::GetInstance()->ErrorMessage("Error registration between robot and image land marks.");
        return;

    }
    
    this->ImageToRobotRegistrationMatrix->DeepCopy(regAlg->GetLandmarkTransformMatrix());

    this->RobotToImageRegistrationMatrix->DeepCopy(this->ImageToRobotRegistrationMatrix);

    this->RobotToImageRegistrationMatrix->Invert();

    char coordinates[256];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            sprintf(coordinates, "%.4f", this->ImageToRobotRegistrationMatrix->GetElement(i,j));

            std::cout << coordinates << std::endl;
            this->SummaryRegistrationMatrixList->InsertCellText(i, j, coordinates); 
        
        }
    }


    regAlg->Delete();

}






