
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
#include "vtkPerkStationPlan.h"

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


struct OverlayHardware
{
  std::string Name;
  double SizeX;
  double SizeY;
  bool FlipHorizontal;
  bool FlipVertical;
};


//BTX
// Struct to hold calibration data.
class OverlayCalibration
{
public:
  
  OverlayCalibration()
  {
    this->Name = "";
    this->SecondMonitorHorizontalFlip = false;
    this->SecondMonitorRotation = 0.0;
    this->SecondMonitorRotationCenter[ 0 ] = 0.0;
    this->SecondMonitorRotationCenter[ 1 ] = 0.0;
    this->SecondMonitorTranslation[ 0 ] = 0.0;
    this->SecondMonitorTranslation[ 1 ] = 0.0;
    this->SecondMonitorVerticalFlip = false;
  }
  
  std::string Name;
  double SecondMonitorRotation;
  double SecondMonitorRotationCenter[ 2 ];
  double SecondMonitorTranslation[ 2 ];
  
  bool SecondMonitorHorizontalFlip;
  bool SecondMonitorVerticalFlip;
};
//ETX


// ============================================================================


class
VTK_PERKSTATIONMODULE_EXPORT
vtkMRMLPerkStationModuleNode
: public vtkMRMLNode
{

public:

  static vtkMRMLPerkStationModuleNode *New();
  vtkTypeMacro( vtkMRMLPerkStationModuleNode, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  
    // Standard MRML functions. -----------------------------------------------
  
  virtual vtkMRMLNode* CreateNodeInstance();
  
  void Init();
  
  virtual void ReadXMLAttributes( const char** atts);
  virtual void WriteXML( ostream& of, int indent );
  
    // Copy the node's attributes to this object
  virtual void Copy( vtkMRMLNode *node );

    // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {
    return "PS";
  };
  
  virtual void UpdateScene( vtkMRMLScene* );
  virtual void UpdateReferenceID( const char *oldID, const char *newID );
  void UpdateReferences();
  
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  
  // Calibration parameters ---------------------------------------------------
  
  double GetSecondMonitorRotation() const;
  void SetSecondMonitorRotation( double rot );
  
  void GetSecondMonitorRotationCenter( double* rotCent) const;
  void SetSecondMonitorRotationCenter( const double* rotCent);
  
  bool GetSecondMonitorVerticalFlip() const;
  void SetSecondMonitorVerticalFlip( bool flip );
  
  bool GetSecondMonitorHorizontalFlip() const;
  void SetSecondMonitorHorizontalFlip( bool flip );
  
  void GetSecondMonitorTranslation( double* tx ) const;
  void SetSecondMonitorTranslation( const double* tx );
  
  double GetTableAtOverlay() const;
  void SetTableAtOverlay( double pos );
  
  vtkGetMacro( CurrentSliceOffset, double );
  vtkSetMacro( CurrentSliceOffset, double );
  
  vtkGetMacro( HardwareIndex, int );
  vtkSetMacro( HardwareIndex, int );
  
  void UpdateHardwareCalibration( double mmX, double mmY );
  void GetSecondMonitorPhysicalSize( double* mm ) const;
  
  
    // Calibration list management.
  
  unsigned int AddCalibration( OverlayCalibration* newCalibration );
  int RemoveCalibrationAtIndex( unsigned int index );
  OverlayCalibration* GetCalibrationAtIndex( unsigned int index );
  int GetNumberOfCalibrations() { return this->CalibrationList.size(); };
  vtkGetMacro( CurrentCalibration, int );
  int SetCurrentCalibrationIndex( int index );
  
  
    // Plan parameters --------------------------------------------------------
  
  
  void GetPlanEntryPoint( double* point, int index = -1 ) const;
  void SetPlanEntryPoint( const double* point );
  
  void GetPlanTargetPoint( double* point, int index = -1 ) const;
  void SetPlanTargetPoint( const double* point );
  
  
  vtkGetMacro( TiltAngle, double );
  vtkSetMacro( TiltAngle, double );
  
  void SetSliceToRAS( vtkMatrix4x4* matrix ) {
    this->SliceToRAS->DeepCopy( matrix );
  };
  
  vtkMatrix4x4* GetSliceToRAS() {
    return this->SliceToRAS.GetPointer();
  };
  
  void AddNewPlan();
  
    // Plan list management.
  
  unsigned int AddPlan( vtkPerkStationPlan* newPlan );
  int RemovePlanAtIndex( unsigned int index );
  vtkPerkStationPlan *GetPlanAtIndex( unsigned int index );
  int GetNumberOfPlans() const { return this->PlanList.size(); };
  vtkGetMacro( CurrentPlanIndex, int );
  int SetCurrentPlanIndex( int index );
  
  
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
  
  bool GetValidated() const;
  void SetValidated( const bool validated );
  int GetNumberOfValidations();
  
  void GetValidationEntryPoint( double* point, int index = -1 ) const;
  void SetValidationEntryPoint( const double* point );
  
  void GetValidationTargetPoint( double* point, int index = -1 ) const;
  void SetValidationTargetPoint( const double* point );
  
  
    // Common parameters ------------------------------------------------------
  
  vtkGetStringMacro( VolumeInUse );
  vtkSetStringMacro( VolumeInUse );
  
  vtkSetMacro( TimeOnCalibrateStep, double );
  vtkGetMacro( TimeOnCalibrateStep, double );
  
  vtkSetMacro( TimeOnPlanStep, double );
  vtkGetMacro( TimeOnPlanStep, double );
  
  vtkSetMacro( TimeOnInsertStep, double );
  vtkGetMacro( TimeOnInsertStep, double );
  
  vtkSetMacro( TimeOnValidateStep, double );
  vtkGetMacro( TimeOnValidateStep, double );
  
  
  vtkMRMLScalarVolumeNode* GetActiveVolumeNode();
  
  int GetNumberOfSteps();
  int GetCurrentStep();
  int GetPreviousStep();
  int SwitchStep( int newStep );
  const char* GetStepName( int i );
  
  PatientPositionEnum GetPatientPosition();
  
  //BTX
  std::vector< OverlayHardware > GetHardwareList();
  //ETX
  
  
  // Computations -------------------------------------------------------------
  
  double GetCurrentTablePosition();
  
  bool GetFinalHorizontalFlip();
  bool GetFinalVerticalFlip();
  
  double GetEntryPointError( int index = -1 );
  double GetEntryPointErrorR( int index = -1 );
  double GetEntryPointErrorA( int index = -1 );
  double GetEntryPointErrorS( int index = -1 );
  double GetTargetPointError( int index = -1 );
  double GetTargetPointErrorR( int index = -1 );
  double GetTargetPointErrorA( int index = -1 );
  double GetTargetPointErrorS( int index = -1 );
  
  double GetActualPlanInsertionAngle( int index = -1 );
  double GetActualPlanInsertionDepth( int index = -1 );
  double GetPlanAngleAxial( int index = -1 );
  double GetPlanAngleSagittal( int index = -1 );
  
  double GetPlanDepth( int index = -1 );
  double GetValidationAngle( int index = -1 );
  double GetValidationDepth( int index = -1 );
  
  double GetDepthError( int index = -1 );
  double GetAngleError( int index = -1 );
  double GetAngleErrorAxial( int index = -1 );
  double GetAngleErrorSagittal( int index = -1 );
  
  
    // Fiducial list containing two fiducials: Entry point and Target point.
  
public:
  vtkGetStringMacro( PlanFiducialsNodeID );
  vtkMRMLFiducialListNode* GetPlanFiducialsNode();
  void SetAndObservePlanFiducialsNodeID( const char* planFiducialsNodeID );
  void RecreateFiducialsNode();
private:
  vtkSetReferenceStringMacro( PlanFiducialsNodeID );
  char* PlanFiducialsNodeID;
  vtkMRMLFiducialListNode* PlanFiducialsNode;
  
  
    // Reference to the planning volume.
 
public:
  vtkGetStringMacro( PlanningVolumeNodeID );
  vtkSetReferenceStringMacro( PlanningVolumeNodeID );
  vtkMRMLScalarVolumeNode* GetPlanningVolumeNode();
private:
  char* PlanningVolumeNodeID;
  
  
    // Reference to the validation volume.

public:
  vtkGetStringMacro( ValidationVolumeNodeID );
  vtkSetReferenceStringMacro( ValidationVolumeNodeID );
  vtkMRMLScalarVolumeNode* GetValidationVolumeNode();
private:
  char* ValidationVolumeNodeID;
 
 
protected:
  
  //BTX
  
  vtkMRMLPerkStationModuleNode();
  ~vtkMRMLPerkStationModuleNode();
  vtkMRMLPerkStationModuleNode( const vtkMRMLPerkStationModuleNode& );
  void operator=( const vtkMRMLPerkStationModuleNode& );
  
  
    // Calibration parameters -------------------------------------------------
  
  std::vector< OverlayCalibration* > CalibrationList;
  int CurrentCalibration; // If <0, no calibration is selected.
  unsigned int CalibrationUID;
  
  double TableAtOverlay;
  
  std::vector< OverlayHardware > HardwareList;
  int HardwareIndex;
  
  
    // Plan parameters --------------------------------------------------------
  
  double TiltAngle;
  
  std::vector< vtkSmartPointer< vtkPerkStationPlan > > PlanList;
  int CurrentPlanIndex; // If <0, no plan is selected.
  
  vtkSmartPointer< vtkMatrix4x4 > SliceToRAS;
  
  unsigned int PlanUID;
  
  
    // Insertion parameters ---------------------------------------------------
  
  vtkSmartPointer< vtkMatrix4x4 > TrackerToPhantomMatrix;
  vtkSmartPointer< vtkMatrix4x4 > PhantomToImageRASMatrix;
  double ToolTipOffset[ 3 ];
  
  int ReferenceBodyToolPort;
  int NeedleToolPort;
  
 
    // Common parameters ------------------------------------------------------
  
  char* VolumeInUse;
  
  double CurrentSliceOffset;    // In RAS.
  PatientPositionEnum PatientPosition;
  
  double TimeOnCalibrateStep;
  double TimeOnPlanStep;
  double TimeOnInsertStep;
  double TimeOnValidateStep;
  
  vtkSmartPointer< vtkStringArray > StepList;
  
  int CurrentStep;
  int PreviousStep;
  
  
    // keep track of all the volumes that were opened, maintain a list
  
  typedef struct {
    std::string Type;
    std::string DiskLocation;
    bool Active;
    vtkMRMLScalarVolumeNode* VolumeNode;
  } VolumeInformationStruct;
  
  std::vector< VolumeInformationStruct > VolumesList;
  

private:
  
  void UpdateHardwareListFile();
  
  std::string HardwareListFileName;
  
  bool Initialized;
  
  //ETX
};

#endif

