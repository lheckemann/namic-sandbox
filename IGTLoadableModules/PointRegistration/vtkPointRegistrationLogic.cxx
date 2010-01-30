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
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkPointRegistrationLogic.h"

#include "vtkMatrix4x4.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkLandmarkTransform.h"
#include "vtkMRMLNode.h"

vtkCxxRevisionMacro(vtkPointRegistrationLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkPointRegistrationLogic);

//---------------------------------------------------------------------------
vtkPointRegistrationLogic::vtkPointRegistrationLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPointRegistrationLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkPointRegistrationLogic::~vtkPointRegistrationLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkPointRegistrationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkPointRegistrationLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkPointRegistrationLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkPointRegistrationLogic *self = reinterpret_cast<vtkPointRegistrationLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPointRegistrationLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkPointRegistrationLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkPointRegistrationLogic::GetCurrentPosition(vtkMRMLNode* tn, double *px, double *py, double *pz)
{
  *px = 0.0;
  *py = 0.0;
  *pz = 0.0;

  if (!tn)
    {
    return;
    }

  vtkMatrix4x4* transform;
  
  transform = (vtkMRMLLinearTransformNode::SafeDownCast(tn))->GetMatrixTransformToParent();

  if (transform)
    {
    // set volume position
    *px = transform->GetElement(0, 3);
    *py = transform->GetElement(1, 3);
    *pz = transform->GetElement(2, 3);
    }
}

//----------------------------------------------------------------------------
int vtkPointRegistrationLogic::PerformPatientToImageRegistration(vtkPoints* patPoints, vtkPoints* imgPoints, vtkMatrix4x4* regMatrix)

{
  //Check if same number of points
  if ((patPoints->GetNumberOfPoints()) != (imgPoints->GetNumberOfPoints()))
    {
    std::cerr << "Patient and Image coordinates have different number of points" << std::endl;
    return 1;
    }

  vtkLandmarkTransform* initialT = vtkLandmarkTransform::New();
  //Patient space
  initialT->SetSourceLandmarks(patPoints);

  //Image space
  initialT->SetTargetLandmarks(imgPoints);

  //Calculate registration matrix
  initialT->SetModeToRigidBody();
  initialT->GetMatrix(regMatrix);
  regMatrix->Print(std::cerr);
  initialT->Delete();
  return 0;
  
}




