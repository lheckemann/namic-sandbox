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

/* MRML includes */
#include "vtkMRMLNode.h"

class VTK_AbdoNav_EXPORT vtkMRMLAbdoNavNode : public vtkMRMLNode
{
 public:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  static vtkMRMLAbdoNavNode* New();
  vtkTypeRevisionMacro(vtkMRMLAbdoNavNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------
  // Functions to be implemented by subclasses of vtkMRMLNode.
  //----------------------------------------------------------------
  /// Create an instance of an AbdoNav node.
  virtual vtkMRMLNode* CreateNodeInstance();
  /// Set node attributes from name/value pairs.
  virtual void ReadXMLAttributes(const char** atts);
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& os, int indent);
  /// Copy parameters (not including ID and Scene) from another node of the same type.
  virtual void Copy(vtkMRMLNode* node);
  /// Get unique node XML tag name (like Volume, Model, etc.).
  virtual const char* GetNodeTagName() { return "AbdoNavParameters"; }
  /// Update the IDs of stored references.
  virtual void UpdateReferenceID(const char* oldID, const char* newID);

  //----------------------------------------------------------------
  // Getters and Setters for the references and data stored in this node.
  //----------------------------------------------------------------
  /// Get the identifier of the original tracker transform.
  vtkGetStringMacro(OriginalTrackerTransformID);
  /// Set the identifier of the original tracker transform.
  vtkSetStringMacro(OriginalTrackerTransformID);
  /// Get the identifier of the static registration transform.
  vtkGetStringMacro(RegistrationTransformID);
  /// Set the identifier of the static registration transform.
  vtkSetStringMacro(RegistrationTransformID);
  /// Get the identifier of the fiducial list.
  vtkGetStringMacro(FiducialListID);
  /// Set the identifier of the fiducial list.
  vtkSetStringMacro(FiducialListID);
  /// Get the identifier of the tracking system being used.
  vtkGetStringMacro(TrackingSystemUsed);
  /// Set the identifier of the tracking system being used.
  vtkSetStringMacro(TrackingSystemUsed);

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkMRMLAbdoNavNode();
  virtual ~vtkMRMLAbdoNavNode();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkMRMLAbdoNavNode(const vtkMRMLAbdoNavNode&); // not implemented
  void operator=(const vtkMRMLAbdoNavNode&);     // not implemented

  //----------------------------------------------------------------
  // The specific references and data stored in this MRML node.
  //----------------------------------------------------------------
  /// Identifier of the original tracker transform.
  char* OriginalTrackerTransformID;
  /// Identifier of the static registration transform.
  char* RegistrationTransformID;
  /// Identifier of the fiducial list used to store the RAS coordinates of:
  ///  - the guidance needle tip
  ///  - a second point on the guidance needle
  ///  - the marker center
  char* FiducialListID;
  /// Identifier of the tracking system being used.
  char* TrackingSystemUsed;

};

#endif // __vtkMRMLAbdoNavNode_h
