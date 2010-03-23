#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkObjectFactory.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyTargetDescriptor);
vtkCxxRevisionMacro(vtkTRProstateBiopsyTargetDescriptor, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
/// Constructor
vtkTRProstateBiopsyTargetDescriptor::vtkTRProstateBiopsyTargetDescriptor()
{
    this->AxisRotation=0;
    this->NeedleAngle=0;
    this->DepthCM=0;
    this->NeedleDepth=0.0;
    this->NeedleTypeUID=-2;
    this->NeedleType = "";
    this->NeedleOvershoot=0.0;
    this->TargetValidated = false;
    this->FoR_STR = "";
    this->ValidationVolumeFoR_STR = "";
    this->OverallError = 0.0;
    this->APError = 0.0;
    this->LRError = 0.0;
    this->ISError = 0.0;
    this->IsOutsideReach=true;
    this->RASLocation[0] = 0;
    this->RASLocation[1] = 0;
    this->RASLocation[2] = 0;
    this->HingePosition[0] = 0;
    this->HingePosition[1] = 0;
    this->HingePosition[2] = 0;
    this->NeedleListIndex = -1;
    this->FiducialIndex = -1;
    //this->SetFilledPoint();
}


/// Destructor
vtkTRProstateBiopsyTargetDescriptor::~vtkTRProstateBiopsyTargetDescriptor(void)
{
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetDescriptor::PrintSelf(ostream& os, vtkIndent indent)
{
  
  this->Superclass::PrintSelf(os,indent);

  /*
  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 */
}

/*
/// Serialize to file
bool vtkTRProstateBiopsyTargetDescriptor::Save(wxFileConfig *ini, const char *iniCategory)
{
    //cPrimitivePoint::Save(ini,iniCategory);

    wxString iniEntry;

    iniEntry=iniCategory;iniEntry+="NeedleType";
    ini->Write(iniEntry,this->NeedleType);

    iniEntry=iniCategory;iniEntry+="Info_NeedleDeepth";
    ini->Write(iniEntry,this->NeedleDeepth);

    iniEntry=iniCategory;iniEntry+="Info_AxisRotationDeg";
    ini->Write(iniEntry,this->AxisRotation);

    iniEntry=iniCategory;iniEntry+="Info_NeedleAngleDeg";
    ini->Write(iniEntry,this->NeedleAngle);

    iniEntry=iniCategory;iniEntry+="Info_DepthCM";
    ini->Write(iniEntry,this->DepthCM);

    iniEntry=iniCategory;iniEntry+="Info_Reacheable";
    if (this->IsOutsideReach) {
        ini->Write(iniEntry,"false");
    } else {
        ini->Write(iniEntry,"true");
    }
  
    iniEntry=iniCategory;iniEntry+="Target_Validated";
    if (this->TargetValidated) {
        ini->Write(iniEntry,"true");
    } else {
        ini->Write(iniEntry,"false");
    }

    iniEntry=iniCategory;iniEntry+="NeedleConfirmationVolume_SeriesIdentifier";
    ini->Write(iniEntry,this->ValidationVolumeFoR_STR);

    iniEntry=iniCategory;iniEntry+="Distance_From_Needle";
    ini->Write(iniEntry,this->ComputedDistanceFromNeedle);



    return true;
}


/// Restore from file
bool vtkTRProstateBiopsyTargetDescriptor::Load(wxFileConfig *ini, const char *iniCategory)
{
    cPrimitivePoint::Load(ini,iniCategory);

    wxString iniEntry;

    iniEntry=iniCategory;iniEntry+="NeedleType";
    ini->Read(iniEntry,&this->NeedleType);

    iniEntry=iniCategory;iniEntry+="Info_NeedleDeepth";
    ini->Read(iniEntry,&this->NeedleDeepth);

    iniEntry=iniCategory;iniEntry+="Info_AxisRotationDeg";
    ini->Read(iniEntry,&this->AxisRotation);

    iniEntry=iniCategory;iniEntry+="Info_NeedleAngleDeg";
    ini->Read(iniEntry,&this->NeedleAngle);

    iniEntry=iniCategory;iniEntry+="Info_DepthCM";
    ini->Read(iniEntry,&this->DepthCM);

    iniEntry=iniCategory;iniEntry+="Info_Reacheable";
    wxString sReacheable;
    ini->Read(iniEntry,&sReacheable,"false");
    this->IsOutsideReach=true;
    if (sReacheable=="true") this->IsOutsideReach=false;

    iniEntry=iniCategory;iniEntry+="Target_Validated";
    wxString sTargetValidated;
    ini->Read(iniEntry, &sTargetValidated); 
    if (sTargetValidated == "true")
      {
      this->TargetValidated=true;
      }
    else
      {
      this->TargetValidated = false;
      }
    
    iniEntry=iniCategory;iniEntry+="NeedleConfirmationVolume_SeriesIdentifier"; 
    wxString str;
    ini->Read(iniEntry,&str);
    this->ValidationVolumeFoR_STR = std::string(str);

    iniEntry=iniCategory;iniEntry+="Distance_From_Needle";
    ini->Read(iniEntry,&this->ComputedDistanceFromNeedle);


    return true;
}
*/