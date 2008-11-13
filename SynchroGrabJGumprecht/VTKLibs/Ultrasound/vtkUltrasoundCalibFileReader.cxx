/*=========================================================================

Module:    $RCSfile: vtkUltrasoundCalibFileReader.cxx,v $
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

  this->HomogeneousMatrix = vtkMatrix4x4::New();
  this->CalibrationMatrix = vtkMatrix4x4::New();

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
    vtkErrorMacro("File already open.");
    return 1;
    }

  if (!this->FileName)
  {
    vtkErrorMacro("File name not provided");
    return 0;
    }
    
  // Need to open a file.  First make sure it exists.  This prevents
  // an empty file from being created on older compilers.
  struct stat fs;
  if(stat(this->FileName, &fs) != 0)
    {
    vtkErrorMacro("Error opening file " << this->FileName);
    return 0;
    }
  this->FileStream = new ifstream(this->FileName, ios::in);
  
  if(!this->FileStream || !(*this->FileStream))
    {
    vtkErrorMacro("Error opening file " << this->FileName);
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
void vtkUltrasoundCalibFileReader::ReadCalibFile()
{
  if (this->OpenCalibFile() == 0)
    {
  // error opening file
  return;
  }
  
  this->Init();
  
  vtkstd::vector<double> numbers;
  // read the leading comments
  this->ReadBlankLines();
  this->ReadComments();
  this->ReadBlankLines();

  // read the date section
  this->ReadText();
  this->ReadBlankLines();
  this->ReadComments();
  this->ReadBlankLines();

  // read the size of the captured freehand images
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  // there should two/three numbers indicating size in x and y, read them into member variable
  if (numbers.size() == 3)
    {
  this->ImageDimensions = 3;
  this->ImageSize[0] = int(numbers.at(0));
  this->ImageSize[1] = int(numbers.at(1));
  this->ImageSize[2] = int(numbers.at(2));
  }
  else if (numbers.size() == 2)
  {
  this->ImageDimensions = 2;
  this->ImageSize[0] = int(numbers.at(0));
  this->ImageSize[1] = int(numbers.at(1));
  this->ImageSize[2] = 0;
  }
  else
  {
    //error
  }
  this->ReadBlankLines();

  // read the position of the US image frame centre
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  // there should two/three numbers indicating origin, read them into member variable
  if (numbers.size() == 3)
    {
  this->ImageOrigin[0] = int(numbers.at(0));
  this->ImageOrigin[1] = int(numbers.at(1));
  this->ImageOrigin[2] = int(numbers.at(2));;
  }
  else if (numbers.size() == 2)
  {
  this->ImageOrigin[0] = int(numbers.at(0));
  this->ImageOrigin[1] = int(numbers.at(1));
  this->ImageOrigin[2] = 0;  
  }
  else
  {
    //error
  }
  this->ReadBlankLines();

  // read the homogeneous transform matrix
  this->ReadComments();
  numbers.clear();
  this->ReadNumbers(numbers);
  if (numbers.size() == 16)
    {
  int vecIndex = 0;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
     {
     this->HomogeneousMatrix->SetElement(i,j,numbers.at(vecIndex++));
     }
  // convert from meters to mm
  this->HomogeneousMatrix->SetElement(0,3, this->HomogeneousMatrix->GetElement(0,3)*1000);
  this->HomogeneousMatrix->SetElement(1,3, this->HomogeneousMatrix->GetElement(1,3)*1000);
  this->HomogeneousMatrix->SetElement(2,3, this->HomogeneousMatrix->GetElement(2,3)*1000);
  this->CalibrationMatrix->DeepCopy(this->HomogeneousMatrix);
    }
  else
    {
  //error
    }
  
  // calculate calib matrix, image spacing,
   
//  Calculate();
   cout << "void vtkUltrasoundCalibFileReader::ReadCalibFile() | Calculate deactivated" << endl;
   
  this->ImageSpacing[0] = 1.0;
  this->ImageSpacing[1] = 1.0;
  this->ImageSpacing[2] = 1.0;
   
   
     
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

