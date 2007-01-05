/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/
#include "vtkmrmlVolume.h"
#include "mrmlVolumeNode.h"
#include "vtkImageData.h"

namespace mrml
{
class vtkVolumeInternals
{
public:
  vtkVolumeInternals()
    {
    this->VolNode = VolumeNode::New();
    }
  VolumeNode::Pointer VolNode;
};

vtkVolume::vtkVolume()
{
  this->Internal = new vtkVolumeInternals;
}

vtkVolume::~vtkVolume()
{
  delete this->Internal;
}

void vtkVolume::SetSourceNode(VolumeNode* node)
{
  this->Internal->VolNode = node;
}

vtkImageData* vtkVolume::GetImageData()
{
  return NULL;
}

void vtkVolume::SetTargetNode(VolumeNode* node)
{
  (void)node;
}

void vtkVolume::SetSourceImage(vtkImageData* img)
{
  (void)img;
}

} // end namespace mrml
