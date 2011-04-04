/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavGUI.h"

/* KWWidgets includes */
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"

/* MRML includes */
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLLinearTransformNode.h"

/* Slicer includes */
#include "vtkSlicerApplication.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerSlicesControlGUI.h"
#include "vtkSlicerTheme.h"

/* STL includes */
#include <limits>

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavGUI, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavGUI);


//---------------------------------------------------------------------------
vtkAbdoNavGUI::vtkAbdoNavGUI()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  //----------------------------------------------------------------
  this->AbdoNavLogic = NULL;
  this->AbdoNavNode = NULL;

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  this->TrackerTransformSelector = NULL;
  this->Point1CheckButton = NULL;
  this->Point1REntry = NULL;
  this->Point1AEntry = NULL;
  this->Point1SEntry = NULL;
  this->Point2CheckButton = NULL;
  this->Point2REntry = NULL;
  this->Point2AEntry = NULL;
  this->Point2SEntry = NULL;
  this->Point3CheckButton = NULL;
  this->Point3REntry = NULL;
  this->Point3AEntry = NULL;
  this->Point3SEntry = NULL;
  this->ResetRegistrationPushButton = NULL;
  this->PerformRegistrationPushButton = NULL;

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  this->ShowLocatorCheckButton = NULL;
  this->ProjectionLengthScale = NULL;
  this->FreezeLocatorCheckButton = NULL;
  this->ShowCrosshairCheckButton = NULL;
  this->DrawNeedleProjectionCheckButton = NULL;
  this->RedSliceMenuButton = NULL;
  this->YellowSliceMenuButton = NULL;
  this->GreenSliceMenuButton = NULL;
  this->SetLocatorAllPushButton = NULL;
  this->SetUserAllPushButton = NULL;
  this->FreezeSliceCheckButton = NULL;
  this->ObliqueCheckButton = NULL;
}


//---------------------------------------------------------------------------
vtkAbdoNavGUI::~vtkAbdoNavGUI()
{
  //----------------------------------------------------------------
  // Cleanup logic values.
  //----------------------------------------------------------------
  // if Logic is NULL, the class was only instantiated but never used,
  // e.g. Slicer was launched with option --ignore_module set to AbdoNav
  if (this->AbdoNavLogic)
    {
    this->RemoveGUIObservers();
    }
  this->SetModuleLogic(NULL);

  // remove AbdoNavNode observers
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  // TODO: all of the following Delete() calls could probably be avoided by using VTK's smart pointers

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  if (this->TrackerTransformSelector)
    {
    this->TrackerTransformSelector->SetParent(NULL);
    this->TrackerTransformSelector->Delete();
    }
  if (this->Point1CheckButton)
    {
    this->Point1CheckButton->SetParent(NULL);
    this->Point1CheckButton->Delete();
    }
  if (this->Point1REntry)
    {
    this->Point1REntry->SetParent(NULL);
    this->Point1REntry->Delete();
    }
  if (this->Point1AEntry)
    {
    this->Point1AEntry->SetParent(NULL);
    this->Point1AEntry->Delete();
    }
  if (this->Point1SEntry)
    {
    this->Point1SEntry->SetParent(NULL);
    this->Point1SEntry->Delete();
    }
  if (this->Point2CheckButton)
    {
    this->Point2CheckButton->SetParent(NULL);
    this->Point2CheckButton->Delete();
    }
  if (this->Point2REntry)
    {
    this->Point2REntry->SetParent(NULL);
    this->Point2REntry->Delete();
    }
  if (this->Point2AEntry)
    {
    this->Point2AEntry->SetParent(NULL);
    this->Point2AEntry->Delete();
    }
  if (this->Point2SEntry)
    {
    this->Point2SEntry->SetParent(NULL);
    this->Point2SEntry->Delete();
    }
  if (this->Point3CheckButton)
    {
    this->Point3CheckButton->SetParent(NULL);
    this->Point3CheckButton->Delete();
    }
  if (this->Point3REntry)
    {
    this->Point3REntry->SetParent(NULL);
    this->Point3REntry->Delete();
    }
  if (this->Point3AEntry)
    {
    this->Point3AEntry->SetParent(NULL);
    this->Point3AEntry->Delete();
    }
  if (this->Point3SEntry)
    {
    this->Point3SEntry->SetParent(NULL);
    this->Point3SEntry->Delete();
    }
  if (this->ResetRegistrationPushButton)
    {
    this->ResetRegistrationPushButton->SetParent(NULL);
    this->ResetRegistrationPushButton->Delete();
    }
  if (this->PerformRegistrationPushButton)
    {
    this->PerformRegistrationPushButton->SetParent(NULL);
    this->PerformRegistrationPushButton->Delete();
    }

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  if (this->ShowLocatorCheckButton)
    {
    this->ShowLocatorCheckButton->SetParent(NULL);
    this->ShowLocatorCheckButton->Delete();
    }
  if (this->ProjectionLengthScale)
    {
    this->ProjectionLengthScale->SetParent(NULL);
    this->ProjectionLengthScale->Delete();
    }
  if (this->FreezeLocatorCheckButton)
    {
    this->FreezeLocatorCheckButton->SetParent(NULL);
    this->FreezeLocatorCheckButton->Delete();
    }
  if (this->ShowCrosshairCheckButton)
    {
    this->ShowCrosshairCheckButton->SetParent(NULL);
    this->ShowCrosshairCheckButton->Delete();
    }
  if (this->DrawNeedleProjectionCheckButton)
    {
    this->DrawNeedleProjectionCheckButton->SetParent(NULL);
    this->DrawNeedleProjectionCheckButton->Delete();
    }
  if (this->RedSliceMenuButton)
    {
    this->RedSliceMenuButton->SetParent(NULL);
    this->RedSliceMenuButton->Delete();
    }
  if (this->YellowSliceMenuButton)
    {
    this->YellowSliceMenuButton->SetParent(NULL);
    this->YellowSliceMenuButton->Delete();
    }
  if (this->GreenSliceMenuButton)
    {
    this->GreenSliceMenuButton->SetParent(NULL);
    this->GreenSliceMenuButton->Delete();
    }
  if (this->SetLocatorAllPushButton)
    {
    this->SetLocatorAllPushButton->SetParent(NULL);
    this->SetLocatorAllPushButton->Delete();
    }
  if (this->SetUserAllPushButton)
    {
    this->SetUserAllPushButton->SetParent(NULL);
    this->SetUserAllPushButton->Delete();
    }
  if (this->FreezeSliceCheckButton)
    {
    this->FreezeSliceCheckButton->SetParent(NULL);
    this->FreezeSliceCheckButton->Delete();
    }
  if (this->ObliqueCheckButton)
    {
    this->ObliqueCheckButton->SetParent(NULL);
    this->ObliqueCheckButton->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkAbdoNavGUI: " << this->GetClassName() << "\n";
  os << indent << "vtkAbdoNavLogic: " << this->AbdoNavLogic << "\n";
  os << indent << "vtkAbdoNavNode: " << this->AbdoNavNode << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Init()
{
  // this is necessary so that an AbdoNavNode can be created from its class name,
  // e.g. when a previously saved scene (containing an AbdoNavNode) is (re-)loaded
  vtkMRMLAbdoNavNode* node = vtkMRMLAbdoNavNode::New();
  this->AbdoNavLogic->GetMRMLScene()->RegisterNodeClass(node);
  node->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Enter()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Exit()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddGUIObservers()
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Set observers on slice views.
  //----------------------------------------------------------------
  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();

  appGUI->GetMainSliceGUI("Red")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Yellow")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Green")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  this->TrackerTransformSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Point1CheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Point2CheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Point3CheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ResetRegistrationPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->PerformRegistrationPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  this->ShowLocatorCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ProjectionLengthScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->FreezeLocatorCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ShowCrosshairCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->DrawNeedleProjectionCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->RedSliceMenuButton->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->YellowSliceMenuButton->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->GreenSliceMenuButton->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->SetLocatorAllPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->SetUserAllPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->FreezeSliceCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ObliqueCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);

  // must be called manually!
  this->AddLogicObservers();
  this->AddMRMLObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveGUIObservers()
{
  //----------------------------------------------------------------
  // Remove observers from slice views.
  //----------------------------------------------------------------
  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();

  if (appGUI && appGUI->GetMainSliceGUI("Red"))
    {
    appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (appGUI && appGUI->GetMainSliceGUI("Yellow"))
    {
    appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (appGUI && appGUI->GetMainSliceGUI("Yellow"))
    {
    appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  if (this->TrackerTransformSelector)
    {
    this->TrackerTransformSelector->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->Point1CheckButton)
    {
    this->Point1CheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->Point2CheckButton)
    {
    this->Point2CheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->Point3CheckButton)
    {
    this->Point3CheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->ResetRegistrationPushButton)
    {
    this->ResetRegistrationPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->PerformRegistrationPushButton)
    {
    this->PerformRegistrationPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  if (this->ShowLocatorCheckButton)
    {
    this->ShowLocatorCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->ProjectionLengthScale)
    {
    this->ProjectionLengthScale->GetWidget()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->FreezeLocatorCheckButton)
    {
    this->FreezeLocatorCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->ShowCrosshairCheckButton)
    {
    this->ShowCrosshairCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->DrawNeedleProjectionCheckButton)
    {
    this->DrawNeedleProjectionCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->RedSliceMenuButton)
    {
    this->RedSliceMenuButton->GetMenu()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->YellowSliceMenuButton)
    {
    this->YellowSliceMenuButton->GetMenu()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->GreenSliceMenuButton)
    {
    this->GreenSliceMenuButton->GetMenu()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->SetLocatorAllPushButton)
    {
    this->SetLocatorAllPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->SetUserAllPushButton)
    {
    this->SetUserAllPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->FreezeSliceCheckButton)
    {
    this->FreezeSliceCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->ObliqueCheckButton)
    {
    this->ObliqueCheckButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  // must be called manually!
  this->RemoveLogicObservers();
  this->RemoveMRMLObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddLogicObservers()
{
  this->RemoveLogicObservers();

  if (this->AbdoNavLogic)
    {
    this->AbdoNavLogic->AddObserver(vtkAbdoNavLogic::StatusUpdateEvent, (vtkCommand*)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveLogicObservers()
{
  if (this->AbdoNavLogic)
    {
    this->AbdoNavLogic->RemoveObservers(vtkAbdoNavLogic::StatusUpdateEvent, (vtkCommand*)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddMRMLObservers()
{
  // add scene observers
  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NewSceneEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveMRMLObservers()
{
  // remove scene observers
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), NULL);
    }

  // remove registration fiducial list observers
  if (this->AbdoNavNode != NULL)
    {
    vtkMRMLFiducialListNode* regFidList = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
    if (regFidList != NULL)
      {
      regFidList->RemoveObservers(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
      regFidList->RemoveObservers(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand);
      regFidList->RemoveObservers(vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand);
      regFidList->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //----------------------------------------------------------------
  // Slice views.
  //
  // If the user clicked in one of the slice views with one of the
  // check buttons associated with the RAS coordinates of
  //  - the guidance needle tip
  //  - a second point on the guidance needle
  //  - the marker center
  // being active at the same time, the corresponding fiducial in
  // AbdoNav's fiducial list is being updated. If there exists no
  // corresponding fiducial in AbdoNav's fiducial list yet, a new
  // fiducial is added.
  //
  // The steps are:
  // 0. determine whether or not AbdoNav is the selected module;
  //    neither update nor create a fiducial if it isn't, thus
  //    exit this function
  // 1. if AbdoNav is the selected module, determine which check
  //    button is active (if there is an active one at all)
  // 2. if there is an active check button:
  //      2.0. create/retrieve AbdoNav's fiducial list
  //      2.1. transform XY mouse coordinates into RAS coordinates
  //      2.2. determine whether or not AbdoNav's fiducial list
  //           already contains a fiducial corresponding to the
  //           active check button
  //      2.3. if AbdoNav's fiducial list already contains a corres-
  //           ponding fiducial, then update it; otherwise, add a new
  //           fiducial
  //----------------------------------------------------------------
  vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
  if (style != NULL && event == vtkCommand::LeftButtonPressEvent)
    {
    // exit this function if AbdoNav isn't the selected module (this->GetModuleName()
    // returns NULL, thus use this->GetGUIName())
    vtkMRMLLayoutNode* layout = vtkMRMLLayoutNode::SafeDownCast(this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLLayoutNode"));
    if (layout && layout->GetSelectedModule() && strcmp(layout->GetSelectedModule(), this->GetGUIName()) != 0)
      {
      // another module is currently selected, thus exit
      return;
      }

    // determine which check button is active
    std::string activeCheckButton = "";
    if (this->Point1CheckButton->GetSelectedState() && this->Point1CheckButton->GetEnabled())
      {
      activeCheckButton = "tipRAS";
      }
    else if (this->Point2CheckButton->GetSelectedState() && this->Point2CheckButton->GetEnabled())
      {
      activeCheckButton = "2ndRAS";
      }
    else if (this->Point3CheckButton->GetSelectedState() && this->Point3CheckButton->GetEnabled())
      {
      activeCheckButton = "markerRAS";
      }

    // if there is an active check button
    if (strcmp(activeCheckButton.c_str(), ""))
      {
      // create an AbdoNavNode if none exists yet
      vtkMRMLAbdoNavNode* anode = this->CheckAndCreateAbdoNavNode();

      // create/retrieve AbdoNav's fiducial list
      vtkMRMLFiducialListNode* fiducialList;
      if (anode->GetRegistrationFiducialListID() == NULL)
        {
        // AbdoNav registration fiducial list doesn't exist yet, thus create it
        fiducialList = vtkMRMLFiducialListNode::New();
        fiducialList->SetName("AbdoNav-RegistrationFiducialList");
        fiducialList->SetDescription("Created by AbdoNav");
        // change default look ("StarBurst2D", 5) since it doesn't really
        // suit the purpose of needle identification; the user can always
        // return to the default look using Slicer's Fiducials module
        fiducialList->SetGlyphTypeFromString("Sphere3D");
        fiducialList->SetSymbolScale(2); // called "Glyph scale" in the Fiducials module
        this->GetMRMLScene()->AddNode(fiducialList);
        fiducialList->Delete();
        // update MRML node
        anode->SetRegistrationFiducialListID(fiducialList->GetID());
        // observe fiducial list in order to update the GUI whenever a fiducial is moved via drag & drop or renamed or renumbered externally via the Fiducials module
        fiducialList->AddObserver(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
        // observe fiducial list in order to update the GUI whenever a fiducial is added externally via the Fiducials module
        fiducialList->AddObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand);
        // observe fiducial list in order to update the GUI whenever a fiducial is removed externally via the Fiducials module
        fiducialList->AddObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand);
        // observe fiducial list in order to update the GUI whenever all fiducials are removed externally via the Fiducials module
        fiducialList->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
        // no need to observe vtkMRMLFiducialListNode::DisplayModifiedEvent or vtkMRMLFiducialListNode::FiducialIndexModifiedEvent
        }
      else
        {
        // AbdoNav fiducial list already exists, thus retrieve it
        fiducialList = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(anode->GetRegistrationFiducialListID()));
        }

      // transform XY mouse coordinates into RAS coordinates
      //
      // first, find out in which slice view the user clicked
      // (necessary information for the XY to RAS conversion)
      vtkSlicerSliceGUI* sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Red");
      vtkRenderWindowInteractor* rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();

      int index = 0;
      while (style != rwi->GetInteractorStyle() && index < 2)
        {
        index++;
        if (index == 1)
          {
          sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Yellow");
          }
        else
          {
          sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Green");
          }
        rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
        }

      // second, transform XY mouse coordinates into RAS coordinates
      int xyPos[2];
      rwi->GetLastEventPosition(xyPos);
      double xyVec[4] = {xyPos[0], xyPos[1], 0, 1};
      double rasVec[4];
      vtkMatrix4x4* matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
      matrix->MultiplyPoint(xyVec, rasVec);

      // determine whether or not AbdoNav's fiducial list
      // already contains a fiducial corresponding to the
      // active check button
      //
      // if so, update it; otherwise, add a new fiducial
      bool fiducialExists = false;
      for (int i = 0; i < fiducialList->GetNumberOfFiducials(); i++)
        {
        if (!strcmp(activeCheckButton.c_str(), fiducialList->GetNthFiducialLabelText(i)))
          {
          // corresponding fiducial already exists, thus update it
          // (fiducial list implementation will invoke proper event
          // and thereby trigger vtkAbdoNavGUI::UpdateGUIFromMRML())
          fiducialList->SetNthFiducialXYZ(i, rasVec[0], rasVec[1], rasVec[2]);
          fiducialExists = true;
          }
        }
      if (fiducialExists == false)
        {
        // corresponding fiducial doesn't exist yet, thus create it
        // (fiducial list implementation will invoke proper event
        // and thereby trigger vtkAbdoNavGUI::UpdateGUIFromMRML())
        fiducialList->AddFiducialWithLabelXYZSelectedVisibility(activeCheckButton.c_str(), rasVec[0], rasVec[1], rasVec[2], 1, 1);
        }
      }
    }

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  else if (this->TrackerTransformSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerTransformSelector->GetSelected());

    // only update MRML node if transform node was created by OpenIGTLinkIF
    if (tnode != NULL && tnode->GetDescription() != NULL && strcmp(tnode->GetDescription(), "Received by OpenIGTLink") == 0)
      {
      this->UpdateMRMLFromGUI();
      }
    else
      {
      vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                       this->GetApplicationGUI()->GetMainSlicerWindow(),
                                       "AbdoNav",
                                       "Selected transform node was not created by OpenIGTLinkIF!",
                                       vtkKWMessageDialog::ErrorIcon);
      this->TrackerTransformSelector->SetSelected(NULL);
      }
    }
  else if (this->Point1CheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    // mimic check button set behavior, i.e. only one check button allowed to be selected at a time
    if (this->Point1CheckButton->GetSelectedState())
      {
      this->Point2CheckButton->SelectedStateOff();
      this->Point3CheckButton->SelectedStateOff();
      }
    }
  else if (this->Point2CheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    // mimic check button set behavior, i.e. only one check button allowed to be selected at a time
    if (this->Point2CheckButton->GetSelectedState())
      {
      this->Point1CheckButton->SelectedStateOff();
      this->Point3CheckButton->SelectedStateOff();
      }
    }
  else if (this->Point3CheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    // mimic check button set behavior, i.e. only one check button allowed to be selected at a time
    if (this->Point3CheckButton->GetSelectedState())
      {
      this->Point1CheckButton->SelectedStateOff();
      this->Point2CheckButton->SelectedStateOff();
      }
    }
  else if (this->ResetRegistrationPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->Point1CheckButton->SetEnabled(true);
    this->Point1CheckButton->SelectedStateOff();
    this->Point2CheckButton->SetEnabled(true);
    this->Point2CheckButton->SelectedStateOff();
    this->Point3CheckButton->SetEnabled(true);
    this->Point3CheckButton->SelectedStateOff();
    this->PerformRegistrationPushButton->SetEnabled(true);
    // unlock fiducial list
    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
    if (fnode)
      {
      fnode->SetLocked(0);
      }
    }
  else if (this->PerformRegistrationPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    if(isnan(this->Point1REntry->GetValueAsDouble()) ||
       isnan(this->Point2REntry->GetValueAsDouble()) ||
       isnan(this->Point3REntry->GetValueAsDouble()))
      {
      vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                       this->GetApplicationGUI()->GetMainSlicerWindow(),
                                       "AbdoNav",
                                       "Invalid registration input parameters!",
                                       vtkKWMessageDialog::ErrorIcon);
      }
    else
      {
      if (this->AbdoNavLogic->PerformRegistration() == EXIT_SUCCESS)
        {
        this->Point1CheckButton->SetEnabled(false);
        this->Point2CheckButton->SetEnabled(false);
        this->Point3CheckButton->SetEnabled(false);
        this->PerformRegistrationPushButton->SetEnabled(false);
        // lock fiducial list
        vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
        if (fnode)
          {
          fnode->SetLocked(1);
          }
        }
      else
        {
        vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                         this->GetApplicationGUI()->GetMainSlicerWindow(),
                                         "AbdoNav",
                                         "Registration failed, check input parameters!",
                                         vtkKWMessageDialog::ErrorIcon);
        }
      }
    }

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  else if (this->ShowLocatorCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->ShowLocatorCheckButton->GetSelectedState();
    this->AbdoNavLogic->ToggleLocatorVisibility(checked);
    }
  else if (this->FreezeLocatorCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->FreezeLocatorCheckButton->GetSelectedState();
    this->AbdoNavLogic->ToggleLocatorFreeze(checked);
    }
  else if (this->ShowCrosshairCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->ShowCrosshairCheckButton->GetSelectedState();
    this->AbdoNavLogic->SetShowCrosshair(checked);

    // only set crosshair if not already set
    if (checked && this->AbdoNavLogic->GetCrosshair() == NULL)
      {
      vtkMRMLCrosshairNode* crosshair = this->GetApplicationGUI()->GetSlicesControlGUI()->GetCrosshairNode();
      if (crosshair)
        {
        crosshair->SetCrosshairName("AbdoNav-Crosshair");
        crosshair->SetCrosshairBehavior(vtkMRMLCrosshairNode::Normal);
        crosshair->SetCrosshairThickness(vtkMRMLCrosshairNode::Fine);
        crosshair->SetNavigation(1);
        this->AbdoNavLogic->SetCrosshair(crosshair);
        }
      }
    }
  else if (this->DrawNeedleProjectionCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->DrawNeedleProjectionCheckButton->GetSelectedState();
    this->AbdoNavLogic->SetDrawNeedleProjection(checked);

    // only set Slicer GUI if not already set
    if (checked && this->AbdoNavLogic->GetAppGUI() == NULL)
      {
      this->AbdoNavLogic->SetAppGUI(this->GetApplicationGUI());
      }
    }
  else if (this->RedSliceMenuButton->GetMenu() == vtkKWMenu::SafeDownCast(caller) && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    this->AbdoNavLogic->SetSliceDriver(0, this->RedSliceMenuButton->GetValue());
    }
  else if (this->YellowSliceMenuButton->GetMenu() == vtkKWMenu::SafeDownCast(caller) && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    this->AbdoNavLogic->SetSliceDriver(1, this->YellowSliceMenuButton->GetValue());
    }
  else if (this->GreenSliceMenuButton->GetMenu() == vtkKWMenu::SafeDownCast(caller) && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    this->AbdoNavLogic->SetSliceDriver(2, this->GreenSliceMenuButton->GetValue());
    }
  else if (this->SetLocatorAllPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->AbdoNavLogic->SetSliceDriver(0, "Locator");
    this->AbdoNavLogic->SetSliceDriver(1, "Locator");
    this->AbdoNavLogic->SetSliceDriver(2, "Locator");
    this->RedSliceMenuButton->SetValue("Locator");
    this->YellowSliceMenuButton->SetValue("Locator");
    this->GreenSliceMenuButton->SetValue("Locator");
    }
  else if (this->SetUserAllPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->AbdoNavLogic->SetSliceDriver(0, "User");
    this->AbdoNavLogic->SetSliceDriver(1, "User");
    this->AbdoNavLogic->SetSliceDriver(2, "User");
    this->RedSliceMenuButton->SetValue("User");
    this->YellowSliceMenuButton->SetValue("User");
    this->GreenSliceMenuButton->SetValue("User");
    }
  else if (this->FreezeSliceCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->FreezeSliceCheckButton->GetSelectedState();
    this->AbdoNavLogic->SetFreezeReslicing(checked);
    }
  else if (this->ObliqueCheckButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    int checked = this->ObliqueCheckButton->GetSelectedState();
    this->AbdoNavLogic->SetObliqueReslicing(checked);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessLogicEvents(vtkObject* caller, unsigned long event, void* vtkNotUsed(callData))
{
  if (this->AbdoNavLogic == vtkAbdoNavLogic::SafeDownCast(caller))
    {
    if (event == vtkAbdoNavLogic::StatusUpdateEvent)
      {
      // TODO: implement or delete (also delete AddLogicObservers() and RemoveLogicObservers())!
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    // update the GUI if an AbdoNavNode was added; an AbdoNavNode is only added
    // when the user loads a previously saved scene that contains an AbdoNavNode
    vtkMRMLAbdoNavNode* anode = vtkMRMLAbdoNavNode::SafeDownCast((vtkObject*)callData);
    if (anode != NULL)
      {
      // a new AbdoNavNode was created and added
      if (this->AbdoNavNode == NULL)
        {
        // set and observe the new node in Logic
        this->AbdoNavLogic->SetAndObserveAbdoNavNode(anode);
        // set and observe the new node in GUI
        vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, anode);
        // if an AbdoNav fiducial list is part of the loaded scene, observe it in order to update the GUI
        vtkMRMLFiducialListNode* fiducialList = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
        if (fiducialList != NULL)
          {
          // observe fiducial list in order to update the GUI whenever a fiducial is moved via drag & drop or renamed or renumbered externally via the Fiducials module
          fiducialList->AddObserver(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
          // observe fiducial list in order to update the GUI whenever a fiducial is added externally via the Fiducials module
          fiducialList->AddObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand);
          // observe fiducial list in order to update the GUI whenever a fiducial is removed externally via the Fiducials module
          fiducialList->AddObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand);
          // observe fiducial list in order to update the GUI whenever all fiducials are removed externally via the Fiducials module
          fiducialList->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand);
          // no need to observe vtkMRMLFiducialListNode::DisplayModifiedEvent or vtkMRMLFiducialListNode::FiducialIndexModifiedEvent
          }
        this->UpdateGUIFromMRML();
        }
      }

    // update the GUI if a fiducial was added externally via the Fiducials module
    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(caller);
    if (fnode != NULL && this->AbdoNavNode != NULL)
      {
      if (!strcmp(fnode->GetID(), this->AbdoNavNode->GetRegistrationFiducialListID()))
        {
        std::cout << "fiducial added" << std::endl;
        this->UpdateGUIFromMRML();
        }
      }
    }
  else if (event == vtkMRMLScene::NodeRemovedEvent)
    {
    // update the GUI if a fiducial was removed externally via the Fiducials module
    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(caller);
    if (fnode != NULL && this->AbdoNavNode != NULL)
      {
      if (!strcmp(fnode->GetID(), this->AbdoNavNode->GetRegistrationFiducialListID()))
        {
        std::cout << "fiducial removed" << std::endl;
        this->UpdateGUIFromMRML();
        }
      }
    }
  else if (event == vtkCommand::ModifiedEvent)
    {
    // update the GUI if an existing AbdoNavNode was modified
    vtkMRMLAbdoNavNode* anode = vtkMRMLAbdoNavNode::SafeDownCast(caller);
    if (anode != NULL && this->AbdoNavNode != NULL)
      {
      if (!strcmp(anode->GetID(), this->AbdoNavNode->GetID()))
        {
        this->UpdateGUIFromMRML();
        }
      }

    // update the GUI if all fiducials were removed externally via the Fiducials module
    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(caller);
    if (fnode != NULL && this->AbdoNavNode != NULL)
      {
      if (!strcmp(fnode->GetID(), this->AbdoNavNode->GetRegistrationFiducialListID()))
        {
        std::cout << "all fiducials removed" << std::endl;
        this->UpdateGUIFromMRML();
        }
      }
    }
  else if (event == vtkMRMLFiducialListNode::FiducialModifiedEvent)
    {
    // update the GUI if a fiducial was moved via drag & drop or renamed or renumbered externally via the Fiducials module
    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(caller);
    if (fnode != NULL && this->AbdoNavNode != NULL)
      {
      if (!strcmp(fnode->GetID(), this->AbdoNavNode->GetRegistrationFiducialListID()))
        {
        std::cout << "fiducial moved or renamed" << std::endl;
        this->UpdateGUIFromMRML();
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateMRMLFromGUI()
{
  // create an AbdoNavNode if none exists yet
  vtkMRMLAbdoNavNode* node = this->CheckAndCreateAbdoNavNode();

  // save old node parameters for undo mechanism
  this->AbdoNavLogic->GetMRMLScene()->SaveStateForUndo(node);

  // set new node parameters from GUI widgets:
  // make sure that only ONE modified event is invoked (if
  // at all) instead of one modified event per changed value
  int modifiedFlag = node->StartModify();
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerTransformSelector->GetSelected());
  if (tnode != NULL)
    {
    node->SetRelativeTrackingTransformID(tnode->GetID());
    }
  node->EndModify(modifiedFlag);
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateGUIFromMRML()
{
  vtkMRMLAbdoNavNode* node = this->AbdoNavNode;
  if (node != NULL)
    {
    // set GUI widgets from AbdoNav parameter node
    vtkMRMLNode* tnode = this->GetMRMLScene()->GetNodeByID(node->GetRelativeTrackingTransformID());
    this->TrackerTransformSelector->SetSelected(tnode);

    vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(node->GetRegistrationFiducialListID()));
    if (fnode != NULL)
      {
      // need to set all values to NaN in case AbdoNav's fiducial list was modified,
      // i.e. in case a fiducial was removed or renamed (unsupported identifier) ex-
      // ternally via the Fiducials module
      this->Point1REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point1AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point1SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point2REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point2AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point2SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point3REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point3AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
      this->Point3SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());

      for (int i = 0; i < fnode->GetNumberOfFiducials(); i++)
        {
        if (!strcmp("tipRAS", fnode->GetNthFiducialLabelText(i)))
          {
          float* guidanceTip = fnode->GetNthFiducialXYZ(i);
          this->Point1REntry->SetValueAsDouble(guidanceTip[0]);
          this->Point1AEntry->SetValueAsDouble(guidanceTip[1]);
          this->Point1SEntry->SetValueAsDouble(guidanceTip[2]);
          }
        else if (!strcmp("2ndRAS", fnode->GetNthFiducialLabelText(i)))
          {
          float* guidanceSecond = fnode->GetNthFiducialXYZ(i);
          this->Point2REntry->SetValueAsDouble(guidanceSecond[0]);
          this->Point2AEntry->SetValueAsDouble(guidanceSecond[1]);
          this->Point2SEntry->SetValueAsDouble(guidanceSecond[2]);
          }
        else if (!strcmp("markerRAS", fnode->GetNthFiducialLabelText(i)))
          {
          float* markerCenter = fnode->GetNthFiducialXYZ(i);
          this->Point3REntry->SetValueAsDouble(markerCenter[0]);
          this->Point3AEntry->SetValueAsDouble(markerCenter[1]);
          this->Point3SEntry->SetValueAsDouble(markerCenter[2]);
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
vtkMRMLAbdoNavNode* vtkAbdoNavGUI::CheckAndCreateAbdoNavNode()
{
  if (this->AbdoNavNode == NULL)
    {
    // no AbdoNav node present yet, thus create a new one
    vtkMRMLAbdoNavNode* node = vtkMRMLAbdoNavNode::New();
    // add the new node to this MRML scene but don't notify:
    // this way, it is known that a node added event is only
    // invoked when the user loads a previously saved scene
    // containing an AbdoNavNode
    this->GetMRMLScene()->AddNodeNoNotify(node);
    // set and observe the new node in Logic
    this->AbdoNavLogic->SetAndObserveAbdoNavNode(node);
    // set and observe the new node in GUI
    vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, node);
    node->Delete();
   }

  return this->AbdoNavNode;
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUI()
{
  // create a page
  this->UIPanel->AddPage("AbdoNav", "AbdoNav", NULL);

  // build the different GUI frames
  BuildGUIHelpFrame();
  BuildGUIRegistrationFrame();
  BuildGUINavigationFrame();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIHelpFrame()
{
  // help text
  const char* help =
    "The **AbdoNav** module is tailored to abdominal cryosurgeries for liver and kidney tumor treatment. "
    "The module helps you to set up a connection to the tracking device via the OpenIGTLinkIF module, to "
    "plan cryoprobe insertion using the Measurements module, to register tracking and scanner coordinate "
    "systems and visualizes the current cryoprobe to be inserted."
    "\n"
    "See <a>http://www.slicer.org/slicerWiki/index.php/Modules:AbdoNav-Documentation-3.6</a> for details "
    "about the module.";
  // about text
  const char* about =
    "The **AbdoNav** module was contributed by Christoph Ammann (Karlsruhe Institute of Technology, KIT) "
    "and Nobuhiko Hata, PhD (Surgical Navigation and Robotics Laboratory, SNR).";

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");
  this->BuildHelpAndAboutFrame(page, help, about);
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIRegistrationFrame()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create collapsible registration frame
  vtkSlicerModuleCollapsibleFrame* registrationFrame = vtkSlicerModuleCollapsibleFrame::New();
  registrationFrame->SetParent(page);
  registrationFrame->Create();
  registrationFrame->SetLabelText("Registration");
  registrationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                registrationFrame->GetWidgetName(),
                page->GetWidgetName());

  // create labelled frame to hold widgets for specifying the tracking information
  vtkKWFrameWithLabel* trackerFrame = vtkKWFrameWithLabel::New();
  trackerFrame->SetParent(registrationFrame->GetFrame());
  trackerFrame->Create();
  trackerFrame->SetLabelText("Specify tracking information");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", trackerFrame->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to specify the tracking information.
  //----------------------------------------------------------------
  // create selector to specify the input tracker transform node
  this->TrackerTransformSelector = vtkSlicerNodeSelectorWidget::New();
  this->TrackerTransformSelector->SetParent(trackerFrame->GetFrame());
  this->TrackerTransformSelector->Create();
  this->TrackerTransformSelector->SetLabelText("Tracker transform node:\t\t");
  this->TrackerTransformSelector->SetBalloonHelpString("Select the transform node created by OpenIGTLinkIF that holds the tracking data of the current cryoprobe relative to the guidance needle.");
  this->TrackerTransformSelector->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform"); // filter: only show vtkMRMLLinearTransformNodes
  this->TrackerTransformSelector->SetMRMLScene(this->GetMRMLScene());
  this->TrackerTransformSelector->SetNewNodeEnabled(0); // turn off user option to create new linear transform nodes
  this->TrackerTransformSelector->SetDefaultEnabled(0); // turn off autoselecting nodes
  this->TrackerTransformSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff(); // don't show indicator

  // add tracker transform selector
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->TrackerTransformSelector->GetWidgetName());

  // create labelled frame to hold widgets for identifying the guidance needle
  vtkKWFrameWithLabel* guidanceNeedleFrame = vtkKWFrameWithLabel::New();
  guidanceNeedleFrame->SetParent(registrationFrame->GetFrame());
  guidanceNeedleFrame->Create();
  guidanceNeedleFrame->SetLabelText("Identify guidance needle");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", guidanceNeedleFrame->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to identify the guidance needle tip.
  //----------------------------------------------------------------
  // create frame required to display the check button and RAS coordinate entries on the left and right side respectively
  vtkKWFrame* point1Frame = vtkKWFrame::New();
  point1Frame->SetParent(guidanceNeedleFrame->GetFrame());
  point1Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", point1Frame->GetWidgetName());

  // create check button to select the guidance needle tip
  this->Point1CheckButton = vtkKWCheckButton::New();
  this->Point1CheckButton->SetParent(point1Frame);
  this->Point1CheckButton->Create();
  this->Point1CheckButton->SetText("Identify guidance needle tip (RAS):\t");
  this->Point1CheckButton->SetBalloonHelpString("Identify the tip of the guidance needle in the CT/MR image.");
  // create entry to hold the R coordinate of the guidance needle tip
  this->Point1REntry = vtkKWEntry::New();
  this->Point1REntry->SetParent(point1Frame);
  this->Point1REntry->Create();
  this->Point1REntry->SetBalloonHelpString("Guidance needle tip, R coordinate.");
  this->Point1REntry->SetWidth(8);
  this->Point1REntry->SetReadOnly(1);
  this->Point1REntry->SetRestrictValueToDouble();
  this->Point1REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the A coordinate of the guidance needle tip
  this->Point1AEntry = vtkKWEntry::New();
  this->Point1AEntry->SetParent(point1Frame);
  this->Point1AEntry->Create();
  this->Point1AEntry->SetBalloonHelpString("Guidance needle tip, A coordinate.");
  this->Point1AEntry->SetWidth(8);
  this->Point1AEntry->SetReadOnly(1);
  this->Point1AEntry->SetRestrictValueToDouble();
  this->Point1AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the S coordinate of the guidance needle tip
  this->Point1SEntry = vtkKWEntry::New();
  this->Point1SEntry->SetParent(point1Frame);
  this->Point1SEntry->Create();
  this->Point1SEntry->SetBalloonHelpString("Guidance needle tip, S coordinate.");
  this->Point1SEntry->SetWidth(8);
  this->Point1SEntry->SetReadOnly(1);
  this->Point1SEntry->SetRestrictValueToDouble();
  this->Point1SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());

  // add check button for the guidance needle tip
  this->Script ("pack %s -side left -anchor nw  -padx 2 -pady 2", this->Point1CheckButton->GetWidgetName());
  // add RAS coordinate entries for the guidance needle tip
  this->Script ("pack %s %s %s -side right -anchor ne -padx 2 -pady 2",
                 this->Point1SEntry->GetWidgetName(),
                 this->Point1AEntry->GetWidgetName(),
                 this->Point1REntry->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to identify the second point on the guidance needle.
  //----------------------------------------------------------------
  // create frame required to display the check button and RAS coordinate entries on the left and right side respectively
  vtkKWFrame* point2Frame = vtkKWFrame::New();
  point2Frame->SetParent(guidanceNeedleFrame->GetFrame());
  point2Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", point2Frame->GetWidgetName());

  // create check button to select the second point on the guidance needle
  this->Point2CheckButton = vtkKWCheckButton::New();
  this->Point2CheckButton->SetParent(point2Frame);
  this->Point2CheckButton->Create();
  this->Point2CheckButton->SetText("Identify second point (RAS):\t\t");
  this->Point2CheckButton->SetBalloonHelpString("Identify a second point on the guidance needle in the CT/MR image.");
  // create entry to hold the R coordinate of the second point on the guidance needle
  this->Point2REntry = vtkKWEntry::New();
  this->Point2REntry->SetParent(point2Frame);
  this->Point2REntry->Create();
  this->Point2REntry->SetBalloonHelpString("Second point on guidance needle, R coordinate.");
  this->Point2REntry->SetWidth(8);
  this->Point2REntry->SetReadOnly(1);
  this->Point2REntry->SetRestrictValueToDouble();
  this->Point2REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the A coordinate of the second point on the guidance needle
  this->Point2AEntry = vtkKWEntry::New();
  this->Point2AEntry->SetParent(point2Frame);
  this->Point2AEntry->Create();
  this->Point2AEntry->SetBalloonHelpString("Second point on guidance needle, A coordinate.");
  this->Point2AEntry->SetWidth(8);
  this->Point2AEntry->SetReadOnly(1);
  this->Point2AEntry->SetRestrictValueToDouble();
  this->Point2AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the S coordinate of the second point on the guidance needle
  this->Point2SEntry = vtkKWEntry::New();
  this->Point2SEntry->SetParent(point2Frame);
  this->Point2SEntry->Create();
  this->Point2SEntry->SetBalloonHelpString("Second point on guidance needle, S coordinate.");
  this->Point2SEntry->SetWidth(8);
  this->Point2SEntry->SetReadOnly(1);
  this->Point2SEntry->SetRestrictValueToDouble();
  this->Point2SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());

  // add check button for the second point on the guidance needle
  this->Script ("pack %s -side left -anchor nw  -padx 2 -pady 2", this->Point2CheckButton->GetWidgetName());
  // add RAS coordinate entries for the second point on the guidance needle
  this->Script ("pack %s %s %s -side right -anchor ne -padx 2 -pady 2",
                 this->Point2SEntry->GetWidgetName(),
                 this->Point2AEntry->GetWidgetName(),
                 this->Point2REntry->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to identify the marker center.
  //----------------------------------------------------------------
  // create frame required to display the check button and RAS coordinate entries on the left and right side respectively
  vtkKWFrame* point3Frame = vtkKWFrame::New();
  point3Frame->SetParent(guidanceNeedleFrame->GetFrame());
  point3Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", point3Frame->GetWidgetName());

  // create check button to select the marker center
  this->Point3CheckButton = vtkKWCheckButton::New();
  this->Point3CheckButton->SetParent(point3Frame);
  this->Point3CheckButton->Create();
  this->Point3CheckButton->SetText("Identify marker center (RAS):\t\t");
  this->Point3CheckButton->SetBalloonHelpString("Identify the marker center in the CT/MR image.");
  // create entry to hold the R coordinate of the marker center
  this->Point3REntry = vtkKWEntry::New();
  this->Point3REntry->SetParent(point3Frame);
  this->Point3REntry->Create();
  this->Point3REntry->SetBalloonHelpString("Marker center, R coordinate.");
  this->Point3REntry->SetWidth(8);
  this->Point3REntry->SetReadOnly(1);
  this->Point3REntry->SetRestrictValueToDouble();
  this->Point3REntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the A coordinate of the marker center
  this->Point3AEntry = vtkKWEntry::New();
  this->Point3AEntry->SetParent(point3Frame);
  this->Point3AEntry->Create();
  this->Point3AEntry->SetBalloonHelpString("Marker center, A coordinate.");
  this->Point3AEntry->SetWidth(8);
  this->Point3AEntry->SetReadOnly(1);
  this->Point3AEntry->SetRestrictValueToDouble();
  this->Point3AEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());
  // create entry to hold the S coordinate of the marker center
  this->Point3SEntry = vtkKWEntry::New();
  this->Point3SEntry->SetParent(point3Frame);
  this->Point3SEntry->Create();
  this->Point3SEntry->SetBalloonHelpString("Marker center, S coordinate.");
  this->Point3SEntry->SetWidth(8);
  this->Point3SEntry->SetReadOnly(1);
  this->Point3SEntry->SetRestrictValueToDouble();
  this->Point3SEntry->SetValueAsDouble(std::numeric_limits<double>::quiet_NaN());

  // add check button for the marker center
  this->Script ("pack %s -side left -anchor nw  -padx 2 -pady 2", this->Point3CheckButton->GetWidgetName());
  // add RAS coordinate entries for the marker center
  this->Script ("pack %s %s %s -side right -anchor ne -padx 2 -pady 2",
                 this->Point3SEntry->GetWidgetName(),
                 this->Point3AEntry->GetWidgetName(),
                 this->Point3REntry->GetWidgetName());

  //----------------------------------------------------------------
  // Create buttons to reset and perform the registration.
  //----------------------------------------------------------------
  // create button to reset the registration
  this->ResetRegistrationPushButton = vtkKWPushButton::New();
  this->ResetRegistrationPushButton->SetParent(registrationFrame->GetFrame());
  this->ResetRegistrationPushButton->Create();
  this->ResetRegistrationPushButton->SetText("Reset Registration");
  this->ResetRegistrationPushButton->SetBalloonHelpString("Redo identification of guidance needle.");

  // add reset registration button
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", this->ResetRegistrationPushButton->GetWidgetName());

  // create button to perform the registration
  this->PerformRegistrationPushButton = vtkKWPushButton::New();
  this->PerformRegistrationPushButton->SetParent(registrationFrame->GetFrame());
  this->PerformRegistrationPushButton->Create();
  this->PerformRegistrationPushButton->SetText("Perform Registration");
  this->PerformRegistrationPushButton->SetBalloonHelpString("Perform registration based on current identification of guidance needle.");

  // add perform registration button
  this->Script("pack %s -side right -anchor ne -padx 2 -pady 2", this->PerformRegistrationPushButton->GetWidgetName());

  // clean up
  registrationFrame->Delete();
  trackerFrame->Delete();
  guidanceNeedleFrame->Delete();
  point1Frame->Delete();
  point2Frame->Delete();
  point3Frame->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUINavigationFrame()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create collapsible navigation frame
  vtkSlicerModuleCollapsibleFrame* navigationFrame = vtkSlicerModuleCollapsibleFrame::New();
  navigationFrame->SetParent(page);
  navigationFrame->Create();
  navigationFrame->SetLabelText("Navigation");
  navigationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                navigationFrame->GetWidgetName(),
                page->GetWidgetName());

  // create labelled frame to hold widgets for setting the locator display options
  vtkKWFrameWithLabel* locatorOptionsFrame = vtkKWFrameWithLabel::New();
  locatorOptionsFrame->SetParent(navigationFrame->GetFrame());
  locatorOptionsFrame->Create();
  locatorOptionsFrame->SetLabelText("Locator display options");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", locatorOptionsFrame->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to set the locator display options.
  //----------------------------------------------------------------
  // create check button to show/hide the locator
  this->ShowLocatorCheckButton = vtkKWCheckButton::New();
  this->ShowLocatorCheckButton->SetParent(locatorOptionsFrame->GetFrame());
  this->ShowLocatorCheckButton->Create();
  this->ShowLocatorCheckButton->SetText("Show locator");
  this->ShowLocatorCheckButton->SetBalloonHelpString("Show/hide the locator.");
  this->ShowLocatorCheckButton->SelectedStateOff();

  // add show locator check button
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", ShowLocatorCheckButton->GetWidgetName());

  // create scale to set the locator projection length
  this->ProjectionLengthScale = vtkKWScaleWithEntry::New();
  this->ProjectionLengthScale->SetParent(locatorOptionsFrame->GetFrame());
  this->ProjectionLengthScale->Create();
  this->ProjectionLengthScale->SetLabelText("Projection length (cm):\t\t");
  this->ProjectionLengthScale->SetBalloonHelpString("Set the projection length of the locator tip (unit: cm, resolution: 0.5 cm). Set to zero to turn projection off.");
  this->ProjectionLengthScale->GetLabel()->SetPadX(16); // indentation: align scale text label with check button text labels
  this->ProjectionLengthScale->GetEntry()->SetWidth(8);
  this->ProjectionLengthScale->GetEntry()->SetReadOnly(1);
  this->ProjectionLengthScale->SetRange(0.0, 50.0);
  this->ProjectionLengthScale->SetResolution(0.5);
  this->ProjectionLengthScale->SetValue(0.0);

  // add projection length scale
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", ProjectionLengthScale->GetWidgetName());

  // create check button to freeze/unfreeze the locator
  this->FreezeLocatorCheckButton = vtkKWCheckButton::New();
  this->FreezeLocatorCheckButton->SetParent(locatorOptionsFrame->GetFrame());
  this->FreezeLocatorCheckButton->Create();
  this->FreezeLocatorCheckButton->SetText("Freeze locator");
  this->FreezeLocatorCheckButton->SetBalloonHelpString("Freeze/unfreeze the locator.");
  this->FreezeLocatorCheckButton->SelectedStateOff();

  // create check button to show/hide the crosshair in the slice views (corresponding to the locator's tip position)
  this->ShowCrosshairCheckButton = vtkKWCheckButton::New();
  this->ShowCrosshairCheckButton->SetParent(locatorOptionsFrame->GetFrame());
  this->ShowCrosshairCheckButton->Create();
  this->ShowCrosshairCheckButton->SetText("Show crosshair");
  this->ShowCrosshairCheckButton->SetBalloonHelpString("Show/hide the crosshair.");
  this->ShowCrosshairCheckButton->SelectedStateOff();

  // create check button to draw/hide the needle projection in the slice views
  this->DrawNeedleProjectionCheckButton = vtkKWCheckButton::New();
  this->DrawNeedleProjectionCheckButton->SetParent(locatorOptionsFrame->GetFrame());
  this->DrawNeedleProjectionCheckButton->Create();
  this->DrawNeedleProjectionCheckButton->SetText("Draw needle projection");
  this->DrawNeedleProjectionCheckButton->SetBalloonHelpString("Draw/hide the needle projection.");
  this->DrawNeedleProjectionCheckButton->SelectedStateOff();

  // add freeze locator and show crosshair check buttons
  this->Script("pack %s %s %s -side top -anchor nw -padx 2 -pady 2",
                FreezeLocatorCheckButton->GetWidgetName(),
                ShowCrosshairCheckButton->GetWidgetName(),
                DrawNeedleProjectionCheckButton->GetWidgetName());

  // create labelled frame to hold widgets for setting the slice driver options
  vtkKWFrameWithLabel* sliceDriverFrame = vtkKWFrameWithLabel::New();
  sliceDriverFrame->SetParent(navigationFrame->GetFrame());
  sliceDriverFrame->Create();
  sliceDriverFrame->SetLabelText("Slice driver options");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", sliceDriverFrame->GetWidgetName());

  //----------------------------------------------------------------
  // Create menu buttons to set the slice driver for each slice orientation separately.
  //----------------------------------------------------------------
  // create frame required to center the menu buttons
  vtkKWFrame* sliceOrientationFrame = vtkKWFrame::New();
  sliceOrientationFrame->SetParent(sliceDriverFrame->GetFrame());
  sliceOrientationFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", sliceOrientationFrame->GetWidgetName());

  // get slice orientation colors red, green and yellow
  vtkSlicerColor* color = ((vtkSlicerApplication*)this->GetApplication())->GetSlicerTheme()->GetSlicerColors();
  // create menu button to set the driver for the red (axial) slice orientation
  this->RedSliceMenuButton = vtkKWMenuButton::New();
  this->RedSliceMenuButton->SetParent(sliceOrientationFrame);
  this->RedSliceMenuButton->Create();
  this->RedSliceMenuButton->SetBalloonHelpString("Set driver for the axial slice orientation.");
  this->RedSliceMenuButton->SetWidth(13);
  this->RedSliceMenuButton->SetBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->RedSliceMenuButton->SetValue("User");
  // create menu button to set the driver for the yellow (sagittal) slice orientation
  this->YellowSliceMenuButton = vtkKWMenuButton::New();
  this->YellowSliceMenuButton->SetParent(sliceOrientationFrame);
  this->YellowSliceMenuButton->Create();
  this->YellowSliceMenuButton->SetBalloonHelpString("Set driver for the sagittal slice orientation.");
  this->YellowSliceMenuButton->SetWidth(13);
  this->YellowSliceMenuButton->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->YellowSliceMenuButton->SetValue("User");
  // create menu button to set the driver for the green (coronal) slice orientation
  this->GreenSliceMenuButton = vtkKWMenuButton::New();
  this->GreenSliceMenuButton->SetParent(sliceOrientationFrame);
  this->GreenSliceMenuButton->Create();
  this->GreenSliceMenuButton->SetBalloonHelpString("Set driver for the coronal slice orientation.");
  this->GreenSliceMenuButton->SetWidth(13);
  this->GreenSliceMenuButton->SetBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->GreenSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->GreenSliceMenuButton->SetValue("User");

  // add red, green and yellow slice orientation slice driver menu buttons
  this->Script("pack %s %s %s -side left -anchor nw -fill x -padx 2 -pady 2",
                RedSliceMenuButton->GetWidgetName(),
                YellowSliceMenuButton->GetWidgetName(),
                GreenSliceMenuButton->GetWidgetName());

  //----------------------------------------------------------------
  // Create widgets to set the slice driver for all slice orientations at once and to set the reslicing options.
  //----------------------------------------------------------------
  // create frame required to center the widgets
  vtkKWFrame* sliceOptionsFrame = vtkKWFrame::New();
  sliceOptionsFrame->SetParent(sliceDriverFrame->GetFrame());
  sliceOptionsFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", sliceOptionsFrame->GetWidgetName());

  // create button to drive all slice orientations by the locator
  this->SetLocatorAllPushButton = vtkKWPushButton::New();
  this->SetLocatorAllPushButton->SetParent(sliceOptionsFrame);
  this->SetLocatorAllPushButton->Create();
  this->SetLocatorAllPushButton->SetText("Locator all");
  this->SetLocatorAllPushButton->SetBalloonHelpString("Drive all slice orientations by the locator.");
  this->SetLocatorAllPushButton->SetWidth(12);
  // create button to drive all slice orientations by the user
  this->SetUserAllPushButton = vtkKWPushButton::New();
  this->SetUserAllPushButton->SetParent(sliceOptionsFrame);
  this->SetUserAllPushButton->Create();
  this->SetUserAllPushButton->SetText("User all");
  this->SetUserAllPushButton->SetBalloonHelpString("Drive all slice orientations by the user.");
  this->SetUserAllPushButton->SetWidth(12);
  // create check button to freeze/unfreeze reslicing if driven by the locator
  this->FreezeSliceCheckButton = vtkKWCheckButton::New();
  this->FreezeSliceCheckButton->SetParent(sliceOptionsFrame);
  this->FreezeSliceCheckButton->Create();
  this->FreezeSliceCheckButton->SetText("Freeze");
  this->FreezeSliceCheckButton->SetBalloonHelpString("Freeze/unfreeze reslicing if driven by the locator.");
  this->FreezeSliceCheckButton->SelectedStateOff();
  // create check button to enable/disable oblique reslicing if driven by the locator
  this->ObliqueCheckButton = vtkKWCheckButton::New();
  this->ObliqueCheckButton->SetParent(sliceOptionsFrame);
  this->ObliqueCheckButton->Create();
  this->ObliqueCheckButton->SetText("Oblique");
  this->ObliqueCheckButton->SetBalloonHelpString("Enable/disable oblique reslicing if driven by the locator.");
  this->ObliqueCheckButton->SelectedStateOff();

  // add drive all slice orientations by locator/user buttons, freeze reslicing and oblique reslicing check buttons
  this->Script("pack %s %s %s %s -side left -anchor nw -fill x -padx 2 -pady 2",
                SetLocatorAllPushButton->GetWidgetName(),
                SetUserAllPushButton->GetWidgetName(),
                FreezeSliceCheckButton->GetWidgetName(),
                ObliqueCheckButton->GetWidgetName());

  // clean up
  navigationFrame->Delete();
  locatorOptionsFrame->Delete();
  sliceDriverFrame->Delete();
  sliceOrientationFrame->Delete();
  sliceOptionsFrame->Delete();
}
