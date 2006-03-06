/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: Node.cxx,v $
Date:      $Date: 2006/02/07 19:19:35 $
Version:   $Revision: 1.4 $

=========================================================================auto=*/
#include "mrmlNode.h"
//#include "vtkObjectFactory.h"

namespace mrml
{
//------------------------------------------------------------------------------
Node::Node()
{
  this->ID = 0;
  
  // By default nodes have no effect on indentation
  this->Indentation = 0;

  // Strings
  this->Description = NULL;

  // By default all MRML nodes have a blank name
  // Must set name to NULL first so that the SetName
  // macro will not free memory.
  this->Name = NULL;
  this->SetName("");

  this->SpaceName = NULL;
  this->SceneRootDir = NULL;
}

//----------------------------------------------------------------------------
Node::~Node()
{
  this->SetDescription(NULL);
  this->SetSpaceName(NULL);
  this->SetName(NULL);
}

//----------------------------------------------------------------------------
void Node::Copy(Node *node)
{
  this->SetDescription(node->GetDescription());
  this->SetSpaceName(node->GetSpaceName());
  std::string buf = node->GetName();
  buf += "1";
  this->SetName(buf.c_str());
}

//----------------------------------------------------------------------------
void Node::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ID:          " << this->ID << "\n";
  os << indent << "Indentation: " << this->Indentation << "\n";
  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void Node::WriteXML(std::ostream& of, int nIndent)
{
  //mrmlErrorMacro("NOT IMPLEMENTED YET");
}

//----------------------------------------------------------------------------
void Node::ReadXMLAttributes(const char** atts)
{
  const char* attName;
  const char* attValue;
  while (*atts != NULL) {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "Name")) {
      this->SetName(attValue);
    }
    else if (!strcmp(attName, "Description")) {
      this->SetDescription(attValue);
    }
    else if (!strcmp(attName, "SpaceName")) {
      this->SetSpaceName(attValue);
    }
  } 
  return;
}

} // namespace mrml
