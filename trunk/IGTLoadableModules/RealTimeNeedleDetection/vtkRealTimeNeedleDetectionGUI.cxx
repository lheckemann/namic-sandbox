/*==========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
  Author: Roland Goerlitz, University of Karlsruhe

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: ??$
  Date:      $Date: 2009/02/12 19:10:09$
  Version:   $Revision: 1.00$

==========================================================================*/


//TODO: maybe remove the DataManager
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

#include "vtkCylinderSource.h"
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

#include "vtkCornerAnnotation.h"

#define PI 3.1415926535897932384626433832795 
//TODO: how do I access the math pi?


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
  this->DataManager = vtkIGTDataManager::New();  //TODO: Do I need the DataManager?
  started    = 0;
  showNeedle = 0;
  
  //----------------------------------------------------------------
  // GUI widgets
  this->pStartButton      = NULL;
  this->pStopButton       = NULL;
  this->pShowNeedleButton = NULL;
  this->pScannerIDEntry   = NULL;
  this->pXLowerEntry      = NULL;
  this->pXUpperEntry      = NULL;
  this->pYLowerEntry      = NULL;
  this->pYUpperEntry      = NULL;
  
  //--------------------------------------------------------------------
  // MRML nodes  
  this->pVolumeNode    = NULL;
  this->pSourceNode    = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)   
  this->TimerFlag = 0;
  
  //----------------------------------------------------------------
  // Image Values - default  
  currentXLowerBound = initialXLowerBound = 0;
  currentXUpperBound = initialXUpperBound = 256;
  currentYLowerBound = initialYLowerBound = 0;
  currentYUpperBound = initialYUpperBound = 256;
  currentXImageRegionSize                 = 256;
  currentYImageRegionSize                 = 256;
  imageDimensions[0]    = 256;
  imageDimensions[1]    = 256;
  imageDimensions[2]    = 1;
  imageSpacing[0]       = 1;  
  imageSpacing[1]       = 1;
  imageSpacing[2]       = 1;
  imageOrigin[0]        = 0;
  imageOrigin[1]        = 0;
  imageOrigin[2]        = 0;
  scalarSize            = 2;                      // scalarType 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  lastModified          = 0;
  pImage                = NULL;
  pImageProcessor       = new ImageProcessor::ImageProcessor();  //TODO:move the new to starting the whole detection
  std::cout << "NeedleDetection constructed" << std::endl;
}

//---------------------------------------------------------------------------
// Destructor
vtkRealTimeNeedleDetectionGUI::~vtkRealTimeNeedleDetectionGUI()
{
  std::cout << "NeedleDetection destruction started" << std::endl;
  if (this->DataManager)
  {
    // If we don't set the scene to NULL for DataManager,
    // Slicer will report a lot leak when it is closed.
    //TODO: Do I need this DataManager?
    this->DataManager->SetMRMLScene(NULL);
    this->DataManager->Delete();
  }

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
  if (this->pShowNeedleButton)
  {
    this->pShowNeedleButton->SetParent(NULL );
    this->pShowNeedleButton->Delete ( );
  }
  if (this->pScannerIDEntry)
  {
    this->pScannerIDEntry->SetParent(NULL);
    this->pScannerIDEntry->Delete();
  }
  if (this->pXLowerEntry)
  {
    this->pXLowerEntry->SetParent(NULL);
    this->pXLowerEntry->Delete();
  }
  if (this->pXUpperEntry)
  {
    this->pXUpperEntry->SetParent(NULL);
    this->pXUpperEntry->Delete();
  }
  if (this->pYLowerEntry)
  {
    this->pYLowerEntry->SetParent(NULL);
    this->pYLowerEntry->Delete();
  }
  if (this->pYUpperEntry)
  {
    this->pYUpperEntry->SetParent(NULL);
    this->pYUpperEntry->Delete();
  }
    
  //-----------------------------------------------------------------
  // Remove MRML nodes
  if(this->pVolumeNode)
    this->pVolumeNode->Delete();
  if(this->pSourceNode)
    this->pSourceNode->Delete();

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
  this->DataManager->SetMRMLScene(this->GetMRMLScene());
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
void vtkRealTimeNeedleDetectionGUI::RemoveGUIObservers ( )
{
  //----------------------------------------------------------------
  // MRML Observers
  this->MRMLObserverManager->RemoveObjectEvents(pVolumeNode);
  //TODO: Remove Observers of MRMLSceneEvents
  
  //----------------------------------------------------------------
  // GUI Observers
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if(this->pStartButton)
    this->pStartButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if(this->pStopButton)
    this->pStopButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if (this->pShowNeedleButton)
    this->pShowNeedleButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    
  this->RemoveLogicObservers();
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
  
  // register the volume node in the event observer  
  vtkMRMLNode *node = NULL; // TODO: is this OK?
  vtkIntArray* nodeEvents = vtkIntArray::New();
  nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
  vtkSetAndObserveMRMLNodeEventsMacro(node,pVolumeNode,nodeEvents);  //TODO: What does this "node" do???
  nodeEvents->Delete();

  //----------------------------------------------------------------
  // GUI Observers
  this->pStartButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pStopButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pShowNeedleButton
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();
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
void vtkRealTimeNeedleDetectionGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkRealTimeNeedleDetectionLogic::StatusUpdateEvent, (vtkCommand*) this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData)
{

  const char* eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)  // This is not used yet, since only mouse clicks on buttons are processed
  {
    vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
  }
  
  else if (this->pStartButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {
    std::cout << "StartButton is pressed." << std::endl;
       
    //-----------------------------------------------------------------------------------------------------------------
    // Set the sourceNode and register it to the event observer | it gets unregistered when the StopButton is pressed
    pSourceNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->pScannerIDEntry->GetValue()));
    vtkMRMLNode* node = NULL; // TODO: is this OK?
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
    vtkSetAndObserveMRMLNodeEventsMacro(node,pSourceNode,nodeEvents);
    nodeEvents->Delete();     
      
    if(pSourceNode)
    { //-----------------------------------------------------------------------------------------------------------------
      // Create the volumeNode and displayNode, which display the detected needle   
      std::cout << "pSourceNode exists" << std::endl;   
      vtkCollection* collectionOfVolumeNodes = this->GetMRMLScene()->GetNodesByName("VolumeNode");
      int nItems = collectionOfVolumeNodes->GetNumberOfItems();
      if(nItems == 0) // pVolumeNode does not exist yet
      {
        std::cout << "items == 0" << std::endl; 
        vtkMRMLScalarVolumeNode* pScalarNode = vtkMRMLScalarVolumeNode::New();
        pScalarNode->SetLabelMap(0);   // set the label map to grey scale
        pVolumeNode = pScalarNode;    
        if(pVolumeNode == NULL)
          std::cout << "ERROR! VolumeNode == NULL!" << std::endl;
        pVolumeNode->UpdateID("VolumeNode");
        pVolumeNode->SetName("VolumeNode");
        pVolumeNode->SetDescription("MRMLNode that displays the tracked needle and the original image");
        pVolumeNode->SetScene(this->GetMRMLScene()); 
        vtkMatrix4x4* matrix = vtkMatrix4x4::New();
        matrix->Element[0][0] = -1.0;
        matrix->Element[1][0] = 0.0;
        matrix->Element[2][0] = 0.0;
        matrix->Element[0][1] = 0.0;
        matrix->Element[1][1] = 1.0;
        matrix->Element[2][1] = 0.0;
        matrix->Element[0][2] = 0.0;
        matrix->Element[1][2] = 0.0;
        matrix->Element[2][2] = -1.0;
        matrix->Element[0][3] = 0.0;
        matrix->Element[0][3] = 0.0;
        matrix->Element[0][3] = 0.0;
        pVolumeNode->SetRASToIJKMatrix(matrix); // TODO: take this out later, used for the scanner simulation
        
        vtkMRMLScalarVolumeDisplayNode* pScalarDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();
        pScalarDisplayNode->SetDefaultColorMap();   
        vtkMRMLScalarVolumeDisplayNode* pDisplayNode = pScalarDisplayNode;         
        this->GetMRMLScene()->AddNode(pDisplayNode);   
        pVolumeNode->SetAndObserveDisplayNodeID(pDisplayNode->GetID());        
        this->GetMRMLScene()->AddNode(pVolumeNode);
        pScalarNode->Delete();
        pScalarDisplayNode->Delete();
        pDisplayNode->Delete();
      }
      else if(nItems >= 1)
      {
        std::cerr << "VolumeNode exists already. Starting again" << std::endl;
        pVolumeNode = vtkMRMLVolumeNode::SafeDownCast(collectionOfVolumeNodes->GetItemAsObject(0));
      }
      else
      {
        std::cerr << "ERROR! More than one VolumeNode" << std::endl; // code should never get here!
        return;
      }
      //--------------------------------------------------------------------------------------
      // Get the image variables
      vtkImageData* pImageData = ((vtkMRMLVolumeNode*) pSourceNode)->GetImageData();
      std::cerr << "got ImageData" << std::endl;
      pImageData->GetDimensions(imageDimensions);
      pImageData->GetSpacing(imageSpacing);
      pImageData->GetOrigin(imageOrigin);
      scalarSize = pImageData->GetScalarSize();
      //TODO: Do I need to get the scalarType, too?
      currentXLowerBound = initialXLowerBound = this->pXLowerEntry->GetValueAsInt();
      currentXUpperBound = initialXUpperBound = this->pXUpperEntry->GetValueAsInt();
      currentYLowerBound = initialYLowerBound = this->pYLowerEntry->GetValueAsInt();
      currentYUpperBound = initialYUpperBound = this->pYUpperEntry->GetValueAsInt();  
      currentXImageRegionSize                 = currentXUpperBound - currentXLowerBound;
      currentYImageRegionSize                 = currentYUpperBound - currentYLowerBound;  
      started = 1; // start checking for changes in pSourceNode to update pVolumeNode     
      std::cerr << "Start checking for changes" << std::endl;
    }
    else // no Scanner node found in MRMLScene
      std::cerr << "ERROR! No Scanner detected. RealTimeNeedleDetection did not start." << std::endl;
  }
  
  else if (this->pStopButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {    
    std::cerr << "StopButton is pressed." << std::endl;
    // unregister the observer of the ScannerMRMLNode
    if(this->pSourceNode)
    {
      this->MRMLObserverManager->RemoveObjectEvents(pSourceNode);
      this->pSourceNode = NULL;  
    }
    
    // Set the VolumeNode to NULL, because it should not get used while started==0
    // It will not get deleted, because it is still referenced in the MRMLScene
    if(this->pVolumeNode)
    {
      //this->GetMRMLScene()->RemoveNodeNoNotify((vtkMRMLNode*) pVolumeNode);
      this->pVolumeNode = NULL; 
    }
    //Don't set the Transform to NULL, because should be able to access it without the detection running
    
    started = 0; // Stop checking MRLM events of pSourceNode 
  }
  
  else if (this->pShowNeedleButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent )
  {
    std::cout << "ShowNeedleButton is pressed." << std::endl;
    showNeedle = this->pShowNeedleButton->GetSelectedState(); 
    if(!showNeedle)
    {
      vtkMRMLModelNode*   pNeedleModel;
      vtkMRMLDisplayNode* pNeedleDisplay;
      vtkCollection* collection = this->GetMRMLScene()->GetNodesByName("NeedleModel");
      if (collection != NULL && collection->GetNumberOfItems() == 0)
      {
        // if a node doesn't exist, do nothing
      }
      else // if a node exists, set visibility to 0
      {
        pNeedleModel = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
        pNeedleDisplay = pNeedleModel->GetDisplayNode();
        pNeedleDisplay->SetVisibility(0);
      }
    }
  }
      
  UpdateGUI(); // enable or disable options for the user
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
  
  if (event == vtkMRMLScene::SceneCloseEvent) //TODO: Do I actually catch this evnent?
  {
    if (this->pShowNeedleButton != NULL && this->pShowNeedleButton->GetSelectedState())
      this->pShowNeedleButton->SelectedStateOff();
  }
  
  else if(started && (lastModified != this->pSourceNode->GetMTime()))
  {
    lastModified = this->pSourceNode->GetMTime(); // This prevents unnecessarily issued ImageDataModifiedEvents from beging processed 
    std::cout << started << ":MRMLEvent processing while started";
    if((this->pSourceNode == vtkMRMLVolumeNode::SafeDownCast(caller)) && (event == vtkMRMLVolumeNode::ImageDataModifiedEvent))
    {
      std::cout << "-SourceNode Event" << std::endl;
      clock_t begin = clock();
      double points[4]; // Array that contains 2 points of the needle transform (x1,y1,x2,y2)
                        // The points[0],points[1] is the point of the needle entering the image
      points[0] = 0.0;
      points[1] = 0.0;
      points[2] = 0.0;
      points[3] = 0.0;
      //------------------------------------------------------------------------------------------------
      // Crop out the imageRegion specified by the X- and Y-boundaries
      vtkImageData* pImageData  = vtkImageData::New();        
      pImageData->DeepCopy(((vtkMRMLVolumeNode*) pSourceNode)->GetImageData());  
      unsigned char* pImageRegion = new unsigned char[currentXImageRegionSize*currentYImageRegionSize*scalarSize]; 
      GetImageRegion(pImageData, pImageRegion);
  
      //--------------------------------------------------------------------------------------------------
      // Use the ImageProcessor to alter the region of interest and calculate the needle position
      // In the ImageProcessor ITK image segmentation/processing classse are used 
      pImageProcessor->SetImage((void*) pImageRegion, currentXImageRegionSize, currentYImageRegionSize, scalarSize, imageSpacing, imageOrigin);
      pImageProcessor->PassOn();
      //pImageProcessor->LaplacianRecursiveGaussian(false,false);
      pImageProcessor->Threshold(false, true, MAX, 0, 15000);
  //    pImageProcessor->SobelFilter(true,true,1);
  //    pImageProcessor->Threshold(true,true,MAX,0,10000);
  //    pImageProcessor->SobelEdgeDetection(false, true);
  //    pImageProcessor->SobelEdgeDetection(true, false);
       //pImageProcessor->GradientMagnitude(false,true);
       //pImageProcessor->Threshold(false,true,0,0,20000);
       //pImageProcessor->Threshold(true,true,MAX,18000,MAX);
       //pImageProcessor->Threshold(true,true,MAX,0,1);
       //pImageProcessor->Write("/projects/mrrobot/goerlitz/test/threshold.png",3);
      pImageProcessor->HoughTransformation(true, points);    
       //pImageProcessor->CannyEdgeDetection(true,false);
             //pImageProcessor->Write("/projects/mrrobot/goerlitz/test/input.png",1);         
           //  pImageProcessor->Write("/projects/mrrobot/goerlitz/test/output.png",4);
      std::cout << "ImageRegion processed" << std::endl;    
      pImageProcessor->GetImage((void*) pImageRegion);
      SetImageRegion(pImageData, pImageRegion);
      //pImageRegion->Delete();  //TODO:DELETE!!
      pVolumeNode->SetAndObserveImageData(pImageData); //automatically removes old observer and sets modified flag, if new image is different  TODO: Does it also delete the old observer?
      pImageData->Delete();
      
      //------------------------------------------------------------------------------------------------
      // Retrieve the needle position and adjust the X- and Y-boundaries
      //TODO: adjust
      if((points[0] == 0) && (points[1] == 0) && (points[2] == 0) && (points[3] == 0))
        std::cerr << "Error! Points of line are all 0.0! No needle detected!" << std::endl;
      else // if everything is ok
      {//TODO: make this generic!! Right now I assume the needle enters from the right
        //switch(needleEnteringDirection)
        std::cout << "bounds: " << currentXLowerBound << "|" << currentYLowerBound << "|" << currentXUpperBound << "|" <<  currentYUpperBound << std::endl;
        std::cout << "points: " << points[0] << "|" << points[1] << "|" << points[2] << "|" <<  points[3] << std::endl;
        
        points[0] += currentXLowerBound;
        points[1] += currentYLowerBound;
        points[2] += currentXLowerBound;
        points[3] += currentYLowerBound;
        
        double vector[2];
        vector[0] = points[2] - points[0];
        vector[1] = points[3] - points[1];
        double length = sqrt(vector[0] * vector[0] + vector[1] * vector[1]);
        std::cout << "length: " << length << std::endl;
        double angle = (atan2(points[1]-points[3],points[0]-points[2]))*180/PI;
        std::cout << "angle: " << angle << std::endl;
        
        //-------------------------------------------------------------------------------------------
        // make the needle transform fit the line detected in the image
        // the origin of the transform is always in the center of the imge: (Dimension*spacing = fov) / 2
        double fovI = imageDimensions[0] * imageSpacing[0] / 2.0;
        double fovJ = imageDimensions[1] * imageSpacing[1] / 2.0;        
        // do not need fovK because the images are 2D only
        double translationLR = -(points[0]-fovI); //(X-axis)
        double translationPA = points[1]-fovJ; //(Y-axis)
        double translationIS = 0;              //(Z-axis)
        
        vtkTransform* transform = vtkTransform::New(); // initialized with identity matrix
        transform->Identity();
        transform->PostMultiply();
        //TODO: switch(needleEnteringDirection)
        transform->RotateZ(-90); 
        transform->RotateZ(-angle);
        transform->Translate(translationLR, translationPA, translationIS);
        
        if(showNeedle)
        {        
          vtkMRMLModelNode*   pNeedleModel;
          vtkMRMLDisplayNode* pNeedleDisplay;
          vtkCollection* collection = this->GetMRMLScene()->GetNodesByName("NeedleModel");
          if (collection != NULL && collection->GetNumberOfItems() == 0)
          {
            // if a node doesn't exist
            pNeedleModel = AddNeedleModel(transform, length);
          }
          else // if a node exists, remove it and make a new one with the current parameters
          {
            vtkMRMLModelNode* pNeedleModelOld = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
            this->GetMRMLScene()->RemoveNode((vtkMRMLNode*) pNeedleModelOld);
            pNeedleModelOld->Delete();
            pNeedleModel = AddNeedleModel(transform, length);
          }
          if (pNeedleModel)
          {
            pNeedleDisplay = pNeedleModel->GetDisplayNode();
            pNeedleDisplay->SetVisibility(1);
          }    
          else // if pNeedleModel == NULL
            std::cerr << "ERROR! No NeedleModel!" << std::endl; // Code should never get here!
        }
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
void vtkRealTimeNeedleDetectionGUI::BuildGUI ( )
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
    
  vtkKWWidget* page   = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  this->BuildHelpAndAboutFrame(page, help, about);
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::BuildGUIForGeneralParameters()
{
  vtkSlicerApplication* app = (vtkSlicerApplication*) this->GetApplication();
  vtkKWWidget* page = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  
  vtkSlicerModuleCollapsibleFrame* parentFrame = vtkSlicerModuleCollapsibleFrame::New();

  parentFrame->SetParent(page);
  parentFrame->Create();
  parentFrame->SetLabelText("needle detection parameters");
  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               parentFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Control frame
  vtkKWFrameWithLabel* controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(parentFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText("Needle detection controls");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", controlFrame->GetWidgetName() );

  // -----------------------------------------
  // push buttons
  vtkKWFrame* buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(controlFrame->GetFrame());
  buttonFrame->Create();
  app->Script("pack %s -fill both -expand true", buttonFrame->GetWidgetName());
                
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

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->pStartButton->GetWidgetName(),
               this->pStopButton->GetWidgetName());
               
  buttonFrame->Delete();

  // ----------------------------------------------------               
  // Scanner ID
  vtkKWFrame *scannerFrame = vtkKWFrame::New();
  scannerFrame->SetParent(controlFrame->GetFrame());
  scannerFrame->Create();
  app->Script("pack %s -fill both -expand true", scannerFrame->GetWidgetName());

  vtkKWLabel *scannerLabel = vtkKWLabel::New();
  scannerLabel->SetParent(scannerFrame);
  scannerLabel->Create();
  scannerLabel->SetWidth(20);
  scannerLabel->SetText("Scanner MRML ID: ");

  this->pScannerIDEntry = vtkKWEntry::New();
  this->pScannerIDEntry->SetParent(scannerFrame);
  this->pScannerIDEntry->Create();
  this->pScannerIDEntry->SetWidth(25);
  this->pScannerIDEntry->SetValue("vtkMRMLScalarVolumeNode1");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              scannerLabel->GetWidgetName() , this->pScannerIDEntry->GetWidgetName());
  
  scannerLabel->Delete();
  scannerFrame->Delete(); 
  
  this->pShowNeedleButton = vtkKWCheckButton::New();
  this->pShowNeedleButton->SetParent(controlFrame->GetFrame());
  this->pShowNeedleButton->Create();
  this->pShowNeedleButton->SelectedStateOff();
  this->pShowNeedleButton->SetText("Show Needle");
  this->pShowNeedleButton->EnabledOff();
  
  this->Script("pack %s -side left -anchor w -padx 2 -pady 2", this->pShowNeedleButton->GetWidgetName());
               
  controlFrame->Delete();
  
  // -----------------------------------------
  // Parameters frame
  vtkKWFrameWithLabel* parametersFrame = vtkKWFrameWithLabel::New();
  parametersFrame->SetParent(parentFrame->GetFrame());
  parametersFrame->Create();
  parametersFrame->SetLabelText("Coordinates");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", parametersFrame->GetWidgetName() );

  // -----------------------------------------
  // Boundary edit fields TODO: Check for bounds! 0 up to imageDimensions is possible
  vtkKWFrame* xFrame = vtkKWFrame::New();
  xFrame->SetParent(parametersFrame->GetFrame());
  xFrame->Create();
  app->Script ( "pack %s -fill both -expand true", xFrame->GetWidgetName());
  vtkKWLabel* xLabel = vtkKWLabel::New();
  xLabel->SetParent(xFrame);
  xLabel->Create();
  xLabel->SetWidth(15);
  xLabel->SetText("Boundaries for X: ");

  this->pXLowerEntry = vtkKWEntry::New();
  this->pXLowerEntry->SetParent(xFrame);
  this->pXLowerEntry->Create();
  this->pXLowerEntry->SetWidth(7);
  this->pXLowerEntry->SetValueAsInt(140);
              
  this->pXUpperEntry = vtkKWEntry::New();
  this->pXUpperEntry->SetParent(xFrame);
  this->pXUpperEntry->Create();
  this->pXUpperEntry->SetWidth(7);
  this->pXUpperEntry->SetValueAsInt(180);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              xLabel->GetWidgetName(), this->pXLowerEntry->GetWidgetName(), this->pXUpperEntry->GetWidgetName());
  xLabel->Delete();
  xFrame->Delete();
  
  vtkKWFrame* yFrame = vtkKWFrame::New();
  yFrame->SetParent(parametersFrame->GetFrame());
  yFrame->Create();
  app->Script ( "pack %s -fill both -expand true", yFrame->GetWidgetName());
  vtkKWLabel* yLabel = vtkKWLabel::New();
  yLabel->SetParent(yFrame);
  yLabel->Create();
  yLabel->SetWidth(15);
  yLabel->SetText("Boundaries for Y: ");

  this->pYLowerEntry = vtkKWEntry::New();
  this->pYLowerEntry->SetParent(yFrame);
  this->pYLowerEntry->Create();
  this->pYLowerEntry->SetWidth(7);
  this->pYLowerEntry->SetValueAsInt(125);
              
  this->pYUpperEntry = vtkKWEntry::New();
  this->pYUpperEntry->SetParent(yFrame);
  this->pYUpperEntry->Create();
  this->pYUpperEntry->SetWidth(7);
  this->pYUpperEntry->SetValueAsInt(150);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              yLabel->GetWidgetName(), this->pYLowerEntry->GetWidgetName(), this->pYUpperEntry->GetWidgetName());
  yLabel->Delete();
  yFrame->Delete();
  
  parametersFrame->Delete();
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
    this->pScannerIDEntry->EnabledOff();
    this->pXLowerEntry->EnabledOff();
    this->pXUpperEntry->EnabledOff();
    this->pYLowerEntry->EnabledOff();
    this->pYUpperEntry->EnabledOff();
    this->pShowNeedleButton->EnabledOn();
  }
  else if(started == 0)
  {
    this->pScannerIDEntry->EnabledOn();
    this->pXLowerEntry->EnabledOn();
    this->pXUpperEntry->EnabledOn();
    this->pYLowerEntry->EnabledOn();
    this->pYUpperEntry->EnabledOn();
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
    if((i >= currentYLowerBound*imageDimensions[1]*scalarSize) && (i < currentYUpperBound*imageDimensions[1]*scalarSize))  // Y-axis restrictions
    {
      if((currentXLowerBound*scalarSize <= (i%(imageDimensions[0]*scalarSize))) && (currentXUpperBound*scalarSize >  (i%(imageDimensions[0]*scalarSize))))
      {            
        pImageRegion[j] = pImage[i];
        j++;
      }
    }
  }
}

//--------------------------------------------------------------------------
// hard copy the region of interest back to the imageRegion byte by byte
void vtkRealTimeNeedleDetectionGUI::SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer();
  for(long i = 0; i <= currentXImageRegionSize * currentYImageRegionSize * scalarSize; i++)
  {
    int positionInMessageImage = currentXLowerBound*scalarSize + (i%(currentXImageRegionSize*scalarSize)) + (i/(currentXImageRegionSize*scalarSize))*imageDimensions[0]*scalarSize;
    pImage[positionInMessageImage+10*imageDimensions[0]*scalarSize] = pImageRegion[i];
  } 
}

//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkRealTimeNeedleDetectionGUI::AddNeedleModel(vtkTransform* transform, double height)
{
  vtkMRMLModelNode*        pNeedleModel;
  vtkMRMLModelDisplayNode* pNeedleDisplay;

  pNeedleModel = vtkMRMLModelNode::New();
  pNeedleDisplay = vtkMRMLModelDisplayNode::New();
  
  this->GetMRMLScene()->SaveStateForUndo();
  this->GetMRMLScene()->AddNode(pNeedleDisplay);
  this->GetMRMLScene()->AddNode(pNeedleModel);  
  
  pNeedleDisplay->SetScene(this->GetMRMLScene());
  
  pNeedleModel->SetName("NeedleModel");
  pNeedleModel->SetScene(this->GetMRMLScene());
  pNeedleModel->SetAndObserveDisplayNodeID(pNeedleDisplay->GetID());
  pNeedleModel->SetHideFromEditors(0);
  
  // Cylinder represents the needle stick
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius(1);
  cylinder->SetHeight(height);
  cylinder->SetCenter(0, 0, 0);
  cylinder->Update();

  // Rotate cylinder
  vtkTransformPolyDataFilter *tFilter1 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans =   vtkTransform::New();
  //trans->RotateX(90.0);
  trans->Translate(0.0, height/2, 0.0);
  trans->Update();
  tFilter1->SetInput(cylinder->GetOutput());
  tFilter1->SetTransform(trans);
  tFilter1->Update();
  
  // Sphere represents the needle tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  //apd->AddInput(cylinder->GetOutput());
  apd->AddInput(tFilter1->GetOutput());
  apd->Update();
  
  // Rotate&translate both, the sphere and the cylinder
  vtkTransformPolyDataFilter *tFilter2 = vtkTransformPolyDataFilter::New();
  tFilter2->SetInput(apd->GetOutput());
  tFilter2->SetTransform(transform);
  tFilter2->Update();  //TODO: Is there a better way to rotate&translate the whole construct?  |  maybe get rid of the sphere?
  
  pNeedleModel->SetAndObservePolyData(tFilter2->GetOutput());
  
  double color[3];
  color[0] = 0.0; // R
  color[1] = 1.0; // G
  color[2] = 0.0; // B
  pNeedleDisplay->SetPolyData(pNeedleModel->GetPolyData());
  pNeedleDisplay->SetColor(color);
  
  trans->Delete();
  tFilter1->Delete();
  tFilter2->Delete();
  cylinder->Delete();
  sphere->Delete();
  apd->Delete();

  pNeedleDisplay->Delete();

  return pNeedleModel;
}

//TODO: take that out when done measuring
double vtkRealTimeNeedleDetectionGUI::diffclock(clock_t clock1,clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
} 






