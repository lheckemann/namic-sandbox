/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: Parser.cxx,v $
Date:      $Date: 2006/03/03 22:26:40 $
Version:   $Revision: 1.7 $

=========================================================================auto=*/
#include "mrmlParser.h"
#include "mrmlNode.h"
#include "mrmlScene.h"
#include "vtkXMLParser.h"

#include <mrmlsys/stl/stack>

namespace mrml
{
class ParserInternals
{
public:
  typedef std::stack< Node *> NodeStackType;
  ParserInternals() {
    this->Parser = vtkXMLParser::New();
    }
  ~ParserInternals() {
    this->Parser->Delete();
  }
  NodeStackType NodeStack;
  void SetFileName(const char* filename) {
    this->Parser->SetFileName(filename);
    }
  void Parse() {
    this->Parser->Parse();
  }
private:
  vtkXMLParser *Parser;
};

//------------------------------------------------------------------------------
Parser::Parser()
{
  this->Internal = new ParserInternals;
}
//------------------------------------------------------------------------------
Parser::~Parser()
{
  delete this->Internal;
}

//------------------------------------------------------------------------------
void Parser::StartElement(const char* tagName, const char** atts)
{
  if (!strcmp(tagName, "MRML")) {
    return;
  }
  const char* className = this->MRMLScene->GetClassNameByTag(tagName);

  if (className == NULL) {
    return;
  }

  Node* node = this->MRMLScene->CreateNodeByClass( className );

  node->ReadXMLAttributes(atts);

  if (node->GetID() == NULL) {
    node->SetID(this->MRMLScene->GetUniqueIDByClass(className));
  }

  ParserInternals::NodeStackType nodeStack = this->Internal->NodeStack;
  if (!nodeStack.empty()) {
    Node* parentNode = nodeStack.top();
    parentNode->ProcessChildNode(node);
    node->ProcessParentNode(parentNode);
  }

  nodeStack.push(node);

  this->MRMLScene->AddNode(node);

  node->Delete();
}

//-----------------------------------------------------------------------------

void Parser::EndElement (const char *name)
{
  if (!strcmp(name, "MRML")) {
    return;
  }
  this->Internal->NodeStack.pop();
}
//-----------------------------------------------------------------------------
void Parser::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
void Parser::SetFileName(const char *filename)
{
  this->Internal->SetFileName(filename);
}
//-----------------------------------------------------------------------------
void Parser::Parse()
{
  this->Internal->Parse();
}

} // end namespace mrml
