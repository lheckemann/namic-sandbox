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

ImageIORegion AnalyzeObjectLabelMapImageIO
::CalculateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requestedRegion ) const
{
  std::cout << "AnalyzeObjectLabelMapImageIO::CalculateStreamableReadRegionFromRequestedRegion() " << std::endl;
  std::cout << "RequestedRegion = " << requestedRegion << std::endl;
  return requestedRegion;
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
    int version = this->m_AnalyzeObjectLabelMapImage->GetVersion();
    int setPointerOfInputImage;
    if(version == VERSION7)
    {
      setPointerOfInputImage = 24 + this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() * 150;
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
    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexIt(this->m_AnalyzeObjectLabelMapImage,this->m_AnalyzeObjectLabelMapImage->GetLargestPossibleRegion());
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
    char *tobuf = (char *)buffer;
    const int VolumeSize=this->m_AnalyzeObjectLabelMapImage->GetXDim() * this->m_AnalyzeObjectLabelMapImage->GetYDim() * this->m_AnalyzeObjectLabelMapImage->GetZDim() *this->m_AnalyzeObjectLabelMapImage->GetNumberOfVolumes();
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
            tobuf[(i+1) * j+i] = RunLengthArray[i].voxel_value;
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
      //return false;
    }

    inputFileStream.close();
}


// This method will only test if the header looks like an
// Nifti Header.  Some code is redundant with ReadImageInformation
// a StateMachine could provide a better implementation
bool AnalyzeObjectLabelMapImageIO::CanReadFile( const char* FileNameToRead )
{
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
    {
    int header[5];
    if ( inputFileStream.read(reinterpret_cast<char *>(header),sizeof(int)*5).fail())
    {
      std::cout<<"Error: Could not read header of "<<m_FileName.c_str()<<std::endl;
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
    this->m_AnalyzeObjectLabelMapImage->SetVersion(header[0]);
    this->m_AnalyzeObjectLabelMapImage->SetXDim(header[1]);
    this->m_AnalyzeObjectLabelMapImage->SetYDim(header[2]);
    this->m_AnalyzeObjectLabelMapImage->SetZDim(header[3]);
    this->m_AnalyzeObjectLabelMapImage->SetNumberOfObjects(header[4]);

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
    }
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

    // Error checking the number of objects in the object file
    if ((this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() < 1) || (this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects() > 255))
    {
      std::cout<< "Error: Invalid number of object files.\n";
      inputFileStream.close();
    }

    //std::ofstream myfile;
    //myfile.open("ReadFromFilePointer35.txt", myfile.app); 
    for (int i = 0; i < this->m_AnalyzeObjectLabelMapImage->GetNumberOfObjects(); i++)
    {
      // Allocating a object to be created
      AnaylzeObjectEntryArray[i] = AnalyzeObjectEntry::New();
      AnaylzeObjectEntryArray[i]->ReadFromFilePointer(inputFileStream,NeedByteSwap, NeedBlendFactor);
        
      //AnaylzeObjectEntryArray[i]->Print(myfile);
    }
    //myfile.close();

    inputFileStream.close();
}

/**
 *
 */
void
AnalyzeObjectLabelMapImageIO
::WriteImageInformation(void) //For Nifti this does not write a file, it only fills in the appropriate header information.
{
}


/**
 *
 */
void
AnalyzeObjectLabelMapImageIO

::Write( const void* buffer)
{
}

} // end namespace itk
