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
#include <stdio.h>
#include <itkObject.h>
#include <itkObjectFactory.h>
#include <itkImage.h>
#include <itkRGBPixel.h>
// TODO: Add namespace  namespace itk {
// TODO: USE GET/SET methods for member variables needs to be put together along with one set of
//       documentation that is syncronized syncronized.
namespace itk{
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
  //TODO:  derive from itk::Object to get these to be formal itk pipeline managable objects.
class AnalyzeObjectEntry : public Object
{
public:
  /** Standard typedefs. */
  typedef AnalyzeObjectEntry Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef itk::RGBPixel<int> intRGBPixel;

  // RGBPixel::pointer RGBPixelPointer = RGBPixel::New();
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AnalyzeObjectEntry, Object);

  /**
   * \brief ~AnalyzeObjectEntry( void ) is the destructor, which does nothing explicitly due to
   * no use of dynamic allocation
   * \param none
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   */
  virtual ~AnalyzeObjectEntry( void ) { }

  /**
   * \brief operator= is the assignment operator, which copies the data values on the
   * right hand side to the AnalyzeObjectEntry variable on the left
   * \param const AnalyzeObjectEntry & rhs
   * \return none
   * Possible Causes of Failure:
   * - unknown
   * \sa AnalyzeObjectEntry
   */
void Copy( AnalyzeObjectEntry::Pointer rhs );

  /**
   * \brief getName/setName
   *
   * The Name member of the AVW_Object structure contains a user
   * defined name for this object. Any zero-terminated string can be used,
   * including stringswith embedded spaces.
   */
  virtual std::string GetName ( void ) const
  {
    itkDebugMacro("returning " << "Name of " << this->m_Name );
    return std::string(this->m_Name);
  }

  /** Set built-in type.  Creates member Set"name"() (e.g., SetVisibility()); */
  virtual void SetName (const std::string _arg)
  {
    char temp[32];
    strncpy(temp,_arg.c_str(),31);
    temp[31]='\0';
    itkDebugMacro("setting " << "Name" << " to " << temp);
    if (strcmp(this->m_Name, temp) != 0)
      {
      strncpy(this->m_Name,temp,32);
      this->Modified();
      }
  }
  /**
   * \brief getDisplayFlag/setDisplayFlag
   *
   * DisplayFlag is used to enable or disable the display of this
   * particular object. A value of zero value indicates
   * that voxels of this object type are ignored or assumed to be outside
   * the threshold range during the raycasting process.
   */
  itkGetConstMacro(DisplayFlag, int);
  itkSetMacro(DisplayFlag, int);

  /**
   * \brief getCopyFlag gets the Copy Flag
   *
   * CopyFlag indicates object translation, rotation, and mirroring are
   * applied to a copy of the actual object, rather
   * than the object itself. [ANALYZE only]
   */
  itkGetConstMacro(CopyFlag, unsigned char);
  itkSetMacro(CopyFlag, unsigned char);

  /**
   * \brief getMirrorFlag/setMirrorFlag
   *
   * MirrorFlag indicates the axis this object is mirrored around.
   * [ANALYZE only]
   */
  itkGetConstMacro(MirrorFlag, unsigned char);
  itkSetMacro(MirrorFlag, unsigned char);

  /**
   * \brief getStatusFlag/setStatusFlag
   *
   * StatusFlag is used to indicate when an object has changed and may
   * need it's minimum bounding box recomputed. [ANALYZE only]
   */
  itkGetConstMacro(StatusFlag, unsigned char);
  itkSetMacro(StatusFlag, unsigned char);

  /**
   * \brief getNeighborsUsedFlag/setNeighborsUsedFlag
   *
   * NeighborsUsedFlag indicates which neighboring voxels are used in
   * calculating the objects shading. [ANALYZE only]
   */
  itkGetConstMacro(NeighborsUsedFlag, unsigned char);
  itkSetMacro(NeighborsUsedFlag, unsigned char);

  /**
   * \brief getShades/setShades
   *
   * Shades indicates the number of shades available for this object.
   * Only 256 (250 in ANALYZE) total shades are available.
   */
  itkGetConstMacro(Shades, int);
  itkSetMacro(Shades, int);

  /**
   * \brief getStartRed/setStartRed
   *
   * StartRed specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  itkGetConstMacro(StartRed, int);
  itkSetMacro(StartRed, int);

  /**
   * \brief getStartGreen/setStartGreen
   *
   * StartGreen specify the starting color for this object. This is usually
   * a darker shade of the ending color.  ANALYZE defaults these values to
   * 10% of the ending color.
   */
  itkGetConstMacro(StartGreen, int);
  itkSetMacro(StartGreen, int);

  /**
   * \brief getStartBlue/setStartBlue
   *
   * StartBlue specify the starting color for this object. This is usually a
   * darker shade of the ending color. ANALYZE defaults these values to 10%
   * of the ending color.
   */
  itkGetConstMacro(StartBlue, int);
  itkSetMacro(StartBlue, int);

  /**
   * \brief getEndRed/setEndRed
   *
   * EndRed specify the ending color for this object.
   */
  itkGetConstMacro(EndRed, int);
  itkSetMacro(EndRed, int);

  /**
   * \brief getEndGreen/setEndGreen
   *
   * EndGreen specify the ending color for this object.
   */
  itkGetConstMacro(EndGreen, int);
  itkSetMacro(EndGreen, int);

  /**
   * \brief getEndBlue/setEndBlue
   *
   * EndBlue specify the ending color for this object.
   */
  itkGetConstMacro(EndBlue, int);
  itkSetMacro(EndBlue, int);

  /**
   * \brief getXRotation
   *
   * XRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(XRotation, int);

  /**
   *\brief getXRotationIncrement
   *
   * XRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  itkGetConstMacro(XRotationIncrement, int);

  /**
   * \brief getYRotation
   *
   * YRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(YRotation, int);

  /**
   *\brief getYRotationIncrement
   *
   * YRotationIncrement specify increments that are applies to XRotation,
   * YRotation, and ZRotation when making a sequence. [ANALYZE only]
   */
  itkGetConstMacro(YRotationIncrement, int);

  /**
   * \brief getZRotation
   *
   * ZRotation specify a rotation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(ZRotation, int);

   /**
    *\brief getZRotationIncrement
    *
    * ZRotationIncrement specify increments that are applies to XRotation,
    * YRotation, and ZRotation when making a sequence. [ANALYZE only]
    */
  itkGetConstMacro(ZRotationIncrement, int);

  /**
   * \brief getXTranslation
   *
   * XTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(XTranslation, int);

   /**
    \*brief getXTranslation
    *
    * XTranslationIncrement specify increments that are applies to
    * XTranslation, YTranslation, and ZTranslation when making a sequence.
    * [ANALYZE only]
    */
  itkGetConstMacro(XTranslationIncrement, int);

  /**
   * \brief getYTranslation
   *
   * YTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(YTranslation, int);

   /**
    *\brief getYTranslationIncrement
    *
    * YTranslationIncrement specify increments that are applies to
    * XTranslation, YTranslation, and ZTranslation when making a sequence.
    * [ANALYZE only]
    */
  itkGetConstMacro(YTranslationIncrement, int);

  /**
   * \brief getZTranslation
   *
   * ZTranslation specify a translation which is applied to this object only.
   * [ANALYZE only]
   */
  itkGetConstMacro(ZTranslation, int);
  
  /**
   *\brief getZTranslation
   *
   * ZTranslationIncrement specify increments that are applies to
   * XTranslation, YTranslation, and ZTranslation when making a sequence.
   * [ANALYZE only]
   */
  itkGetConstMacro(ZTranslationIncrement, int);

  /**
   * \brief getXCenter/setXCenter
   *
   * XCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  itkGetConstMacro(XCenter, int);
  itkSetMacro(XCenter, int);

  /**
   * \brief getYCenter/setYCenter
   *
   * YCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  itkGetConstMacro(YCenter, int);
  itkSetMacro(YCenter, int);

  /**
   * \brief getZCenter/setZCenter
   *
   * ZCenter specify the rotation center, relative to the volumes center,
   * which the XRotation, YRotation, and ZRotation are rotated around.
   * [ANALYZE only]
   */
  itkGetConstMacro(ZCenter, int);
  itkSetMacro(ZCenter, int);

  /**
   * \brief getMinimumXValue/setMinimumXValue
   *
   * MinimumXValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MinimumXValue, short int);
  itkSetMacro(MinimumXValue, int);

  /**
   * \brief getMinimumYValue/setMinimumYValue
   *
   * MinimumYValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MinimumYValue, short int);
  itkSetMacro(MinimumYValue, int);

  /**
   * \brief getMinimumZValue/setMinimumZValue
   *
   * MinimumZValue specify the minimum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MinimumZValue, short int);
  itkSetMacro(MinimumZValue, int);

  /**
   * \brief getMaximumXValue/setMaximumXValue
   *
   * MaximumXValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MaximumXValue, short int);
  itkSetMacro(MaximumXValue, int);

  /**
   * \brief getMaximumYValue/setMaximumYValue
   *
   * MaximumYValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MaximumYValue, short int);
  itkSetMacro(MaximumYValue, int);

  /**
   * \brief getMaximumZValue/setMaximumZValue
   *
   * MaximumZValue specify the maximum enclosing brick used by ANALYZE to
   * increase the rendering speed of individual objects during object
   * translations and rotations. [ANALYZE only]
   */
  itkGetConstMacro(MaximumZValue, short int);
  itkSetMacro(MaximumZValue, int);

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
  itkGetConstMacro(Opacity, float);
  itkSetMacro(Opacity, float);

  /**
   * \brief getOpacityThickness/setOpacityThickness
   *
   * The thickness of the object
   */
  itkGetConstMacro(OpacityThickness, int);
  itkSetMacro(OpacityThickness, int);

  /**
   * \brief getBlendFactor/setBlendFactor
   *
   * Determines the amount of object color verses
   * composite color. A value of 1.0, causes all the
   * color to come from the object. A value of 0.0
   * causes all the color to come from the alpha map.
   * A value of .5 will cause half to come from each.
   */
  itkGetConstMacro(BlendFactor, int);
  itkSetMacro(BlendFactor, int);

  /**
   * \brief setStartColor
   *
   * Set the starting colors (red, green, blue) for creating the colormap.
   */
  itkSetMacro( StartColor, intRGBPixel);
 
  /**
   * \brief setEndColor
   *
   * Set the ending colors (red, green, blue) for creating the colormap.
   */
  itkSetMacro(EndColor, intRGBPixel);


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



protected:
  /**
   * \brief AnalyzeObjectEntry( ) is the default constructor, initializes to 0 or NULL
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
  intRGBPixel m_StartColor;  //Three seperate Start Colors (Red, Green, Blue) have been put together to use the set macro.
  int m_EndRed;
  int m_EndGreen;
  int m_EndBlue;
  intRGBPixel m_EndColor;    //Three seperate End Colors (Red, Green, Blue) have been put together to use the set macro.
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
}
#endif                           // __OBJECTENTR_H__
