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

/* VTK includes */
#include "vtkMatrix4x4.h"

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

  this->TrackingTransformID = NULL;
  this->RegistrationTransformID = NULL;
  this->RegistrationFiducialListID = NULL;
  this->TargetFiducialListID = NULL;
  this->GuidanceToolType = NULL;
  this->ToolBoxPropertiesFile = NULL;
  this->GuidanceTipOffset[0] = this->GuidanceTipOffset[1] = this->GuidanceTipOffset[2] = std::numeric_limits<float>::quiet_NaN();
  this->FRE = std::numeric_limits<double>::quiet_NaN();
  this->ElapsedTime = std::numeric_limits<double>::quiet_NaN();
  this->RecordedPositions = vtkCollection::New();
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::~vtkMRMLAbdoNavNode()
{
  this->TrackingTransformID = NULL;
  this->RegistrationTransformID = NULL;
  this->RegistrationFiducialListID = NULL;
  this->TargetFiducialListID = NULL;
  this->GuidanceToolType = NULL;
  this->ToolBoxPropertiesFile = NULL;
  this->GuidanceTipOffset[0] = this->GuidanceTipOffset[1] = this->GuidanceTipOffset[2] = std::numeric_limits<float>::quiet_NaN();
  this->FRE = std::numeric_limits<double>::quiet_NaN();
  this->ElapsedTime = std::numeric_limits<double>::quiet_NaN();
  if (this->RecordedPositions)
    {
    this->RecordedPositions->RemoveAllItems();
    this->RecordedPositions->Delete();
    this->RecordedPositions = NULL;
    }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << indent << "TrackingTransformID: " << (this->TrackingTransformID ? this->TrackingTransformID : "(none)") << "\n";
  os << indent << "RegistrationTransformID: " << (this->RegistrationTransformID ? this->RegistrationTransformID : "(none)") << "\n";
  os << indent << "RegistrationFiducialListID: " << (this->RegistrationFiducialListID ? this->RegistrationFiducialListID : "(none)") << "\n";
  os << indent << "TargetFiducialListID: " << (this->TargetFiducialListID ? this->TargetFiducialListID : "(none)") << "\n";
  os << indent << "GuidanceToolType: " << (this->GuidanceToolType ? this->GuidanceToolType : "(none)") << "\n";
  os << indent << "ToolBoxPropertiesFile: " << (this->ToolBoxPropertiesFile ? this->ToolBoxPropertiesFile : "(none)") << "\n";
  os << indent << "GuidanceTipOffset: " << this->GuidanceTipOffset[0] << " " << this->GuidanceTipOffset[1] << " " << this->GuidanceTipOffset[2] << "\n";
  os << indent << "FRE: " << this->FRE << "\n";
  os << indent << "ElapsedTime: " << this->ElapsedTime << "\n";
  os << indent << "RecordedPositions: " << (this->RecordedPositions ? this->RecordedPositions->GetNumberOfItems() : 0) << "\n";
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

  // read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "TrackingTransformID"))
      {
      this->SetTrackingTransformID(attValue);
      this->Scene->AddReferencedNodeID(this->TrackingTransformID, this);
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
    else if (!strcmp(attName, "TargetFiducialListID"))
      {
      this->SetTargetFiducialListID(attValue);
      this->Scene->AddReferencedNodeID(this->TargetFiducialListID, this);
      }
    else if (!strcmp(attName, "GuidanceToolType"))
      {
      this->SetGuidanceToolType(attValue);
      }
    else if (!strcmp(attName, "ToolBoxPropertiesFile"))
      {
      this->SetToolBoxPropertiesFile(attValue);
      }
    else if (!strcmp(attName, "GuidanceTipOffset"))
      {
      std::stringstream ss;
      float val;
      ss << attValue;
      for (int i = 0; i < 3; i++)
        {
        ss >> val;
        this->GuidanceTipOffset[i] = val;
        }
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
    if (this->TrackingTransformID)
      {
      ss << this->TrackingTransformID;
      os << indent << " TrackingTransformID=\"" << ss.str() << "\"";
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
    if (this->TargetFiducialListID)
      {
      ss << this->TargetFiducialListID;
      os << indent << " TargetFiducialListID=\"" << ss.str() << "\"";
      }
  }
  {
    std::stringstream ss;
    if (this->GuidanceToolType)
      {
      ss << this->GuidanceToolType;
      os << indent << " GuidanceToolType=\"" << ss.str() << "\"";
      }
  }
  {
    std::stringstream ss;
    if (this->ToolBoxPropertiesFile)
      {
      ss << this->ToolBoxPropertiesFile;
      os << indent << " ToolBoxPropertiesFile=\"" << ss.str() << "\"";
      }
  }
  {
    os.setf(ios::scientific, ios::floatfield);
    os.precision(8);
    os << indent << " GuidanceTipOffset=\"" << this->GuidanceTipOffset[0] << " " << this->GuidanceTipOffset[1] << " " << this->GuidanceTipOffset[2] << "\"";
    os.unsetf(ios::floatfield);
    os.precision(6);
  }
  {
    os.setf(ios::scientific, ios::floatfield);
    os.precision(8);
    os << indent << " FRE=\"" << this->FRE << "\"";
    os.unsetf(ios::floatfield);
    os.precision(6);
  }
  {
    os.setf(ios::scientific, ios::floatfield);
    os.precision(8);
    os << indent << " ElapsedTime=\"" << this->ElapsedTime << "\"";
    os.unsetf(ios::floatfield);
    os.precision(6);
  }
  {
    if (this->RecordedPositions)
      {
      os.setf(ios::scientific, ios::floatfield);
      os.precision(8);
      for (int i = 0; i < this->RecordedPositions->GetNumberOfItems(); i++)
        {
        vtkMatrix4x4* recPos = vtkMatrix4x4::SafeDownCast(this->RecordedPositions->GetItemAsObject(i));
        os << indent << " RecordedPosition" << i << "=\"" << recPos->Element[0][3] << ",," << recPos->Element[1][3] << ",," << recPos->Element[2][3] << "\"";
        }
      os.unsetf(ios::floatfield);
      os.precision(6);
      }
  }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::Copy(vtkMRMLNode* anode)
{
  Superclass::Copy(anode);
  vtkMRMLAbdoNavNode* node = (vtkMRMLAbdoNavNode*)anode;

  this->SetTrackingTransformID(node->GetTrackingTransformID());
  this->SetRegistrationTransformID(node->GetRegistrationTransformID());
  this->SetRegistrationFiducialListID(node->GetRegistrationFiducialListID());
  this->SetTargetFiducialListID(node->GetTargetFiducialListID());
  this->SetGuidanceToolType(node->GetGuidanceToolType());
  this->SetToolBoxPropertiesFile(node->GetToolBoxPropertiesFile());
  this->SetGuidanceTipOffset(node->GetGuidanceTipOffset());
  this->SetFRE(node->GetFRE());
  this->SetElapsedTime(node->GetElapsedTime());
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::UpdateReferenceID(const char* oldID, const char* newID)
{
  if (!strcmp(oldID, this->TrackingTransformID))
    {
    this->SetTrackingTransformID(newID);
    }
  else if (!strcmp(oldID, this->RegistrationTransformID))
    {
    this->SetRegistrationTransformID(newID);
    }
  else if (!strcmp(oldID, this->RegistrationFiducialListID))
    {
    this->SetRegistrationFiducialListID(newID);
    }
  else if (!strcmp(oldID, this->TargetFiducialListID))
    {
    this->SetTargetFiducialListID(newID);
    }
}
