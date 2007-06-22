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
#ifndef __UIIG_OBJECTENTRY_H_
#define __UIIG_OBJECTENTRY_H_

//
// Name: ObjectEntry.h
// Author: John Dill
// Date: 5/16/00
// Classes:
// CObjectEntry - Represents a single object for the ObjectMap
//

#include <string>
#include <vector>
#include <stdio.h>
#include <uiigcore/imageflat.h>
#include <uiigfileio/fileio.h>

namespace uiig
{
  /**
   * \struct Object
   * \brief This object holds the header information from the object file
   */
  struct __Object
  {
    /**
     * The Name member of the AVW_Object structure contains a user
     * defined name for this object. Any zero-terminated string can be used,
     * including stringswith embedded spaces.
     */
    char Name[32];

    /**
     * DisplayFlag is used to enable or disable the display of this
     * particular object. A value of zero value indicates
     * that voxels of this object type are ignored or assumed to be outside
     * the threshold range during the raycasting process.
     */
    int DisplayFlag;

    /**
     * CopyFlag indicates object translation, rotation, and mirroring are
     * applied to a copy of the actual object, rather
     * than the object itself. [ANALYZE only]
     */
    unsigned char CopyFlag;

    /**
     * MirrorFlag indicates the axis this object is mirrored around.
     * [ANALYZE only]
     */
    unsigned char MirrorFlag;

    /**
     * StatusFlag is used to indicate when an object has changed and may
     * need it's minimum bounding box recomputed. [ANALYZE only]
     */
    unsigned char StatusFlag;

    /**
     * NeighborsUsedFlag indicates which neighboring voxels are used in
     * calculating the objects shading. [ANALYZE only]
     */
    unsigned char NeighborsUsedFlag;

    /**
     * Shades indicates the number of shades available for this object.
     * Only 256 (250 in ANALYZE) total shades are available.
     */
    int Shades;

    /**
     * StartRed specify the starting color for this object. This is usually a
     * darker shade of the ending color. ANALYZE defaults these values to 10%
     * of the ending color.
     */
    int StartRed;

    /**
     * StartGreen specify the starting color for this object. This is usually
     * a darker shade of the ending color.  ANALYZE defaults these values to
     * 10% of the ending color.
     */
    int StartGreen;

    /**
     * StartBlue specify the starting color for this object. This is usually a
     * darker shade of the ending color. ANALYZE defaults these values to 10%
     * of the ending color.
     */
    int StartBlue;

    /**
     * EndRed specify the ending color for this object.
     */
    int EndRed;

    /**
     * EndGreen specify the ending color for this object.
     */
    int EndGreen;

    /**
     * EndBlue specify the ending color for this object.
     */
    int EndBlue;

    /**
     * XRotation specify a rotation which is applied to this object only.
     * [ANALYZE only]
     */

    int XRotation;

    /**
     * YRotation specify a rotation which is applied to this object only.
     * [ANALYZE only]
     */
    int YRotation;

    /**
     * ZRotation specify a rotation which is applied to this object only.
     * [ANALYZE only]
     */
    int ZRotation;

    /**
     * XTranslation specify a translation which is applied to this object only.
     * [ANALYZE only]
     */
    int XTranslation;

    /**
     * YTranslation specify a translation which is applied to this object only.
     * [ANALYZE only]
     */
    int YTranslation;

    /**
     * ZTranslation specify a translation which is applied to this object only.
     * [ANALYZE only]
     */
    int ZTranslation;

    /**
     * XCenter specify the rotation center, relative to the volumes center,
     * which the XRotation, YRotation, and ZRotation are rotated around.
     * [ANALYZE only]
     */
    int XCenter;

    /**
     * YCenter specify the rotation center, relative to the volumes center,
     * which the XRotation, YRotation, and ZRotation are rotated around.
     * [ANALYZE only]
     */
    int YCenter;

    /**
     * ZCenter specify the rotation center, relative to the volumes center,
     * which the XRotation, YRotation, and ZRotation are rotated around.
     * [ANALYZE only]
     */
    int ZCenter;

    /**
     * XRotationIncrement specify increments that are applies to XRotation,
     * YRotation, and ZRotation when making a sequence. [ANALYZE only]
     */
    int XRotationIncrement;

    /**
     * YRotationIncrement specify increments that are applies to XRotation,
     * YRotation, and ZRotation when making a sequence. [ANALYZE only]
     */
    int YRotationIncrement;

    /**
     * ZRotationIncrement specify increments that are applies to XRotation,
     * YRotation, and ZRotation when making a sequence. [ANALYZE only]
     */
    int ZRotationIncrement;

    /**
     * XTranslationIncrement specify increments that are applies to
     * XTranslation, YTranslation, and ZTranslation when making a sequence.
     * [ANALYZE only]
     */
    int XTranslationIncrement;

    /**
     * YTranslationIncrement specify increments that are applies to
     * XTranslation, YTranslation, and ZTranslation when making a sequence.
     * [ANALYZE only]
     */
    int YTranslationIncrement;

    /**
     * ZTranslationIncrement specify increments that are applies to
     * XTranslation, YTranslation, and ZTranslation when making a sequence.
     * [ANALYZE only]
     */
    int ZTranslationIncrement;

    /**
     * MinimumXValue specify the minimum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MinimumXValue;

    /**
     * MinimumYValue specify the minimum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MinimumYValue;

    /**
     * MinimumZValue specify the minimum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MinimumZValue;

    /**
     * MaximumXValue specify the maximum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MaximumXValue;

    /**
     * MaximumYValue specify the maximum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MaximumYValue;

    /**
     * MaximumZValue specify the maximum enclosing brick used by ANALYZE to
     * increase the rendering speed of individual objects during object
     * translations and rotations. [ANALYZE only]
     */
    short int MaximumZValue;

    /**
     * Opacity and OpacityThickness are used only when rendering 24-bit
     * transparency images. Opacity is a floating point value between .0001
     * (very transparent) and 1.0 (opaque). The Opacity value is multiplied
     * times the color of each surface voxel intersected, it is also
     * subtracted from 1.0 to determine the amount of shading which can be
     * applied by objects intersected after this object. Ray-casting continues
     * as long as it is possible for remaining objects along the ray path to
     * make contributions. The OpacityThickness parameter allows the
     * surface determined color, to be added in multiple times for thicker
     * objects. This allows the thickness of each object to make a
     * contribution into what can be seen behind it. A value of 1 for
     * OpacityThickness results in only the surface of each object having a
     * contribution.
     */
    float Opacity;
    int OpacityThickness;

    /**
     * Dummy is not used, but included for possible future expansion.
     */
    int Dummy;

  };

  typedef struct __Object Object;


  /**
   * Constant defining the maximum number of shades possible for an object as of Version 6
   */
  const int MAXANALYZESHADES = 250;

  /**
   * Constants representing the current version number of the object map file for Analyze
   */
  const int VERSION1 = 880102;
  const int VERSION2 = 880801;
  const int VERSION3 = 890102;
  const int VERSION4 = 900302;
  const int VERSION5 = 910402;
  const int VERSION6 = 910926;
  const int VERSION8 = 20050829;

  /**
   * Buffer size for reading in the run length encoded object data
   */
  const int BUFFERSIZE = 16384;

  /**
   * \class CObjectEntry
   * \brief This class encapsulates a single object in an Analyze object file
   */
  class CObjectEntry
  {
    public:

      /**
       * \author John Dill
       * \brief CObjectEntry( void ) is the default constructor, initializes to 0 or NULL
       * \param none
       * \return none
       * Possible Causes of Failure:
       * - unknown
       */
      CObjectEntry( void );

      /**
       * \author John Dill
       * \brief CObjectEntry( const CObjectEntry & rhs ) is the copy constructor,
       * initializes to an existing object
       * \param const CObject & rhs
       * \return none
       * Possible Causes of Failure:
       * - unknown
       * \sa CObjectEntry
       */
      CObjectEntry( const CObjectEntry & rhs );

      /**
       * \author John Dill
       * \brief CObjectEntry( Object & rhs ) is a constructor that creates an ObjectEntry
       * from an Object structure
       * \param const Object & rhs
       * \return none
       * Possible Causes of Failure:
       * - unknown
       * \sa CObjectEntry
       * \sa Object
       */
      CObjectEntry( const Object & rhs );

      /**
       * \author John Dill
       * \brief ~CObjectEntry( void ) is the destructor, which does nothing explicitly due to
       * no use of dynamic allocation
       * \param none
       * \return none
       * Possible Causes of Failure:
       * - unknown
       * \sa CObjectEntry
       */
      ~CObjectEntry( void );

      /**
       * \author John Dill
       * \brief operator= is the assignment operator, which copies the data values on the
       * right hand side to the CObjectEntry variable on the left
       * \param const CObjectEntry & rhs
       * \return none
       * Possible Causes of Failure:
       * - unknown
       * \sa CObjectEntry
       */
      CObjectEntry & operator=( const CObjectEntry & rhs );

      /**
       * \author John Dill
       * \brief getName gets the tag name of the object
       * \param none
       * \return std::string
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      std::string getName( void ) const;

      /**
       * \author John Dill
       * \brief getNameASCII gets the tag name of the as a C character string
       * \param none
       * \return const char *
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      const char * getNameASCII( void ) const;

      /**
       * \author John Dill
       * \brief getDisplayFlag gets the Display Flag
       * \param none
       * \return DisplayFlag integer
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getDisplayFlag( void ) const;

      /**
       * \author John Dill
       * \brief getCopyFlag gets the Copy Flag
       * \param none
       * \return CopyFlag unsigned char
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      unsigned char getCopyFlag( void ) const;

      /**
       * \author John Dill
       * \brief getMirrorFlag gets the Mirror Flag
       * \param none
       * \return MirrorFlag unsigned char
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      unsigned char getMirrorFlag( void ) const;

      /**
       * \author John Dill
       * \brief getStatusFlag gets the Status Flag
       * \param none
       * \return StatusFlag unsigned char
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      unsigned char getStatusFlag( void ) const;

      /**
       * \author John Dill
       * \brief getNeighborsUsedFlag gets the Neighbors Used Flag
       * \param none
       * \return NeighborsUsedFlag unsigned char
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      unsigned char getNeighborsUsedFlag( void ) const;

      /**
       * \author John Dill
       * \brief getShades gets Shades
       * \param none
       * \return Shades int, the number of shades for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getShades( void ) const;

      /**
       * \author John Dill
       * \brief getStartRed gets StartRed
       * \param none
       * \return StartRed int, specifies the starting color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getStartRed( void ) const;

      /**
       * \author John Dill
       * \brief getStartGreen gets StartGreen
       * \param none
       * \return StartGreen int, specifies the starting color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getStartGreen( void ) const;

      /**
       * \author John Dill
       * \brief getStartBlue gets StartBlue
       * \param none
       * \return StartBlue int, specifies the starting color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getStartBlue( void ) const;

      /**
       * \author John Dill
       * \brief getEndRed gets EndRed
       * \param none
       * \return EndRed int, specifies the ending color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getEndRed( void ) const;

      /**
       * \author John Dill
       * \brief getEndGreen gets EndGreen
       * \param none
       * \return EndGreen int, specifies the ending color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getEndGreen( void ) const;

      /**
       * \author John Dill
       * \brief getEndBlue gets EndBlue
       * \param none
       * \return EndBlue int, specifies the ending color for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getEndBlue( void ) const;

      /**
       * \author John Dill
       * \brief getXRotation gets XRotation
       * \param none
       * \return getXRotation int, specifies the rotation for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getXRotation( void ) const;

      /**
       * \author John Dill
       * \brief getYRotation gets YRotation
       * \param none
       * \return getYRotation int, specifies the rotation for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getYRotation( void ) const;

      /**
       * \author John Dill
       * \brief getZRotation gets ZRotation
       * \param none
       * \return ZRotation int, specifies the rotation for the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getZRotation( void ) const;

      /**
       * \author John Dill
       * \brief getXTranslation gets XTranslation
       * \param none
       * \return XTranslation int, specifies the translation of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getXTranslation( void ) const;

      /**
       * \author John Dill
       * \brief getYTranslation gets YTranslation
       * \param none
       * \return YTranslation int, specifies the translation of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getYTranslation( void ) const;

      /**
       * \author John Dill
       * \brief getZTranslation gets ZTranslation
       * \param none
       * \return ZTranslation int, specifies the translation of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getZTranslation( void ) const;

      /**
       * \author John Dill
       * \brief getXCenter gets XCenter
       * \param none
       * \return XCenter int, specifies the rotation center
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getXCenter( void ) const;

      /**
       * \author John Dill
       * \brief getYCenter gets YCenter
       * \param none
       * \return YCenter int, specifies the rotation center
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getYCenter( void ) const;

      /**
       * \author John Dill
       * \brief getZCenter gets ZCenter
       * \param none
       * \return ZCenter int, specifies the rotation center
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getZCenter( void ) const;

      /**
       * \author John Dill
       * \brief getXRotationIncrement gets XRotationIncrement
       * \param none
       * \return XRotationIncrement int, specifies the rotation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getXRotationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getYRotationIncrement gets YRotationIncrement
       * \param none
       * \return YRotationIncrement int, specifies the rotation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getYRotationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getZRotationIncrement gets ZRotationIncrement
       * \param none
       * \return ZRotationIncrement int, specifies the rotation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getZRotationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getXTranslationIncrement gets XTranslationIncrement
       * \param none
       * \return XTranslationIncrement int, specifies the translation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getXTranslationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getYTranslationIncrement gets YTranslationIncrement
       * \param none
       * \return YTranslationIncrement int, specifies the translation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getYTranslationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getZTranslationIncrement gets ZTranslationIncrement
       * \param none
       * \return ZTranslationIncrement int, specifies the translation increment
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getZTranslationIncrement( void ) const;

      /**
       * \author John Dill
       * \brief getMinimumXValue gets MinimumXValue
       * \param none
       * \return MinimumXValue short int, specifies the minimum X coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMinimumXValue( void ) const;

      /**
       * \author John Dill
       * \brief getMinimumYValue gets MinimumYValue
       * \param none
       * \return MinimumYValue short int, specifies the minimum Y coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMinimumYValue( void ) const;

      /**
       * \author John Dill
       * \brief getMinimumZValue gets MinimumZValue
       * \param none
       * \return MinimumZValue short int, specifies the minimum Z coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMinimumZValue( void ) const;

      /**
       * \author John Dill
       * \brief getMaximumXValue gets MaximumXValue
       * \param none
       * \return MaximumXValue short int, specifies the maximum X coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMaximumXValue( void ) const;

      /**
       * \author John Dill
       * \brief getMaximumYValue gets MaximumYValue
       * \param none
       * \return MaximumYValue short int, specifies the maximum Y coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMaximumYValue( void ) const;

      /**
       * \author John Dill
       * \brief getMaximumZValue gets MaximumZValue
       * \param none
       * \return MaximumZValue short int, specifies the maximum Z coordinate of the enclosing brick of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      short int getMaximumZValue( void ) const;

      /**
       * \author John Dill
       * \brief getOpacity gets Opacity
       * \param none
       * \return Opacity float, the current opacity setting of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      float getOpacity( void ) const;

      /**
       * \author John Dill
       * \brief getOpacityThickness gets OpacityThickness
       * \param none
       * \return Opacity int, the current opacity thickness setting of the object
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getOpacityThickness( void ) const;

      /**
       * \author John Dill
       * \brief getDummy gets Dummy
       * \param none
       * \return Dummy int, room for expansion
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      int getDummy( void ) const;

      /**
       * \author John Dill
       * \brief getObjectPointer returns a handle to the Object structure encapsulated in the CObjectEntry class
       * \param none
       * \return myObject Object *
       * Possible Causes of Failure:
       * - unknown
       * \sa ObjectEntry
       */
      Object * getObjectPointer(void);

      /**
       * \author John Dill
       * \brief SetName sets Name
       * \param std::string name
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - std::string name is longer than the data structure name in Object
       * \sa Object
       */
      bool setName( const std::string name );

      /**
       * \author John Dill
       * \brief setDisplayFlag sets DisplayFlag
       * \param int displayflag
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setDisplayFlag( const int displayflag );

      /**
       * \author John Dill
       * \brief setCopyFlag sets CopyFlag
       * \param unsigned char copyflag
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setCopyFlag( const unsigned char copyflag );

      /**
       * \author John Dill
       * \brief setMirrorFlag sets MirrorFlag
       * \param unsigned char mirrorflag
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMirrorFlag( const unsigned char mirrorflag );

      /**
       * \author John Dill
       * \brief setStatusFlag sets StatusFlag
       * \param unsigned char statusflag
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setStatusFlag( const unsigned char statusflag );

      /**
       * \author John Dill
       * \brief setNeighborsUsedFlag sets NeighborsUsedFlag
       * \param unsigned char neighborsusedflag
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setNeighborsUsedFlag( const unsigned char neighborsusedflag );

      /**
       * \author John Dill
       * \brief setShades sets Shades
       * \param int shades
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - parameter shades exceeds Analyze's limit on shades for an object (256 or 250 in Analyze)
       * \sa Object
       */
      bool setShades( const int shades );

      /**
       * \author John Dill
       * \brief setStartColor sets StartRed, StartGreen, StartBlue for creating the colormap
       * \param int startred
       * \param int startgreen
       * \param int startblue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setStartColor( const int startred, const int startgreen, const int startblue );

      /**
       * \author John Dill
       * \brief setStartRed sets StartRed for creating the colormap
       * \param int startred
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setStartRed( const int startred );

      /**
       * \author John Dill
       * \brief setStartGreen sets StartGreen for creating the colormap
       * \param int startgreen
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setStartGreen( const int startgreen );

      /**
       * \author John Dill
       * \brief setStartBlue sets StartBlue for creating the colormap
       * \param int startblue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setStartBlue( const int startblue );

      /**
       * \author John Dill
       * \brief setEndColor sets EndRed, EndGreen, EndBlue for creating the colormap
       * \param int endred
       * \param int endgreen
       * \param int endblue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setEndColor( const int endred, const int endgreen, const int endblue );

      /**
       * \author John Dill
       * \brief setEndRed sets EndRed for creating the colormap
       * \param int endred
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setEndRed( const int endred );

      /**
       * \author John Dill
       * \brief setEndGreen sets EndGreen for creating the colormap
       * \param int endgreen
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setEndGreen( const int endgreen );

      /**
       * \author John Dill
       * \brief setEndBlue sets EndBlue for creating the colormap
       * \param int endblue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setEndBlue( const int endblue );

      /**
       * \author John Dill
       * \brief setRotation sets XRotation, YRotation, ZRotation for the rotation of the object
       * \param int xrotation
       * \param int yrotation
       * \param int zrotation
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setRotation( const int xrotation, const int yrotation, const int zrotation );

      /**
       * \author John Dill
       * \brief setTranslation sets XTranslation, YTranslation, ZTranslation for the translation of the object
       * \param int xtranslation
       * \param int ytranslation
       * \param int ztranslation
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setTranslation( const int xtranslation, const int ytranslation, const int ztranslation );

      /**
       * \author John Dill
       * \brief setCenter sets XCenter, YCenter, ZCenter for the center of the object
       * \param int xcenter
       * \param int ycenter
       * \param int zcenter
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setCenter( const int xcenter, const int ycenter, const int zcenter );

      /**
       * \author John Dill
       * \brief setCenter sets XCenter, for the center of the object
       * \param int xcenter
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setXCenter( const int xcenter );

      /**
       * \author John Dill
       * \brief setCenter sets YCenter, for the center of the object
       * \param int ycenter
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setYCenter( const int ycenter );

      /**
       * \author John Dill
       * \brief setCenter sets ZCenter, for the center of the object
       * \param int zcenter
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setZCenter( const int zcenter );

      /**
       * \author John Dill
       * \brief setRotationIncrement sets XRotationIncrement, YRotationIncrement, ZRotationIncrement
       * for the rotation increment of the object
       * \param int xrotationincrement
       * \param int yrotationincrement
       * \param int zrotationincrement
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setRotationIncrement( const int xrotationincrement,
        const int yrotationincrement,
        const int zrotationincrement );

      /**
       * \author John Dill
       * \brief setTranslationIncrement sets XTranslationIncrement,
       * YTranslationIncrement, ZTranslationIncrement
       * for the translation increment of the object
       * \param int xtranslationincrement
       * \param int ytranslationincrement
       * \param int ztranslationincrement
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setTranslationIncrement( const int xtranslationincrement,
        const int ytranslationincrement,
        const int ztranslationincrement );

      /**
       * \author John Dill
       * \brief setMinimumCoordinate sets MinimumXValue, MinimumYValue, MinimumZValue which represents the
       * minimum coordinate of the bounding brick of the object used for rendering by Analyze
       * \param int minimumxvalue
       * \param int minimumyvalue
       * \param int minimumzvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMinimumCoordinate( const int minimumxvalue, const int minimumyvalue, const int minimumzvalue );

      /**
       * \author John Dill
       * \brief setMinimumXValue sets MinimumXValue, the minimum x coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int minimumxvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMinimumXValue( const int minimumxvalue );

      /**
       * \author John Dill
       * \brief setMinimumYValue sets MinimumYValue, the minimum y coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int minimumyvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMinimumYValue( const int minimumyvalue );

      /**
       * \author John Dill
       * \brief setMinimumZValue sets MinimumZValue, the minimum z coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int minimumzvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMinimumZValue( const int minimumzvalue );

      /**
       * \author John Dill
       * \brief setMaximumCoordinate sets MaximumXValue,
       * MaximumYValue, MaximumZValue which represents the
       * maximum coordinate of the bounding brick of the object used for rendering by Analyze
       * \param int maximumxvalue
       * \param int maximumyvalue
       * \param int maximumzvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMaximumCoordinate( const int maximumxvalue, const int maximumyvalue, const int maximumzvalue );

      /**
       * \author John Dill
       * \brief setMaximumXValue sets MaximumXValue, the maximum x coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int maximumxvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMaximumXValue( const int maximumxvalue );

      /**
       * \author John Dill
       * \brief setMaximumYValue sets MaximumYValue, the maximum y coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int maximumyvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMaximumYValue( const int maximumyvalue );

      /**
       * \author John Dill
       * \brief setMaximumZValue sets MaximumZValue, the maximum z coordinate of the bounding brick of
       * the object used for rendering by Analyze
       * \param int maximumzvalue
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setMaximumZValue( const int maximumzvalue );

      /**
       * \author John Dill
       * \brief setOpacity sets the current Opacity for the object
       * \param float opacity
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setOpacity( const float opacity );

      /**
       * \author John Dill
       * \brief setOpacity sets the current OpacityThickness for the object
       * \param int opacitythickness
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setOpacityThickness( const int opacitythickness );

      /**
       * \author John Dill
       * \brief setDummy sets the dummy variable for the object, used for future expansion
       * \param int dummy
       * \return bool, true - success, false - failure
       * Possible Causes of Failure:
       * - unknown
       * \sa Object
       */
      bool setDummy( const int dummy );

    private:
      /**
       * Encapsulated Object structure in this class CObjectEntry
       */
      Object myObject;
  };
}                                // end of namespace uiig
#endif                           // __UIIG_OBJECTENTRY_H_
