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
    this->m_AnaylzeObjectEntryArray.resize(1);
   this->m_AnaylzeObjectEntryArray[0] = itk::AnalyzeObjectEntry::New();
    this->m_AnaylzeObjectEntryArray[0]->SetName("Original");
    this->SetNumberOfObjects(0);
    this->SetNumberOfVolumes(1);
    this->SetVersion(VERSION7);
    this->SetXDim(1);
    this->SetYDim(1);
    this->SetZDim(1);
    itk::Image<unsigned char,3>::SizeType size = {{1,1,1}};
    itk::Image<unsigned char,3>::IndexType orgin = {{0,0,0}};
    itk::Image<unsigned char,3>::RegionType region;
    region.SetSize(size);
    region.SetIndex(orgin);
    this->SetRegions(region);
    this->Allocate();
    this->FillBuffer(0);

    ////TODO:  Clear the image this->ImageClear();
    //{
    //  for (int i = 0; i < 256; i++)
    //  {
    //    m_AnaylzeObjectEntryArray[i] = NULL;
    //  }
    //}

    //// Setting object zero as the background
    //m_AnaylzeObjectEntryArray[0] = AnalyzeObjectEntry::New();
    //this->getObjectEntry(0)->SetName("Background");
    //this->getObjectEntry(0)->SetDisplayFlag(0);
    //this->getObjectEntry(0)->SetOpacity(0);
    //this->getObjectEntry(0)->SetOpacityThickness(0);
    //this->getObjectEntry(0)->SetEndRed(0);
    //this->getObjectEntry(0)->SetEndGreen(0);
    //this->getObjectEntry(0)->SetEndBlue(0);
    //this->getObjectEntry(0)->SetShades(1);
  }

  AnalyzeObjectMap::~AnalyzeObjectMap( void )
  {
    /*for(int i=0; i < 256; i++)
    {
      if(m_AnaylzeObjectEntryArray[i].IsNotNull())
      {
        m_AnaylzeObjectEntryArray[i] = NULL;
      }
    }*/
  }
  AnalyzeObjectEntryArrayType *AnalyzeObjectMap::GetAnalyzeObjectEntryArrayPointer()
  {
    return &(this->m_AnaylzeObjectEntryArray);
  }

  itk::AnalyzeObjectMap::Pointer AnalyzeObjectMap::PickOneEntry(int numberOfEntry)
  {
    itk::AnalyzeObjectMap::Pointer newObjectMap = itk::AnalyzeObjectMap::New();
    newObjectMap->SetRegions(this->GetLargestPossibleRegion());
    newObjectMap->Allocate();
    newObjectMap->AddObject(this->m_AnaylzeObjectEntryArray[numberOfEntry]->GetName());
    //itk::AnalyzeObjectEntry::Pointer newObjectEntry = this->getObjectEntry(numberOfEntry);
    newObjectMap->m_AnaylzeObjectEntryArray[1] = this->m_AnaylzeObjectEntryArray[numberOfEntry];
    //newObjectMap = this;
    itk::ThresholdImageFilter<itk::Image<unsigned char, 3>>::Pointer changeOldObjectMap = itk::ThresholdImageFilter<itk::Image<unsigned char, 3>>::New();
    
    changeOldObjectMap->SetInput(this);
    changeOldObjectMap->SetOutsideValue(0);
    changeOldObjectMap->ThresholdOutside(numberOfEntry,numberOfEntry);
//    itk::Image<unsigned char,3> newestObjectMap = changeOldObjectMap->m;
    changeOldObjectMap->Update();
    changeOldObjectMap->SetInput(changeOldObjectMap->GetOutput());
    changeOldObjectMap->SetOutsideValue(1);
    changeOldObjectMap->ThresholdAbove(numberOfEntry-1);
    itk::Image<unsigned char, 3>::Pointer newestObjectMapImage = changeOldObjectMap->GetOutput();
    changeOldObjectMap->Update();

    newObjectMap->SetPixelContainer(newestObjectMapImage->GetPixelContainer());
    //changeOldObjectMap->SetOutsideValue(1);
    //changeOldObjectMap->ThresholdOutside(0,0);
    //changeOldObjectMap->Update();
    return newObjectMap;
  }

  itk::Image<itk::RGBPixel<unsigned char>, 3>::Pointer AnalyzeObjectMap::ObjectMapToRGBImage()
  {
    RGBImageType::Pointer RGBImage = RGBImageType::New();
    RGBImage->SetRegions(this->GetLargestPossibleRegion());
    RGBImage->Allocate();
    itk::ImageRegionIterator<RGBImageType> RGBIterator(RGBImage, this->GetLargestPossibleRegion());
    itk::ImageRegionIterator<itk::Image<unsigned char, 3>> ObjectIterator(this, this->GetLargestPossibleRegion());

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

  void AnalyzeObjectMap::AddObjectBasedOnImagePixel(itk::Image<unsigned char, 3>::Pointer Image, int value, std::string ObjectName, int Red, int Green, int Blue)
  {
    
    itk::ImageRegion<3> ObjectMapRegion = this->GetLargestPossibleRegion();
    itk::ImageRegion<3> ImageRegion = Image->GetLargestPossibleRegion();
    if(  ImageRegion != ObjectMapRegion)
    {
      //this->Graft(Image);
      this->SetRegions(Image->GetLargestPossibleRegion());
      
      this->Allocate();
      //this->FillBuffer(0);
      this->SetXDim(this->GetLargestPossibleRegion().GetSize(0));
      this->SetYDim(this->GetLargestPossibleRegion().GetSize(1));
      this->SetZDim(this->GetLargestPossibleRegion().GetSize(2));
    }
    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexImage(Image, Image->GetLargestPossibleRegion());

    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexObjectMap(this,Image->GetLargestPossibleRegion());
    
    this->AddObject(ObjectName);
    int i = this->GetNumberOfObjects();
    this->m_AnaylzeObjectEntryArray[i]->SetEndRed(Red);
    this->m_AnaylzeObjectEntryArray[i]->SetEndGreen(Green);
    this->m_AnaylzeObjectEntryArray[i]->SetEndBlue(Blue);
    for(indexImage.Begin();!indexImage.IsAtEnd(); ++indexImage, ++indexObjectMap)
    {
      int k = indexImage.Get();
      if(k!=0)
      {
        int j =1;
      }
      
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
  void AnalyzeObjectMap::AddObject(std::string ObjectName)
  {
    this->m_AnaylzeObjectEntryArray.insert(this->m_AnaylzeObjectEntryArray.end(), itk::AnalyzeObjectEntry::New());
    this->SetNumberOfObjects(this->GetNumberOfObjects()+1);
    this->m_AnaylzeObjectEntryArray[this->GetNumberOfObjects()]->SetName(ObjectName);
  }

  /*NOTE: This function will move all object entry's so that the vector stays in the smallest order starting from 0.*/
  void AnalyzeObjectMap::DeleteObject(std::string ObjectName)
  {
    int i = this->FindObject(ObjectName);
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
    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexIt(this,this->GetLargestPossibleRegion());
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

  int AnalyzeObjectMap::FindObject(std::string ObjectName)
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

}
