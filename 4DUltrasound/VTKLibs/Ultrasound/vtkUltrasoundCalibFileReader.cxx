/*=========================================================================

Module: $RCSfile: vtkUltrasoundCalibFileReader.cxx,v $
Author: Siddharth Vikal, Queens School Of Computing
Author: Jan Gumprecht, Harvard Medical School

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
Copyright (c) 2009, Birgham and Women's Hospital
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

 * Neither the name of Queen's University, Harvard Medical School,
   Brigham and Women's Hospitla nor the names of any
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

#include "SynchroGrabConfigure.h"
#include "vtkUltrasoundCalibFileReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

#include <sys/stat.h>
#include <assert.h>

vtkCxxRevisionMacro(vtkUltrasoundCalibFileReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkUltrasoundCalibFileReader);

const double vtkUltrasoundCalibFileReader::DefaultClipPixels[4] = {269, 0, 344, 440}; // static member


//----------------------------------------------------------------------------
vtkUltrasoundCalibFileReader::vtkUltrasoundCalibFileReader()
{
  this->FileName = 0;

  this->FileStream = 0;

  this->ImageDimensions = 0;

  this->ImageSize[0] = 0;
  this->ImageSize[1] = 0;
  this->ImageSize[2] = 0;

  this->ImageSpacing[0] = 0;
  this->ImageSpacing[1] = 0;
  this->ImageSpacing[2] = 0;

  this->ImageOrigin[0] = 0;
  this->ImageOrigin[1] = 0;
  this->ImageOrigin[2] = 0;

  this->ClipRectangle[0] = 0;
  this->ClipRectangle[1] = 0;
  this->ClipRectangle[2] = 0;
  this->ClipRectangle[3] = 0;
  
  this->ImageMargin[0] = 0;
  this->ImageMargin[1] = 0;
  this->ImageMargin[2] = 0;
  this->ImageMargin[3] = 0;

  this->HomogeneousMatrix = vtkMatrix4x4::New();
  this->CalibrationMatrix = vtkMatrix4x4::New();

  this->LogStream.ostream::rdbuf(cerr.rdbuf());
  
  this->UltrasoundScanDepth = -1;//mm
  this->UltrasoundScanFanHeight = -1;//Pixel
  
  this->ShrinkFactor[0] = 1;
  this->ShrinkFactor[1] = 1;
  this->ShrinkFactor[2] = 1;
  
  this->TransformationFactorMmToPixel = 1;
  
  this->ObliquenessAdjustmentMatrix = vtkMatrix4x4::New();
  this->ObliquenessAdjustmentMatrix->Identity();
  
  this->CoordinateTransformationMatrix = vtkMatrix4x4::New();
  this->CoordinateTransformationMatrix->Identity();
  
  this->MaximumVolumeSize[0] = -1;
  this->MaximumVolumeSize[1] = -1;
  this->MaximumVolumeSize[2] = -1;
}

//----------------------------------------------------------------------------
vtkUltrasoundCalibFileReader::~vtkUltrasoundCalibFileReader()
{
  this->SetFileName(0);
  this->FileStream = 0;
  this->HomogeneousMatrix->Delete();
  this->CalibrationMatrix->Delete();
}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: "
     << (this->FileName? this->FileName:"(none)") << "\n";
  os << indent << "Image size: " << this->ImageSize[0] << " " << this->ImageSize[1] <<" " << this->ImageSize[2]<< "\n";
  os << indent << "Image origin: " << this->ImageOrigin[0] << " "<< this->ImageOrigin[1]<<" "<<this->ImageOrigin[2] << "\n";
  os << indent << "Image spacing: " << this->ImageSpacing[0] << " "<<this->ImageSpacing[1]<<" "<<this->ImageSpacing[2] << "\n";
  os << indent << "Calibration matrix: " << this->CalibrationMatrix << "\n";
  if (this->CalibrationMatrix)
    {
    this->CalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
    }  
}

//----------------------------------------------------------------------------
int vtkUltrasoundCalibFileReader::OpenCalibFile()
{
  if(this->FileStream)
    {
    this->LogStream << "WARNING: Calibration file already open.";
    return 1;
    }

  if (!this->FileName)
  {
    this->LogStream << "ERROR: Calibration file name not provided";
    return 0;
    }
    
  // Need to open a file.  First make sure it exists.  This prevents
  // an empty file from being created on older compilers.
  struct stat fs;
  if(stat(this->FileName, &fs) != 0)
    {
    this->LogStream << "ERROR opening calibration file " << this->FileName;
    return 0;
    }
  this->FileStream = new ifstream(this->FileName, ios::in);
  
  if(!this->FileStream || !(*this->FileStream))
    {
    this->LogStream << "ERROR opening calibration file " << this->FileName;
    if(this->FileStream)
      {
      delete this->FileStream;
      this->FileStream = 0;
      }
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::CloseCalibFile()
{  
  if(this->FileStream)
    {
    // We opened the file.  Close it.
    this->FileStream->close();
    delete this->FileStream;
    this->FileStream = 0;    
    }
}
//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::Init()
{
  // read all the lines of the file into a vector of strings/lines
  char curLine[256] = "";
  int lineCount = 0;
  //while (!this->FileStream->eof())
  while (this->FileStream->good())
    {
  ++lineCount;
  this->FileStream->getline(curLine, 256, '\n');  
  this->Lines.push_back(curLine);
  }
  this->LinesIterator = this->Lines.begin();
}
//----------------------------------------------------------------------------
int vtkUltrasoundCalibFileReader::ReadCalibFile()
{
  if (this->OpenCalibFile() == 0)
    {
    // error opening file
    return -1;
    }
  
  this->Init();
  
  vtkstd::vector<double> numbers;
  //Read leading comments-------------------------------------------------------
  this->ReadBlankLines();
  this->ReadComments();
  this->ReadBlankLines();

  //Read image setting----------------------------------------------------------
  this->ReadComments();
  this->ReadBlankLines();
  
  //Read image size
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  if (numbers.size() == 2)
    {
    this->ImageDimensions = 2;
    this->ImageSize[0] = int(numbers.at(0));
    this->ImageSize[1] = int(numbers.at(1));
    this->ImageSize[2] = 0;
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read image size" << endl;
    return -1;
    }
  this->ReadBlankLines();
  
  //Read image margins
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  // there should two/three numbers indicating size in x and y, read them into member variable
  if (numbers.size() == 4)
    {
    this->ImageMargin[1] = int(numbers.at(0));//Top and Bottom are exchanged
    this->ImageMargin[0] = int(numbers.at(1));
    this->ImageMargin[2] = int(numbers.at(2));
    this->ImageMargin[3] = int(numbers.at(3));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Image margin is " << this->ImageMargin[0] << "| "<< this->ImageMargin[1]<< "| "<< this->ImageMargin[2] << "| "<< this->ImageMargin[3] << endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read Image margins" << endl;
    return -1;
    }
  this->ReadBlankLines();
  
  //----------------------------------------------------------------------------
  //Calibration matrices
  this->ReadComments();
  this->ReadBlankLines();
  
  //Read Tracker Offset
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
   
  if (numbers.size() == 3)
    {
    this->TrackerOffset[0] = int(numbers.at(0));
    this->TrackerOffset[1] = int(numbers.at(1));
    this->TrackerOffset[2] = int(numbers.at(2));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << setprecision(8) <<"CF-INFO: Tracker offset is " << this->TrackerOffset[0] << "| "<< this->TrackerOffset[1]<< "| "<< this->TrackerOffset[2]<< endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read Tracker offset" << endl;
    return -1;
    }
  this->ReadBlankLines();
  
  //Read Tracker obliqueness adjustment matrix
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  int vecIndex = 0;
  if (numbers.size() == 16)
    {
    for (int i = 0; i < 4; i++)
      {
      for (int j = 0; j < 4; j++)
       {
       this->ObliquenessAdjustmentMatrix->Element[i][j] = numbers.at(vecIndex++);
       }
      }
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Obliqueness adjustment matrix is: " << endl;
    this->ObliquenessAdjustmentMatrix->Print(this->LogStream);
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read obliqueness adjustment matrix" << endl;
    return -1;
    }
  
   this->ReadBlankLines();
    
  //Read Tracker coordinate adjustment matrix
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  vecIndex = 0;
  if (numbers.size() == 16)
    {
    for (int i = 0; i < 4; i++)
      {
      for (int j = 0; j < 4; j++)
       {
       this->CoordinateTransformationMatrix->Element[i][j] = numbers.at(vecIndex++);
       }
      }
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Coordinate transformation matrix is: " << endl;
    this->CoordinateTransformationMatrix->Print(this->LogStream);
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read coordinate transformation matrix" << endl;
    return -1;
    }
  
  
  this->ReadBlankLines();
  
  //Read system offset
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  if (numbers.size() == 3)
    {
    this->SystemOffset[0] = int(numbers.at(0));
    this->SystemOffset[1] = int(numbers.at(1));
    this->SystemOffset[2] = int(numbers.at(2));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: System offset is " << this->SystemOffset[0] << "| "<< this->SystemOffset[1]<< "| "<< this->SystemOffset[2]<< endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read system offset" << endl;
    return -1;
    }
  
  
  this->ReadBlankLines();
  
  //----------------------------------------------------------------------------
  //System setting section
  this->ReadComments();
  this->ReadBlankLines();
  
  //Read max volume size
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  if (numbers.size() == 3)
    {
    this->MaximumVolumeSize[0] = int(numbers.at(0));
    this->MaximumVolumeSize[1] = int(numbers.at(1));
    this->MaximumVolumeSize[2] = int(numbers.at(2));

    #ifdef HIGH_DEFINITION
    this->MaximumVolumeSize[0] *= 3; //Byte
    this->MaximumVolumeSize[1] *= 3; //Byte
    this->MaximumVolumeSize[2] *= 3; //Byte
    #endif
    
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Maximum volume size is " << this->MaximumVolumeSize[0] << "|" << this->MaximumVolumeSize[1] << "|" << this->MaximumVolumeSize[2] << endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read maximum volume size" << endl;
    return -1;
    }
  
  this->ReadBlankLines();
  
  //Read video device name
  this->ReadComments();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
  
  //Read Video Channel
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
  
  //Read Video Mode
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
  
  //Read Delay Factor
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  if(numbers.size() == 1)
    {
    this->DelayFactor = int(numbers.at(0));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Delay factor size is " << this->DelayFactor << endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read delay factor" << endl;
    return -1;
    }
  
  
  this->ReadBlankLines();
  
  //----------------------------------------------------------------------------
  //Ultrasound setting section
  this->ReadComments();
  this->ReadBlankLines();
  
  //Read Frames per second
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
  
  //Read Ultrasound Scan depth
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  if(numbers.size() == 1)
    {
    this->UltrasoundScanDepth = int(numbers.at(0));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Ultrasound scan depth is " << this->UltrasoundScanDepth << endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read ultrasound scan depth" << endl;
    return -1;
    }
  
  this->ReadBlankLines();
  
  //Read Ultrasound Scan fan height
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  if(numbers.size() == 1)
    {
    this->UltrasoundScanFanHeight = int(numbers.at(0));
    #ifdef DEBUG_CALIBRATIONFILE_READER
    this->LogStream << "CF-INFO: Ultrasound scan fan height is " << this->UltrasoundScanFanHeight << endl;
    #endif
    }
  else
    {
    this->LogStream << "ERROR in Calibration File: Cannot read ultrasound scan fan height" << endl;
    return -1;
    }
  
  this->ReadBlankLines();
  
  //----------------------------------------------------------------------------
  //Instrument setting section
  this->ReadComments();
  this->ReadBlankLines();
  
  //Instrument tracking rate per second
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
    
  //----------------------------------------------------------------------------
  //OpenIGTLink settings
  this->ReadComments();
  this->ReadBlankLines();
  
  //OpenIGTLinkServer
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();

  //OpenIGTLinkPort US Tracker
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  this->ReadBlankLines();
  
  //OpenIGTLinkPort Instrument Tracker
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  
  //----------------------------------------------------------------------------
  //End of reading section do calculations
  
  this->TransformationFactorMmToPixel = this->UltrasoundScanFanHeight / this->UltrasoundScanDepth * 4.0 / 3.0;
  
  #ifdef HIGH_DEFINITION
  this->SystemOffset[0] = this->SystemOffset[0] * this->TransformationFactorMmToPixel;
  this->SystemOffset[1] = this->SystemOffset[1] * this->TransformationFactorMmToPixel;
  this->SystemOffset[2] = this->SystemOffset[2] * this->TransformationFactorMmToPixel;
  #endif

  
  //cout << "ShrinkFractor[0]: " << this->ShrinkFactor[1] << endl;
  
#ifdef DO_CALIBRATION_FILE_CALCULATIONS
  // calculate calib matrix, image spacing,   
  Calculate();  
#else
  //The calculations don't work at the moment
  
  #ifdef HIGH_DEFINITION
  this->ShrinkFactor[0] = 1; 
  this->ShrinkFactor[1] = 1;
  #else
  this->ShrinkFactor[0] = (int) (this->TransformationFactorMmToPixel + 0.5);//X 
  this->ShrinkFactor[1] = (int) (this->TransformationFactorMmToPixel + 0.5);//Y
//  this->ShrinkFactor[0] = 1;//X 
//  this->ShrinkFactor[1] = 1;//Y
  #endif
  
  this->LogStream << "CF-INFO: Shrinkfactor is: " << this->ShrinkFactor[0] << " | " << this->ShrinkFactor[1] << " | " << this->ShrinkFactor[2]<< endl;
  
  this->ClipRectangle[0] = 0;
  this->ClipRectangle[1] = 0;
  this->ClipRectangle[2] = (this->ImageSize[0] - this->ImageMargin[2] - this->ImageMargin[3] -1) / this->ShrinkFactor[0];
  this->ClipRectangle[3] = (this->ImageSize[1] - this->ImageMargin[0] - this->ImageMargin[1] -1) / this->ShrinkFactor[1];

  this->LogStream << "CF-INFO: ClipRectangle is: " << this->ClipRectangle[0] << " - " << this->ClipRectangle[1] << " | " << this->ClipRectangle[2] <<" - " << this->ClipRectangle[3] << endl;
  
  #ifdef HIGH_DEFINITION
//  this->ImageSpacing[0] = 0.25;
//  this->ImageSpacing[1] = 0.25;
//  this->ImageSpacing[2] = 0.25;
  this->ImageSpacing[0] = 1;
  this->ImageSpacing[1] = 1;
  this->ImageSpacing[2] = 1;
  #else
  this->ImageSpacing[0] = 1.0;
  this->ImageSpacing[1] = 1.0;
  this->ImageSpacing[2] = 1.0;
  #endif
  //this->LogStream << "Calibration File: ClipRectangle 0 - "<< this->ClipRectangle[2] <<"| 0 - "<< this->ClipRectangle[3]<< endl;
  
#endif
   
  // close the file stream
  this->CloseCalibFile();
  
}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::Calculate()
{

  // calculate the magnitude of the first 2 columns
  double xnorm = sqrt(this->HomogeneousMatrix->GetElement(0,0)*this->HomogeneousMatrix->GetElement(0,0) + this->HomogeneousMatrix->GetElement(1,0)*this->HomogeneousMatrix->GetElement(1,0) + this->HomogeneousMatrix->GetElement(2,0)*this->HomogeneousMatrix->GetElement(2,0));
  double ynorm = sqrt(this->HomogeneousMatrix->GetElement(0,1)*this->HomogeneousMatrix->GetElement(0,1) + this->HomogeneousMatrix->GetElement(1,1)*this->HomogeneousMatrix->GetElement(1,1) + this->HomogeneousMatrix->GetElement(2,1)*this->HomogeneousMatrix->GetElement(2,1));

  // calculate the PixelSpacing (in millimetres)
  this->ImageSpacing[0] = xnorm*1000;
  this->ImageSpacing[1] = ynorm*1000;

  this->ImageSpacing[2] = this->ImageSpacing[0];
  this->ImageOrigin[0] = - this->ImageOrigin[0] * this->ImageSpacing[0];
  this->ImageOrigin[1] = ( this->ImageOrigin[1] - this->ImageSize[1] + 1 ) * this->ImageSpacing[1];

  // normalize the length of the first 2 columns to make unit vectors
  for( int i= 0; i < 3; i++)
    {
  this->CalibrationMatrix->SetElement(i,0, this->CalibrationMatrix->GetElement(i,0)/xnorm);
  this->CalibrationMatrix->SetElement(i,1, this->CalibrationMatrix->GetElement(i,1)/ynorm);    
    }

  // change the sign of the y values to vertically flip
  // because VTK uses lower-left origin convention
  vtkMatrix4x4 *flipmatrix = vtkMatrix4x4::New();
  double  flip[16] = {1, 0, 0, 0,0,-1, 0, 0,0, 0, 1, 0,0, 0, 0, 1};
  flipmatrix->DeepCopy(flip);

  // matrix multiplication
  vtkMatrix4x4::Multiply4x4(this->CalibrationMatrix,flipmatrix,this->CalibrationMatrix);

  // make a proper rotation matrix: set third column to the cross
  // product of the 1st and 2nd columns
  this->CalibrationMatrix->SetElement(0,2, this->CalibrationMatrix->GetElement(1,0)*this->CalibrationMatrix->GetElement(2,1) - this->CalibrationMatrix->GetElement(2,0)*this->CalibrationMatrix->GetElement(1,1));
  this->CalibrationMatrix->SetElement(1,2, this->CalibrationMatrix->GetElement(2,0)*this->CalibrationMatrix->GetElement(0,1) - this->CalibrationMatrix->GetElement(0,0)*this->CalibrationMatrix->GetElement(2,1));
  this->CalibrationMatrix->SetElement(2,2, this->CalibrationMatrix->GetElement(0,0)*this->CalibrationMatrix->GetElement(1,1) - this->CalibrationMatrix->GetElement(1,0)*this->CalibrationMatrix->GetElement(0,1));
  
  // the clip window is not currently stored in the calibration file, so we have to use the default value
  this->ClipRectangle[0] = vtkUltrasoundCalibFileReader::DefaultClipPixels[0]*this->ImageSpacing[0]+this->ImageOrigin[0];
  this->ClipRectangle[1] = vtkUltrasoundCalibFileReader::DefaultClipPixels[1]*this->ImageSpacing[1]+this->ImageOrigin[1];
  this->ClipRectangle[2] = vtkUltrasoundCalibFileReader::DefaultClipPixels[2]*this->ImageSpacing[0]+this->ImageOrigin[0];
  this->ClipRectangle[3] = vtkUltrasoundCalibFileReader::DefaultClipPixels[3]*this->ImageSpacing[1]+this->ImageOrigin[1];
}
//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::ReadComments()
{
  /*
        """Read all comment lines starting with '#'.
        """
        # list to store comment lines
        comments = []
        while lines and lines[0].strip() != "" and lines[0][0] == '#':
            comments.append(lines[0])
            # remove the first line from the list
            del lines[0]
        return comments
  */
  if (this->LinesIterator != this->Lines.end())
    {
  vtkstd::string curLine = *this->LinesIterator;
  while (!curLine.empty() && (curLine.c_str()[0] == '#') )
    {
    this->LinesIterator++;
    curLine = *this->LinesIterator;
    }
    }
}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::ReadNumbers(vtkstd::vector<double> & numbers)
{
  /*
  """Read numbers until comment line or blank line.
  """
  numbers = []
  while lines and lines[0].strip() != "" and lines[0][0] != '#':
      numbers.append(map(float, lines[0].split()))
      # remove the first line from the list
      del lines[0]
  return numbers
  */
  if (this->LinesIterator != this->Lines.end())
    {
  vtkstd::string curLine = *this->LinesIterator;
  while (!curLine.empty() && (curLine.c_str()[0] != '#') )
    {
    const char *token = curLine.data();
    for ( unsigned int index = 0; index < curLine.length(); ) /* loop through each character on the line */         
      {            
    /* search for delimiters */            
    size_t len = strcspn(token, "\t \n");
    if (len >0)
      numbers.push_back(atof(token));
    token += len + 1;
    index += len + 1;
      }
    this->LinesIterator++;
    curLine = *this->LinesIterator;
    }
  }

}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::ReadText()
{
  /*
        """Read text until comment line or blank line.
        """
        text = []
        while lines and lines[0].strip() != "" and lines[0][0] != '#':
            text.append(lines[0])
            # remove the first line from the list
            del lines[0]
        return text
    */
  if (this->LinesIterator != this->Lines.end())
    {
  vtkstd::string curLine = *this->LinesIterator;
  while (!curLine.empty() && (curLine.c_str()[0] != '#') )
    {
    this->LinesIterator++;
    curLine = *this->LinesIterator;
    }
    }
}

//----------------------------------------------------------------------------
void vtkUltrasoundCalibFileReader::ReadBlankLines()
{
  // Read consecutive blank lines in the file.
  // while lines and lines[0].strip() == "":
  //        del lines[0]
  if (this->LinesIterator != this->Lines.end())
    {
    vtkstd::string curLine = *this->LinesIterator;
    while (curLine.empty())
    {
    this->LinesIterator++;
    curLine = *this->LinesIterator;
    }
    }
    
}

/******************************************************************************
 * void vtkUltrasoundCalibFileReader::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  15.February 2009
 *
 * ****************************************************************************/
void vtkUltrasoundCalibFileReader::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkUltrasoundCalibFileReader::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  15.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkUltrasoundCalibFileReader::GetLogStream()
{
  return this->LogStream;
}
