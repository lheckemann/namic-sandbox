/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/
/// vtkMRMLAbdoNavNode - manages the data of the abdominal navigation module
///
/// This class stores references and data used by the abdominal navigation
/// module.

#ifndef __vtkMRMLAbdoNavNode_h
#define __vtkMRMLAbdoNavNode_h

/* AbdoNav includes */
#include "vtkAbdoNavWin32Header.h"

/* VTK includes */
#include "vtkMRMLNode.h"

class VTK_AbdoNav_EXPORT vtkMRMLAbdoNavNode : public vtkMRMLNode
{
 public:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  static vtkMRMLAbdoNavNode *New();
  vtkTypeRevisionMacro(vtkMRMLAbdoNavNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Create an instance of an AbdoNav node
  virtual vtkMRMLNode* CreateNodeInstance();

  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes(const char** atts);

  // Write this node's information to a MRML file in XML format
  virtual void WriteXML(ostream& os, int indent);

  // Copy parameters (not including ID and Scene) from another node of the same type
  virtual void Copy(vtkMRMLNode *node);

  // Get unique node XML tag name (like Volume, Model, etc.)
  virtual const char* GetNodeTagName() { return "AbdoNavParameters"; }

  // Update the IDs of stored references
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

  //----------------------------------------------------------------
  // Getters and Setters for the references and data stored in this node.

  // fill in
  // vtkGetMacro(SomeValue, int);
  // vtkSetMacro(SomeValue, int);
  // vtkGetStringMacro(SomeVolumeRef);
  // vtkSetStringMacro(SomeVolumeRef);

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkMRMLAbdoNavNode();
  virtual ~vtkMRMLAbdoNavNode();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkMRMLAbdoNavNode(const vtkMRMLAbdoNavNode&);
  void operator=(const vtkMRMLAbdoNavNode&);

  //----------------------------------------------------------------
  // The specific references and data stored in this MRML node.

  // fill in
  // int SomeValue;
  // char* SomeVolumeRef;

};

#endif // __vtkMRMLAbdoNavNode_h
