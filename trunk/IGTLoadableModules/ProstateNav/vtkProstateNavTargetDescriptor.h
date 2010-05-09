/*=auto=========================================================================

Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: $
Date:      $Date: $
Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkProstateNavTargetDescriptor - describes the target, along with targeting paramteres and validation info
// .SECTION Description
// vtkProstateNavTargetDescriptor describes the target, along with targeting paramteres and validation info

#ifndef _vtkProstateNavTargetDescriptor_h
#define _vtkProstateNavTargetDescriptor_h
#pragma once

#include <stdio.h>
#include <string>

#include "vtkObject.h"
#include "vtkProstateNavWin32Header.h"

struct VTK_PROSTATENAV_EXPORT NeedleDescriptorStruct
{
  // ID:  unique id
  std::string ID;

  // NeedleName: a prefix for target point names
  std::string TargetNamePrefix;

  // Description: descriptive name of the needle
  std::string Description;

  // NeedleLength: maximum possible insertion depth, in mm - just for checking if the target is reachable
  double Length;

  // Overshoot: where is the target compared to the needle tip, in mm
  // if positive, then target is in front of the needle tip (biopsy)
  // if negative, then target is towards the needle base (seed placement)
  double Overshoot; 

  // NeedleExtension: maximum extension of the needle from the needle tip,
  // positive if the needle extends towards the needle tip (biopsy) - just used for display
  double Extension; 

  // NeedleTargetSize: length of the biopsy core or seed - just for display
  double TargetSize; 

  // LastTargetIndex stores the last index that was used for adding a target for this needle.
  // It is useful for generating unique target names.
  int LastTargetIndex;
};

class VTK_PROSTATENAV_EXPORT vtkProstateNavTargetDescriptor :
  public vtkObject
{
public:

  static vtkProstateNavTargetDescriptor *New();
  vtkTypeRevisionMacro(vtkProstateNavTargetDescriptor,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DeepCopy(vtkProstateNavTargetDescriptor& src);

  //BTX

  // Description
  // Set/get validation info: Validation volume FoR string
  void SetName(std::string str){this->Name = str;}
  std::string GetName() const {return this->Name;};

  // Description
  // Set/get validation volume FoR string
  void SetComment(std::string comment){this->Comment = comment;}
  std::string GetComment() const {return this->Comment;};

  // Description
  // Set/get : ras location
  vtkSetVector3Macro(RASLocation,double);
  vtkGetVectorMacro(RASLocation,double,3);
  std::string GetRASLocationString() const
  {
    char ras[50];
    sprintf(ras, "R %.1f, A %.1f, S %.1f", this->RASLocation[0], this->RASLocation[1], this->RASLocation[2]);
    return std::string(ras);
  }

  // Description
  // Set/get : ras location
  vtkSetVector4Macro(RASOrientation,double);
  vtkGetVectorMacro(RASOrientation,double,4);

  // Description
  // Set/get MRML reference to the targeting volume
  void SetTargetingVolumeRef(std::string foRStr){this->TargetingVolumeRef = foRStr;}
  std::string GetTargetingVolumeRef() const {return this->TargetingVolumeRef;};

  // Description
  // Set/get FoR string of the targeting volume
  void SetTargetingVolumeFoR(std::string foRStr){this->TargetingVolumeFoR = foRStr;}
  std::string GetTargetingVolumeFoR() const {return this->TargetingVolumeFoR;};

  // Description
  // Set/get the needle ID (not MRML)
  void SetNeedleID(std::string needleID) { this->NeedleID = needleID; }
  std::string GetNeedleID() const { return this->NeedleID;};

  // Description
  // Set/get the fiducial point ID (defined in the fiducial list node)
  void SetFiducialID(std::string fidId){this->FiducialID = fidId;}
  std::string GetFiducialID() const {return this->FiducialID; };

  // Description
  // Set/get target completed info. 
  // Indicates whether the target was already completed (biopsy taken, seed inserted, etc.)
  vtkGetMacro(TargetCompleted,bool);
  vtkSetMacro(TargetCompleted,bool);

  // Description
  // Set/get the target completed info, which was displayed for the user
  // when the targeting was completed.
  void SetTargetCompletedInfo(std::string info){this->TargetCompletedInfo = info;}
  std::string GetTargetCompletedInfo() const {return this->TargetCompletedInfo; };

  // Description
  // Set/get Needle placement is validated
  vtkGetMacro(TargetValidated,bool);
  vtkSetMacro(TargetValidated,bool);

  // Description
  // Set/get validation volume MRML ID
  void SetValidationVolumeRef(std::string refstr){this->ValidationVolumeRef = refstr;}
  std::string GetValidationVolumeRef() const {return this->ValidationVolumeRef;};

  // Description
  // Set/get validation volume FoR string
  void SetValidationVolumeFoR(std::string forstr){this->ValidationVolumeFoR = forstr;}
  std::string GetValidationVolumeFoR() const {return this->ValidationVolumeFoR;};

  // Description
  // Set/get validation info: needle tip point
  vtkSetVector3Macro(NeedleTipValidationPosition,double);
  vtkGetVectorMacro(NeedleTipValidationPosition,double,3);

  // Description
  // Set/get validation info: needle base point (any point along the needle)
  vtkSetVector3Macro(NeedleBaseValidationPosition,double);
  vtkGetVectorMacro(NeedleBaseValidationPosition,double,3);

  // Description
  // Set/get validation info: computed distance of the original target from the actual needle line
  vtkGetMacro(ErrorDistanceFromNeedleLine,double);
  vtkSetMacro(ErrorDistanceFromNeedleLine,double);

  // Description
  // Set/get validation info: error vector (difference between defined and actually reached target)
  vtkSetVector3Macro(ErrorVector,double);
  vtkGetVectorMacro(ErrorVector,double,3);

  //ETX

private:
  //BTX

  // Target definition
  std::string Name;
  // Any comment that the operator might find important to record for this target
  std::string Comment;
  double RASLocation[3];
  double RASOrientation[4];
  // ID of the volume where the target was defined
  std::string TargetingVolumeRef;
  // TargetingVolumeFoR is the DICOM Frame of Reference UID
  // The FoR string can be read from the TargetingVolumeRef if that is a DICOM image
  // but other image file formats usually cannot store this ID, so it's better to store keep it here, too
  std::string TargetingVolumeFoR;
  std::string NeedleID;    
  std::string FiducialID;

  // Targeting
  bool TargetCompleted;
  std::string TargetCompletedInfo;

  // Targeting validation
  bool TargetValidated;
  std::string ValidationVolumeRef;
  std::string ValidationVolumeFoR;
  double NeedleTipValidationPosition[3];
  double NeedleBaseValidationPosition[3];
  double ErrorDistanceFromNeedleLine;
  double ErrorVector[3];

  //ETX

  vtkProstateNavTargetDescriptor(void);
  ~vtkProstateNavTargetDescriptor(void);

  vtkProstateNavTargetDescriptor(const vtkProstateNavTargetDescriptor&);
  void operator=(const vtkProstateNavTargetDescriptor&);
};

#endif
