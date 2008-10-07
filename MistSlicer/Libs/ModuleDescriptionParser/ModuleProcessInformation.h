/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ModuleDescriptionParser/ModuleProcessInformation.h $
  Date:      $Date: 2008-07-08 02:56:02 +0900 (火, 08 7 2008) $
  Version:   $Revision: 7251 $

==========================================================================*/

#ifndef __ModuleProcessInformation_h
#define __ModuleProcessInformation_h

#include <ostream>
#include <cstring>

extern "C" {
  struct ModuleProcessInformation
  {
    /** Inputs from calling application to the module **/
    unsigned char Abort;
    
    /** Outputs from the module to the calling application **/
    float Progress;      // Overall progress
    float StageProgress; // Progress of a single stage in an algorithm
    char  ProgressMessage[1024];
    void (*ProgressCallbackFunction)(void *);
    void *ProgressCallbackClientData;

    double ElapsedTime;

    void Initialize()
      {
        Abort = 0;
        Progress = 0;
        strcpy(ProgressMessage, "");
        ElapsedTime = 0.0;
      };

    void SetProgressCallback( void (*fun)(void *), void *who )
      {
        ProgressCallbackFunction = fun;
        ProgressCallbackClientData = who;
      };
  };
};

std::ostream& operator<<(std::ostream &os, const ModuleProcessInformation &p);
    

#endif
