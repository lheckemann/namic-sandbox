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
     // if positive, then target is towards the needle base (seed placement)
     // if negative, then target is in front of the needle tip (biopsy)
     double Overshoot; 

     // NeedleExtension: maximum extension of the needle from the needle tip,
     // negative if the needle extends towards the needle tip (biopsy) - just for display
     double Extension; 

     // NeedleTargetSize: length of the biopsy core or seed - just for display
     double TargetSize; 

     // LastTargetId stores the last index that was used for adding a target for this needle.
     // It is useful for generating unique target names.
     int LastTargetId;
   };

class VTK_PROSTATENAV_EXPORT vtkProstateNavTargetDescriptor :
  public vtkObject
{
public:

  static vtkProstateNavTargetDescriptor *New();
  vtkTypeRevisionMacro(vtkProstateNavTargetDescriptor,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

    //BTX
    // Description
    // Set/get validation info: Validation volume FoR string
    void SetName(std::string str){this->Name = str;}
    std::string GetName() const {return this->Name;};

    void SetNeedleID(std::string needleID)
    { 
        this->NeedleID = needleID;
    }
    std::string GetNeedleID() const { return this->NeedleID;};
    //ETX

    // Description
    // Set/get : ras location
    vtkSetVector3Macro(RASLocation,double);
    vtkGetVectorMacro(RASLocation,double,3);

    // Description
    // Set/get : ras location
    vtkSetVector4Macro(RASOrientation,double);
    vtkGetVectorMacro(RASOrientation,double,4);

    //BTX
    std::string GetRASLocationString() const
    {
        char ras[50];
        sprintf(ras, "R %.1f, A %.1f, S %.1f", this->RASLocation[0], this->RASLocation[1], this->RASLocation[2]);
        return std::string(ras);
    }
    //ETX
    
    // Description
    // Set/get Needle placement is validated
    vtkGetMacro(TargetValidated,bool);
    vtkSetMacro(TargetValidated,bool);

    // Description
    // Set/get validation info: needle tip point
    vtkSetVector3Macro(NeedleTipValidationPosition,double);
    vtkGetVectorMacro(NeedleTipValidationPosition,double,3);

    // Description
    // Set/get validation info: needle base point (any point along the needle)
    vtkSetVector3Macro(NeedleBaseValidationPosition,double);
    vtkGetVectorMacro(NeedleBaseValidationPosition,double,3);

    // Description
    // Set/get validation info: computed distance from the needle
    vtkGetMacro(OverallError,double);
    vtkSetMacro(OverallError,double);

    // Description
    // Set/get validation info: computed distance from the needle
    vtkGetMacro(APError,double);
    vtkSetMacro(APError,double);

    // Description
    // Set/get validation info: computed distance from the needle
    vtkGetMacro(LRError,double);
    vtkSetMacro(LRError,double);

    // Description
    // Set/get validation info: computed distance from the needle
    vtkGetMacro(ISError,double);
    vtkSetMacro(ISError,double);

    //BTX
    // Description
    // Set/get validation info: Validation volume FoR string
    void SetNeedleConfirmationVolumeFoRStr(std::string foRStr){this->ValidationVolumeFoR_STR = foRStr;}
    std::string GetNeedleConfirmationVolumeFoRStr() const {return this->ValidationVolumeFoR_STR;};

    // Description
    // Set/get FoR string of the targeting volume
    void SetTargetingFoRStr(std::string foRStr){this->TargetingFoR_STR = foRStr;}
    std::string GetTargetingFoRStr() const {return this->TargetingFoR_STR;};

    // Description
    // Set/get FoR string of the calibration volume
    void SetCalibrationFoRStr(std::string foRStr){this->CalibrationFoR_STR = foRStr;}
    std::string GetCalibrationFoRStr() const {return this->CalibrationFoR_STR;};

    void SetFiducialID(std::string fidId){this->FiducialID = fidId;}
    std::string GetFiducialID() const {return this->FiducialID; };

    void DeepCopy(vtkProstateNavTargetDescriptor& src);

    //ETX
        
private:
    //BTX

    // Target definition => add targeting volume ref?
    std::string Name;
    double RASLocation[3];
    double RASOrientation[4];
    std::string TargetingFoR_STR;
    std::string FiducialID; // ID of the fiducial in the FiducialListNode
    std::string NeedleID; // ID of the needle type

    // Calibration ??? => remove it
    std::string CalibrationFoR_STR; /// string identifier for the calibration volume
    
    // Add bTargetCompleted?
    // Add sTargetingParameters?

    // Validation => replace FoR with verif volume ref?
    bool TargetValidated;  ///< Indicates whether it was validated against needle insertion
    std::string ValidationVolumeFoR_STR; /// string identifier for the volume used for needle confirmation        
    double NeedleTipValidationPosition[3];
    double NeedleBaseValidationPosition[3];
    double OverallError; /// < Calculated value, ComputedDistanceFromNeedle
    double APError;/// < Calculated value
    double LRError;/// < Calculated value
    double ISError;/// < Calculated value
    
    //ETX
    
    vtkProstateNavTargetDescriptor(void);
    ~vtkProstateNavTargetDescriptor(void);

    /*vtkProstateNavTargetDescriptor(const vtkProstateNavTargetDescriptor&);
    void operator=(const vtkProstateNavTargetDescriptor&);*/
};

#endif
