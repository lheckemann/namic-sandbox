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
#include "mrmlXMLParser.h"

#include <mrmlsys/stl/stack>

namespace mrml
{
class ParserInternals
{
public:
  typedef std::stack< Node *> NodeStackType;
  ParserInternals() {
    this->MRMLScene = Scene::New();
    }
  ~ParserInternals() {
    }
  NodeStackType NodeStack;
  Scene::Pointer MRMLScene;
};

Scene *Parser::GetScene()
{
  return this->Internal->MRMLScene.GetPointer();
}

void Parser::SetScene(Scene *s)
{
  this->Internal->MRMLScene = s;
}

//------------------------------------------------------------------------------
Parser::Parser()
{
  this->FileName = 0;
  this->Internal = new ParserInternals;
}
//------------------------------------------------------------------------------
Parser::~Parser()
{
  this->SetFileName(0);
  delete this->Internal;
}

//------------------------------------------------------------------------------
void Parser::StartElement(const char* tagName, const char** atts)
{
  if (!strcmp(tagName, "MRML")) {
    return;
  }
  const char* className = this->Internal->MRMLScene->GetClassNameByTag(tagName);

  if (className == NULL) {
    return;
  }

  Node* node = this->Internal->MRMLScene->CreateNodeByClass( className );

  node->ReadXMLAttributes(atts);

  if (node->GetID() == NULL) {
    node->SetID(this->Internal->MRMLScene->GetUniqueIDByClass(className));
  }

  ParserInternals::NodeStackType nodeStack = this->Internal->NodeStack;
  if (!nodeStack.empty()) {
    Node* parentNode = nodeStack.top();
    parentNode->ProcessChildNode(node);
    node->ProcessParentNode(parentNode);
  }

  nodeStack.push(node);

  this->Internal->MRMLScene->AddNode(node);

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
void Parser::Parse()
{
  this->ParseFile(this->FileName);
}
//-----------------------------------------------------------------------------
void Parser::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}

} // end namespace mrml
