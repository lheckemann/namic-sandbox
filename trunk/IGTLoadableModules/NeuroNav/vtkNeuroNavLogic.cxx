/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkNeuroNavLogic.h"

#include "vtkLandmarkTransform.h"
#include "vtkCylinderSource.h"

#include "vtkSlicerConfigure.h" /* Slicer3_USE_* */

#ifdef Slicer3_USE_IGSTK
#include "igstkAuroraTracker.h"
#endif

vtkCxxRevisionMacro(vtkNeuroNavLogic, "$Revision: 1.9.12.1 $");

vtkNeuroNavLogic* vtkNeuroNavLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkNeuroNavLogic");
  if(ret)
    {
      return (vtkNeuroNavLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkNeuroNavLogic;
}


vtkNeuroNavLogic::vtkNeuroNavLogic()
{
#ifdef Slicer3_USE_IGSTK
  igstk::RealTimeClock::Initialize();
#endif
}



vtkNeuroNavLogic::~vtkNeuroNavLogic()
{

}



void vtkNeuroNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkObject::PrintSelf(os, indent);

    os << indent << "vtkNeuroNavLogic:             " << this->GetClassName() << "\n";

}

