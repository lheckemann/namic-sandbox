
#ifndef __vtkMRMLPerkStationModuleNode_h
#define __vtkMRMLPerkStationModuleNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"



#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"

#include "vtkPerkStationModule.h"

class vtkImageData;
class vtkMRMLLinearTransformNode;
class vtkMRMLFiducialListNode;
class vtkMRMLScalarVolumeNode;


enum VolumeType
{
  VOL_GENERIC, // any other than the specific volumes 
  VOL_CALIBRATE_PLAN,  
  VOL_VALIDATION
};


enum PatientPosition
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


class
VTK_PERKSTATIONMODULE_EXPORT
vtkMRMLPerkStationModuleNode
: public vtkMRMLNode
{

public:

  static vtkMRMLPerkStationModuleNode *New();
  vtkTypeMacro( vtkMRMLPerkStationModuleNode, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Create instance of a PS node.
  virtual vtkMRMLNode* CreateNodeInstance();
  
  // Description:
  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);
  
  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML( ostream& of, int indent );
  
  // Description:
  // Copy the node's attributes to this object
  virtual void Copy( vtkMRMLNode *node );

  // Description:
  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() { return "PS"; };

  // calibrate parameters:
  
  // Description:
  // Get/Set  VerticalFlip(module parameter)
  vtkGetMacro(VerticalFlip, bool);
  vtkSetMacro(VerticalFlip, bool);

  // Description:
  // Get/Set  HorizontalFlip(module parameter)
  vtkGetMacro(HorizontalFlip, bool);
  vtkSetMacro(HorizontalFlip, bool);


  // Description:
  // Get/Set  UserScaling(module parameter)
  vtkGetVector2Macro(UserScaling, double);
  vtkSetVector2Macro(UserScaling, double);

  // Description:
  // Get/Set ActualScaling (module parameter)
  vtkGetVector2Macro(ActualScaling, double);
  vtkSetVector2Macro(ActualScaling, double);

  // Description:
  // Get/Set  UserTranslation(module parameter)
  vtkGetVector2Macro(ClinicalModeTranslation, double);
  vtkSetVector2Macro(ClinicalModeTranslation, double);

  // Description:
  // Get/Set  UserTranslation(module parameter)
  vtkGetVector3Macro(UserTranslation, double);
  vtkSetVector3Macro(UserTranslation, double);

  // Description:
  // Get/Set ActualTranslation (module parameter)
  vtkGetVector3Macro(ActualTranslation, double);
  vtkSetVector3Macro(ActualTranslation, double);

  // Description:
  // Get/Set  CenterOfRotation(module parameter)
  vtkGetVector3Macro(CenterOfRotation, double);
  vtkSetVector3Macro(CenterOfRotation, double);

  // Description:
  // Get/Set ActualRotation (module parameter)
  vtkGetMacro(ClinicalModeRotation, double);
  vtkSetMacro(ClinicalModeRotation, double);

  // Description:
  // Get/Set  UserRotation(module parameter)
  vtkGetMacro(UserRotation, double);
  vtkSetMacro(UserRotation, double);

  // Description:
  // Get/Set ActualRotation (module parameter)
  vtkGetMacro(ActualRotation, double);
  vtkSetMacro(ActualRotation, double);

  // Description:
  // Get/Set the Transform Node that encapsulates translation, rotation, scaling
  vtkGetObjectMacro( CalibrationMRMLTransformNode, vtkMRMLLinearTransformNode );
  //void SetTransformNodeMatrix(vtkMatrix4x4 *matrix);
  
  // plan parameters:
  
  // Description:
  // Get/Set the Fiducial list node that contains entry point and target point
  vtkGetObjectMacro( PlanMRMLFiducialListNode, vtkMRMLFiducialListNode );


  // Description:
  // Get/Set  PlanEntryPoint(module parameter)
  vtkGetVector3Macro( PlanEntryPoint, double );
  vtkSetVector3Macro( PlanEntryPoint, double );

  // Description:
  // Get/Set  PlanTargetPoint(module parameter)
  vtkGetVector3Macro( PlanTargetPoint, double );
  vtkSetVector3Macro( PlanTargetPoint, double );

  // Description:
  // Get/Set  UserInsertionAngle(module parameter)
  vtkGetMacro( UserPlanInsertionAngle, double );
  vtkSetMacro( UserPlanInsertionAngle, double );

  // Description:
  // Get/Set ActualInsertionAngle (module parameter)
  vtkGetMacro( ActualPlanInsertionAngle, double );
  vtkSetMacro( ActualPlanInsertionAngle, double );

  // Get/Set  TiltAngle(module parameter)
  vtkGetMacro( TiltAngle, double );
  vtkSetMacro( TiltAngle, double );

  void SetOriginalSliceToRAS(vtkMatrix4x4 *matrix){this->OriginalSliceToRAS->DeepCopy(matrix);};
  vtkMatrix4x4 *GetOriginalSliceToRAS(){return this->OriginalSliceToRAS;};

  void SetTiltSliceToRAS(vtkMatrix4x4 *matrix){this->TiltSliceToRAS->DeepCopy(matrix);};
  vtkMatrix4x4 *GetTiltSliceToRAS(){return this->TiltSliceToRAS;};

  // Description:
  // Get/Set  UserInsertionDepth(module parameter)
  vtkGetMacro(UserPlanInsertionDepth, double);
  vtkSetMacro(UserPlanInsertionDepth, double);

  // Description:
  // Get/Set ActualInsertionDepth (module parameter)
  vtkGetMacro(ActualPlanInsertionDepth, double);
  vtkSetMacro(ActualPlanInsertionDepth, double);

  // insert parameters:
  vtkGetMacro(ReferenceBodyToolPort, int);
  vtkSetMacro(ReferenceBodyToolPort, int);

  vtkGetMacro(NeedleToolPort, int);
  vtkSetMacro(NeedleToolPort, int);

  void SetTrackerToPhantomMatrix(vtkMatrix4x4 *matrix){this->TrackerToPhantomMatrix->DeepCopy(matrix);};
  vtkMatrix4x4 *GetTrackerToPhantomMatrix(){return this->TrackerToPhantomMatrix;};

  void SetPhantomToImageRASMatrix(vtkMatrix4x4 *matrix){this->PhantomToImageRASMatrix->DeepCopy(matrix);};
  vtkMatrix4x4 *GetPhantomToImageRASMatrix(){return this->PhantomToImageRASMatrix;};

  // Description:
  // Get/Set tool tip offset. This offset (x, y, z) is added to needle tool coordinate measured by the tracker.
  // The needle tool is usually pre-calibrated by the manufacturer and the tip offset is stored in its SROM,
  // but it is not loaded on an old tracker (e.g. Aurora firmware Rev.006 or earlier version). Also it is usually recommended
  // to do pivoting calibration just before each experiment in order to ensure the accuracy. So this variable
  // is used to modify the offset in such case.
  void SetToolTipOffset(double *tool_tip_offset){memcpy(this->ToolTipOffset, tool_tip_offset, sizeof(double)*3);};
  double *GetToolTipOffset(){return this->ToolTipOffset;};

  // validate parameters:
 
  // Description:
  // Get/Set  ValidateEntryPoint(module parameter)
  vtkGetVector3Macro(ValidateEntryPoint, double);
  vtkSetVector3Macro(ValidateEntryPoint, double);

  // Description:
  // Get/Set  ValidateTargetPoint(module parameter)
  vtkGetVector3Macro(ValidateTargetPoint, double);
  vtkSetVector3Macro(ValidateTargetPoint, double);

  // Description:
  // Get/Set ValidateInsertionDepth (module parameter)
  vtkGetMacro(ValidateInsertionDepth, double);
  vtkSetMacro(ValidateInsertionDepth, double);


  // evaluate parameters:
  vtkGetVector2Macro( CalibrateTranslationError, double );  
  vtkGetMacro( CalibrateRotationError, double );
  vtkGetVector2Macro( CalibrateScaleError, double );
  vtkGetMacro( PlanInsertionAngleError, double );
  vtkGetMacro( PlanInsertionDepthError, double );
  vtkGetMacro( EntryPointError, double );
  vtkGetMacro( TargetPointError, double );


  // common:
  vtkSetMacro( TimeSpentOnCalibrateStep, double );
  vtkGetMacro( TimeSpentOnCalibrateStep, double );

  vtkSetMacro( TimeSpentOnPlanStep, double );
  vtkGetMacro( TimeSpentOnPlanStep, double );

  vtkSetMacro( TimeSpentOnInsertStep, double );
  vtkGetMacro( TimeSpentOnInsertStep, double );

  vtkSetMacro( TimeSpentOnValidateStep, double );
  vtkGetMacro( TimeSpentOnValidateStep, double );


  // Description:

  // Get/Set volume in use
  vtkGetStringMacro( VolumeInUse );
  vtkSetStringMacro( VolumeInUse );

  // Get/Set input volume MRML Id
  vtkGetStringMacro( PlanningVolumeRef );
  vtkSetStringMacro( PlanningVolumeRef );

  vtkGetStringMacro( ValidationVolumeRef );
  vtkSetStringMacro( ValidationVolumeRef );

  // calibration/planning volume node
  vtkMRMLScalarVolumeNode *GetPlanningVolumeNode()
  { return this->PlanningVolumeNode; };
  void SetPlanningVolumeNode( vtkMRMLScalarVolumeNode *planVolNode );
  
  // validation volume node
  vtkMRMLScalarVolumeNode *GetValidationVolumeNode()
  { return this->ValidationVolumeNode; };
  void SetValidationVolumeNode( vtkMRMLScalarVolumeNode *validationVolNode );

  vtkMRMLScalarVolumeNode *GetActiveVolumeNode();

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID( const char *oldID, const char *newID );

  

  void CalculateCalibrateScaleError();
  void CalculateCalibrateTranslationError();
  void CalculateCalibrateRotationError();

  void CalculatePlanInsertionAngleError();
  void CalculatePlanInsertionDepthError();

  void CalculateEntryPointError();
  void CalculateTargetPointError();

  void AddVolumeInformationToList( vtkMRMLScalarVolumeNode *volNode,
                                   const char *diskLocation, char *type ); 
  
  int GetNumberOfSteps();
  int GetCurrentStep();
  int GetPreviousStep();
  int SwitchStep(int newStep);
  const char* GetStepName(int i);
  
  
  PatientPosition GetPatientPosition();
  
  vtkGetMacro( TableAtScanner, double );
  vtkSetMacro( TableAtScanner, double );
  
  vtkGetMacro( TableAtOverlay, double );
  vtkSetMacro( TableAtOverlay, double );
  
  
private:
  PatientPosition m_PatientPosition;
  
  
protected:

  vtkMRMLPerkStationModuleNode();
  ~vtkMRMLPerkStationModuleNode();
  vtkMRMLPerkStationModuleNode(const vtkMRMLPerkStationModuleNode&);
  void operator=(const vtkMRMLPerkStationModuleNode&);

   // the slicer's volume transform node
  vtkMRMLLinearTransformNode *CalibrationMRMLTransformNode;

  void InitializeTransform();

  // slicer's fiducial list node for marking entry and target points
  vtkMRMLFiducialListNode *PlanMRMLFiducialListNode;
  
  void InitializeFiducialListNode();
  
  
  // calibrate parameters:

  // "Clinical mode"
  double ClinicalModeRotation;
  double ClinicalModeTranslation[2];
  
  double SliceCenterOfRotation[ 2 ];
  double SliceRotation;
  
  
  
  // "Training mode"
  // flip parameters
  bool VerticalFlip;
  bool HorizontalFlip;

  // scale parameters
  double UserScaling[2];
  double ActualScaling[2];

  // translate parameters
  double UserTranslation[3];
  double ActualTranslation[3];

  // rotate parameters
  double UserRotation;  
  double ActualRotation;
  double CenterOfRotation[3];
  
  
    // plan parameters
    
  double PlanEntryPoint[3];
  double PlanTargetPoint[3];

  double UserPlanInsertionAngle;
  double ActualPlanInsertionAngle;

  double UserPlanInsertionDepth;
  double ActualPlanInsertionDepth;
  
  
  // out of plane tilt angle applicable for new perk station design
  // this is applicable when the entry and target are not on the same slice
  // an oblique slice has to be extracted that contains both entry and target
  double TiltAngle;

  vtkMatrix4x4 *OriginalSliceToRAS;
  vtkMatrix4x4 *TiltSliceToRAS;

  
  // insert parameters
  vtkMatrix4x4 *TrackerToPhantomMatrix;
  vtkMatrix4x4 *PhantomToImageRASMatrix;
  double ToolTipOffset[3];

  int ReferenceBodyToolPort;
  int NeedleToolPort;


  // validate parameters
  double ValidateEntryPoint[3];
  double ValidateTargetPoint[3];

  double ValidateInsertionDepth;
  // evaluate parameters

  double CalibrateTranslationError[2]; // error in mm
  double CalibrateScaleError[2]; // expressed in %ge
  double CalibrateRotationError; // in degrees

  double PlanInsertionAngleError; // in degrees
  double PlanInsertionDepthError; // in mm

  double EntryPointError; // error in mm
  double TargetPointError; //error in mm

  // elapsed time at each step
  double TimeSpentOnCalibrateStep;
  double TimeSpentOnPlanStep;
  double TimeSpentOnInsertStep;
  double TimeSpentOnValidateStep;


  // common parameters  
  char* PlanningVolumeRef;
  char* ValidationVolumeRef;
  char* VolumeInUse; 

  // scalar volume node
  vtkMRMLScalarVolumeNode *PlanningVolumeNode;
  vtkMRMLScalarVolumeNode *ValidationVolumeNode;
  

  

  //keep track of all the volumes that were opened, maintain a list
  //BTX
  typedef struct{
    std::string Type;
    std::string DiskLocation;
    bool Active;
    vtkMRMLScalarVolumeNode *VolumeNode;
  } VolumeInformationStruct;
  std::vector<VolumeInformationStruct> VolumesList;
  //ETX
  
  
  double TableAtScanner;
  double TableAtOverlay;
  

protected:

  vtkStringArray *StepList;
  int CurrentStep;
  int PreviousStep;

};

#endif

