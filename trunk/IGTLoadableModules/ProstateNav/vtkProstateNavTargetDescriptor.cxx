#include "vtkProstateNavTargetDescriptor.h"
#include "vtkObjectFactory.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavTargetDescriptor);
vtkCxxRevisionMacro(vtkProstateNavTargetDescriptor, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
/// Constructor
vtkProstateNavTargetDescriptor::vtkProstateNavTargetDescriptor()
{
  this->Name="";  
  this->NeedleLength=0.0;
  this->NeedleType = "";
  this->FiducialID = "";
  this->NeedleOvershoot=0.0;
  this->TargetValidated = false;
  this->NeedleTipValidationPosition[0]=0;
  this->NeedleTipValidationPosition[1]=0;
  this->NeedleTipValidationPosition[2]=0;
  this->NeedleBaseValidationPosition[0]=0;
  this->NeedleBaseValidationPosition[1]=0;
  this->NeedleBaseValidationPosition[2]=0;
  this->CalibrationFoR_STR.clear();
  this->TargetingFoR_STR.clear();
  this->ValidationVolumeFoR_STR.clear();
  this->OverallError = 0.0;
  this->APError = 0.0;
  this->LRError = 0.0;
  this->ISError = 0.0;
  this->RASLocation[0] = 0;
  this->RASLocation[1] = 0;
  this->RASLocation[2] = 0;
  this->RASOrientation[0] = 0;
  this->RASOrientation[1] = 0;
  this->RASOrientation[2] = 0;
  this->RASOrientation[3] = 0;
}


/// Destructor
vtkProstateNavTargetDescriptor::~vtkProstateNavTargetDescriptor(void)
{
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetDescriptor::DeepCopy(vtkProstateNavTargetDescriptor& src)
{
  this->Name=src.Name;
  this->NeedleLength=src.NeedleLength;
  this->NeedleType =src.NeedleType;
  this->FiducialID = src.FiducialID;
  this->NeedleOvershoot=src.NeedleOvershoot;
  this->TargetValidated =src.TargetValidated;
  this->NeedleTipValidationPosition[0]=src.NeedleTipValidationPosition[0];
  this->NeedleTipValidationPosition[1]=src.NeedleTipValidationPosition[1];
  this->NeedleTipValidationPosition[2]=src.NeedleTipValidationPosition[2];
  this->NeedleBaseValidationPosition[0]=src.NeedleBaseValidationPosition[0];
  this->NeedleBaseValidationPosition[1]=src.NeedleBaseValidationPosition[1];
  this->NeedleBaseValidationPosition[2]=src.NeedleBaseValidationPosition[2];
  this->CalibrationFoR_STR =src.CalibrationFoR_STR;
  this->TargetingFoR_STR =src.TargetingFoR_STR;
  this->ValidationVolumeFoR_STR=src.ValidationVolumeFoR_STR;
  this->OverallError=src.OverallError;
  this->APError=src.APError;
  this->LRError=src.LRError;
  this->ISError=src.ISError;
  this->RASLocation[0]=src.RASLocation[0];
  this->RASLocation[1] =src.RASLocation[1];
  this->RASLocation[2] =src.RASLocation[2];
  this->RASOrientation[0] = src.RASOrientation[0];
  this->RASOrientation[1] = src.RASOrientation[1];
  this->RASOrientation[2] = src.RASOrientation[2];
  this->RASOrientation[3] = src.RASOrientation[3];
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetDescriptor::PrintSelf(ostream& os, vtkIndent indent)
{
  
  this->Superclass::PrintSelf(os,indent);

  /*
  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 */
}
