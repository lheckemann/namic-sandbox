/*=========================================================================

Module:    $RCSfile: vtkV4LVideoSource.h,v $
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
  
// .NAME vtkV4LVideoSource - VTK interface for video input from UltraSound machine
// .SECTION Description
// vtkV4LVideoSource is a class for providing video input interfaces between VTK and UltraSound machine.
// The goal is to provide the ability to be able to do acquisition
// in various imaging modes, buffer the image/volume series being acquired
// and stream the frames to output. 
// Note that the data coming out of the UltraSound rp through ulterius is always RGB
// This class talks to UltraSound's Ulterius SDK for executing the tasks 
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// .SECTION Usage
//  sonixGrabber->SetSonixIP("130.15.7.212");
//  sonixGrabber->SetImagingMode(0);
//  sonixGrabber->SetAcquisitionDataType(0x00000004);
//  sonixGrabber->Record();  
//  imageviewer->SetInput(sonixGrabber->GetOutput());
//  See SonixVideoSourceTest.cxx for more details
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource




#ifndef __vtkV4LVideoSource_h
#define __vtkV4LVideoSource_h

#include "vtkUltrasoundWin32Header.h"
#include "vtkVideoSource.h"

#define VOLUME_X_LENGTH 256
#define VOLUME_Y_LENGTH 256
#define VOLUME_Z_LENGTH 1

#define BITS_PER_PIXEL 8

//BTX

class uDataDesc;
class ulterius;

class VTK_ULTRASOUND_EXPORT vtkV4LVideoSource;

class VTK_EXPORT vtkV4LVideoSource : public vtkVideoSource
{
public:
  //static vtkV4LVideoSource *New();
  vtkTypeRevisionMacro(vtkV4LVideoSource,vtkVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   
  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkOutputWindow object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.   The single instance will be unreferenced when
  // the program exits.
  static vtkV4LVideoSource* New();
  // Description:
  // Return the singleton instance with no reference counting.
  static vtkV4LVideoSource* GetInstance();

  // Description:
  // Supply a user defined output window. Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance(vtkV4LVideoSource *instance);
  
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

  // Description:
  // Give the IP address of the UltraSound machine
  void SetUltraSoundIP(char *UltraSoundIP);
  
  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/

  // Description:
  // Request a particular mode of imaging (e.g. B-mode (0), M-mode(1), Color-doppler(2), pulsed-doppler(3); default: B-mode).
  // Frequency corresponds to paramID value of 414 for Uterius SDK
  vtkGetMacro(Frequency, int);
  vtkSetMacro(Frequency, int);

  // Description:
  // Get/set the depth (mm) of B-mode ultrasound; valid range: ; in increments of 
  // Depth corresponds to paramID value of 206 for Uterius SDK
  vtkGetMacro(Depth, int);
  vtkSetMacro(Depth, int);

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

protected:
  vtkV4LVideoSource();
  ~vtkV4LVideoSource();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

//  ulterius *ult;
//  uDataDesc *DataDescriptor;
  int Frequency;
  int Depth;
  float FrameRate;
  int AcquisitionDataType;
  int ImagingMode;
  
  char *UltraSoundHostIP;

//  void UnpackRasterLine(char *outptr, char *inptr, int start, int count);

  void DoFormatSetup();

  
  //Fill Framebuffer with artificial images
  void FillFrameBuffer();
  
private:
 

  static vtkV4LVideoSource* Instance;
  vtkV4LVideoSource(const vtkV4LVideoSource&);  // Not implemented.
  void operator=(const vtkV4LVideoSource&);  // Not implemented.
};

#endif





