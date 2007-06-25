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
#include "objectentry.h"

AnalyzeObjectEntry::AnalyzeObjectEntry( void )
{
  for (unsigned int i = 0; i < sizeof(this->m_Name); i++)
  {
    this->m_Name[i] = '\0';
  }
  this->m_DisplayFlag = 1;
  this->m_CopyFlag = 0;
  this->m_MirrorFlag = 0;
  this->m_StatusFlag = 0;
  this->m_NeighborsUsedFlag = 0;
  this->m_Shades = 1;
  this->m_StartRed = 0;
  this->m_StartGreen = 0;
  this->m_StartBlue = 0;
  this->m_EndRed = 0;
  this->m_EndGreen = 0;
  this->m_EndBlue = 0;
  this->m_XRotation = 0;
  this->m_YRotation = 0;
  this->m_ZRotation = 0;
  this->m_XTranslation = 0;
  this->m_YTranslation = 0;
  this->m_ZTranslation = 0;
  this->m_XCenter = 0;
  this->m_YCenter = 0;
  this->m_ZCenter = 0;
  this->m_XRotationIncrement = 0;
  this->m_YRotationIncrement = 0;
  this->m_ZRotationIncrement = 0;
  this->m_XTranslationIncrement = 0;
  this->m_YTranslationIncrement = 0;
  this->m_ZTranslationIncrement = 0;
  this->m_MinimumXValue = 0;
  this->m_MinimumYValue = 0;
  this->m_MinimumZValue = 0;
  this->m_MaximumXValue = 0;
  this->m_MaximumYValue = 0;
  this->m_MaximumZValue = 0;
  this->m_Opacity = 0.5;
  this->m_OpacityThickness = 1;
  this->m_BlendFactor = 0;
}


AnalyzeObjectEntry::AnalyzeObjectEntry( const AnalyzeObjectEntry & rhs )
{
  const char * name = rhs.getNameASCII();

  for (unsigned int i = 0; i < sizeof(this->m_Name); i++)
  {
    this->m_Name[i] = name[i];
  }

  this->m_DisplayFlag = rhs.getDisplayFlag();
  this->m_CopyFlag = rhs.getCopyFlag();
  this->m_MirrorFlag = rhs.getMirrorFlag();
  this->m_StatusFlag = rhs.getStatusFlag();
  this->m_NeighborsUsedFlag = rhs.getNeighborsUsedFlag();
  this->m_Shades = rhs.getShades();
  this->m_StartRed = rhs.getStartRed();
  this->m_StartGreen = rhs.getStartGreen();
  this->m_StartBlue = rhs.getStartBlue();
  this->m_EndRed = rhs.getEndRed();
  this->m_EndGreen = rhs.getEndGreen();
  this->m_EndBlue = rhs.getEndBlue();
  this->m_XRotation = rhs.getXRotation();
  this->m_YRotation = rhs.getYRotation();
  this->m_ZRotation = rhs.getZRotation();
  this->m_XTranslation = rhs.getXTranslation();
  this->m_YTranslation = rhs.getYTranslation();
  this->m_ZTranslation = rhs.getZTranslation();
  this->m_XCenter = rhs.getXCenter();
  this->m_YCenter = rhs.getYCenter();
  this->m_ZCenter = rhs.getZCenter();
  this->m_XRotationIncrement = rhs.getXRotationIncrement();
  this->m_YRotationIncrement = rhs.getYRotationIncrement();
  this->m_ZRotationIncrement = rhs.getZRotationIncrement();
  this->m_XTranslationIncrement = rhs.getXTranslationIncrement();
  this->m_YTranslationIncrement = rhs.getYTranslationIncrement();
  this->m_ZTranslationIncrement = rhs.getZTranslationIncrement();
  this->m_MinimumXValue = rhs.getMinimumXValue();
  this->m_MinimumYValue = rhs.getMinimumYValue();
  this->m_MinimumZValue = rhs.getMinimumZValue();
  this->m_MaximumXValue = rhs.getMaximumXValue();
  this->m_MaximumYValue = rhs.getMaximumYValue();
  this->m_MaximumZValue = rhs.getMaximumZValue();
  this->m_Opacity = rhs.getOpacity();
  this->m_OpacityThickness = rhs.getOpacityThickness();
  this->m_BlendFactor = rhs.getBlendFactor();
}


AnalyzeObjectEntry::~AnalyzeObjectEntry( void )
{

}


AnalyzeObjectEntry & AnalyzeObjectEntry::operator=( const AnalyzeObjectEntry & rhs )
{
  const char * name = rhs.getNameASCII();

  for (unsigned int i = 0; i < sizeof(this->m_Name); i++)
    this->m_Name[i] = name[i];

  this->m_DisplayFlag = rhs.getDisplayFlag();
  this->m_CopyFlag = rhs.getCopyFlag();
  this->m_MirrorFlag = rhs.getMirrorFlag();
  this->m_StatusFlag = rhs.getStatusFlag();
  this->m_NeighborsUsedFlag = rhs.getNeighborsUsedFlag();
  this->m_Shades = rhs.getShades();
  this->m_StartRed = rhs.getStartRed();
  this->m_StartGreen = rhs.getStartGreen();
  this->m_StartBlue = rhs.getStartBlue();
  this->m_EndRed = rhs.getEndRed();
  this->m_EndGreen = rhs.getEndGreen();
  this->m_EndBlue = rhs.getEndBlue();
  this->m_XRotation = rhs.getXRotation();
  this->m_YRotation = rhs.getYRotation();
  this->m_ZRotation = rhs.getZRotation();
  this->m_XTranslation = rhs.getXTranslation();
  this->m_YTranslation = rhs.getYTranslation();
  this->m_ZTranslation = rhs.getZTranslation();
  this->m_XCenter = rhs.getXCenter();
  this->m_YCenter = rhs.getYCenter();
  this->m_ZCenter = rhs.getZCenter();
  this->m_XRotationIncrement = rhs.getXRotationIncrement();
  this->m_YRotationIncrement = rhs.getYRotationIncrement();
  this->m_ZRotationIncrement = rhs.getZRotationIncrement();
  this->m_XTranslationIncrement = rhs.getXTranslationIncrement();
  this->m_YTranslationIncrement = rhs.getYTranslationIncrement();
  this->m_ZTranslationIncrement = rhs.getZTranslationIncrement();
  this->m_MinimumXValue = rhs.getMinimumXValue();
  this->m_MinimumYValue = rhs.getMinimumYValue();
  this->m_MinimumZValue = rhs.getMinimumZValue();
  this->m_MaximumXValue = rhs.getMaximumXValue();
  this->m_MaximumYValue = rhs.getMaximumYValue();
  this->m_MaximumZValue = rhs.getMaximumZValue();
  this->m_Opacity = rhs.getOpacity();
  this->m_OpacityThickness = rhs.getOpacityThickness();
  this->m_BlendFactor = rhs.getBlendFactor();

  return *this;
}


std::string AnalyzeObjectEntry::getName( void ) const
{
  std::string s(this->m_Name);
  return s;
}


const char * AnalyzeObjectEntry::getNameASCII( void ) const
{
  return this->m_Name;
}


int AnalyzeObjectEntry::getDisplayFlag( void ) const
{
  return this->m_DisplayFlag;
}


unsigned char AnalyzeObjectEntry::getCopyFlag( void ) const
{
  return this->m_CopyFlag;
}


unsigned char AnalyzeObjectEntry::getMirrorFlag( void ) const
{
  return this->m_MirrorFlag;
}


unsigned char AnalyzeObjectEntry::getStatusFlag( void ) const
{
  return this->m_StatusFlag;
}


unsigned char AnalyzeObjectEntry::getNeighborsUsedFlag( void ) const
{
  return this->m_NeighborsUsedFlag;
}


int AnalyzeObjectEntry::getShades( void ) const
{
  return this->m_Shades;
}


int AnalyzeObjectEntry::getStartRed( void ) const
{
  return this->m_StartRed;
}


int AnalyzeObjectEntry::getStartGreen( void ) const
{
  return this->m_StartGreen;
}


int AnalyzeObjectEntry::getStartBlue( void ) const
{
  return this->m_StartBlue;
}


int AnalyzeObjectEntry::getEndRed( void ) const
{
  return this->m_EndRed;
}


int AnalyzeObjectEntry::getEndGreen( void ) const
{
  return this->m_EndGreen;
}


int AnalyzeObjectEntry::getEndBlue( void ) const
{
  return this->m_EndBlue;
}


int AnalyzeObjectEntry::getXRotation( void ) const
{
  return this->m_XRotation;
}


int AnalyzeObjectEntry::getYRotation( void ) const
{
  return this->m_YRotation;
}


int AnalyzeObjectEntry::getZRotation( void ) const
{
  return this->m_ZRotation;
}


int AnalyzeObjectEntry::getXTranslation( void ) const
{
  return this->m_XTranslation;
}


int AnalyzeObjectEntry::getYTranslation( void ) const
{
  return this->m_YTranslation;
}


int AnalyzeObjectEntry::getZTranslation( void ) const
{
  return this->m_ZTranslation;
}


int AnalyzeObjectEntry::getXCenter( void ) const
{
  return this->m_XCenter;
}


int AnalyzeObjectEntry::getYCenter( void ) const
{
  return this->m_YCenter;
}


int AnalyzeObjectEntry::getZCenter( void ) const
{
  return this->m_ZCenter;
}


int AnalyzeObjectEntry::getXRotationIncrement( void ) const
{
  return this->m_XRotationIncrement;
}


int AnalyzeObjectEntry::getYRotationIncrement( void ) const
{
  return this->m_YRotationIncrement;
}


int AnalyzeObjectEntry::getZRotationIncrement( void ) const
{
  return this->m_ZRotationIncrement;
}


int AnalyzeObjectEntry::getXTranslationIncrement( void ) const
{
  return this->m_XTranslationIncrement;
}


int AnalyzeObjectEntry::getYTranslationIncrement( void ) const
{
  return this->m_YTranslationIncrement;
}


int AnalyzeObjectEntry::getZTranslationIncrement( void ) const
{
  return this->m_ZTranslationIncrement;
}


short int AnalyzeObjectEntry::getMinimumXValue( void ) const
{
  return this->m_MinimumXValue;
}


short int AnalyzeObjectEntry::getMinimumYValue( void ) const
{
  return this->m_MinimumYValue;
}


short int AnalyzeObjectEntry::getMinimumZValue( void ) const
{
  return this->m_MinimumZValue;
}


short int AnalyzeObjectEntry::getMaximumXValue( void ) const
{
  return this->m_MaximumXValue;
}


short int AnalyzeObjectEntry::getMaximumYValue( void ) const
{
  return this->m_MaximumYValue;
}


short int AnalyzeObjectEntry::getMaximumZValue( void ) const
{
  return this->m_MaximumZValue;
}


float AnalyzeObjectEntry::getOpacity( void ) const
{
  return this->m_Opacity;
}


int AnalyzeObjectEntry::getOpacityThickness( void ) const
{
  return this->m_OpacityThickness;
}


int AnalyzeObjectEntry::getBlendFactor( void ) const
{
  return this->m_BlendFactor;
}


Object * AnalyzeObjectEntry::getObjectPointer( void )
{
  return &(this->myObject);
}


bool AnalyzeObjectEntry::setName( const std::string name )
{
  if (name.size() > sizeof(this->m_Name))
  {
    strncpy( this->m_Name, name.c_str(),31 );
    fprintf(stderr, "String name is too long.  Please limit to %d characters.\n", static_cast<int>(sizeof(this->m_Name)));
    return false;
  }
  else
  {
    strcpy( this->m_Name, name.c_str() );
  }
  return true;
}


bool AnalyzeObjectEntry::setDisplayFlag( const int displayflag )
{
  this->m_DisplayFlag = displayflag;
  return true;
}


bool AnalyzeObjectEntry::setCopyFlag( const unsigned char copyflag )
{
  this->m_CopyFlag = copyflag;
  return true;
}


bool AnalyzeObjectEntry::setMirrorFlag( const unsigned char mirrorflag )
{
  this->m_MirrorFlag = mirrorflag;
  return true;
}


bool AnalyzeObjectEntry::setStatusFlag( const unsigned char statusflag )
{
  this->m_StatusFlag = statusflag;
  return true;
}


bool AnalyzeObjectEntry::setNeighborsUsedFlag( const unsigned char neighborsusedflag )
{
  this->m_NeighborsUsedFlag = neighborsusedflag;
  return true;
}


bool AnalyzeObjectEntry::setShades( const int shades )
{
  if (shades > 250)
  {
    printf("Maximum number of shades used for an Analyze objectmap is 250 for version 6.\n");
    this->m_Shades = 250;
  }
  else if (shades < 1)
  {
    printf("Object entry must have at least one shade.\n");
    this->m_Shades = 1;
  }
  else
  {
    this->m_Shades = shades;
  }
  return true;
}


bool AnalyzeObjectEntry::setStartColor( const int startred, const int startgreen, const int startblue )
{
  this->m_StartRed = startred;
  this->m_StartGreen = startgreen;
  this->m_StartBlue = startblue;
  return true;
}


bool AnalyzeObjectEntry::setStartRed( const int startred )
{
  this->m_StartRed = startred;
  return true;
}


bool AnalyzeObjectEntry::setStartGreen( const int startgreen )
{
  this->m_StartGreen = startgreen;
  return true;
}


bool AnalyzeObjectEntry::setStartBlue( const int startblue )
{
  this->m_StartBlue = startblue;
  return true;
}


bool AnalyzeObjectEntry::setEndColor( const int endred,const int endgreen, const int endblue )
{
  this->m_EndRed = endred;
  this->m_EndGreen = endgreen;
  this->m_EndBlue = endblue;
  return true;
}


bool AnalyzeObjectEntry::setEndRed( const int endred )
{
  this->m_EndRed = endred;
  return true;
}


bool AnalyzeObjectEntry::setEndGreen( const int endgreen )
{
  this->m_EndGreen = endgreen;
  return true;
}


bool AnalyzeObjectEntry::setEndBlue( const int endblue )
{
  this->m_EndBlue = endblue;
  return true;
}


bool AnalyzeObjectEntry::setRotation( const int xrotation, const int yrotation, const int zrotation )
{
  this->m_XRotation = xrotation;
  this->m_YRotation = yrotation;
  this->m_ZRotation = zrotation;
  return true;
}


bool AnalyzeObjectEntry::setTranslation( const int xtranslation, const int ytranslation,const int ztranslation )
{
  this->m_XTranslation = xtranslation;
  this->m_YTranslation = ytranslation;
  this->m_ZTranslation = ztranslation;
  return true;
}


bool AnalyzeObjectEntry::setCenter(const int xcenter,const int ycenter, const int zcenter )
{
  this->m_XCenter = xcenter;
  this->m_YCenter = ycenter;
  this->m_ZCenter = zcenter;
  return true;
}


bool AnalyzeObjectEntry::setXCenter( const int xcenter )
{
  this->m_XCenter = xcenter;
  return true;
}


bool AnalyzeObjectEntry::setYCenter( const int ycenter )
{
  this->m_YCenter = ycenter;
  return true;
}


bool AnalyzeObjectEntry::setZCenter( const int zcenter )
{
  this->m_ZCenter = zcenter;
  return true;
}


bool AnalyzeObjectEntry::setRotationIncrement( const int xrotationincrement, const int yrotationincrement, const int zrotationincrement )
{
  this->m_XRotationIncrement = xrotationincrement;
  this->m_YRotationIncrement = yrotationincrement;
  this->m_ZRotationIncrement = zrotationincrement;
  return true;
}


bool AnalyzeObjectEntry::setTranslationIncrement( const int xtranslationincrement, const int ytranslationincrement, const int ztranslationincrement )
{
  this->m_XTranslationIncrement = xtranslationincrement;
  this->m_YTranslationIncrement = ytranslationincrement;
  this->m_ZTranslationIncrement = ztranslationincrement;
  return true;
}


bool AnalyzeObjectEntry::setMinimumCoordinate( const int minimumxvalue, const int minimumyvalue, const int minimumzvalue )
{
  this->m_MinimumXValue = minimumxvalue;
  this->m_MinimumYValue = minimumyvalue;
  this->m_MinimumZValue = minimumzvalue;
  return true;
}


bool AnalyzeObjectEntry::setMinimumXValue( const int minimumxvalue )
{
  this->m_MinimumXValue = minimumxvalue;
  return true;
}


bool AnalyzeObjectEntry::setMinimumYValue( const int minimumyvalue )
{
  this->m_MinimumYValue = minimumyvalue;
  return true;
}


bool AnalyzeObjectEntry::setMinimumZValue( const int minimumzvalue )
{
  this->m_MinimumZValue = minimumzvalue;
  return true;
}


bool AnalyzeObjectEntry::setMaximumCoordinate( const int maximumxvalue, const int maximumyvalue, const int maximumzvalue )
{
  this->m_MaximumXValue = maximumxvalue;
  this->m_MaximumYValue = maximumyvalue;
  this->m_MaximumZValue = maximumzvalue;
  return true;
}


bool AnalyzeObjectEntry::setMaximumXValue( const int maximumxvalue )
{
  this->m_MaximumXValue = maximumxvalue;
  return true;
}


bool AnalyzeObjectEntry::setMaximumYValue( const int maximumyvalue )
{
  this->m_MaximumYValue = maximumyvalue;
  return true;
}


bool AnalyzeObjectEntry::setMaximumZValue( const int maximumzvalue )
{
  this->m_MaximumZValue = maximumzvalue;
  return true;
}


bool AnalyzeObjectEntry::setOpacity( const float opacity )
{
  this->m_Opacity = opacity;
  return true;
}


bool AnalyzeObjectEntry::setOpacityThickness( const int opacitythickness )
{
  this->m_OpacityThickness = opacitythickness;
  return true;
}


bool AnalyzeObjectEntry::setBlendFactor( const int BlendFactor )
{
  this->m_BlendFactor = BlendFactor;
  return true;
}
