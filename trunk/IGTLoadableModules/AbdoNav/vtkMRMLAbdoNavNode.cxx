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
  this->TrackerTransformNodeID = NULL;
  this->TrackingSystemUsed = NULL;
  this->SetGuidanceNeedleTip(std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN());
  this->SetGuidanceNeedleSecond(std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN());

  this->HideFromEditors = true;
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::~vtkMRMLAbdoNavNode()
{
  this->TrackerTransformNodeID = NULL;
  this->TrackingSystemUsed = NULL;
  this->SetGuidanceNeedleTip(std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN());
  this->SetGuidanceNeedleSecond(std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN());
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os, indent);

  os << indent << "TrackerTransformNodeID: " << (this->TrackerTransformNodeID ? this->TrackerTransformNodeID : "(none)") << "\n";
  os << indent << "TrackingSystemUsed: " << (this->TrackingSystemUsed ? this->TrackingSystemUsed : "(none)") << "\n";
  os << indent << "GuidanceNeedleTip: " << this->GuidanceNeedleTip[0] << " " << this->GuidanceNeedleTip[1] << " " << this->GuidanceNeedleTip[2] << "\n";
  os << indent << "GuidanceNeedleSecond: " << this->GuidanceNeedleSecond[0] << " " << this->GuidanceNeedleSecond[1] << " " << this->GuidanceNeedleSecond[2] << "\n";
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "TrackerTransformNodeID"))
      {
      this->SetTrackerTransformNodeID(attValue);
      this->Scene->AddReferencedNodeID(this->TrackerTransformNodeID, this);
      }
    else if (!strcmp(attName, "TrackingSystemUsed"))
      {
      this->SetTrackingSystemUsed(attValue);
      }
    else if (!strcmp(attName, "GuidanceNeedleTip"))
      {
      std::stringstream ss;
      double val;
      ss << attValue;
      for (int i = 0; i < 3; i++)
        {
        ss >> val;
        this->GuidanceNeedleTip[i] = val;
        }
      }
    else if (!strcmp(attName, "GuidanceNeedleSecond"))
      {
      std::stringstream ss;
      double val;
      ss << attValue;
      for (int i = 0; i < 3; i++)
        {
        ss >> val;
        this->GuidanceNeedleSecond[i] = val;
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::WriteXML(ostream& os, int nIndent)
{
  Superclass::WriteXML(os, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);
  {
    std::stringstream ss;
    if (this->TrackerTransformNodeID)
      {
      ss << this->TrackerTransformNodeID;
      os << indent << " TrackerTransformNodeID=\"" << ss.str() << "\"";
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
  os << indent << " GuidanceNeedleTip=\"" << this->GuidanceNeedleTip[0] << " " << this->GuidanceNeedleTip[1] << " " << this->GuidanceNeedleTip[2] << "\"";
  os << indent << " GuidanceNeedleSecond=\"" << this->GuidanceNeedleSecond[0] << " " << this->GuidanceNeedleSecond[1] << " " << this->GuidanceNeedleSecond[2] << "\"";
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::Copy(vtkMRMLNode* anode)
{
  Superclass::Copy(anode);
  vtkMRMLAbdoNavNode* node = (vtkMRMLAbdoNavNode*)anode;

  this->SetTrackerTransformNodeID(node->GetTrackerTransformNodeID());
  this->SetTrackingSystemUsed(node->GetTrackingSystemUsed());
  this->SetGuidanceNeedleTip(node->GetGuidanceNeedleTip());
  this->SetGuidanceNeedleSecond(node->GetGuidanceNeedleSecond());
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::UpdateReferenceID(const char* oldID, const char* newID)
{
  if (!strcmp(oldID, this->TrackerTransformNodeID))
    {
    this->SetTrackerTransformNodeID(newID);
    }
}
