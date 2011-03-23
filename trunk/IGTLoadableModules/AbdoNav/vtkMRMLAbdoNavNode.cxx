/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkMRMLAbdoNavNode.h"

/* MRML includes */
#include "vtkMRMLScene.h"

/* STL includes */
#include <limits>
#include <sstream>

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMRMLAbdoNavNode, "$Revision: $");
vtkStandardNewMacro(vtkMRMLAbdoNavNode);


//---------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLAbdoNavNode::CreateNodeInstance()
{
  // first try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLAbdoNavNode");
  if(ret)
    {
      return (vtkMRMLAbdoNavNode*)ret;
    }
  // if the factory was unable to create the object, then create it here
  return new vtkMRMLAbdoNavNode;
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::vtkMRMLAbdoNavNode()
{
  this->HideFromEditors = true;

  this->RelativeTrackingTransformID = NULL;
  this->RegistrationTransformID = NULL;
  this->RegistrationFiducialListID = NULL;
  this->TrackingSystemUsed = NULL;
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::~vtkMRMLAbdoNavNode()
{
  this->RelativeTrackingTransformID = NULL;
  this->RegistrationTransformID = NULL;
  this->RegistrationFiducialListID = NULL;
  this->TrackingSystemUsed = NULL;
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os, indent);

  os << indent << "RelativeTrackingTransformID: " << (this->RelativeTrackingTransformID ? this->RelativeTrackingTransformID : "(none)") << "\n";
  os << indent << "RegistrationTransformID: " << (this->RegistrationTransformID ? this->RegistrationTransformID : "(none)") << "\n";
  os << indent << "RegistrationFiducialListID: " << (this->RegistrationFiducialListID ? this->RegistrationFiducialListID : "(none)") << "\n";
  os << indent << "TrackingSystemUsed: " << (this->TrackingSystemUsed ? this->TrackingSystemUsed : "(none)") << "\n";
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "RelativeTrackingTransformID"))
      {
      this->SetRelativeTrackingTransformID(attValue);
      this->Scene->AddReferencedNodeID(this->RelativeTrackingTransformID, this);
      }
    else if (!strcmp(attName, "RegistrationTransformID"))
      {
      this->SetRegistrationTransformID(attValue);
      this->Scene->AddReferencedNodeID(this->RegistrationTransformID, this);
      }
    else if (!strcmp(attName, "RegistrationFiducialListID"))
      {
      this->SetRegistrationFiducialListID(attValue);
      this->Scene->AddReferencedNodeID(this->RegistrationFiducialListID, this);
      }
    else if (!strcmp(attName, "TrackingSystemUsed"))
      {
      this->SetTrackingSystemUsed(attValue);
      }
    }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::WriteXML(ostream& os, int nIndent)
{
  Superclass::WriteXML(os, nIndent);

  // write all MRML node attributes into output stream
  vtkIndent indent(nIndent);
  {
    std::stringstream ss;
    if (this->RelativeTrackingTransformID)
      {
      ss << this->RelativeTrackingTransformID;
      os << indent << " RelativeTrackingTransformID=\"" << ss.str() << "\"";
      }
  }
  {
    std::stringstream ss;
    if (this->RegistrationTransformID)
      {
      ss << this->RegistrationTransformID;
      os << indent << " RegistrationTransformID=\"" << ss.str() << "\"";
      }
  }
  {
    std::stringstream ss;
    if (this->RegistrationFiducialListID)
      {
      ss << this->RegistrationFiducialListID;
      os << indent << " RegistrationFiducialListID=\"" << ss.str() << "\"";
      }
  }
  {
    std::stringstream ss;
    if (this->TrackingSystemUsed)
      {
      ss << this->TrackingSystemUsed;
      os << indent << " TrackingSystemUsed=\"" << ss.str() << "\"";
      }
  }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::Copy(vtkMRMLNode* anode)
{
  Superclass::Copy(anode);
  vtkMRMLAbdoNavNode* node = (vtkMRMLAbdoNavNode*)anode;

  this->SetRelativeTrackingTransformID(node->GetRelativeTrackingTransformID());
  this->SetRegistrationTransformID(node->GetRegistrationTransformID());
  this->SetRegistrationFiducialListID(node->GetRegistrationFiducialListID());
  this->SetTrackingSystemUsed(node->GetTrackingSystemUsed());
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::UpdateReferenceID(const char* oldID, const char* newID)
{
  if (!strcmp(oldID, this->RelativeTrackingTransformID))
    {
    this->SetRelativeTrackingTransformID(newID);
    }
  else if (!strcmp(oldID, this->RegistrationTransformID))
    {
    this->SetRegistrationTransformID(newID);
    }
  else if (!strcmp(oldID, this->RegistrationFiducialListID))
    {
    this->SetRegistrationFiducialListID(newID);
    }
}
