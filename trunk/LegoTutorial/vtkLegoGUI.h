// .NAME vtkLegoGUI 
// .SECTION Description
// Main Volumes GUI and mediator methods for slicer3. 

#ifndef __vtkLegoGUI_h
#define __vtkLegoGUI_h

#ifdef WIN32
#include "vtkLegoWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkLegoLogic.h"

#include "vtkIGTDataManager.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkIGTPat2ImgRegistration.h"  // for registration between robot and image coordinate systems
#include "NXT_USB.h" // for communication with the Lego Mindstorms NXT

#include <string>

class vtkKWPushButton;
class vtkKWEntryWithLabel;
class vtkKWCheckButton;
class vtkKWLabel;
class vtkKWCanvas;
class vtkKWRadioButton;
class vtkKWMultiColumnList;

// Description:    
// This class implements Slicer's Volumes GUI
//
class VTK_LEGO_EXPORT vtkLegoGUI : public vtkSlicerModuleGUI
{
 public:

    // Description:
    // Usual vtk class functions
    static vtkLegoGUI* New (  );
    vtkTypeRevisionMacro ( vtkLegoGUI, vtkSlicerModuleGUI );
    void PrintSelf (ostream& os, vtkIndent indent );

    // Description:
    // Get methods on class members (no set methods required)
    vtkGetObjectMacro ( Logic, vtkLegoLogic );

    // Description:
    // API for setting VolumeNode, VolumeLogic and
    // for both setting and observing them.
    void SetModuleLogic ( vtkLegoLogic *logic )
            { this->SetLogic ( vtkObjectPointer (&this->Logic), logic ); }
    void SetAndObserveModuleLogic ( vtkLegoLogic *logic )
            { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }

    // Description:
    // This method builds the Lego module GUI
    virtual void BuildGUI ( );

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
    // Deals with mouse events
    void HandleMouseEvent( vtkSlicerInteractorStyle *style );
    
    // Description:
    // Describe behavior at module startup and exit.
    virtual void Enter ( );
    virtual void Exit ( );

    void Init();

    //BTX
    static void DataCallback( vtkObject *caller, unsigned long eid, void *clientData, void *callData );

 protected:

/* FUNCTIONS */

    // Description:
    // Constructor and destructor
    vtkLegoGUI ( );
    virtual ~vtkLegoGUI ( );

    // Description:
    // Connection and configuration of the Lego Mindstorms NXT
    int ConnectToLegoDevice();
    int SetupLegoDevice();

    // Description:
    // Gets robot registration coordinates
    void CalculateRegistrationMatrixForBasicTutorial();
    int GetRobotRegistrationCoordinatesForAdvancedTutorial();
    int CalculateRegistrationMatrixForAdvancedTutorial();

    // Description:
    // Executes the biopsy
    int ExecuteBiopsy();


/* ATTRIBUTES */

    // Description:
    // The connection to the Lego Mindstorms NXT
    NXT_USB *LegoUSB;

    // Description:
    // Connection status to the Lego Mindstorms NXT
    bool Connected;

    // Description:
    // True if we're doing the basic tutorial, false otherwise
    bool BasicTutorial;

    // Description:
    // Goal coordinates of the target
    double TargetInImageCoordinates[3]; // in the image coordinate system (R, A, S)
    double TargetInRobotCoordinates[3]; // in the robot coordinate system (X, Y, Z)

    // Description:
    // Current coordinates of the actuator
    double CurrentActuatorPositionInImageCoordinates[3]; // in the image coordinate system (R, A, S)
    double CurrentActuatorPositionInRobotCoordinates[3]; // in the robot coordinate system (X, Y, Z)
    double CurrentNormalToActuator[3]; // the current normal to the needle actuator, in the image coordianate system

    // Description:
    // Final coordinate of the actuator (X, Y, Z)
    double FinalActuatorPositionInRobotCoordinates[3]; // in the robot coordinate system

    // Description:
    // The current number of motor rotations since last reset (motorA, motorB, motorC)
    double CurrentMotorRotations[3];

    // Description:
    // The motorA values cooresponding to the detected registration points in the robot coordinate system
    double * MotorAValuesForRobotRegistrationCoordinates; 
    
    // Description
    // The coordinates used for registration:  size NUM_REGISTRATION_POINTS x 3
    double ** ImageRegistrationCoordinates; // in the image coordinate system (R, A, S)
    double ** RobotRegistrationCoordinates; // in the robot coordinate system (X, Y, Z)

    // Description:
    // Registration matrices
    vtkMatrix4x4* ImageToRobotRegistrationMatrix; // transforms image coordinates to robot coordinates
    vtkMatrix4x4* RobotToImageRegistrationMatrix; // transforms robot coordinates to image coordinates

    // Description:
    // Module logic and mrml pointers
    vtkLegoLogic *Logic;

    // Description:
    // For "update locator" functionality
    vtkMatrix4x4 *LocatorMatrix;
    vtkMRMLModelNode *LocatorModelNode;
    vtkMRMLModelDisplayNode *LocatorModelDisplayNode;

    //BTX
    std::string LocatorModelID;
    //ETX
    vtkIGTDataManager *DataManager;
    vtkIGTPat2ImgRegistration *Pat2ImgReg;

    vtkCallbackCommand *DataCallbackCommand;

    // Description:
    // Access the slice windows
    vtkSlicerSliceLogic *Logic0;
    vtkSlicerSliceLogic *Logic1;
    vtkSlicerSliceLogic *Logic2;
    vtkMRMLSliceNode *SliceNode0;
    vtkMRMLSliceNode *SliceNode1;
    vtkMRMLSliceNode *SliceNode2;
    vtkSlicerSliceControllerWidget *Control0;
    vtkSlicerSliceControllerWidget *Control1;
    vtkSlicerSliceControllerWidget *Control2;
    int NeedOrientationUpdate0;
    int NeedOrientationUpdate1;
    int NeedOrientationUpdate2;

    // Description:
    // GUI objects for the "Connect to Lego Device" frame
    vtkKWCheckButton *LegoConnectCheckButton;
    vtkKWLabel *LegoConnectLabel;

    // Description:
    // GUI objects for the "Basic Tutorial" frame
    vtkKWRadioButton *BasicTutorialRadioButton;
    vtkKWMultiColumnList *BasicTutorialRegistrationMatrixList;

    // Description:
    // GUI objects for the "Advanced Tutorial" frame
    vtkKWRadioButton *AdvancedTutorialRadioButton;
    vtkKWPushButton *RegistrationScanButton;
    vtkKWLabel *RegistrationScanStatusLabel;
    vtkKWCanvas *RegistrationCanvasTop;
    vtkKWCanvas *RegistrationCanvasBottom;
    vtkKWMultiColumnList *RegistrationRobotCoordinatesList;
    vtkKWPushButton *RegistrationPhantomOKButton;
    vtkKWPushButton *RegistrationPhantomResetButton;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio0;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry0;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton0;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio1;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry1;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton1;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio2;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry2;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton2;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio3;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry3;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton3;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio4;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry4;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton4;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio5;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry5;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton5;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio6;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry6;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton6;
    vtkKWRadioButton *RegistrationImageCoordinatesRadio7;
    vtkKWEntryWithLabel *RegistrationImageCoordinatesEntry7;
    vtkKWPushButton *RegistrationImageCoordinatesOKButton7;
    vtkKWMultiColumnList *SummaryList;

    // Description:
    // GUI objects for the "Planning" frame
    vtkKWRadioButton *TargetCoordinatesRadio;
    vtkKWEntryWithLabel *TargetCoordinatesEntry;
    vtkKWPushButton *AddTargetPushButton;

    // Description:
    // GUI objects for the "Execute Biopsy" frame
    vtkKWMultiColumnList *SummaryRegistrationMatrixList;
    vtkKWText* SummaryTargetImageCoordinateText;
    vtkKWPushButton *ExecuteBiopsyPushButton;
    vtkKWText* SummaryTargetRobotCoordinateText;
    vtkKWText* SummaryFinalRobotCoordinateText;



 private:

/* FUNCTIONS */

    vtkLegoGUI ( const vtkLegoGUI& ); // Not implemented.
    void operator = ( const vtkLegoGUI& ); //Not implemented.

    // Description:
    // For forward kinematics of the robot: find coordinate of actuator given motor rotations
    void ForwardKinematics (double motorA, double motorB, double motorC, bool up, double * returnArray);
    void ForwardKinematics (double motorA, double motorB, double motorC, bool up, double n, double * returnArray);
    double GetAlpha (double motorA);
    double GetAlphaRegistrationTopSwipe (double motorA);
    double GetMotorARegistrationTopSwipe (double alpha);
    double GetAlphaRegistrationBottomSwipe (double motorA);
    double GetMotorARegistrationBottomSwipe (double alpha);
    double GetBetaDown (double motorB);
    double GetBetaUp (double motorB);
    double GetPhiDown (double motorB);
    double GetPhiUp (double motorB);
    double GetN (double motorC);

    // Description:
    // For inverse kinematics of the robot: find motor rotations needed given a goal actuator position
    double InverseKinematics (double X, double Y, double Z, bool up, double * returnArray);
    double GetMotorA (double alpha);
    double GetMotorBBetaDown (double beta);
    double GetMotorBBetaUp (double beta);
    double GetMotorBPhiDown (double phi);
    double GetMotorBPhiUp (double phi);
    double GetMotorC (double n);

    // Description:
    // Gets the number of MotorB rotations that should be made if the arm is going back up,
    // given the number of motorB rotations given to put the arm down
    double DownToUpMotorB(double motorBDown);

    // Description:
    // Get robot coordinates for registration
    int SwipeForRegistrationPoints(int motorBDistance, int maxUSDistance, int ** swipeDistances);
    void PreprocessDistances (int filterWidth, int maxUSDistance, int ** origDistances, int ** goodPoints, int ** filteredDistances);
    void FindMotorAValuesForRobotRegistrationCoordinatesTopSwipe(int ** distances, int numNotError);
    void FindMotorAValuesForRobotRegistrationCoordinatesBottomSwipe(int** distances, int numNotError);
    void CalculateRobotRegistrationCoordinates(double * motorA, double ** returnArray);

    // Description:
    // Functions that do registration
    void ImageToRobot(double * imageCoordinates, double * robotCoordinates);
    void RobotToImage(double * robotCoordinates, double * imageCoordinates);
    
    // Description:
    // Moves a motor while keeping track of current actuator position
    void MoveMotorWithUpdate(int port, int power, int tachoCount);

    // Description:
    // For the "update locator" functionality
    void UpdateAll ( );
    void UpdateLocator ( );
    void UpdateSliceDisplay( float nx, float ny, float nz, 
                            float tx, float ty, float tz, 
                            float px, float py, float pz );

    // Description:
    // Builds the GUI frames
    void BuildGUIForLegoDeviceFrame();
    void BuildGUIForBasicTutorialFrame();
    void BuildGUIForAdvancedTutorialFrame ();
    void BuildGUIForPlanningFrame();
    void BuildGUIForBiopsyFrame();

    // Description:
    // Displays values on a graph
    void CreateBasicGraph(vtkKWCanvas* registrationCanvas);
    void DisplayUltrasoundDistancesOnGraph (int n, int** pts, vtkKWCanvas* registrationCanvas);

    // Description:
    // Small utility functions
    void MedianFilter(int** original, int** filtered, int arrayLength, int filterWidth);
    int Median(int* window, int windowLength);
    void StripCoordinatesFromString(const char* rasText, double * coordinates);
    int GetPower(int motor);
    double RadiansToDegrees (double rad);
    double DegreesToRadians (double deg);

/* ATTRIBUTES */

};

#endif

/* TODO
- Should some of my stuff be virtual?
- should most functions be public or private?
- vtk standards say attributes are protected - ok?
-where to put constants? in .cxx file ok?
*/
