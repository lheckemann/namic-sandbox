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
    this->SetNumberOfObjects(0);
    this->SetNumberOfVolumes(0);
    this->SetVersion(VERSION7);
    this->SetXDim(0);
    this->SetYDim(0);
    this->SetZDim(0);
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

  void AnalyzeObjectMap::AddObjectBasedOnImagePixel(itk::Image<unsigned char, 3>::Pointer Image, unsigned char value, std::string ObjectName)
  {
    itk::ImageRegion<3> ObjectMapRegion = this->GetLargestPossibleRegion();
    itk::ImageRegion<3> ImageRegion = Image->GetLargestPossibleRegion();
    if(  !ImageRegion.IsInside(ObjectMapRegion))
    {
      this->SetRegions(Image->GetLargestPossibleRegion());
      this->Allocate();
    }
    itk::ImageRegionIterator<itk::Image<unsigned char,3 > > indexObjectMap(this,Image->GetLargestPossibleRegion());
    itk::ImageRegionIterator<itk::Image<unsigned char,3>> indexImage(Image, Image->GetLargestPossibleRegion());
    this->AddObject(ObjectName);
    int i = this->GetNumberOfObjects();
    for(indexImage.Begin();!indexImage.IsAtEnd(); ++indexImage, ++indexObjectMap)
    {
      if(indexImage.Get() == value)
      {
        indexObjectMap.Set(i);
      }
    }
  }

  /*NOTE: This function will add an object entry to the end of the vector.  However, you will still have to fill in the values that you would like stored.
  TODO: Rastor through the image to place the value at the specifed locations.*/
  void AnalyzeObjectMap::AddObject(std::string ObjectName)
  {
    this->m_AnaylzeObjectEntryArray.insert(this->m_AnaylzeObjectEntryArray.end(), itk::AnalyzeObjectEntry::New());
    this->m_AnaylzeObjectEntryArray[this->GetNumberOfObjects()]->SetName(ObjectName);
    this->SetNumberOfObjects(this->GetNumberOfObjects()+1);
  }

  /*NOTE: This function will move all object entry's so that the vector stays in the smallest order starting from 0.*/
  void AnalyzeObjectMap::DeleteObject(std::string ObjectName)
  {
    int i = this->FindObject(ObjectName);
    if(i == -1)
    {
      return;
    }
    for(int j = i-1; j < this->GetNumberOfObjects()-1; j++)
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
      if(ObjectName.compare(this->m_AnaylzeObjectEntryArray[i]->GetName()))
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
