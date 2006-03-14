/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeData.cxx,v $
Date:      $Date: 2006/02/07 19:19:35 $
Version:   $Revision: 1.4 $

=========================================================================auto=*/
#include "vtkmrmlVolumeData.h"
#include "mrmlVolumeNode.h"
#include "vtkImageData.h"

namespace vtkmrml
{
//------------------------------------------------------------------------------
VolumeData::VolumeData()
{
  this->ImageData = vtkImageData::New();
}

//----------------------------------------------------------------------------
VolumeData::~VolumeData()
{
  this->ImageData->Delete();
}

//----------------------------------------------------------------------------
void VolumeData::PrintSelf(std::ostream& os, mrml::Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void VolumeData::SetSourceNode(mrml::VolumeNode* node)
{
}

} // namespace vtkmrml

