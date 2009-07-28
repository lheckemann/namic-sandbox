/*=========================================================================

Module:    $RCSfile: vtkUltrasoundCalibFileReader.h,v $
Author: Siddharth Vikal, Queen's school of computing

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

// .NAME vtkUltrasoundCalibFileReader - 
// .SECTION Description: a file to read the calibration file generated for the ultrasound probe
// Note that the file reading is strongly tied to the current file format.; Any changes in file format
// will result in changes to the function: ReadCalibFile
// The class outputs following information as read, calculated from the calib file:
// 1) Image origin
// 2) Image size
// 3) Image spacing
// 4) 4 x 4 homogeneous matrix as in the file
// 5) 4 x 4 calibration matrix calculated
// vtkUltrasoundCalibFileReader class usage
// calibReader->SetFileName("..");
// calibReader->ReadCalibFile();
// calibReader->GetImageOrigin();
// calibReader->GetImageSize();
// calibReader->GetImageSpacing();
// calibReader->GetCalibrationMatrix();


#ifndef __vtkUltrasoundCalibFileReader_h
#define __vtkUltrasoundCalibFileReader_h

#include "vtkUltrasoundWin32Header.h"
#include "vtkAlgorithm.h"

#include "SynchroGrabConfigure.h"

#include <vtkstd/vector> // Required for vector


class vtkInformationVector;
class vtkMatrix4x4;

class VTK_ULTRASOUND_EXPORT vtkUltrasoundCalibFileReader : public vtkAlgorithm
{
public:
  static vtkUltrasoundCalibFileReader *New();
  vtkTypeRevisionMacro(vtkUltrasoundCalibFileReader,vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Get/Set the name of the input file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Get image dimensions
  vtkGetMacro(ImageDimensions, int);
  // Description:
  // Get image size
  vtkGetVector3Macro(ImageSize,int);

  // Description:
  // Get the spacing of the data in the file.
  vtkGetVector3Macro(ImageSpacing,double);
  
  // Description:
  // Get the origin of the data (location of first pixel in the file).  
  vtkGetVector3Macro(ImageOrigin,double);

  vtkGetVector4Macro(ClipRectangle,double);

  // Description:
  // Get the homogeneous calibration matrix as originally in file except in mm as opposed to m in file.
  vtkMatrix4x4 *GetHomogeneousMatrix(){return this->HomogeneousMatrix;};

  // Description:
  // Get the calibration matrix.
  vtkMatrix4x4 *GetCalibrationMatrix(){return this->CalibrationMatrix;};

  // Description:
  // Reads file, does file i/o; main function to be called
  int ReadCalibFile();

  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();
  
  vtkGetVector4Macro(ImageMargin, int);
  
  vtkGetVector3Macro(ShrinkFactor, int);
  
  vtkGetVector3Macro(SystemOffset, double);
  
  vtkGetVector3Macro(MaximumVolumeSize, double);
  
  vtkGetMacro(DelayFactor, int);
  
  vtkGetMacro(TransformationFactorMmToPixel, double);
  
  vtkGetVector3Macro(TrackerOffset, double);
  
  vtkGetMacro(ObliquenessAdjustmentMatrix, vtkMatrix4x4*);
  
  vtkGetMacro(CoordinateTransformationMatrix, vtkMatrix4x4*);
  
  vtkGetStringMacro(VideoSource);

  vtkGetMacro(VideoChannel, int);

  vtkGetMacro(VideoMode, int);

  vtkGetMacro(FramesPerSecond, int);

  vtkGetStringMacro(OpenIGTLinkServer);

  vtkGetMacro(OpenIGTLinkServerPortUltrasound, int);

  vtkGetMacro(OpenIGTLinkServerPortTrackedInstrument, int);

protected:
  vtkUltrasoundCalibFileReader();
  ~vtkUltrasoundCalibFileReader();
   
  // Internal utility methods.
  int OpenCalibFile();
  void CloseCalibFile();
  void Init();
  void ReadText(std::vector<char, std::allocator<char> >& text);
  void ReadComments();
  void ReadNumbers(vtkstd::vector<double> & numbers);
  void ReadBlankLines();
  void Calculate();

  //File stream for error logging
  ofstream LogStream;

  // The input file's name.
  char* FileName;
  
  // Whether there was an error reading the XML.
  int ReadError;

  //Image Settings
  int ImageSize[3];
  int ImageDimensions;
  int ImageMargin[4];

  //Calibration Matrices
  double TrackerOffset[3];
  vtkMatrix4x4 *ObliquenessAdjustmentMatrix;
  vtkMatrix4x4 *CoordinateTransformationMatrix;
  double SystemOffset[3];

  //System Settings
  double MaximumVolumeSize[3];
  char *VideoSource;
  int VideoChannel;
  int VideoMode;
  int DelayFactor;

  //Ultrasound Settings
  int FramesPerSecond;
  double UltrasoundScanDepth;
  double UltrasoundScanFanHeight;

  //Instrument Settings
  int InstrumentTrackingRate;

  //OpenIGTLink Settings
  char *OpenIGTLinkServer;
  int OpenIGTLinkServerPortUltrasound;
  int OpenIGTLinkServerPortTrackedInstrument;

  double ImageSpacing[3];
  double ImageOrigin[3];
  double ClipRectangle[4];
  static const double DefaultClipPixels[4];
  int ShrinkFactor[3];
  double TransformationFactorMmToPixel;
  
  vtkMatrix4x4 *HomogeneousMatrix;
  vtkMatrix4x4 *CalibrationMatrix;

  vtkstd::vector <vtkstd::string> Lines;
  vtkstd::vector<vtkstd::string>::iterator LinesIterator;
private:
  // The stream used to read the input if it is in a file.
  ifstream* FileStream;  
 
  
private:
  vtkUltrasoundCalibFileReader(const vtkUltrasoundCalibFileReader&);  // Not implemented.
  void operator=(const vtkUltrasoundCalibFileReader&);  // Not implemented.
};

#endif
