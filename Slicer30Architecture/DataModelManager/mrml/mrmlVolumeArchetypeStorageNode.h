/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: VolumeArchetypeStorageNode.h,v $
  Date:      $Date: 2006/03/03 22:26:41 $
  Version:   $Revision: 1.1 $

=========================================================================auto=*/
// .NAME VolumeArchetypeStorageNode - MRML node for representing a volume (image stack).
// .SECTION Description
// Storage nodes describe data sets that can be thought of as stacks of 2D
// images that form a 3D volume.  Storage nodes describe where the images
// are stored on disk, how to render the data (window and level), and how
// to read the files.  This information is extracted from the image
// headers (if they exist) at the time the MRML file is generated.
// Consequently, MRML files isolate MRML browsers from understanding how
// to read the myriad of file formats for medical data.

#ifndef __mrmlVolumeArchetypeStorageNode_h
#define __mrmlVolumeArchetypeStorageNode_h

#include "mrmlStorageNode.h"

namespace mrml
{
class MRMLCommon_EXPORT VolumeArchetypeStorageNode : public StorageNode
{
public:
  // Description:
  mrmlTypeMacro(VolumeArchetypeStorageNode, StorageNode);
  mrmlNewMacro(Self);

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

   // Description:
  // Read data and set it in the referenced node
  // NOTE: Subclasses should implement this method
  virtual void ReadData(Node *refNode);

  // Description:
  // Write data from a  referenced node
  // NOTE: Subclasses should implement this method
  virtual void WriteData(Node *refNode);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(std::ostream& of, Indent indent);

 // Description:
  // Set dependencies between this node and the parent node
  // when parsing XML file
  virtual void ProcessParentNode(Node *parentNode);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(VolumeArchetypeStorageNode *node);

  // Description:
  // Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() { return "VolumeArchetypeStorage"; };

  // Description:
  // A file name or one name in a series
  mrmlSetStringMacro(FileArcheType);
  mrmlGetStringMacro(FileArcheType);

protected:
  VolumeArchetypeStorageNode();
  ~VolumeArchetypeStorageNode();

  char *FileArcheType;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:
  VolumeArchetypeStorageNode(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace mrml
#endif

