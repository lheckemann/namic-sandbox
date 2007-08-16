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

//Acknowdlegment: Biomedical Imaging Resource
//         Mayo Foundation
//For lending us documention about the layout of an object map and the entries that correspond with the object map



//Also, do we need a printself?  Yes, and we will have to change a lot of naming, and stuff to meet ITK specifics.

//Example 4 erroding
//WHY should we care?
//Add in code snippets into the latex file


#include "itkAnalyzeObjectMap.h"
#include "itkImageRegionIterator.h"
namespace itk{
  AnalyzeObjectMap::AnalyzeObjectMap( void ): m_Version(VERSION7),m_NumberOfObjects(0)
  {
    //Create an object map of size 1,1,1 and have the pixles be 0.  Also, create one
    //object entry just like Analyze does with the name "Original", this entry
    //is usually the background.
    this->m_AnaylzeObjectEntryArray.resize(1);
    this->m_AnaylzeObjectEntryArray[0] = itk::AnalyzeObjectEntry::New();
    this->m_AnaylzeObjectEntryArray[0]->SetName("Original");
    this->SetNumberOfObjects(0);
    this->SetVersion(VERSION7);
    ImageType::SizeType size = {{1,1,1}};
    ImageType::IndexType orgin = {{0,0,0}};
    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(orgin);
    this->SetRegions(region);
    this->Allocate();
    this->FillBuffer(0);
  }

  AnalyzeObjectMap::~AnalyzeObjectMap( void )
  {
  }
  AnalyzeObjectEntryArrayType *AnalyzeObjectMap::GetAnalyzeObjectEntryArrayPointer()
  {
    return &(this->m_AnaylzeObjectEntryArray);
  }

  //This function will have the user pick which entry they want to be placed into
  //a new object map that will be returned.  The function will also go through the image
  //and change the values so that there is either 0 or 1.  1 corresponds to the entry
  //the user specified and 0 is the background.
  itk::AnalyzeObjectMap::Pointer AnalyzeObjectMap::PickOneEntry(const int numberOfEntry)
  {
    itk::AnalyzeObjectMap::Pointer newObjectMap = itk::AnalyzeObjectMap::New();
    newObjectMap->SetRegions(this->GetLargestPossibleRegion());
    newObjectMap->Allocate();
    newObjectMap->AddObjectEntry(this->m_AnaylzeObjectEntryArray[numberOfEntry]->GetName());
    newObjectMap->m_AnaylzeObjectEntryArray[1] = this->m_AnaylzeObjectEntryArray[numberOfEntry];
    itk::ThresholdImageFilter<ImageType>::Pointer changeOldObjectMap = itk::ThresholdImageFilter<ImageType>::New();
    
    changeOldObjectMap->SetInput(this);
    changeOldObjectMap->SetOutsideValue(0);
    changeOldObjectMap->ThresholdOutside(numberOfEntry,numberOfEntry);
    changeOldObjectMap->Update();
    changeOldObjectMap->SetInput(changeOldObjectMap->GetOutput());
    changeOldObjectMap->SetOutsideValue(1);
    changeOldObjectMap->ThresholdAbove(numberOfEntry-1);
    ImageType::Pointer newestObjectMapImage = changeOldObjectMap->GetOutput();
    changeOldObjectMap->Update();
    newObjectMap->SetPixelContainer(newestObjectMapImage->GetPixelContainer());
    return newObjectMap;
  }

  //This function will convert an object map into an unsigned char RGB image.
  //TODO: Need to include the object entries into the meta data.
  itk::Image<itk::RGBPixel<unsigned char>, 3>::Pointer AnalyzeObjectMap::ObjectMapToRGBImage()
  {
    RGBImageType::Pointer RGBImage = RGBImageType::New();
    RGBImage->SetRegions(this->GetLargestPossibleRegion());
    RGBImage->Allocate();
    itk::ImageRegionIterator<RGBImageType> RGBIterator(RGBImage, this->GetLargestPossibleRegion());
    itk::ImageRegionIterator<ImageType> ObjectIterator(this, this->GetLargestPossibleRegion());

    /*std::ofstream myfile;
    myfile.open("RGBImageVoxels2.txt");*/
    for(ObjectIterator.Begin(), RGBIterator.Begin(); !ObjectIterator.IsAtEnd(); ++ObjectIterator, ++RGBIterator)
    {
      RGBPixelType setColors;
      int i = ObjectIterator.Get();
      if(i != 0)
      {
        int j =1;
      }
      setColors.SetBlue(this->m_AnaylzeObjectEntryArray[ObjectIterator.Get()]->GetEndBlue());
      setColors.SetGreen(this->m_AnaylzeObjectEntryArray[ObjectIterator.Get()]->GetEndGreen());
      setColors.SetRed(this->m_AnaylzeObjectEntryArray[ObjectIterator.Get()]->GetEndRed());

      RGBIterator.Set(setColors);
      //myfile<<RGBIterator.Get()<<std::endl;
    }
    //myfile.close();
    return RGBImage;
  }

  //This function will take in an unsigned char of dimension size 3 and go through it and figure out the value the user wants picked out.  The user will also have to
  //specify what they want the new entry's name to be.  The user can also specify what RGB values they want but if they are not speficied the default values
  //are 0.
  void AnalyzeObjectMap::AddObjectEntryBasedOnImagePixel(const ImageType::Pointer Image, const int value, const std::string ObjectName, const int Red,const int Green,const int Blue)
  {
    itk::ImageRegion<3> ObjectMapRegion = this->GetLargestPossibleRegion();
    itk::ImageRegion<3> ImageRegion = Image->GetLargestPossibleRegion();
    if(  ImageRegion != ObjectMapRegion && ImageRegion.IsInside(ObjectMapRegion))
    {
      this->SetRegions(Image->GetLargestPossibleRegion());
      this->Allocate();
    }
    itk::ImageRegionIterator<ImageType > indexImage(Image, Image->GetLargestPossibleRegion());

    itk::ImageRegionIterator<ImageType > indexObjectMap(this,Image->GetLargestPossibleRegion());
    
    this->AddObjectEntry(ObjectName);
    int i = this->GetNumberOfObjects();
    this->m_AnaylzeObjectEntryArray[i]->SetEndRed(Red);
    this->m_AnaylzeObjectEntryArray[i]->SetEndGreen(Green);
    this->m_AnaylzeObjectEntryArray[i]->SetEndBlue(Blue);
    for(indexImage.Begin();!indexImage.IsAtEnd(); ++indexImage, ++indexObjectMap)
    {
      if(indexImage.Get() == value)
      {
        indexObjectMap.Set(i);
      }
      if(indexObjectMap.Get() > i)
      {
        indexObjectMap.Set(0);
      }
    }
  }
  

  /*NOTE: This function will add an object entry to the end of the vector.  However, you will still have to fill in the values that you would like stored.
  TODO: Rastor through the image to place the value at the specifed locations.*/
  void AnalyzeObjectMap::AddObjectEntry(const std::string ObjectName)
  {
    this->m_AnaylzeObjectEntryArray.insert(this->m_AnaylzeObjectEntryArray.end(), itk::AnalyzeObjectEntry::New());
    this->SetNumberOfObjects(this->GetNumberOfObjects()+1);
    this->m_AnaylzeObjectEntryArray[this->GetNumberOfObjects()]->SetName(ObjectName);
  }

  /*NOTE: This function will move all object entry's so that the vector stays in the smallest order starting from 0.*/
  void AnalyzeObjectMap::DeleteObjectEntry(const std::string ObjectName)
  {
    int i = this->FindObjectEntry(ObjectName);
    if(i == -1)
    {
      return;
    }
    for(int j = i; j < this->GetNumberOfObjects()-1; j++)
    {
      this->m_AnaylzeObjectEntryArray[j] = this->m_AnaylzeObjectEntryArray[j+1];
    }
    this->m_AnaylzeObjectEntryArray.erase(this->m_AnaylzeObjectEntryArray.end()-1);
    this->SetNumberOfObjects(this->GetNumberOfObjects()-1);
    this->m_AnaylzeObjectEntryArray.resize(this->GetNumberOfObjects());
    itk::ImageRegionIterator<ImageType > indexIt(this,this->GetLargestPossibleRegion());
    for(indexIt.Begin();!indexIt.IsAtEnd(); ++indexIt)
    {
      if(indexIt.Get() == i)
      {
        indexIt.Set(0);
      }
      else if(indexIt.Get()>i)
      {
        indexIt.Set(indexIt.Get()-1);
      }
    }
  }

  //This function will go through the entries looking for the specfic name.  If no name was found then the function
  //will return -1.  So, if you use this, then make sure you check to see if -1 was returned.
  int AnalyzeObjectMap::FindObjectEntry(const std::string ObjectName)
  {
    for(int i=0; i < this->GetNumberOfObjects(); i++)
    {
      if(!ObjectName.compare(this->m_AnaylzeObjectEntryArray.at(i)->GetName()))
      {
        return i;
      }
    }
    //If not found return -1
    return -1;
  }

  void AnalyzeObjectMap::PlaceObjectMapEntriesIntoMetaData()
  {
    itk::AnalyzeObjectEntryArrayType *my_reference=this->GetAnalyzeObjectEntryArrayPointer();

    MetaDataDictionary &thisDic=this->GetMetaDataDictionary();
    itk::EncapsulateMetaData<itk::AnalyzeObjectEntryArrayType>(thisDic,ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY,*my_reference);
  }

  AnalyzeObjectEntry::Pointer AnalyzeObjectMap::GetObjectEntry( const int index )
  {
    return this->m_AnaylzeObjectEntryArray.at(index);
  }

  void AnalyzeObjectMap::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

}
