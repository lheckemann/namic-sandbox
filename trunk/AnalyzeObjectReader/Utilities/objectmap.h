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
//TODO:  #include "itkImage.h"



//Need to put this into the ITK namespace.
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

    /**
   * Constant defining the maximum number of shades possible for an object as of Version 6
   */
  const int MAXANALYZESHADES = 250;



  /**
   * Buffer size for reading in the run length encoded object data
   */
  const int BUFFERSIZE = 16384;

  //TODO:  derive from itk::Image object so that an AnalyzeObjectMap to get these to be formal itk pipeline managable objects.
  class AnalyzeObjectMap: public itk::Image<unsigned char,3>
  {
    public:
      /**
       * \brief the default constructor, initializes to 0 or NULL
       */
      AnalyzeObjectMap( void );

      /**
       * \brief the copy constructor,
       * initializes to an existing object
       * \param AnalyzeObjectMap & rhs
       * \sa AnalyzeObjectMap
       */
      AnalyzeObjectMap( const AnalyzeObjectMap & rhs );

      /**
       * \brief the constructor that creates and ObjectMap
       * \param const int _iX
       * \param const int _iY
       * \param const int _iZ
       * \sa AnalyzeObjectMap
       */
      AnalyzeObjectMap(const int _iX, const int _iY, const int _iZ);

      /**
       * \brief the destructor for AnalyzeObjectMap
       */
      ~AnalyzeObjectMap( void );

      /**
       * \brief an assignment operator
       * \param const AnalyzeObjectMap & rhs
       * \return AnalyzeObjectMap &, a new object created from the right hand side
       */
      AnalyzeObjectMap & operator=( const AnalyzeObjectMap & rhs );

      /**
       * \brief returns index to object called ObjectName
       * \param ObjectName is the name of the object to find index to.
       * \return const int index if ObjectName exist else -1
       */
      int getObjectIndex( const std::string & ObjectName );

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
       * \brief This function clears an object map and reinitializes it to single background
       * \param _iX x dimension
       * \param _iY y dimension
       * \param _iZ z dimension
       */
      void ReinitializeObjectMap(const int _iX, const int _iY, const int _iZ);

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
       * \brief This function is used to add an object by setting tags if the
       * voxel intensities from the input image are in the range MinRange-MaxRange.
       * \param InputImage The image to use for tagging
       * \param MinRange The lower bound on the voxels
       * \param MaxRange The upper bound on the voxels
       * \param ObjectName the new name for the object
       * \param EndRed the brightest shade of the Red component of the desired color
       * \param EndGreen the brightest shade of the Green component of the desired color
       * \param EndBlue the brightest shade of the Blue component of the desired color
       * \param Shades the number of shades to assign to this object
       * \param OverWriteObjectFlag a flag to determine whether a new object has precedence over a
       * previously defined object
       * \return true if successful
       */
      bool AddObjectInRange(const itk::Image<unsigned char, 3>::Pointer & InputImage,
        const int MinRange, const int MaxRange,
        std::string ObjectName, const int EndRed, const int EndGreen,
        const int EndBlue, const int Shades, bool OverWriteObjectFlag);

      /**
       * \brief This function will remove an object from the object map by its name and shift all tags in
       * the object file down by one if its current tag is greater than the object's tag being removed
       * \param ObjectName string of name to remove
       * \return returns true if successful
       */
      bool RemoveObjectByName(const std::string & ObjectName);

      /**
       * \brief This function will remove objects by the tags from MinRange to MaxRange
       * \param MinRange the lower index of the range of objects to remove
       * \param MaxRange the upper index of the range of objects to remove
       * \return returns true if successful
       */
      bool RemoveObjectByRange( const unsigned char MinRange, const unsigned char MaxRange );

      /**
       * \brief This function will check to see if the new object
       * will cause an overlap with objects in the objectmap between MinObjectRange and
       * MaxObjectRange for the object defined by voxel intensities from the input image
       * that are in the range MinRange-MaxRange.
       * \param InputImage The image to check against the current objectmap
       * \param MinRange The lower bound on the voxels
       * \param MaxRange The upper bound on the voxels
       * \param MinObjectRange The lower bound of the objects in the objectmap to check
       * \param MinObjectRange The upper bound of the objects in the objectmap to check
       * \return true if overlap will occur
       */
      bool CheckObjectOverlap( const itk::Image<unsigned char,3>::Pointer & InputImage, const int MinRange, const int MaxRange, const int MinObjectRange = 1, const int MaxObjectRange = 255 );

      /**
       * \brief This function will write out an object with a background by looking up its name in the
       * objectmap.
       * \param ObjectName string of name of object to write out to a file
       * \param filename the filename to write the object
       */
      bool WriteObjectByName(const std::string & ObjectName, const std::string & filename);

      /**
       * \brief This function will write out the objects in the form of separate objects
       * \param filename the base filename for all the objects, will append object tag and .obj
       * automatically
       */
      bool WriteDisplayedObjects(const std::string &filenamebase);

      /**
       * \brief This function calculates the bounding regions and center of the object
       */
      bool CalculateBoundingRegionAndCenter( void );

      /**
       * \brief This function recalculates the number of shades allocated to each object to
       * evenly shade each object, since limit of Analyze shades is 255
       * \return int shades allocated per object
       */
      int EvenlyShade( void );

      /**
       * \brief This function assigns one shade per object for displaying raw colors without
       * any shading.
       * \return none
       */
      void ConstShade( void );

      /**
       * \brief This function is used to get the Analyze version of the object
       */
      int getVersion( void ) const;

      /**
       * \brief This function is used to determine the number of objects in the Object map
       */
      int getNumberOfObjects( void ) const;

      /**
       * \brief This function is used to determine if the object at the specific index is
       * to be shown
       * \param index
       */
      unsigned char isObjectShown( const unsigned char index ) const;

      /**
       * \brief This function gets the Minimum pixel value of the Object to
       * be shown
       * \param index
       */
      unsigned char getMinimumPixelValue( const unsigned char index ) const;

      /**
       * \brief This function is used to get Maximum pixel value of the object to
       * be shown
       * \param index
       */
      unsigned char getMaximumPixelValue( const unsigned char index ) const;

    /**
     * \brief This function is used to set Minimum pixel value of the Object to be shown
     * \param index the object index
     * \param value the minimum value
     */
    void setMinimumPixelValue( const unsigned char index, const unsigned char value );

    /**
     * \brief This function is used to set Maximum pixel value of the Object to be shown
     * \param index the object index
     * \param value the maximum value
     */
    void setMaximumPixelValue( const unsigned char index, const unsigned char value );

    private:
      bool CopyBaseImage( const AnalyzeObjectMap& rhs );
      /** Version of object file */
      int Version;
      /** Number of Objects in the object file */
      int NumberOfObjects;
      /** Pointers to individual objects in the object map, maximum of 256 */
      AnalyzeObjectEntry::Pointer AnaylzeObjectEntryArray[256];
      /** Flag to determine the display of the layer */
      unsigned char ShowObject[256];
      unsigned char MinimumPixelValue[256];
      unsigned char MaximumPixelValue[256];
      int NeedsSaving;
      int NeedsRegionsCalculated;
      itk::Image<unsigned char, 3>::Pointer m_LableMap;

//TODO The following should be private member functions of the AnalyzeObjectMap class

   /**
    * \brief This function takes a file pointer and an image and runlength encodes the
    * gray levels to the file
    * \param SourceImage The image to be runlength encoded
    * \param The pointer to the file to be written out.
    * \return returns true if successful
    */
   bool RunLengthEncodeImage(const itk::Image<unsigned char,3>::Pointer SourceImage, FILE *fptr);

   /**
    * \brief This function takes a string and removes the spaces
    * \param output the string with the spaces removed
    * \param input the string to remove the spaces
    */
   void RemoveSpaceFromString(std::string & output, const std::string & input);


  };
  }
#endif                           // __OBJECTMAP_H_
