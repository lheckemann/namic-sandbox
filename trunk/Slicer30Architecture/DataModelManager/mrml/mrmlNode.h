/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   MRML
  Module:    $RCSfile: mrmlNode.h,v $
  Date:      $Date: 2006/02/11 17:20:11 $
  Version:   $Revision: 1.9 $

=========================================================================auto=*/
// .NAME mrmlNode - Abstract Superclass for all specific types of MRML nodes.
// .SECTION Description
// This node encapsulates the functionality common to all types of MRML nodes.
// This includes member variables for ID, Description, and Options,
// as well as member functions to Copy() and Write().

#ifndef __mrmlNode_h
#define __mrmlNode_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

namespace mrml
{
class Scene;
class MRMLCommon_EXPORT Node : public Object
{
public:
  typedef Node Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Self, Superclass);

  // Description:
  // Set node attributes
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void ReadXMLAttributes(const char** atts);

  // Description:
  // Set dependencies between this node and the parent node
  // when parsing XML file
  virtual void ProcessParentNode(Node *parentNode) {};

  // Description:
  // Set dependencies between this node and a child node
  // when parsing XML file
  virtual void ProcessChildNode(Node *childNode) {};

  // Description:
  // Updates other nodes in the scene depending on this node
  // This method is called automatically by XML parser after all nodes are created
  virtual void UpdateScene(Scene *scene) {};

  // Description:
  // Write this node's information to a MRML file in XML format.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void WriteXML(std::ostream& of, Indent ind);

  // Description:
  // Copy everything from another node of the same type.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  void Copy(Node *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  // NOTE: Subclasses should implement this method
  virtual const char* GetNodeTagName() = 0;

  // Description:
  // Text description of this node, to be set by the user
  mrmlSetStringMacro(Description);
  mrmlGetStringMacro(Description);

  // Description:
  // Root directory of MRML scene
  mrmlSetStringMacro(SceneRootDir);
  mrmlGetStringMacro(SceneRootDir);

  // Description:
  // Name of this node, to be set by the user
  mrmlSetStringMacro(Name);
  mrmlGetStringMacro(Name);


  // Description:
  // Node's effect on indentation when displaying the
  // contents of a MRML file. (0, +1, -1)
  mrmlGetMacro(Indentation, int);

  // Description:
  // ID use by other nodes to reference this node in XML
  mrmlSetStringMacro(ID);
  mrmlGetStringMacro(ID);

protected:
  Node();
  ~Node();

  /** Print the object information in a stream. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  mrmlSetMacro(Indentation, int);

  char *Description;
  char *SceneRootDir;
  char *Name;
  char *ID;
  int Indentation;

private:
  Node(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace mrml
#endif



