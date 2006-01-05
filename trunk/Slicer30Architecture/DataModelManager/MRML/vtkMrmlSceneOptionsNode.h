/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlSceneOptionsNode.h,v $
  Date:      $Date: 2005/12/20 22:44:29 $
  Version:   $Revision: 1.6.8.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlSceneOptionsNode_h
#define __vtkMrmlSceneOptionsNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlSceneOptionsNode : public vtkMrmlNode
{
public:
  static vtkMrmlSceneOptionsNode *New();
  vtkTypeMacro(vtkMrmlSceneOptionsNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // ViewUp (vtk camera option)
  vtkSetStringMacro(ViewUp);
  vtkGetStringMacro(ViewUp);
  
  // Description:
  // Position (vtk camera option)
  vtkSetStringMacro(Position);
  vtkGetStringMacro(Position);
  
  // Description:
  // FocalPoint (vtk camera option)
  vtkSetStringMacro(FocalPoint);
  vtkGetStringMacro(FocalPoint);
  
  // Description:
  // Clipping Range (3D view)
  vtkSetStringMacro(ClippingRange);
  vtkGetStringMacro(ClippingRange);
  
  // Description:
  // View mode (3D, 4x512, 4x256, etc.)
  vtkSetStringMacro(ViewMode);
  vtkGetStringMacro(ViewMode);
  
  // Description:
  // Background color of the 3D view
  vtkSetStringMacro(ViewBgColor);
  vtkGetStringMacro(ViewBgColor);

  // Description:
  // Resolution of the texture
  vtkSetStringMacro(ViewTextureResolution);
  vtkGetStringMacro(ViewTextureResolution);
  
  // Description:
  // Interpolate the slices On or Off
  vtkSetStringMacro(ViewTextureInterpolation);
  vtkGetStringMacro(ViewTextureInterpolation);
  
  // Description:
  // Show 3D axes?
  vtkBooleanMacro(ShowAxes,int);
  vtkSetMacro(ShowAxes,int);
  vtkGetMacro(ShowAxes,int);
  
  // Description:
  // Show 3D cube?
  vtkBooleanMacro(ShowBox,int);
  vtkSetMacro(ShowBox,int);
  vtkGetMacro(ShowBox,int);

  // Description:
  // Show slice window text?
  vtkBooleanMacro(ShowAnnotations,int);
  vtkSetMacro(ShowAnnotations,int);
  vtkGetMacro(ShowAnnotations,int);

  // Description:
  // Show 3D outline around slices?
  vtkBooleanMacro(ShowSliceBounds,int);
  vtkSetMacro(ShowSliceBounds,int);
  vtkGetMacro(ShowSliceBounds,int);
  
  // Description:
  // Show 3D letters?
  vtkBooleanMacro(ShowLetters,int);
  vtkSetMacro(ShowLetters,int);
  vtkGetMacro(ShowLetters,int);

  // Description:
  // Show crosshair?
  vtkBooleanMacro(ShowCross,int);
  vtkSetMacro(ShowCross,int);
  vtkGetMacro(ShowCross,int);
  
  // Description:
  // Show hash marks?
  vtkBooleanMacro(ShowHashes,int);
  vtkSetMacro(ShowHashes,int);
  vtkGetMacro(ShowHashes,int);

  // Description:
  // not used in Slicer, only for SPLViz compatibility
  vtkBooleanMacro(ShowMouse,int);
  vtkSetMacro(ShowMouse,int);
  vtkGetMacro(ShowMouse,int);
  
  // Description:
  // DICOM start directory
  vtkSetStringMacro(DICOMStartDir);
  vtkGetStringMacro(DICOMStartDir);
  
  // Description:
  // File name sort parameter
  vtkSetStringMacro(FileNameSortParam);
  vtkGetStringMacro(FileNameSortParam);
  
  // Description:
  // DICOM data dictionary
  vtkSetStringMacro(DICOMDataDictFile);
  vtkGetStringMacro(DICOMDataDictFile);
  
  // Description:
  // Width of DICOM preview
  vtkSetMacro(DICOMPreviewWidth,int);
  vtkGetMacro(DICOMPreviewWidth,int);
  
  // Description:
  // Height of DICOM preview
  vtkSetMacro(DICOMPreviewHeight,int);
  vtkGetMacro(DICOMPreviewHeight,int);
  
  // Description:
  // Highest value in DICOM preview
  vtkSetMacro(DICOMPreviewHighestValue,int);
  vtkGetMacro(DICOMPreviewHighestValue,int);
 
};

#endif
