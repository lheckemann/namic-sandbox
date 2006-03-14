/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: StorageNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.1 $

=========================================================================auto=*/

#include "mrmlStorageNode.h"

// Initialize static member that controls resampling -- 
// old comment: "This offset will be changed to 0.5 from 0.0 per 2/8/2002 Slicer 
// development meeting, to move ijk coordinates to voxel centers."


namespace mrml
{
//----------------------------------------------------------------------------
StorageNode::StorageNode()
{
}

//----------------------------------------------------------------------------
StorageNode::~StorageNode()
{
}

//----------------------------------------------------------------------------
void StorageNode::WriteXML(std::ostream& of, Indent indent)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
  (void)of; (void)indent;
}

//----------------------------------------------------------------------------
void StorageNode::ReadXMLAttributes(const char** atts)
{
  this->Superclass::ReadXMLAttributes(atts);
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void StorageNode::Copy(StorageNode *node)
{
  this->Superclass::Copy(node);
}

//----------------------------------------------------------------------------
void StorageNode::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}
} // end namespace mrml

