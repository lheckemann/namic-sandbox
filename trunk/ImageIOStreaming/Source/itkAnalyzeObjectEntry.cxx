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
#include "itkAnalyzeObjectEntry.h"

namespace itk{



AnalyzeObjectEntry::AnalyzeObjectEntry( const AnalyzeObjectEntry & rhs )
{
//Copy construction is not allowed, so no need to fill this code in.
}
AnalyzeObjectEntry::~AnalyzeObjectEntry( void )
{
  //if(this->m_Name)
  //{
  //  this->m_Name = "";
  //}
  if(this->m_DisplayFlag)
  {
    this->m_DisplayFlag = NULL;
  }  
  if(this->m_CopyFlag)
  {
    this->m_CopyFlag = NULL;
  }
  if(this->m_MirrorFlag)
  {
    this->m_MirrorFlag = NULL;
  }
  if(this->m_StatusFlag)
  {
    this->m_StatusFlag = NULL;
  }
  if(this->m_NeighborsUsedFlag)
  {
    this->m_NeighborsUsedFlag = NULL;
  }
  if(this->m_Shades)
  {
    this->m_Shades = NULL;
  }
  if(this->m_StartRed)
  {
    this->m_StartRed = NULL;
  }
  if(this->m_StartGreen)
  {
    this->m_StartGreen = NULL;
  }
  if(this->m_StartBlue)
  {
    this->m_StartBlue = NULL;
  }
  if(this->m_EndRed)
  {
    this->m_EndRed = NULL;
  }
  if(this->m_EndGreen)
  {
    this->m_EndGreen = NULL;
  }
  if(this->m_EndBlue)
  {
    this->m_EndBlue = NULL;
  }
  if(this->m_XRotation)
  {
    this->m_XRotation = NULL;
  }
  if(this->m_YRotation)
  {
    this->m_YRotation = NULL;
  }
  if(this->m_ZRotation)
  {
    this->m_ZRotation = NULL;
  }
  if(this->m_XTranslation)
  {
    this->m_XTranslation = NULL;
  }
  if(this->m_YTranslation)
  {
    this->m_YTranslation = NULL;
  }
  if(this->m_ZTranslation)
  {
    this->m_ZTranslation = NULL;
  }
  if(this->m_XCenter)
  {
    this->m_XCenter = NULL;
  }
  if(this->m_YCenter)
  {
    this->m_YCenter = NULL;
  }
  if(this->m_ZCenter)
  {
    this->m_ZCenter = NULL;
  }
  if(this->m_XRotationIncrement)
  {
    this->m_XRotationIncrement = NULL;
  }
  if(this->m_YRotationIncrement)
  {
    this->m_YRotationIncrement = NULL;
  }
  if(this->m_ZRotationIncrement)
  {
    this->m_ZRotationIncrement = NULL;
  }
  if(this->m_XTranslationIncrement)
  {
    this->m_XTranslationIncrement = NULL;
  }
  if(this->m_YTranslationIncrement)
  {
    this->m_YTranslationIncrement = NULL;
  }
  if(this->m_ZTranslationIncrement)
  {
    this->m_ZTranslationIncrement = NULL;
  }
  if(this->m_MinimumXValue)
  {
    this->m_MinimumXValue = NULL;
  }
  if(this->m_MinimumYValue)
  {
    this->m_MinimumYValue = NULL;
  }
  if(this->m_MinimumZValue)
  {
    this->m_MinimumZValue = NULL;
  }
  if(this->m_MaximumXValue)
  {
    this->m_MaximumXValue = NULL;
  }
  if(this->m_MaximumYValue)
  {
    this->m_MaximumYValue = NULL;
  }
  if(this->m_MaximumZValue)
  {
    this->m_MaximumZValue = NULL;
  }
  if(this->m_Opacity)
  {
    this->m_Opacity = NULL;
  }
  if(this->m_OpacityThickness)
  {
    this->m_OpacityThickness = NULL;
  }
  if(this->m_BlendFactor)
  {
    this->m_BlendFactor = NULL;
  }
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

void AnalyzeObjectEntry::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

}
