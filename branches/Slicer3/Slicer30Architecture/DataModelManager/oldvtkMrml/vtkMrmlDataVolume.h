/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlDataVolume.h,v $
  Date:      $Date: 2005/12/20 22:44:22 $
  Version:   $Revision: 1.14.2.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlDataVolume_h
#define __vtkMrmlDataVolume_h

#include <stdlib.h>

#include "vtkMrmlData.h"

#include "vtkMrmlVolumeNode.h"
#include "vtkImageAccumulateDiscrete.h"
#include "vtkImageBimodalAnalysis.h"
#include "vtkImageResize.h"
#include "vtkImagePlot.h"
#include "vtkImageData.h"
#include "vtkStackOfPolygons.h"
#include "vtkSlicer.h"
#include "vtkMrmlDataVolumeReadWrite.h"
#include "vtkPTSWriter.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class vtkImageDICOMReader;
class vtkImageReader;

//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlDataVolume : public vtkMrmlData
{
public:
  static vtkMrmlDataVolume *New();
  vtkTypeMacro(vtkMrmlDataVolume,vtkMrmlData);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Provides opportunity to insure internal consistency before access. 
  // Transfers all ivars from MrmlNode to internal VTK objects
  // Also causes ImageData's source (if any) to update itself.
  void Update();
  unsigned long int GetMTime();

  // Description:
  // Set the image data
  // Use GetOutput to get the image data.
  vtkSetObjectMacro(ImageData, vtkImageData);
  vtkImageData* GetOutput();
  char* GetOutputPointer(int zslice);

  // Description:
  // Read/Write image 
  int Read();
  int Write();
  int WritePTS(char *filename);
  int WritePTSFromStack(int window, char *filename,
                        vtkMatrix4x4 *RasToIjkMatrix,
                        char *order, int activeSlice);

  //--------------------------------------------------------------------------
  // Read/Write volume data contained by this object.
  // This sub-object is specific to each
  // type of volume that needs to be read in.  This can be used
  // to clean up the special cases which handle
  // volumes of various types, such as dicom, header, etc.  In
  // future these things can be moved here.  Each read/write 
  // sub-object corresponds to a vtkMrmlVolumeReadWriteNode subclass.
  // These subclasses write any needed info in the MRML file.
  //--------------------------------------------------------------------------
  vtkSetObjectMacro(ReadWrite,vtkMrmlDataVolumeReadWrite);
  vtkGetObjectMacro(ReadWrite,vtkMrmlDataVolumeReadWrite);

  // Description:
  // Set Histogram attributes
  void SetHistogramHeight(int h) ;
  void SetHistogramWidth(int w);
  vtkSetVector3Macro(HistogramColor, vtkFloatingPointType);

  // Description:
  // Get Histogram plot (an image)
  vtkImageData *GetHistogramPlot();

  int GetBimodalThreshold() ;

  // Description:
  // Assumes GetHistogramPlot has already been called
  int MapBinToScalar(int b) ;
  int MapScalarToBin(int s) ;
  
  // Description:
  // Set/Get window/level/threshold slider range
  vtkGetMacro(RangeLow,  vtkFloatingPointType);
  vtkGetMacro(RangeHigh, vtkFloatingPointType);
  vtkSetMacro(RangeLow,  vtkFloatingPointType);
  vtkSetMacro(RangeHigh, vtkFloatingPointType);
  vtkGetMacro(RangeAuto, int);
  vtkSetMacro(RangeAuto, int);
  vtkBooleanMacro(RangeAuto, int);

  void StackSetPolygon(int window, vtkPoints *poly, int s, int p, int d,
                       int closed, int preshape, int label);
  
  void StackRemovePolygon(int window, int s, int p);

  void RasStackRemovePolygon(int window, int s, int p);

  void RasStackSetPolygon(int window, vtkPoints *rasPoly, int s, int p, int d,
                          int closed, int preshape, int label);

};

#endif
