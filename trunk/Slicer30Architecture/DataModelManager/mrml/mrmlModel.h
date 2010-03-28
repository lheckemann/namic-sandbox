/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.12 $

=========================================================================auto=*/
#ifndef __mrmlModel_h
#define __mrmlModel_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

namespace mrml
{

class MRMLCommon_EXPORT Model : public Object
{
public:
  // Description:
  mrmlTypeMacro(Model, Object);
  mrmlNewMacro(Self);

protected:
  Model();
  ~Model();

  /** Print the object information in a stream. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const {
    this->Superclass::PrintSelf(os,indent);
    }

private:
};

} // end namespace mrml

#endif