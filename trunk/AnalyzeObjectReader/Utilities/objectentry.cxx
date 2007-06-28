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



AnalyzeObjectEntry::AnalyzeObjectEntry( const AnalyzeObjectEntry & rhs )
{
//Copy construction is not allowed, so no need to fill this code in.
}

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
  this->m_MaximumXValue = 10;
  this->m_MaximumYValue = 0;
  this->m_MaximumZValue = 0;
  this->m_Opacity = 0.5;
  this->m_OpacityThickness = 1;
  this->m_BlendFactor = 0;
}

//AnalyzeObjectEntry & AnalyzeObjectEntry::operator=( const AnalyzeObjectEntry & rhs )
void AnalyzeObjectEntry::Copy( AnalyzeObjectEntry::Pointer rhs )
{
  //TODO:   This needs to be filled in!!!
  this->SetBlendFactor(rhs->GetBlendFactor());
  this->SetCenter(rhs->GetCenter());
  this->SetCopyFlag(rhs->GetCopyFlag());
  this->SetDisplayFlag(rhs->GetDisplayFlag());
  this->SetEndBlue(rhs->GetEndBlue());
  this->SetEndColor(rhs->GetEndColor());
  this->SetEndGreen(rhs->GetEndGreen());
  this->SetEndRed(rhs->GetEndRed());
  this->SetMaximumCoordinateValue(rhs->GetMaximumCoordinateValue());
  this->SetMaximumXValue(rhs->GetMaximumXValue());
  this->SetMaximumYValue(rhs->GetMaximumYValue());
  this->SetMaximumZValue(rhs->GetMaximumZValue());
  this->SetMinimumCoordinateValue(rhs->GetMinimumCoordinateValue());
  this->SetMinimumXValue(rhs->GetMinimumXValue());
  this->SetMinimumYValue(rhs->GetMinimumYValue());
  this->SetMinimumZValue(rhs->GetMinimumZValue());
  this->SetMirrorFlag(rhs->GetMirrorFlag());
  this->SetName(rhs->GetName());
  this->SetNeighborsUsedFlag(rhs->GetNeighborsUsedFlag());
  this->SetOpacity(rhs->GetOpacity());
  this->SetOpacityThickness(rhs->GetOpacityThickness());
  this->SetRotation(rhs->GetRotation());
  this->SetRotationIncrement(rhs->GetRotationIncrement());
  this->SetShades(rhs->GetShades());
  this->SetStartBlue(rhs->GetStartBlue());
  this->SetStartColor(rhs->GetStartColor());
  this->SetStartGreen(rhs->GetStartGreen());
  this->SetStartRed(rhs->GetStartRed());
  this->SetStatusFlag(rhs->GetStatusFlag());
  this->SetTranslation(rhs->GetTranslation());
  this->SetTranslationIncrement(rhs->GetTranslationIncrement());
  this->SetXCenter(rhs->GetXCenter());
  this->SetYCenter(rhs->GetYCenter());
  this->SetZCenter(rhs->GetZCenter());
}

}
