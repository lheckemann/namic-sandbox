/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/

#ifndef __vtkmrmlVolume_h
#define __vtkmrmlVolume_h

#include "mrmlVolume.h"
#include "mrmlObjectFactory.h"

class vtkImageData;
namespace mrml
{

class VolumeNode;
class vtkVolume : public Volume
{
public:
  typedef vtkVolume Self;
  typedef Volume Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  mrmlTypeMacro(Self, Superclass);

  /** Method for creation through the object factory */
  mrmlNewMacro(Self);

  void SetSourceNode(VolumeNode* node);
  vtkImageData* GetImageData();

  void SetTargetNode(VolumeNode* node);
  void SetSourceImage(vtkImageData* );

protected:
  vtkVolume();
  ~vtkVolume();

private:
  vtkVolume(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace mrml
#endif
