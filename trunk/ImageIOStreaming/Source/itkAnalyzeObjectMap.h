/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkNiftiImageIO.cxx,v $
Language:  C++
Date:      $Date: 2007/07/27 18:00:56 $
Version:   $Revision: 1.37 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnalyzeObjectMap_h
#define __itkAnalyzeObjectMap_h

#include <stdio.h>
#include <string>
#include <vector>
#include "itkAnalyzeObjectEntry.h"
#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkByteSwapper.h"
#include <itkMetaDataDictionary.h>
#include "itkMetaDataObject.h"
#include "itkThresholdImageFilter.h"
#include "itkImage.h"

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
template <class TImage = itk::Image<unsigned char, 4>, class TRGBImage = itk::Image<itk::RGBPixel<unsigned char>,4> >


/** \class AnalyzeObjectMap
 * \brief A class that is an image with functions that let the user change aspects of the class.  This
 * is a templated class where most everything will depend on the Image type that is used.
 */
  class AnalyzeObjectMap: public TImage
{
    public:
      

      /** Standard typedefs. */
      typedef AnalyzeObjectMap              Self;
      typedef TImage                        Superclass;
      typedef SmartPointer<Self>            Pointer;
      typedef SmartPointer<const Self>      ConstPointer;
      
      typedef TImage                        ImageType;
      typedef itk::AnalyzeObjectMap<TImage> ObjectMapType;

      typedef typename TImage::PixelType    PixelType;
      
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
       * \brief GetNumberOfObjects/SetNumberOfObjects
       *
       * This function is used to Get/Set the number of objects in the Object map
       */
      itkSetMacro(NumberOfObjects, int);
      itkGetConstMacro(NumberOfObjects, int);

      /**
       * \brief PickOneEntry
       */
      typename itk::AnalyzeObjectMap<TImage>::Pointer PickOneEntry(const int numberOfEntry = -1);

      /**
       * \brief ObjectMapToRGBImage
       */
      typename TRGBImage::Pointer ObjectMapToRGBImage();

      /**
       * \brief AddObjectEntryBasedOnImagePixel
       */
      void AddObjectEntryBasedOnImagePixel(ImageType *Image,const int value = -1,const std::string ObjectName = "",const int Red = 0,const int Green = 0,const int Blue = 0);

      /**
       * \brief AddObjectEntry
       */
      void AddAnalyzeObjectEntry(const std::string ObjectName = "");

      /**
       * \brief DeleteObjectEntry
       *
       * This function will move all object entry's so that the vector stays in the smallest order starting from 0.
       */
      void DeleteAnalyzeObjectEntry(const std::string ObjectName = "");

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

      void ImageToObjectMap(ImageType *image);

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
