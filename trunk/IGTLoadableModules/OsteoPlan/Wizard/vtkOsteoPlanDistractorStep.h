/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkOsteoPlanDistractorStep_h
#define __vtkOsteoPlanDistractorStep_h

#include "vtkOsteoPlanStep.h"
#include "vtkDistractorDefinition.h"
#include "vtkCollection.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkIGTPat2ImgRegistration.h"
#include "vtkMRMLModelNode.h"
#include "vtkMatrix4x4.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWCheckButton.h"
#include "vtkKWMenuButtonWithLabel.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerModelsLogic.h"
#include "expat.h"


class vtkSlicerInteractorStyle;
class vtkKWFrame;
class vtkKWLabel;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWScale;
class vtkKWCheckButton;
class vtkKWMenuButtonWithLabel;
class vtkDistractorDefinition;
class vtkCollection;
class vtkMRMLFiducialListNode;
class vtkIGTPat2ImgRegistration;
class vtkMRMLModelNode;
class vtkMatrix4x4;
class vtkKWFileBrowserDialog;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanDistractorStep : public vtkOsteoPlanStep
{
 public:

  static vtkOsteoPlanDistractorStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanDistractorStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void UpdateGUI();

  void HandleMouseEvent(vtkSlicerInteractorStyle* style);

  void  MoveDistractor(double value,
                       vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode,
                       vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode,
                       vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode,
                       vtkMatrix4x4* mat);

  void MoveSlider(double value, vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode, vtkMatrix4x4* mat);
  void MovePiston(double value, vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode, vtkMatrix4x4* mat);
  void MoveCylinder(double value, vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode, vtkMatrix4x4* mat);

  void OpenDistractorFile(const char* xmlFile);

  static void startElement(void *userData, const XML_Char *name, const XML_Char **atts);
  static void endElement(void *userData, const XML_Char *name);

  void MoveBones(vtkMRMLModelNode* BonePlateModel, vtkMRMLLinearTransformNode* BonePlateTransform,
                 double value);

  vtkGetObjectMacro(DistractorSelected, vtkDistractorDefinition);
  vtkSetObjectMacro(DistractorSelected, vtkDistractorDefinition);

  vtkGetObjectMacro(DistractorList, vtkCollection);


 protected:
  vtkOsteoPlanDistractorStep();
  ~vtkOsteoPlanDistractorStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  void MoveModel();

  //------------------------------
  // Widgets

  vtkKWScale                  *MovingScale;
  vtkKWPushButton             *LoadDistractorButton;
  vtkSlicerNodeSelectorWidget *BonePlateModelSelector;
  vtkSlicerNodeSelectorWidget *BoneRailModelSelector;
  vtkKWCheckButton            *ApplyDistractorToBones;
  vtkKWPushButton             *PlaceFiduButton;
  vtkKWPushButton             *RegisterButton;
  vtkKWMenuButtonWithLabel    *DistractorMenuSelector;
  vtkKWFileBrowserDialog *     DistractorSelector;

  //------------------------------
  // Transformations

  vtkMRMLLinearTransformNode* SliderTransformNode;
  vtkMRMLLinearTransformNode* PistonTransformNode;
  vtkMRMLLinearTransformNode* CylinderTransformNode;
  vtkMRMLLinearTransformNode* BonePlateTransform;

  //------------------------------
  // Registration

  bool                       boolPlacingFiducials;
  vtkMRMLFiducialListNode*   RegistrationFiducialList;
  vtkIGTPat2ImgRegistration* DistToBones;

  //------------------------------
  // Models

  vtkMRMLModelNode* BonePlateModel;
  vtkMRMLModelNode* BoneRailModel;

  //------------------------------
  // Paths

  //BTX
  std::string DistName;
  std::string RailModelPath;
  std::string SliderModelPath;
  std::string PistonModelPath;
  std::string CylinderModelPath;

  std::string xmlPath;
  //ETX

  //------------------------------
  // Distractor

  vtkDistractorDefinition* DistractorSelected;
  vtkDistractorDefinition* DistractorObject;
  vtkCollection*           DistractorList;

  //------------------------------
  // Callback

  bool ProcessingCallback;

 private:
  vtkOsteoPlanDistractorStep(const vtkOsteoPlanDistractorStep&);
  void operator=(const vtkOsteoPlanDistractorStep&);

};

#endif

