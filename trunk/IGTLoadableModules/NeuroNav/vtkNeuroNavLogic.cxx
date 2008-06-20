/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkNeuroNavLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerColorLogic.h"

#include "vtkCylinderSource.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"


#include "vtkMultiThreader.h"


vtkCxxRevisionMacro(vtkNeuroNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkNeuroNavLogic);

//---------------------------------------------------------------------------
vtkNeuroNavLogic::vtkNeuroNavLogic()
{

  this->SliceDriver[0] = vtkNeuroNavLogic::SLICE_DRIVER_USER;
  this->SliceDriver[1] = vtkNeuroNavLogic::SLICE_DRIVER_USER;
  this->SliceDriver[2] = vtkNeuroNavLogic::SLICE_DRIVER_USER;

  // If the following code doesn't work, slice nodes should be obtained from application GUI
  this->SliceNode[0] = NULL;
  this->SliceNode[1] = NULL;
  this->SliceNode[2] = NULL;

  this->NeedRealtimeImageUpdate0 = 0;
  this->NeedRealtimeImageUpdate1 = 0;
  this->NeedRealtimeImageUpdate2 = 0;
  this->ImagingControl = 0;

  // Timer Handling

  this->EnableOblique = false;
  this->FreezePlane   = false;

}


//---------------------------------------------------------------------------
vtkNeuroNavLogic::~vtkNeuroNavLogic()
{


}


//---------------------------------------------------------------------------
void vtkNeuroNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkNeuroNavLogic:             " << this->GetClassName() << "\n";

}






