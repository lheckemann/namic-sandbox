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
class mrmlCommon_EXPORT Node : public Object
{
public:
  typedef Node Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  // Method for defining the name of the class
  mrmlTypeMacro(Node, Object);

  // Description:
  // Create instance of the default node. Like New only virtual
  // NOTE: Subclasses should implement this method
  virtual Node* CreateNodeInstance() = 0;

  // Description:
  // Set node attributes
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void ReadXMLAttributes(const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void WriteXML(std::ostream& of, int indent);
  
  // Read data for the node
  // NOTE: Subclasses should implement this method
  virtual void ReadData() = 0;
  
  // Write data for the node
  // NOTE: Subclasses should implement this method
  virtual void WriteData() = 0;

  // Description:
  // Copy everything from another node of the same type.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void Copy(Node *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  // NOTE: Subclasses should implement this method
  virtual const char* GetNodeTagName() = 0;
  
  // Description:
  // Set/Get a numerical ID for the calling program to use to keep track
  // of its various node objects.
  mrmlSetMacro(ID, unsigned int);
  mrmlGetMacro(ID, unsigned int);
  
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
  // Name of space in which this node lives
  mrmlSetStringMacro(SpaceName);
  mrmlGetStringMacro(SpaceName);

  // Description:
  // Node's effect on indentation when displaying the
  // contents of a MRML file. (0, +1, -1)
  mrmlGetMacro(Indentation, int);
  
protected:
  Node();
  // critical to have a virtual destructor!
  virtual ~Node();

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, Indent indent) const;

  mrmlSetMacro(Indentation, int);
  
  unsigned int ID;
  char *Description;
  char *SceneRootDir;
  char *Name;
  char *SpaceName;
  int Indentation;

private:
  Node(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace mrml
#endif



