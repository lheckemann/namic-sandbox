/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTransRectalProstateRobotNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkMRMLTransRectalProstateRobotNode.h"
#include "vtkMRMLScene.h"
#include "vtkProstateNavTargetDescriptor.h"

//------------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode* vtkMRMLTransRectalProstateRobotNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransRectalProstateRobotNode");
  if(ret)
    {
      return (vtkMRMLTransRectalProstateRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransRectalProstateRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode* vtkMRMLTransRectalProstateRobotNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransRectalProstateRobotNode");
  if(ret)
    {
      return (vtkMRMLTransRectalProstateRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransRectalProstateRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode::vtkMRMLTransRectalProstateRobotNode()
{
  this->CalibrationAlgo=vtkSmartPointer<vtkTransRectalFiducialCalibrationAlgo>::New();
  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    this->CalibrationMarkerValid[i]=false;
  } 
  ResetCalibrationData();
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode::~vtkMRMLTransRectalProstateRobotNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLTransRectalProstateRobotNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLTransRectalProstateRobotNode *node = (vtkMRMLTransRectalProstateRobotNode *) anode;

  //int type = node->GetType();
  
}


void vtkMRMLTransRectalProstateRobotNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
  return;
}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
bool vtkMRMLTransRectalProstateRobotNode::FindTargetingParams(vtkProstateNavTargetDescriptor *targetDesc)
{
  // this is used for coverage area computation (IsOutsideReach means that the target is outside the robot's coverage area)
  return this->CalibrationAlgo->FindTargetingParams(targetDesc);
}


std::string vtkMRMLTransRectalProstateRobotNode::GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc)
{
  bool validTargeting=FindTargetingParams(targetDesc);

  std::ostrstream os;  
  os << "Needle type:"<<targetDesc->GetNeedleTypeString()<<std::endl;
  os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
  os << "RAS location: "<<targetDesc->GetRASLocationString().c_str()<<std::endl;
  if (validTargeting)
  {
    os << "Reachable: "<<targetDesc->GetReachableString().c_str()<<std::endl;
    os << "Depth: "<<targetDesc->GetDepthCM()<<" cm"<<std::endl;
    os << "Device rotation: "<<targetDesc->GetAxisRotation()<<" deg"<<std::endl;
    os << "Needle angle: "<<targetDesc->GetNeedleAngle()<<" deg"<<std::endl;
  }
  os << std::ends;
  std::string result=os.str();
  os.rdbuf()->freeze();
  return result;
}

//------------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::SetCalibrationMarker(unsigned int markerNr, double markerRAS[3])
{
  if (markerNr<0 || markerNr>=CALIB_MARKER_COUNT)
    {
    vtkErrorMacro("SetCalibrationMarker: Invalid calibration marker index "<<markerNr);
    return;
    }

  this->CalibrationMarkerPositions[markerNr][0]=markerRAS[0];
  this->CalibrationMarkerPositions[markerNr][1]=markerRAS[1];
  this->CalibrationMarkerPositions[markerNr][2]=markerRAS[2];
  this->CalibrationMarkerValid[markerNr]=true;
}
//------------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::GetCalibrationMarker(unsigned int markerNr, double &r, double &a, double &s, bool &valid)
{
  if (markerNr<0 || markerNr>=4)
    {
    vtkErrorMacro("GetCalibrationMarker: Invalid calibration marker index "<<markerNr);
    return;
    }  
  r=this->CalibrationMarkerPositions[markerNr][0];
  a=this->CalibrationMarkerPositions[markerNr][1];
  s=this->CalibrationMarkerPositions[markerNr][2];
  valid=this->CalibrationMarkerValid[markerNr];
}
//------------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::RemoveAllCalibrationMarkers()
{
  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    this->CalibrationMarkerValid[i]=false;
  }
}

//------------------------------------------------------------------------------
bool vtkMRMLTransRectalProstateRobotNode::SegmentRegisterMarkers(vtkMRMLScalarVolumeNode *calibVol, double thresh[4], double fidDimsMm[3], double radiusMm, bool bUseRadius, double initialAngle, std::string &resultDetails, bool enableAutomaticCenterpointAdjustment)
{
  TRProstateBiopsyCalibrationFromImageInput in;

  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    bool valid=false;
    GetCalibrationMarker(i, in.MarkerInitialPositions[i][0], in.MarkerInitialPositions[i][1], in.MarkerInitialPositions[i][2], valid); 
    if (!valid)
    {
      vtkErrorMacro("SegmentRegisterMarkers marker "<<i+1<<" is undefined");
      resultDetails="Not all calibration markers are defined";
      return false;
    }
    in.MarkerSegmentationThreshold[i]=thresh[i];
  }
  for (int i=0; i<3; i++)
  {
    in.MarkerDimensionsMm[i]=fidDimsMm[i];
  }
  in.MarkerRadiusMm=radiusMm;
  in.RobotInitialAngle=initialAngle;
  
  if (calibVol==0)
  {
    vtkErrorMacro("SegmentRegisterMarkers CalibrationVolume is invalid");
    resultDetails="Calibration volume is invalid";
    return false;
  }
  vtkSmartPointer<vtkMatrix4x4> ijkToRAS = vtkSmartPointer<vtkMatrix4x4>::New(); 
  calibVol->GetIJKToRASMatrix(ijkToRAS);
  in.VolumeIJKToRASMatrix=ijkToRAS;
  in.VolumeImageData=calibVol->GetImageData();

  TRProstateBiopsyCalibrationFromImageOutput res;

  this->CalibrationAlgo->SetEnableMarkerCenterpointAdjustment(enableAutomaticCenterpointAdjustment);

  bool success=true;
  if (!this->CalibrationAlgo->CalibrateFromImage(in, res))
  {
    // calibration failed

    ResetCalibrationData();

    std::ostrstream os;
    // if a marker not found, then make the error report more precise
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
    {      
      if (!res.MarkerFound[i])
      {
        os << "Marker "<<i+1<<" cannot be detected. ";
        success=false;
      }
    } 
    os << "Calibration failed." << std::ends;
    resultDetails=os.str();
    os.rdbuf()->freeze();  
    this->Modified();
  }
  else
  {    
    // calibration is successful

    /*commented out to keep the original marker guesses
    // update manually set marker positions with the result of the marker detection
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
    {      
      SetCalibrationMarker(i, res.MarkerPositions[i]);
      in.MarkerSegmentationThreshold[i]=thresh[i];
    }
    */

    const TRProstateBiopsyCalibrationData calibData=this->CalibrationAlgo->GetCalibrationData();
    SetCalibrationData(calibData);    

    resultDetails="Calibration is successfully completed.";    
    success=true;  
    this->Modified();
  }  
  return success;
}

//------------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::ResetCalibrationData()
{
  this->CalibrationData.AxesDistance = -1;
  this->CalibrationData.AxesAngleDegrees = 0;
  this->CalibrationData.RobotRegistrationAngleDegrees = 0;
  for (int i=0; i<3; i++)
  {
    this->CalibrationData.I1[i]=0.0;
    this->CalibrationData.I2[i]=0.0;
    this->CalibrationData.v1[i]=0.0;
    this->CalibrationData.v2[i]=0.0;
  }
  this->CalibrationData.CalibrationValid=false;
}

//------------------------------------------------------------------------------
vtkImageData* vtkMRMLTransRectalProstateRobotNode::GetCalibMarkerPreProcOutput(int i)
{
  return this->CalibrationAlgo->GetCalibMarkerPreProcOutput(i);
}
