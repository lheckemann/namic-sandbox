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
#ifndef __OBJECTENTR_H__
#define __OBJECTENTR_H__

#include <string>
#include <vector>
#include <stdio.h>

//TODO:  USE GET/SET methods for member variables needs to be put together along with one set of
//       documentation that is syncronized syncronized.

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
  const int VERSION7 = 20050829;

  /**
   * Buffer size for reading in the run length encoded object data
   */
  const int BUFFERSIZE = 16384;

  /**
   * \class AnalyzeObjectEntry
   * \brief This class encapsulates a single object in an Analyze object file
   */
class AnalyzeObjectEntry
{
public:

  /**
   * \brief AnalyzeObjectEntry( void ) is the default constructor, initializes to 0 or NULL
   * \param none
   * \return none
   * Possible Causes of Failure:
   * - unknown
   */
  AnalyzeObjectEntry( void );

  /**
   * \brief AnalyzeObjectEntry( const AnalyzeObjectEntry & rhs ) is the copy constructor,
   * initializes to an existing object
   * \param const CObject & rhs
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   */
  AnalyzeObjectEntry( const AnalyzeObjectEntry & rhs );

  /**
   * \brief AnalyzeObjectEntry( Object & rhs ) is a constructor that creates an ObjectEntry
   * from an Object structure
   * \param const Object & rhs
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   * \sa Object
   */
  AnalyzeObjectEntry( const Object & rhs );

  /**
   * \brief ~AnalyzeObjectEntry( void ) is the destructor, which does nothing explicitly due to
   * no use of dynamic allocation
   * \param none
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   */
  ~AnalyzeObjectEntry( void );

  /**
   * \brief operator= is the assignment operator, which copies the data values on the
   * right hand side to the AnalyzeObjectEntry variable on the left
   * \param const AnalyzeObjectEntry & rhs
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   */
  AnalyzeObjectEntry & operator=( const AnalyzeObjectEntry & rhs );

  /**
   * \brief getNameASCII gets the tag name of the as a C character string
   * \param none
   * \return const char *
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  const char * getNameASCII( void ) const;


  /**
   * \brief getName/setName
   * The Name member of the AVW_Object structure contains a user
   * defined name for this object. Any zero-terminated string can be used,
   * including stringswith embedded spaces.
   */
  std::string getName( void ) const;
  bool setName( const std::string name );

  /**
   * \brief getDisplayFlag gets the Display Flag
   * \param none
   * \return DisplayFlag integer
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */

  /**
   * DisplayFlag is used to enable or disable the display of this
   * particular object. A value of zero value indicates
   * that voxels of this object type are ignored or assumed to be outside
   * the threshold range during the raycasting process.
   */
  int getDisplayFlag( void ) const;

  /**
   * \brief getCopyFlag gets the Copy Flag
   * \param none
   * \return CopyFlag unsigned char
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  unsigned char getCopyFlag( void ) const;

  /**
   * \brief getMirrorFlag gets the Mirror Flag
   * \param none
   * \return MirrorFlag unsigned char
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  unsigned char getMirrorFlag( void ) const;

  /**
   * \brief getStatusFlag gets the Status Flag
   * \param none
   * \return StatusFlag unsigned char
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  unsigned char getStatusFlag( void ) const;

  /**
   * \brief getNeighborsUsedFlag gets the Neighbors Used Flag
   * \param none
   * \return NeighborsUsedFlag unsigned char
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  unsigned char getNeighborsUsedFlag( void ) const;

  /**
   * \brief getShades gets Shades
   * \param none
   * \return Shades int, the number of shades for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getShades( void ) const;

  /**
   * \brief getStartRed gets StartRed
   * \param none
   * \return StartRed int, specifies the starting color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getStartRed( void ) const;

  /**
   * \brief getStartGreen gets StartGreen
   * \param none
   * \return StartGreen int, specifies the starting color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getStartGreen( void ) const;

  /**
   * \brief getStartBlue gets StartBlue
   * \param none
   * \return StartBlue int, specifies the starting color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getStartBlue( void ) const;

  /**
   * \brief getEndRed gets EndRed
   * \param none
   * \return EndRed int, specifies the ending color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getEndRed( void ) const;

  /**
   * \brief getEndGreen gets EndGreen
   * \param none
   * \return EndGreen int, specifies the ending color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getEndGreen( void ) const;

  /**
   * \brief getEndBlue gets EndBlue
   * \param none
   * \return EndBlue int, specifies the ending color for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getEndBlue( void ) const;

  /**
   * \brief getXRotation gets XRotation
   * \param none
   * \return getXRotation int, specifies the rotation for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getXRotation( void ) const;

  /**
   * \brief getYRotation gets YRotation
   * \param none
   * \return getYRotation int, specifies the rotation for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getYRotation( void ) const;

  /**
   * \brief getZRotation gets ZRotation
   * \param none
   * \return ZRotation int, specifies the rotation for the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getZRotation( void ) const;

  /**
   * \brief getXTranslation gets XTranslation
   * \param none
   * \return XTranslation int, specifies the translation of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getXTranslation( void ) const;

  /**
   * \brief getYTranslation gets YTranslation
   * \param none
   * \return YTranslation int, specifies the translation of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getYTranslation( void ) const;

  /**
   * \brief getZTranslation gets ZTranslation
   * \param none
   * \return ZTranslation int, specifies the translation of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getZTranslation( void ) const;

  /**
   * \brief getXCenter gets XCenter
   * \param none
   * \return XCenter int, specifies the rotation center
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getXCenter( void ) const;

  /**
   * \brief getYCenter gets YCenter
   * \param none
   * \return YCenter int, specifies the rotation center
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getYCenter( void ) const;

  /**
   * \brief getZCenter gets ZCenter
   * \param none
   * \return ZCenter int, specifies the rotation center
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getZCenter( void ) const;

  /**
   * \brief getXRotationIncrement gets XRotationIncrement
   * \param none
   * \return XRotationIncrement int, specifies the rotation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getXRotationIncrement( void ) const;

  /**
   * \brief getYRotationIncrement gets YRotationIncrement
   * \param none
   * \return YRotationIncrement int, specifies the rotation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getYRotationIncrement( void ) const;

  /**
   * \brief getZRotationIncrement gets ZRotationIncrement
   * \param none
   * \return ZRotationIncrement int, specifies the rotation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getZRotationIncrement( void ) const;

  /**
   * \brief getXTranslationIncrement gets XTranslationIncrement
   * \param none
   * \return XTranslationIncrement int, specifies the translation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getXTranslationIncrement( void ) const;

  /**
   * \brief getYTranslationIncrement gets YTranslationIncrement
   * \param none
   * \return YTranslationIncrement int, specifies the translation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getYTranslationIncrement( void ) const;

  /**
   * \brief getZTranslationIncrement gets ZTranslationIncrement
   * \param none
   * \return ZTranslationIncrement int, specifies the translation increment
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getZTranslationIncrement( void ) const;

  /**
   * \brief getMinimumXValue gets MinimumXValue
   * \param none
   * \return MinimumXValue short int, specifies the minimum X coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMinimumXValue( void ) const;

  /**
   * \brief getMinimumYValue gets MinimumYValue
   * \param none
   * \return MinimumYValue short int, specifies the minimum Y coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMinimumYValue( void ) const;

  /**
   * \brief getMinimumZValue gets MinimumZValue
   * \param none
   * \return MinimumZValue short int, specifies the minimum Z coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMinimumZValue( void ) const;

  /**
   * \brief getMaximumXValue gets MaximumXValue
   * \param none
   * \return MaximumXValue short int, specifies the maximum X coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMaximumXValue( void ) const;

  /**
   * \brief getMaximumYValue gets MaximumYValue
   * \param none
   * \return MaximumYValue short int, specifies the maximum Y coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMaximumYValue( void ) const;

  /**
   * \brief getMaximumZValue gets MaximumZValue
   * \param none
   * \return MaximumZValue short int, specifies the maximum Z coordinate of the enclosing brick of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  short int getMaximumZValue( void ) const;

  /**
   * \brief getOpacity gets Opacity
   * \param none
   * \return Opacity float, the current opacity setting of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  float getOpacity( void ) const;

  /**
   * \brief getOpacityThickness gets OpacityThickness
   * \param none
   * \return Opacity int, the current opacity thickness setting of the object
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getOpacityThickness( void ) const;

  /**
   * \brief getBlendFactor gets BlendFactor
   * \param none
   * \return BlendFactor int, room for expansion
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  int getBlendFactor( void ) const;

  /**
   * \brief setDisplayFlag sets DisplayFlag
   * \param int displayflag
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setDisplayFlag( const int displayflag );

  /**
   * \brief setCopyFlag sets CopyFlag
   * \param unsigned char copyflag
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setCopyFlag( const unsigned char copyflag );

  /**
   * \brief setMirrorFlag sets MirrorFlag
   * \param unsigned char mirrorflag
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setMirrorFlag( const unsigned char mirrorflag );

  /**
   * \brief setStatusFlag sets StatusFlag
   * \param unsigned char statusflag
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setStatusFlag( const unsigned char statusflag );

  /**
   * \brief setNeighborsUsedFlag sets NeighborsUsedFlag
   * \param unsigned char neighborsusedflag
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setNeighborsUsedFlag( const unsigned char neighborsusedflag );

  /**
   * \brief setShades sets Shades
   * \param int shades
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - parameter shades exceeds Analyze's limit on shades for an object (256 or 250 in Analyze)
   * \sa Object
   */
  bool setShades( const int shades );

  /**
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
   * \brief setStartRed sets StartRed for creating the colormap
   * \param int startred
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setStartRed( const int startred );

  /**
   * \brief setStartGreen sets StartGreen for creating the colormap
   * \param int startgreen
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setStartGreen( const int startgreen );

  /**
   * \brief setStartBlue sets StartBlue for creating the colormap
   * \param int startblue
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setStartBlue( const int startblue );

  /**
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
   * \brief setEndRed sets EndRed for creating the colormap
   * \param int endred
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setEndRed( const int endred );

  /**
   * \brief setEndGreen sets EndGreen for creating the colormap
   * \param int endgreen
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setEndGreen( const int endgreen );

  /**
   * \brief setEndBlue sets EndBlue for creating the colormap
   * \param int endblue
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setEndBlue( const int endblue );

  /**
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
   * \brief setCenter sets XCenter, for the center of the object
   * \param int xcenter
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setXCenter( const int xcenter );

  /**
   * \brief setCenter sets YCenter, for the center of the object
   * \param int ycenter
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setYCenter( const int ycenter );

  /**
   * \brief setCenter sets ZCenter, for the center of the object
   * \param int zcenter
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setZCenter( const int zcenter );

  /**
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
   * \brief setOpacity sets the current Opacity for the object
   * \param float opacity
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setOpacity( const float opacity );

  /**
   * \brief setOpacity sets the current OpacityThickness for the object
   * \param int opacitythickness
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setOpacityThickness( const int opacitythickness );

  /**
   * \brief setBlendFactor sets the dummy variable for the object, used for future expansion
   * \param int dummy
   * \return bool, true - success, false - failure
   * Possible Causes of Failure:
   * - unknown
   * \sa Object
   */
  bool setBlendFactor( const int dummy );

private:
  char m_Name[32];
  int m_DisplayFlag;

  /**
   * CopyFlag indicates object translation, rotation, and mirroring are
   * applied to a copy of the actual object, rather
   * than the object itself. [ANALYZE only]
   */
  unsigned char m_CopyFlag;

  /**
   * MirrorFlag indicates the axis this object is mirrored around.
   * [ANALYZE only]
   */
  unsigned char m_MirrorFlag;

  /**
   * StatusFlag is used to indicate when an object has changed and may
   * need it's minimum bounding box recomputed. [ANALYZE only]
   */
  unsigned char m_StatusFlag;

  /**
   * NeighborsUsedFlag indicates which neighboring voxels are used in
   * calculating the objects shading. [ANALYZE only]
   */
  unsigned char m_NeighborsUsedFlag;

  /**
   * Shades indicates the number of shades available for this object.
   * Only 256 (250 in ANALYZE) total shades are available.
   */
  int m_Shades;

  /**
   * StartRed specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  int m_StartRed;

  /**
   * StartGreen specify the starting color for this object. This is usually
   * a darker shade of the ending color.  ANALYZE defaults these values to
   * 10% of the ending color.
   */
  int m_StartGreen;

  /**
   * StartBlue specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  int m_StartBlue;

  /**
   * EndRed specify the ending color for this object.
   */
  int m_EndRed;

  /**
   * EndGreen specify the ending color for this object.
   */
  int m_EndGreen;

  /**
   * EndBlue specify the ending color for this object.
   */
  int m_EndBlue;

  /**
   * XRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */

  int m_XRotation;

  /**
   * YRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  int m_YRotation;

  /**
   * ZRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  int m_ZRotation;

  /**
   * XTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int m_XTranslation;

  /**
   * YTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int m_YTranslation;

  /**
   * ZTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int m_ZTranslation;

  /**
   * XCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int m_XCenter;

  /**
   * YCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int m_YCenter;

  /**
   * ZCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int m_ZCenter;

  /**
   * XRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  int m_XRotationIncrement;

  /**
   * YRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  int m_YRotationIncrement;

  /**
   * ZRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  int m_ZRotationIncrement;

  /**
   * XTranslationIncrement specify increments that are applies to
   * XTranslation, YTranslation, and ZTranslation when making a sequence.
   * [ANALYZE only]
   */
  int m_XTranslationIncrement;

  /**
   * YTranslationIncrement specify increments that are applies to
   * XTranslation, YTranslation, and ZTranslation when making a sequence.
   * [ANALYZE only]
   */
  int m_YTranslationIncrement;

  /**
   * ZTranslationIncrement specify increments that are applies to
   * XTranslation, YTranslation, and ZTranslation when making a sequence.
   * [ANALYZE only]
   */
  int m_ZTranslationIncrement;

  /**
   * MinimumXValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MinimumXValue;

  /**
   * MinimumYValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MinimumYValue;

  /**
   * MinimumZValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MinimumZValue;

  /**
   * MaximumXValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MaximumXValue;

  /**
   * MaximumYValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MaximumYValue;

  /**
   * MaximumZValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int m_MaximumZValue;

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
  float m_Opacity;

  /**
   * The thickness of the object
   */
  int m_OpacityThickness;
  /**
   * Determines the amount of object color verses
   * composite color. A value of 1.0, causes all the
   * color to come from the object. A value of 0.0
   * causes all the color to come from the alpha map.
   * A value of .5 will cause half to come from each.
   */
  float m_BlendFactor;
};
#endif                           // __OBJECTENTR_H__
