/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: ModelStorageNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.1 $

=========================================================================auto=*/

#include "mrmlModelStorageNode.h"
#include "mrmlModelNode.h"

#include "vtkMatrix4x4.h"

#include "vtkBYUReader.h" 
#include "vtkPolyDataReader.h"
#include "vtkSTLReader.h"
//TODO: read in a free surfer file
//#include "vtkFSSurfaceReader.h"



// Initialize static member that controls resampling -- 
// old comment: "This offset will be changed to 0.5 from 0.0 per 2/8/2002 Slicer 
// development meeting, to move ijk coordinates to voxel centers."


namespace mrml
{
//----------------------------------------------------------------------------
ModelStorageNode::ModelStorageNode()
{
  this->FileName = NULL;
}

//----------------------------------------------------------------------------
ModelStorageNode::~ModelStorageNode()
{
  this->SetFileName(NULL);
}

//----------------------------------------------------------------------------
void ModelStorageNode::WriteXML(std::ostream& of, Indent indent)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
  (void)of; (void)indent;
}

//----------------------------------------------------------------------------
void ModelStorageNode::ReadXMLAttributes(const char** atts)
{
  this->Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "FileName")) {
      this->SetFileName(attValue);
    }
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void ModelStorageNode::Copy(ModelStorageNode *node)
{
  this->Superclass::Copy(node);

  // Strings
  this->SetFileName(node->FileName);
}

//----------------------------------------------------------------------------
void ModelStorageNode::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void ModelStorageNode::ProcessParentNode(Node *parentNode)
{
  this->ReadData(parentNode);
}

//----------------------------------------------------------------------------
void ModelStorageNode::ReadData(Node *refNode)
{
  //FIXME
  //if (!refNode->IsA("ModelNode") ) {
  //  vtkErrorMacro("Reference node is not a ModelNode");
  //  return;
  //}

  ModelNode *modelNode = dynamic_cast <ModelNode *> (refNode);

  if (modelNode->GetPolyData()) {
    //modelNode->GetPolyData()->Delete();
    modelNode->SetPolyData (NULL);
  }

  std::string fullName;
  if (this->SceneRootDir != NULL) {
    fullName = std::string(this->SceneRootDir) + std::string(this->GetFileName());
  }
  else {
    fullName = std::string(this->GetFileName());
  }

  if (fullName == std::string("")) {
    mrmlErrorMacro("ModelNode: File name not specified");
  }

  // compute file prefix
  std::string name(fullName);
  std::string::size_type loc = name.find(".");
  if( loc == std::string::npos ) {
    mrmlErrorMacro("ModelNode: no file extention specified");
  }
  std::string extention = name.substr(loc);
  
  if ( extention == std::string(".g")) {
    vtkBYUReader *reader = vtkBYUReader::New();
    reader->SetGeometryFileName(fullName.c_str());
    reader->Update();
    //modelNode->SetPolyData(reader->GetOutput());
    abort();
  }
  else if (extention == std::string(".vtk")) {
    vtkPolyDataReader *reader = vtkPolyDataReader::New();
    reader->SetFileName(fullName.c_str());
    reader->Update();
    //modelNode->SetPolyData(reader->GetOutput());
    abort();
  }  
  else if ( extention == std::string(".orig") ||
            extention == std::string(".inflated") || 
            extention == std::string(".pial") ) {
    //TODO: read in a free surfer file
    //vtkFSSurfaceReader *reader = vtkFSSurfaceReader::New();
    //reader->SetFileName(fullName.c_str());
    //reader->Update();
    //modelNode->SetPolyData(reader->GetOutput());
  }  
  else if (extention == std::string(".stl")) {
    vtkSTLReader *reader = vtkSTLReader::New();
    reader->SetFileName(fullName.c_str());
    //modelNode->SetPolyData(reader->GetOutput());
    abort();
    reader->Update();
  }
}

//----------------------------------------------------------------------------
void ModelStorageNode::WriteData(Node *refNode)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
}

} // end namespace mrml
