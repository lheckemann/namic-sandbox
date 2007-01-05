/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlVolumeNode.h,v $
  Date:      $Date: 2005/12/20 22:44:32 $
  Version:   $Revision: 1.40.2.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlVolumeNode_h
#define __vtkMrmlVolumeNode_h

#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"
#include "vtkMrmlVolumeReadWriteNode.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif


class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeNode : public vtkMrmlNode
{
  public:
  static vtkMrmlVolumeNode *New();
  vtkTypeMacro(vtkMrmlVolumeNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // these control the center of the voxel in ijk space - default is 0.5, center
  // of the voxel while 0.0 would be upper corner
  static void SetGlobalVoxelOffset(float offset);
  static float GetGlobalVoxelOffset();
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);


  //--------------------------------------------------------------------------
  // How to Read/Write volume data contained by this node.
  // This sub-node should contain information specific to each
  // type of volume that needs to be read in.  This can be used
  // to clean up the special cases in this file which handle
  // volumes of various types, such as dicom, header, etc.  In
  // future these things can be moved to the sub-node specific for that
  // type of volume.  The sub-nodes here that describe specific volume
  // types each correspond to an implementation of the reader/writer,
  // which can be found in a vtkMrmlDataVolumeReadWrite subclass.
  //--------------------------------------------------------------------------
  vtkSetObjectMacro(ReadWriteNode, vtkMrmlVolumeReadWriteNode);
  vtkGetObjectMacro(ReadWriteNode, vtkMrmlVolumeReadWriteNode);

  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Volume ID
  vtkSetStringMacro(VolumeID);
  vtkGetStringMacro(VolumeID);
  
  // Description:
  // The pattern for naming the files expressed in C language syntax. 
  // For example, a file named skin.001 has a patter of %s.%03d, and a file
  // named skin.1 has a pattern of %s.%d
  vtkSetStringMacro(FilePattern);
  vtkGetStringMacro(FilePattern);

  // Description:
  // Prefix of the image files. For example, the prefix of /d/skin.001 is /d/skin
  vtkSetStringMacro(FilePrefix);
  vtkGetStringMacro(FilePrefix);

  // Description:
  // Type of data file - e.g. Analyze, Dicom, Headerless, etc.  If not set, slicer will 
  // try to figure it from the other header values
  vtkSetStringMacro(FileType);
  vtkGetStringMacro(FileType);

  //--------------------------------------------------------------------------
  // Header Information
  //--------------------------------------------------------------------------

  // Description:
  // Two numbers: the first and last image numbers in the volume. 
  // For example: 1 124
  vtkGetVector2Macro(ImageRange, int);
  vtkSetVector2Macro(ImageRange, int);

  // Description:
  // Two numbers: the number of columns and rows of pixels in each image
  vtkGetVector2Macro(Dimensions, int);
  vtkSetVector2Macro(Dimensions, int);

  // Description:
  // Three numbers for the dimensions of each voxel, in millimeters
  vtkGetVector3Macro(Spacing, vtkFloatingPointType);
  vtkSetVector3Macro(Spacing, vtkFloatingPointType);
  
  // Description:
  // The type of data in the file. One of: Char, UnsignedChar, Short, 
  // UnsignedShort, Int, UnsignedInt, Long, UnsignedLong, Float, Double
  vtkSetMacro(ScalarType, int);
  vtkGetMacro(ScalarType, int);
  void SetScalarTypeToUnsignedChar() 
    ;
  void SetScalarTypeToChar() 
    ;
  void SetScalarTypeToShort() ;
  void SetScalarTypeToUnsignedShort() 
    ;
  void SetScalarTypeToInt() ;
  void SetScalarTypeToUnsignedInt() ;
  void SetScalarTypeToLong() ;
  void SetScalarTypeToUnsignedLong() ;
  void SetScalarTypeToFloat() ;
  void SetScalarTypeToDouble() ;
  char* GetScalarTypeAsString();
  
  // Description:
  // The number of scalar components for each voxel. 
  // Gray-level data has 1. Color data has 3
  vtkGetMacro(NumScalars, int);
  vtkSetMacro(NumScalars, int);

  // Description:
  // Describes the order of bytes for each voxel.  Little endian 
  // positions the least-significant byte on the rightmost end, 
  // and is true of data generated on a PC or SGI.
  vtkGetMacro(LittleEndian, int);
  vtkSetMacro(LittleEndian, int);
  vtkBooleanMacro(LittleEndian, int);

  // Description:
  // The gantry tilt, in degrees, for CT slices
  vtkGetMacro(Tilt, vtkFloatingPointType);
  vtkSetMacro(Tilt, vtkFloatingPointType);


  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Numerical ID of the color lookup table to use for rendering the volume
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

  // Description:
  // Indicates if this volume is a label map, which is the output of 
  // segmentation that labels each voxel according to its tissue type.  
  // The alternative is a gray-level or color image.
  vtkGetMacro(LabelMap, int);
  vtkSetMacro(LabelMap, int);
  vtkBooleanMacro(LabelMap, int);

  // Description:
  // Specifies whether windowing and leveling are to be performed automatically
  vtkBooleanMacro(AutoWindowLevel, int);
  vtkGetMacro(AutoWindowLevel, int);
  vtkSetMacro(AutoWindowLevel, int);

  // Description:
  // The window value to use when autoWindowLevel is 'no'
  vtkGetMacro(Window, vtkFloatingPointType);
  vtkSetMacro(Window, vtkFloatingPointType);

  // Description:
  // The level value to use when autoWindowLevel is 'no'
  vtkGetMacro(Level, vtkFloatingPointType);
  vtkSetMacro(Level, vtkFloatingPointType);

  // Description:
  // Specifies whether to apply the threshold
  vtkBooleanMacro(ApplyThreshold, int);
  vtkGetMacro(ApplyThreshold, int);
  vtkSetMacro(ApplyThreshold, int);

  // Description:
  // Specifies whether the threshold should be set automatically
  vtkBooleanMacro(AutoThreshold, int);
  vtkGetMacro(AutoThreshold, int);
  vtkSetMacro(AutoThreshold, int);

  // Description:
  // The upper threshold value to use when autoThreshold is 'no'
  // Warning:
  // XXX-MH Should be floating point....
  vtkGetMacro(UpperThreshold, vtkFloatingPointType);
  vtkSetMacro(UpperThreshold, vtkFloatingPointType);

  // Description:
  // The lower threshold value to use when autoThreshold is 'no'
  // Warning:
  // XXX-MH Should be floating point....
  vtkGetMacro(LowerThreshold, vtkFloatingPointType);
  vtkSetMacro(LowerThreshold, vtkFloatingPointType);

  // Description:
  // MR Diffusion Tensor Images may be saved on disk with 
  // the frequency encode direction non-standard.  For supine
  // scans in the slicer, set this to 1 for in-plane 
  // rotation/axis swaps which will put the images into standard
  // ax/sag/cor orientations.  This is currently a boolean variable
  // here and not written to MRML; its effects are seen in the 
  // RasToIjk matrix.  In future this boolean, or a better description
  // of the transformation, may be put into the MRML format.
  vtkGetMacro(FrequencyPhaseSwap, int);
  vtkSetMacro(FrequencyPhaseSwap, int);

  // Description:
  // Set/Get interpolate reformated slices
  vtkGetMacro(Interpolate, int);
  vtkSetMacro(Interpolate, int);
  vtkBooleanMacro(Interpolate, int);

  //--------------------------------------------------------------------------
  // RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  // Description:
  // The order of slices in the volume. One of: LR (left-to-right), 
  // RL, AP, PA, IS, SI. This information is encoded in the rasToIjkMatrix.
  // This matrix can be computed either from corner points, or just he
  // scanOrder.
  void ComputeRasToIjkFromScanOrder(char *order);
  static char* ComputeScanOrderFromRasToIjk(vtkMatrix4x4 *RasToIjk);

  void ComputePositionMatrixFromRasToVtk(vtkMatrix4x4* RasToVtkMatrix);

  void SetScanOrder(char *s);
  vtkGetStringMacro(ScanOrder);

  // Description:
  // Compute the rasToIjkMatrix from the corner points of the volume.

  int ComputeRasToIjkFromCorners(vtkFloatingPointType *fc, vtkFloatingPointType *ftl, vtkFloatingPointType *ftr, 
    vtkFloatingPointType *fbr, vtkFloatingPointType *lc, vtkFloatingPointType *ltl, vtkFloatingPointType zoffset);

  int ComputeRasToIjkFromCorners(vtkFloatingPointType *fc, vtkFloatingPointType *ftl, vtkFloatingPointType *ftr, 
                                 vtkFloatingPointType *fbr, vtkFloatingPointType *lc, vtkFloatingPointType *ltl)
  ;

  int ComputeRasToIjkFromCorners(
    vtkFloatingPointType fcR,  vtkFloatingPointType fcA,  vtkFloatingPointType fcS,
    vtkFloatingPointType ftlR, vtkFloatingPointType ftlA, vtkFloatingPointType ftlS, 
    vtkFloatingPointType ftrR, vtkFloatingPointType ftrA, vtkFloatingPointType ftrS, 
    vtkFloatingPointType fbrR, vtkFloatingPointType fbrA, vtkFloatingPointType fbrS, 
    vtkFloatingPointType lcR,  vtkFloatingPointType lcA,  vtkFloatingPointType lcS, 
    vtkFloatingPointType ltlR, vtkFloatingPointType ltlA, vtkFloatingPointType ltlS) ;

int ComputeRasToIjkFromCorners(
    vtkFloatingPointType fcR,  vtkFloatingPointType fcA,  vtkFloatingPointType fcS,
    vtkFloatingPointType ftlR, vtkFloatingPointType ftlA, vtkFloatingPointType ftlS, 
    vtkFloatingPointType ftrR, vtkFloatingPointType ftrA, vtkFloatingPointType ftrS, 
    vtkFloatingPointType fbrR, vtkFloatingPointType fbrA, vtkFloatingPointType fbrS, 
    vtkFloatingPointType lcR,  vtkFloatingPointType lcA,  vtkFloatingPointType lcS, 
    vtkFloatingPointType ltlR, vtkFloatingPointType ltlA, vtkFloatingPointType ltlS,
    vtkFloatingPointType zoffset) ;

  // Description:
  // The matrix that transforms a point in RAS space to IJK space. 
  // The 4x4 matrix is listed as a string of 16 numbers in row-major order.
  vtkSetStringMacro(RasToIjkMatrix);
  vtkGetStringMacro(RasToIjkMatrix);

  // Description:
  // Same as the rasToIjkMatrix except the y-axis is directed up instead 
  // of down, as is the convention in VTK
  vtkSetStringMacro(RasToVtkMatrix);
  vtkGetStringMacro(RasToVtkMatrix);

  // Description:
  // A ScaledIJK-to-RAS matrix formed by accounting for the position and 
  // orientation of the volume, but not the voxel size
  vtkSetStringMacro(PositionMatrix);
  vtkGetStringMacro(PositionMatrix);


  // Description:
  // This function solves the 4x4 matrix equation
  // A*B=C for the unknown matrix A, given matrices B and C.
  // While this is equivalent to A=C*Inverse(B), this function uses
  // faster and more accurate methods (LU factorization) than finding a 
  // matrix inverse and multiplying.  Returns 0 on failure.
  static int SolveABeqCforA(vtkMatrix4x4 * A,  vtkMatrix4x4 * B,
                vtkMatrix4x4 * C);
  static int SolveABeqCforB(vtkMatrix4x4 * A,  vtkMatrix4x4 * B,
                vtkMatrix4x4 * C);

  //--------------------------------------------------------------------------
  // Used by the MRML Interpreter
  //--------------------------------------------------------------------------
  
  // Description:
  // Full path name
  vtkSetStringMacro(FullPrefix);
  vtkGetStringMacro(FullPrefix);

  // Description:
  // Specify whether to use the RasToIjk or RasToVtk (y-flipped) matrix
  vtkGetMacro(UseRasToVtkMatrix, int);
  vtkSetMacro(UseRasToVtkMatrix, int);
  vtkBooleanMacro(UseRasToVtkMatrix, int);

  // Description:
  // Specify the registration matrix which this routine uses
  // to rebuild the WldToIjk matrix.
  // Uses the value of UseRasToVtkMatrix.
  void SetRasToWld(vtkMatrix4x4 *reg);

  // Description:
  // Get the final result of registration
  vtkGetObjectMacro(WldToIjk, vtkMatrix4x4);
  vtkGetObjectMacro(RasToWld, vtkMatrix4x4);
  vtkGetObjectMacro(RasToIjk, vtkMatrix4x4);
  vtkGetObjectMacro(Position, vtkMatrix4x4);

  // DICOMFileList
  int GetNumberOfDICOMFiles();
  void AddDICOMFileName(char *);
  char *GetDICOMFileName(int idx);
  void SetDICOMFileName(int idx, char *str);
  void DeleteDICOMFileNames();
  char **GetDICOMFileNamesPointer();

  int GetNumberOfDICOMMultiFrameOffsets();
  void AddDICOMMultiFrameOffset(int offset);
  int GetDICOMMultiFrameOffset(int idx);
  void DeleteDICOMMultiFrameOffsets();
  int *GetDICOMMultiFrameOffsetPointer();
  // End
  
};

#endif

