/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkBetaProbeNavLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkBetaProbeNavLogic_h
#define __vtkBetaProbeNavLogic_h

#include "vtkBetaProbeNavWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkPoints;
class vtkFloatArray;
class vtkPolyData;
class vtkMRMLModelNode;
class vtkMRMLNode;
class vtkMRMLModelDisplayNode;
class vtkMRMLScalarVolumeDisplayNode;
class vtkImageData;
class vtkMRMLScalarVolumeNode;

class VTK_BetaProbeNav_EXPORT vtkBetaProbeNavLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkBetaProbeNavLogic *New();
  
  vtkTypeRevisionMacro(vtkBetaProbeNavLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  void CollectData(vtkMRMLNode*, vtkMRMLNode*);
  vtkMRMLModelNode* RepresentData(vtkMRMLModelNode* mnode);
  vtkMRMLModelNode* RepresentDataRT(vtkMRMLModelNode* mnode);
  vtkMRMLScalarVolumeNode* PaintImage(vtkMRMLNode* inode, vtkMRMLScalarVolumeNode* snode);
  void ClearArrays();
  double GetMaxRange() { return this->maxRange; };
  void SetMaxRange(double rg) { this->maxRange = rg; };
  vtkImageData* GetImageData() { return this->image; };
  void SetImageData (vtkImageData* im) { this->image = im; };

 protected:
  
  vtkBetaProbeNavLogic();
  ~vtkBetaProbeNavLogic();

  void operator=(const vtkBetaProbeNavLogic&);
  vtkBetaProbeNavLogic(const vtkBetaProbeNavLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();
  
  vtkCallbackCommand *DataCallbackCommand;

 private:
   vtkPoints* Points;
   vtkFloatArray* SmoothScalars;
   vtkFloatArray* BetaScalars;
   vtkFloatArray* GammaScalars;
   vtkFloatArray* nSmoothScalars;
   vtkFloatArray* nBetaScalars;
   vtkFloatArray* nGammaScalars;
   vtkPolyData* CountMap;
   vtkMRMLModelDisplayNode* dispNode;
   vtkImageData* image;
   double maxRange;
   double probeDiam;

};

#endif


  
