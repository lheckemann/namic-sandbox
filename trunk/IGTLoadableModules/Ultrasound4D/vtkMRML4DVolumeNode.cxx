/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLOsteoPlanNode.cxx,v $
  Date:      $Date: 2006/03/17 15:10:10 $
  Version:   $Revision: 1.2 $

  =========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"
#include "vtkMRML4DVolumeNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkObjectFactory.h"
#include "vtkCollection.h"

//------------------------------------------------------------------------------
vtkMRML4DVolumeNode* vtkMRML4DVolumeNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRML4DVolumeNode");
  if(ret)
    {
    return (vtkMRML4DVolumeNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRML4DVolumeNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRML4DVolumeNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRML4DVolumeNode");
  if(ret)
    {
    return (vtkMRML4DVolumeNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRML4DVolumeNode;
}

//----------------------------------------------------------------------------
vtkMRML4DVolumeNode::vtkMRML4DVolumeNode()
{
  this->VolumeCollection = vtkCollection::New();
  this->SerieID = "";
}

//----------------------------------------------------------------------------
vtkMRML4DVolumeNode::~vtkMRML4DVolumeNode()
{

  if(this->VolumeCollection)
    {
/*
    if(this->VolumeCollection->GetNumberOfItems() > 0)
      {
      // Delete all nodes before deleting
      for(int i = 0; i < this->VolumeCollection->GetNumberOfItems(); i++)
        {
        this->VolumeCollection->GetItemAsObject(i)->Delete();
        }
      }
*/
    this->VolumeCollection->RemoveAllItems();
    this->VolumeCollection->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkMRML4DVolumeNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);
  std::stringstream ss;
  ss << this->GetSerieID();
  of << indent << " SerieID=\"" << ss.str() << "\"";

}

//----------------------------------------------------------------------------
void vtkMRML4DVolumeNode::ReadXMLAttributes(const char** atts)
{
  int disableModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while(*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);

    if(!strcmp(attName, "SerieID"))
      {
      std::stringstream sid(attValue);
      this->SetSerieID(sid.str().c_str());
      }
    }

  this->EndModify(disableModify);
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRML4DVolumeNode::Copy(vtkMRMLNode *anode)
{
}

//----------------------------------------------------------------------------
void vtkMRML4DVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
}
