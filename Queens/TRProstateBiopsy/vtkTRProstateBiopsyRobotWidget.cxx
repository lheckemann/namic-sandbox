/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkTRProstateBiopsyRobotWidget.h"

#include "vtkMRMLTRProstateBiopsyModuleNode.h"

#include "vtkKWRenderWidget.h"
#include "vtkSlicerViewerWidget.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyRobotWidget);
vtkCxxRevisionMacro(vtkTRProstateBiopsyRobotWidget, "$Revision: 1.0 $");
//------------------------------------------------------------------------------
vtkTRProstateBiopsyRobotWidget::vtkTRProstateBiopsyRobotWidget()
{
  this->MainViewerWidget = NULL;
  this->ProcessingMRMLEvent = 0;
  this->ProcessingWidgetEvent = 0;

}


//----------------------------------------------------------------------------
vtkTRProstateBiopsyRobotWidget::~vtkTRProstateBiopsyRobotWidget()
{
  this->SetMainViewerWidget(NULL);
  this->RemoveMRMLObservers();
  this->RemoveRobotWidgets();
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//--------------------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::AddMRMLObservers ()
{
  
 // observe scene for add/remove nodes
  vtkIntArray *events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::SceneClosingEvent);

  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  this->SetAndObserveMRMLSceneEvents(this->MRMLScene, events);
  events->Delete();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveMRMLObservers()
{
  this->RemoveMRMLRobotObservers();  
  this->SetAndObserveMRMLScene(NULL);
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::CreateWidget ( )
{
  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  this->AddMRMLObservers();  
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::ProcessMRMLEvents ( vtkObject *caller,
                                                unsigned long event, 
                                                void *callData )
{
  if (this->ProcessingMRMLEvent != 0 )
    {
    vtkDebugMacro("Returning because already processing an event, " << this->ProcessingMRMLEvent);
    return;
    }
  if (this->ProcessingWidgetEvent != 0 )
    {
    vtkDebugMacro("Returning because already processing an event, " << this->ProcessingMRMLEvent);
    return;
    }
    
  this->ProcessingMRMLEvent = event;

  vtkMRMLTRProstateBiopsyModuleNode *callerRobotNode = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(caller);
  vtkMRMLTRProstateBiopsyModuleNode *callDataNode =  NULL;
  callDataNode = reinterpret_cast<vtkMRMLTRProstateBiopsyModuleNode *>(callData);
  vtkMRMLScene *callScene = vtkMRMLScene::SafeDownCast(caller);


  if (callScene != NULL && event == vtkMRMLScene::SceneCloseEvent)
    {
    this->RemoveRobotWidgets();
    this->ProcessingMRMLEvent = 0;
    this->RequestRender();
    return;
    }

  // if get a node added event from the scene
  else if (callScene != NULL &&
           event == vtkMRMLScene::NodeAddedEvent &&
           callDataNode != NULL &&
           vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast ( (vtkObjectBase *)callData ))
    {
      this->UpdateFromMRML();
      this->RequestRender();
    }
  else if (callScene != NULL &&
           event == vtkMRMLScene::NodeRemovedEvent &&
           callDataNode != NULL)
    {
    vtkMRMLTRProstateBiopsyModuleNode *robot = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast ( (vtkObjectBase *)callData );
    if ( robot )
      {
      this->RemoveRobotWidget(robot->GetID());
      this->RequestRender();
      }
    }
  else if (callerRobotNode != NULL &&
           event == vtkCommand::ModifiedEvent)
    {
    this->UpdateRobotFromMRML(callerRobotNode);
    this->RequestRender();
    }
    
  this->ProcessingMRMLEvent = 0;
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RequestRender()
{
  this->MainViewerWidget->RequestRender();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::Render()
{
  this->MainViewerWidget->Render();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::UpdateFromMRML()
{
  vtkMRMLScene *scene = this->GetMRMLScene();
  if (scene == NULL)
    {
    vtkErrorMacro("...the scene is null... returning");
    return;
    }

  // remove not used boxes
  std::map<std::string, RobotWidget*>::iterator iter;
  std::map<std::string, RobotWidget*> temp;
  for (iter = this->DisplayedRobotWidgets.begin(); iter != this->DisplayedRobotWidgets.end(); iter++)
    {
    if (scene->GetNodeByID(iter->first.c_str()) != NULL)
      {
      temp[iter->first] = iter->second;
      }
    else
      {
      this->RemoveRobotWidget(iter->second);
      }
    }
  this->DisplayedRobotWidgets = temp;

  int nnodes = scene->GetNumberOfNodesByClass("vtkMRMLTRProstateBiopsyModuleNode");
  for (int n=0; n<nnodes; n++)
    {
    vtkMRMLTRProstateBiopsyModuleNode *robot = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(scene->GetNthNodeByClass(n, "vtkMRMLTRProstateBiopsyModuleNode"));
    this->UpdateRobotFromMRML(robot);
    }

  //this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::UpdateRobotFromMRML(vtkMRMLTRProstateBiopsyModuleNode *robot)
{
  if (robot == NULL)
    {
    vtkWarningMacro("UpdateRobotFromMRML: null input list!");
    return;
    }
  
  this->AddMRMLRobotObservers(robot);
  
  std::string robotID = std::string(robot->GetID());
  
  RobotWidget *robotWidget = this->GetRobotWidgetByID(robot->GetID());
  
  vtkSlicerBoxRepresentation *rep = NULL;

  if (!robotWidget)
    {
    // create one
    robotWidget = new RobotWidget;    
    robotWidget->SetRenderer(this->MainViewerWidget->GetMainViewer());   
    //this->MainViewerWidget->GetMainViewer()->RemoveViewProp(NULL);
    this->DisplayedRobotWidgets[robotID] = robotWidget;
    }

  robotWidget->UpdatePosition(robot);
}

//---------------------------------------------------------------------------
vtkTRProstateBiopsyRobotWidget::RobotWidget *vtkTRProstateBiopsyRobotWidget::GetRobotWidgetByID (const char *id)
{  
  if ( !id )      
    {
    return (NULL);
    }
  std::string sid = id;
  
  std::map< std::string, RobotWidget *>::iterator iter;
  iter = this->DisplayedRobotWidgets.find(sid);
  if (iter != this->DisplayedRobotWidgets.end())
    {
    return (iter->second);
    }
  else
    {
    return (NULL);
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveRobotWidgets()
{
  std::map< std::string, RobotWidget *>::iterator iter;
  for (iter = this->DisplayedRobotWidgets.begin();
       iter != this->DisplayedRobotWidgets.end();
       iter++)
    {
    if (iter->second != NULL)
      {
      RobotWidget* robotWidget = iter->second;
      this->RemoveRobotWidget(robotWidget);
      }
    } 
  this->DisplayedRobotWidgets.clear();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveRobotWidget(const char *robotID)
{
  if (robotID == NULL)
    {
    return;
    }
  
  RobotWidget* robotWidget = this->GetRobotWidgetByID(robotID);
  if  (robotWidget)
    {
    this->RemoveRobotWidget(robotWidget);
    this->DisplayedRobotWidgets.erase(std::string(robotID));
    }
  else
    {
    vtkWarningMacro("RemoveRobotWidget: couldn't find point widget");
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveRobotWidget(RobotWidget* robotWidget)
{
  //vtkEventBroker::GetInstance()->RemoveObservationsForSubjectByTag( robotWidget, 0 );
  //robotWidget->RemoveAllObservers();  
  robotWidget->SetRenderer(NULL);
  delete robotWidget;
  robotWidget = NULL;
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::AddMRMLRobotObservers(vtkMRMLTRProstateBiopsyModuleNode *robot)
{
  if (robot == NULL)
    {
    return;
    }
  
  if (robot->HasObserver ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand ) == 0)
    {
    robot->AddObserver ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand );
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveMRMLRobotObservers()
{
  if (this->GetMRMLScene() == NULL)
    {
    return;
    }
  // remove the observers on all the Robot nodes
  vtkMRMLTRProstateBiopsyModuleNode *robot;
  int nnodes = this->MRMLScene->GetNumberOfNodesByClass("vtkMRMLTRProstateBiopsyModuleNode");
  for (int n=0; n<nnodes; n++)
    {
    robot = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(this->GetMRMLScene()->GetNthNodeByClass(n, "vtkMRMLTRProstateBiopsyModuleNode"));
    this->RemoveMRMLRobotNodeObservers(robot);
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyRobotWidget::RemoveMRMLRobotNodeObservers(vtkMRMLTRProstateBiopsyModuleNode *robot)
{
  if (robot == NULL)
    {
    vtkWarningMacro("RemoveRobotObserversForList: null input list!");
    return;
    }
  
  if (robot->HasObserver (vtkCommand::ModifiedEvent, this->MRMLCallbackCommand ) == 1)
    {
    robot->RemoveObservers ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand );
    }
  if (robot->HasObserver( vtkMRMLTransformableNode::TransformModifiedEvent, this->MRMLCallbackCommand ) == 1)
    {
    robot->RemoveObservers ( vtkMRMLTransformableNode::TransformModifiedEvent, this->MRMLCallbackCommand );
    }
} 



vtkTRProstateBiopsyRobotWidget::RobotWidget::RobotWidget()
{
  this->NeedleTrajectoryLine=vtkSmartPointer<vtkLineSource>::New();
  this->NeedleTrajectoryActor=vtkSmartPointer<vtkActor>::New();
  this->RobotManipulatorActor=vtkSmartPointer<vtkActor>::New();
  this->RobotManipulatorSheathLine=vtkSmartPointer<vtkLineSource>::New();
  this->Renderer=NULL;
  SetupNeedleTrajectoryActor();
  SetupRobotManipulatorActor();
}

vtkTRProstateBiopsyRobotWidget::RobotWidget::~RobotWidget()
{
  SetRenderer(NULL);
}

void vtkTRProstateBiopsyRobotWidget::RobotWidget::SetRenderer(vtkKWRenderWidget* renderer)
{
  if (renderer==this->Renderer)
  {
    // no change
    return;
  }
  if (this->Renderer!=NULL)
  {
    // remove from old renderer
    this->Renderer->RemoveViewProp(this->NeedleTrajectoryActor);
    this->Renderer->RemoveViewProp(this->RobotManipulatorActor);
  }
  this->Renderer=renderer;
  if (this->Renderer!=NULL)
  {
    // add to new renderer
    this->Renderer->AddViewProp(this->NeedleTrajectoryActor);
    this->Renderer->AddViewProp(this->RobotManipulatorActor);
  }
}

void vtkTRProstateBiopsyRobotWidget::RobotWidget::UpdatePosition(vtkMRMLTRProstateBiopsyModuleNode *robot)
{
  this->UpdateNeedleTrajectoryPosition(robot);
  this->UpdateRobotManipulatorPosition(robot);
}

void vtkTRProstateBiopsyRobotWidget::RobotWidget::UpdateNeedleTrajectoryPosition(vtkMRMLTRProstateBiopsyModuleNode *robotNode)
{
  if (robotNode==NULL)
  {
    // node not yet available
    return;
  }

  int targetIndex=robotNode->GetCurrentTargetIndex();  
  vtkTRProstateBiopsyTargetDescriptor* targetDesc=NULL;
  if (targetIndex>=0)
  {
    targetDesc=robotNode->GetTargetDescriptorAtIndex(targetIndex);
  }

  if (targetDesc==NULL)
  {
    this->NeedleTrajectoryActor->SetVisibility(false);
    return;
  }

  // get RAS points of start and end point of needle
  // for the 3D viewer, the RAS coodinates are the world coordinates!!
  // this makes things simpler
  // render the needle as a thin pipe

  // start point is the target RAS
  double targetRAS[3];
  targetDesc->GetRASLocation(targetRAS); 

  double targetHingeRAS[3];
  targetDesc->GetHingePosition(targetHingeRAS);

  double needleVector[3];
  needleVector[0] = targetRAS[0] - targetHingeRAS[0];
  needleVector[1] = targetRAS[1] - targetHingeRAS[1];
  needleVector[2] = targetRAS[2] - targetHingeRAS[2];
  vtkMath::Normalize(needleVector);

  double overshoot = targetDesc->GetNeedleOvershoot();

  double needleEndRAS[3];
  needleEndRAS[0] = targetRAS[0] + overshoot*needleVector[0];
  needleEndRAS[1] = targetRAS[1] + overshoot*needleVector[1];
  needleEndRAS[2] = targetRAS[2] + overshoot*needleVector[2];

  // set up the line actors      
  this->NeedleTrajectoryLine->SetPoint1(needleEndRAS);
  this->NeedleTrajectoryLine->SetPoint2(targetHingeRAS);

  this->NeedleTrajectoryActor->SetVisibility(true);
}
//----------------------------------------------------------------------------

void vtkTRProstateBiopsyRobotWidget::RobotWidget::UpdateRobotManipulatorPosition(vtkMRMLTRProstateBiopsyModuleNode *robotNode)
{
  if (robotNode==NULL)
  {
    // node not yet available
    return;
  }

  vtkSmartPointer<vtkMatrix4x4> manipTransform=vtkSmartPointer<vtkMatrix4x4>::New();
  if (!robotNode->GetRobotManipulatorTransform(manipTransform))
  {
    // no calibration
    this->RobotManipulatorActor->SetVisibility(false);
    return;
  }

  double point1Manip[4]={-100,0,0 ,1}; // hinge point
  double point2Manip[4]={200,0,0 ,1}; // robot base point
  
  double point1Ras[4]={0,0,0 ,1};
  double point2Ras[4]={0,0,0 ,1};

  manipTransform->MultiplyPoint(point1Manip, point1Ras);
  manipTransform->MultiplyPoint(point2Manip, point2Ras);

  // set up the line actors      
  this->RobotManipulatorSheathLine->SetPoint1(point1Ras);
  this->RobotManipulatorSheathLine->SetPoint2(point2Ras);

  this->RobotManipulatorActor->SetVisibility(true);
}
//----------------------------------------------------------------------------



void vtkTRProstateBiopsyRobotWidget::RobotWidget::SetupNeedleTrajectoryActor()
{
  this->NeedleTrajectoryLine->SetResolution(100); 

  double color[3]={1.0, 1.0, 0.0};
  this->NeedleTrajectoryActor->GetProperty()->SetColor(color);

  vtkSmartPointer<vtkPolyDataMapper> axis1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis1Mapper->SetInputConnection(this->NeedleTrajectoryLine->GetOutputPort());
  this->NeedleTrajectoryActor->SetMapper(axis1Mapper);  
  this->NeedleTrajectoryActor->SetVisibility(false);
}

void vtkTRProstateBiopsyRobotWidget::RobotWidget::SetupRobotManipulatorActor()
{
  this->RobotManipulatorSheathLine->SetResolution(100);

  vtkSmartPointer<vtkTubeFilter> sheathTube=vtkSmartPointer<vtkTubeFilter>::New();
  sheathTube->SetInputConnection(RobotManipulatorSheathLine->GetOutputPort());
  //sheathTube->SetRadius(14.5);
  sheathTube->SetRadius(13.0); // TODO: read this from a model descriptor
  sheathTube->SetNumberOfSides(20);
  sheathTube->CappingOn();

  vtkSmartPointer<vtkAppendPolyData> apd = vtkSmartPointer<vtkAppendPolyData>::New();
  apd->AddInputConnection(sheathTube->GetOutputPort());
  apd->Update();

  double color[3]={1.0, 1.0, 0.0};
  this->RobotManipulatorActor->GetProperty()->SetColor(color);
  this->RobotManipulatorActor->GetProperty()->SetOpacity(0.5);

  vtkSmartPointer<vtkPolyDataMapper> axis1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis1Mapper->SetInputConnection(apd->GetOutputPort());
  this->RobotManipulatorActor->SetMapper(axis1Mapper);  
  this->RobotManipulatorActor->SetVisibility(false);
}
