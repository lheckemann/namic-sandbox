/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/
#include "vtkmrmlModel.h"

#include "vtkPolyData.h"

namespace mrml
{

vtkModel::vtkModel()
{
  this->PolyData = vtkPolyData::New();
}

vtkModel::~vtkModel()
{
  this->PolyData->Delete();
}

vtkPolyData* vtkModel::GetPolyData()
{
  return this->PolyData;
}

} // end namespace mrml
