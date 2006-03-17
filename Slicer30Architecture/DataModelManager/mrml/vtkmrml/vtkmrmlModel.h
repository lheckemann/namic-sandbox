/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/
#ifndef __vtkmrmlModel_h
#define __vtkmrmlModel_h

#include "mrmlModel.h"

class vtkPolyData;
namespace mrml
{
class MRMLCommon_EXPORT vtkModel : public Model
{
public:
  typedef vtkModel Self;
  typedef Model Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Self, Superclass);
  mrmlNewMacro(Self);

  vtkPolyData* GetPolyData();

protected:
  vtkModel();
  ~vtkModel();

  /** Print the object information in a stream. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const {
    this->Superclass::PrintSelf(os,indent);
    }

private:
  vtkPolyData* PolyData;
};

} // end namespace mrml

#endif
