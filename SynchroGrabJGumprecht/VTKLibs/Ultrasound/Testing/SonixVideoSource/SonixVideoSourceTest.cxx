/*=========================================================================

Module:    $RCSfile: SonixVideoSourceTest.cxx,v $

Module:    $RCSfile: vtkSonixVideoSource.h,v $
Author:  Siddharth Vikal, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/  
//
// This example creates a polygonal model of a cone, and then renders it to
// the screen. It will rotate the cone 360 degrees and then exit. The basic
// setup of source -> mapper -> actor -> renderer -> renderwindow is 
// typical of most VTK programs.
//

// First include the required header files for the VTK classes we are using.

#include <stdlib.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkSonixVideoSource.h"
#include "vtkImageViewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"

vtkSonixVideoSource *sonixGrabber = NULL;
vtkImageViewer *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    viewer->Render();
        
    //update the timer so it will trigger again
    //VTKI_TIMER_UPDATE = 1
    iren->CreateTimer(VTKI_TIMER_UPDATE);
  }
};
int main()
{
   //Add the video source here
  sonixGrabber = vtkSonixVideoSource::New();
  sonixGrabber->SetSonixIP("130.15.7.212");
  sonixGrabber->SetImagingMode(0);
  sonixGrabber->SetAcquisitionDataType(0x00000004);
  sonixGrabber->Record();        //start recording frame from the video

  viewer = vtkImageViewer::New();
  viewer->SetInput(sonixGrabber->GetOutput());   //set image to the render and window
  viewer->SetColorWindow(255);
  viewer->SetColorLevel(127.5);
  viewer->SetZSlice(0);
  
  //Create the interactor that handles the event loop
  iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(viewer->GetRenderWindow());
  viewer->SetupInteractor(iren);

  viewer->Render();  //must be called after iren and viewer are linked
  //or there will be problems
  
  //establish timer event and create timer
  vtkMyCallback *call = vtkMyCallback::New();
  iren->AddObserver(vtkCommand::TimerEvent, call);
  iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();
  
  //delete all instances and release the hold the win32videosource
  //has on the pci card  
  sonixGrabber->ReleaseSystemResources();
  sonixGrabber->Delete();
  viewer->Delete();
  iren->Delete();

  return 0;
}


