/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   VTKMRML
  Module:    $RCSfile: mrmlVolumeData.h,v $
  Date:      $Date: 2006/02/11 17:20:11 $
  Version:   $Revision: 1.9 $

=========================================================================auto=*/
// .NAME mrmlVolumeData - Abstract Superclass for all specific types of MRML nodes.
// .SECTION Description
// This node encapsulates the functionality common to all types of MRML nodes.
// This includes member variables for ID, Description, and Options,
// as well as member functions to Copy() and Write().

#ifndef __vtkmrmlVolumeData_h
#define __vtkmrmlVolumeData_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

namespace mrml { class VolumeNode; }
class vtkImageData;
namespace vtkmrml
{
class MRMLCommon_EXPORT VolumeData : public mrml::Object
{
public:
  typedef VolumeData Self;
  typedef mrml::Object Superclass;
  typedef mrml::SmartPointer< Self > Pointer;
  typedef mrml::SmartPointer< const Self > ConstPointer;

  // Description:
  // Method for defining the name of the class
  mrmlTypeMacro(VolumeData, mrml::Object);

  void SetSourceNode(mrml::VolumeNode* node);

  mrmlGetObjectMacro(ImageData,vtkImageData);

protected:
  VolumeData();
  ~VolumeData();

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, mrml::Indent indent) const;

private:
  vtkImageData *ImageData;

  VolumeData(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace vtkmrml
#endif



