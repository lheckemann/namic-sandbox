#include <string>
#include <iostream>
#include <sstream>

#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkObjectFactory.h"
#include "vtkTractographyVisualizationGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerNodeSelectorWidget.h"


//------------------------------------------------------------------------------
vtkTractographyVisualizationGUI* vtkTractographyVisualizationGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
      vtkObjectFactory::CreateInstance("vtkTractographyVisualizationGUI");
  if(ret)
  {
    return (vtkTractographyVisualizationGUI*)ret;
  }

  // If the factory was unable to create the object, then create it here.
  return new vtkTractographyVisualizationGUI;
}


//------------------------------------------------------------------------------
vtkTractographyVisualizationGUI::vtkTractographyVisualizationGUI()
{
  this->Logic = NULL;
  this->TractographyVisualizationNode = NULL;

  this->NodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->InputModelSelector = vtkSlicerNodeSelectorWidget::New();
  this->Threshold = vtkKWScaleWithEntry::New();
  this->OutputModelSelector = vtkSlicerNodeSelectorWidget::New();
  this->ValueTypeMenu = vtkKWMenuButtonWithLabel::New();
  this->RemovalModeMenu = vtkKWMenuButtonWithLabel::New();
  this->LoadButton = vtkKWPushButton::New();
  this->SaveButton = vtkKWPushButton::New();
  this->TubeButton = vtkKWCheckButtonWithLabel::New();
  this->RecordFaButton = vtkKWCheckButtonWithLabel::New();
  this->RecordRaButton = vtkKWCheckButtonWithLabel::New();
  this->RecordTraceButton = vtkKWCheckButtonWithLabel::New();
  this->RecordAvgFaButton = vtkKWCheckButtonWithLabel::New();

  this->oldInputModelNode = NULL;
  this->inputNodeAvailable = false;
}


//------------------------------------------------------------------------------
vtkTractographyVisualizationGUI::~vtkTractographyVisualizationGUI()
{
  if (this->NodeSelector)
  {
    this->NodeSelector->SetParent(NULL);
    this->NodeSelector->Delete();
    this->NodeSelector = NULL;
  }
  if (this->InputModelSelector)
  {
    this->InputModelSelector->SetParent(NULL);
    this->InputModelSelector->Delete();
    this->InputModelSelector = NULL;
  }
  if (this->Threshold)
  {
    this->Threshold->SetParent(NULL);
    this->Threshold->Delete();
    this->Threshold = NULL;
  }
  if (this->OutputModelSelector)
  {
    this->OutputModelSelector->SetParent(NULL);
    this->OutputModelSelector->Delete();
    this->OutputModelSelector = NULL;
  }
  if (this->LoadButton)
  {
    this->LoadButton->SetParent(NULL);
    this->LoadButton->Delete();
    this->LoadButton = NULL;
  }
  if (this->SaveButton)
  {
    this->SaveButton->SetParent(NULL);
    this->SaveButton->Delete();
    this->SaveButton = NULL;
  }
  if (this->ValueTypeMenu)
  {
    this->ValueTypeMenu->SetParent(NULL);
    this->ValueTypeMenu->Delete();
    this->ValueTypeMenu = NULL;
  }
  if (this->RemovalModeMenu)
  {
    this->RemovalModeMenu->SetParent(NULL);
    this->RemovalModeMenu->Delete();
    this->RemovalModeMenu = NULL;
  }
  if (this->TubeButton)
  {
    this->TubeButton->SetParent(NULL);
    this->TubeButton->Delete();
    this->TubeButton = NULL;
  }
  if (this->RecordFaButton)
  {
    this->RecordFaButton->SetParent(NULL);
    this->RecordFaButton->Delete();
    this->RecordFaButton = NULL;
  }
  if (this->RecordRaButton)
  {
    this->RecordRaButton->SetParent(NULL);
    this->RecordRaButton->Delete();
    this->RecordRaButton = NULL;
  }
  if (this->RecordTraceButton)
  {
    this->RecordTraceButton->SetParent(NULL);
    this->RecordTraceButton->Delete();
    this->RecordTraceButton = NULL;
  }
  if (this->RecordAvgFaButton)
  {
    this->RecordAvgFaButton->SetParent(NULL);
    this->RecordAvgFaButton->Delete();
    this->RecordAvgFaButton = NULL;
  }

  this->SetLogic(NULL);
  vtkSetMRMLNodeMacro(this->TractographyVisualizationNode, NULL);
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::PrintSelf(ostream& os, vtkIndent indent)
{

}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::AddGUIObservers() 
{
  this->Threshold->AddObserver(vtkKWScale::ScaleValueChangingEvent,
                               (vtkCommand *)this->GUICallbackCommand);
  this->Threshold->AddObserver(vtkKWScale::ScaleValueStartChangingEvent,
                               (vtkCommand *)this->GUICallbackCommand);
  this->Threshold->AddObserver(vtkKWScale::ScaleValueChangedEvent,
                               (vtkCommand *)this->GUICallbackCommand);

  this->NodeSelector->AddObserver(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  
  this->InputModelSelector->AddObserver(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  
  this->OutputModelSelector->AddObserver(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  

  this->LoadButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                (vtkCommand *)this->GUICallbackCommand);
  this->SaveButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                (vtkCommand *)this->GUICallbackCommand);

  this->ValueTypeMenu->GetWidget()->GetMenu()->AddObserver(
      vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->RemovalModeMenu->GetWidget()->GetMenu()->AddObserver(
      vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->TubeButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent,
      (vtkCommand *)this->GUICallbackCommand);
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::RemoveGUIObservers()
{
  this->Threshold->RemoveObservers(vtkKWScale::ScaleValueChangingEvent,
                                   (vtkCommand *)this->GUICallbackCommand);
  this->Threshold->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent,
                                   (vtkCommand *)this->GUICallbackCommand);
  this->Threshold->RemoveObservers(vtkKWScale::ScaleValueChangedEvent,
                                   (vtkCommand *)this->GUICallbackCommand);

  this->NodeSelector->RemoveObservers(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  
  this->InputModelSelector->RemoveObservers(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  
  this->OutputModelSelector->RemoveObservers(
      vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
      (vtkCommand *)this->GUICallbackCommand);  

  this->LoadButton->RemoveObservers(vtkKWPushButton::InvokedEvent,
                                    (vtkCommand *)this->GUICallbackCommand);
  this->SaveButton->RemoveObservers(vtkKWPushButton::InvokedEvent,
                                    (vtkCommand *)this->GUICallbackCommand);

  this->ValueTypeMenu->GetWidget()->GetMenu()->RemoveObservers(
      vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->RemovalModeMenu->GetWidget()->GetMenu()->RemoveObservers(
      vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->TubeButton->GetWidget()->RemoveObservers(
      vtkKWCheckButton::SelectedStateChangedEvent,
      (vtkCommand *)this->GUICallbackCommand);
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::ProcessGUIEvents(vtkObject *caller,
                                                       unsigned long event,
                                                       void *callData) 
{
  //vtkstd::cout << "gui proc events" << vtkstd::endl;
  vtkKWScaleWithEntry *s = vtkKWScaleWithEntry::SafeDownCast(caller);
  vtkKWPushButton *b = vtkKWPushButton::SafeDownCast(caller);
  vtkKWCheckButton *c = vtkKWCheckButton::SafeDownCast(caller);
  vtkSlicerNodeSelectorWidget *selector =
      vtkSlicerNodeSelectorWidget::SafeDownCast(caller);
  vtkKWMenu *m = vtkKWMenu::SafeDownCast(caller);


  if (s == this->Threshold && (event == vtkKWScale::ScaleValueChangedEvent ||
      event == vtkKWScale::ScaleValueChangingEvent))
  {
    //vtkstd::cout << "threshold" << vtkstd::endl;

    if (this->InputModelSelector->GetSelected())
    {
      if (this->inputNodeAvailable)
      {
        this->UpdateMRML();
        this->Logic->Apply();
      }
    }
  }
  else if (selector == this->InputModelSelector &&
           event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent &&
           this->InputModelSelector->GetSelected()) 
  { 
    //vtkstd::cout << "inputmodelselect" << vtkstd::endl;
    if (this->InputModelSelector->GetSelected() != this->oldInputModelNode)
    {
      this->LoadButton->EnabledOn();

      this->oldInputModelNode = this->InputModelSelector->GetSelected();
      this->OutputModelSelector->EnabledOff();
      this->ValueTypeMenu->EnabledOff();
      this->RemovalModeMenu->EnabledOff();
      this->Threshold->EnabledOff();
      this->SaveButton->EnabledOff();
      this->TubeButton->EnabledOff();

      this->inputNodeAvailable = false;
      this->Threshold->SetValue(0.0);

      // Make sure that it's not the temp node.
      if (this->InputModelSelector->GetSelected() == this->Logic->GetTempNode())
      {
        this->LoadButton->EnabledOff();
        return;
      }

      this->UpdateMRML();

      this->Logic->CleanUp();
    }
  }
  else if (selector == this->OutputModelSelector &&
           event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent &&
           this->OutputModelSelector->GetSelected()) 
  { 
    //vtkstd::cout << "outputmodelselect" << vtkstd::endl;
    this->UpdateMRML();

    if ((this->InputModelSelector->GetSelected() !=
         this->OutputModelSelector->GetSelected()) &&
        (this->OutputModelSelector->GetSelected() !=
         this->Logic->GetTempNode())) {
      this->SaveButton->EnabledOn();
    } else {
      this->SaveButton->EnabledOff();
    }
  }
  else  if (selector == this->NodeSelector &&
            event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent &&
            this->NodeSelector->GetSelected()) 
  { 
    //vtkstd::cout << "node" << vtkstd::endl;
    vtkMRMLTractographyVisualizationNode* n =
        this->GetTractographyVisualizationNode();
    this->Logic->SetAndObserveTractographyVisualizationNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->TractographyVisualizationNode, n);
    this->UpdateGUI();
  }
  else if (b == this->LoadButton && event == vtkKWPushButton::InvokedEvent)
  {
    //vtkstd::cout << "load" << vtkstd::endl;
    if (this->InputModelSelector->GetSelected())
    {
      if (this->Logic->CreateTempNode())
      {
        this->LoadButton->EnabledOff();

        this->Logic->Apply();

        this->OutputModelSelector->EnabledOn();

        // Activate the FA option only if FA is stored with the nodel
        if (this->GetLogic()->GetFaAvailable())
        {
          this->ValueTypeMenu->EnabledOn();
        }

        this->RemovalModeMenu->EnabledOn();
        this->Threshold->EnabledOn();

        if ((this->InputModelSelector->GetSelected() !=
             this->OutputModelSelector->GetSelected()) &&
            (this->OutputModelSelector->GetSelected() !=
             this->Logic->GetTempNode())) {
          this->SaveButton->EnabledOn();
        }

        this->TubeButton->EnabledOn();
        this->inputNodeAvailable = true;
      }
    }
    else
    {
      vtkErrorMacro("No input model node selected.");
    }
  }
  else if (b == this->SaveButton && event == vtkKWPushButton::InvokedEvent)
  {
    bool rec_fa = this->RecordFaButton->GetWidget()->GetSelectedState();
    bool rec_ra = this->RecordRaButton->GetWidget()->GetSelectedState();
    bool rec_trace = this->RecordTraceButton->GetWidget()->GetSelectedState();
    bool rec_avg_fa = this->RecordAvgFaButton->GetWidget()->GetSelectedState();
    this->Logic->Reduce(rec_fa, rec_ra, rec_trace, rec_avg_fa);
  }
  else if (c == this->TubeButton->GetWidget() &&
           event == vtkKWCheckButton::SelectedStateChangedEvent)
  {
    //vtkstd::cout << "tubes" << vtkstd::endl;
    this->UpdateMRML();
    if (GetTractographyVisualizationNode()->GetShowTubes())
    {
      this->Threshold->EnabledOff();
    }
    else
    {
      this->Threshold->EnabledOn();
    }
    this->Logic->ToggleTubeDisplay();
    this->Logic->Apply();
  }
  else if (m == this->RemovalModeMenu->GetWidget()->GetMenu() &&
           event == vtkKWMenu::MenuItemInvokedEvent)
  {
    //vtkstd::cout << "mode change" << vtkstd::endl;
    this->UpdateMRML();
    this->Logic->Apply();
  }
  else if (m == this->ValueTypeMenu->GetWidget()->GetMenu() &&
           event == vtkKWMenu::MenuItemInvokedEvent)
  {
    //vtkstd::cout << "value type change" << vtkstd::endl;
    this->UpdateMRML();
    this->Logic->SetValueType();
    this->Logic->Apply();
  }

  else
  {
    //vtkstd::cout << "else" << vtkstd::endl;
    if (!(s == this->Threshold &&
        event == vtkKWScale::ScaleValueStartChangingEvent)) 
    {
      vtkErrorMacro("Unexpected Event.");
    }
  }
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::UpdateMRML()
{
  //vtkstd::cout << "update" << vtkstd::endl;
  vtkMRMLTractographyVisualizationNode* n =
      this->GetTractographyVisualizationNode();
  if (n == NULL)
  {
    // No parameter node selected yet, create new.
    this->NodeSelector->SetSelectedNew("vtkMRMLTractographyVisualizationNode");
    this->NodeSelector->ProcessNewNodeCommand(
        "vtkMRMLTractographyVisualizationNode", "Parameters");
    n = vtkMRMLTractographyVisualizationNode::SafeDownCast(
        this->NodeSelector->GetSelected());

    // Set an observe new node in Logic.
    this->Logic->SetAndObserveTractographyVisualizationNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->TractographyVisualizationNode, n);
  }

  // Save node parameters for Undo.
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);

  n->SetThreshold(this->Threshold->GetValue());
  
  if (this->InputModelSelector->GetSelected())
  {
    n->SetInputModelRef(this->InputModelSelector->GetSelected()->GetID());
  }
  if (this->OutputModelSelector->GetSelected())
  {
    n->SetOutputModelRef(this->OutputModelSelector->GetSelected()->GetID());
  }

  n->SetShowTubes(this->TubeButton->GetWidget()->GetSelectedState());

  if (!strcmp(this->ValueTypeMenu->GetWidget()->GetValue(), "Covariance norm"))
  {
    n->SetValueType(0);
  }
  else if (!strcmp(this->ValueTypeMenu->GetWidget()->GetValue(), "FA"))
  {
    n->SetValueType(1);
  }

  if (!strcmp(this->RemovalModeMenu->GetWidget()->GetValue(), "Segments only"))
  {
    n->SetRemovalMode(0);
  }
  else if (!strcmp(this->RemovalModeMenu->GetWidget()->GetValue(),
                   "Complete fibers"))
  {
    n->SetRemovalMode(1);
  }

}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::UpdateGUI()
{
  //vtkstd::cout << "update gui" << vtkstd::endl;
  vtkMRMLTractographyVisualizationNode* n =
      this->GetTractographyVisualizationNode();
  if (n)
  {
    //this->Threshold->SetValue(n->GetThreshold());
    //this->SetThresholdSlider(n->GetThresholdMin(), n->GetThresholdMax(),
    //                         std::max(n->GetThreshold(), n->GetThresholdMin()));
  }
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::ProcessMRMLEvents(vtkObject *caller,
                                                        unsigned long event,
                                                        void *callData) 
{
  //vtkstd::cout << "process mrml" << vtkstd::endl;
  // If parameter node has been changed externally, update GUI widgets with
  // new values.
  vtkMRMLTractographyVisualizationNode* node =
      vtkMRMLTractographyVisualizationNode::SafeDownCast(caller);
  if (node && this->GetTractographyVisualizationNode() == node) 
  {
    this->SetThresholdSlider(node->GetThresholdMin(), node->GetThresholdMax(),
                             std::max(node->GetThreshold(),
                             node->GetThresholdMin()));
  }
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::BuildGUI()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkMRMLTractographyVisualizationNode* gadNode =
      vtkMRMLTractographyVisualizationNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(gadNode);
  gadNode->Delete();

  this->UIPanel->AddPage("TractographyVisualization",
                         "TractographyVisualization", NULL);
  // Module GUI frame.
  const char *help = "The Tractography Visualization module can be used to "
      "display fibers that were obtained by different mutli-tensor "
      "tractography methods. There's automatic false positive detections for "
      "fibers that are unlikely to exists. It's also possible to calculate "
      "statistic measurements (FA, etc.) on the fibers.";
  const char *about = "Written by SL.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget("TractographyVisualization");
  this->BuildHelpAndAboutFrame(page, help, about);
    
  // A frame for the visualization details and the fiber removal.
  vtkSlicerModuleCollapsibleFrame *moduleFrame =
      vtkSlicerModuleCollapsibleFrame::New();
  moduleFrame->SetParent(
      this->UIPanel->GetPageWidget("TractographyVisualization"));
  moduleFrame->Create();
  moduleFrame->SetLabelText("Fiber Visualization");
  moduleFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
              moduleFrame->GetWidgetName(), this->UIPanel->GetPageWidget(
              "TractographyVisualization")->GetWidgetName());
  
  this->NodeSelector->SetNodeClass("vtkMRMLTractographyVisualizationNode", NULL,
                                   NULL, "Parameters");
  this->NodeSelector->SetNewNodeEnabled(1);
  this->NodeSelector->NoneEnabledOff();
  this->NodeSelector->SetShowHidden(1);
  this->NodeSelector->SetParent(moduleFrame->GetFrame());
  this->NodeSelector->Create();
  this->NodeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->NodeSelector->UpdateMenu();
  this->NodeSelector->SetWidth(400);
  this->NodeSelector->SetLabelText("Parameters");
  this->NodeSelector->SetBalloonHelpString("Select a parameter node from the "
                                           "current mrml scene.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
              this->NodeSelector->GetWidgetName());

  this->InputModelSelector->SetNodeClass("vtkMRMLModelNode", NULL, NULL, NULL);
  this->InputModelSelector->SetParent(moduleFrame->GetFrame());
  this->InputModelSelector->Create();
  this->InputModelSelector->SetMRMLScene(
      this->GetApplicationLogic()->GetMRMLScene());
  this->InputModelSelector->UpdateMenu();
  this->InputModelSelector->SetWidth(400);
  this->InputModelSelector->SetLabelText("Input Model");
  this->InputModelSelector->SetBalloonHelpString("Select a input model.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4",
              this->InputModelSelector->GetWidgetName());

  this->LoadButton->SetParent(moduleFrame->GetFrame());
  this->LoadButton->Create();
  this->LoadButton->EnabledOff();
  this->LoadButton->SetText("Load");
  this->LoadButton->SetWidth(12);
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
              this->LoadButton->GetWidgetName());

  this->ValueTypeMenu->SetParent(moduleFrame->GetFrame());
  this->ValueTypeMenu->Create();
  this->ValueTypeMenu->EnabledOff();
  this->ValueTypeMenu->SetLabelText("Slider value");
  this->ValueTypeMenu->GetWidget()->GetMenu()->AddRadioButton("Covariance norm");
  this->ValueTypeMenu->GetWidget()->GetMenu()->AddRadioButton("FA");
  this->ValueTypeMenu->GetWidget()->SetValue("Covariance norm");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
               this->ValueTypeMenu->GetWidgetName());

  this->RemovalModeMenu->SetParent(moduleFrame->GetFrame());
  this->RemovalModeMenu->Create();
  this->RemovalModeMenu->EnabledOff();
  this->RemovalModeMenu->SetLabelText("Fiber Removal Mode");
  this->RemovalModeMenu->GetWidget()->GetMenu()->AddRadioButton("Segments only");
  this->RemovalModeMenu->GetWidget()->GetMenu()->AddRadioButton("Complete fibers");
  this->RemovalModeMenu->GetWidget()->SetValue("Only Segments");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
               this->RemovalModeMenu->GetWidgetName());

  this->TubeButton->SetParent(moduleFrame->GetFrame());
  this->TubeButton->Create();
  this->TubeButton->EnabledOff();
  this->TubeButton->SetLabelText("Display tubes");
  this->TubeButton->SetBalloonHelpString("The fibers are shown as tubes if "
                                         "activated.");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4",
               this->TubeButton->GetWidgetName());

  this->Threshold->SetParent(moduleFrame->GetFrame());
  this->Threshold->SetLabelText("Threshold");
  this->Threshold->Create();
  int width = this->Threshold->GetScale()->GetWidth();
  this->Threshold->GetScale()->SetWidth(width);
  this->Threshold->SetRange(0.0, 100.0);
  this->Threshold->EnabledOff();
  this->Threshold->SetValue(0.0);
  this->Threshold->SetResolution (0.25);
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4 -fill x", 
              this->Threshold->GetWidgetName());

  moduleFrame->Delete();

  // Another frame for statistical analysis.
  moduleFrame = vtkSlicerModuleCollapsibleFrame::New();
  moduleFrame->SetParent(
      this->UIPanel->GetPageWidget("TractographyVisualization"));
  moduleFrame->Create();
  moduleFrame->SetLabelText("Reduce and Calculate Statistics");
  moduleFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
              moduleFrame->GetWidgetName(), this->UIPanel->GetPageWidget(
              "TractographyVisualization")->GetWidgetName());

  this->OutputModelSelector->SetNodeClass("vtkMRMLModelNode", NULL, NULL, NULL);
  this->OutputModelSelector->SetNewNodeEnabled(1);
  this->OutputModelSelector->SetParent(moduleFrame->GetFrame());
  this->OutputModelSelector->Create();
  this->OutputModelSelector->SetMRMLScene(this->GetMRMLScene());
  this->OutputModelSelector->UpdateMenu();
  this->OutputModelSelector->SetLabelText("Output Model");
  this->OutputModelSelector->SetBalloonHelpString("Select a output model.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4",
              this->OutputModelSelector->GetWidgetName());

  this->RecordFaButton->SetParent(moduleFrame->GetFrame());
  this->RecordFaButton->Create();
  this->RecordFaButton->SetLabelText("Record FA");
  this->RecordFaButton->SetBalloonHelpString("Record FA");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4",
               this->RecordFaButton->GetWidgetName());

  this->RecordRaButton->SetParent(moduleFrame->GetFrame());
  this->RecordRaButton->Create();
  this->RecordRaButton->SetLabelText("Record RA");
  this->RecordRaButton->SetBalloonHelpString("Record RA");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4",
               this->RecordRaButton->GetWidgetName());

  this->RecordTraceButton->SetParent(moduleFrame->GetFrame());
  this->RecordTraceButton->Create();
  this->RecordTraceButton->SetLabelText("Record trace");
  this->RecordTraceButton->SetBalloonHelpString("Record Trace");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4",
               this->RecordTraceButton->GetWidgetName());

  this->RecordAvgFaButton->SetParent(moduleFrame->GetFrame());
  this->RecordAvgFaButton->Create();
  this->RecordAvgFaButton->SetLabelText("Record average FA");
  this->RecordAvgFaButton->SetBalloonHelpString("Record average FA");
  this->Script("pack %s -side top -anchor e -padx 20 -pady 4",
               this->RecordAvgFaButton->GetWidgetName());

  this->SaveButton->SetParent(moduleFrame->GetFrame());
  this->SaveButton->Create();
  this->SaveButton->SetText("Render");
  this->SaveButton->SetWidth(12);
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
              this->SaveButton->GetWidgetName());

  moduleFrame->Delete();
}

//------------------------------------------------------------------------------
void vtkTractographyVisualizationGUI::SetThresholdSlider(float min, float max,
                                                         float value)
{
  //vtkstd::cout << "set slider thres" << vtkstd::endl;
  bool tmp = this->inputNodeAvailable;
  this->inputNodeAvailable = false;

  this->Threshold->SetRange(min, max);
  this->Threshold->SetValue(value);

  this->inputNodeAvailable = tmp;
}
