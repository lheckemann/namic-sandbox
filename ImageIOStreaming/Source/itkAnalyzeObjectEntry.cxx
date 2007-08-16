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
}

AnalyzeObjectEntry::AnalyzeObjectEntry( void )
{
  for (unsigned int i = 0; i < sizeof(this->m_Name); i++)
  {
    this->m_Name[i] = '\0';
  }
  this->SetDisplayFlag(1);
  this->SetCopyFlag(0);
  this->SetMirrorFlag(0);
  this->SetStatusFlag(0);
  this->SetNeighborsUsedFlag(0);
  this->SetShades(1);
  this->SetStartRed(0);
  this->SetStartGreen(0);
  this->SetStartBlue(0);
  this->SetEndRed(0);
  this->SetEndGreen(0);
  this->SetEndBlue(0);
  this->SetXRotation(0);
  this->SetYRotation(0);
  this->SetZRotation(0);
  this->SetXTranslation(0);
  this->SetYTranslation(0);
  this->SetZTranslation(0);
  this->SetXCenter(0);
  this->SetYCenter(0);
  this->SetZCenter(0);
  this->SetXRotationIncrement(0);
  this->SetYRotationIncrement(0);
  this->SetZRotationIncrement(0);
  this->SetXTranslationIncrement(0);
  this->SetYTranslationIncrement(0);
  this->SetZTranslationIncrement(0);
  this->SetMinimumXValue(0);
  this->SetMinimumYValue(0);
  this->SetMinimumZValue(0);
  this->SetMaximumXValue(0);
  this->SetMaximumYValue(0);
  this->SetMaximumZValue(0);
  this->SetOpacity(0.5);
  this->SetOpacityThickness(1);
  this->SetBlendFactor(0);
}

//AnalyzeObjectEntry & AnalyzeObjectEntry::operator=( const AnalyzeObjectEntry & rhs )

//Copy everything but the name.  Each ObjectEntry must have a unique name.
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
