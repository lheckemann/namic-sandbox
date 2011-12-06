/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

// .NAME vtkDistractorModelingLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT.


#ifndef __vtkDistractorModelingLogic_h
#define __vtkDistractorModelingLogic_h

#include "vtkDistractorModelingWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkSlicerModelsLogic.h"
#include "expat.h"


class vtkIGTLConnector;
class vtkSlicerModelsLogic;

class VTK_DistractorModeling_EXPORT vtkDistractorModelingLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };

  // Distractor Info Structure
  typedef struct{
    double RailAnchor[3];
    double SliderAnchor[3];
    double PistonAnchor[3];
    double CylinderAnchor[3];

    double newSliderAnchorX;
    double newSliderAnchorZ;

    double PistonRotationAngle_deg;

    double Range[2];
  }DistractorInfoStruct;


  std::string RailModelPath;
  std::string SliderModelPath;
  std::string PistonModelPath;
  std::string CylinderModelPath;
  //ETX

  DistractorInfoStruct  Distractor1;


 public:

  static vtkDistractorModelingLogic     *New();

  vtkTypeRevisionMacro(vtkDistractorModelingLogic,vtkObject);
  void  PrintSelf(ostream&, vtkIndent);

  void  MoveDistractor(double value,
                       vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode,
                       vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode,
                       vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode);

  void MoveSlider(double value, vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode);
  void MovePiston(double value, vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode);
  void MoveCylinder(double value, vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode);

  void OpenDistractorFile(const char* xmlFile);

  static void startElement(void *userData, const XML_Char *name, const XML_Char **atts);
  static void endElement(void *userData, const XML_Char *name);

  void LoadModel(const char* modelPath);

 protected:

  vtkDistractorModelingLogic();
  ~vtkDistractorModelingLogic();

  void operator=(const vtkDistractorModelingLogic&);
  vtkDistractorModelingLogic(const vtkDistractorModelingLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif



