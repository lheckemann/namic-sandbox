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

class ModelInternals;
class MRMLCommon_EXPORT Model : public Object
{
public:
  typedef Model Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Self, Superclass);
  mrmlNewMacro(Self);

  Model();
  ~Model();

protected:
  /** Print the object information in a stream. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const {
    this->Superclass::PrintSelf(os,indent);
    }

private:
  ModelInternals *Internal;
};

} // end namespace mrml

#endif
