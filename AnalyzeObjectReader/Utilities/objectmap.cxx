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

  bool AnalyzeObjectMap::ReadObjectFile( const std::string& filename )
  {
    // Opening the file
    std::ifstream inputFileStream;
    inputFileStream.open(filename.c_str(), std::ios::binary | std::ios::in);
    if ( !inputFileStream.is_open())
    {
      std::cout<< "Error: Could not open %s\n", filename.c_str();
      exit(-1);
    }

    // Reading the header, which contains the version number, the size, and the
    // number of objects
    bool NeedByteSwap=false;
    {
    int header[5];
    if ( inputFileStream.read(reinterpret_cast<char *>(header),sizeof(int)*5).fail())
    {
      std::cout<<"Error: Could not read header of "<<filename.c_str()<<std::endl;
      exit(-1);
    }
    //Do byte swapping if necessary.
    if(header[0] == -1913442047|| header[0] ==1323699456 )    // Byte swapping needed (Number is byte swapped number of VERSION7 )
    {
      NeedByteSwap = true;
      //NOTE: All analyze object maps should be big endian on disk in order to be valid
      //      The following function calls will swap the bytes when on little endian machines.
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[0]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[1]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[2]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[3]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[4]));
    }
    // Reading the Header into the class
    this->SetVersion(header[0]);
    this->SetXDim(header[1]);
    this->SetYDim(header[2]);
    this->SetZDim(header[3]);
    this->SetNumberOfObjects(header[4]);

    if(this->GetVersion() != header[0])
    {
        std::cout<<"GetVersion() does not equal what was read in."<<std::endl;
        inputFileStream.close();
        return false;
    }
    if(this->GetXDim() != header[1])
    {
        std::cout<<"GetXDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
        return false;
    }
    if(this->GetYDim() != header[2])
    {
        std::cout<<"GetYDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
        return false;
    }
    if(this->GetZDim() != header[3])
    {
        std::cout<<"GetZDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
        return false;
    }
    if(this->GetNumberOfObjects() != header[4])
    {
        std::cout<<"GetNumberOfObjects() does not equal what was read in."<<std::endl;
        inputFileStream.close();
        return false;
    }
    }
    // In version 7, the header file has a new field after number of objects, before name,
    // which is nvols, with type int. This field allows 4D object maps. 
    // Further updating of objectmap related programs are to be developed to 
    // obtain, utilize this field. Xiujuan Geng May 04, 2007
    bool NeedBlendFactor = false;
    if( this->GetVersion() == VERSION7 )
    {
      int nvols = 1;
      if ( (inputFileStream.read(reinterpret_cast<char *>(&nvols),sizeof(int)*1)).fail() )
      {
        std::cout<<"Error: Could not read header of "<< filename.c_str()<<std::endl;
        exit(-1);
      }

      if(NeedByteSwap)
      {
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&nvols);
      }
      this->SetNumberOfVolumes(nvols);
      if(this->GetNumberOfVolumes() != nvols)
      {
          std::cout<<"GetNumberOfVolumes() does not equal what was read in."<<std::endl;
      }
      NeedBlendFactor = true;
    }

    const int VolumeSize=this->GetXDim() * this->GetYDim() * this->GetZDim() *this->GetNumberOfVolumes();

    // Error checking the number of objects in the object file
    if ((this->GetNumberOfObjects() < 1) || (this->GetNumberOfObjects() > 255))
    {
      std::cout<< "Error: Invalid number of object files.\n";
      inputFileStream.close();
      return false;
    }

    //std::ofstream myfile;
    //myfile.open("ReadFromFilePointer35.txt", myfile.app); 
    for (int i = 0; i < this->GetNumberOfObjects(); i++)
    {
      // Allocating a object to be created
      AnaylzeObjectEntryArray[i] = AnalyzeObjectEntry::New();
      AnaylzeObjectEntryArray[i]->ReadFromFilePointer(inputFileStream,NeedByteSwap, NeedBlendFactor);
        
      //AnaylzeObjectEntryArray[i]->Print(myfile);
    }
    //myfile.close();

    //Now the file pointer is pointing to the image region
    itk::Image<unsigned char,3>::SizeType ImageSize;
    ImageSize[0]=this->GetXDim();
    ImageSize[1]=this->GetYDim();
    ImageSize[2]=this->GetZDim();
    itk::Image<unsigned char,3>::IndexType ImageIndex;
    ImageIndex[0]=0;
    ImageIndex[1]=0;
    ImageIndex[2]=0;  

    itk::Image<unsigned char,3>::RegionType ImageRegion;
    ImageRegion.SetSize(ImageSize);
    ImageRegion.SetIndex(ImageIndex);
    this->SetRegions(ImageRegion);
    
    //TODO: Image spacing needs fixing.  Will need to look to see if a 
    //      .nii, .nii.gz, or a .hdr file
    //      exists for the same .obj file.
    //      If so, then read in the spacing for those images.
    itk::Image<unsigned char,3>::SpacingType ImageSpacing;
    ImageSpacing[0]=1.0F;
    ImageSpacing[1]=1.0F;
    ImageSpacing[2]=1.0F;
    this->SetSpacing(ImageSpacing);
    this->Allocate();

    // Decoding the run length encoded raw data into an unsigned char volume
    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexIt(this,this->GetLargestPossibleRegion());
    struct RunLengthStruct {
        unsigned char voxel_count;
        unsigned char voxel_value;
    } ;
    typedef struct RunLengthStruct RunLengthElement;
    RunLengthElement RunLengthArray[NumberOfRunLengthElementsPerRead];

    // The file consists of unsigned character pairs which represents the encoding of the data
    // The character pairs have the form of length, tag value.  Note also that the data in
    // Analyze object files are run length encoded a plane at a time.

    int index=0;
    int voxel_count_sum=0;
    {
      //std::ofstream myfile;
      //myfile.open("VoxelInformation15.txt", myfile.app);
      while (!inputFileStream.read(reinterpret_cast<char *>(RunLengthArray), sizeof(RunLengthElement)*NumberOfRunLengthElementsPerRead).eof())
      {
        for (int i = 0; i < NumberOfRunLengthElementsPerRead; i++)
        {
           //myfile<< "Assigning: " << (int)RunLengthArray[i].voxel_count 
           //  << " voxels of label " << (int)RunLengthArray[i].voxel_value
           //  << std::endl;
          if(RunLengthArray[i].voxel_count == 0)
          {
                std::cout<<"Invalid Length "<<(int)RunLengthArray[i].voxel_count<<std::endl;
                exit(-1);
          }
          for (int j = 0; j < RunLengthArray[i].voxel_count; j++)
          {

            indexIt.Set(RunLengthArray[i].voxel_value) ;
            ++indexIt;
            index++;
          }
          voxel_count_sum+=RunLengthArray[i].voxel_count;
          //myfile <<"index = "<<index
          //  << " voxel_count_sum= " << voxel_count_sum
          //  << " Volume size = "<<VolumeSize<<std::endl;
          if ( index > VolumeSize )
          {
            std::cout<<"BREAK!\n";
            exit(-1);
          }
        }
      }
      //myfile.close();
    }

    if (index != VolumeSize)
    {
      std::cout<< "Warning: Error decoding run-length encoding."<<std::endl;
      if(index < VolumeSize)
      {
        std::cout<<"Warning: file underrun."<<std::endl;
      }
      else
      {
        std::cout<<"Warning: file overrun."<<std::endl;
      }
      return false;
    }

    inputFileStream.close();

    return true;
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
