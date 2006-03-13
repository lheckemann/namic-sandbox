/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlTree.cxx,v $
  Date:      $Date: 2006/02/14 20:40:15 $
  Version:   $Revision: 1.23 $

=========================================================================auto=*/
#include "mrmlTree.h"
#include "mrmlNode.h"

//#include <vtkstd/fstream>
#include <list>

namespace mrml
{

class TreeInternals
{
public:
typedef std::list<Node*> Collection;
  // Used to be a vtkCollection
  Collection Nodes;
};

//------------------------------------------------------------------------------
Tree::Tree()
{
  this->Internal = new TreeInternals;
}

//------------------------------------------------------------------------------
Tree::~Tree()
{
  delete this->Internal;
}

//------------------------------------------------------------------------------
void Tree::Write(const char *filename)
{
  /*
  vtkCollectionElement *elem = this->Top;
  vtkMrmlNode *node;
  ofstream file;
  int indent=0, deltaIndent;

  // Open file
  file.open(filename);
  if (file.fail())
  {
    vtkErrorMacro("Write: Could not open file " << filename);
    cerr << "Write: Could not open file " << filename;
#if (VTK_MAJOR_VERSION <= 5)      
    this->SetErrorCode(2);
#else
    this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
#endif
    return;
  }
  
  file << "<?xml version=\"1.0\" standalone='no'?>\n";
  file << "<!DOCTYPE MRML SYSTEM \"mrml20.dtd\">\n";
  file << "<MRML>\n";
   
  // Write each node
  while (elem != NULL)
  {
    node = (vtkMrmlNode*)elem->Item;

    deltaIndent = node->GetIndent();
    if ( deltaIndent < 0 )
    {
      indent -=2;
    }

    node->Write(file, indent);

    if ( deltaIndent > 0 )
    {
      indent += 2;
    }

    elem = elem->Next;
  }

  file << "</MRML>\n";

  // Close file
  file.close();
#if (VTK_MAJOR_VERSION <= 5)      
    this->SetErrorCode(0);
#else
    this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("NoError"));
#endif
*/
}

//------------------------------------------------------------------------------
unsigned long Tree::GetNumberOfNodesByClass(const char *className)
{
  int num=0;
  /*
  vtkCollectionElement *elem=this->Top;

  while (elem != NULL)
  {
    if (!strcmp(elem->Item->GetClassName(), className))
    {
      num++;
    }
    elem = elem->Next;
  }*/
  return num;
}

//------------------------------------------------------------------------------
Node *Tree::GetNextNodeByClass(const char *className)
{
  /*
  vtkCollectionElement *elem=this->Current;

  if (elem != NULL)
  {
    elem = elem->Next;
  }

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className))
  {
    elem = elem->Next;
  }
  
  if (elem != NULL)
  {
    this->Current = elem;
    return (vtkMrmlNode*)(elem->Item);
  }
  else
  {
    return NULL;
  }*/
  return NULL;
}

//------------------------------------------------------------------------------
Node* Tree::InitTraversalByClass(const char *className)
{
/*  vtkCollectionElement *elem=this->Top;

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className))
  {
    elem = elem->Next;
  }
  
  if (elem != NULL)
  {
    this->Current = elem;
    return (vtkMrmlNode*)(elem->Item);
  }
  else
  {
    return NULL;
  }*/
  return NULL;
}

//------------------------------------------------------------------------------
Node* Tree::GetNthNode(unsigned long n)
{
/*  vtkCollectionElement *elem;

  if(n < 0 || n >= this->NumberOfItems)
  {
    return NULL;
  }
  
  elem = this->Top;
  for (int j = 0; j < n; j++, elem = elem->Next) 
  {}

  return (vtkMrmlNode*)(elem->Item);*/
  return NULL;
}

//------------------------------------------------------------------------------
Node* Tree::GetNthNodeByClass(unsigned long n, const char *className)
{
/*  vtkCollectionElement *elem;
  int j=0;

  elem = this->Top;
  while (elem != NULL)
  {
    if (strcmp(elem->Item->GetClassName(), className) == 0)
    {
      if (j == n)
      {
        return (vtkMrmlNode*)(elem->Item);
      }
      j++;
    }
    elem = elem->Next;
  }
*/
  return NULL;
}

//------------------------------------------------------------------------------
void Tree::InsertAfterNode(Node *ref, Node *node) // item, n
{
/*  int i;
  vtkCollectionElement *elem, *newElem;
  
  // Empty list
  if (!this->Top)
  {
    return;
  }

  newElem = new vtkCollectionElement;
  n->Register(this);
  newElem->Item = n;

  elem = this->Top;
  for (i = 0; i < this->NumberOfItems; i++)
  {
    if (elem->Item == item)
    {
      newElem->Next = elem->Next;
      elem->Next = newElem;

      if (this->Bottom == elem)
      {
        this->Bottom = newElem;
      }
      this->NumberOfItems++;
      return;
    }
    else
    {
      elem = elem->Next;
    }
  }*/
}

//------------------------------------------------------------------------------
void Tree::InsertBeforeNode(Node *item, Node *n)
{
  /*
  int i;
  vtkCollectionElement *elem, *newElem, *prev;
  
  // Empty list
  if (!this->Top)
  {
    return;
  }

  newElem = new vtkCollectionElement;
  n->Register(this);
  newElem->Item = n;


  elem = this->Top;

  // if insert before 1st elem
  if (elem->Item == item)
  {
    newElem->Next = this->Top;
    this->Top = newElem;
    this->NumberOfItems++;
    return;
  }

  for (i = 1; i < this->NumberOfItems; i++)
  {
    prev = elem;
    elem = elem->Next;

    if (elem->Item == item)
    {
      newElem->Next = prev->Next;
      prev->Next = newElem;
      this->NumberOfItems++;
      return;
    }
  }*/
  
}

//------------------------------------------------------------------------------
void Tree::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace mrml
