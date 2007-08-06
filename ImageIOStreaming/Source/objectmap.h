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
#include "objectentry.h"
#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkByteSwapper.h"
#include <itkMetaDataDictionary.h>
#include "itkMetaDataObject.h"
#include <itkUnaryFunctorImageFilter.h>
#include <itkScalarToRGBPixelFunctor.h>


namespace itk
{
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

  typedef std::vector<AnalyzeObjectEntry::Pointer>  AnalyzeObjectEntryArrayType;
  const char *const ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY = "ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY";

  /**
  * Buffer size for reading in the run length encoded object data
  */
  const int NumberOfRunLengthElementsPerRead = 1;

  class AnalyzeObjectMap: public itk::Image<unsigned char,3>
  {
    public:
      

      /** Standard typedefs. */
      typedef AnalyzeObjectMap Self;
      typedef Image<unsigned char, 3>  Superclass;
      typedef SmartPointer<Self>  Pointer;
      typedef SmartPointer<const Self>  ConstPointer;

      typedef itk::RGBPixel<unsigned char> RGBPixelType;

      typedef itk::Image<RGBPixelType, 3> RGBImageType;
      
      /** Method for creation through the object factory. */
      itkNewMacro(Self);

      /** Run-time type information (and related methods). */
      itkTypeMacro(AnalyzeObjectMap, Image );

      /**
       * \brief the destructor for AnalyzeObjectMap
       */
      virtual ~AnalyzeObjectMap( void );

      /**
       * \brief an assignment operator
       * \param const AnalyzeObjectMap & rhs
       * \return AnalyzeObjectMap &, a new object created from the right hand side
       */
      AnalyzeObjectMap & operator=( const AnalyzeObjectMap & rhs );

      
      AnalyzeObjectEntryArrayType * GetAnalyzeObjectEntryArrayPointer();
      /**
       * \brief returns a reference to an object
       * \param const int index
       * \return AnalyzeObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      AnalyzeObjectEntry::Pointer getObjectEntry( const int index );

      /**
       * \brief returns a reference to an object
       * \param const int index
       * \return AnalyzeObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      const AnalyzeObjectEntry::Pointer getObjectEntry( const int index ) const;

      /**
       * \brief This function is used to read in the object file
       * \param const std::string & filename
       */
      bool ReadObjectFile( const std::string& filename );

      /**
       * \brief This function is used to write the object file
       * \param const std:: string & filename
       */
      bool WriteObjectFile( const std::string& filename );


      /**
       * \brief GetVersion/SetVersion
       *
       * This function is used to Get/Set the Analyze version of the object
       */
      itkSetMacro(Version, int);
      itkGetConstMacro(Version, int);

      /**
       * \brief GetXDim/SetXDim
       *
       * This function is used to Get/Set the width of an object
       */
      itkSetMacro(XDim, int);
      itkGetConstMacro(XDim, int);

      /**
       * \brief GetYDim/SetYDim
       *
       * This function is used to Get/Set the height of an object
       */
      itkSetMacro(YDim, int);
      itkGetConstMacro(YDim, int);

      /**
       * \brief GetZDim/SetZDim
       *
       * This function is used to Get/Set the depth of an object
       */
      itkSetMacro(ZDim, int);
      itkGetConstMacro(ZDim, int);

      /**
       * \brief GetNumberOfObjects/SetNumberOfObjects
       *
       * This function is used to Get/Set the number of objects in the Object map
       */
      itkSetMacro(NumberOfObjects, int);
      itkGetConstMacro(NumberOfObjects, int);

      /**
       * \brief GetNumberOfVolumes/SetNumberOfVolumes
       * 
       * This function is used to Get/Set the number of object volumes
       */
      itkSetMacro(NumberOfVolumes, int);
      itkGetConstMacro(NumberOfVolumes, int);

      itk::Image<RGBPixelType, 3>::Pointer ObjectMapToRGBImage();

      void AddObjectBasedOnImagePixel(itk::Image<unsigned char, 3>::Pointer Image,unsigned char value, std::string ObjectName, int Red = 0, int Green = 0, int Blue = 0);

      void AddObject(std::string ObjectName);

      void DeleteObject(std::string ObjectName);

      int FindObject(std::string ObjectName);

      void PlaceObjectMapEntriesIntoMetaData();

     protected:
       /**
        * \brief the default constructor, initializes to 0 or NULL
        */
      AnalyzeObjectMap( void );

      /**
       * \brief the copy constructor,
       * THIS IS NOT ALLOWED
       */
      AnalyzeObjectMap( const AnalyzeObjectMap & rhs ) { /*Explicitly not allowed*/ };

   private:
      /**
       * \brief This function takes a string and removes the spaces
       * \param output the string with the spaces removed
       * \param input the string to remove the spaces
       */
      void RemoveSpaceFromString(std::string & output, const std::string & input);


      /** Version of object file */
      int m_Version;
      /** Number of Objects in the object file */
      int m_NumberOfObjects;
      /** Pointers to individual objects in the object map, maximum of 256 */
      AnalyzeObjectEntryArrayType m_AnaylzeObjectEntryArray;


///HACK--DELETE THESE LATER
      //TODO:  The next 4 ivars need to be removed!!  They are aliases to the image dimensions.
      int m_XDim;
      int m_YDim;
      int m_ZDim;
      int m_NumberOfVolumes;

      
  };
}
#endif                           // __OBJECTMAP_H_
