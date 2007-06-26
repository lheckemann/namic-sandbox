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
//#include <vector>
#include <stdio.h>
#include <itkDataObject.h>

// TODO:  Add namespace  namespace itk {
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
  //TODO:  derive from itk::DataObject to get these to be formal itk pipeline managable objects.
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
   * \brief getDisplayFlag/setDisplayFlag
   * DisplayFlag is used to enable or disable the display of this
   * particular object. A value of zero value indicates
   * that voxels of this object type are ignored or assumed to be outside
   * the threshold range during the raycasting process.
   */
  int getDisplayFlag( void ) const;
  bool setDisplayFlag( const int displayflag );

  /**
   * \brief getCopyFlag gets the Copy Flag
   *
   * CopyFlag indicates object translation, rotation, and mirroring are
   * applied to a copy of the actual object, rather
   * than the object itself. [ANALYZE only]
   */
  unsigned char getCopyFlag( void ) const;
  bool setCopyFlag( const unsigned char copyflag );


  /**
   * \brief getMirrorFlag/setMirrorFlag
   *
   * MirrorFlag indicates the axis this object is mirrored around.
   * [ANALYZE only]
   */
  unsigned char getMirrorFlag( void ) const;
  bool setMirrorFlag( const unsigned char mirrorflag );

  /**
   * \brief getStatusFlag/setStatusFlag
   *
   * StatusFlag is used to indicate when an object has changed and may
   * need it's minimum bounding box recomputed. [ANALYZE only]
   */
  unsigned char getStatusFlag( void ) const;
  bool setStatusFlag( const unsigned char statusflag );

  /**
   * \brief getNeighborsUsedFlag/setNeighborsUsedFlag
   *
   * NeighborsUsedFlag indicates which neighboring voxels are used in
   * calculating the objects shading. [ANALYZE only]
   */
  unsigned char getNeighborsUsedFlag( void ) const;
  bool setNeighborsUsedFlag( const unsigned char neighborsusedflag );

  /**
   * \brief getShades/setShades
   *
   * Shades indicates the number of shades available for this object.
   * Only 256 (250 in ANALYZE) total shades are available.
   */
  int getShades( void ) const;
  bool setShades( const int shades );

  /**
   * \brief getStartRed/setStartRed
   *
   * StartRed specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  int getStartRed( void ) const;
  bool setStartRed( const int startred );

  /**
   * \brief getStartGreen/setStartGreen
   *
   * StartGreen specify the starting color for this object. This is usually
   * a darker shade of the ending color.  ANALYZE defaults these values to
   * 10% of the ending color.
   */
  int getStartGreen( void ) const;
  bool setStartGreen( const int startgreen );

  /**
   * \brief getStartBlue/setStartBlue
   *
   * StartBlue specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  int getStartBlue( void ) const;
  bool setStartBlue( const int startblue );

  /**
   * \brief getEndRed/setEndRed
   *
   * EndRed specify the ending color for this object.
   */
  int getEndRed( void ) const;
  bool setEndRed( const int endred );

  /**
   * \brief getEndGreen/setEndGreen
   *
   * EndGreen specify the ending color for this object.
   */
  int getEndGreen( void ) const;
  bool setEndGreen( const int endgreen );

  /**
   * \brief getEndBlue/setEndBlue
   *
   * EndBlue specify the ending color for this object.
   */
  int getEndBlue( void ) const;
  bool setEndBlue( const int endblue );

  /**
   * \brief getXRotation
   *
   * XRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  int getXRotation( void ) const;

  /**
   *\brief getXRotationIncrement
   *
   * XRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  int getXRotationIncrement( void ) const;

  /**
   * \brief getYRotation
   *
   * YRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  int getYRotation( void ) const;

  /**
   *\brief getYRotationIncrement
   *
   * YRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  int getYRotationIncrement( void ) const;

  /**
   * \brief getZRotation
   *
   * ZRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  int getZRotation( void ) const;

   /**
    *\brief getZRotationIncrement
    *
    * ZRotationIncrement specify increments that are applies to XRotation,
    * YRotation, and ZRotation when making a sequence. [ANALYZE only]
    */
  int getZRotationIncrement( void ) const;

  /**
   * \brief getXTranslation
   *
   * XTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int getXTranslation( void ) const;

   /**
    \*brief getXTranslation
    *
    * XTranslationIncrement specify increments that are applies to
    * XTranslation, YTranslation, and ZTranslation when making a sequence.
    * [ANALYZE only]
    */
  int getXTranslationIncrement( void ) const;

  /**
   * \brief getYTranslation
   *
   * YTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int getYTranslation( void ) const;

   /**
    *\brief getYTranslationIncrement
    *
    * YTranslationIncrement specify increments that are applies to
    * XTranslation, YTranslation, and ZTranslation when making a sequence.
    * [ANALYZE only]
    */
  int getYTranslationIncrement( void ) const;

  /**
   * \brief getZTranslation
   *
   * ZTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  int getZTranslation( void ) const;
  
  /**
   *\brief getZTranslation
   *
   * ZTranslationIncrement specify increments that are applies to
   * XTranslation, YTranslation, and ZTranslation when making a sequence.
   * [ANALYZE only]
   */
  int getZTranslationIncrement( void ) const;

  /**
   * \brief getXCenter/setXCenter
   *
   * XCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int getXCenter( void ) const;
  bool setXCenter( const int xcenter );

  /**
   * \brief getYCenter/setYCenter
   *
   * YCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int getYCenter( void ) const;
  bool setYCenter( const int ycenter );

  /**
   * \brief getZCenter/setZCenter
   *
   * ZCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  int getZCenter( void ) const;
  bool setZCenter( const int zcenter );


  /**
   * \brief getMinimumXValue/setMinimumXValue
   *
   * MinimumXValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMinimumXValue( void ) const;
  bool setMinimumXValue( const int minimumxvalue );

  /**
   * \brief getMinimumYValue/setMinimumYValue
   *
   * MinimumYValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMinimumYValue( void ) const;
  bool setMinimumYValue( const int minimumyvalue );

  /**
   * \brief getMinimumZValue/setMinimumZValue
   *
   * MinimumZValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMinimumZValue( void ) const;
  bool setMinimumZValue( const int minimumzvalue );

  /**
   * \brief getMaximumXValue/setMaximumXValue
   *
   * MaximumXValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMaximumXValue( void ) const;
  bool setMaximumXValue( const int maximumxvalue );

  /**
   * \brief getMaximumYValue/setMaximumYValue
   *
   * MaximumYValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMaximumYValue( void ) const;
  bool setMaximumYValue( const int maximumyvalue );

  /**
   * \brief getMaximumZValue/setMaximumZValue
   *
   * MaximumZValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  short int getMaximumZValue( void ) const;
  bool setMaximumZValue( const int maximumzvalue );

  /**
   * \brief getOpacity/setOpacity
   *
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
  float getOpacity( void ) const;
  bool setOpacity( const float opacity );

  /**
   * \brief getOpacityThickness/setOpacityThickness
   *
   * The thickness of the object
   */
  int getOpacityThickness( void ) const;
  bool setOpacityThickness( const int opacitythickness );

  /**
   * \brief getBlendFactor/setBlendFactor
   *
   * Determines the amount of object color verses
   * composite color. A value of 1.0, causes all the
   * color to come from the object. A value of 0.0
   * causes all the color to come from the alpha map.
   * A value of .5 will cause half to come from each.
   */
  int getBlendFactor( void ) const;
  bool setBlendFactor( const int dummy );
  

  /**
   * \brief setStartColor
   *
   * Set the starting colors (red, green, blue) for creating the colormap.
   */
  bool setStartColor( const int startred, const int startgreen, const int startblue );


  /**
   * \brief setEndColor
   *
   * Set the ending colors (red, green, blue) for creating the colormap.
   */
  bool setEndColor( const int endred, const int endgreen, const int endblue );


  /**
   * \brief setRotation
   *
   * Set the rotation of the object (xRotation, yRotation, zRotation).
   */
  bool setRotation( const int xrotation, const int yrotation, const int zrotation );

  /**
   * \brief setTranslation
   * 
   * Set the translation of the object (xTranslation, yTranslation, zTranslation).
   */
  bool setTranslation( const int xtranslation, const int ytranslation, const int ztranslation );

  /**
   * \brief setCenter
   *
   * Set the center of the object (xCenter, yCenter, zCenter).
   */
  bool setCenter( const int xcenter, const int ycenter, const int zcenter );


  /**
   * \brief setRotationIncrement
   *
   * Set the rotation increment (xRotationIncrement, yRotationIncrement, zRotationIncrement).
   */
  bool setRotationIncrement( const int xrotationincrement,
    const int yrotationincrement,
    const int zrotationincrement );

  /**
   * \brief setTranslationIncrement
   * 
   * Set the traslation increment of the object (xTranslationIncrement, yTranslationIncrement, zTranslatoinIncrement).
   */
  bool setTranslationIncrement( const int xtranslationincrement,
    const int ytranslationincrement,
    const int ztranslationincrement );

  /**
   * \brief setMinimumCoordinate
   *
   * Set the minimum coordinate of the bounding brick of the object used for rendering by Analyze
   */
  bool setMinimumCoordinate( const int minimumxvalue, const int minimumyvalue, const int minimumzvalue );


  /**
   * \brief setMaximumCoordinate
   * 
   * Set the maximum coordinate of the bounding brick of the object used for rendering by Analyze
   */
  bool setMaximumCoordinate( const int maximumxvalue, const int maximumyvalue, const int maximumzvalue );


private:
  char m_Name[32];
  int m_DisplayFlag;
  unsigned char m_CopyFlag;
  unsigned char m_MirrorFlag;
  unsigned char m_StatusFlag;
  unsigned char m_NeighborsUsedFlag;
  int m_Shades;
  int m_StartRed;
  int m_StartGreen;
  int m_StartBlue;
  int m_EndRed;
  int m_EndGreen;
  int m_EndBlue;
  int m_XRotation;
  int m_YRotation;
  int m_ZRotation;
  int m_XTranslation;
  int m_YTranslation;
  int m_ZTranslation;
  int m_XCenter;
  int m_YCenter;
  int m_ZCenter;
  int m_XRotationIncrement;
  int m_YRotationIncrement;
  int m_ZRotationIncrement;
  int m_XTranslationIncrement;
  int m_YTranslationIncrement;
  int m_ZTranslationIncrement;
  short int m_MinimumXValue;
  short int m_MinimumYValue;
  short int m_MinimumZValue;
  short int m_MaximumXValue;
  short int m_MaximumYValue;
  short int m_MaximumZValue;
  float m_Opacity;
  int m_OpacityThickness;
  float m_BlendFactor;
};
#endif                           // __OBJECTENTR_H__
