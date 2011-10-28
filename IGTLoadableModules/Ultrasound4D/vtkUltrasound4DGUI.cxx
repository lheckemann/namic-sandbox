/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkUltrasound4DGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"
#include "vtkSlicerColorLogic.h"
#include "vtkCollection.h"
#include "vtkImageData.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWTopLevel.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWPushButton.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWScale.h"

#include "vtkCornerAnnotation.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRML4DVolumeNode.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <string>
#include <time.h>


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkUltrasound4DGUI );
vtkCxxRevisionMacro ( vtkUltrasound4DGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
vtkUltrasound4DGUI::vtkUltrasound4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUltrasound4DGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets

  this->OpenIGTLinkNodeSelector = NULL;
  this->OpenIGTLinkNode = NULL;
//  this->OpenIGTLinkNodeCollection = vtkCollection::New();
  this->NumberOfNodesReceived = 0;

  this->FourDVolumeNodeSelector = NULL;
  this->FourDVolumeNode = NULL;

  this->DisplayableScalarVolumeNode = NULL;
  this->DisplayableImageData = NULL;
  this->SliderVolumeSelector = NULL;
  this->PlayVolumeButton = NULL;
  this->IsPlaying = false;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkUltrasound4DGUI::~vtkUltrasound4DGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if(this->OpenIGTLinkNodeSelector)
    {
    this->OpenIGTLinkNodeSelector->SetParent(NULL);
    this->OpenIGTLinkNodeSelector->Delete();
    }
/*
  if(this->OpenIGTLinkNodeCollection)
  {
  // Delete all nodes inside first
  for(int i=0; i<this->OpenIGTLinkNodeCollection->GetNumberOfItems();i++)
  {
  this->OpenIGTLinkNodeCollection->GetItemAsObject(i)->Delete();
  }

  this->OpenIGTLinkNodeCollection->Delete();
  }
*/
  if(this->FourDVolumeNodeSelector)
    {
    this->FourDVolumeNodeSelector->Delete();
    }

  if(this->DisplayableScalarVolumeNode)
    {
    this->DisplayableScalarVolumeNode->Delete();
    }

  if(this->DisplayableImageData)
    {
    this->DisplayableImageData->Delete();
    }

  if(this->SliderVolumeSelector)
    {
    this->SliderVolumeSelector->SetParent(NULL);
    this->SliderVolumeSelector->Delete();
    }

  if(this->PlayVolumeButton)
    {
    this->PlayVolumeButton->SetParent(NULL);
    this->PlayVolumeButton->Delete();
    }


  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::Init()
{
  //Register new node type to the scene
  vtkMRMLScene* scene = this->GetMRMLScene();
  vtkMRML4DVolumeNode* sNode = vtkMRML4DVolumeNode::New();
  scene->RegisterNodeClass(sNode);
  sNode->Delete();
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::Enter()
{
  vtkMRMLScalarVolumeDisplayNode *displayNode = NULL;

  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

  // Create Displayable Scalar Volume Node
  if(!this->DisplayableScalarVolumeNode)
    {
    this->DisplayableScalarVolumeNode = vtkMRMLScalarVolumeNode::New();
    this->DisplayableScalarVolumeNode->SetName("TimeSerieDisplay");
    this->DisplayableImageData = vtkImageData::New();

    this->DisplayableImageData->SetDimensions(256,256,256);
    this->DisplayableImageData->SetExtent(0, 255, 0, 255, 0, 255);
    this->DisplayableImageData->SetSpacing(1.0, 1.0, 1.0);
    this->DisplayableImageData->SetOrigin(0.0, 0.0, 0.0);
    this->DisplayableImageData->SetNumberOfScalarComponents(1);
    this->DisplayableImageData->SetScalarTypeToUnsignedShort();
    this->DisplayableImageData->AllocateScalars();

    unsigned short* dest = (unsigned short*) this->DisplayableImageData->GetScalarPointer();
    if(dest)
      {
      memset(dest, 0x00, 256*256*256*sizeof(unsigned short));
      this->DisplayableImageData->Update();
      }
    this->DisplayableScalarVolumeNode->SetAndObserveImageData(this->DisplayableImageData);

    displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    displayNode->SetScene(this->GetMRMLScene());
    double range[2] = {0.0, 256.0};

    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]));
    this->GetMRMLScene()->AddNode(displayNode);

    vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
    displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());

    this->DisplayableScalarVolumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());

    this->GetMRMLScene()->AddNode(this->DisplayableScalarVolumeNode);
    this->GetMRMLScene()->Modified();

    // TODO: Center Image
    this->CenterImage(this->DisplayableScalarVolumeNode);

    displayNode->Delete();
    colorLogic->Delete();
    }

  if(this->DisplayableScalarVolumeNode && this->SliderVolumeSelector)
    {
    this->SliderVolumeSelector->GetWidget()->InvokeEvent(vtkKWScale::ScaleValueChangingEvent);
    }

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Ultrasound4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if(this->OpenIGTLinkNodeSelector)
    {
    this->OpenIGTLinkNodeSelector
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->OpenIGTLinkNode)
    {
    this->OpenIGTLinkNode
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->FourDVolumeNodeSelector)
    {
    this->FourDVolumeNodeSelector
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->SliderVolumeSelector->GetWidget())
    {
    this->SliderVolumeSelector->GetWidget()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->PlayVolumeButton)
    {
    this->PlayVolumeButton
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::SceneLoadEndEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if(this->OpenIGTLinkNodeSelector)
    {
    this->OpenIGTLinkNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->FourDVolumeNodeSelector)
    {
    this->FourDVolumeNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->SliderVolumeSelector->GetWidget())
    {
    this->SliderVolumeSelector->GetWidget()
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->PlayVolumeButton)
    {
    this->PlayVolumeButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkUltrasound4DLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessGUIEvents(vtkObject *caller,
                                          unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if(this->OpenIGTLinkNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    vtkMRMLIGTLConnectorNode* getNode = vtkMRMLIGTLConnectorNode::SafeDownCast(this->OpenIGTLinkNodeSelector->GetSelected());
    if(getNode)
      {
      this->OpenIGTLinkNode = getNode;
      if(this->OpenIGTLinkNode)
        {
        this->OpenIGTLinkNode
          ->AddObserver(vtkMRMLIGTLConnectorNode::ReceiveEvent, (vtkCommand*)this->GUICallbackCommand);
        }
      }
    }

  if(this->FourDVolumeNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    //TODO: Check if copying is running to avoid copying in different nodes if 4D Volume Node is changed during copying
    if(this->FourDVolumeNodeSelector->GetSelected())
      {
      this->FourDVolumeNode = vtkMRML4DVolumeNode::SafeDownCast(this->FourDVolumeNodeSelector->GetSelected());

      // New node created (because no SerieID)
      if(!strcmp(this->FourDVolumeNode->GetSerieID().c_str(), ""))
        {
        // Create "SerieID" attribute
        time_t ltime;
        struct tm *Tm;
        ltime = time(NULL);
        Tm = localtime(&ltime);
        std::ostringstream out;
        out << Tm->tm_mon+1 << Tm->tm_mday << Tm->tm_year+1900 << Tm->tm_hour << Tm->tm_min << Tm->tm_sec;

        // Set SerieID in the 4D Volume node
        this->FourDVolumeNode->SetSerieID(out.str());
        int numberOfSeries = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRML4DVolumeNode");
        std::ostringstream nameSerie;
        nameSerie << "TimeSerie" << numberOfSeries;
        this->FourDVolumeNode->SetName(nameSerie.str().c_str());
        this->FourDVolumeNodeSelector->UpdateMenu();

        // Reset values when selecting a new node
        if(this->SliderVolumeSelector)
          {
          this->NumberOfNodesReceived = 0;
          this->SliderVolumeSelector->GetWidget()->SetValue(0);
          this->SliderVolumeSelector->GetWidget()->SetRange(0,0);
          this->SliderVolumeSelector->SetLabelText("/ 0");
          }
        }
      else
        {
        // TODO: Get SerieID and all node with this SerieID to populate vtkCollection and select this vtkCollection
        if(this->SliderVolumeSelector && this->FourDVolumeNode)
          {
          this->NumberOfNodesReceived = this->FourDVolumeNode->GetNumberOfVolumes();
          std::ostringstream numVol;
          numVol << "/ " << this->NumberOfNodesReceived-1;
          this->SliderVolumeSelector->GetWidget()->SetRange(0, this->NumberOfNodesReceived-1);
          this->SliderVolumeSelector->SetLabelText(numVol.str().c_str());
          }
        }
      }
    }

  if(this->OpenIGTLinkNode == vtkMRMLIGTLConnectorNode::SafeDownCast(caller)
     && event == vtkMRMLIGTLConnectorNode::ReceiveEvent)
    {
    if(this->FourDVolumeNode)
      {
      if(this->OpenIGTLinkNode->GetNumberOfIncomingMRMLNodes())
        {
        this->NumberOfNodesReceived++;

        // Update ImageData (before copying)
        vtkMRMLScalarVolumeNode* currentData = vtkMRMLScalarVolumeNode::SafeDownCast(this->OpenIGTLinkNode->GetIncomingMRMLNode(0));
        currentData->GetImageData()->Update();

        // Copy ScalarVolumeNode to the new one
        vtkMRMLScalarVolumeNode* newNode = vtkMRMLScalarVolumeNode::New();
        newNode->Copy(currentData);
        newNode->SetHideFromEditors(0);
        newNode->SetSaveWithScene(1);
        newNode->SetScene(this->GetMRMLScene());

        // Set Name
        int numberOfVolumesInIt = this->FourDVolumeNode->GetNumberOfVolumes();
        std::stringstream NodeName;
        NodeName << this->FourDVolumeNode->GetName() << "Volume" << numberOfVolumesInIt;
        newNode->SetName(NodeName.str().c_str());

        if(strcmp(this->FourDVolumeNode->GetSerieID().c_str(), ""))
          {
          //newNode->SetSerieID(this->FourDVolumeNode->GetSerieID().c_str());
          newNode->SetAttribute("SerieID",this->FourDVolumeNode->GetSerieID().c_str());
          }

        // Copy Image Data to the new one
        vtkImageData* newData = vtkImageData::New();
        newData->DeepCopy(currentData->GetImageData());

        // Link Image Data to the new ScalarVolume Node and add to scene
        newNode->SetAndObserveImageData(newData);
        this->GetMRMLScene()->AddNode(newNode);
        this->GetMRMLScene()->Modified();

        // Add Node to the collection
        this->FourDVolumeNode->AddVolume(newNode);

        // Delete Image Data and Scalar Volume
        newData->Delete();
        newNode->Delete();
        }

      if(this->SliderVolumeSelector)
        {
        std::stringstream out;
        out << "/ " << this->NumberOfNodesReceived-1;
        this->SliderVolumeSelector->SetLabelText(out.str().c_str());
        this->SliderVolumeSelector->GetWidget()->SetRange(0,this->NumberOfNodesReceived-1);
        this->SliderVolumeSelector->GetWidget()->SetValue(this->NumberOfNodesReceived-1);
        }
      }
    }

  if(this->SliderVolumeSelector->GetWidget() == vtkKWScale::SafeDownCast(caller)
     && event                                == vtkKWScale::ScaleValueChangingEvent)
    {
    if(this->SliderVolumeSelector && this->FourDVolumeNode)
      {
      if(this->FourDVolumeNode->GetNumberOfVolumes() > 0)
        {
        double                   NodeNumber   = this->SliderVolumeSelector->GetWidget()->GetValue();
        vtkMRMLScalarVolumeNode* selectedNode = this->FourDVolumeNode->GetItemAsVolume(NodeNumber);

        this->GetLogic()->CopyVolume(this->DisplayableScalarVolumeNode, selectedNode);
        this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->Render();
        }
      }
    }

  if(this->PlayVolumeButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->PlayVolumeButton)
      {
      if(this->IsPlaying)
        {
        this->PlayVolumeButton->SetText("Play");
        this->IsPlaying = false;

        // TODO: Stop the loop
        }
      else
        {
        this->PlayVolumeButton->SetText("Pause");
        this->IsPlaying = true;

        // Start playing volumes
        if(this->FourDVolumeNode && this->SliderVolumeSelector && this->DisplayableScalarVolumeNode)
          {
          this->GetLogic()->PlayVolumes(this->DisplayableScalarVolumeNode, this->FourDVolumeNode->GetVolumeCollection(), this->SliderVolumeSelector->GetWidget());
          }
        }
      }
    }

}


void vtkUltrasound4DGUI::DataCallback(vtkObject *caller,
                                      unsigned long eid, void *clientData, void *callData)
{
  vtkUltrasound4DGUI *self = reinterpret_cast<vtkUltrasound4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUltrasound4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessLogicEvents ( vtkObject *caller,
                                              unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkUltrasound4DLogic::SafeDownCast(caller))
    {
    if (event == vtkUltrasound4DLogic::StatusUpdateEvent)
      {
      }
    }
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  if(event == vtkMRMLScene::SceneLoadEndEvent)
    {
    // Populate vtkCollection of 4DVolumeNode after loading scene with same SerieID vtkMRMLScalarVolumeNode
    int numberOfTimeSeries = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRML4DVolumeNode");
    if(numberOfTimeSeries > 0)
      {
      std::vector<vtkMRMLNode *> VectorTimeSeries;
      this->GetMRMLScene()->GetNodesByClass("vtkMRML4DVolumeNode", VectorTimeSeries);

      // Look for all TimeSeries
      for(unsigned int i = 0; i < VectorTimeSeries.size(); i++)
        {
        vtkMRML4DVolumeNode* TimeSerie = vtkMRML4DVolumeNode::SafeDownCast(VectorTimeSeries[i]);
        int numberOfVolumes = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLScalarVolumeNode");

        if(numberOfVolumes > 0)
          {
          std::vector<vtkMRMLNode *> VectorVolumes;
          this->GetMRMLScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", VectorVolumes);

          // Look for all Volume with same SerieID than current TimeSerie
          for(unsigned int j=0; j < VectorVolumes.size(); j++)
            {
            vtkMRMLScalarVolumeNode* VolumeToCheck = vtkMRMLScalarVolumeNode::SafeDownCast(VectorVolumes[j]);
            if(VolumeToCheck->GetAttribute("SerieID") != NULL)
              {
              if(!strcmp(TimeSerie->GetSerieID().c_str(),VolumeToCheck->GetAttribute("SerieID")))
                {
                TimeSerie->AddVolume(VolumeToCheck);
                }
              }
            }
          }
        }

      // Update Selector Menu and number of nodes in the TimeSerie
      if(this->FourDVolumeNodeSelector)
        {
        this->FourDVolumeNodeSelector->SetSelected(VectorTimeSeries[0]);
        this->FourDVolumeNodeSelector->UpdateMenu();
        this->FourDVolumeNodeSelector->InvokeEvent(vtkSlicerNodeSelectorWidget::NodeSelectedEvent);
        }
      }
    this->DisplayableScalarVolumeNode = NULL;
    }
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(),
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "Ultrasound4D", "Ultrasound4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForSlidingData();
}


void vtkUltrasound4DGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:Ultrasound4D</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Ultrasound4D" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::BuildGUIForSlidingData()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Ultrasound4D");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Simulator");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  this->OpenIGTLinkNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->OpenIGTLinkNodeSelector->SetParent(conBrowsFrame->GetFrame());
  this->OpenIGTLinkNodeSelector->Create();
  this->OpenIGTLinkNodeSelector->SetNewNodeEnabled(0);
  this->OpenIGTLinkNodeSelector->SetNodeClass("vtkMRMLIGTLConnectorNode",NULL,NULL,NULL);
  this->OpenIGTLinkNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->OpenIGTLinkNodeSelector->UpdateMenu();

  this->FourDVolumeNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->FourDVolumeNodeSelector->SetParent(conBrowsFrame->GetFrame());
  this->FourDVolumeNodeSelector->Create();
  this->FourDVolumeNodeSelector->SetNewNodeEnabled(1);
  this->FourDVolumeNodeSelector->SetNodeClass("vtkMRML4DVolumeNode",NULL,NULL,NULL);
  this->FourDVolumeNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->FourDVolumeNodeSelector->UpdateMenu();

  this->SliderVolumeSelector = vtkKWScaleWithLabel::New();
  this->SliderVolumeSelector->SetParent(conBrowsFrame->GetFrame());
  this->SliderVolumeSelector->Create();
  this->SliderVolumeSelector->SetLabelPositionToRight();
  this->SliderVolumeSelector->GetLabel()->SetText("/ 0");
  this->SliderVolumeSelector->GetWidget()->SetSliderLength(20);
  this->SliderVolumeSelector->GetWidget()->SetRange(0,0);

  this->PlayVolumeButton = vtkKWPushButton::New();
  this->PlayVolumeButton->SetParent(conBrowsFrame->GetFrame());
  this->PlayVolumeButton->Create();
  this->PlayVolumeButton->SetText("Play");

  app->Script("pack %s %s %s %s -side top -anchor nw -fill x -padx 2 -pady 2",
              this->OpenIGTLinkNodeSelector->GetWidgetName(),
              this->FourDVolumeNodeSelector->GetWidgetName (),
              this->SliderVolumeSelector->GetWidgetName(),
              this->PlayVolumeButton->GetWidgetName());


  conBrowsFrame->Delete();
}

//----------------------------------------------------------------------------
void vtkUltrasound4DGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkUltrasound4DGUI::CenterImage(vtkMRMLVolumeNode *volumeNode)
{
  if ( volumeNode )
    {
    vtkImageData *image = volumeNode->GetImageData();
    if (image)
      {
      vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
      volumeNode->GetIJKToRASMatrix(ijkToRAS);

      double dimsH[4];
      double rasCorner[4];
      int *dims = image->GetDimensions();
      dimsH[0] = dims[0] - 1;
      dimsH[1] = dims[1] - 1;
      dimsH[2] = dims[2] - 1;
      dimsH[3] = 0.;
      ijkToRAS->MultiplyPoint(dimsH, rasCorner);

      double origin[3];
      int i;
      for (i = 0; i < 3; i++)
        {
        origin[i] = -0.5 * rasCorner[i];
        }
      volumeNode->SetDisableModifiedEvent(1);
      volumeNode->SetOrigin(origin);
      volumeNode->SetDisableModifiedEvent(0);
      volumeNode->InvokePendingModifiedEvent();

      ijkToRAS->Delete();
      }
    }
}

