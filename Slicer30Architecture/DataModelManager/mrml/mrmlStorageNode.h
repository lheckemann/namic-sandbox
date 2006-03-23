/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: StorageNode.h,v $
  Date:      $Date: 2006/03/03 22:26:41 $
  Version:   $Revision: 1.1 $

=========================================================================auto=*/
// .NAME StorageNode - MRML node for representing a volume (image stack).
// .SECTION Description
// Storage nodes describe data sets that can be thought of as stacks of 2D 
// images that form a 3D volume.  Storage nodes describe where the images 
// are stored on disk, how to render the data (window and level), and how 
// to read the files.  This information is extracted from the image 
// headers (if they exist) at the time the MRML file is generated.  
// Consequently, MRML files isolate MRML browsers from understanding how 
// to read the myriad of file formats for medical data. 

#ifndef __mrmlStorageNode_h
#define __mrmlStorageNode_h

#include "mrmlNode.h"

namespace mrml
{
class MRMLCommon_EXPORT StorageNode : public Node
{
public:
  // Description:
  mrmlTypeMacro(StorageNode, Node);

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Read data and set it in the referenced node
  // NOTE: Subclasses should implement this method
  virtual void ReadData(Node *refNode) = 0;

  // Description:
  // Write data from a  referenced node
  // NOTE: Subclasses should implement this method
  virtual void WriteData(Node *refNode) = 0;

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(std::ostream& of, Indent ind);

  // Description:
  // Copy the node's attributes to this object
  void Copy(StorageNode *node);

  // Description:
  // Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() = 0;


protected:
  StorageNode();
  ~StorageNode();

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  StorageNode(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace mrml
#endif

