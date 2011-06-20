/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMotionTrackerIcons.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkMotionTrackerIcons.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro ( vtkMotionTrackerIcons );
vtkCxxRevisionMacro ( vtkMotionTrackerIcons, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkMotionTrackerIcons::vtkMotionTrackerIcons ( )
{
        //--- window level preset icons
  this->PlayIcon = vtkKWIcon::New();
  this->PauseIcon = vtkKWIcon::New();
  
  // now generate the icons from images
  this->AssignImageDataToIcons ( );
}

//---------------------------------------------------------------------------
vtkMotionTrackerIcons::~vtkMotionTrackerIcons ( )
{
  // window level presets icons
  if ( this->PlayIcon )
    {
    this->PlayIcon->Delete ( );
    this->PlayIcon = NULL;
    }
  if ( this->PauseIcon )
    {
    this->PauseIcon->Delete ( );
    this->PauseIcon = NULL;
    }
}

//---------------------------------------------------------------------------
void vtkMotionTrackerIcons::AssignImageDataToIcons ( )
{
  this->PlayIcon->SetImage ( image_MotionTracker_Play,
                             image_MotionTracker_Play_width,
                             image_MotionTracker_Play_height,
                             image_MotionTracker_Play_pixel_size,
                             image_MotionTracker_Play_length, 0 );
  this->PauseIcon->SetImage ( image_MotionTracker_Pause,
                             image_MotionTracker_Pause_width,
                             image_MotionTracker_Pause_height,
                             image_MotionTracker_Pause_pixel_size,
                             image_MotionTracker_Pause_length, 0 );

}

//---------------------------------------------------------------------------
void vtkMotionTrackerIcons::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "MotionTrackerIcons: " << this->GetClassName ( ) << "\n";
    os << indent << "PlayIcon" << this->GetPlayIcon ( ) << "\n";
    os << indent << "PauseIcon" << this->GetPauseIcon ( ) << "\n";

}
