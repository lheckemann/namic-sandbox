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

namespace itk{
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
//Copy construction is not allowed, so no need to fill this code in.
}


AnalyzeObjectEntry::~AnalyzeObjectEntry( void )
{

}


AnalyzeObjectEntry & AnalyzeObjectEntry::operator=( const AnalyzeObjectEntry & rhs )
{

  //TODO:   This needs to be filled in!!!
  assert(0==1);
#if 0
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
#endif
  return *this;
}

}
