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

#include "vtkTRProstateBiopsyWin32Header.h"



class vtkTRProstateBiopsyTargetDescriptor{
public:

    static vtkTRProstateBiopsyTargetDescriptor *New();

    // Desciption
    // Save/load from ini file
    //virtual bool Save(wxFileConfig *ini, const char *iniCategory);
    //virtual bool Load(wxFileConfig *ini, const char *iniCategory);

    // Description
    // Set/get Targeting parameter: rotation
    void SetRotation(double r) { this->AxisRotation = r; }
    double GetRotation(void) { return this->AxisRotation; }

    // Description
    // Set/get Targeting parameter: needle angle
    void SetNeedleAngle(double a) { this->NeedleAngle = a; }
    double GetNeedleAngle(void) { return this->NeedleAngle; }

    // Description
    // Set/get Targeting parameter: depth in centimeters
    void SetDepthCM(double d) { this->DepthCM = d; }
    double GetDepthCM(void) { return this->DepthCM; }

    // Description
    // Set/get Targeting parameter: is reachable or not 
    void SetIsOutside(bool out) { this->IsOutsideReach = out; }
    bool GetIsOutside(void) { return this->IsOutsideReach; }
    //BTX
    std::string GetReachableString()
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
    std::string GetNeedleTypeString(){ return this->NeedleType;}
    int GetNeedleTypeUID(void) {return this->NeedleTypeUID; }
    //ETX
    // Description
    // Set/get Targeting parameter: needle type depth
    double GetNeedleTypeDepth(void) {return this->NeedleDepth; }
    double GetNeedleTypeOvershoot(void) {return this->NeedleOvershoot; }

    // Description
    // Set/get Targeting parameter: hinge position
    void SetHingePosition(double x, double y, double z) 
    { 
        this->Hinge[0] = x; 
        this->Hinge[1] = y; 
        this->Hinge[2] = z; 
    }
    void GetHingePosition(double &x, double &y, double &z) 
    { 
        x = this->Hinge[0]; 
        y = this->Hinge[1]; 
        z = this->Hinge[2]; 
    }

    // Description
    // Set/get : ras location
    void SetRASLocation(double x, double y, double z) 
    { 
        this->RASLocation[0] = x; 
        this->RASLocation[1] = y; 
        this->RASLocation[2] = z; 
    }
    void GetRASLocation(double &x, double &y, double &z) 
    { 
        x = this->RASLocation[0]; 
        y = this->RASLocation[1]; 
        z = this->RASLocation[2]; 
    }
    //BTX
    std::string GetRASLocationString()
    {
        char ras[50];
        sprintf(ras, "R %.1f, A %.1f, S %.1f", this->RASLocation[0], this->RASLocation[1], this->RASLocation[2]);
        return std::string(ras);
    }
    //ETX
    // Description
    // Set/get Validation info: is validated?   
    void SetTargetValidated (bool valid) { this->TargetValidated = valid;}
    bool GetTargetValidated(){return this->TargetValidated;}

    // Description
    // Set/get validation info: computed distance from the needle
    void SetOverallError ( double error){ this->OverallError = error;}
    double GetOverallError(){return this->OverallError;}

    // Description
    // Set/get validation info: computed distance from the needle
    void SetAPError ( double error){ this->APError = error;}
    double GetAPError(){return this->APError;}

    // Description
    // Set/get validation info: computed distance from the needle
    void SetLRError ( double error){ this->LRError = error;}
    double GetLRError(){return this->LRError;}

    // Description
    // Set/get validation info: computed distance from the needle
    void SetISError ( double error){ this->ISError = error;}
    double GetISError(){return this->ISError;}

    // Description
    // Set/get MRML fiducial list related info
    void SetNeedleIndex ( int needleIndex){ this->NeedleListIndex = needleIndex;}
    int GetNeedleIndex(){return this->NeedleListIndex;}

    // Description
    // Set/get MRML fiducial list related info
    void SetFiducialIndex ( int fiducialIndex){ this->FiducialIndex = fiducialIndex;}
    int GetFiducialIndex(){return this->FiducialIndex;}

    //BTX
    // Description
    // Set/get validation info: Validation volume FoR string
    void SetNeedleConfirmationVolumeFoRStr(std::string foRStr){this->ValidationVolumeFoR_STR = foRStr;}
    std::string GetNeedleConfirmationVolumeFoRStr(){return this->ValidationVolumeFoR_STR;}

     // Description
    // Set/get FoR string
    void SetFoRStr(std::string foRStr){this->FoR_STR = foRStr;}
    std::string GetFoRStr(){return this->FoR_STR;}

    //ETX
protected:

    vtkTRProstateBiopsyTargetDescriptor(void);
    ~vtkTRProstateBiopsyTargetDescriptor(void);
    
    
private:
    double AxisRotation;    ///< Calculated value: Axis rotation in degree
    double NeedleAngle;     ///< Calculated value: Needle angle in degree
    double DepthCM;         ///< Calculated value: Insertion deepth in cm
    bool IsOutsideReach;    ///< Calculated value: Can be reaced?
    double NeedleDepth; ///< Depth depending on needle type (biopsy, placement...)
    int NeedleTypeUID;          ///< Needle type ID for this target
    double NeedleOvershoot; ///< Overshooting
    double Hinge[3];
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
    

    vtkTRProstateBiopsyTargetDescriptor(const vtkTRProstateBiopsyTargetDescriptor&);
    void operator=(const vtkTRProstateBiopsyTargetDescriptor&);
};




#endif
