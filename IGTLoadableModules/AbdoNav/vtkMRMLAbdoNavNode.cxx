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

/* STL includes */
#include <sstream>

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMRMLAbdoNavNode, "$Revision: $");
vtkStandardNewMacro(vtkMRMLAbdoNavNode);


//---------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLAbdoNavNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLAbdoNavNode");
  if(ret)
    {
      return (vtkMRMLAbdoNavNode*)ret;
    }
  // If the factory was unable to create the object, then create it here
  return new vtkMRMLAbdoNavNode;
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::vtkMRMLAbdoNavNode()
{
  // fill in
  // this->SomeValue = 1;
  // this->SomeVolumeRef = NULL;
  this->HideFromEditors = true;
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode::~vtkMRMLAbdoNavNode()
{
  // fill in
  // this->SetSomeVolumeRef(NULL);
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os, indent);

  // fill in
  // os << indent << "SomeValue: "     << this->SomeValue << "\n";
  // os << indent << "SomeVolumeRef: " <<
  //       (this->SomeVolumeRef ? this->SomeVolumeRef : "(none)") << "\n";
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
    // fill in
    // if (!strcmp(attName, "SomeValue"))
    //   {
    //   std::stringstream ss;
    //   ss << attValue;
    //   ss >> this->SomeValue;
    //   }
    // else if (!strcmp(attName, "SomeVolumeRef"))
    //   {
    //   this->SetSomeVolumeRef(attValue);
    //   this->Scene->AddReferencedNodeID(this->SomeVolumeRef, this);
    //   }
    }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::WriteXML(ostream& os, int nIndent)
{
  Superclass::WriteXML(os, nIndent);

  // Write all MRML node attributes into output stream

  vtkIndent indent(nIndent);

  // fill in
  // {
  //   std::stringstream ss;
  //   ss << this->SomeValue;
  //   os << indent << " SomeValue=\"" << ss.str() << "\"";
  // }
  // {
  //   std::stringstream ss;
  //   if (this->SomeVolumeRef)
  //     {
  //     ss << this->SomeVolumeRef;
  //     os << indent << " SomeVolumeRef=\"" << ss.str() << "\"";
  //    }
  // }
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::Copy(vtkMRMLNode* anode)
{
  Superclass::Copy(anode);
  vtkMRMLAbdoNavNode* node = (vtkMRMLAbdoNavNode*)anode;

  // fill in
  // this->SetSomeValue(node->SomeValue);
  // this->SetSomeVolumeRef(node->SomeVolumeRef);
}


//---------------------------------------------------------------------------
void vtkMRMLAbdoNavNode::UpdateReferenceID(const char* oldID, const char* newID)
{
  // fill in
  // if (!strcmp(oldID, this->SomeVolumeRef)) // zero value indicates that both strings are equal
  //   {
  //   this->SomeVolumeRef(newID);
  //   }
}
