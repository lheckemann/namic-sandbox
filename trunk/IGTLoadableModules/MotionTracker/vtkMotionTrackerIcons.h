/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMotionTrackerIcons.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/

#ifndef __vtkMotionTrackerIcons_h
#define __vtkMotionTrackerIcons_h

#include "vtkMotionTrackerWin32Header.h"
#include "vtkKWObject.h"
#include "vtkKWResourceUtilities.h"
#include "vtkKWIcon.h"
#include "vtkSlicerIcons.h"
#include "vtkMotionTrackerIcons.h"

#include "./Resources/vtkMotionTracker_ImageData.h"

class VTK_MotionTracker_EXPORT vtkMotionTrackerIcons : public vtkSlicerIcons
{
 public:
    static vtkMotionTrackerIcons* New ( );
    vtkTypeRevisionMacro ( vtkMotionTrackerIcons, vtkSlicerIcons );
    void PrintSelf ( ostream& os, vtkIndent indent );

    // Description:
    // Get toolbar vtkKWIcons
    vtkGetObjectMacro (PlayIcon, vtkKWIcon);
    vtkGetObjectMacro (PauseIcon, vtkKWIcon);
    virtual void AssignImageDataToIcons ( );
    
 protected:
    vtkMotionTrackerIcons ( );
    virtual ~vtkMotionTrackerIcons ( );
    
    vtkKWIcon *PlayIcon;
    vtkKWIcon *PauseIcon;

 private:
    vtkMotionTrackerIcons (const vtkMotionTrackerIcons&); // Not implemented
    void operator = ( const vtkMotionTrackerIcons& ); // Not implemented
    
};

#endif



