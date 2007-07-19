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
#include <itksys/SystemTools.hxx>
#include <itkImageIOBase.h>

namespace itk{
  template <typename ReadType>
  void ReadBytes(std::ifstream & inputFileStream,ReadType * dest, const int Replications, const bool NeedByteSwap)
  {
    if(inputFileStream.read(reinterpret_cast<char *>(dest), sizeof(ReadType) * Replications).fail())
    {
        std::cout<<"6: Unable to read in object #1 description."<<std::endl;
    }
    if(NeedByteSwap)
    {
      itk::ByteSwapper<ReadType>::SwapFromSystemToBigEndian(dest);
    }
  }


  /**
   * \class AnalyzeObjectEntry
   * \brief This class encapsulates a single object in an Analyze object file
   */
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
      itkGetConstMacro(BlendFactor, float);
      itkSetMacro(BlendFactor, float);

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

      void Print(std::ostream &myfile) 
      {
        myfile<<this->m_Name<<"\n";
        myfile<<m_DisplayFlag<<"\n";
        myfile<<(int)m_CopyFlag<<"\n";
        myfile<<(int)m_MirrorFlag<<"\n";
        myfile<<(int)m_StatusFlag<<"\n";
        myfile<<(int)m_NeighborsUsedFlag<<"\n";
        myfile<<m_Shades<<"\n";
        myfile<<m_StartRed<<"\n";
        myfile<<m_StartGreen<<"\n";
        myfile<<m_StartBlue<<"\n";
        myfile<<m_EndRed<<"\n";
        myfile<<m_EndGreen<<"\n";
        myfile<<m_EndBlue<<"\n";
        myfile<<m_XRotation<<"\n";
        myfile<<this->m_YRotation<<"\n";
        myfile<<this->m_ZRotation<<"\n";
        myfile<<this->m_XTranslation<<"\n";
        myfile<<this->m_YTranslation<<"\n";
        myfile<<this->m_ZTranslation<<"\n";
        myfile<<this->m_XCenter<<"\n";
        myfile<<this->m_YCenter<<"\n";
        myfile<<this->m_ZCenter<<"\n";
        myfile<<this->m_XRotationIncrement<<"\n";
        myfile<<this->m_YRotationIncrement<<"\n";
        myfile<<this->m_ZRotationIncrement<<"\n";
        myfile<<this->m_XTranslationIncrement<<"\n";
        myfile<<this->m_YTranslationIncrement<<"\n";
        myfile<<this->m_ZTranslationIncrement<<"\n";
        myfile<<this->m_MinimumXValue<<"\n";
        myfile<<this->m_MinimumYValue<<"\n";
        myfile<<this->m_MinimumZValue<<"\n";
        myfile<<this->m_MaximumXValue<<"\n";
        myfile<<this->m_MaximumYValue<<"\n";
        myfile<<this->m_MaximumZValue<<"\n";
        myfile<<this->m_Opacity<<"\n";
        myfile<<this->m_OpacityThickness<<"\n";
        myfile<<m_BlendFactor<<"\n";
        myfile<<"===========================================================================================\n";
      }

      void ReadFromFilePointer(std::ifstream & inputFileStream, const bool NeedByteSwap, const bool NeedBlendFactor)
      {
        //IntFunc
        ReadBytes<char>(inputFileStream, this->m_Name, 32,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &(this->m_DisplayFlag),1,NeedByteSwap);
        ReadBytes<unsigned char>(inputFileStream, &m_CopyFlag,1,NeedByteSwap);
        ReadBytes<unsigned char>(inputFileStream, &m_MirrorFlag,1,NeedByteSwap);
        ReadBytes<unsigned char>(inputFileStream, &m_StatusFlag,1,NeedByteSwap);
        ReadBytes<unsigned char>(inputFileStream, &m_NeighborsUsedFlag,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_Shades,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_StartRed,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_StartGreen,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_StartBlue,1,NeedByteSwap);
        m_StartColor.SetBlue(m_StartBlue);
        m_StartColor.SetGreen(m_StartGreen);
        m_StartColor.SetRed(m_StartRed);
        ReadBytes<int>(inputFileStream, &m_EndRed,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_EndGreen,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_EndBlue,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_XRotation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_YRotation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_ZRotation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_XTranslation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_YTranslation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_ZTranslation,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_XCenter,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_YCenter,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_ZCenter,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_XRotationIncrement,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_YRotationIncrement,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_ZRotationIncrement,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_XTranslationIncrement,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_YTranslationIncrement,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_ZTranslationIncrement,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MinimumXValue,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MinimumYValue,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MinimumZValue,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MaximumXValue,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MaximumYValue,1,NeedByteSwap);
        ReadBytes<short int>(inputFileStream, &m_MaximumZValue,1,NeedByteSwap);
        ReadBytes<float>(inputFileStream, &m_Opacity,1,NeedByteSwap);
        ReadBytes<int>(inputFileStream, &m_OpacityThickness,1,NeedByteSwap);
        if(NeedBlendFactor)
        {
          ReadBytes<float>(inputFileStream, &m_BlendFactor,1,NeedByteSwap);
        }
      }

      void SwapObjectEndedness()
      {
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_DisplayFlag));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_Shades));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_StartRed));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_StartGreen));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_StartBlue));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_EndRed));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_EndGreen));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_EndBlue));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_XRotation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_YRotation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_ZRotation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_XTranslation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_YTranslation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_ZTranslation));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_XCenter));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_YCenter));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_ZCenter));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_XRotationIncrement));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_YRotationIncrement));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_ZRotationIncrement));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_XTranslationIncrement));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_YTranslationIncrement));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_ZTranslationIncrement));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MinimumXValue));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MinimumYValue));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MinimumZValue));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MaximumXValue));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MaximumYValue));
        itk::ByteSwapper<short int>::SwapFromSystemToBigEndian(&(this->m_MaximumZValue));
        itk::ByteSwapper<float>::SwapFromSystemToBigEndian(&(this->m_Opacity));
        itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&(this->m_OpacityThickness));
        itk::ByteSwapper<float>::SwapFromSystemToBigEndian(&(this->m_BlendFactor));
      }

      void Write(std::ofstream &inputFileStream)
      {
        inputFileStream.write(reinterpret_cast<char *>(m_Name), sizeof(char)*32);
        inputFileStream.write(reinterpret_cast<char *>(&m_DisplayFlag), sizeof(m_DisplayFlag));
        inputFileStream.write(reinterpret_cast<char *>(&m_CopyFlag), sizeof(m_CopyFlag));
        inputFileStream.write(reinterpret_cast<char *>(&m_MirrorFlag), sizeof(m_MirrorFlag));
        inputFileStream.write(reinterpret_cast<char *>(&m_StatusFlag), sizeof(m_StatusFlag));
        inputFileStream.write(reinterpret_cast<char *>(&m_NeighborsUsedFlag), sizeof(m_NeighborsUsedFlag));
        inputFileStream.write(reinterpret_cast<char *>(&m_Shades), sizeof(m_Shades));
        inputFileStream.write(reinterpret_cast<char *>(&m_StartRed), sizeof(m_StartRed));
        inputFileStream.write(reinterpret_cast<char *>(&m_StartGreen), sizeof(m_StartGreen));
        inputFileStream.write(reinterpret_cast<char *>(&m_StartBlue), sizeof(m_StartBlue));
        inputFileStream.write(reinterpret_cast<char *>(&m_EndRed), sizeof(m_EndRed));
        inputFileStream.write(reinterpret_cast<char *>(&m_EndGreen), sizeof(m_EndGreen));
        inputFileStream.write(reinterpret_cast<char *>(&m_EndBlue), sizeof(m_EndBlue));
        inputFileStream.write(reinterpret_cast<char *>(&m_XRotation), sizeof(m_XRotation));
        inputFileStream.write(reinterpret_cast<char *>(&m_YRotation), sizeof(m_YRotation));
        inputFileStream.write(reinterpret_cast<char *>(&m_ZRotation), sizeof(m_ZRotation));
        inputFileStream.write(reinterpret_cast<char *>(&m_XTranslation), sizeof(m_XTranslation));
        inputFileStream.write(reinterpret_cast<char *>(&m_YTranslation), sizeof(m_YTranslation));
        inputFileStream.write(reinterpret_cast<char *>(&m_ZTranslation), sizeof(m_ZTranslation));
        inputFileStream.write(reinterpret_cast<char *>(&m_XCenter), sizeof(m_XCenter));
        inputFileStream.write(reinterpret_cast<char *>(&m_YCenter), sizeof(m_YCenter));
        inputFileStream.write(reinterpret_cast<char *>(&m_ZCenter), sizeof(m_ZCenter));
        inputFileStream.write(reinterpret_cast<char *>(&m_XRotationIncrement), sizeof(m_XRotationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_YRotationIncrement), sizeof(m_YRotationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_ZRotationIncrement), sizeof(m_ZRotationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_XTranslationIncrement), sizeof(m_XTranslationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_YTranslationIncrement), sizeof(m_YTranslationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_ZTranslationIncrement), sizeof(m_ZTranslationIncrement));
        inputFileStream.write(reinterpret_cast<char *>(&m_MinimumXValue), sizeof(m_MinimumXValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_MinimumYValue), sizeof(m_MinimumYValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_MinimumZValue), sizeof(m_MinimumZValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_MaximumXValue), sizeof(m_MaximumXValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_MaximumYValue), sizeof(m_MaximumYValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_MaximumZValue), sizeof(m_MaximumZValue));
        inputFileStream.write(reinterpret_cast<char *>(&m_Opacity), sizeof(m_Opacity));
        inputFileStream.write(reinterpret_cast<char *>(&m_OpacityThickness), sizeof(m_OpacityThickness));
        inputFileStream.write(reinterpret_cast<char *>(&m_BlendFactor), sizeof(m_BlendFactor));
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
