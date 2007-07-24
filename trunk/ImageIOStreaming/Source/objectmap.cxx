/*************************************************************************
Copyright (c) 2007, Regents of the University of Iowa

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The University of Iowa nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/
//
// Name: objectmap.cpp
// Author: John Dill
// Date: 5/17/00
//

#include "objectmap.h"
#include "itkImageRegionIterator.h"
namespace itk{
  AnalyzeObjectMap::AnalyzeObjectMap( void ): m_Version(VERSION7),m_NumberOfObjects(0)
  {
    //TODO:  Clear the image this->ImageClear();
    {
      for (int i = 0; i < 256; i++)
      {
        AnaylzeObjectEntryArray[i] = NULL;
      }
    }

    // Setting object zero as the background
    AnaylzeObjectEntryArray[0] = AnalyzeObjectEntry::New();
    this->getObjectEntry(0)->SetName("Background");
    this->getObjectEntry(0)->SetDisplayFlag(0);
    this->getObjectEntry(0)->SetOpacity(0);
    this->getObjectEntry(0)->SetOpacityThickness(0);
    this->getObjectEntry(0)->SetEndRed(0);
    this->getObjectEntry(0)->SetEndGreen(0);
    this->getObjectEntry(0)->SetEndBlue(0);
    this->getObjectEntry(0)->SetShades(1);
  }

  AnalyzeObjectMap::~AnalyzeObjectMap( void )
  {
    for(int i=0; i < 256; i++)
    {
      if(AnaylzeObjectEntryArray[i].IsNotNull())
      {
        AnaylzeObjectEntryArray[i] = NULL;
      }
    }
  }

  AnalyzeObjectMap &  AnalyzeObjectMap::operator=( const AnalyzeObjectMap & rhs )
  {
    //Not yet implemented
    exit(-1);
    #if 0 // The following is incomplete
    //this->CopyBaseImage( rhs );

    this->SetVersion(rhs.GetVersion());
    this->SetNumberOfObjects(rhs.GetNumberOfObjects());
    for(int i=0; i < 256; i++)
    {
      if(rhs.AnaylzeObjectEntryArray[i].IsNotNull())
      {
        AnaylzeObjectEntryArray[i] = AnalyzeObjectEntry::New();
        AnaylzeObjectEntryArray[i]->Copy(rhs.AnaylzeObjectEntryArray[i]);
      }
      else
      {
        if(AnaylzeObjectEntryArray[i].IsNotNull())
        {
          AnaylzeObjectEntryArray[i] = NULL;
        }
      }
    }
    #endif
    return *this;
  }

  AnalyzeObjectEntry::Pointer AnalyzeObjectMap::getObjectEntry( const int index )
  {
    return AnaylzeObjectEntryArray[index];
  }

  const AnalyzeObjectEntry::Pointer  AnalyzeObjectMap::getObjectEntry( const int index ) const
  {
    return AnaylzeObjectEntryArray[index];
  }

 

  bool AnalyzeObjectMap::WriteObjectFile( const std::string& filename )
  {
    std::string tempfilename=filename;
    
    // This assumes that the final '.' in a file name is the delimiter
    // for the file's extension type
    const std::string::size_type it = filename.find_last_of( "." );
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if (fileExt != ".obj")
    {
      tempfilename=filename+".obj";
    }

    // Opening the file
    std::ofstream outputFileStream;
    outputFileStream.open(tempfilename.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if ( !outputFileStream.is_open())
    {
      std::cout<<"Error: Could not open "<< tempfilename.c_str()<<std::endl;
      exit(-1);
    }

    int header[5];
    header[0]=this->GetVersion();
    header[1]=this->GetXDim();
    header[2]=this->GetYDim();
    header[3]=this->GetZDim();
    header[4]=this->GetNumberOfObjects();

    //All object maps are written in BigEndian format as required by the AnalyzeObjectMap documentation.
    bool NeedByteSwap=itk::ByteSwapper<int>::SystemIsLittleEndian();
    if(NeedByteSwap)
    {
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[0]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[1]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[2]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[3]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[4]));
    }

    // Writing the header, which contains the version number, the size, and the
    // number of objects
    if(outputFileStream.write(reinterpret_cast<char *>(header), sizeof(int)*5).fail())
    {
      std::cout<<"Error: Could not write header of "<< filename.c_str()<<std::endl;
      exit(-1);
    }

    if(this->GetVersion() == VERSION7 )
    {
      int nvols = this->GetNumberOfVolumes(); 
      if( NeedByteSwap )
      {
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&nvols);
      }
      if(outputFileStream.write(reinterpret_cast<char *>(&nvols), sizeof(int)).fail())
      {
        std::cout<<"Could not write the number of volumes to the file"<<std::endl;
      }
    }

    // Error checking the number of objects in the object file
    if ((this->GetNumberOfObjects() < 0) || (this->GetNumberOfObjects() > 255))
    {
      std::cout<<( stderr, "Error: Invalid number of object files.\n" );
      outputFileStream.close();
      return false;
    }

    // Since the NumberOfObjects does not reflect the background, the background will be included
    for (int i = 0; i < this->GetNumberOfObjects(); i++)
    {
      // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
      // not affect the current object itself
      AnalyzeObjectEntry *ObjectWrite = this->getObjectEntry(i);
      if (NeedByteSwap == true)
      {
        ObjectWrite->SwapObjectEndedness();
      }
      ObjectWrite->Write(outputFileStream); 

    }
    RunLengthEncodeImage(outputFileStream);
    outputFileStream.close();
    return true;
  }


  bool itk::AnalyzeObjectMap::RunLengthEncodeImage(std::ofstream &fptr)
  {
    if (fptr == NULL)
    {
      std::cout<<"Error: Null file pointer for runlength encoding data."<<std::endl;
      return false;
    }

    // Encoding the run length encoded raw data into an unsigned char volume
    const int VolumeSize=this->GetXDim()*this->GetYDim()*this->GetZDim();
    const int PlaneSize = this->GetXDim()*this->GetYDim();
    int bufferindex=0;
    int planeindex=0;
    int runlength=0;
    unsigned char CurrentObjIndex=0;
    const  int buffer_size=16384;      //NOTE: This is probably overkill, but it will work
    unsigned char buffer[buffer_size];

    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexIt(this,this->GetLargestPossibleRegion());
    for(indexIt.GoToBegin();!indexIt.IsAtEnd();++indexIt)
    {
      if (runlength==0)
      {
          CurrentObjIndex = indexIt.Get();
          runlength=1;
      }
      else
      {
        if (CurrentObjIndex==indexIt.Get())
        {
          runlength++;
          if (runlength==255)
          {
            buffer[bufferindex]=runlength;
            buffer[bufferindex+1]=CurrentObjIndex;
            bufferindex+=2;
            runlength=0;
          }
        }
        else
        {
          buffer[bufferindex]=runlength;
          buffer[bufferindex+1]=CurrentObjIndex;
          bufferindex+=2;
          CurrentObjIndex=indexIt.Get();
          runlength=1;
        }
      }

      planeindex++;
      if (planeindex==PlaneSize)
      {
        // Just finished with a plane of data, so encode it
        planeindex=0;
        if (runlength!=0)
        {
          buffer[bufferindex]=runlength;
          buffer[bufferindex+1]=CurrentObjIndex;
          bufferindex+=2;
          runlength=0;
        }
      }
      if (bufferindex==buffer_size)
      {
        // buffer full
        if (fptr.write(reinterpret_cast<char *>(buffer), buffer_size).fail())
        {
            std::cout<<"error: could not write buffer"<<std::endl;
            exit(-1);
        }
        bufferindex=0;
      }

    }
    if (bufferindex!=0)
    {
      if (runlength!=0)
      {
        buffer[bufferindex]=runlength;
        buffer[bufferindex+1]=CurrentObjIndex;
        bufferindex+=2;
      }
      if(fptr.write(reinterpret_cast<char *>(buffer), bufferindex).fail())
      {
        std::cout<<"error: could not write buffer"<<std::endl;
        exit(-1);
      }
    }
    
    return true;
  }
}
