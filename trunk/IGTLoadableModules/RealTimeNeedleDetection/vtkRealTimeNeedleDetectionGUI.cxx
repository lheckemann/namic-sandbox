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

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntry.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkRealTimeNeedleDetectionGUI);
vtkCxxRevisionMacro (vtkRealTimeNeedleDetectionGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constructor
vtkRealTimeNeedleDetectionGUI::vtkRealTimeNeedleDetectionGUI()
{
  std::cout << "NeedleDetectionConstructor" << std::endl;
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void *> (this));
  this->DataCallbackCommand->SetCallback(vtkRealTimeNeedleDetectionGUI::DataCallback);
  this->DataManager = vtkIGTDataManager::New();  //TODO: Do I need the DataManager?
  started = 0;
  
  //----------------------------------------------------------------
  // GUI widgets
  this->pStartButton      = NULL;
  this->pStopButton       = NULL;
  this->pScannerIDEntry   = NULL;
  this->pXLowerEntry      = NULL;
  this->pXUpperEntry      = NULL;
  this->pYLowerEntry      = NULL;
  this->pYUpperEntry      = NULL;
  
  //--------------------------------------------------------------------
  // MRML nodes  
  this->pVolumeNode    = NULL;
  this->pSourceNode    = NULL;
  this->pTransformNode = NULL;
  
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
}

//---------------------------------------------------------------------------
// Destructor
vtkRealTimeNeedleDetectionGUI::~vtkRealTimeNeedleDetectionGUI()
{
  std::cout << "NeedleDetectionDestructor" << std::endl;
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
  //TODO: Delete pTransformNode

  //----------------------------------------------------------------
  // Unregister Logic class
  this->SetModuleLogic(NULL);
  
  //----------------------------------------------------------------
  // Delete pointers
  //delete pImageProcessor; //TODO: Does not work properly yet
  // TODO: delete pImage
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
        pVolumeNode->SetDescription("MRMLNode that displays the tracked needle");
        pVolumeNode->SetScene(this->GetMRMLScene());  
        
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
      else if(nItems == 1)
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
      std::cerr << "got data from image" << std::endl;
     // pImageData->Delete();  // If I delete this, I get a segmentation fault after pressing start and stop twice due to double freed memory -> I suggest GetImageData() does not deeply copy the ImageData
      std::cerr << "ImageData deleted" << std::endl;
      currentXLowerBound = initialXLowerBound = this->pXLowerEntry->GetValueAsInt();
      currentXUpperBound = initialXUpperBound = this->pXUpperEntry->GetValueAsInt();
      currentYLowerBound = initialYLowerBound = this->pYLowerEntry->GetValueAsInt();
      currentYUpperBound = initialYUpperBound = this->pYUpperEntry->GetValueAsInt();  
      currentXImageRegionSize                 = currentXUpperBound - currentXLowerBound;
      currentYImageRegionSize                 = currentYUpperBound - currentYLowerBound;  
      started = 1; // start checking for changes in pSourceNode to update pVolumeNode     
      std::cerr << "Start" << std::endl;
    }
    else // no Scanner node found in MRMLScene
      std::cerr << "ERROR! No Scanner detected. RealTimeNeedleDetection did not start." << std::endl;
  }
  
  if (this->pStopButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
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
    
    started = 0; // Stop checking MRLM events of pSourceNode 
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
  
  if(started && (lastModified != this->pSourceNode->GetMTime()))
  {
    lastModified = this->pSourceNode->GetMTime(); // This prevents unnecessarily issued ImageDataModifiedEvents from beging processed 
    std::cout << started << ":MRMLEvent processing while started";
    if((this->pSourceNode == vtkMRMLVolumeNode::SafeDownCast(caller)) && (event == vtkMRMLVolumeNode::ImageDataModifiedEvent))
    {
      std::cout << "-SourceNode Event" << std::endl;
      double points[4]; // Array that contains 2 points of the needle transform (x1,y1,x2,y2)
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
             pImageProcessor->Write("/projects/mrrobot/goerlitz/test/input.png",1);         
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
      
//      currentXLowerBound;
//      currentXUpperBound;        
//      currentYLowerBound;     
//      currentYUpperBound;
      currentXImageRegionSize = currentXUpperBound - currentXLowerBound;
      currentYImageRegionSize = currentYUpperBound - currentYLowerBound;    
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
  // Connection frame
  vtkKWFrameWithLabel* controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(parentFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText("Connection");
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
  controlFrame->Delete();
  
  // -----------------------------------------
  // Parameters frame
  vtkKWFrameWithLabel* parametersFrame = vtkKWFrameWithLabel::New();
  parametersFrame->SetParent(parentFrame->GetFrame());
  parametersFrame->Create();
  parametersFrame->SetLabelText("Coordinates");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", parametersFrame->GetWidgetName() );

  // -----------------------------------------
  // Boundary edit fields TODO: Check for bounds! 0 to imageDimensions
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
  this->pXLowerEntry->SetValueAsInt(120);
              
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
  this->pYLowerEntry->SetValueAsInt(120);
              
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


