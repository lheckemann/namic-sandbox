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

namespace mrml
{
//------------------------------------------------------------------------------
Node::Node()
{
  this->ID = NULL;

  // By default nodes have no effect on indentation
  this->Indentation = 0;

  // Strings
  this->Description = NULL;

  // By default all MRML nodes have a blank name
  // Must set name to NULL first so that the SetName
  // macro will not free memory.
  this->Name = NULL;
  this->SetName("");

  this->SceneRootDir = NULL;
}

//----------------------------------------------------------------------------
Node::~Node()
{
  this->SetDescription(NULL);
  this->SetName(NULL);
  this->SetID(NULL);
}

//----------------------------------------------------------------------------
void Node::Copy(Node *node)
{
  this->SetDescription(node->GetDescription());
  std::string buf = node->GetName();
  buf += "1";
  this->SetName(buf.c_str());
  this->SetID(node->GetID());
  //TODO create unique id
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
void Node::WriteXML(std::ostream& of, Indent ind)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
}

//----------------------------------------------------------------------------
void Node::ReadXMLAttributes(const char** atts)
{
  const char* attName;
  const char* attValue;
  while (*atts != NULL) {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "ID")) {
      this->SetID(attValue);
    }
    else if (!strcmp(attName, "Name")) {
      this->SetName(attValue);
    }
    else if (!strcmp(attName, "Description")) {
      this->SetDescription(attValue);
    }
  }
  return;
}

} // namespace mrml