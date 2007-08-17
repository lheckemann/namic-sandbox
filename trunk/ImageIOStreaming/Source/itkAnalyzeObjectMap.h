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
#ifndef __OBJECTMAP_H_
#define __OBJECTMAP_H_

#include <stdio.h>
#include <string>
#include <vector>
#include "itkAnalyzeObjectEntry.h"
#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkByteSwapper.h"
#include <itkMetaDataDictionary.h>
#include "itkMetaDataObject.h"
#include <itkUnaryFunctorImageFilter.h>
#include <itkScalarToRGBPixelFunctor.h>
#include <itkThresholdImageFilter.h>

const char *const ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY = "ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY";
/**
  * Constants representing the current version number of the object map file for Analyze
  */
  const int VERSION1 = 880102;
  const int VERSION2 = 880801;
  const int VERSION3 = 890102;
  static const int VERSION4 = 900302;
  static const int VERSION5 = 910402;
  static const int VERSION6 = 910926;
  static const int VERSION7 = 20050829;
namespace itk
{

  typedef std::vector<AnalyzeObjectEntry::Pointer>  AnalyzeObjectEntryArrayType;
  template <class TImage>

  
  
  


/** \class AnalyzeObjectMap
 * \brief A class that is an image with functions that let the user change aspects of the class.
 */
//We are initialzing the object map to dimension 4 to take care of the highest possible dimension that an
//object map can be.
  class AnalyzeObjectMap: public TImage
  {
    public:
      

      /** Standard typedefs. */
      typedef AnalyzeObjectMap Self;
      typedef TImage  Superclass;
      typedef SmartPointer<Self>  Pointer;
      typedef SmartPointer<const Self>  ConstPointer;

      typedef itk::RGBPixel<unsigned char> RGBPixelType;

      typedef itk::Image<RGBPixelType, 4> RGBImageType;
      
      typedef TImage ImageType;
      
      /** Method for creation through the object factory. */
      itkNewMacro(Self);

      /** Run-time type information (and related methods). */
      itkTypeMacro(AnalyzeObjectMap, TImage );

      

      /**
       * \brief an assignment operator
       * \param const AnalyzeObjectMap & rhs
       * \return AnalyzeObjectMap &, a new object created from the right hand side
       */
      AnalyzeObjectMap & operator=( const AnalyzeObjectMap & rhs );

      
      AnalyzeObjectEntryArrayType * GetAnalyzeObjectEntryArrayPointer();


      /**
       * \brief GetVersion/SetVersion
       *
       * This function is used to Get/Set the Analyze version of the object
       */
      itkSetMacro(Version, int);
      itkGetConstMacro(Version, int);

      /**
       * \brief GetNumberOfObjects/SetNumberOfObjects
       *
       * This function is used to Get/Set the number of objects in the Object map
       */
      itkSetMacro(NumberOfObjects, int);
      itkGetConstMacro(NumberOfObjects, int);

      /**
       * \brief PickOneEntry
       */
     // itk::AnalyzeObjectMap<itk::Image<unsigned char, 4>>::Pointer PickOneEntry(const int numberOfEntry = -1);

      /**
       * \brief ObjectMapToRGBImage
       */
      itk::Image<RGBPixelType, 4>::Pointer ObjectMapToRGBImage();

      /**
       * \brief AddObjectEntryBasedOnImagePixel
       */
      void AddObjectEntryBasedOnImagePixel(ImageType *Image,const int value = -1,const std::string ObjectName = "",const int Red = 0,const int Green = 0,const int Blue = 0);

      /**
       * \brief AddObjectEntry
       */
      void AddObjectEntry(const std::string ObjectName = "");

      /**
       * \brief DeleteObjectEntry
       *
       * This function will move all object entry's so that the vector stays in the smallest order starting from 0.
       */
      void DeleteObjectEntry(const std::string ObjectName = "");

      /**
       * \brief FindObject
       */
      int FindObjectEntry(const std::string ObjectName = "");

      /**
       * \brief PlaceObjectMapEntriesIntoMetaData
       */
      void PlaceObjectMapEntriesIntoMetaData();

      /**
       * \brief returns a reference to an object
       * \param const int index
       * \return AnalyzeObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      AnalyzeObjectEntry::Pointer GetObjectEntry( const int index );

      /**
       * \brief returns a reference to an object
       * \param const int index
       * \return AnalyzeObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      const AnalyzeObjectEntry::Pointer GetObjectEntry( const int index ) const;

//itk::AnalyzeObjectMap<TImage> * TransformImage(ImageType *image);
      void TransformImage(ImageType *image);

     protected:
       /**
        * \brief the default constructor, initializes to 0 or NULL
        */
      AnalyzeObjectMap( void );

      /**
       * \brief the destructor for AnalyzeObjectMap
       */
      virtual ~AnalyzeObjectMap( void );

      /**
       * \brief the copy constructor,
       * THIS IS NOT ALLOWED
       */
      AnalyzeObjectMap( const AnalyzeObjectMap & rhs ) { /*Explicitly not allowed*/ };

      void PrintSelf(std::ostream& os, Indent indent) const;

   private:
      /** Version of object file */
      int m_Version;
      /** Number of Objects in the object file */
      int m_NumberOfObjects;
      /** Pointers to individual objects in the object map, maximum of 256 */
      AnalyzeObjectEntryArrayType m_AnaylzeObjectEntryArray;
  };
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAnalyzeObjectMap.txx"
#endif
#endif                           // __OBJECTMAP_H_
