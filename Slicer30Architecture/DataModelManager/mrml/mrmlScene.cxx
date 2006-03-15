/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: Scene.cxx,v $
Date:      $Date: 2006/03/03 22:26:40 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/
#include "mrmlScene.h"
#include "mrmlParser.h"
#include "mrmlNode.h"

#include <mrmlsys/SystemTools.hxx>
#include <mrmlsys/stl/string>

#include "vtkCollection.h"

namespace mrml
{
class SceneInternals
{
public:
  SceneInternals() {};
  ~SceneInternals() {};
  // Expose a vtkCollection-like interface
  typedef Object* CollectionElement;
  typedef std::list<CollectionElement> ObjectsList;
  void AddItem(CollectionElement elem) {
    this->Collection.push_back(elem);
    }
  void RemoveAllItems() {
    this->Collection.clear();
  }
  int GetNumberOfItems() {
    return this->Collection.size();
  }
  CollectionElement GetItemAsObject (int i) {
    if( i >= 0 && i <= (int)this->Collection.size() ) {
      ObjectsList::const_iterator it = this->Collection.begin();
      for(int j=0; j<i; ++j, ++it);
      return *it;
      }
    return NULL;
  }
  ObjectsList::const_iterator GetTop() {
    return this->Collection.begin();
  }
  ObjectsList::const_iterator GetBottom() {
    return this->Collection.end();
  }
  const ObjectsList &GetObjectsList() const {
    return this->Collection;
    }
  std::map< std::string, int> UniqueIDByClass;
  std::vector< std::string > UniqueIDs;
  std::vector< Node* > RegisteredNodeClasses;
  std::vector< std::string > RegisteredNodeTags;
private:
  //vtkCollection *Collection;
  ObjectsList Collection;
};

//------------------------------------------------------------------------------
Scene::Scene()
{
  this->Internal = new SceneInternals;
  this->URL = NULL;
  this->ClassNameList = NULL;
  this->Internal->RegisteredNodeClasses.clear();
  this->Internal->UniqueIDByClass.clear();
}
//------------------------------------------------------------------------------
Scene::~Scene()
{
  delete this->Internal;
}



//------------------------------------------------------------------------------
void Scene::RegisterNodeClass(Node* node)
{
  this->Internal->RegisteredNodeClasses.push_back(node);
  this->Internal->RegisteredNodeTags.push_back(std::string(node->GetNodeTagName()));
}

//------------------------------------------------------------------------------
const char* Scene::GetClassNameByTag(const char *tagName)
{
  for (unsigned int i=0; i<this->Internal->RegisteredNodeTags.size(); i++) {
    if (!strcmp(this->Internal->RegisteredNodeTags[i].c_str(), tagName)) {
      return (this->Internal->RegisteredNodeClasses[i])->GetNameOfClass();
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
void Scene::AddNode(Node *n) 
{
  //TODO convert URL to Root directory
  mrmlsys_stl::string root = mrmlsys::SystemTools::GetParentDirectory(this->GetURL());   
  n->SetSceneRootDir(root.c_str());
  this->AddItem(n);
}

//------------------------------------------------------------------------------
int Scene::Connect()
{
  if (this->URL == NULL) {
    mrmlErrorMacro("Need URL specified");
    return 0;
  }
  this->Internal->RemoveAllItems();
  Parser::Pointer parser = Parser::New();
  parser->SetMRMLScene(this);
  parser->SetFileName(URL);
  parser->Parse();
  /*parser->Delete();*/

  // create node references
  int nnodes = this->Internal->GetNumberOfItems();
  Node *node = NULL;
  // FIXME this is not the way to iterate on list elements
  for (int n=0; n<nnodes; n++) {
    node = (Node *)this->Internal->GetItemAsObject(n);
    node->UpdateScene(this);
  }
  return 1;
}

//------------------------------------------------------------------------------
int Scene::Commit(const char* url)
{
  if (url == NULL) {
    url = URL;
  }

  Node *node;
  ofstream file;
  int indent=0, deltaIndent;

  // Open file
  file.open(url);
  if (file.fail()) {
    mrmlErrorMacro("Write: Could not open file " << url);
    std::cerr << "Write: Could not open file " << url;
    //FIXME
    //this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
    abort();
    return 1;
  }

  file << "<?xml version=\"1.0\" standalone='no'?>\n";
  file << "<!DOCTYPE MRML SYSTEM \"mrml20.dtd\">\n";
  file << "<MRML>\n";

  // Write each node
  SceneInternals::ObjectsList::const_iterator it = this->Internal->GetTop();
  SceneInternals::ObjectsList::const_iterator end = this->Internal->GetBottom();
  while (it != end) {
    node = static_cast<Node*>(*it); //(Node*)elem->Item;
    assert( node );

    deltaIndent = node->GetIndentation();
    if ( deltaIndent < 0 ) {
      indent -=2;
    }

    node->WriteXML(file, indent);
    
    if ( deltaIndent > 0 ) {
      indent += 2;
    }
    
    ++it; //elem = elem->Next;
  }

  file << "</MRML>\n";

  // Close file
  file.close();
  //this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("NoError")); //FIXME
  return 1;
}

//------------------------------------------------------------------------------
int Scene::GetNumberOfNodesByClass(const char *className) const
{
  int num=0;

  SceneInternals::ObjectsList::const_iterator it = this->Internal->GetTop();
  SceneInternals::ObjectsList::const_iterator end = this->Internal->GetBottom();
  while (it != end) {
    SceneInternals::CollectionElement elem = *it;
    if (!strcmp(elem->GetNameOfClass(), className)) {
      num++;
    }
    ++it; //elem = elem->Next;
  }
  return num;
}

//------------------------------------------------------------------------------
std::list< std::string > Scene::GetNodeClassesList() const
{
  std::list< std::string > classes;
  SceneInternals::ObjectsList::const_iterator it = this->Internal->GetTop();
  SceneInternals::ObjectsList::const_iterator end = this->Internal->GetBottom();
  while (it != end) {
    SceneInternals::CollectionElement elem = *it;
    classes.push_back(elem->GetNameOfClass());
    ++it; //elem = elem->Next;
  }
  // FIXME
  classes.unique(); // removes consecutive duplicate elements, is it really indended ?
  return classes;
}

//------------------------------------------------------------------------------
const char* Scene::GetNodeClasses()
{
  std::list< std::string > classes = this->GetNodeClassesList();
  std::string classList;

  std::list< std::string >::const_iterator iter;
  // Iterate through list and output each element.
  for (iter = classes.begin(); iter != classes.end(); iter++) {
    if (!(iter == classes.begin())) {
      classList += std::string(" ");
    }
    classList.append(*iter);
  }
  SetClassNameList(classList.c_str());
  return this->ClassNameList;
}



//------------------------------------------------------------------------------
Node *Scene::GetNextNodeByClass(const char *className)
{
#if 0
  CollectionElement *elem = this->Current;

  if (elem != NULL) {
    elem = elem->Next;
  }

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className)) {
    elem = elem->Next;
  }

  if (elem != NULL) {
    this->Current = elem;
    return (Node*)(elem->Item);
  }
  else {
    return NULL;
  }
#endif
  return NULL;
}

//------------------------------------------------------------------------------
Node* Scene::InitTraversalByClass(const char *className)
{
#if 0
  CollectionElement *elem = this->Top;

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className)) {
    elem = elem->Next;
  }

  if (elem != NULL) {
    this->Current = elem;
    return (Node*)(elem->Item);
  }
  else {
    return NULL;
  }
#endif
  return NULL;
}

//------------------------------------------------------------------------------
Node* Scene::GetNthNode(int n)
{
  SceneInternals::CollectionElement elem = this->Internal->GetItemAsObject(n);
  return static_cast<Node*>(elem);
}

//------------------------------------------------------------------------------
Node* Scene::GetNthNodeByClass(int n, const char *className)
{
#if 0
  int j=0;

  CollectionElement *elem;
  elem = this->Top;
  while (elem != NULL) {
    if (strcmp(elem->Item->GetClassName(), className) == 0) {
      if (j == n) {
        return (Node*)(elem->Item);
      }
      j++;
    }
    elem = elem->Next;
  }

#endif
  return NULL;
}

//------------------------------------------------------------------------------
Collection* Scene::GetNodesByName(const char* name)
{
#if 0
  Collection* nodes = Collection::New();

  CollectionElement *elem = this->Top;
  Node* node;
  while (elem != NULL) {
    node = (Node*)elem->Item;
    if (!strcmp(node->GetName(), name)) {
      nodes->AddItem(node);
    }

    elem = elem->Next;
  }
  return nodes;
#endif
  return NULL;
}


//------------------------------------------------------------------------------
Collection* Scene::GetNodesByID(const char* id)
{
#if 0
  Collection* nodes = Collection::New();

  CollectionElement *elem = this->Top;
  Node* node;
  while (elem != NULL) {
    node = (Node*)elem->Item;
    if (node->GetID() && !strcmp(node->GetID(), id)) {
      nodes->AddItem(node);
    }

    elem = elem->Next;
  }
  return nodes;
#endif
  return 0;
}

//------------------------------------------------------------------------------
Collection* Scene::GetNodesByClassByID(const char* className, const char* id)
{
#if 0
  Collection* nodes = Collection::New();

  CollectionElement *elem = this->Top;
  Node* node;

  while (elem != NULL) {
    node = (Node*)elem->Item;
    if (node->GetID() && !strcmp(node->GetID(), id) 
      && strcmp(elem->Item->GetClassName(), className) == 0) {
      nodes->AddItem(node);
    }

    elem = elem->Next;
  }
  return nodes;
#endif
  return NULL;
}

//------------------------------------------------------------------------------
Collection* Scene::GetNodesByClassByName(const char* className, const char* name)
{
#if 0
  Collection* nodes = Collection::New();

  CollectionElement *elem=this->Top;
  Node* node;

  while (elem != NULL) {
    node = (Node*)elem->Item;
    if (!strcmp(node->GetName(), name) && strcmp(elem->Item->GetClassName(), className) == 0) {
      nodes->AddItem(node);
    }

    elem = elem->Next;
  }
  return nodes;
#endif
  return NULL;
}

//------------------------------------------------------------------------------
int Scene::GetTransformBetweenNodes(Node *node1,
                                    Node *node2,
                                    Transform *xform)
{
  mrmlErrorMacro("NOT IMPLEMENTEED YET");
  return 1;
}

//------------------------------------------------------------------------------
int Scene::GetTransformBetweenSpaces( const char *space1,
                                      const char *space2,
                                      Transform *xform )
{
  mrmlErrorMacro("NOT IMPLEMENTEED YET");
  return 1;
}


//------------------------------------------------------------------------------
void Scene::InsertAfterNode(Node *item, Node *n)
{
#if 0
  int i;
  CollectionElement *elem, *newElem;

  // Empty list
  if (!this->Top) {
    return;
  }

  newElem = new CollectionElement;
  n->Register(this);
  newElem->Item = n;

  elem = this->Top;
  for (i = 0; i < this->NumberOfItems; i++) {
    if (elem->Item == item) {
      newElem->Next = elem->Next;
      elem->Next = newElem;
      
      if (this->Bottom == elem) {
        this->Bottom = newElem;
      }
      this->NumberOfItems++;
      return;
    }
    else {
      elem = elem->Next;
    }
  }
#endif
}

//------------------------------------------------------------------------------
void Scene::InsertBeforeNode(Node *item, Node *n)
{
#if 0
  int i;
  CollectionElement *elem, *newElem, *prev;

  // Empty list
  if (!this->Top) {
    return;
  }

  newElem = new CollectionElement;
  n->Register(this);
  newElem->Item = n;


  elem = this->Top;

  // if insert before 1st elem
  if (elem->Item == item) {
    newElem->Next = this->Top;
    this->Top = newElem;
    this->NumberOfItems++;
    return;
  }

  for (i = 1; i < this->NumberOfItems; i++) {
    prev = elem;
    elem = elem->Next;

    if (elem->Item == item) {
      newElem->Next = prev->Next;
      prev->Next = newElem;
      this->NumberOfItems++;
      return;
    }
  }
#endif
}

//------------------------------------------------------------------------------
void Scene::PrintSelf(ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
  std::list<std::string> classes = this->GetNodeClassesList();

  std::list< std::string >::const_iterator iter;
  // Iterate through list and output each element.
  for (iter = classes.begin(); iter != classes.end(); ++iter) {
    const std::string &className = *iter;
    os << indent << "Number Of Nodes for class " << className.c_str() << " : " 
      << this->GetNumberOfNodesByClass(className.c_str()) << "\n";
  }
}

//------------------------------------------------------------------------------
const char* Scene::GetUniqueIDByClass(const char* className)
{
  std::string sname = className;
  if (this->Internal->UniqueIDByClass.find(sname)
    == this->Internal->UniqueIDByClass.end() ) {
    this->Internal->UniqueIDByClass[className] = 1;
  }
  int id = this->Internal->UniqueIDByClass[sname];

  std::string name;

  while (true) {
    std::stringstream ss;
    ss << className;
    ss << id;
    name = ss.str();
    bool nameExists = false;
    for (unsigned int i=0; i< this->Internal->UniqueIDs.size(); i++) {
      if (this->Internal->UniqueIDs[i] == name) {
        nameExists = true;
        break;
      }
    }
    if (nameExists) {
      id++;
      continue;
    }
    else {
      break;
    }
  }
  this->Internal->UniqueIDByClass[className] = id + 1;
  this->Internal->UniqueIDs.push_back(name);
  return this->Internal->UniqueIDs[
    this->Internal->UniqueIDs.size()-1].c_str();
}

//------------------------------------------------------------------------------
Node* Scene::CreateNodeByClass(const char* className)
{
  return NULL;
}

//------------------------------------------------------------------------------
void Scene::AddItem(Object *o)
{
  this->Internal->AddItem(o);
}
} // end namespace mrml
