/*==========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
  Author: Roland Goerlitz, University of Karlsruhe

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: ??$
  Date:      $Date: 2009/03/09 09:10:09$
  Version:   $Revision: 1.00$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkRealTimeNeedleDetectionGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLTransformableNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLROINode.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntry.h"
#include "vtkKWCheckButton.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkCornerAnnotation.h"

#define PI 3.1415926535897932384626433832795 
//TODO: how do I access the math pi?
#define PATIENTLEFT      1
#define PATIENTRIGHT     2
#define PATIENTPOSTERIOR 3
#define PATIENTANTERIOR  4
#define PATIENTINFERIOR  5
#define PATIENTSUPERIOR  6

#define DEFAULTTHRESHOLD 48000
#define DEFAULTINTENSITY 15000
#define DEFAULTERODE     3
#define DEFAULTDILATE    2

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkRealTimeNeedleDetectionGUI);
vtkCxxRevisionMacro (vtkRealTimeNeedleDetectionGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constructor
vtkRealTimeNeedleDetectionGUI::vtkRealTimeNeedleDetectionGUI()
{
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void *> (this));
  this->DataCallbackCommand->SetCallback(vtkRealTimeNeedleDetectionGUI::DataCallback);
  started       = 0;
  ROIpresent    = 0;
  showNeedle    = 0;
  scanPlane     = 0;
  needleOrigin  = PATIENTLEFT; //if this default value gets changed, the initial value for pEntryPointButtonSet needs to get changed, too
  
  //----------------------------------------------------------------
  // GUI widgets
  this->pVolumeSelector           = NULL; 
  this->pThresholdScale           = NULL;
  this->pIntensityScale           = NULL;
  this->pGaussScale               = NULL;
  this->pDilateEntry              = NULL;
  this->pErodeEntry               = NULL;
  this->pEntryPointButtonSet      = NULL;
  this->pStartButton              = NULL;
  this->pStopButton               = NULL;
  this->pShowNeedleButton         = NULL;
  this->pXLowerEntry              = NULL;
  this->pXUpperEntry              = NULL;
  this->pYLowerEntry              = NULL;
  this->pYUpperEntry              = NULL;
  this->pZLowerEntry              = NULL;
  this->pZUpperEntry              = NULL;
  
  //--------------------------------------------------------------------
  // MRML nodes  
  this->pSourceNode          = NULL;
  this->pOutputNode          = NULL;
  this->pNeedleModelNode     = NULL;
  this->pNeedleTransformNode = NULL;
  this->pScanPlaneNormalNode = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)   
  this->TimerFlag = 0;
   
  //----------------------------------------------------------------
  // Image Values - default  
  currentXLowerBound        = 0;
  currentXUpperBound        = 256;
  currentYLowerBound        = 0;
  currentYUpperBound        = 256;
  currentZLowerBound        = 0;
  currentZUpperBound        = 256;
  currentXImageRegionSize   = 256;
  currentYImageRegionSize   = 256;
  currentZImageRegionSize   = 256;
  needleDetectionThreshold  = DEFAULTTHRESHOLD;
  needleIntensity           = DEFAULTINTENSITY;
  gaussVariance             = 0;                  // = LaplacianGaussian Filter does not get used
  imageDimensions[0]        = 256;
  imageDimensions[1]        = 256;
  imageDimensions[2]        = 1;
  imageSpacing[0]           = 1;  
  imageSpacing[1]           = 1;
  imageSpacing[2]           = 1;
  imageOrigin[0]            = 0;
  imageOrigin[1]            = 0;
  imageOrigin[2]            = 0;
  scalarSize                = 2;                      // scalarType 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  lastModified              = 0;
  pImage                    = NULL;
  pImageProcessor           = new ImageProcessor::ImageProcessor();  //TODO:move the new to starting the whole detection
  std::cout << "NeedleDetection constructed" << std::endl;
}

//---------------------------------------------------------------------------
// Destructor
vtkRealTimeNeedleDetectionGUI::~vtkRealTimeNeedleDetectionGUI()
{
  std::cout << "NeedleDetection destruction started" << std::endl;
  //----------------------------------------------------------------
  // Remove Callbacks
  if (this->DataCallbackCommand)
    this->DataCallbackCommand->Delete();

  //----------------------------------------------------------------
  // Remove Observers
  this->RemoveGUIObservers();
  //TODO: remove MRMLObservers

  //----------------------------------------------------------------
  // Remove GUI widgets
  if (this->pVolumeSelector)
  {
    this->pVolumeSelector->SetParent(NULL);
    this->pVolumeSelector->Delete();
  }
  if(this->pThresholdScale) 
  {
    this->pThresholdScale->SetParent(NULL);
    this->pThresholdScale->Delete();
  }
  if(this->pIntensityScale) 
  {
    this->pIntensityScale->SetParent(NULL);
    this->pIntensityScale->Delete();
  }
   if(this->pGaussScale) 
  {
    this->pGaussScale->SetParent(NULL);
    this->pGaussScale->Delete();
  }
  if (this->pErodeEntry)
  {
    this->pErodeEntry->SetParent(NULL);
    this->pErodeEntry->Delete();
  }
  if (this->pErodeEntry)
  {
    this->pDilateEntry->SetParent(NULL);
    this->pDilateEntry->Delete();
  }
  if(this->pEntryPointButtonSet)
  {
    this->pEntryPointButtonSet->SetParent(NULL);
    this->pEntryPointButtonSet->Delete();
  }
  if(this->pStartButton)
  {
    this->pStartButton->SetParent(NULL);
    this->pStartButton->Delete();
  }
  if(this->pStopButton)
  {
    this->pStopButton->SetParent(NULL);
    this->pStopButton->Delete();
  }
  if(this->pShowNeedleButton)
  {
    this->pShowNeedleButton->SetParent(NULL );
    this->pShowNeedleButton->Delete ( );
  }
  
  if (this->pXLowerEntry)
  {
    this->pXLowerEntry->SetParent(NULL);
    this->pXLowerEntry->Delete();
  }
  if(this->pXUpperEntry)
  {
    this->pXUpperEntry->SetParent(NULL);
    this->pXUpperEntry->Delete();
  }
  if (this->pYLowerEntry)
  {
    this->pYLowerEntry->SetParent(NULL);
    this->pYLowerEntry->Delete();
  }
  if(this->pYUpperEntry)
  {
    this->pYUpperEntry->SetParent(NULL);
    this->pYUpperEntry->Delete();
  }
  if(this->pZLowerEntry)
  {
    this->pZLowerEntry->SetParent(NULL);
    this->pZLowerEntry->Delete();
  }
  if (this->pZUpperEntry)
  {
    this->pZUpperEntry->SetParent(NULL);
    this->pZUpperEntry->Delete();
  }
    
  //-----------------------------------------------------------------
  // Remove MRML nodes
  if(this->pOutputNode)
    this->pOutputNode->Delete(); 
  if(this->pSourceNode)           
    this->pSourceNode->Delete();
  if(this->pNeedleModelNode)
    this->pNeedleModelNode->Delete(); 
  if(this->pNeedleTransformNode)
    this->pNeedleTransformNode->Delete();
  if(this->pScanPlaneNormalNode)
    this->pScanPlaneNormalNode->Delete();

  //----------------------------------------------------------------
  // Unregister Logic class
  this->SetModuleLogic(NULL);
  
  //----------------------------------------------------------------
  // Delete pointers
  //delete pImageProcessor; //TODO: Does not work properly yet
  // TODO: delete pImage
  
  std::cout << "NeedleDetection destructed" << std::endl;
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Init()
{
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Enter()
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Exit()
{
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "RealTimeNeedleDetectionGUI: " << this->GetClassName() << "\n";
  os << indent << "Logic: " << this->GetLogic() << "\n";
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML Observers
  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
  {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
  }
  events->Delete();
  
  //----------------------------------------------------------------
  // GUI Observers
  this->pVolumeSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pThresholdScale->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pIntensityScale->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pGaussScale->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pStartButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pStopButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pShowNeedleButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);

  this->AddLogicObservers();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::RemoveGUIObservers ( )
{
  //----------------------------------------------------------------
  // MRML Observers
  //this->MRMLObserverManager->RemoveObjectEvents(pSourceNode);
  //this->MRMLObserverManager->RemoveObjectEvents(pNeedleModelNode);
  //TODO: Remove Observers of MRMLSceneEvents
  
  //----------------------------------------------------------------
  // GUI Observers
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  this->pVolumeSelector->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*) this->GUICallbackCommand);
  if(this->pThresholdScale)
    this->pThresholdScale->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  if(this->pIntensityScale)
    this->pIntensityScale->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  if(this->pGaussScale)
    this->pGaussScale->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  if (this->pEntryPointButtonSet)
  {
    this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  }
  if(this->pStartButton)
    this->pStartButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if(this->pStopButton)
    this->pStopButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if (this->pShowNeedleButton)
    this->pShowNeedleButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    
  this->RemoveLogicObservers();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkRealTimeNeedleDetectionLogic::StatusUpdateEvent, (vtkCommand*) this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand*) this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //TODO:check events first in these ifs, because it is probably faster than the downcast

  const char* eventName = vtkCommand::GetStringFromEventId(event);

  if(strcmp(eventName, "LeftButtonPressEvent") == 0)  // This is not used yet, since only mouse clicks on buttons are processed
  {
    vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
  }
  
  else if(this->pThresholdScale == vtkKWScaleWithEntry::SafeDownCast(caller) && event == vtkKWScale::ScaleValueChangedEvent)
  {
    std::cout << "NeedleThreshold changed." << std::endl;
    needleDetectionThreshold = this->pThresholdScale->GetValue();
  }
  
  else if(this->pIntensityScale == vtkKWScaleWithEntry::SafeDownCast(caller) && event == vtkKWScale::ScaleValueChangedEvent)   
  {
    std::cout << "NeedleIntensity changed." << std::endl;
    needleIntensity = this->pIntensityScale->GetValue();
  }
  
  else if(this->pGaussScale == vtkKWScaleWithEntry::SafeDownCast(caller) && event == vtkKWScale::ScaleValueChangedEvent)   
  {
    std::cout << "Gaussvariance changed." << std::endl;
    gaussVariance = this->pGaussScale->GetValue();
  }
  
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTLEFT) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->GetSelectedState() == 1)
  { 
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTLEFT;
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->GetSelectedState() == 1)
  {
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTRIGHT; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->GetSelectedState() == 1)
  {
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTPOSTERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->GetSelectedState() == 1)
  {
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTANTERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->GetSelectedState() == 1)
  {
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTINFERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->GetSelectedState() == 1)
  {
    std::cout << "NeedleOrientatoinButton is pressed." << std::endl;
    needleOrigin = PATIENTSUPERIOR; 
  }  
  
  else if(this->pStartButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {
    std::cout << "StartButton is pressed." << std::endl;
       
    //-----------------------------------------------------------------------------------------------------------------
    // Register the scanner node as pSourceNode to the event observer | it gets unregistered when the StopButton is pressed
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(this->pVolumeSelector->GetSelected());  
    if(!node)
      std::cout << "no node" << std::endl;
    else
      std::cout << "node exists" << std::endl;
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
    vtkSetAndObserveMRMLNodeEventsMacro(pSourceNode, node, nodeEvents);   //this registers the in pVolumeSelector selected node to pSourceNode  
    if(!node)
      std::cout << "no node" << std::endl;
    else
      std::cout << "node exists" << std::endl;
    nodeEvents->Delete();   
    //TODO:Steve Do I have to delete node? Does it actually exist?
    if(pSourceNode)
    { //-----------------------------------------------------------------------------------------------------------------
      // Create the OutputNode and the corresponding displayNode, which displays the detected needle   
      std::cout << "pSourceNode exists" << std::endl;   
      if(pOutputNode == NULL) // pOutputNode does not exist yet
      {
        vtkMRMLScalarVolumeNode* pScalarNode = vtkMRMLScalarVolumeNode::New();
        pScalarNode->SetLabelMap(0);   // set the label map to grey scale
        pOutputNode = pScalarNode;    
        if(pOutputNode == NULL)
          std::cout << "ERROR! OutputNode == NULL!" << std::endl;
        pOutputNode->UpdateID("OutputNode");
        pOutputNode->SetName("OutputNode");
        pOutputNode->SetDescription("MRMLNode that displays the tracked needle and the original image");
        pOutputNode->SetScene(this->GetMRMLScene()); 
              
        vtkMRMLScalarVolumeDisplayNode* pScalarDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();
        pScalarDisplayNode->SetDefaultColorMap();   
        vtkMRMLScalarVolumeDisplayNode* pDisplayNode = pScalarDisplayNode;         
        this->GetMRMLScene()->AddNode(pDisplayNode);   
        pOutputNode->SetAndObserveDisplayNodeID(pDisplayNode->GetID());        
        this->GetMRMLScene()->AddNode(pOutputNode);
        pScalarNode->Delete();
        pScalarDisplayNode->Delete();
        pDisplayNode->Delete(); 
      }
      else //OutputNode exists already
      {
        std::cerr << "OutputNode exists already. Starting needle tracking again" << std::endl;        
      }      
      //-------------------------------------------------------------------
      // Create a MRMLModelNode that displays the needle
      if(!pNeedleModelNode)       
      {
        pNeedleModelNode = vtkMRMLModelNode::New();
        pNeedleModelNode->SetName("NeedleModel");
        pNeedleModelNode->UpdateID("NeedleModel");
        pNeedleModelNode->SetScene(this->GetMRMLScene());
        pNeedleModelNode->SetHideFromEditors(0);
              
        this->GetMRMLScene()->AddNode(pNeedleModelNode);
        std::cout << "NeedleModelNode added" << std::endl;
        MakeNeedleModel();
        
        vtkMRMLDisplayNode* pNeedleDisplay = pNeedleModelNode->GetDisplayNode();  //GetDisplayNode() does not create a new reference -> do not delete later on
        pNeedleDisplay->SetVisibility(0);
        pNeedleModelNode->Modified();
        this->GetMRMLScene()->Modified();        
      }
      
      //------------------------------------------------------------------------------------------------
      // Create a TransformNode that changes according to the found needle and will be observed by the ModelNode to display the needle  
      if(!pNeedleTransformNode) // If the NeedleTransformNode doesn't exist yet -> make a new one 
      {
        pNeedleTransformNode = vtkMRMLLinearTransformNode::New();
        pNeedleTransformNode->SetName("NeedleTransform");
        pNeedleTransformNode->UpdateID("NeedleTransform");
        pNeedleTransformNode->SetScene(this->GetMRMLScene());
        pNeedleTransformNode->SetHideFromEditors(0);
        vtkMatrix4x4* transform = vtkMatrix4x4::New(); // vtkMatrix is initialized with the identity matrix
        pNeedleTransformNode->ApplyTransform(transform);  // SetAndObserveMatrixTransformToParent called in this function
        transform->Delete();        
        this->GetMRMLScene()->AddNode(pNeedleTransformNode);         
        std::cout << "NeedleTransformNode added" << std::endl;
        
        pNeedleModelNode->SetAndObserveTransformNodeID(pNeedleTransformNode->GetID());
        pNeedleModelNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      }
      
      //------------------------------------------------------------------------------------------------
      // Create a TransformNode that is the normal of the scan plane  
      if(!pScanPlaneNormalNode) // If the NeedleTransformNode doesn't exist yet -> make a new one 
      {
        pScanPlaneNormalNode = vtkMRMLLinearTransformNode::New();
        pScanPlaneNormalNode->SetName("ScanPlaneNormal");
        pScanPlaneNormalNode->UpdateID("ScanPlaneNormal");
        pScanPlaneNormalNode->SetScene(this->GetMRMLScene());
        pScanPlaneNormalNode->SetHideFromEditors(0);
        vtkMatrix4x4* transform = vtkMatrix4x4::New(); // vtkMatrix is initialized with the identity matrix
        pScanPlaneNormalNode->ApplyTransform(transform);  // SetAndObserveMatrixTransformToParent called in this function
        
        this->GetMRMLScene()->AddNode(pScanPlaneNormalNode);         
        std::cout << "ScanPlaneNormalNode added" << std::endl;
      }
      
      started = 1; // start checking for changes in pSourceNode to update pOutputNode     
      std::cerr << "Start checking for changes" << std::endl;
    }
    else // no Scanner node selected from MRMLScene
      std::cerr << "ERROR! No Scanner detected. RealTimeNeedleDetection did not start." << std::endl;
  }
  
  else if (this->pStopButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {    
    std::cerr << "StopButton is pressed." << std::endl;
    // unregister the observer of the ScannerMRMLNode | do not delete pSourceNode, because it gets used again when start is pressed again
    if(this->pSourceNode)
    {
      this->MRMLObserverManager->RemoveObjectEvents(pSourceNode);
      this->pSourceNode = NULL;  
    }
       
    started = 0; // Stop checking MRLM events of pSourceNode 
  }
  
  else if (this->pShowNeedleButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent )
  {
    std::cout << "ShowNeedleButton is pressed." << std::endl;
    showNeedle = this->pShowNeedleButton->GetSelectedState(); 
    if(showNeedle)
    {      
      vtkMRMLDisplayNode* pNeedleDisplay = pNeedleModelNode->GetDisplayNode(); //GetDisplayNode() does not create a new reference -> do not delete later on 
      if(pNeedleDisplay)
      {
        pNeedleDisplay->SetVisibility(1);
        pNeedleModelNode->Modified();
        this->GetMRMLScene()->Modified();
      }
      else
        std::cerr << "Error! DisplayNode for needle does not exist!" << std::endl;      
    }
    else // !showNeedle
    {
      if(pNeedleModelNode) // if a node exists, set visibility to 0
      {
        vtkMRMLDisplayNode* pNeedleDisplay = pNeedleModelNode->GetDisplayNode();  
        if(pNeedleDisplay)
        {
          pNeedleDisplay->SetVisibility(0);
          pNeedleModelNode->Modified();
          this->GetMRMLScene()->Modified();
        }
        else
          std::cerr << "Error! DisplayNode for needle does not exist!" << std::endl;
      }
      else 
      {
        std::cout << "Error! Node does not exist!" << std::endl;
         // If a node doesn't exist, do nothing! However, this should never get called
      }
    }
  }
      
  UpdateGUI(); // enable or disable GUIoptions for the user
} 

void vtkRealTimeNeedleDetectionGUI::DataCallback(vtkObject* caller, unsigned long eid, void* clientData, void* callData)
{
  vtkRealTimeNeedleDetectionGUI* self = reinterpret_cast<vtkRealTimeNeedleDetectionGUI*> (clientData);
  vtkDebugWithObjectMacro(self, "In vtkRealTimeNeedleDetectionGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessLogicEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (this->GetLogic() == vtkRealTimeNeedleDetectionLogic::SafeDownCast(caller))
  {
    if (event == vtkRealTimeNeedleDetectionLogic::StatusUpdateEvent)
    {
      //this->UpdateDeviceStatus();
    }
  }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData )
{
  //TODO: If new mrmlNode added -> pScannerIDEntry=new mrml node 
  //TODO: if MRMLNode deleted -> pScannerIDEntry=""
  
  if(event == vtkMRMLScene::SceneCloseEvent) //TODO: Do I actually catch this event?
  {
    if (this->pShowNeedleButton != NULL && this->pShowNeedleButton->GetSelectedState())
      this->pShowNeedleButton->SelectedStateOff();
  }
   
  else if(vtkMRMLScene::SafeDownCast(caller) == this->GetMRMLScene() && event == vtkMRMLScene::NodeAddedEvent)
  {
    vtkMRMLNode* node =  reinterpret_cast<vtkMRMLNode*> (callData);
    if(node->IsA("vtkMRMLROINode"))
    {
      ROIpresent++;
      UpdateGUI();
    }
  }
  
  else if(vtkMRMLScene::SafeDownCast(caller) == this->GetMRMLScene() && event == vtkMRMLScene::NodeRemovedEvent)
  {
    vtkMRMLNode* node =  reinterpret_cast<vtkMRMLNode*> (callData);
    if(node->IsA("vtkMRMLROINode"))
    {
      ROIpresent--;
      UpdateGUI();
    }
  }
  
  else if(started && (lastModified != this->pSourceNode->GetMTime()))
  {
    lastModified = this->pSourceNode->GetMTime(); // This prevents unnecessarily issued ImageDataModifiedEvents from beging processed 
    if((this->pSourceNode == vtkMRMLVolumeNode::SafeDownCast(caller)) && (event == vtkMRMLVolumeNode::ImageDataModifiedEvent))
    {
      clock_t begin = clock();
      
      //--------------------------------------------------------------------------------------
      // Get the image variables
      vtkImageData* pImageData  = vtkImageData::New();        
      pImageData->DeepCopy(((vtkMRMLVolumeNode*) pSourceNode)->GetImageData());
      pImageData->GetDimensions(imageDimensions);
      pImageData->GetSpacing(imageSpacing);
      pImageData->GetOrigin(imageOrigin);
      // the origin in slicer is always in the center of the imge: (Dimension*spacing = fov) / 2
      // the image received from OpenIGTLink needs to be shifted in every iteration
      double fovI = imageDimensions[0] * imageSpacing[0] / 2.0;
      double fovJ = imageDimensions[1] * imageSpacing[1] / 2.0;
      double fovK = imageDimensions[2] * imageSpacing[2] / 2.0;  
      scalarSize = pImageData->GetScalarSize();
      orientOutputImage(fovI, fovJ, fovK);
      //TODO: Do I need to get the scalarType, too?
      if(ROIpresent)
      {
        std::cout << "MRLMROINode exists" << std::endl;
        vtkCollection* collectionOfROINodes = this->GetMRMLScene()->GetNodesByName("MRMLROINode");
        int nItems = collectionOfROINodes->GetNumberOfItems();
        if(nItems > 0)
        {
          vtkMRMLROINode* pROINode = vtkMRMLROINode::SafeDownCast(collectionOfROINodes->GetItemAsObject(0));
          double center[3];
          double radius[3];
                 
          pROINode->GetRadiusXYZ(radius);
          pROINode->GetXYZ(center);
          this->pXLowerEntry->SetValueAsInt((int) ((-center[0]) - radius[0] + fovI)); // negative center point for the x-axis, because the ROIMRMLNode coordinates are in RAS (LR direction of X-axis), 
          this->pXUpperEntry->SetValueAsInt((int) ((-center[0]) + radius[0] + fovI)); // but the slicer axial and coronal view, which are used as reference, switch the direction (RL direction of X-axis)
          this->pYLowerEntry->SetValueAsInt((int) ((-center[1]) - radius[1] + fovJ)); // negative center point for the y-axis, because the ROIMRMLNode coordinates are in RAS (PA direction of Y-axis),
          this->pYUpperEntry->SetValueAsInt((int) ((-center[1]) + radius[1] + fovJ)); // but the slicer axial and sagital view, which are used as reference, switch the direction (AP direction of Y-axis)
          this->pZLowerEntry->SetValueAsInt((int) (center[2] - radius[2] + fovK));
          this->pZUpperEntry->SetValueAsInt((int) (center[2] + radius[2] + fovK));
          
//          if(needleOrigin == PATIENTINFERIOR) //TODO:!!!ATTENTION!!! This is only for testing!!!
//          {
//            this->pZLowerEntry->SetValueAsInt((int) (center[2] - radius[2] + fovJ));  // !!!ATTTENTION!!! This should be fovK!!!
//            this->pZUpperEntry->SetValueAsInt((int) (center[2] + radius[2] + fovJ));
//          }          
        }         
      }
      currentXLowerBound      = this->pXLowerEntry->GetValueAsInt();
      currentXUpperBound      = this->pXUpperEntry->GetValueAsInt();
      currentYLowerBound      = this->pYLowerEntry->GetValueAsInt();
      currentYUpperBound      = this->pYUpperEntry->GetValueAsInt();  
      currentZLowerBound      = this->pZLowerEntry->GetValueAsInt();
      currentZUpperBound      = this->pZUpperEntry->GetValueAsInt(); 
      currentXImageRegionSize = currentXUpperBound - currentXLowerBound;
      currentYImageRegionSize = currentYUpperBound - currentYLowerBound;
      currentZImageRegionSize = currentZUpperBound - currentZLowerBound; 
      
      //TODO:Do something (getMatrix?) to get the current plane of the slice for the sizing variables
      //if(axial, sagital or coronal)       
      imageRegionSize[0]  = currentXImageRegionSize;
      imageRegionLower[0] = currentXLowerBound;
      imageRegionUpper[0] = currentXUpperBound;  
      
      if((needleOrigin == PATIENTLEFT) || (needleOrigin == PATIENTPOSTERIOR))//TODO:!!!ATTENTION!!! This is only for testing!!!
      {
        imageRegionSize[1]  = currentYImageRegionSize;
        imageRegionLower[1] = currentYLowerBound;           
        imageRegionUpper[1] = currentYUpperBound;        
      }
      else if(needleOrigin == PATIENTINFERIOR)
      {
        imageRegionSize[1]  = currentZImageRegionSize;
        imageRegionLower[1] = currentZLowerBound;           
        imageRegionUpper[1] = currentZUpperBound;        
      }
            
      double points[4]; // Array that contains 2 points of the needle transform (x1,y1,x2,y2)
                        // points[0],points[1] is the point of the needle entering the image
      points[0] = 0.0;
      points[1] = 0.0;
      points[2] = 0.0;
      points[3] = 0.0;
      //------------------------------------------------------------------------------------------------
      // Crop out the imageRegion specified by the boundaries for the imageRegion
      unsigned char* pImageRegionInput = new unsigned char[imageRegionSize[0]*imageRegionSize[1]*scalarSize];
      unsigned char* pImageRegionOutput1 = new unsigned char[imageRegionSize[0]*imageRegionSize[1]*scalarSize];
      unsigned char* pImageRegionOutput2 = new unsigned char[imageRegionSize[0]*imageRegionSize[1]*scalarSize];
      unsigned char* pImageRegionOutput3 = new unsigned char[imageRegionSize[0]*imageRegionSize[1]*scalarSize]; 
      unsigned char* pImageRegionOutput4 = new unsigned char[imageRegionSize[0]*imageRegionSize[1]*scalarSize]; 
      GetImageRegion(pImageData, pImageRegionInput);
  
      //--------------------------------------------------------------------------------------------------
      // Use the ImageProcessor to alter the region of interest and calculate the needle position
      // In the ImageProcessor ITK image segmentation/processing classse are used 
      pImageProcessor->SetImage((void*) pImageRegionInput, imageRegionSize[0], imageRegionSize[1], scalarSize, imageSpacing, imageOrigin);
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/1-Input.png",INPUT);
      
      //TODO:write all images not only in tmp, but also in the output!
                 
      if(gaussVariance)
      {
        pImageProcessor->LaplacianRecursiveGaussian(gaussVariance, false, true);  //makes the line white -> no inversion needed
        pImageProcessor->LaplacianRecursiveGaussian(gaussVariance, false, false); 
        pImageProcessor->GetImage((void*) pImageRegionOutput1);
        SetImageRegion(pImageData, pImageRegionOutput1, 1);  // write the region of interest after top/left in the MRI image received from the scanner
        //TODO:DELETE pImageRegionOutput1!!        
      }
      else
        pImageProcessor->Invert(false, true);        
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/2-InvertORLaPlacianGaussian.png",TMP);
      
      pImageProcessor->DilateAndErode(true, true, this->pErodeEntry->GetValueAsInt(), this->pDilateEntry->GetValueAsInt()); // 2 == dilate value, default: 3 == erode value
      pImageProcessor->DilateAndErode(true, false, this->pErodeEntry->GetValueAsInt(), this->pDilateEntry->GetValueAsInt());
      pImageProcessor->GetImage((void*) pImageRegionOutput2);
      SetImageRegion(pImageData, pImageRegionOutput2, 2);  // write the region of interest after top/right in the MRI image received from the scanner
      //TODO:DELETE pImageRegionOutput2!!        
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/3-DilateAndErode.png",TMP);
      
      pImageProcessor->Threshold(true, true, 0, (int) needleDetectionThreshold, MAX);
      pImageProcessor->Threshold(true, false, 0, (int) needleDetectionThreshold, MAX);
      pImageProcessor->GetImage((void*) pImageRegionOutput3);
      SetImageRegion(pImageData, pImageRegionOutput3, 3);  // write the region of interest bottom/left in the MRI image received from the scanner
      //TODO:DELETE pImageRegionOutput3!!      
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/4-Threshold.png",TMP);      
      
      pImageProcessor->BinaryThinning(true, true);  // needs inverted images, because it thins to a white line
      pImageProcessor->BinaryThinning(true, false);
      pImageProcessor->GetImage((void*) pImageRegionOutput4);
      SetImageRegion(pImageData, pImageRegionOutput4, 4);  // write the region of interest bottom/right in the MRI image received from the scanner
      //TODO:DELETE pImageRegionOutput4!!  
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/5-Thinning.png",TMP);
      
//      pImageProcessor->SobelFilter(true, true, 1);     
  //    pImageProcessor->SobelFilter(true,true,1);
  //    pImageProcessor->Threshold(true,true,MAX,0,10000);
  //    pImageProcessor->SobelEdgeDetection(false, true);
  //    pImageProcessor->SobelEdgeDetection(true, false);
       //pImageProcessor->GradientMagnitude(false,true);
       //pImageProcessor->Threshold(false,true,0,0,20000);
       //pImageProcessor->Threshold(true,true,MAX,18000,MAX);
       //pImageProcessor->Threshold(true,true,MAX,0,1);
      switch (needleOrigin) 
      {
        case PATIENTLEFT: 
        {   
          pImageProcessor->HoughTransformation(true, points, needleIntensity, ENTERINGRIGHT); //Does not need inputTmpImage to be true, because without input true, the houghtransformation does not work          
          break;
        }
        case PATIENTPOSTERIOR: 
        {   
          pImageProcessor->HoughTransformation(true, points, needleIntensity, ENTERINGBOTTOM); //Does not need inputTmpImage to be true, because without input true, the houghtransformation does not work          
          break;
        }
        case PATIENTINFERIOR:
        {
          //pImageProcessor->FlipTheImageAboutTheX-Axis;
          pImageProcessor->HoughTransformation(true, points, needleIntensity, ENTERINGBOTTOM); //Does not need inputTmpImage to be true, because without input true, the houghtransformation does not work 
          break;
        }
        default:
          std::cerr << "ERROR! needleOrigin has an unsupported value!" << std::endl;
          break;
      } //end switch    
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/6-Output.png",OUTPUT);      
//      pImageProcessor->GetImage((void*) pImageRegionOutput4);
//      SetImageRegion(pImageData, pImageRegionOutput4, 4);  // write the region of interest bottom/right in the MRI image received from the scanner
//      //pImageRegionOutput->Delete();  //TODO:DELETE pImageRegion!!
      pOutputNode->SetAndObserveImageData(pImageData); //automatically removes old observer and sets modified flag, if new image is different  TODO: Does it also delete the old observer?
      pImageData->Delete();
      
      //------------------------------------------------------------------------------------------------
      // Retrieve the line from the houghtransformation
      if((points[0] == 0) && (points[1] == 0) && (points[2] == 0) && (points[3] == 0)) //If the points are still the default value something went wrong
        std::cerr << "Error! Points of line are all 0.0! No needle detected!" << std::endl;
      else // if everything is ok
      {//TODO: make this generic!! Right now I assume the needle enters from the patientleft or patientinferior                 
        points[0] += imageRegionLower[0];
        points[1] += imageRegionLower[1];
        points[2] += imageRegionLower[0];
        points[3] += imageRegionLower[1];
        std::cout << "bounds: " << imageRegionLower[0] << "|" << imageRegionLower[1] << "|" << imageRegionUpper[0] << "|" <<  imageRegionUpper[1] << std::endl;
        std::cout << "points: " << points[0] << "|" << points[1] << "|" << points[2] << "|" <<  points[3] << std::endl;
        
        double vector[2]; //vector from Tip of the needle to the end
        vector[0] = points[0] - points[2];
        vector[1] = points[1] - points[3];
        double length = sqrt(vector[0] * vector[0] + vector[1] * vector[1]);
        std::cout << "length: " << length << std::endl;
        double angle = (atan2(vector[1],vector[0]))*180/PI;  // atan2(y,x) to calculate the "angle between the two points" == angle of the vector to the X-axis
        std::cout << "angle: " << angle << std::endl;
                  
        //TODO: Take this out later on          //std::cout << atan2(-1,3)*180/PI << "|" << atan2(1,3)*180/PI << "|" << atan2(3,1)*180/PI << "|" << atan2(3,-1)*180/PI << "|" << std::endl;
        
        //-------------------------------------------------------------------------------------------
        // make the needle transform fit the line detected in the image
        double translationLR = 0;   //(X-axis)
        double translationPA = 0;   //(Y-axis)
        double translationIS = 0;   //(Z-axis)
        
        vtkTransform* transform = vtkTransform::New(); // initialized with identity matrix
        transform->Identity();
        transform->PostMultiply(); // = global coordinate system
        switch (needleOrigin) 
        {
          case PATIENTLEFT: //and axial! TODO: Take care of differences in scan plane
          {              
            transform->RotateZ(90+angle); // rotate +90 degrees to have the cylinder pointing from right to left
            translationLR = -(points[2]-fovI); // negative because positive X-axis direction in RAS-coordinates points to the patient right, but in the slicer axial and coronal view it points to the patient left 
            translationPA = -(points[3]-fovJ); // negative because positive Y-axis direction in RAS-coordinates points to the patient anterior, but in the slicer axial and sagital view it points to the patient posterior                
            break;
          }
          case PATIENTPOSTERIOR: //and axial! TODO: Take care of differences in scan plane
          {              
            transform->RotateZ(90+angle); // rotate +90 degrees to have the cylinder pointing from right to left
            translationLR = -(points[2]-fovI); // negative because positive X-axis direction in RAS-coordinates points to the patient right, but in the slicer axial and coronal view it points to the patient left 
            translationPA = -(points[3]-fovJ); // negative because positive Y-axis direction in RAS-coordinates points to the patient anterior, but in the slicer axial and sagital view it points to the patient posterior                
            break;
          }
          case PATIENTINFERIOR:
          {
            transform->RotateX(90); // rotate to have the cylinder pointing from superior to inferior
            //transform->RotateZ(-angle); TODO: get the right angle!
            translationLR = -(points[2]-fovI); // negative because positive X-axis direction in RAS-coordinates points to the patient right, but in the slicer axial and coronal view it points to the patient left 
            translationIS = points[3]-fovJ;    //TODO:!!!!ATTENTION!!!! This should be fovK, but because of the scannersimulation it is not!!!!
            break;
          }
          default:
            std::cerr << "ERROR! needleOrigin has an unsupported value!" << std::endl;
            break;
        } //end switch          
        transform->Translate(translationLR, translationPA, translationIS);    
            
        vtkMatrix4x4* transformToParentNeedle = pNeedleTransformNode->GetMatrixTransformToParent();
        transformToParentNeedle->DeepCopy(transform->GetMatrix()); // This calls the modified event
        
        transform->Translate(-translationLR, -translationPA, -translationIS);
        transform->RotateZ(90); 
        vtkMatrix4x4* transformToParentScanPlane = pScanPlaneNormalNode->GetMatrixTransformToParent();
        transformToParentScanPlane->DeepCopy(transform->GetMatrix()); // This calls the modified event
        transform->Delete();
          
//        currentXLowerBound = (int) points[2] - 20;
//        //currentYLowerBound = (int) points[3] - 10;
//        currentXUpperBound = (int) points[0];        
//        //currentYUpperBound = (int) points[1] + 10;
//        std::cout << "bounds: " << currentXLowerBound << "|" << currentYLowerBound << "|" << currentXUpperBound << "|" <<  currentYUpperBound << std::endl;
//        currentXImageRegionSize = currentXUpperBound - currentXLowerBound;
//        currentYImageRegionSize = currentYUpperBound - currentYLowerBound;       
     

      }  
      clock_t end = clock();
      cout << "               Time elapsed: " << double(diffclock(end,begin)) << " ms"<< endl;  
    }
    started++;  //I also use started as a counter of the frames -> TODO:Check if started gets bigger than int
  }
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
  {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), this->TimerInterval, this, "ProcessTimerEvents");        
  }
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::BuildGUI()
{
  this->UIPanel->AddPage("RealTimeNeedleDetection", "RealTimeNeedleDetection", NULL);

  BuildGUIForHelpFrame();
  BuildGUIForGeneralParameters();
}


void vtkRealTimeNeedleDetectionGUI::BuildGUIForHelpFrame ()
{
  const char* help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:RealTimeNeedleDetection</a> for details.";
  const char* about =
    "This module is designed and implemented by Roland Goerlitz for the Brigham and Women's Hospital.\nThis work is supported by Junichi Tokuda and Nobuhiko Hata.";
    
  vtkKWWidget* page = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  this->BuildHelpAndAboutFrame(page, help, about);
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::BuildGUIForGeneralParameters()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  
  vtkSlicerModuleCollapsibleFrame* parentFrame = vtkSlicerModuleCollapsibleFrame::New();

  parentFrame->SetParent(page);
  parentFrame->Create();
  parentFrame->SetLabelText("needle detection parameters");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               parentFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // control frame
  vtkKWFrameWithLabel* controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(parentFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText("Needle detection controls");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", controlFrame->GetWidgetName());

  // ----------------------------------------------------               
  // scanner ID
  this->pVolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->pVolumeSelector->SetNodeClass("vtkMRMLVolumeNode", NULL, NULL, NULL);
  this->pVolumeSelector->SetParent(controlFrame->GetFrame() );
  this->pVolumeSelector->Create();
  this->pVolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->pVolumeSelector->UpdateMenu();

  this->pVolumeSelector->SetBorderWidth(2);
  this->pVolumeSelector->SetLabelText( "Scanner MRMLNode: ");
  this->pVolumeSelector->SetBalloonHelpString("select an input scanner from the current MRML scene.");
  this->Script("pack %s -side top -anchor e -padx 2 -pady 2", 
                this->pVolumeSelector->GetWidgetName());
   
  // ------------------------------------------------------
  // Needle entering direction button set
  vtkKWFrame *buttonSetFrame = vtkKWFrame::New();
  buttonSetFrame->SetParent(controlFrame->GetFrame());
  buttonSetFrame->Create();
  this->Script ( "pack %s -fill both -expand true",  
                buttonSetFrame->GetWidgetName());

  vtkKWLabel *buttonSetLabel = vtkKWLabel::New();
  buttonSetLabel->SetParent(buttonSetFrame);
  buttonSetLabel->Create();
  buttonSetLabel->SetWidth(15);
  buttonSetLabel->SetText("Entering direction \n of the needle: ");

  this->pEntryPointButtonSet = vtkKWRadioButtonSet::New();
  this->pEntryPointButtonSet->SetParent(buttonSetFrame);
  this->pEntryPointButtonSet->Create();
  this->pEntryPointButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->pEntryPointButtonSet->UniformColumnsOn();
  this->pEntryPointButtonSet->UniformRowsOn();  

  this->pEntryPointButtonSet->AddWidget(PATIENTLEFT);
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->SetText("Left");
  this->pEntryPointButtonSet->AddWidget(PATIENTRIGHT);
  this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->SetText("Right");
  this->pEntryPointButtonSet->AddWidget(PATIENTPOSTERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->SetText("Posterior");
  this->pEntryPointButtonSet->AddWidget(PATIENTANTERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->SetText("Anterior");
  this->pEntryPointButtonSet->AddWidget(PATIENTINFERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->SetText("Inferior");
  this->pEntryPointButtonSet->AddWidget(PATIENTSUPERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->SetText("Superior");  
  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              buttonSetLabel->GetWidgetName() , this->pEntryPointButtonSet->GetWidgetName());
  
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->SelectedStateOn();  //default, always needs to correspond to the member variable needleOrigin
  this->pEntryPointButtonSet->EnabledOn();
  buttonSetFrame->Delete();
  
  // ------------------------------------------------------
  // Dilate and Erode Value Entry  
  vtkKWFrame* dilateErodeFrame = vtkKWFrame::New();
  dilateErodeFrame->SetParent(controlFrame->GetFrame());
  dilateErodeFrame->Create();
  this->Script ("pack %s -fill both -expand true", dilateErodeFrame->GetWidgetName());
  
  vtkKWLabel* dilateLabel = vtkKWLabel::New();
  dilateLabel->SetParent(dilateErodeFrame);
  dilateLabel->Create();
  dilateLabel->SetWidth(7);
  dilateLabel->SetText("Dilate: ");

  this->pDilateEntry = vtkKWEntry::New();
  this->pDilateEntry->SetParent(dilateErodeFrame);
  this->pDilateEntry->Create();
  this->pDilateEntry->SetWidth(7);
  this->pDilateEntry->SetValueAsInt(DEFAULTDILATE);
  
  vtkKWLabel* erodeLabel = vtkKWLabel::New();
  erodeLabel->SetParent(dilateErodeFrame);
  erodeLabel->Create();
  erodeLabel->SetWidth(7);
  erodeLabel->SetText("Erode: ");
              
  this->pErodeEntry = vtkKWEntry::New();
  this->pErodeEntry->SetParent(dilateErodeFrame);
  this->pErodeEntry->Create();
  this->pErodeEntry->SetWidth(7);
  this->pErodeEntry->SetValueAsInt(DEFAULTERODE);
  
  this->Script("pack %s %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              dilateLabel->GetWidgetName(), this->pDilateEntry->GetWidgetName(), erodeLabel->GetWidgetName(), this->pErodeEntry->GetWidgetName());
  
  dilateLabel->Delete();  
  erodeLabel->Delete();
  dilateErodeFrame->Delete(); 
   
  // ------------------------------------------------------
  // threshold slider button  
  vtkKWFrame* sliderFrame = vtkKWFrame::New();
  sliderFrame->SetParent(controlFrame->GetFrame());
  sliderFrame->Create();
  this->Script("pack %s -fill both -expand true", sliderFrame->GetWidgetName());
  
  this->pThresholdScale = vtkKWScaleWithEntry::New();
  this->pThresholdScale->SetParent(sliderFrame);
  this->pThresholdScale->SetLabelText("Threshold");
  this->pThresholdScale->Create();
  this->pThresholdScale->GetScale()->SetLength(180); 
  this->pThresholdScale->SetRange(0,MAX);
  this->pThresholdScale->SetResolution(10);
  //TODO:Steve can I constrict the values to integer?  -> floor?
  this->pThresholdScale->SetValue(DEFAULTTHRESHOLD);  
  this->Script("pack %s -side left -padx 2 -pady 2", this->pThresholdScale->GetWidgetName());  
  
  sliderFrame->Delete(); 
  
  // ------------------------------------------------------
  // intensity slider button  
  vtkKWFrame* sliderFrame2 = vtkKWFrame::New();
  sliderFrame2->SetParent(controlFrame->GetFrame());
  sliderFrame2->Create();
  this->Script("pack %s -fill both -expand true", sliderFrame2->GetWidgetName());
  
  this->pIntensityScale = vtkKWScaleWithEntry::New();
  this->pIntensityScale->SetParent(sliderFrame2);
  this->pIntensityScale->SetLabelText("Needle Intensity");
  this->pIntensityScale->Create();
  this->pIntensityScale->GetScale()->SetLength(180); 
  this->pIntensityScale->SetRange(0,MAX);
  this->pIntensityScale->SetResolution(10);
  //TODO:Steve can I constrict the values to integer?  -> floor?
  this->pIntensityScale->SetValue(DEFAULTINTENSITY);  
  this->Script("pack %s -side left -padx 2 -pady 2", this->pIntensityScale->GetWidgetName());  
  
  sliderFrame2->Delete();
  
  // ------------------------------------------------------
  // gauss variance slider button  
  vtkKWFrame* sliderFrame3 = vtkKWFrame::New();
  sliderFrame3->SetParent(controlFrame->GetFrame());
  sliderFrame3->Create();
  this->Script("pack %s -fill both -expand true", sliderFrame3->GetWidgetName());
  
  this->pGaussScale = vtkKWScaleWithEntry::New();
  this->pGaussScale->SetParent(sliderFrame3);
  this->pGaussScale->SetLabelText("Gaussian Variance");
  this->pGaussScale->Create();
  this->pGaussScale->GetScale()->SetLength(180); 
  this->pGaussScale->SetRange(0,10);
  this->pGaussScale->SetResolution(1);
  //TODO:Steve can I constrict the values to integer?  -> floor?
  this->pGaussScale->SetValue(0);  
  this->Script("pack %s -side left -padx 2 -pady 2", this->pGaussScale->GetWidgetName());  
  
  sliderFrame3->Delete();  
  
  // -----------------------------------------
  // push buttons
  vtkKWFrame* buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(controlFrame->GetFrame());
  buttonFrame->Create();
  this->Script("pack %s -fill both -expand true", buttonFrame->GetWidgetName());
                
  this->pStartButton = vtkKWPushButton::New();
  this->pStartButton->SetParent(buttonFrame);
  this->pStartButton->Create();
  this->pStartButton->SetText("Start");
  this->pStartButton->SetWidth(15);

  this->pStopButton = vtkKWPushButton::New();
  this->pStopButton->SetParent(buttonFrame);
  this->pStopButton->Create();
  this->pStopButton->SetText("Stop");
  this->pStopButton->SetWidth(15);
  this->pStopButton->EnabledOff();
  
  this->pShowNeedleButton = vtkKWCheckButton::New();
  this->pShowNeedleButton->SetParent(buttonFrame);
  this->pShowNeedleButton->Create();
  this->pShowNeedleButton->SelectedStateOff();
  this->pShowNeedleButton->SetText("Show Needle");
  this->pShowNeedleButton->EnabledOff();

  this->Script("pack %s %s %s %s -side left -padx 2 -pady 2", 
               this->pStartButton->GetWidgetName(),
               this->pStopButton->GetWidgetName(),
               this->pShowNeedleButton->GetWidgetName(),
               this->pShowNeedleButton->GetWidgetName());                          
  
  buttonFrame->Delete(); 
   
  controlFrame->Delete();  
  
  // -----------------------------------------
  // Coordinates frame
  vtkKWFrameWithLabel* coordinatesFrame = vtkKWFrameWithLabel::New();
  coordinatesFrame->SetParent(parentFrame->GetFrame());
  coordinatesFrame->Create();
  coordinatesFrame->SetLabelText("Coordinates");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", coordinatesFrame->GetWidgetName() );

  // -----------------------------------------
  // Boundary edit fields TODO: Check for bounds! 0 up to imageDimensions is possible
  vtkKWFrame* descriptionFrame = vtkKWFrame::New();
  descriptionFrame->SetParent(coordinatesFrame->GetFrame());
  descriptionFrame->Create();
  this->Script ( "pack %s -fill both -expand true", descriptionFrame->GetWidgetName());
  vtkKWTextWithHyperlinksWithScrollbars* descriptionText = vtkKWTextWithHyperlinksWithScrollbars::New();  //TODO:Steve what should I use, if I only need text?
  descriptionText->SetParent(descriptionFrame);
  descriptionText->Create(); 
  descriptionText->SetHorizontalScrollbarVisibility(0);
  descriptionText->SetVerticalScrollbarVisibility(0);
  descriptionText->GetWidget()->SetReliefToFlat();
  descriptionText->GetWidget()->SetWrapToWord();
  descriptionText->GetWidget()->QuickFormattingOn();
  descriptionText->SetText("Set the boundaries for the part of the image where the needle detection will start. They can be set manually or with an MRMLROINode. If a MRLMROINode exists, all the following values will be overriden." );
  //Important that Read only after SetText otherwise it doesn't work
  descriptionText->GetWidget()->ReadOnlyOn();
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4", descriptionText->GetWidgetName());
  descriptionText->Delete();
  descriptionFrame->Delete();    
    
  vtkKWFrame* xFrame = vtkKWFrame::New();
  xFrame->SetParent(coordinatesFrame->GetFrame());
  xFrame->Create();
  this->Script ( "pack %s -fill both -expand true", xFrame->GetWidgetName());
  vtkKWLabel* xLabel = vtkKWLabel::New();
  xLabel->SetParent(xFrame);
  xLabel->Create();
  xLabel->SetWidth(25);
  xLabel->SetText("Boundaries in LR-direction: ");

  this->pXLowerEntry = vtkKWEntry::New();
  this->pXLowerEntry->SetParent(xFrame);
  this->pXLowerEntry->Create();
  this->pXLowerEntry->SetWidth(7);
  this->pXLowerEntry->SetValueAsInt(100);
  //this->pXLowerEntry->SetValueAsInt(135);
              
  this->pXUpperEntry = vtkKWEntry::New();
  this->pXUpperEntry->SetParent(xFrame);
  this->pXUpperEntry->Create();
  this->pXUpperEntry->SetWidth(7);
  this->pXUpperEntry->SetValueAsInt(180);
  //this->pXUpperEntry->SetValueAsInt(155);

  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              xLabel->GetWidgetName(), this->pXLowerEntry->GetWidgetName(), this->pXUpperEntry->GetWidgetName());
  xLabel->Delete();
  xFrame->Delete();
  
  vtkKWFrame* yFrame = vtkKWFrame::New();
  yFrame->SetParent(coordinatesFrame->GetFrame());
  yFrame->Create();
  this->Script ("pack %s -fill both -expand true", yFrame->GetWidgetName());
  vtkKWLabel* yLabel = vtkKWLabel::New();
  yLabel->SetParent(yFrame);
  yLabel->Create();
  yLabel->SetWidth(25);
  yLabel->SetText("Boundaries in PA-direction: ");

  this->pYLowerEntry = vtkKWEntry::New();
  this->pYLowerEntry->SetParent(yFrame);
  this->pYLowerEntry->Create();
  this->pYLowerEntry->SetWidth(7);
  this->pYLowerEntry->SetValueAsInt(125);
              
  this->pYUpperEntry = vtkKWEntry::New();
  this->pYUpperEntry->SetParent(yFrame);
  this->pYUpperEntry->Create();
  this->pYUpperEntry->SetWidth(7);
  this->pYUpperEntry->SetValueAsInt(145);

  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              yLabel->GetWidgetName(), this->pYLowerEntry->GetWidgetName(), this->pYUpperEntry->GetWidgetName());
  yLabel->Delete();
  yFrame->Delete();
  
  vtkKWFrame* zFrame = vtkKWFrame::New();
  zFrame->SetParent(coordinatesFrame->GetFrame());
  zFrame->Create();
  this->Script ("pack %s -fill both -expand true", zFrame->GetWidgetName());
  vtkKWLabel* zLabel = vtkKWLabel::New();
  zLabel->SetParent(zFrame);
  zLabel->Create();
  zLabel->SetWidth(25);
  zLabel->SetText("Boundaries in IS-direction: ");

  this->pZLowerEntry = vtkKWEntry::New();
  this->pZLowerEntry->SetParent(zFrame);
  this->pZLowerEntry->Create();
  this->pZLowerEntry->SetWidth(7);
  this->pZLowerEntry->SetValueAsInt(50);
              
  this->pZUpperEntry = vtkKWEntry::New();
  this->pZUpperEntry->SetParent(zFrame);
  this->pZUpperEntry->Create();
  this->pZUpperEntry->SetWidth(7);
  this->pZUpperEntry->SetValueAsInt(120);

  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              zLabel->GetWidgetName(), this->pZLowerEntry->GetWidgetName(), this->pZUpperEntry->GetWidgetName());
  zLabel->Delete();
  zFrame->Delete();
  
  coordinatesFrame->Delete();
  
  parentFrame->Delete();
  UpdateGUI();
}


//----------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::UpdateAll()
{
  UpdateGUI();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::UpdateGUI()
{
  if(started)
  {
    this->pVolumeSelector->EnabledOff();
    this->pEntryPointButtonSet->EnabledOff();
    this->pStartButton->EnabledOff();
    this->pStopButton->EnabledOn();
    this->pShowNeedleButton->EnabledOn();  // this Button is disabled until the needle detection was started for the first time  TODO: I should change it to being disabled until the needlemodel was created
    this->pXLowerEntry->EnabledOff();
    this->pXUpperEntry->EnabledOff();
    this->pYLowerEntry->EnabledOff();
    this->pYUpperEntry->EnabledOff();
    this->pZLowerEntry->EnabledOff();
    this->pZUpperEntry->EnabledOff();
  }
  else if(started == 0)
  {
    this->pVolumeSelector->EnabledOn();
    this->pEntryPointButtonSet->EnabledOn();
    this->pStartButton->EnabledOn();
    this->pStopButton->EnabledOff();  
    if(ROIpresent)
    {
      this->pXLowerEntry->EnabledOff();
      this->pXUpperEntry->EnabledOff();
      this->pYLowerEntry->EnabledOff();
      this->pYUpperEntry->EnabledOff();
      this->pZLowerEntry->EnabledOff();
      this->pZUpperEntry->EnabledOff();
    }
    else 
    {
      this->pXLowerEntry->EnabledOn();
      this->pXUpperEntry->EnabledOn();
      this->pYLowerEntry->EnabledOn();
      this->pYUpperEntry->EnabledOn();
      this->pZLowerEntry->EnabledOn();
      this->pZUpperEntry->EnabledOn();
    }
  }
}

// Function to evoke changes in the image in every itaration | not used anymore
void vtkRealTimeNeedleDetectionGUI::ProcessImage(vtkImageData* pImageData, int i)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer();
  for (int j = 50000; j< 50000+i*500; j++)
    if((int) pImage[j] != 0)
      pImage[j] = 0;    
  std::cout << "Image processed" << std::endl;      
}

//-----------------------------------------------------------------------------
// hard copy the region of interest of the original image byte by byte
void vtkRealTimeNeedleDetectionGUI::GetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer(); 
  int j = 0;
  for(long i = 0; i < (imageDimensions[0] * imageDimensions[1] * scalarSize) ; i++)
  {
    if((i >= imageRegionLower[1]*imageDimensions[1]*scalarSize) && (i < imageRegionUpper[1]*imageDimensions[1]*scalarSize))  // Y-axis restrictions
    {
      if((imageRegionLower[0]*scalarSize <= (i%(imageDimensions[0]*scalarSize))) && (imageRegionUpper[0]*scalarSize >  (i%(imageDimensions[0]*scalarSize))))
      {            
        pImageRegion[j] = pImage[i];
        j++;
      }
    }
  }
}

//--------------------------------------------------------------------------
// hard copy the region of interest back to the imageRegion byte by byte
// put it in the original image according to position (1:top/left, 2:top/right, 3:bottom/left, 4:bottom/right)
void vtkRealTimeNeedleDetectionGUI::SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion, int position)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer();
  int positionInMessageImage = 0;
  for(long i = 0; i <= imageRegionSize[0] * imageRegionSize[1] * scalarSize; i++)
  {
    if(position == 1 || position == 3) //the position of the region of interest is to the left
      positionInMessageImage = 5*scalarSize + (i%(imageRegionSize[0]*scalarSize)) + (i/(imageRegionSize[0]*scalarSize))*imageDimensions[0]*scalarSize;
    else //the position of the region of interest is to the right
      positionInMessageImage = (imageDimensions[0]-5-imageRegionSize[0])*scalarSize + (i%(imageRegionSize[0]*scalarSize)) + (i/(imageRegionSize[0]*scalarSize))*imageDimensions[0]*scalarSize;    
    //old command to get the exact position on the x-axis in the image:  positionInMessageImage = imageRegionLower[0]*scalarSize + (i%(imageRegionSize[0]*scalarSize)) + (i/(imageRegionSize[0]*scalarSize))*imageDimensions[0]*scalarSize;
      
    if(position == 1 || position == 2) //the position of the region of interest is above the original image
      pImage[positionInMessageImage+5*imageDimensions[0]*scalarSize] = pImageRegion[i];      
    else //the position of the region of interest is below the original image
      pImage[positionInMessageImage+(imageDimensions[1]-imageRegionSize[1]-5)*imageDimensions[0]*scalarSize] = pImageRegion[i];
  }    
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::MakeNeedleModel()
{  
  int heightOfNeedle = 80;
  vtkMRMLModelDisplayNode* pNeedleModelDisplay = vtkMRMLModelDisplayNode::New();
  GetMRMLScene()->AddNode(pNeedleModelDisplay);
  pNeedleModelDisplay->SetScene(this->GetMRMLScene());
  pNeedleModelNode->SetAndObserveDisplayNodeID(pNeedleModelDisplay->GetID());
  
  // Cylinder represents the needle stick
  vtkCylinderSource* pCylinder = vtkCylinderSource::New();
  pCylinder = vtkCylinderSource::New();
  pCylinder->SetRadius(1);
  pCylinder->SetHeight(heightOfNeedle);
  pCylinder->SetCenter(0, 0, 0);
  pCylinder->Update();

  // Translate cylinder to have the end at the tip of the needle
  vtkTransformPolyDataFilter *tFilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans = vtkTransform::New();
  trans->Translate(0.0, heightOfNeedle/2, 0.0);
  trans->Update();
  tFilter->SetInput(pCylinder->GetOutput());
  tFilter->SetTransform(trans);
  tFilter->Update();
  
  // Sphere represents the needle tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  apd->AddInput(tFilter->GetOutput());
  apd->Update();
  
  pNeedleModelNode->SetAndObservePolyData(apd->GetOutput());
  
  double color[3];
  color[0] = 0.0; // R
  color[1] = 1.0; // G
  color[2] = 0.0; // B
  pNeedleModelDisplay->SetPolyData(pNeedleModelNode->GetPolyData());
  pNeedleModelDisplay->SetColor(color);
  
  trans->Delete();
  tFilter->Delete();
  sphere->Delete();
  pCylinder->Delete();
  apd->Delete();
  
  pNeedleModelDisplay->Delete();
  std::cout << "made NeedleModel" << std::endl;
}


void vtkRealTimeNeedleDetectionGUI::orientOutputImage(double fovI, double fovJ, double fovK)
{
  vtkMatrix4x4* matrix = vtkMatrix4x4::New(); 
  //identity
    matrix->Element[0][0] = 1.0;
    matrix->Element[1][0] = 0.0;
    matrix->Element[2][0] = 0.0;
    matrix->Element[0][1] = 0.0;
    matrix->Element[1][1] = 1.0;
    matrix->Element[2][1] = 0.0;
    matrix->Element[0][2] = 0.0;
    matrix->Element[1][2] = 0.0;
    matrix->Element[2][2] = 1.0;
    matrix->Element[0][3] = fovI;
    matrix->Element[1][3] = fovJ;
    matrix->Element[2][3] = 0.0;
  
  //axial
  //  matrix->Element[0][0] = -1.0;
  //  matrix->Element[1][0] = 0.0;
  //  matrix->Element[2][0] = 0.0;
  //  matrix->Element[0][1] = 0.0;
  //  matrix->Element[1][1] = -1.0;
  //  matrix->Element[2][1] = 0.0;
  //  matrix->Element[0][2] = 0.0;
  //  matrix->Element[1][2] = 0.0;
  //  matrix->Element[2][2] = 1.0;
  //  matrix->Element[0][3] = fovI;
  //  matrix->Element[1][3] = fovJ;
  //  matrix->Element[2][3] = 0.0;
  
  //coronal probably wrong
  //  matrix->Element[0][0] = -1.0;
  //  matrix->Element[1][0] = 0.0;
  //  matrix->Element[2][0] = 0.0;
  //  matrix->Element[0][1] = 0.0;
  //  matrix->Element[1][1] = 0.0;
  //  matrix->Element[2][1] = -1.0;
  //  matrix->Element[0][2] = 0.0;
  //  matrix->Element[1][2] = -1.0;
  //  matrix->Element[2][2] = 0.0;
  //  matrix->Element[0][3] = fovI;
  //  matrix->Element[1][3] = 0.0;
  //  matrix->Element[2][3] = -fovK;
  
  //sagital probably wrong
  //  matrix->Element[0][0] = 0.0;
  //  matrix->Element[1][0] = 1.0;
  //  matrix->Element[2][0] = 0.0;
  //  matrix->Element[0][1] = 0.0;
  //  matrix->Element[1][1] = 0.0;
  //  matrix->Element[2][1] = -1.0;
  //  matrix->Element[0][2] = -1.0;
  //  matrix->Element[1][2] = 0.0;
  //  matrix->Element[2][2] = 0.0;
  //  matrix->Element[0][3] = 0.0;
  //  matrix->Element[1][3] = fovJ;
  //  matrix->Element[2][3] = -fovK;
  
  
  //!!ATTENTION!!! Take this out when really testing on the scanner
  pOutputNode->SetRASToIJKMatrix(matrix); // TODO: make this matrix generic! right now it is adapted to the scanner simulation
  matrix->Delete();                       // this matrix should not be needed for real images, because the header information should be correct
}



//TODO: take that out when done measuring
double vtkRealTimeNeedleDetectionGUI::diffclock(clock_t clock1,clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
} 






