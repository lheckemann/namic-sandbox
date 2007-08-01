/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnalyzeObjectLabelMapImageIO.cxx,v $
Language:  C++
Date:      $Date: 2007/03/29 18:39:28 $
Version:   $Revision: 1.32 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for mwore information.

=========================================================================*/

#include "itkAnalyzeObjectLabelMapImageIO.h"
#include "itkIOCommon.h"
#include "itkExceptionObject.h"
#include "itkByteSwapper.h"
#include "itkMetaDataObject.h"
#include "itkSpatialOrientationAdapter.h"
#include <itksys/SystemTools.hxx>
#include <vnl/vnl_math.h>
#include "itk_zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace itk
{
  //Streaming not yet supported, so use the default base class to return the LargestPossibleRegion
#if _USE_STREAMABLE_REGION_FOR_AOLM
ImageIORegion AnalyzeObjectLabelMapImageIO
::GenerateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requestedRegion ) const
{
  std::cout << "AnalyzeObjectLabelMapImageIO::GenerateStreamableReadRegionFromRequestedRegion() " << std::endl;
  std::cout << "RequestedRegion = " << requestedRegion << std::endl;
  return requestedRegion;
}
#endif

AnalyzeObjectLabelMapImageIO::AnalyzeObjectLabelMapImageIO()
{
  //Nothing to do during initialization.
}

AnalyzeObjectLabelMapImageIO::~AnalyzeObjectLabelMapImageIO()
{
}

void AnalyzeObjectLabelMapImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

bool AnalyzeObjectLabelMapImageIO::CanWriteFile(const char * FileNameToWrite)
{
    // This assumes that the final '.' in a file name is the delimiter
    // for the file's extension type
  std::string filename = FileNameToWrite;
    const std::string::size_type it = filename.find_last_of( "." );
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if (fileExt != ".obj")
    {
      return false;
    }

    return true;
}


void AnalyzeObjectLabelMapImageIO::Read(void* buffer)
{  


  
  std::ifstream inputFileStream;
    inputFileStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
    if ( !inputFileStream.is_open())
    {
      std::cout<< "Error: Could not open %s\n", m_FileName.c_str();
      exit(-1);
    }
    //this->m_AnalyzeObjectLabelMapImage = itk::AnalyzeObjectMap::New();
    int version = this->m_AnalyzeObjectLabelMapImage->GetVersion();
    int setPointerOfInputImage;
    if(version == VERSION7)
    {
      std::cout<<this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects()<<std::endl;
      setPointerOfInputImage = 24 + this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() * 152;
    }
    else
    {
      setPointerOfInputImage = 20 + this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() * 146;
    }
    inputFileStream.seekg(setPointerOfInputImage);

  //Now the file pointer is pointing to the image region
    itk::Image<unsigned char,3>::SizeType ImageSize;
    ImageSize[0]=this->m_AnalyzeObjectLabelMapImage->GetXDim();
    ImageSize[1]=this->m_AnalyzeObjectLabelMapImage->GetYDim();
    ImageSize[2]=this->m_AnalyzeObjectLabelMapImage->GetZDim();
    itk::Image<unsigned char,3>::IndexType ImageIndex;
    ImageIndex[0]=0;
    ImageIndex[1]=0;
    ImageIndex[2]=0;  

    itk::Image<unsigned char,3>::RegionType ImageRegion;
    ImageRegion.SetSize(ImageSize);
    ImageRegion.SetIndex(ImageIndex);
    this->m_AnalyzeObjectLabelMapImage->SetRegions(ImageRegion);
    
    //TODO: Image spacing needs fixing.  Will need to look to see if a 
    //      .nii, .nii.gz, or a .hdr file
    //      exists for the same .obj file.
    //      If so, then read in the spacing for those images.
    itk::Image<unsigned char,3>::SpacingType ImageSpacing;
    ImageSpacing[0]=1.0F;
    ImageSpacing[1]=1.0F;
    ImageSpacing[2]=1.0F;
    this->m_AnalyzeObjectLabelMapImage->SetSpacing(ImageSpacing);
    this->m_AnalyzeObjectLabelMapImage->Allocate();

    // Decoding the run length encoded raw data into an unsigned char volume
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
    unsigned char *tobuf = (unsigned char *)buffer;

  //    ImageIORegion regionToRead = this->GetIORegion();
  //ImageIORegion::SizeType size = regionToRead.GetSize();
  //ImageIORegion::IndexType start = regionToRead.GetIndex();
  //const int DimensionSize = regionToRead.GetImageDimension();
  //int dimensions[4] = {1};
  //int startPoint[4] = {1};
  //for(int i = 0; i<4; i ++)
  //{
  //  dimensions[i] = regionToRead.GetSize(i);
  //  startPoint[i] = regionToRead.GetIndex(i);
  //}
  //  int xDim = 1;
  //  int yDim = 1;
  //  int zDim = 1;
  //  switch (regionToRead.GetImageDimension())
  //  {
  //    case 3:
  //      zDim = regionToRead.GetSize(2);
  //    case 2:
  //      yDim = regionToRead.GetSize(1);
  //    case 1:
  //      xDim = regionToRead.GetSize(0);
  //      break;
  //    default:
  //      break;
  //  }

    /*const int numberOfBytesToRead = 2 * startPoint[0] * startPoint[1] * startPoint[2];
        int *cornersOfRegion;
        cornersOfRegion = new int(numberOfBytesToRead);
        for(int i = 1; i <zDim+1; i++)
        {
          for(int j = 1; j<(2 * startPoint[0] * startPoint[1] * startPoint[2]); j++)
          {
            if((j-1)%2 != 2 && j!=1)
            {
              cornersOfRegion[j] = (startPoint[0] * startPoint[1] * startPoint[2] + xDim * (j/2) + startPoint[1] + (j-1)/2) * i;
            }
            else if(j%2 != 2)
            {
              cornersOfRegion[j] = (startPoint[0] * startPoint[1] * startPoint[2] + xDim * (j/2)) * i;
            }
            else
            {
              cornersOfRegion[0] = startPoint[0] * startPoint[1] * startPoint[2]; 
            }
          }
        }
        std::cout<<"Can not deal with the image size right now"<<std::endl;*/
    const int VolumeSize=this->m_AnalyzeObjectLabelMapImage->GetXDim() * this->m_AnalyzeObjectLabelMapImage->GetYDim() * this->m_AnalyzeObjectLabelMapImage->GetZDim() *this->m_AnalyzeObjectLabelMapImage->GetNumberOfVolumes();
    //const int VolumeSize = zDim * yDim *xDim;
    {
      std::ofstream myfile;
      myfile.open("VoxelInformation16.txt", myfile.app);
      int n= 0;
      while (!inputFileStream.read(reinterpret_cast<char *>(RunLengthArray), sizeof(RunLengthElement)*NumberOfRunLengthElementsPerRead).eof())
      {
        n++;
        for (int i = 0; i < NumberOfRunLengthElementsPerRead; i++)
        {
           myfile<< "Assigning: " << (int)RunLengthArray[i].voxel_count 
             << " voxels of label " << (int)RunLengthArray[i].voxel_value
             << std::endl;
          if(RunLengthArray[i].voxel_count == 0)
          {
                std::cout<<"Inside AnaylzeObjectLabelMap Invalid Length "<<(int)RunLengthArray[i].voxel_count<<std::endl;
                exit(-1);
          }
          for (int j = 0; j < RunLengthArray[i].voxel_count; j++)
          {

            //indexIt.Set(RunLengthArray[i].voxel_value) ;
            tobuf[index] = RunLengthArray[i].voxel_value;
            //++indexIt;
            index++;
          }
          voxel_count_sum+=RunLengthArray[i].voxel_count;
          myfile <<"index = "<<index
            << " voxel_count_sum= " << voxel_count_sum
            << " Volume size = "<<VolumeSize<<std::endl;
          if ( index > VolumeSize )
          {
            std::cout<<"BREAK!\n";
            exit(-1);
          }
        }
      }
      myfile.close();
      n++;
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
      //return false;
    }

    //cornersOfRegion
    inputFileStream.close();

    std::ofstream check;
    check.open("CheckBuffer.txt");
    for(int i=0; i<VolumeSize;i++)
    {
      check<<(int)tobuf[i]<<std::endl;
    }
    check.close();
}


// This method will only test if the header looks like an
// Nifti Header.  Some code is redundant with ReadImageInformation
// a StateMachine could provide a better implementation
bool AnalyzeObjectLabelMapImageIO::CanReadFile( const char* FileNameToRead )
{
  std::cout<<"I am in Can Read File for AnalyzeObjectLabelMapImageIO"<<std::endl;
  std::string filename = FileNameToRead;
    // This assumes that the final '.' in a file name is the delimiter
    // for the file's extension type
    const std::string::size_type it = filename.find_last_of( "." );
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if (fileExt != ".obj")
    {
      return false;
    }

    return true;
}

                              
    
void AnalyzeObjectLabelMapImageIO::ReadImageInformation()
{
  m_ComponentType = CHAR;
  m_PixelType = SCALAR;
  // Opening the file
    std::ifstream inputFileStream;
    inputFileStream.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
    if ( !inputFileStream.is_open())
    {
      std::cout<< "Error: Could not open %s\n", m_FileName.c_str();
      exit(-1);
    }

    // Reading the header, which contains the version number, the size, and the
    // number of objects
    bool NeedByteSwap=false;
    
    int header[5];
    if ( inputFileStream.read(reinterpret_cast<char *>(header),sizeof(int)*5).fail())
    {
      std::cout<<"Error: Could not read header of "<<m_FileName.c_str()<<std::endl;
      std::cout<<header[0]<<std::endl;
      std::cout<<header[1]<<std::endl;
      std::cout<<header[2]<<std::endl;
      std::cout<<header[3]<<std::endl;
      std::cout<<header[4]<<std::endl;
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

    this->m_AnalyzeObjectLabelMapImage = itk::AnalyzeObjectMap::New();
    // Reading the Header into the class
    this->m_AnalyzeObjectLabelMapImage->SetVersion(header[0]);
    this->m_AnalyzeObjectLabelMapImage->SetXDim(header[1]);
    this->m_AnalyzeObjectLabelMapImage->SetYDim(header[2]);
    this->m_AnalyzeObjectLabelMapImage->SetZDim(header[3]);
    this->m_AnalyzeObjectLabelMapImage->SetNumberOfObjects(header[4]);

    // In version 7, the header file has a new field after number of objects, before name,
    // which is nvols, with type int. This field allows 4D object maps. 
    // Further updating of objectmap related programs are to be developed to 
    // obtain, utilize this field. Xiujuan Geng May 04, 2007
    bool NeedBlendFactor = false;
    if( this->m_AnalyzeObjectLabelMapImage->GetVersion() == VERSION7 )
    {
      int nvols = 1;
      if ( (inputFileStream.read(reinterpret_cast<char *>(&nvols),sizeof(int)*1)).fail() )
      {
        std::cout<<"Error: Could not read header of "<< m_FileName.c_str()<<std::endl;
        exit(-1);
      }

      if(NeedByteSwap)
      {
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&nvols);
      }
      this->m_AnalyzeObjectLabelMapImage->SetNumberOfVolumes(nvols);
      if(this->m_AnalyzeObjectLabelMapImage->GetNumberOfVolumes() != nvols)
      {
          std::cout<<"GetNumberOfVolumes() does not equal what was read in."<<std::endl;
      }
      NeedBlendFactor = true;
    }
    
    if(this->m_AnalyzeObjectLabelMapImage->GetNumberOfVolumes() >1 )
    {
      this->SetNumberOfDimensions(4);
    }
    else if(this->m_AnalyzeObjectLabelMapImage->GetZDim() >1 )
    {
      this->SetNumberOfDimensions(3);
    }
    else if(this->m_AnalyzeObjectLabelMapImage->GetYDim() >1 )
    {
      this->SetNumberOfDimensions(2);
    }
    else if(this->m_AnalyzeObjectLabelMapImage->GetXDim() >1 )
    {
      this->SetNumberOfDimensions(1);
    }

    switch(this->GetNumberOfDimensions())
    {
    case 4:
      this->SetDimensions(3,this->m_AnalyzeObjectLabelMapImage->GetNumberOfVolumes());
      this->SetSpacing(3, 1);
    case 3:
      this->SetDimensions(2,this->m_AnalyzeObjectLabelMapImage->GetZDim());
      this->SetSpacing(2, 1);
    case 2:
      this->SetDimensions(1,this->m_AnalyzeObjectLabelMapImage->GetYDim());
      this->SetSpacing(1,1);
    case 1:
      this->SetDimensions(0, this->m_AnalyzeObjectLabelMapImage->GetXDim());
      this->SetSpacing(0,1);
    }

    m_Origin[0] = m_Origin[1] = 0;
    if(this->GetNumberOfDimensions() > 2)
      {
      m_Origin[2] = 0;
      }
    std::vector<double> dirx(this->GetNumberOfDimensions(),0), diry(this->GetNumberOfDimensions(),0), dirz(this->GetNumberOfDimensions(),0);
  dirx[0] = 1; dirx[1] = 0; dirx[2] = 0;
  diry[0] = 0; diry[1] = 1; diry[2] = 0;
  dirz[0] = 0; dirz[1] = 0; dirz[2] = 1;

  this->SetDirection(0,dirx);
  this->SetDirection(1,diry);
  if(this->GetNumberOfDimensions() > 2)
    {
    this->SetDirection(2,dirz);
    }
    if(this->m_AnalyzeObjectLabelMapImage->GetVersion() != header[0])
    {
        std::cout<<"GetVersion() does not equal what was read in."<<std::endl;
        inputFileStream.close();
    }
    if(this->m_AnalyzeObjectLabelMapImage->GetXDim() != header[1])
    {
        std::cout<<"GetXDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
    }
    if(this->m_AnalyzeObjectLabelMapImage->GetYDim() != header[2])
    {
        std::cout<<"GetYDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
     }
    if(this->m_AnalyzeObjectLabelMapImage->GetZDim() != header[3])
    {
        std::cout<<"GetZDim() does not equal what was read in."<<std::endl;
        inputFileStream.close();
    }
    if(this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() != header[4])
    {
        std::cout<<"GetNumberOfObjects() does not equal what was read in."<<std::endl;
        inputFileStream.close();
    }
    

    // Error checking the number of objects in the object file
    if ((this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() < 1) || (this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() > 255))
    {
      std::cout<< "Error: Invalid number of object files.\n";
      inputFileStream.close();
    }

    //std::ofstream myfile;
    //myfile.open("ReadFromFilePointer35.txt", myfile.app);
    itk::AnalyzeObjectEntryArrayType *my_reference=this->m_AnalyzeObjectLabelMapImage->GetAnalyzeObjectEntryArrayPointer();
    (*my_reference).resize(this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects());
    for (int i = 0; i < this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects(); i++)
    {
      // Allocating a object to be created
      (*my_reference)[i] = AnalyzeObjectEntry::New();
      (*my_reference)[i]->ReadFromFilePointer(inputFileStream,NeedByteSwap, NeedBlendFactor);
    }
    //myfile.close();
    inputFileStream.close();
    //Now fill out the MetaDataHeader
    MetaDataDictionary &thisDic=this->GetMetaDataDictionary();
    EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,std::string(typeid(unsigned char).name()));
    EncapsulateMetaData<itk::AnalyzeObjectEntryArrayType>(thisDic,ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY,*my_reference);
}

/**
 *
 */
void
AnalyzeObjectLabelMapImageIO
::WriteImageInformation(void) //For Nifti this does not write a file, it only fills in the appropriate header information.
{
  std::cout<<"I am in the writeimageinformaton"<<std::endl;
  std::string tempfilename = this->GetFileName();
  // Opening the file
    std::ofstream outputFileStream;
    outputFileStream.open(tempfilename.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if ( !outputFileStream.is_open())
    {
      std::cout<<"Error: Could not open "<< tempfilename.c_str()<<std::endl;
      exit(-1);
    }
    itk::AnalyzeObjectEntryArrayType my_reference;
    itk::ExposeMetaData<itk::AnalyzeObjectEntryArrayType>(this->GetMetaDataDictionary(),ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY, my_reference);
    int header[6];
    header[0]=VERSION7;
    header[1]=this->GetDimensions(0);
    header[2]=this->GetDimensions(1);
    header[3]=this->GetDimensions(2);
    header[4]=my_reference.size();
    header[5]=1;

    //All object maps are written in BigEndian format as required by the AnalyzeObjectMap documentation.
    bool NeedByteSwap=itk::ByteSwapper<int>::SystemIsLittleEndian();
    if(NeedByteSwap)
    {
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[0]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[1]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[2]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[3]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[4]));
      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[5]));
    }

    // Writing the header, which contains the version number, the size, and the
    // number of objects
    if(outputFileStream.write(reinterpret_cast<char *>(header), sizeof(int)*6).fail())
    {
      std::cout<<"Error: Could not write header of "<< tempfilename.c_str()<<std::endl;
      exit(-1);
    }

    // Error checking the number of objects in the object file
    if ((my_reference.size() < 0) || (my_reference.size() > 255))
    {
      std::cout<<( stderr, "Error: Invalid number of object files.\n" );
      outputFileStream.close();
    }
  

  // Since the NumberOfObjects does not reflect the background, the background will be included
    for (int i = 0; i < my_reference.size(); i++)
    {
      // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
      // not affect the current object itself
      AnalyzeObjectEntry *ObjectWrite = my_reference[i];
      if (NeedByteSwap == true)
      {
        ObjectWrite->SwapObjectEndedness();
      }
      ObjectWrite->Write(outputFileStream); 

    }

  outputFileStream.close();
}


/**
 *
 */
void
AnalyzeObjectLabelMapImageIO

::Write( const void* buffer)
{
  this->WriteImageInformation();
  std::string tempfilename = this->GetFileName();
  // Opening the file
    std::ofstream outputFileStream;
    outputFileStream.open(tempfilename.c_str(), std::ios::binary | std::ios::out | std::ios::app);
    if ( !outputFileStream.is_open())
    {
      std::cout<<"Error: Could not open "<< tempfilename.c_str()<<std::endl;
      exit(-1);
    }
    itk::AnalyzeObjectEntryArrayType my_reference;
    itk::ExposeMetaData<itk::AnalyzeObjectEntryArrayType>(this->GetMetaDataDictionary(),ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY, my_reference);
    //int setPointerOfInputImage = 24 + (my_reference.size())*152;//this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() * 152;;
    
    //outputFileStream.seekp(1550);
    

  // Encoding the run length encoded raw data into an unsigned char volume
    const int VolumeSize=this->GetDimensions(0)*this->GetDimensions(1)*this->GetDimensions(2);
    const int PlaneSize = this->GetDimensions(0)*this->GetDimensions(1);
    int bufferindex=0;
    int planeindex=0;
    int runlength=0;
    unsigned char CurrentObjIndex=0;
    const  int buffer_size=16384;      //NOTE: This is probably overkill, but it will work
    unsigned char bufferObjectMap[buffer_size];

    unsigned char *bufferChar = (unsigned char *)buffer;

    for(int i=0;i<VolumeSize;i++)
    {
      if (runlength==0)
      {
          CurrentObjIndex = bufferChar[i];
          runlength=1;
      }
      else
      {
        if (CurrentObjIndex==bufferChar[i])
        {
          runlength++;
          if (runlength==255)
          {
            bufferObjectMap[bufferindex]=runlength;
            bufferObjectMap[bufferindex+1]=CurrentObjIndex;
            bufferindex+=2;
            runlength=0;
          }
        }
        else
        {
          bufferObjectMap[bufferindex]=runlength;
          bufferObjectMap[bufferindex+1]=CurrentObjIndex;
          bufferindex+=2;
          CurrentObjIndex=bufferChar[i];
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
          bufferObjectMap[bufferindex]=runlength;
          bufferObjectMap[bufferindex+1]=CurrentObjIndex;
          bufferindex+=2;
          runlength=0;
        }
      }
      if (bufferindex==buffer_size)
      {
        // buffer full
        if (outputFileStream.write(reinterpret_cast<char *>(bufferObjectMap), buffer_size).fail())
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
        bufferObjectMap[bufferindex]=runlength;
        bufferObjectMap[bufferindex+1]=CurrentObjIndex;
        bufferindex+=2;
      }
      if(outputFileStream.write(reinterpret_cast<char *>(bufferObjectMap), bufferindex).fail())
      {
        std::cout<<"error: could not write buffer"<<std::endl;
        exit(-1);
      }
    }
}

} // end namespace itk
