/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyTargetDescriptor - describes the target, along with targeting paramteres and validation info
// .SECTION Description
// vtkTRProstateBiopsyTargetDescriptor describes the target, along with targeting paramteres and validation info

#ifndef _vtkTRProstateBiopsyTargetDescriptor_h
#define _vtkTRProstateBiopsyTargetDescriptor_h
#pragma once

#include <stdio.h>
#include <string>

#include "vtkObject.h"
#include "vtkTRProstateBiopsyWin32Header.h"



class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyTargetDescriptor :
  public vtkObject
{
public:

  static vtkTRProstateBiopsyTargetDescriptor *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyTargetDescriptor,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

    // Desciption
    // Save/load from ini file
    //virtual bool Save(wxFileConfig *ini, const char *iniCategory);
    //virtual bool Load(wxFileConfig *ini, const char *iniCategory);

    // Description
    // Set/get Targeting parameter: rotation
    vtkGetMacro(AxisRotation,double);
    vtkSetMacro(AxisRotation,double);

    // Description
    // Set/get Targeting parameter: needle angle
    vtkGetMacro(NeedleAngle,double);
    vtkSetMacro(NeedleAngle,double);

    // Description
    // Set/get Targeting parameter: depth in centimeters
    vtkGetMacro(DepthCM,double);
    vtkSetMacro(DepthCM,double);

    // Description
    // Set/get Needle overshoot
    vtkGetMacro(NeedleOvershoot,double);
    vtkSetMacro(NeedleOvershoot,double);


    // Description
    // Set/get Targeting parameter: is reachable or not 
    vtkGetMacro(IsOutsideReach,bool);
    vtkSetMacro(IsOutsideReach,bool);

    //BTX
    std::string GetReachableString() const
        { 
            if (this->IsOutsideReach) 
                return "No";
            else
                return "Yes";
        }
    //ETX

    //BTX
    // Description
    // Set/get Targeting parameter: needle type
    void SetNeedleType(std::string NeedleType, int TypeID, double Depth, double Overshoot) 
    { 
        this->NeedleType = NeedleType;
        this->NeedleTypeUID = TypeID;
        this->NeedleDepth = Depth;
        this->NeedleOvershoot = Overshoot;
    }
    std::string GetNeedleTypeString() const { return this->NeedleType;};
    vtkGetMacro(NeedleTypeUID, int);
    //ETX
    // Description
    // Set/get Targeting parameter: needle type depth
    vtkGetMacro(NeedleDepth,double);
    vtkSetMacro(NeedleDepth,double);

    // Description
    // Set/get Targeting parameter: hinge position (in RAS coordinate system)
    vtkSetVector3Macro(HingePosition,double);
    vtkGetVectorMacro(HingePosition,double,3);

    // Description
    // Set/get : ras location
    vtkSetVector3Macro(RASLocation,double);
    vtkGetVectorMacro(RASLocation,double,3);

    //BTX
    std::string GetRASLocationString() const
    {
        char ras[50];
        sprintf(ras, "R %.1f, A %.1f, S %.1f", this->RASLocation[0], this->RASLocation[1], this->RASLocation[2]);
        return std::string(ras);
    }
    //ETX
    // Description
    // Set/get Validation info: is validated?   
    vtkGetMacro(TargetValidated,bool);
    vtkSetMacro(TargetValidated,bool);

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

    // Description
    // Set/get MRML fiducial list related info
    vtkGetMacro(NeedleListIndex,double);
    vtkSetMacro(NeedleListIndex,double);

    // Description
    // Set/get MRML fiducial list related info
    vtkGetMacro(FiducialIndex,double);
    vtkSetMacro(FiducialIndex,double);

    //BTX
    // Description
    // Set/get validation info: Validation volume FoR string
    void SetNeedleConfirmationVolumeFoRStr(std::string foRStr){this->ValidationVolumeFoR_STR = foRStr;}
    std::string GetNeedleConfirmationVolumeFoRStr() const {return this->ValidationVolumeFoR_STR;};

     // Description
    // Set/get FoR string
    void SetFoRStr(std::string foRStr){this->FoR_STR = foRStr;}
    std::string GetFoRStr() const {return this->FoR_STR;};

    //ETX
        
private:
    double AxisRotation;    ///< Calculated value: Axis rotation in degree
    double NeedleAngle;     ///< Calculated value: Needle angle in degree
    double DepthCM;         ///< Calculated value: Insertion deepth in cm
    bool IsOutsideReach;    ///< Calculated value: Can be reaced?
    double NeedleDepth; ///< Depth depending on needle type (biopsy, placement...)
    int NeedleTypeUID;          ///< Needle type ID for this target
    double NeedleOvershoot; ///< Overshooting
    double HingePosition[3];
    double RASLocation[3];
    //BTX
    std::string FoR_STR;
    std::string ValidationVolumeFoR_STR; /// string identifier for the volume used for needle confirmation
    std::string NeedleType;
    //ETX
    bool TargetValidated;  ///< Indicates whether it was validated against needle insertion
    double OverallError; /// < Calculated value, ComputedDistanceFromNeedle
    double APError;/// < Calculated value
    double LRError;/// < Calculated value
    double ISError;/// < Calculated value

    // MRML fiducial management
    // indices of fiducial lists
    int NeedleListIndex; // index of which target fiducial list to use, as the fiducial lists are indexed by needle
    int FiducialIndex; // which fiducial within the particular needle list
    
    vtkTRProstateBiopsyTargetDescriptor(void);
    ~vtkTRProstateBiopsyTargetDescriptor(void);

    /*vtkTRProstateBiopsyTargetDescriptor(const vtkTRProstateBiopsyTargetDescriptor&);
    void operator=(const vtkTRProstateBiopsyTargetDescriptor&);*/
};

#endif
