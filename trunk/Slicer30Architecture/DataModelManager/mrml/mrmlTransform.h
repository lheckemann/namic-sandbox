/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/

#include "mrmlObject.h"

namespace mrml
{

class Transform : public Object
{
public:
  typedef Transform Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  mrmlTypeMacro(Self, Superclass);

  /** Method for creation through the object factory */
  mrmlNewMacro(Self);

protected:
  Transform() {};
  ~Transform() {};
private:
  Transform(const Transform&); //purposely not implemented
  void operator=(const Transform&); //purposely not implemented
};

} // end namespace mrml
