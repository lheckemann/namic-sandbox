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
//
// Name: objectentry.cpp
// Author: John Dill
// Date: 5/17/00
//

#include "objectentry.h"

uiig::CObjectEntry::CObjectEntry( void )
{
  for (unsigned int i = 0; i < sizeof(myObject.Name); i++)
  {
    myObject.Name[i] = '\0';
  }
  myObject.DisplayFlag = 1;
  myObject.CopyFlag = 0;
  myObject.MirrorFlag = 0;
  myObject.StatusFlag = 0;
  myObject.NeighborsUsedFlag = 0;
  myObject.Shades = 1;
  myObject.StartRed = 0;
  myObject.StartGreen = 0;
  myObject.StartBlue = 0;
  myObject.EndRed = 0;
  myObject.EndGreen = 0;
  myObject.EndBlue = 0;
  myObject.XRotation = 0;
  myObject.YRotation = 0;
  myObject.ZRotation = 0;
  myObject.XTranslation = 0;
  myObject.YTranslation = 0;
  myObject.ZTranslation = 0;
  myObject.XCenter = 0;
  myObject.YCenter = 0;
  myObject.ZCenter = 0;
  myObject.XRotationIncrement = 0;
  myObject.YRotationIncrement = 0;
  myObject.ZRotationIncrement = 0;
  myObject.XTranslationIncrement = 0;
  myObject.YTranslationIncrement = 0;
  myObject.ZTranslationIncrement = 0;
  myObject.MinimumXValue = 0;
  myObject.MinimumYValue = 0;
  myObject.MinimumZValue = 0;
  myObject.MaximumXValue = 0;
  myObject.MaximumYValue = 0;
  myObject.MaximumZValue = 0;
  myObject.Opacity = 0.5;
  myObject.OpacityThickness = 1;
  myObject.Dummy = 0;
}


uiig::CObjectEntry::CObjectEntry( const uiig::CObjectEntry & rhs )
{
  const char * name = rhs.getNameASCII();

  for (unsigned int i = 0; i < sizeof(myObject.Name); i++)
  {
    myObject.Name[i] = name[i];
  }

  myObject.DisplayFlag = rhs.getDisplayFlag();
  myObject.CopyFlag = rhs.getCopyFlag();
  myObject.MirrorFlag = rhs.getMirrorFlag();
  myObject.StatusFlag = rhs.getStatusFlag();
  myObject.NeighborsUsedFlag = rhs.getNeighborsUsedFlag();
  myObject.Shades = rhs.getShades();
  myObject.StartRed = rhs.getStartRed();
  myObject.StartGreen = rhs.getStartGreen();
  myObject.StartBlue = rhs.getStartBlue();
  myObject.EndRed = rhs.getEndRed();
  myObject.EndGreen = rhs.getEndGreen();
  myObject.EndBlue = rhs.getEndBlue();
  myObject.XRotation = rhs.getXRotation();
  myObject.YRotation = rhs.getYRotation();
  myObject.ZRotation = rhs.getZRotation();
  myObject.XTranslation = rhs.getXTranslation();
  myObject.YTranslation = rhs.getYTranslation();
  myObject.ZTranslation = rhs.getZTranslation();
  myObject.XCenter = rhs.getXCenter();
  myObject.YCenter = rhs.getYCenter();
  myObject.ZCenter = rhs.getZCenter();
  myObject.XRotationIncrement = rhs.getXRotationIncrement();
  myObject.YRotationIncrement = rhs.getYRotationIncrement();
  myObject.ZRotationIncrement = rhs.getZRotationIncrement();
  myObject.XTranslationIncrement = rhs.getXTranslationIncrement();
  myObject.YTranslationIncrement = rhs.getYTranslationIncrement();
  myObject.ZTranslationIncrement = rhs.getZTranslationIncrement();
  myObject.MinimumXValue = rhs.getMinimumXValue();
  myObject.MinimumYValue = rhs.getMinimumYValue();
  myObject.MinimumZValue = rhs.getMinimumZValue();
  myObject.MaximumXValue = rhs.getMaximumXValue();
  myObject.MaximumYValue = rhs.getMaximumYValue();
  myObject.MaximumZValue = rhs.getMaximumZValue();
  myObject.Opacity = rhs.getOpacity();
  myObject.OpacityThickness = rhs.getOpacityThickness();
  myObject.Dummy = rhs.getDummy();
}


uiig::CObjectEntry::~CObjectEntry( void )
{

}


uiig::CObjectEntry & uiig::CObjectEntry::operator=( const uiig::CObjectEntry & rhs )
{
  const char * name = rhs.getNameASCII();

  for (unsigned int i = 0; i < sizeof(myObject.Name); i++)
    myObject.Name[i] = name[i];

  myObject.DisplayFlag = rhs.getDisplayFlag();
  myObject.CopyFlag = rhs.getCopyFlag();
  myObject.MirrorFlag = rhs.getMirrorFlag();
  myObject.StatusFlag = rhs.getStatusFlag();
  myObject.NeighborsUsedFlag = rhs.getNeighborsUsedFlag();
  myObject.Shades = rhs.getShades();
  myObject.StartRed = rhs.getStartRed();
  myObject.StartGreen = rhs.getStartGreen();
  myObject.StartBlue = rhs.getStartBlue();
  myObject.EndRed = rhs.getEndRed();
  myObject.EndGreen = rhs.getEndGreen();
  myObject.EndBlue = rhs.getEndBlue();
  myObject.XRotation = rhs.getXRotation();
  myObject.YRotation = rhs.getYRotation();
  myObject.ZRotation = rhs.getZRotation();
  myObject.XTranslation = rhs.getXTranslation();
  myObject.YTranslation = rhs.getYTranslation();
  myObject.ZTranslation = rhs.getZTranslation();
  myObject.XCenter = rhs.getXCenter();
  myObject.YCenter = rhs.getYCenter();
  myObject.ZCenter = rhs.getZCenter();
  myObject.XRotationIncrement = rhs.getXRotationIncrement();
  myObject.YRotationIncrement = rhs.getYRotationIncrement();
  myObject.ZRotationIncrement = rhs.getZRotationIncrement();
  myObject.XTranslationIncrement = rhs.getXTranslationIncrement();
  myObject.YTranslationIncrement = rhs.getYTranslationIncrement();
  myObject.ZTranslationIncrement = rhs.getZTranslationIncrement();
  myObject.MinimumXValue = rhs.getMinimumXValue();
  myObject.MinimumYValue = rhs.getMinimumYValue();
  myObject.MinimumZValue = rhs.getMinimumZValue();
  myObject.MaximumXValue = rhs.getMaximumXValue();
  myObject.MaximumYValue = rhs.getMaximumYValue();
  myObject.MaximumZValue = rhs.getMaximumZValue();
  myObject.Opacity = rhs.getOpacity();
  myObject.OpacityThickness = rhs.getOpacityThickness();
  myObject.Dummy = rhs.getDummy();

  return *this;
}


std::string uiig::CObjectEntry::getName( void ) const
{
  std::string s(myObject.Name);
  return s;
}


const char * uiig::CObjectEntry::getNameASCII( void ) const
{
  return myObject.Name;
}


int uiig::CObjectEntry::getDisplayFlag( void ) const
{
  return myObject.DisplayFlag;
}


unsigned char uiig::CObjectEntry::getCopyFlag( void ) const
{
  return myObject.CopyFlag;
}


unsigned char uiig::CObjectEntry::getMirrorFlag( void ) const
{
  return myObject.MirrorFlag;
}


unsigned char uiig::CObjectEntry::getStatusFlag( void ) const
{
  return myObject.StatusFlag;
}


unsigned char uiig::CObjectEntry::getNeighborsUsedFlag( void ) const
{
  return myObject.NeighborsUsedFlag;
}


int uiig::CObjectEntry::getShades( void ) const
{
  return myObject.Shades;
}


int uiig::CObjectEntry::getStartRed( void ) const
{
  return myObject.StartRed;
}


int uiig::CObjectEntry::getStartGreen( void ) const
{
  return myObject.StartGreen;
}


int uiig::CObjectEntry::getStartBlue( void ) const
{
  return myObject.StartBlue;
}


int uiig::CObjectEntry::getEndRed( void ) const
{
  return myObject.EndRed;
}


int uiig::CObjectEntry::getEndGreen( void ) const
{
  return myObject.EndGreen;
}


int uiig::CObjectEntry::getEndBlue( void ) const
{
  return myObject.EndBlue;
}


int uiig::CObjectEntry::getXRotation( void ) const
{
  return myObject.XRotation;
}


int uiig::CObjectEntry::getYRotation( void ) const
{
  return myObject.YRotation;
}


int uiig::CObjectEntry::getZRotation( void ) const
{
  return myObject.ZRotation;
}


int uiig::CObjectEntry::getXTranslation( void ) const
{
  return myObject.XTranslation;
}


int uiig::CObjectEntry::getYTranslation( void ) const
{
  return myObject.YTranslation;
}


int uiig::CObjectEntry::getZTranslation( void ) const
{
  return myObject.ZTranslation;
}


int uiig::CObjectEntry::getXCenter( void ) const
{
  return myObject.XCenter;
}


int uiig::CObjectEntry::getYCenter( void ) const
{
  return myObject.YCenter;
}


int uiig::CObjectEntry::getZCenter( void ) const
{
  return myObject.ZCenter;
}


int uiig::CObjectEntry::getXRotationIncrement( void ) const
{
  return myObject.XRotationIncrement;
}


int uiig::CObjectEntry::getYRotationIncrement( void ) const
{
  return myObject.YRotationIncrement;
}


int uiig::CObjectEntry::getZRotationIncrement( void ) const
{
  return myObject.ZRotationIncrement;
}


int uiig::CObjectEntry::getXTranslationIncrement( void ) const
{
  return myObject.XTranslationIncrement;
}


int uiig::CObjectEntry::getYTranslationIncrement( void ) const
{
  return myObject.YTranslationIncrement;
}


int uiig::CObjectEntry::getZTranslationIncrement( void ) const
{
  return myObject.ZTranslationIncrement;
}


short int uiig::CObjectEntry::getMinimumXValue( void ) const
{
  return myObject.MinimumXValue;
}


short int uiig::CObjectEntry::getMinimumYValue( void ) const
{
  return myObject.MinimumYValue;
}


short int uiig::CObjectEntry::getMinimumZValue( void ) const
{
  return myObject.MinimumZValue;
}


short int uiig::CObjectEntry::getMaximumXValue( void ) const
{
  return myObject.MaximumXValue;
}


short int uiig::CObjectEntry::getMaximumYValue( void ) const
{
  return myObject.MaximumYValue;
}


short int uiig::CObjectEntry::getMaximumZValue( void ) const
{
  return myObject.MaximumZValue;
}


float uiig::CObjectEntry::getOpacity( void ) const
{
  return myObject.Opacity;
}


int uiig::CObjectEntry::getOpacityThickness( void ) const
{
  return myObject.OpacityThickness;
}


int uiig::CObjectEntry::getDummy( void ) const
{
  return myObject.Dummy;
}


uiig::Object * uiig::CObjectEntry::getObjectPointer( void )
{
  return &(this->myObject);
}


bool uiig::CObjectEntry::setName( const std::string name )
{
  if (name.size() > sizeof(myObject.Name))
  {
    strncpy( myObject.Name, name.c_str(),31 );
    fprintf(stderr, "String name is too long.  Please limit to %d characters.\n", static_cast<int>(sizeof(myObject.Name)));
    return false;
  }
  else
  {
    strcpy( myObject.Name, name.c_str() );
  }
  return true;
}


bool uiig::CObjectEntry::setDisplayFlag( const int displayflag )
{
  myObject.DisplayFlag = displayflag;
  return true;
}


bool uiig::CObjectEntry::setCopyFlag( const unsigned char copyflag )
{
  myObject.CopyFlag = copyflag;
  return true;
}


bool uiig::CObjectEntry::setMirrorFlag( const unsigned char mirrorflag )
{
  myObject.MirrorFlag = mirrorflag;
  return true;
}


bool uiig::CObjectEntry::setStatusFlag( const unsigned char statusflag )
{
  myObject.StatusFlag = statusflag;
  return true;
}


bool uiig::CObjectEntry::setNeighborsUsedFlag( const unsigned char neighborsusedflag )
{
  myObject.NeighborsUsedFlag = neighborsusedflag;
  return true;
}


bool uiig::CObjectEntry::setShades( const int shades )
{
  if (shades > 250)
  {
    printf("Maximum number of shades used for an Analyze objectmap is 250 for version 6.\n");
    myObject.Shades = 250;
  }
  else if (shades < 1)
  {
    printf("Object entry must have at least one shade.\n");
    myObject.Shades = 1;
  }
  else
  {
    myObject.Shades = shades;
  }
  return true;
}


bool uiig::CObjectEntry::setStartColor( const int startred, const int startgreen, const int startblue )
{
  myObject.StartRed = startred;
  myObject.StartGreen = startgreen;
  myObject.StartBlue = startblue;
  return true;
}


bool uiig::CObjectEntry::setStartRed( const int startred )
{
  myObject.StartRed = startred;
  return true;
}


bool uiig::CObjectEntry::setStartGreen( const int startgreen )
{
  myObject.StartGreen = startgreen;
  return true;
}


bool uiig::CObjectEntry::setStartBlue( const int startblue )
{
  myObject.StartBlue = startblue;
  return true;
}


bool uiig::CObjectEntry::setEndColor( const int endred,const int endgreen, const int endblue )
{
  myObject.EndRed = endred;
  myObject.EndGreen = endgreen;
  myObject.EndBlue = endblue;
  return true;
}


bool uiig::CObjectEntry::setEndRed( const int endred )
{
  myObject.EndRed = endred;
  return true;
}


bool uiig::CObjectEntry::setEndGreen( const int endgreen )
{
  myObject.EndGreen = endgreen;
  return true;
}


bool uiig::CObjectEntry::setEndBlue( const int endblue )
{
  myObject.EndBlue = endblue;
  return true;
}


bool uiig::CObjectEntry::setRotation( const int xrotation, const int yrotation, const int zrotation )
{
  myObject.XRotation = xrotation;
  myObject.YRotation = yrotation;
  myObject.ZRotation = zrotation;
  return true;
}


bool uiig::CObjectEntry::setTranslation( const int xtranslation, const int ytranslation,const int ztranslation )
{
  myObject.XTranslation = xtranslation;
  myObject.YTranslation = ytranslation;
  myObject.ZTranslation = ztranslation;
  return true;
}


bool uiig::CObjectEntry::setCenter(const int xcenter,const int ycenter, const int zcenter )
{
  myObject.XCenter = xcenter;
  myObject.YCenter = ycenter;
  myObject.ZCenter = zcenter;
  return true;
}


bool uiig::CObjectEntry::setXCenter( const int xcenter )
{
  myObject.XCenter = xcenter;
  return true;
}


bool uiig::CObjectEntry::setYCenter( const int ycenter )
{
  myObject.YCenter = ycenter;
  return true;
}


bool uiig::CObjectEntry::setZCenter( const int zcenter )
{
  myObject.ZCenter = zcenter;
  return true;
}


bool uiig::CObjectEntry::setRotationIncrement( const int xrotationincrement, const int yrotationincrement, const int zrotationincrement )
{
  myObject.XRotationIncrement = xrotationincrement;
  myObject.YRotationIncrement = yrotationincrement;
  myObject.ZRotationIncrement = zrotationincrement;
  return true;
}


bool uiig::CObjectEntry::setTranslationIncrement( const int xtranslationincrement, const int ytranslationincrement, const int ztranslationincrement )
{
  myObject.XTranslationIncrement = xtranslationincrement;
  myObject.YTranslationIncrement = ytranslationincrement;
  myObject.ZTranslationIncrement = ztranslationincrement;
  return true;
}


bool uiig::CObjectEntry::setMinimumCoordinate( const int minimumxvalue, const int minimumyvalue, const int minimumzvalue )
{
  myObject.MinimumXValue = minimumxvalue;
  myObject.MinimumYValue = minimumyvalue;
  myObject.MinimumZValue = minimumzvalue;
  return true;
}


bool uiig::CObjectEntry::setMinimumXValue( const int minimumxvalue )
{
  myObject.MinimumXValue = minimumxvalue;
  return true;
}


bool uiig::CObjectEntry::setMinimumYValue( const int minimumyvalue )
{
  myObject.MinimumYValue = minimumyvalue;
  return true;
}


bool uiig::CObjectEntry::setMinimumZValue( const int minimumzvalue )
{
  myObject.MinimumZValue = minimumzvalue;
  return true;
}


bool uiig::CObjectEntry::setMaximumCoordinate( const int maximumxvalue, const int maximumyvalue, const int maximumzvalue )
{
  myObject.MaximumXValue = maximumxvalue;
  myObject.MaximumYValue = maximumyvalue;
  myObject.MaximumZValue = maximumzvalue;
  return true;
}


bool uiig::CObjectEntry::setMaximumXValue( const int maximumxvalue )
{
  myObject.MaximumXValue = maximumxvalue;
  return true;
}


bool uiig::CObjectEntry::setMaximumYValue( const int maximumyvalue )
{
  myObject.MaximumYValue = maximumyvalue;
  return true;
}


bool uiig::CObjectEntry::setMaximumZValue( const int maximumzvalue )
{
  myObject.MaximumZValue = maximumzvalue;
  return true;
}


bool uiig::CObjectEntry::setOpacity( const float opacity )
{
  myObject.Opacity = opacity;
  return true;
}


bool uiig::CObjectEntry::setOpacityThickness( const int opacitythickness )
{
  myObject.OpacityThickness = opacitythickness;
  return true;
}


bool uiig::CObjectEntry::setDummy( const int dummy )
{
  myObject.Dummy = dummy;
  return true;
}
