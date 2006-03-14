/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
// .NAME Scene - a list of actors
// .SECTION Description
// Scene represents and provides methods to manipulate a list of
// MRML objects. The list is core and duplicate
// entries are not prevented.
//
// .SECTION see also
// Node vtkCollection

#ifndef __mrmlScene_h
#define __mrmlScene_h

#include "mrmlObject.h"
#include <list>
#include <string>

namespace mrml
{
class Node;
class Collection;
class Transform;
class SceneInternals;
class MRMLCommon_EXPORT Scene : public Object
{
public:
  typedef Scene Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Self, Superclass);

  // Filename should be file://path/file.xml
  mrmlSetStringMacro(URL);
  mrmlGetStringMacro(URL);

  int Connect();

  int Commit(const char* url = NULL);

  // Create node with a given class
  Node* CreateNodeByClass(const char* className);

  // Register node class with the Scene so that it can create it from
  // a class name
  void RegisterNodeClass(Node* node);

  const char* GetClassNameByTag(const char *tagName);

  // Description:
  // Add a path to the list.
  void AddNode(Node *n);

  // Description:
  // Remove a path from the list.
  void RemoveNode(Node *n);

  // Description:
  // Determine whether a particular node is present. Returns its position
  // in the list.
  int IsNodePresent(Node *n);

  // Description:
  // Get the next path in the list.
  Node *GetNextNode();

  Node *GetNextNodeByClass(const char* className);

  Collection *GetNodesByName(const char* name);

  Collection *GetNodesByID(const char* name);

  Collection *GetNodesByClassByID(const char* className, const char* id);

  Collection *GetNodesByClassByName(const char* className, const char* name);

  Node* GetNthNode(int n);

  Node* GetNthNodeByClass(int n, const char* className );

  int GetNumberOfNodesByClass(const char* className) const;

  //BTX
  std::list<std::string> GetNodeClassesList() const;
  //ETX

  // returns list of names
  const char* GetNodeClasses();

  const char* GetUniqueIDByClass(const char* className);

  void InsertAfterNode( Node *item, Node *newItem);
  void InsertBeforeNode( Node *item, Node *newItem);

  int GetTransformBetweenNodes( Node *node1, Node *node2,
                                Transform *xform );

  int GetTransformBetweenSpaces( const char *space1, const char *space2,
                                 Transform *xform );

protected:
  Scene();
  ~Scene();

  char *URL;

  //BTX
  //std::map< std::string, int> UniqueIDByClass;
  //std::vector< std::string > UniqueIDs;
  //std::vector< vtkMRMLNode* > RegisteredNodeClasses;
  //std::vector< std::string > RegisteredNodeTags;
  //ETX

  Node* InitTraversalByClass(const char *className);

  mrmlSetMacro(ErrorCode,unsigned long);
  mrmlGetMacro(ErrorCode,unsigned long);

  mrmlSetStringMacro(ClassNameList);
  mrmlGetStringMacro(ClassNameList);

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(Object *o); // { this->vtkCollection::AddItem(o); };
  void RemoveItem(Object *o); // { this->vtkCollection::RemoveItem(o); };
  void RemoveItem(int i); // { this->vtkCollection::RemoveItem(i); };
  int  IsItemPresent(Object *o); // { return this->vtkCollection::IsItemPresent(o);};

  unsigned long ErrorCode;

  char* ClassNameList;

private:
  Scene(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  SceneInternals *Internal;
};
} // end namespace mrml
#endif
