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
#include <itkByteSwapper.h>
#include <iostream>
#include <fstream>
// TODO: Add namespace  namespace itk {
// TODO: USE GET/SET methods for member variables needs to be put together along with one set of
//       documentation that is syncronized syncronized.
namespace itk{
template <typename ReadType>
void ReadBytes(FILE *fptr,ReadType * dest, const int Replications, const bool NeedByteSwap)
{
  if (::fread(dest, sizeof(ReadType), Replications, fptr) != Replications)
  {
    ::fprintf(stderr, "6: Unable to read in object #1 description.\n");
    exit(-1);
  }
 //TODO:  Need to fix bug swapping.
  if(NeedByteSwap)
  {
      itk::ByteSwapper<ReadType>::SwapFromSystemToBigEndian(dest);
    
   // SwapObjectEndedness(AnaylzeObjectEntryArray[1]->getObjectPointer());
  }

}

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
  typedef itk::Index<3> Index;

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
  itkGetConstMacro(StartColor, intRGBPixel);
 
  /**
   * \brief setEndColor
   *
   * Set the ending colors (red, green, blue) for creating the colormap.
   */
  itkSetMacro(EndColor, intRGBPixel);
  itkGetConstMacro(EndColor, intRGBPixel);


  /**
   * \brief setRotation
   *
   * Set the rotation of the object (xRotation, yRotation, zRotation).
   */
  itkSetMacro(Rotation, Index);
  itkGetConstMacro(Rotation, Index);
  
  /**
   * \brief setTranslation
   * 
   * Set the translation of the object (xTranslation, yTranslation, zTranslation).
   */
  itkSetMacro(Translation, Index);
  itkGetConstMacro(Translation, Index);
  
  /**
   * \brief setCenter
   *
   * Set the center of the object (xCenter, yCenter, zCenter).
   */
  itkSetMacro(Center, Index);
  itkGetConstMacro(Center, Index);
  

  /**
   * \brief setRotationIncrement
   *
   * Set the rotation increment (xRotationIncrement, yRotationIncrement, zRotationIncrement).
   */
  itkSetMacro(RotationIncrement, Index);
  itkGetConstMacro(RotationIncrement, Index);
  
  /**
   * \brief setTranslationIncrement
   * 
   * Set the traslation increment of the object (xTranslationIncrement, yTranslationIncrement, zTranslatoinIncrement).
   */
  itkSetMacro(TranslationIncrement, Index);
  itkGetConstMacro(TranslationIncrement, Index);
  
  /**
   * \brief setMinimumCoordinate
   *
   * Set the minimum coordinate of the bounding brick of the object used for rendering by Analyze
   */
  itkSetMacro(MinimumCoordinateValue, Index);
  itkGetConstMacro(MinimumCoordinateValue, Index);
  

  /**
   * \brief setMaximumCoordinate
   * 
   * Set the maximum coordinate of the bounding brick of the object used for rendering by Analyze
   */
  itkSetMacro(MaximumCoordinateValue, Index);
  itkGetConstMacro(MaximumCoordinateValue, Index);
  

  void ReadFromFilePointer(FILE *fptr, const bool NeedByteSwap){
      int color;
      std::ofstream myfile;
  myfile.open("ReadFromFilePointer27.txt", myfile.app);
  //IntFunc
  ReadBytes<char>(fptr, this->m_Name, 32,NeedByteSwap);
  
  myfile<<this->m_Name<<"\n";
  //std::cout<<this->m_Name<<"\n";


  ReadBytes<int>(fptr, &(this->m_DisplayFlag),1,NeedByteSwap);
  //std::cout<<m_DisplayFlag<<std::endl;
  myfile<<m_DisplayFlag<<"\n";


  ReadBytes<unsigned char>(fptr, &m_CopyFlag,1,NeedByteSwap);
  //std::cout<<(int)m_CopyFlag<<std::endl;
  myfile<<(int)m_CopyFlag<<"\n";


  ReadBytes<unsigned char>(fptr, &m_MirrorFlag,1,NeedByteSwap);
  //std::cout<<(int)m_MirrorFlag<<std::endl;
  myfile<<(int)m_MirrorFlag<<"\n";


  ReadBytes<unsigned char>(fptr, &m_StatusFlag,1,NeedByteSwap);
  //std::cout<<(int)m_StatusFlag<<std::endl;
  myfile<<(int)m_StatusFlag<<"\n";


  ReadBytes<unsigned char>(fptr, &m_NeighborsUsedFlag,1,NeedByteSwap);
  //std::cout<<(int)m_NeighborsUsedFlag<<std::endl;
  myfile<<(int)m_NeighborsUsedFlag<<"\n";


  ReadBytes<int>(fptr, &m_Shades,1,NeedByteSwap);
  myfile<<m_Shades<<"\n";
  //std::cout<<m_Shades<<std::endl;


  ReadBytes<int>(fptr, &m_StartRed,1,NeedByteSwap);
  //std::cout<<m_StartRed<<std::endl;
  myfile<<m_StartRed<<"\n";


  ReadBytes<int>(fptr, &m_StartGreen,1,NeedByteSwap);
  //std::cout<<m_StartGreen<<std::endl;
  myfile<<m_StartGreen<<"\n";


  ReadBytes<int>(fptr, &m_StartBlue,1,NeedByteSwap);
  //std::cout<<m_StartBlue<<std::endl;
  myfile<<m_StartBlue<<"\n";

  //ReadBytes<intRGBPixel>(fptr, &m_StartColor,1,NeedByteSwap);  //Three seperate Start Colors (Red, Green, Blue) have been put together to use the set macro.

  m_StartColor.SetBlue(m_StartBlue);
  m_StartColor.SetGreen(m_StartGreen);
  m_StartColor.SetRed(m_StartRed);

  ReadBytes<int>(fptr, &m_EndRed,1,NeedByteSwap);
  //std::cout<<m_EndRed<<std::endl;
  myfile<<m_EndRed<<"\n";


  ReadBytes<int>(fptr, &m_EndGreen,1,NeedByteSwap);
  //std::cout<<m_EndGreen<<std::endl;
  myfile<<m_EndGreen<<"\n";


  ReadBytes<int>(fptr, &m_EndBlue,1,NeedByteSwap);
  //std::cout<<m_EndBlue<<std::endl;
  myfile<<m_EndBlue<<"\n";
 
  //ReadBytes<intRGBPixel>(fptr, &m_EndColor,1,NeedByteSwap);    //Three seperate End Colors (Red, Green, Blue) have been put together to use the set macro.

  ReadBytes<int>(fptr, &m_XRotation,1,NeedByteSwap);
  myfile<<this->m_XRotation<<"\n";


  ReadBytes<int>(fptr, &m_YRotation,1,NeedByteSwap);
  myfile<<this->m_YRotation<<"\n";


  ReadBytes<int>(fptr, &m_ZRotation,1,NeedByteSwap);
  myfile<<this->m_ZRotation<<"\n";


 // ReadBytes<Index>(fptr, &m_Rotation,1,NeedByteSwap);          //Three seperate Rotations (x, y, z) have been put together to use the set macro.
  ReadBytes<int>(fptr, &m_XTranslation,1,NeedByteSwap);
  myfile<<this->m_XTranslation<<"\n";


  ReadBytes<int>(fptr, &m_YTranslation,1,NeedByteSwap);
  myfile<<this->m_YTranslation<<"\n";


  ReadBytes<int>(fptr, &m_ZTranslation,1,NeedByteSwap);
  myfile<<this->m_ZTranslation<<"\n";


  //ReadBytes<Index>(fptr, &m_Translation,1,NeedByteSwap);       //Three seperate Translations (x, y, z) have been put together to use the set macro.
  ReadBytes<int>(fptr, &m_XCenter,1,NeedByteSwap);
  myfile<<this->m_XCenter<<"\n";


  ReadBytes<int>(fptr, &m_YCenter,1,NeedByteSwap);
  myfile<<this->m_YCenter<<"\n";


  ReadBytes<int>(fptr, &m_ZCenter,1,NeedByteSwap);
  myfile<<this->m_ZCenter<<"\n";


  //ReadBytes<Index>(fptr, &m_Center,1,NeedByteSwap);            //Three seperate Centers (x, y, z) have been put together to use the set macro.
  ReadBytes<int>(fptr, &m_XRotationIncrement,1,NeedByteSwap);
  myfile<<this->m_XRotationIncrement<<"\n";


  ReadBytes<int>(fptr, &m_YRotationIncrement,1,NeedByteSwap);
  myfile<<this->m_YRotationIncrement<<"\n";


  ReadBytes<int>(fptr, &m_ZRotationIncrement,1,NeedByteSwap);
  myfile<<this->m_ZRotationIncrement<<"\n";


  //ReadBytes<Index>(fptr, &m_RotationIncrement,1,NeedByteSwap);  //Three seperate Rotation Increments (x, y, z) have been put together to use the set macro.
  ReadBytes<int>(fptr, &m_XTranslationIncrement,1,NeedByteSwap);
  myfile<<this->m_XTranslationIncrement<<"\n";


  ReadBytes<int>(fptr, &m_YTranslationIncrement,1,NeedByteSwap);
  myfile<<this->m_YTranslationIncrement<<"\n";


  ReadBytes<int>(fptr, &m_ZTranslationIncrement,1,NeedByteSwap);
  myfile<<this->m_ZTranslationIncrement<<"\n";


  //ReadBytes<Index>(fptr, &m_TranslationIncrement,1,NeedByteSwap); //Three seperate Translation Increments (x, y, z) have been put together to use the set macro.
  ReadBytes<short int>(fptr, &m_MinimumXValue,1,NeedByteSwap);
  myfile<<this->m_MinimumXValue<<"\n";


  ReadBytes<short int>(fptr, &m_MinimumYValue,1,NeedByteSwap);
  myfile<<this->m_MinimumYValue<<"\n";


  ReadBytes<short int>(fptr, &m_MinimumZValue,1,NeedByteSwap);
  myfile<<this->m_MinimumZValue<<"\n";


  //ReadBytes<Index>(fptr, &m_MinimumCoordinateValue,1,NeedByteSwap);  //Three seperate Minimum Coordinate Values (x, y, z) have been put together to use the set macro.
  ReadBytes<short int>(fptr, &m_MaximumXValue,1,NeedByteSwap);
  myfile<<this->m_MaximumXValue<<"\n";


  ReadBytes<short int>(fptr, &m_MaximumYValue,1,NeedByteSwap);
  myfile<<this->m_MaximumYValue<<"\n";


  ReadBytes<short int>(fptr, &m_MaximumZValue,1,NeedByteSwap);
  myfile<<this->m_MaximumZValue<<"\n";


  //ReadBytes<Index>(fptr, &m_MaximumCoordinateValue,1,NeedByteSwap);   //Three seperate Maximum Coordiante Values (x, y, z) have been put together to use the set macro.
  ReadBytes<float>(fptr, &m_Opacity,1,NeedByteSwap);
  myfile<<this->m_Opacity<<"\n";


  ReadBytes<int>(fptr, &m_OpacityThickness,1,NeedByteSwap);
  myfile<<this->m_OpacityThickness<<"\n";

  ReadBytes<float>(fptr, &m_BlendFactor,1,NeedByteSwap);
  myfile<<m_BlendFactor<<"\n";

  myfile<<"===========================================================================================\n";
  //std::cout<<"==========================================================================================="<<std::endl;
  myfile.close();
  }
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
  char m_Name[33];                     /*bytes   0-31*/
  int m_DisplayFlag;                   /*bytes  32-35*/
  unsigned char m_CopyFlag;            /*bytes  36-36*/
  unsigned char m_MirrorFlag;          /*bytes  37-37*/
  unsigned char m_StatusFlag;          /*bytes  38-38*/
  unsigned char m_NeighborsUsedFlag;   /*bytes  39-39*/
  int m_Shades;                        /*bytes  40-43*/
  int m_StartRed;                      /*bytes  44-47*/
  int m_StartGreen;                    /*bytes  48-51*/
  int m_StartBlue;                     /*bytes  52-55*/
  int m_EndRed;                        /*bytes  53-58*/
  int m_EndGreen;                      /*bytes  59-62*/
  int m_EndBlue;                       /*bytes  63-66*/
  int m_XRotation;                     /*bytes  67-70*/
  int m_YRotation;                     /*bytes  71-74*/
  int m_ZRotation;                     /*bytes  75-78*/  
  int m_XTranslation;                  /*bytes  79-82*/
  int m_YTranslation;                  /*bytes  83-86*/
  int m_ZTranslation;                  /*bytes  87-90*/ 
  int m_XCenter;                       /*bytes  91-94*/
  int m_YCenter;                       /*bytes  95-98*/
  int m_ZCenter;                       /*bytes  99-102*/        
  int m_XRotationIncrement;            /*bytes  103-106*/
  int m_YRotationIncrement;            /*bytes  107-110*/
  int m_ZRotationIncrement;            /*bytes  111-114*/
  int m_XTranslationIncrement;         /*bytes  115-118*/
  int m_YTranslationIncrement;         /*bytes  119-121*/
  int m_ZTranslationIncrement;         /*bytes  122-125*/
  short int m_MinimumXValue;           /*bytes  126-127*/
  short int m_MinimumYValue;           /*bytes  128-129*/
  short int m_MinimumZValue;           /*bytes  130-131*/
  short int m_MaximumXValue;           /*bytes  132-133*/
  short int m_MaximumYValue;           /*bytes  134-135*/
  short int m_MaximumZValue;           /*bytes  136-137*/
  float m_Opacity;                     /*bytes  138-141*/
  int m_OpacityThickness;              /*bytes  142-145*/
  float m_BlendFactor;                 /*bytes  146-149*/


  //Three seperate Start Colors (Red, Green, Blue) have been put together to use the set macro.
  intRGBPixel m_StartColor;  
    //Three seperate End Colors (Red, Green, Blue) have been put together to use the set macro.
  intRGBPixel m_EndColor; 
    //Three seperate Rotations (x, y, z) have been put together to use the set macro.
  Index m_Rotation;
  //Three seperate Translations (x, y, z) have been put together to use the set macro.
  Index m_Translation;  
  //Three seperate Centers (x, y, z) have been put together to use the set macro.
  Index m_Center; 
  //Three seperate Rotation Increments (x, y, z) have been put together to use the set macro.
  Index m_RotationIncrement; 
  //Three seperate Translation Increments (x, y, z) have been put together to use the set macro.
  Index m_TranslationIncrement; 
  //Three seperate Minimum Coordinate Values (x, y, z) have been put together to use the set macro.
  Index m_MinimumCoordinateValue; 
  //Three seperate Maximum Coordiante Values (x, y, z) have been put together to use the set macro.
  Index m_MaximumCoordinateValue; 
};
}
#endif                           // __OBJECTENTR_H__
