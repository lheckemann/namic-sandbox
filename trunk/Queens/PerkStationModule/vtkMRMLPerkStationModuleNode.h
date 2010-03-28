
#ifndef __vtkMRMLPerkStationModuleNode_h
#define __vtkMRMLPerkStationModuleNode_h

#include <string>
#include <vector>

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"

#include "vtkPerkStationModule.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLLinearTransformNode.h"


class vtkImageData;
class vtkMRMLScalarVolumeNode;



// PerkStation specific enums -------------------------------------------------

enum
{
  WORKPHASE_CALIBRATION = 0,
  WORKPHASE_PLANNING = 1,
  WORKPHASE_INSERTION = 2,
  WORKPHASE_VALIDATION = 3
};

enum VolumeType
{
  VOL_GENERIC, // any other than the specific volumes 
  VOL_CALIBRATE_PLAN,  
  VOL_VALIDATION
};

enum PatientPositionEnum
  {
  PPNA, // Not defined
  HFP,  // Head First-Prone
  HFS,  // Head First-Supine
  HFDR, // Head First-Decubitus Right
  HFDL, // Head First-Decubitus Left
  FFDR, // Feet First-Decubitus Right
  FFDL, // Feet First-Decubitus Left
  FFP,  // Feet First-Prone
  FFS   // Feet First-Supine
  };
  
enum OverlayMonitor
{
  MONITOR_SIEMENS = 0,
  MONITOR_VIEWSONIC = 1,
  MONITOR_ACER = 2
};

// ----------------------------------------------------------------------------


class
VTK_PERKSTATIONMODULE_EXPORT
vtkMRMLPerkStationModuleNode
: public vtkMRMLNode
{

public:

  static vtkMRMLPerkStationModuleNode *New();
  vtkTypeMacro( vtkMRMLPerkStationModuleNode, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual vtkMRMLNode* CreateNodeInstance();
  
  virtual void ReadXMLAttributes( const char** atts);
  virtual void WriteXML( ostream& of, int indent );
  
    // Copy the node's attributes to this object
  virtual void Copy( vtkMRMLNode *node );

    // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {
    return "PS";
  };
  
  virtual void SaveClibration( std::ostream& out );
  virtual bool LoadCalibration( std::istream& in );
  
  virtual void SaveExperiment( std::ostream& out );
  virtual bool LoadExperiment( std::istream& in );
  
  
  // Calibration parameters ---------------------------------------------------
  
  vtkGetMacro( SecondMonitorRotation, double );
  vtkSetMacro( SecondMonitorRotation, double );
  
  vtkGetVector2Macro( SecondMonitorRotationCenter, double );
  vtkSetVector2Macro( SecondMonitorRotationCenter, double );
  
  vtkGetMacro( SecondMonitorVerticalFlip, bool );
  vtkSetMacro( SecondMonitorVerticalFlip, bool );
  
  vtkGetMacro( SecondMonitorHorizontalFlip, bool );
  vtkSetMacro( SecondMonitorHorizontalFlip, bool );
  
  vtkGetVector2Macro( SecondMonitorTranslation, double );
  vtkSetVector2Macro( SecondMonitorTranslation, double );
  
  vtkGetMacro( TableAtScanner, double );
  vtkSetMacro( TableAtScanner, double );
  
  vtkGetMacro( TableAtOverlay, double );
  vtkSetMacro( TableAtOverlay, double );
  
  vtkGetMacro( CurrentSliceOffset, double );
  vtkSetMacro( CurrentSliceOffset, double );
  
  vtkSmartPointer< vtkMRMLLinearTransformNode >
  GetCalibrationMRMLTransformNode() {
    return this->CalibrationMRMLTransformNode;
  }
  
  
  // Plan parameters ----------------------------------------------------------
  
  vtkGetVector3Macro( PlanEntryPoint, double );
  vtkSetVector3Macro( PlanEntryPoint, double );
  
  vtkGetVector3Macro( PlanTargetPoint, double );
  vtkSetVector3Macro( PlanTargetPoint, double );
  
  vtkGetMacro( TiltAngle, double );
  vtkSetMacro( TiltAngle, double );
  
  void SetSliceToRAS( vtkMatrix4x4* matrix ) {
    this->SliceToRAS->DeepCopy( matrix );
  };
  
  vtkMatrix4x4* GetSliceToRAS() {
    return this->SliceToRAS.GetPointer();
  };
  
  
    // Insertion parameters ---------------------------------------------------
  
  vtkGetMacro( ReferenceBodyToolPort, int );
  vtkSetMacro( ReferenceBodyToolPort, int );
  
  vtkGetMacro( NeedleToolPort, int );
  vtkSetMacro( NeedleToolPort, int );
  
  void SetTrackerToPhantomMatrix( vtkMatrix4x4 *matrix ) {
    this->TrackerToPhantomMatrix->DeepCopy( matrix );
  };
  
  vtkMatrix4x4* GetTrackerToPhantomMatrix() {
    return this->TrackerToPhantomMatrix;
  };
  
  void SetPhantomToImageRASMatrix( vtkMatrix4x4* matrix ) {
    this->PhantomToImageRASMatrix->DeepCopy( matrix );
  };
  
  vtkMatrix4x4* GetPhantomToImageRASMatrix() {
    return this->PhantomToImageRASMatrix;
  };
  
  
  // Description:
  // Get/Set tool tip offset. This offset (x, y, z) is added to needle tool
  // coordinate measured by the tracker. The needle tool is usually
  // pre-calibrated by the manufacturer and the tip offset is stored in its SROM,
  // but it is not loaded on an old tracker (e.g. Aurora firmware Rev.006 or
  // earlier version). Also it is usually recommended
  // to do pivoting calibration just before each experiment in order to ensure
  // the accuracy. So this variable is used to modify the offset in such case.
  
  void SetToolTipOffset( double *tool_tip_offset ) {
    memcpy( this->ToolTipOffset, tool_tip_offset, sizeof( double ) * 3 );
  };
  
  double *GetToolTipOffset() {
    return this->ToolTipOffset;
  };
  
  
    // Validation parameters --------------------------------------------------
  
  vtkGetVector3Macro(ValidateEntryPoint, double);
  vtkSetVector3Macro(ValidateEntryPoint, double);

  vtkGetVector3Macro(ValidateTargetPoint, double);
  vtkSetVector3Macro(ValidateTargetPoint, double);


    // Common parameters ------------------------------------------------------
  
  vtkGetStringMacro( PlanningVolumeRef );
  vtkSetStringMacro( PlanningVolumeRef );
  
  vtkGetStringMacro( ValidationVolumeRef );
  vtkSetStringMacro( ValidationVolumeRef );
  
  vtkGetStringMacro( VolumeInUse );
  vtkSetStringMacro( VolumeInUse );
  
  vtkMRMLScalarVolumeNode* GetPlanningVolumeNode();
  
  void SetPlanningVolumeNode( vtkMRMLScalarVolumeNode *planVolNode );
  
    // validation volume node
  vtkMRMLScalarVolumeNode *GetValidationVolumeNode()
    { return this->ValidationVolumeNode; };
  void SetValidationVolumeNode( vtkMRMLScalarVolumeNode *validationVolNode );
  
  vtkSetMacro( TimeSpentOnCalibrateStep, double );
  vtkGetMacro( TimeSpentOnCalibrateStep, double );

  vtkSetMacro( TimeSpentOnPlanStep, double );
  vtkGetMacro( TimeSpentOnPlanStep, double );

  vtkSetMacro( TimeSpentOnInsertStep, double );
  vtkGetMacro( TimeSpentOnInsertStep, double );

  vtkSetMacro( TimeSpentOnValidateStep, double );
  vtkGetMacro( TimeSpentOnValidateStep, double );
  
  
  vtkMRMLFiducialListNode* GetPlanMRMLFiducialListNode() {
    return this->PlanMRMLFiducialListNode.GetPointer();
  }
  
  
  vtkMRMLScalarVolumeNode* GetActiveVolumeNode();

    // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID( const char *oldID, const char *newID );
  
  void AddVolumeInformationToList( vtkMRMLScalarVolumeNode *volNode,
                                   const char *diskLocation, char *type ); 
  
  int GetNumberOfSteps();
  int GetCurrentStep();
  int GetPreviousStep();
  int SwitchStep( int newStep );
  const char* GetStepName( int i );
  
  
  PatientPositionEnum GetPatientPosition();
  
  double GetCurrentTablePosition();
  
  
  // Computations -------------------------------------------------------------
  
  double GetEntryPointError();
  double GetTargetPointError();
  
  double GetActualPlanInsertionAngle();
  double GetActualPlanInsertionDepth();
  
  double GetValidationDepth();
  
protected:
  
  vtkMRMLPerkStationModuleNode();
  ~vtkMRMLPerkStationModuleNode();
  vtkMRMLPerkStationModuleNode( const vtkMRMLPerkStationModuleNode& );
  void operator=( const vtkMRMLPerkStationModuleNode& );
  
  void InitializeTransform();

  void InitializeFiducialListNode();
  
  
  // Calibration parameters ---------------------------------------------------
  
  double SecondMonitorRotation;
  double SecondMonitorRotationCenter[ 2 ];
  double SecondMonitorTranslation[ 2 ];
  
  bool SecondMonitorHorizontalFlip;
  bool SecondMonitorVerticalFlip;
  
  double TableAtScanner;
  double TableAtOverlay;
  double CurrentSliceOffset;    // In RAS.
  
   // Slicer's volume transform node.
  vtkSmartPointer< vtkMRMLLinearTransformNode > CalibrationMRMLTransformNode;
  
  PatientPositionEnum PatientPosition;
  
  
    // Plan parameters --------------------------------------------------------
  
  double PlanEntryPoint[ 3 ];
  double PlanTargetPoint[ 3 ];
  
  double TiltAngle;
  
  vtkSmartPointer< vtkMatrix4x4 > SliceToRAS;
  
  
    // Insertion parameters ---------------------------------------------------
  
  vtkSmartPointer< vtkMatrix4x4 > TrackerToPhantomMatrix;
  vtkSmartPointer< vtkMatrix4x4 > PhantomToImageRASMatrix;
  double ToolTipOffset[ 3 ];
  
  int ReferenceBodyToolPort;
  int NeedleToolPort;
  
  
    // Validation parameters --------------------------------------------------
  
  double ValidateEntryPoint[ 3 ];
  double ValidateTargetPoint[ 3 ];
  
  
    // Common parameters ------------------------------------------------------
  
  char* PlanningVolumeRef;
  char* ValidationVolumeRef;
  char* VolumeInUse; 
  
  vtkMRMLScalarVolumeNode* PlanningVolumeNode;
  vtkMRMLScalarVolumeNode* ValidationVolumeNode;
  
  double TimeSpentOnCalibrateStep;
  double TimeSpentOnPlanStep;
  double TimeSpentOnInsertStep;
  double TimeSpentOnValidateStep;
  
  vtkSmartPointer< vtkMRMLFiducialListNode > PlanMRMLFiducialListNode;
  
  vtkSmartPointer< vtkStringArray > StepList;
  int CurrentStep;
  int PreviousStep;
  
  
  //keep track of all the volumes that were opened, maintain a list
  //BTX
  typedef struct {
    std::string Type;
    std::string DiskLocation;
    bool Active;
    vtkMRMLScalarVolumeNode* VolumeNode;
  } VolumeInformationStruct;
  
  std::vector< VolumeInformationStruct > VolumesList;
  //ETX
  
};

#endif
