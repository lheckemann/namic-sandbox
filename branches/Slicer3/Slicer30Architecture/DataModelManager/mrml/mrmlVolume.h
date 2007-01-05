/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/
#ifndef __mrmlVolume_h
#define __mrmlVolume_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

namespace mrml
{

class MRMLCommon_EXPORT Volume : public Object
{
public:
  /** Method for defining the name of the class */
  mrmlTypeMacro(Volume, Object);

  /** Method for creation through the object factory */
  mrmlNewMacro(Self);

protected:
  Volume() {};
  ~Volume() {};

private:
  Volume(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace mrml
#endif
