/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: VolumeArchetypeStorageNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:41 $
Version:   $Revision: 1.1 $

=========================================================================auto=*/

#include "mrmlVolumeArchetypeStorageNode.h"
#include "mrmlVolumeNode.h"
#include <math.h> // for sqrt

//#include "vtkMatrix4x4.h"
//#include "vtkImageData.h"
//#include "vtkITKArchetypeImageSeriesReader.h"
//#include "vtkITKArchetypeImageSeriesScalarReader.h"

// Initialize static member that controls resampling --
// old comment: "This offset will be changed to 0.5 from 0.0 per 2/8/2002 Slicer 
// development meeting, to move ijk coordinates to voxel centers."


namespace mrml
{
//----------------------------------------------------------------------------
VolumeArchetypeStorageNode::VolumeArchetypeStorageNode()
{
  this->FileArcheType = NULL;
}

//----------------------------------------------------------------------------
VolumeArchetypeStorageNode::~VolumeArchetypeStorageNode()
{
  this->SetFileArcheType(NULL);
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::WriteXML(std::ostream& of, Indent indent)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
  (void)of; (void)indent;
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::ReadXMLAttributes(const char** atts)
{
  this->Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "FileArcheType")) {
      this->SetFileArcheType(attValue);
    }
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void VolumeArchetypeStorageNode::Copy(VolumeArchetypeStorageNode *node)
{
  this->Superclass::Copy(node);

  // Strings
  this->SetFileArcheType(node->FileArcheType);
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FileArcheType: " <<
    (this->FileArcheType ? this->FileArcheType : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::ProcessParentNode(Node *parentNode)
{
  this->ReadData(parentNode);
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::ReadData(Node *refNode)
{
//  if (!refNode->IsA("VolumeNode") ) {
//    vtkErrorMacro("Reference node is not a VolumeNode");
//    return;
//  }

  VolumeNode *volNode = dynamic_cast <VolumeNode *> (refNode);

  if (volNode->GetVolume()) {
    //volNode->GetImageData()->Delete();
    volNode->SetVolume(NULL);
  }

  std::string fullName;
  if (this->SceneRootDir != NULL) {
    fullName = std::string(this->SceneRootDir) + std::string(this->GetFileArcheType());
  }
  else {
    fullName = std::string(this->GetFileArcheType());
  }

  if (fullName == std::string("")) {
    mrmlErrorMacro("VolumeNode: File name not specified");
  }

  //TODO: handle both scalars and vectors
  //FIXME
  abort();
  //vtkITKArchetypeImageSeriesScalarReader* reader = vtkITKArchetypeImageSeriesScalarReader::New();

  //reader->SetArchetype(fullName.c_str());
  //reader->Update();
  //volNode->SetImageData(reader->GetOutput());

  // set volume attributes
  //vtkMatrix4x4* mat = reader->GetRasToIjkMatrix();
  //mat->Invert();

  // normalize direction vectors
  for (int row=0; row<3; row++) {
    double len =0;
    int col;
    for (col=0; col<3; col++) {
      /*len += mat->GetElement(row, col) * mat->GetElement(row, col);*/
    }
    len = sqrt(len);
    for (col=0; col<3; col++) {
      /*mat->SetElement(row, col,  mat->GetElement(row, col)/len);*/
    }
  }
  //volNode->SetIjkToRasMatrix(mat);
}

//----------------------------------------------------------------------------
void VolumeArchetypeStorageNode::WriteData(Node *refNode)
{
  mrmlErrorMacro("NOT IMPLEMENTED YET");
}

} // end namespace mrml
