/*=========================================================================

Module:  $RCSfile: vtkVideoSourceSimulator.h,v $
Author:  Jan Gumprecht, Harvard Medical School

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
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

 * Neither the name of Harvard Medical School nor the names of any
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
  

#ifndef __vtkVideoSourceSimulator_h
#define __vtkVideoSourceSimulator_h

#include "SynchroGrabConfigure.h"

#include "vtkUltrasoundWin32Header.h"
#include "vtkVideoSource.h"

//#undef NEW_SIMULATOR
#define NEW_SIMULATOR

#define BITS_PER_PIXEL 8

class vtkMultiThreader;

//Imaging mode | not used in the simualtor. Only here for consistency.
//enum {BMode};

//Acquistiontypes | not used in the simualtor. Only here for consistency.
//enum {udtBPost};


class VTK_ULTRASOUND_EXPORT vtkVideoSourceSimulator;

class VTK_EXPORT vtkVideoSourceSimulator : public vtkVideoSource
{
public:
  static vtkVideoSourceSimulator *New();
  vtkTypeRevisionMacro(vtkVideoSourceSimulator,vtkVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   
  
  // Description:
  // Return the singleton instance with no reference counting.
  static vtkVideoSourceSimulator* GetInstance();

  // Description:
  // Supply a user defined output window. Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance(vtkVideoSourceSimulator *instance);
  
  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called. 
  virtual void Record();

  // Description:
  // Play through the 'tape' sequentially at the specified frame rate.
  // If you have just finished Recoding, you should call Rewind() first.
  virtual void Play();

  // Description:
  // Stop recording or playing.
  virtual void Stop();

  // Description:
  // Grab a single video frame.
  void Grab();

  // Description:
  // Request a particular vtk output format (default: VTK_RGB).
  void SetOutputFormat(int format);

  // Description:
  // Request a particular mode of imaging (e.g. B-mode (0), M-mode(1), Color-doppler(2), pulsed-doppler(3); default: B-mode).
  void SetImagingMode(int mode){ImagingMode = mode;};
  void GetImagingMode(int & mode){mode = ImagingMode;};

  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/

  // Description:
  // Get/set the frame rate (fps) of B-mode ultrasound; valid range: ; in increments of 
  // FrameRate corresponds to paramID value of 584 for Uterius SDK
  vtkGetMacro(FrameRate, float);
  vtkSetMacro(FrameRate, float);

  // Description:
  // Request a particular data type from UltraSound machine by means of a mask.
  // Range of types supported:  1) Screen (800 x 600); 2) B Pre Scan Converted; 3) B Post Scan Converted (8 bit);
  //              4) B Post Scan Converted (32 bit); 5) RF; 6) M Pre Scan Converted;
  //              7) M Post Scan Converted; 8) PW RF; 9) PW Spectrum;
  //              10)Color RF; 11) Color Post; 12) Color Sigma;
  //              13)Color Velocity; 14) Elasto + B-image (32); 15) Elasto Overlay (8 bit); 16) Elasto Pre Scan Coverted (8 bit)
  // Currently supported data masks: 1) Screen
  // The mask must be applied before any data can be acquired via realtime imaging or cine retreival
  vtkGetMacro(AcquisitionDataType, int);
  vtkSetMacro(AcquisitionDataType, int);
  

  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  void Initialize();

  // Description:
  // Free the driver (this is called automatically inside the
  // destructor).
  void ReleaseSystemResources();

  //Description:
  // Request data method override
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
#ifdef NEW_SIMULATOR
  //Add an image to the frame buffer
  void InternalGrab();
#else
  //Fill Framebuffer with artificial images
  void FillFrameBuffer();
  
#endif

protected:
  vtkVideoSourceSimulator();
  ~vtkVideoSourceSimulator();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  float FrameRate;
  int AcquisitionDataType;
  int ImagingMode;
  
  void DoFormatSetup();
  
  //Multithreader to run a thread of recording and/or playing
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;
  
  
private:

  static vtkVideoSourceSimulator* Instance;
  vtkVideoSourceSimulator(const vtkVideoSourceSimulator&);  // Not implemented.
  void operator=(const vtkVideoSourceSimulator&);  // Not implemented.
};

#endif





