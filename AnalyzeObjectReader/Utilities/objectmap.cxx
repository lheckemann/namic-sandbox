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

    
    AnalyzeObjectMap::AnalyzeObjectMap( void ): Version(VERSION7),NumberOfObjects(0),NeedsSaving(0), NeedsRegionsCalculated(0)
{
  //TODO:  Clear the image this->ImageClear();
  {
    for (int i = 0; i < 256; i++)
    {
      AnaylzeObjectEntryArray[i] = NULL;
      ShowObject[i] = 0;
      MinimumPixelValue[i] = 0;
      MaximumPixelValue[i] = 0;
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


//AnalyzeObjectMap::AnalyzeObjectMap( const int _iX, const int _iY, const int _iZ )
//:Version(VERSION7),
//NumberOfObjects(0),NeedsSaving(0), NeedsRegionsCalculated(0)
//{
//  //TODO:  Clear this this->ImageClear();
//  {
//    for (int i = 0; i < 256; i++)
//    {
//      AnaylzeObjectEntryArray[i] = NULL;
//      ShowObject[i] = 0;
//      MinimumPixelValue[i] = 0;
//      MaximumPixelValue[i] = 0;
//    }
//  }
//
//  // Setting object zero as the background
//  AnaylzeObjectEntryArray[0] = AnalyzeObjectEntry::New();
//  this->getObjectEntry(0)->SetName("Background");
//  this->getObjectEntry(0)->SetDisplayFlag(0);
//  this->getObjectEntry(0)->SetOpacity(0);
//  this->getObjectEntry(0)->SetOpacityThickness(0);
//  this->getObjectEntry(0)->SetEndRed(0);
//  this->getObjectEntry(0)->SetEndGreen(0);
//  this->getObjectEntry(0)->SetEndBlue(0);
//  this->getObjectEntry(0)->SetShades(1);
//}


AnalyzeObjectMap::AnalyzeObjectMap( const AnalyzeObjectMap & rhs )
{
  this->CopyBaseImage( rhs );
  Version=rhs.Version;
  NumberOfObjects=rhs.NumberOfObjects;
  NeedsSaving=rhs.NeedsSaving;
  NeedsRegionsCalculated=rhs.NeedsRegionsCalculated;
  for(int i=0; i < 256; i++)
  {
    if((rhs.AnaylzeObjectEntryArray[i]).IsNotNull())
    {
        AnaylzeObjectEntryArray[i] = AnalyzeObjectEntry::New();
        AnaylzeObjectEntryArray[i]->Copy(rhs.AnaylzeObjectEntryArray[i]);
    }
    else
    {
      AnaylzeObjectEntryArray[i] = NULL;
    }
    ShowObject[i]=rhs.ShowObject[i];
    MinimumPixelValue[i]=rhs.MinimumPixelValue[i];
    MaximumPixelValue[i]=rhs.MaximumPixelValue[i];
  }
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


bool AnalyzeObjectMap::CopyBaseImage( const AnalyzeObjectMap& rhs )
{
  // This is the summary of what the CImage's do for their copying
#if 0
  //TODO: Initialzie the image size and copy the Lablemap
  this->ImageReinitialize(
    rhs.getXDim(), rhs.getXVox(),
    rhs.getYDim(), rhs.getYVox(),
    rhs.getZDim(), rhs.getZVox(),
    rhs.getTDim(), rhs.getTVox());
  ::memcpy( static_cast<void *>( this->getDataPtr() ), static_cast<void *>(rhs.getDataPtr()) ,
    sizeof(unsigned char)*rhs.getTotalVox() );
  // End of hacked copy method
#endif
  return true;
}


AnalyzeObjectMap &  AnalyzeObjectMap::operator=( const AnalyzeObjectMap & rhs )
{

  this->CopyBaseImage( rhs );

  Version=rhs.Version;
  NumberOfObjects=rhs.NumberOfObjects;
  NeedsSaving=rhs.NeedsSaving;
  NeedsRegionsCalculated=rhs.NeedsRegionsCalculated;
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
    ShowObject[i]=rhs.ShowObject[i];
    MinimumPixelValue[i]=rhs.MinimumPixelValue[i];
    MaximumPixelValue[i]=rhs.MaximumPixelValue[i];
  }
  return *this;
}

int AnalyzeObjectMap::getObjectIndex( const std::string &ObjectName  )
{
  for(int index=0; index<=this->getNumberOfObjects(); index++)
  {
    if(ObjectName == this->getObjectEntry(index)->GetName() )
    {
      return index;
    }
  }
  //Failure if not found.
  return -1;
}


AnalyzeObjectEntry::Pointer AnalyzeObjectMap::getObjectEntry( const int index )
{
    return AnaylzeObjectEntryArray[index];
}

const AnalyzeObjectEntry::Pointer  AnalyzeObjectMap::getObjectEntry( const int index ) const
{
  return AnaylzeObjectEntryArray[index];
}


void AnalyzeObjectMap::ReinitializeObjectMap(const int _iX, const int _iY, const int _iZ)
{
//TODO  this->ImageReinitialize(_iX, _iY, _iZ);
//TODO  this->ImageClear();
  {
    for(int i=0; i < 256; i++)
    {
      if(AnaylzeObjectEntryArray[i].IsNotNull())
      {
        AnaylzeObjectEntryArray[i]  =  NULL;
      }
    }
  }

  {
    for (int i = 0; i < 256; i++)
    {
      AnaylzeObjectEntryArray[i] = NULL;
      ShowObject[i] = 0;
      MinimumPixelValue[i] = 0;
      MaximumPixelValue[i] = 0;
    }
  }

  Version = VERSION7;
  NumberOfObjects = 0;
  NeedsSaving = 0;
  NeedsRegionsCalculated = 0;

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





bool AnalyzeObjectMap::ReadObjectFile( const std::string& filename )
{
  // Opening the file
  std::ifstream inputFileStream;
  inputFileStream.open(filename.c_str(), std::ios::binary | std::ios::in);
  if ( !inputFileStream.is_open())
  {
    ::fprintf( stderr, "Error: Could not open %s\n", filename.c_str());
    exit(-1);
  }


  // Reading the header, which contains the version number, the size, and the
  // number of objects
  int header[5];
  if ( inputFileStream.read(reinterpret_cast<char *>(header),sizeof(int)*1).fail())
  {
      std::cout<<"Error: Could not read header of "<<filename.c_str()<<std::endl;
    exit(-1);
  }
  bool NeedByteSwap=false;
  std::cout<<header[0]<<std::endl;
  //Do byte swapping if necessary.
    if(header[0] == -1913442047|| header[0] ==33333333 )    // Byte swapping needed (Number is byte swapped number of VERSIONy or VERSION8 )
    {
        if(itk::ByteSwapper<int>::SystemIsLittleEndian())
        {
            itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[0]));
        }
        NeedByteSwap = true;
    }

    if(inputFileStream.readsome(reinterpret_cast<char *>(&(header[1])),sizeof(int)*4) != sizeof(int)*4)
    {
        std::cout<<"Could not read in the other header information"<<std::endl;
        exit(-1);
      
    }

    if(NeedByteSwap)
    {
        if(itk::ByteSwapper<int>::SystemIsLittleEndian())
        {
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[1]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[2]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[3]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[4]));
          //itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[5]));
        }
    }

    // Reading the Header into the class
    this->Version = header[0];
    this->SetXDim(header[1]);
    this->SetYDim(header[2]);
    this->SetZDim(header[3]);
    this->NumberOfObjects = header[4];
    
    




  // In version 7, the header file has a new field after number of objects, before name,
  // which is nvols, with type int. This field allows 4D object maps. 
  // Further updating of objectmap related programs are to be developed to 
  // obtain, utilize this field. Xiujuan Geng May 04, 2007
  int nvols[1] = {1};
  bool NeedBlendFactor = false;
  if( Version == VERSION7 )
  {
    
    if ( (inputFileStream.read(reinterpret_cast<char *>(nvols),sizeof(int)*1)).fail() )
    {
        std::cout<<"Error: Could not read header of "<< filename.c_str()<<std::endl;
      exit(-1);
    }
    if(NeedByteSwap)
    {
        if(itk::ByteSwapper<int>::SystemIsLittleEndian())
        {
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(nvols[0]));
        }
    }
    NeedBlendFactor = true;

  }

  const int VolumeSize=this->GetXDim() * this->GetYDim() * this->GetZDim() *nvols[0];


  // Error checking the number of objects in the object file
  if ((NumberOfObjects < 1) || (NumberOfObjects > 255))
  {
    ::fprintf( stderr, "Error: Invalid number of object files.\n" );
    inputFileStream.close();
    return false;
  }

  std::ofstream myfile;
  myfile.open("ReadFromFilePointer29.txt", myfile.app); 
  for (int i = 0; i < NumberOfObjects; i++)
  {
    // Allocating a object to be created
    AnaylzeObjectEntryArray[i] = AnalyzeObjectEntry::New();
    AnaylzeObjectEntryArray[i]->ReadFromFilePointer(inputFileStream,NeedByteSwap, NeedBlendFactor);
      
    AnaylzeObjectEntryArray[i]->Print(myfile);
  }
  myfile.close();


 //TODO:  Now the file pointer is pointing to the image region
  // Creating the image volume
  //Set size of the image


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
  
  //TODO: Image spacing needs fixing
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
        std::ofstream myfile;
  myfile.open("VoxelInformation9.txt", myfile.app);
     while (!inputFileStream.read(reinterpret_cast<char *>(RunLengthArray), sizeof(RunLengthElement)*NumberOfRunLengthElementsPerRead).eof())
    {
      for (int i = 0; i < NumberOfRunLengthElementsPerRead; i++)
      {
         myfile<< "Assigning: " << (int)RunLengthArray[i].voxel_count 
             << " voxels of label " << (int)RunLengthArray[i].voxel_value
             << std::endl;
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
  }



  if (index != VolumeSize)
  {
    ::fprintf(stderr, "Warning: Error decoding run-length encoding.  \n");
    if(index < VolumeSize)
    {
        std::cout<<index<<"\n";
        std::cout<<VolumeSize<<"\n";
      ::fprintf(stderr, "Warning: file underrun.  \n");
    }
    else
    {
      ::fprintf(stderr, "Warning: file overrun.  \n");
    }
    return false;
  }

  inputFileStream.close();

  return true;
}


bool AnalyzeObjectMap::WriteObjectFile( const std::string& filename )
{
  
  
  // This assumes that the final '.' in a file name is the delimiter
  // for the file's extension type
  const std::string::size_type it = filename.find_last_of( "." );

  std::string tempfilename=filename;

  //Note: if there is no extension, then it=basic_string::npos;
  if (it > filename.length())
  {

    tempfilename+=".obj";
  }
  else
  {
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if (fileExt != ".obj")
    {
      tempfilename+=".obj";
    }
  }



    // Opening the file
  std::fstream inputFileStream;
  inputFileStream.open(tempfilename.c_str(), std::ios::binary | std::ios::in);
  if ( !inputFileStream.is_open())
  {
      std::cout<<"Error: Could not open "<< filename.c_str()<<std::endl;
    exit(-1);
  }

  bool NeedByteSwap=false;


  int header[5];
  header[0]=Version;
  header[1]=this->GetXDim();
  header[2]=this->GetYDim();
  header[3]=this->GetZDim();
  header[4]=this->NumberOfObjects + 1;     // Include the background object when writing the .obj file

  

  if(itk::ByteSwapper<int>::SystemIsLittleEndian())
    {
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[1]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[2]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[3]));
          itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(header[4]));
    }


  // Writing the header, which contains the version number, the size, and the
  // number of objects
    if(inputFileStream.write(reinterpret_cast<char *>(header), sizeof(int)*5).fail())
    {
        std::cout<<"Error: Could not write header of "<< filename.c_str()<<std::endl;
        exit(-1);
    }
//
//  if( Version == VERSION7 )
//  {
//    // Set number of volumes to be 1. Need be changed later on. Xiujuan Geng, May 04, 2007
//    int nvols[1];    
//    nvols[0] = 1; 
//    if( NeedByteSwap )
//    {
//      itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(nvols[0]));
//    }
//    inputFileStream.write(reinterpret_cast<char *>(nvols), sizeof(int));    
//  }
//
//  // Error checking the number of objects in the object file
//  if ((NumberOfObjects < 0) || (NumberOfObjects > 255))
//  {
//      std::cout<<( stderr, "Error: Invalid number of object files.\n" );
//    inputFileStream.close();
//    return false;
//  }
//
//  // Since the NumberOfObjects does not reflect the background, the background will be included
//  for (int i = 0; i < NumberOfObjects + 1; i++)
//  {
//    // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
//    // not affect the current object itself
//      AnalyzeObjectEntry *ObjectWrite = this->getObjectEntry(i);
//
//    if (NeedByteSwap == true)
//    {
//      ObjectWrite->SwapObjectEndedness();
//    }
//
//   ObjectWrite->Write(inputFileStream); 
//
//    
//    
//#if 0
//    // Writing the ObjectEntry structures
//    if (::fwrite(ObjectWrite.getObjectPointer(), sizeof(Object), 1, fptr) != 1)
//    {
//      ::fprintf(stderr, "13: Unable to write in object #%d description of %s\n", i, filename.c_str());
//      exit(-1);
//    }
//#endif
//  }
//  RunLengthEncodeImage(*this, inputFileStream);
  inputFileStream.close();
  return true;
}


#if 0
bool AnalyzeObjectMap::AddObjectInRange(const CImage<float> & InputImage,
const float MinRange, const float MaxRange,
std::string ObjectName, const int EndRed,
const int EndGreen, const int EndBlue,
const int Shades, bool OverWriteObjectFlag)
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }
  this->NumberOfObjects++;
  this->AnaylzeObjectEntryArray[NumberOfObjects]= AnalyzeObjectEntry::New();
  AnaylzeObjectEntryArray[NumberOfObjects]->SetName(ObjectName);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetEndColor(EndRed,EndGreen,EndBlue);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetStartColor(static_cast<int>(0.1F*EndRed),
    static_cast<int>(0.1F*EndGreen),
    static_cast<int>(0.1F*EndBlue));
  AnaylzeObjectEntryArray[NumberOfObjects]->SetShades(Shades);

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j< ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const float CurrentValue=InputImage(i,j,k);

        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          // Check to see if there is another object here (not background)
          if (this->Pixel(i,j,k) > 0)
          {
            // This pixel has already been assigned an object
            if (OverWriteObjectFlag == true)
              this->Pixel(i,j,k)=NumberOfObjects;
            // else do nothing
          }
          else
          {
            // Must be background
            this->Pixel(i,j,k)=NumberOfObjects;
          }
        }
      }
    }
  }
  return true;
}

bool AnalyzeObjectMap::AddObjectInRange(const CImage<unsigned char> & InputImage,
const int MinRange, const int MaxRange,
std::string ObjectName, const int EndRed, const int EndGreen,
const int EndBlue, const int Shades, bool OverWriteObjectFlag)
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }
  this->NumberOfObjects++;
  this->AnaylzeObjectEntryArray[NumberOfObjects]= AnalyzeObjectEntry::New();
  AnaylzeObjectEntryArray[NumberOfObjects]->SetName(ObjectName);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetEndColor(EndRed,EndGreen,EndBlue);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetStartColor(static_cast<int>(0.1F*EndRed),
    static_cast<int>(0.1F*EndGreen),
    static_cast<int>(0.1F*EndBlue));
  AnaylzeObjectEntryArray[NumberOfObjects]->SetShades(Shades);

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const int CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) > 0)
          {
            // This pixel has already been assigned an object
            if (OverWriteObjectFlag == true)
              this->Pixel(i,j,k)=NumberOfObjects;
            // else do nothing
          }
          else
          {
            // Must be background
            this->Pixel(i,j,k)=NumberOfObjects;
          }
        }
      }
    }
  }
  return true;
}
#endif


bool AnalyzeObjectMap::RemoveObjectByName(const std::string & ObjectName)
{
  if (ObjectName == std::string("Background"))
  {
    // Can not delete background object
    ::fprintf(stderr, "Can not remove the background object.\n");
    return false;
  }

  int objectTag = 0;
  {
    for (int i = 1; i <= this->getNumberOfObjects(); i++)
    {
      if (ObjectName == this->getObjectEntry(i)->GetName())
      {
        objectTag = i;
        break;
      }
    }
  }

  if (objectTag == 0)
  {
    // Object name not found
    printf("Object name %s was not found.\n", ObjectName.c_str());
    return false;
  }

  // Go through all the object headers and shift them down
  {
    for (int j = objectTag; j < this->getNumberOfObjects(); j++)
    {
      this->getObjectEntry(j) = this->getObjectEntry(j+1);
    }
  }

  // Deleting the last extra data structure
  delete AnaylzeObjectEntryArray[this->getNumberOfObjects()];
  AnaylzeObjectEntryArray[this->getNumberOfObjects()] = NULL;

  #if 0
  // Changing the image object identifiers
  {

    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if (this->ConstPixel(i) == objectTag)
      {
        // Assign to the background
        this->Pixel(i) = 0;
      }
      else if (this->ConstPixel(i) > objectTag)
      {
        this->Pixel(i) = this->Pixel(i) - 1;
      }

    }
  }

  NumberOfObjects = NumberOfObjects - 1;
  #endif

  return true;
}


bool AnalyzeObjectMap::RemoveObjectByRange(const unsigned char MinRange, const unsigned char MaxRange)
{
  int startObjectTag = MinRange;
  int endObjectTag = MaxRange;

  // Validating ranges
  if (MinRange == 0)
  {
    // Do not delete background object, but continue with the rest of the objects in range
    printf("Can not delete the background object.\n");
    startObjectTag = 1;
  }

  if (MaxRange > this->getNumberOfObjects())
  {
    // Set the end to the number of objects
    printf("Max Range too large.  Setting it too maximum number of objects in the objectmap.\n");
    endObjectTag = this->getNumberOfObjects();
  }

  if (MinRange > MaxRange)
  {
    // Parameters are backwards
    ::fprintf(stderr, "Minimum Range %d and Maximum Range %d for removing objects are invalid.\n",
      MinRange, MaxRange);
    return false;
  }

  int NumberToDelete = endObjectTag - startObjectTag + 1;

  // Go through all the object headers and shift them down
  {
    for (int j = endObjectTag + 1; j <= this->getNumberOfObjects(); j++)
    {
      this->getObjectEntry(j-NumberToDelete) = this->getObjectEntry(j);
    }
  }

  // Deleting the last extra data structure
  {
    for (int i = this->getNumberOfObjects(); i > this->getNumberOfObjects() - NumberToDelete; i--)
    {
      delete AnaylzeObjectEntryArray[i];
      AnaylzeObjectEntryArray[i] = NULL;
    }
  }

#if 0
  // Changing the image object identifiers
  {
    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if ((this->ConstPixel(i) >= startObjectTag) && (this->ConstPixel(i) <= endObjectTag))
      {
        // Assign to the background
        this->Pixel(i) = 0;
      }
      else if (this->ConstPixel(i) > endObjectTag)
      {
        this->Pixel(i) = this->Pixel(i) - NumberToDelete;
      }
    }
  }

  NumberOfObjects = NumberOfObjects - NumberToDelete;
#endif
  return true;
}

#if 0
bool AnalyzeObjectMap::CheckObjectOverlap( const CImage<float> & InputImage, const float MinRange, const float MaxRange, const int MinObjectRange, const int MaxObjectRange )
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }

  // Error checking
  if ( MinObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MinObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MaxObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < MinObjectRange )
  {
    fprintf( stderr, "Error. MaxObjectRange can not be less than MinObjectRange!\n" );
    exit(-1);
  }

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const float CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) >= MinObjectRange && this->Pixel(i,j,k) <= MaxObjectRange)
          {
            // This pixel has already been assigned an object in the defined range, overlap occuring
            return true;
          }
        }
      }
    }
  }
  return false;
}


bool AnalyzeObjectMap::CheckObjectOverlap( const CImage<unsigned char> & InputImage, const int MinRange, const int MaxRange, const int MinObjectRange, const int MaxObjectRange )
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }

  // Error checking
  if ( MinObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MinObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MaxObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < MinObjectRange )
  {
    fprintf( stderr, "Error. MaxObjectRange can not be less than MinObjectRange!\n" );
    exit(-1);
  }

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const int CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) >= MinObjectRange && this->Pixel(i,j,k) <= MaxObjectRange)
          {
            // This pixel has already been assigned an object in the defined range, overlap occuring
            return true;
          }
        }
      }
    }
  }
  return false;
}
#endif

bool AnalyzeObjectMap::WriteObjectByName(const std::string & ObjectName, const std::string & filename)
{
  // Finding the object index with the name indicated
  if (ObjectName == std::string("Background"))
  {
    // Can not write a background object
    ::fprintf(stderr, "Error: Can not write out a background object.\n");
    return false;
  }

  int objectTag = 0;
  {
    for (int i = 1; i <= this->getNumberOfObjects(); i++)
    {
      if (ObjectName == this->getObjectEntry(i)->GetName())
      {
        objectTag = i;
        break;
      }
    }
  }

  if (objectTag == 0)
  {
    ::fprintf(stderr, "Error.  Object name not found in current objectmap.\n");
    return false;
  }

  FILE *fptr;
  std::string tempfilename=filename;

  // This assumes that the final '.' in a file name is the delimiter
  // for the file's extension type
  const std::string::size_type it = filename.find_last_of( "." );

  //Note: if there is no extension, then it=basic_string::npos;
  if(it > filename.length())
  {
    tempfilename+=".obj";
  }
  else
  {
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if(fileExt != ".obj")
    {
      tempfilename+=".obj";
    }
  }

  // Opening the file
  if ( ( fptr = ::fopen( tempfilename.c_str(), "wb" ) ) == NULL)
  {
    ::fprintf( stderr, "Error: Could not open %s\n", filename.c_str() );
    exit(-1);
  }

  int header[5];

  // Reading the Header into the class
  header[0]=VERSION7;
#if 0
  header[1]=this->getXDim();
  header[2]=this->getYDim();
  header[3]=this->getZDim();
  header[4]=2;                   // Including the background object when writing the .obj file

  bool NeedByteSwap = false;

  FileIOUtility util;
  // Byte swapping needed, Analyze object files are always written in BIG ENDIAN format
  if(util.getMachineEndianess() == util.DT_LITTLE_ENDIAN)
  {
    NeedByteSwap=true;
    util.FourByteSwap(&(header[0]));
    util.FourByteSwap(&(header[1]));
    util.FourByteSwap(&(header[2]));
    util.FourByteSwap(&(header[3]));
    util.FourByteSwap(&(header[4]));
  }

  // Creating the binary mask
  CImageFlat<unsigned char> binaryMask(this->getXDim(), this->getYDim(), this->getZDim());

  #endif
//TODO  binaryMask.ImageClear();

  // Creating the binary mask
  {
#if 0
    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if (this->ConstPixel(i) == objectTag)
      {
        binaryMask.Pixel(i) = 1;
      }
    }
#endif
  }

  // Writing the header, which contains the version number, the size, and the
  // number of objects
  if ( ::fwrite( header, sizeof(int), 5, fptr) != 5 )
  {
    ::fprintf( stderr, "Error: Could not write header of %s\n", filename.c_str());
    exit(-1);
  }

  // Since the NumberOfObjects does not reflect the background, the background will be written out

  // Writing the background header
  // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
  // not affect the current object itself
#if 0
  AnalyzeObjectEntry ObjectWrite = this->getObjectEntry(0);

  if (NeedByteSwap == true)
  {
    SwapObjectEndedness(ObjectWrite.getObjectPointer());
  }


  if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
  {
    ::fprintf(stderr, "13: Unable to write object background of %s\n", filename.c_str());
    exit(-1);
  }

  ObjectWrite = this->getObjectEntry(objectTag);

  if (NeedByteSwap == true)
  {
    SwapObjectEndedness(ObjectWrite.getObjectPointer());
  }

  // Writing the object header
  if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
  {
    ::fprintf(stderr, "13: Unable to write in object #%d description of %s\n", objectTag, filename.c_str());
    exit(-1);
  }

  RunLengthEncodeImage(binaryMask, fptr);

  ::fclose(fptr);
  #endif
  return true;
}


bool AnalyzeObjectMap::WriteDisplayedObjects(const std::string & filenamebase)
{
  FILE *fptr;
  std::string tempfilename=filenamebase;

  for (int objectTag = 1; objectTag <= this->getNumberOfObjects(); objectTag++)
  {

    // DEBUG
    // Look at the case when the user puts in a file with an .obj extension.  Append the
    // number in the middle of the filename, not at the end

    if (getObjectEntry(objectTag)->GetDisplayFlag() != 0)
    {
      // Append the object number to the filename
      std::string tempfilenumbername = tempfilename + std::string("_");

      std::string filenumber("#");
      filenumber[0] = (char)((int)'0' + objectTag);
      tempfilenumbername += filenumber;

      tempfilenumbername+=".obj";

      // Opening the file
      if ( ( fptr = ::fopen( tempfilenumbername.c_str(), "wb" ) ) == NULL)
      {
        ::fprintf( stderr, "Error: Could not open %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      int header[5];

      // Reading the Header into the class
      header[0]=VERSION7;
#if 0
      header[1]=this->getXDim();
      header[2]=this->getYDim();
      header[3]=this->getZDim();
      header[4]=2;               // Including the background object when writing the .obj file

      bool NeedByteSwap = false;

      FileIOUtility util;
      // Byte swapping needed, Analyze files are always written in BIG ENDIAN format
      if(util.getMachineEndianess() == util.DT_LITTLE_ENDIAN)
      {
        NeedByteSwap=true;
        util.FourByteSwap(&(header[0]));
        util.FourByteSwap(&(header[1]));
        util.FourByteSwap(&(header[2]));
        util.FourByteSwap(&(header[3]));
        util.FourByteSwap(&(header[4]));
      }

      // Creating the binary mask
      CImageFlat<unsigned char> binaryMask(this->getXDim(), this->getYDim(), this->getZDim());
//TODO      binaryMask.ImageClear();

      // Creating the binary mask
      for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
      {
        if (this->ConstPixel(i) == objectTag)
        {
          binaryMask.Pixel(i) = 1;
        }
      }

      // Writing the header, which contains the version number, the size, and the
      // number of objects
      if ( ::fwrite( header, sizeof(int), 5, fptr) != 5 )
      {
        ::fprintf( stderr, "Error: Could not write header of %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      // Since the NumberOfObjects does not reflect the background, the background will be written out

      // Writing the background header
      // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
      // not affect the current object itself
      AnalyzeObjectEntry ObjectWrite = this->getObjectEntry(0);

      if (NeedByteSwap == true)
      {
        SwapObjectEndedness(ObjectWrite.getObjectPointer());
      }

      if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
      {
        ::fprintf(stderr, "13: Unable to write object background of %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      ObjectWrite = this->getObjectEntry(objectTag);

      if (NeedByteSwap == true)
      {
        SwapObjectEndedness(ObjectWrite.getObjectPointer());
      }

      // Writing the object header
      if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
      {
        ::fprintf(stderr, "13: Unable to write in object #%d description of %s\n",
          objectTag, tempfilenumbername.c_str());
        exit(-1);
      }

      RunLengthEncodeImage(binaryMask, fptr);

      fclose(fptr);
    }
  }
  

#endif
  }
  }
  return true;
}

#if 0
bool AnalyzeObjectMap::CalculateBoundingRegionAndCenter( void )
{

  for (int i = 0; i < NumberOfObjects; i++)
  {
    AnaylzeObjectEntryArray[i]->SetMinimumXValue(this->getXDim());
    AnaylzeObjectEntryArray[i]->SetMaximumXValue(0);
    AnaylzeObjectEntryArray[i]->SetMinimumYValue(this->getYDim());
    AnaylzeObjectEntryArray[i]->SetMaximumYValue(0);
    AnaylzeObjectEntryArray[i]->SetMinimumZValue(this->getZDim());
    AnaylzeObjectEntryArray[i]->SetMaximumZValue(0);
  }

  const int PlaneSize = this->getXDim()*this->getYDim();

  std::vector<long int> xsum(this->getNumberOfObjects());
  std::vector<long int> ysum(this->getNumberOfObjects());
  std::vector<long int> zsum(this->getNumberOfObjects());
  std::vector<long int> xnum(this->getNumberOfObjects());
  std::vector<long int> ynum(this->getNumberOfObjects());
  std::vector<long int> znum(this->getNumberOfObjects());

  {
    for (int i = 0; i < this->getNumberOfObjects(); i++)
    {
      xsum[i] = 0;
      ysum[i] = 0;
      zsum[i] = 0;
      xnum[i] = 0;
      ynum[i] = 0;
      znum[i] = 0;
    }
  }

  int xIndex, yIndex, zIndex, r, s, t;

  for (zIndex=0, t=0; zIndex < this->getZDim(); zIndex++, t+=PlaneSize)
  {
    for (yIndex=0, s=t; yIndex < this->getYDim(); yIndex++, s+=this->getXDim())
    {
      for (xIndex=0, r=s; xIndex < this->getXDim(); xIndex++, r++)
      {
        int object = this->ConstPixel(r);
        if (object > this->getNumberOfObjects())
        {
          ::fprintf(stderr, "Error: There are greater index than number of objects.\n");
          exit(-1);
        }
        if (xIndex < getObjectEntry(object)->GetMinimumXValue())
          getObjectEntry(object)->SetMinimumXValue(xIndex);
        if (xIndex > getObjectEntry(object)->GetMaximumXValue())
          getObjectEntry(object)->SetMaximumXValue(xIndex);

        if (yIndex < getObjectEntry(object)->GetMinimumYValue())
          getObjectEntry(object)->SetMinimumYValue(yIndex);
        if (yIndex > getObjectEntry(object)->GetMaximumYValue()
          getObjectEntry(object)->SetMaximumYValue(yIndex);

        if (zIndex < getObjectEntry(object)->GetMinimumZValue())
          getObjectEntry(object)->SetMinimumZValue(zIndex);
        if (zIndex > getObjectEntry(object)->GetMaximumZValue())
          getObjectEntry(object)->SetMaximumZValue(zIndex);

        xsum[object]+=xIndex;
        ysum[object]+=yIndex;
        zsum[object]+=zIndex;
        xnum[object]++;
        ynum[object]++;
        znum[object]++;
      }
    }
  }

  {
    for (int i = 0; i < this->getNumberOfObjects(); i++)
    {
      if (xnum[i] == 0)
        getObjectEntry(i)->SetXCenter(0);
      else
        getObjectEntry(i)->SetXCenter((short int)(xsum[i]/xnum[i] - this->getXDim()/2));

      if (ynum[i] == 0)
        getObjectEntry(i)->SetYCenter(0);
      else
        getObjectEntry(i)->SetYCenter((short int)(ysum[i]/ynum[i] - this->getYDim()/2));

      if (znum[i] == 0)
        getObjectEntry(i)->SetZCenter(0);
      else
        getObjectEntry(i)->SetZCenter((short int)(zsum[i]/znum[i] - this->getZDim()/2));
    }
  }

  return true;
}


int AnalyzeObjectMap::EvenlyShade( void )
{
  int NumberOfShades = 255 / this->getNumberOfObjects();

  // Not allocating shades to the background object
  for (int i = 0; i < this->getNumberOfObjects(); i++)
    getObjectEntry(i+1)->SetShades(NumberOfShades);

  return NumberOfShades;
}


void AnalyzeObjectMap::ConstShade( void )
{
  // Not allocating shades to the background object, and setting each shade to 1
  for (int i = 0; i < this->getNumberOfObjects(); i++)
    getObjectEntry(i+1)->SetShades( 1 );
}


int AnalyzeObjectMap::getVersion( void ) const
{
  return Version;
}
#endif

int AnalyzeObjectMap::getNumberOfObjects( void ) const
{
  return this->NumberOfObjects;
}

#if 0
unsigned char AnalyzeObjectMap::isObjectShown( const unsigned char index ) const
{
  return ShowObject[index];
}


unsigned char AnalyzeObjectMap::getMinimumPixelValue( const unsigned char index ) const
{
  return MinimumPixelValue[index];
}


unsigned char AnalyzeObjectMap::getMaximumPixelValue( const unsigned char index ) const
{
  return MaximumPixelValue[index];
}

void AnalyzeObjectMap::setMinimumPixelValue( const unsigned char index, const unsigned char value )
{
  MinimumPixelValue[index] = value;
}

void AnalyzeObjectMap::setMaximumPixelValue( const unsigned char index, const unsigned char value )
{
  MaximumPixelValue[index] = value;
}
#endif

bool itk::AnalyzeObjectMap::RunLengthEncodeImage(itk::AnalyzeObjectMap SourceImage, std::fstream &fptr)
{
  if (fptr == NULL)
  {
    fprintf(stderr, "Error: Null file pointer for runlength encoding data.\n");
    return false;
  }

  // Encoding the run length encoded raw data into an unsigned char volume
  const int VolumeSize=SourceImage.GetXDim()*SourceImage.GetYDim()*SourceImage.GetZDim();
  const int PlaneSize = SourceImage.GetXDim()*SourceImage.GetYDim();
  int bufferindex=0;
  int planeindex=0;
  int runlength=0;
  unsigned char CurrentObjIndex=0;
  const  int buffer_size=16384;
                                 //NOTE: This is probably overkill, but it will work
  unsigned char buffer[buffer_size];
  //for almost all cases

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
  }
  return true;
}
#if 0

void CompactString( std::string & output, const std::string input )
{
  char *buffer = new char(input.length() + 1);
  int count = 0;

  for (unsigned int i = 0; i < input.length(); i++)
  {
    if (input[i] != ' ')
    {
      buffer[count] = input[i];
    }
  }

  buffer[input.length()] = '\0';

  output = std::string(buffer);

  delete[] buffer;
}
#endif
}
