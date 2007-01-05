/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: mrmlMRMLVolumeNode.h,v $
  Date:      $Date: 2006/03/03 22:26:41 $
  Version:   $Revision: 1.10 $

=========================================================================auto=*/
// .NAME mrmlMRMLVolumeNode - MRML node for representing a volume (image stack).
// .SECTION Description
// Volume nodes describe data sets that can be thought of as stacks of 2D 
// images that form a 3D volume.  Volume nodes describe where the images 
// are stored on disk, how to render the data (window and level), and how 
// to read the files.  This information is extracted from the image 
// headers (if they exist) at the time the MRML file is generated.  
// Consequently, MRML files isolate MRML browsers from understanding how 
// to read the myriad of file formats for medical data. 

#ifndef __mrmlVolumeNode_h
#define __mrmlVolumeNode_h

#include "mrmlNode.h"

namespace mrml
{
class Scene;
class StorageNode;
class Volume;
class VolumeNodeInternals;
class MRMLCommon_EXPORT VolumeNode : public Node
{
public:
  // Description:
  mrmlTypeMacro(VolumeNode, Node);
  mrmlNewMacro(Self);

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(std::ostream& of, Indent ind);

  // Description:
  // Copy the node's attributes to this object
  void Copy(VolumeNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() { return "Volume"; }

  // Description:
  // Finds the storage node and read the data
  virtual void UpdateScene(Scene *scene);

  // Description:
  // Two numbers: the number of columns and rows of pixels in each image
  mrmlGetVector3Macro(FileDimensions, int);
  mrmlSetVector3Macro(FileDimensions, int);

  // Description:
  // Three numbers for the dimensions of each voxel, in millimeters
  mrmlGetVector3Macro(FileSpacing, double);
  mrmlSetVector3Macro(FileSpacing, double);

  // Description:
  // The type of data in the file. One of: Char, UnsignedChar, Short,
  // UnsignedShort, Int, UnsignedInt, Long, UnsignedLong, Float, Double
  mrmlSetMacro(FileScalarType, int);
  mrmlGetMacro(FileScalarType, int);

  //void SetFileScalarTypeToUnsignedChar() 
  //  {this->SetFileScalarType(VTK_UNSIGNED_CHAR);};
  //void SetFileScalarTypeToChar() 
  //  {this->SetFileScalarType(VTK_CHAR);};
  //void SetFileScalarTypeToShort() {
  //  this->SetFileScalarType(VTK_SHORT);};
  //void SetFileScalarTypeToUnsignedShort() 
  //  {this->SetFileScalarType(VTK_UNSIGNED_SHORT);};
  //void SetFileScalarTypeToInt() {
  //  this->SetFileScalarType(VTK_INT);};
  //void SetFileScalarTypeToUnsignedInt() {
  //  this->SetFileScalarType(VTK_UNSIGNED_INT);};
  //void SetFileScalarTypeToLong() {
  //  this->SetFileScalarType(VTK_LONG);};
  //void SetFileScalarTypeToUnsignedLong() {
  //  this->SetFileScalarType(VTK_UNSIGNED_LONG);};
  //void SetFileScalarTypeToFloat() {
  //  this->SetFileScalarType(VTK_FLOAT);};
  //void SetFileScalarTypeToDouble() {
  //  this->SetFileScalarType(VTK_DOUBLE);};

  //const char* GetFileScalarTypeAsString();

  // Description:
  // The number of scalar components for each voxel.
  // Gray-level data has 1. Color data has 3
  mrmlGetMacro(FileNumberOfScalarComponents, int);
  mrmlSetMacro(FileNumberOfScalarComponents, int);

  // Description:
  // Describes the order of bytes for each voxel.  Little endian
  // positions the least-significant byte on the rightmost end,
  // and is true of data generated on a PC or SGI.
  mrmlGetMacro(FileLittleEndian, int);
  mrmlSetMacro(FileLittleEndian, int);
  mrmlBooleanMacro(FileLittleEndian, int);

  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------

  // Description:
  // Numerical ID of the color lookup table to use for rendering the volume
  mrmlSetStringMacro(LUTName);
  mrmlGetStringMacro(LUTName);

  // Description:
  // Indicates if this volume is a label map, which is the output of
  // segmentation that labels each voxel according to its tissue type.
  // The alternative is a gray-level or color image.
  mrmlGetMacro(LabelMap, int);
  mrmlSetMacro(LabelMap, int);
  mrmlBooleanMacro(LabelMap, int);

  // Description:
  // Specifies whether windowing and leveling are to be performed automatically
  mrmlBooleanMacro(AutoWindowLevel, int);
  mrmlGetMacro(AutoWindowLevel, int);
  mrmlSetMacro(AutoWindowLevel, int);

  // Description:
  // The window value to use when autoWindowLevel is 'no'
  mrmlGetMacro(Window, double);
  mrmlSetMacro(Window, double);

  // Description:
  // The level value to use when autoWindowLevel is 'no'
  mrmlGetMacro(Level, double);
  mrmlSetMacro(Level, double);

  // Description:
  // Specifies whether to apply the threshold
  mrmlBooleanMacro(ApplyThreshold, int);
  mrmlGetMacro(ApplyThreshold, int);
  mrmlSetMacro(ApplyThreshold, int);

  // Description:
  // Specifies whether the threshold should be set automatically
  mrmlBooleanMacro(AutoThreshold, int);
  mrmlGetMacro(AutoThreshold, int);
  mrmlSetMacro(AutoThreshold, int);

  // Description:
  // The upper threshold value to use when autoThreshold is 'no'
  // Warning:
  // XXX-MH Should be floating point....
  mrmlGetMacro(UpperThreshold, double);
  mrmlSetMacro(UpperThreshold, double);

  // Description:
  // The lower threshold value to use when autoThreshold is 'no'
  // Warning:
  // XXX-MH Should be floating point....
  mrmlGetMacro(LowerThreshold, double);
  mrmlSetMacro(LowerThreshold, double);

  // Description:
  // Set/Get interpolate reformated slices
  mrmlGetMacro(Interpolate, int);
  mrmlSetMacro(Interpolate, int);
  mrmlBooleanMacro(Interpolate, int);

  //--------------------------------------------------------------------------
  // RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  // Description:
  // The order of slices in the volume. One of: LR (left-to-right),
  // RL, AP, PA, IS, SI. This information is encoded in the rasToIjkMatrix.
  // This matrix can be computed either from corner points, or just he
  // scanOrder.
  //static void ComputeIjkToRasFromScanOrder(char *order, vtkMatrix4x4 *IjkToRas);

  //static const char* ComputeScanOrderFromIjkToRas(vtkMatrix4x4 *IjkToRas);

  mrmlGetStringMacro(ScanOrder);
  mrmlSetStringMacro(ScanOrder);

  void SetIjkToRasDirections(double dirs[9]);
  void SetIjkToRasDirections(double ir, double ia, double is,
                             double jr, double ja, double js,
                             double kr, double ka, double ks);
  void SetIToRasDirection(double ir, double ia, double is);
  void SetJToRasDirection(double jr, double ja, double ijs);
  void SetKToRasDirection(double kr, double ka, double ks);

  void GetIjkToRasDirections(double dirs[9]);
  void GetIToRasDirection(double dirs[3]);
  void GetJToRasDirection(double dirs[3]);
  void GetKToRasDirection(double dirs[3]);
  double* GetIjkToRasDirections();

  //void GetIjkToRasMatrix(vtkMatrix4x4* mat);
  //void SetIjkToRasMatrix(vtkMatrix4x4* mat);


  // Description:
  // Numerical ID of the storage MRML node
  mrmlSetStringMacro(StorageNodeID);
  mrmlGetStringMacro(StorageNodeID);

  // Description:
  // Numerical ID of the display MRML node
  mrmlSetStringMacro(DisplayNodeID);
  mrmlGetStringMacro(DisplayNodeID);

  StorageNode* GetStorage() const;
  void SetStorage(StorageNode*);

  Volume* GetVolume() const;
  void SetVolume(Volume*);


protected:
  VolumeNode();
  ~VolumeNode();

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, Indent indent) const;

  // Strings
  char *LUTName;
  char *ScanOrder;

  int FileScalarType;
  int FileNumberOfScalarComponents;
  int FileLittleEndian;
  double FileSpacing[3];
  int FileDimensions[3];

  double Window;
  double Level;
  double UpperThreshold;
  double LowerThreshold;


  // Booleans
  int LabelMap;
  int Interpolate;
  int AutoWindowLevel;
  int ApplyThreshold;
  int AutoThreshold;

  double IjkToRasDirections[9];
  double IToRasDirections[3];
  double JToRasDirections[3];
  double KToRasDirections[3];

  char *StorageNodeID;
  char *DisplayNodeID;

  // Moved into VolumeNodeInternals
  //StorageNode        *Storage;
  //Image              *ImageData;

private:
  VolumeNode(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  VolumeNodeInternals *Internal;
};

} // end namespace mrml
#endif

