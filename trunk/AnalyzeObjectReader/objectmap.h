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
#ifndef __UIIG_OBJECTMAP_H_
#define __UIIG_OBJECTMAP_H_

/**
 * \todo templetize AddObjectInRange
 * \todo Add getObjectIndex ( object_name )
 */
//
// Name: objectmap.h
// Author: John Dill
// Date: 5/16/00
// Classes:
// CObjectMap - Represents an Analyze ObjectMap (defining color coded structures for an image)
//

#include <stdio.h>
#include <string>
#include <vector>
#include <uiigcore/imageflat.h>
#include <uiigfileio/fileio.h>
#include <uiigfileio/fileutils.h>
#include "objectentry.h"

/**
 * \author John Dill
 * \brief This function takes a file pointer and an image and runlength encodes the
 * gray levels to the file
 * \param SourceImage The image to be runlength encoded
 * \param The pointer to the file to be written out.
 * \return returns true if successful
 */
bool RunLengthEncodeImage(const uiig::CImageFlat<unsigned char> & SourceImage, FILE *fptr);

/**
 * \author John Dill
 * \brief This function takes a string and removes the spaces
 * \param output the string with the spaces removed
 * \param input the string to remove the spaces
 */
void RemoveSpaceFromString(std::string & output, const std::string & input);

namespace uiig
{
  class CObjectMap : public uiig::CImageFlat<unsigned char>
  {
    public:
      /**
       * \author John Dill
       * \brief the default constructor, initializes to 0 or NULL
       */
      CObjectMap( void );

      /**
       * \author John Dill
       * \brief the copy constructor,
       * initializes to an existing object
       * \param CObjectMap & rhs
       * \sa CObjectMap
       */
      CObjectMap( const CObjectMap & rhs );

      /**
       * \author John Dill
       * \brief the constructor that creates and ObjectMap
       * \param const int _iX
       * \param const int _iY
       * \param const int _iZ
       * \sa CObjectMap
       */
      CObjectMap(const int _iX, const int _iY, const int _iZ);

      /**
       * \author John Dill
       * \brief the destructor for CObjectMap
       */
      ~CObjectMap( void );

      /**
       * \author John Dill
       * \brief an assignment operator
       * \param const CObjectMap & rhs
       * \return CObjectMap &, a new object created from the right hand side
       */
      CObjectMap & operator=( const CObjectMap & rhs );

      /**
       * \author Hans J. Johnson
       * \brief returns index to object called ObjectName
       * \param ObjectName is the name of the object to find index to.
       * \return const int index if ObjectName exist else -1
       */
      int getObjectIndex( const std::string & ObjectName );

      /**
       * \author John Dill
       * \brief returns a reference to an object
       * \param const int index
       * \return CObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      CObjectEntry & getObjectEntry( const int index );

      /**
       * \author John Dill
       * \brief returns a reference to an object
       * \param const int index
       * \return CObjectEntry &, an object reference from the array of 256 objects in the objectmap
       */
      const CObjectEntry & getObjectEntry( const int index ) const;

      /**
       * \author John Dill
       * \brief This function clears an object map and reinitializes it to single background
       * \param _iX x dimension
       * \param _iY y dimension
       * \param _iZ z dimension
       */
      void ReinitializeObjectMap(const int _iX, const int _iY, const int _iZ);

      /**
       * \author Hans J. Johnson
       * \brief This function is used to read in the object file
       * \param const std::string & filename
       */
      bool ReadObjectFile( const std::string& filename );

      /**
       * \author Hans J. Johnson
       * \brief This function is used to write the object file
       * \param const std:: string & filename
       */
      bool WriteObjectFile( const std::string& filename );

      /**
       * \author Hans J. Johnson
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
      bool AddObjectInRange(const uiig::CImage<float> & InputImage,
        const float MinRange, const float MaxRange,
        std::string ObjectName, const int EndRed, const int EndGreen,
        const int EndBlue, const int Shades, bool OverWriteObjectFlag);

      /**
       * \author John Dill
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
      bool AddObjectInRange(const uiig::CImage<unsigned char> & InputImage,
        const int MinRange, const int MaxRange,
        std::string ObjectName, const int EndRed, const int EndGreen,
        const int EndBlue, const int Shades, bool OverWriteObjectFlag);

      /**
       * \author John Dill
       * \brief This function will remove an object from the object map by its name and shift all tags in
       * the object file down by one if its current tag is greater than the object's tag being removed
       * \param ObjectName string of name to remove
       * \return returns true if successful
       */
      bool RemoveObjectByName(const std::string & ObjectName);

      /**
       * \author John Dill
       * \brief This function will remove objects by the tags from MinRange to MaxRange
       * \param MinRange the lower index of the range of objects to remove
       * \param MaxRange the upper index of the range of objects to remove
       * \return returns true if successful
       */
      bool RemoveObjectByRange( const unsigned char MinRange, const unsigned char MaxRange );

      /**
       * \author John Dill
       * \brief This function will check to see if the new object
       * will cause an overlap with objects in the objectmap between MinObjectRange and
       * MaxObjectRange for the object defined by voxel intensities from the input image
       * that are in the range MinRange-MaxRange.
       * \param InputImage The image to use for tagging
       * \param MinRange The lower bound on the voxels
       * \param MaxRange The upper bound on the voxels
       * \param MinObjectRange The lower bound of the objects in the objectmap to check
       * \param MinObjectRange The upper bound of the objects in the objectmap to check
       * \return true if overlap will occur
       */
      bool CheckObjectOverlap( const uiig::CImage<float> & InputImage, const float MinRange, const float MaxRange, const int MinObjectRange = 1, const int MaxObjectRange = 255 );

      /**
       * \author John Dill
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
      bool CheckObjectOverlap( const uiig::CImage<unsigned char> & InputImage, const int MinRange, const int MaxRange, const int MinObjectRange = 1, const int MaxObjectRange = 255 );

      /**
       * \author John Dill
       * \brief This function will write out an object with a background by looking up its name in the
       * objectmap.
       * \param ObjectName string of name of object to write out to a file
       * \param filename the filename to write the object
       */
      bool WriteObjectByName(const std::string & ObjectName, const std::string & filename);

      /**
       * \author John Dill
       * \brief This function will write out the objects in the form of separate objects
       * \param filename the base filename for all the objects, will append object tag and .obj
       * automatically
       */
      bool WriteDisplayedObjects(const std::string &filenamebase);

      /**
       * \author Hans J. Johnson
       * \brief This function calculates the bounding regions and center of the object
       */
      bool CalculateBoundingRegionAndCenter( void );

      /**
       * \author John Dill
       * \brief This function recalculates the number of shades allocated to each object to
       * evenly shade each object, since limit of Analyze shades is 255
       * \return int shades allocated per object
       */
      int EvenlyShade( void );

      /**
       * \author John Dill
       * \brief This function assigns one shade per object for displaying raw colors without
       * any shading.
       * \return none
       */
      void ConstShade( void );

      /**
       * \author John Dill
       * \brief This function is used to get the Analyze version of the object
       */
      int getVersion( void ) const;

      /**
       * \author John Dill
       * \brief This function is used to determine the number of objects in the Object map
       */
      int getNumberOfObjects( void ) const;

      /**
       * \author John Dill
       * \brief This function is used to determine if the object at the specific index is
       * to be shown
       * \param index
       */
      unsigned char isObjectShown( const unsigned char index ) const;

      /**
       * \author John Dill
       * \brief This function gets the Minimum pixel value of the Object to
       * be shown
       * \param index
       */
      unsigned char getMinimumPixelValue( const unsigned char index ) const;

      /**
       * \author John Dill
       * \brief This function is used to get Maximum pixel value of the object to
       * be shown
       * \param index
       */
      unsigned char getMaximumPixelValue( const unsigned char index ) const;

    /**
     * \author John Dill
     * \brief This function is used to set Minimum pixel value of the Object to be shown
     * \param index the object index
     * \param value the minimum value
     */
    void setMinimumPixelValue( const unsigned char index, const unsigned char value );   

    /**
     * \author John Dill
     * \brief This function is used to set Maximum pixel value of the Object to be shown
     * \param index the object index
     * \param value the maximum value
     */
    void setMaximumPixelValue( const unsigned char index, const unsigned char value );   

    private:
      bool CopyBaseImage( const CObjectMap& rhs );
      /**
       * Version of object file
       */
      int Version;

      /**
       * Number of Objects in the object file
       */
      int NumberOfObjects;

      /**
       * Pointers to individual objects in the object map, maximum of 256
       */
      CObjectEntry * ObjectArray[256];

      /**
       * Flag to determine the display of the layer
       */
      unsigned char ShowObject[256];

      /**
       * [ANALYZE only]
       */
      unsigned char MinimumPixelValue[256];

      /**
       * [ANALYZE only]
       */
      unsigned char MaximumPixelValue[256];

      /**
       * [ANALYZE only]
       */
      int NeedsSaving;

      /**
       * [ANALYZE only]
       */
      int NeedsRegionsCalculated;

  };
}                                // end of namespace uiig
#endif                           // __UIIG_OBJECTMAP_H_
