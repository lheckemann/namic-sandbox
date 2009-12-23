#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"

#include "vtkSlicerDiffusionRSHGlyphDisplayWidget.h"

#include "vtkKWFrameWithLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWScale.h"
#include "vtkKWMenuButton.h"
#include "vtkKWCheckButton.h"

#include "vtkMRMLDiffusionRSHDisplayPropertiesNode.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerDiffusionRSHGlyphDisplayWidget );
vtkCxxRevisionMacro ( vtkSlicerDiffusionRSHGlyphDisplayWidget, "$Revision: 1.0 $");


//---------------------------------------------------------------------------
vtkSlicerDiffusionRSHGlyphDisplayWidget::vtkSlicerDiffusionRSHGlyphDisplayWidget ( )
  {

  this->DiffusionRSHDisplayPropertiesNodeID = NULL;

  this->Frame = NULL;
  
  this->GlyphGeometryMenu = NULL;
  
  this->GlyphMinMaxNormalizationButton = NULL;
  this->GlyphScale = NULL;
  this->GlyphResolutionScale = NULL;
  this->GlyphAngularResolutionScale = NULL;

  }


//---------------------------------------------------------------------------
vtkSlicerDiffusionRSHGlyphDisplayWidget::~vtkSlicerDiffusionRSHGlyphDisplayWidget ( )
  {
  this->RemoveMRMLObservers();
  this->RemoveWidgetObservers();

  if (this->GlyphGeometryMenu)
    {
    this->GlyphGeometryMenu->SetParent(NULL);
    this->GlyphGeometryMenu->Delete();
    this->GlyphGeometryMenu = NULL;
    }
    
  if (this->GlyphMinMaxNormalizationButton)
    {
    this->GlyphMinMaxNormalizationButton->SetParent(NULL);
    this->GlyphMinMaxNormalizationButton->Delete();
    this->GlyphMinMaxNormalizationButton = NULL;
    }

  if (this->GlyphScale)
    {
    this->GlyphScale->SetParent(NULL);
    this->GlyphScale->Delete();
    this->GlyphScale = NULL;
    }

  if (this->GlyphResolutionScale)
    {
    this->GlyphResolutionScale->SetParent(NULL);
    this->GlyphResolutionScale->Delete();
    this->GlyphResolutionScale = NULL;
    }

  if (this->GlyphAngularResolutionScale)
    {
    this->GlyphAngularResolutionScale->SetParent(NULL);
    this->GlyphAngularResolutionScale->Delete();
    this->GlyphAngularResolutionScale = NULL;
    }

  if (this->Frame)
    {
    this->Frame->SetParent(NULL);
    this->Frame->Delete();
    this->Frame = NULL;
    }

  this->SetMRMLScene ( NULL );
  this->SetDiffusionRSHDisplayPropertiesNodeID (NULL);

  }


//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::PrintSelf ( ostream& os, vtkIndent indent )
  {
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "vtkSlicerDiffusionRSHGlyphDisplayWidget: " << this->GetClassName ( ) << "\n";
  os << indent << "DiffusionRSHDisplayPropertiesNode ID: " << this->GetDiffusionRSHDisplayPropertiesNodeID() << "\n";

  // print widgets?
  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::SetDiffusionRSHDisplayPropertiesNode ( vtkMRMLDiffusionRSHDisplayPropertiesNode *node )
  {
  if (node == NULL)
    {
    this->AddMRMLObservers();
    return;
    }
  vtkDebugWithObjectMacro(this,"Setting DTI Display props node" <<  node->GetID());

  // 
  // Set the member variables and do a first process
  //
  // Stop observing the old node
  this->RemoveMRMLObservers();

  this->SetDiffusionRSHDisplayPropertiesNodeID( node->GetID() );

  // Observe the display node. It may be used for various types of actual (data) nodes.
  // Keeping track of these is handled by widgets using this widget.
  // Start observing the new node
  this->AddMRMLObservers();

  if ( node )
    {
    this->ProcessMRMLEvents(node, vtkCommand::ModifiedEvent, NULL);
    }
  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::ProcessWidgetEvents ( vtkObject *caller,
                                                                      unsigned long event, void *callData )
  {

  vtkMRMLDiffusionRSHDisplayPropertiesNode *displayNode = 
    vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(
    this->DiffusionRSHDisplayPropertiesNodeID));
  if (displayNode == NULL)
    {
    vtkDebugWithObjectMacro(this,"Process WIDGET... Events, NO display node!");
    return;
    }

  // process glyph geometry menu events
  vtkKWMenu *glyphMenuButton = vtkKWMenu::SafeDownCast(caller);
  if (glyphMenuButton == this->GlyphGeometryMenu->GetWidget()->GetMenu())
    vtkDebugWithObjectMacro(this,"Process WIDGET... Events, glyph menu event!" << event);

  if (glyphMenuButton == this->GlyphGeometryMenu->GetWidget()->GetMenu() && 
    event == vtkKWMenu::MenuItemInvokedEvent)
    {
    displayNode->SetGlyphGeometry(this->GlyphGeometryMap[std::string(this->GlyphGeometryMenu->GetWidget()->GetValue())]);
    return;
    }

  // process glyph scale events
  if (vtkKWScale::SafeDownCast(caller) == this->GlyphScale->GetWidget() && 
    event == vtkKWScale::ScaleValueChangedEvent)
    {
    displayNode->SetGlyphScaleFactor(this->GlyphScale->GetWidget()->GetValue());
    return;
    }

  // process glyph resolution events
  if ( vtkKWScale::SafeDownCast(caller) == this->GlyphResolutionScale->GetWidget() && 
    event == vtkKWScale::ScaleValueChangedEvent)
    {
    displayNode->SetSpatialGlyphResolution((int)(this->GlyphResolutionScale->GetWidget()->GetValue()));
    return;
    }

  // process glyph angular resolution events
  if ( vtkKWScale::SafeDownCast(caller) == this->GlyphAngularResolutionScale->GetWidget() && 
    event == vtkKWScale::ScaleValueChangedEvent)
    {
    displayNode->SetAngularGlyphResolution((int)(this->GlyphAngularResolutionScale->GetWidget()->GetValue()));
    return;
    }
    
  // process Min Max Button!
  if (this->GlyphMinMaxNormalizationButton == vtkKWCheckButton::SafeDownCast(caller) && 
    event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      std::cerr << "RSHGlyphDisplayWidget: Norm setGlyph : " << this->GlyphMinMaxNormalizationButton->GetSelectedState() << std::endl;
      displayNode->SetGlyphMinMaxNormalization( this->GlyphMinMaxNormalizationButton->GetSelectedState() );
      return;
    }

  }


//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::ProcessMRMLEvents ( vtkObject *caller,
                                                                    unsigned long event, void *callData )
  {

  if (!this->DiffusionRSHDisplayPropertiesNodeID )
    {
    vtkDebugWithObjectMacro(this,"Process MRML Events null display node ");
    return;
    }

  vtkMRMLDiffusionRSHDisplayPropertiesNode *node = vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(caller);

  // if this event comes from our node, it is not null, and has been modified
  if (node == this->MRMLScene->GetNodeByID(this->DiffusionRSHDisplayPropertiesNodeID) && 
    node != NULL && event == vtkCommand::ModifiedEvent)
    {
    vtkDebugWithObjectMacro(this,"Process MRML Events updating widget ");

    this->UpdateWidget();
    }
  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::AddMRMLObservers ( )
  {


  if ( !this->DiffusionRSHDisplayPropertiesNodeID )
    {
    return;
    }

  vtkMRMLDiffusionRSHDisplayPropertiesNode *node = vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(this->DiffusionRSHDisplayPropertiesNodeID));

  if (node != NULL)
    {
    node->AddObserver(vtkCommand::ModifiedEvent,
      (vtkCommand *)this->MRMLCallbackCommand );      
    }

  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::RemoveMRMLObservers ( )
  {
  if ( !this->DiffusionRSHDisplayPropertiesNodeID )
    {
    return;
    }

  vtkMRMLDiffusionRSHDisplayPropertiesNode *node = vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(this->DiffusionRSHDisplayPropertiesNodeID));

  if (node != NULL)
    {
    node->RemoveObservers(vtkCommand::ModifiedEvent,
      (vtkCommand *)this->MRMLCallbackCommand );    
    }
  }

// called from process MRML events
//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::UpdateWidget()
  {

  if ( this->DiffusionRSHDisplayPropertiesNodeID )
    {

    vtkMRMLDiffusionRSHDisplayPropertiesNode *displayNode = 
      vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(
      this->DiffusionRSHDisplayPropertiesNodeID));
    if (displayNode != NULL) 
      {
      this->GlyphGeometryMenu->GetWidget()->SetValue(displayNode->GetGlyphGeometryAsString());

      this->GlyphMinMaxNormalizationButton->SetSelectedState(displayNode->GetGlyphMinMaxNormalization());

      this->GlyphScale->GetWidget()->SetValue(displayNode->GetGlyphScaleFactor());

      this->GlyphResolutionScale->GetWidget()->SetValue(displayNode->GetSpatialGlyphResolution());

      this->GlyphAngularResolutionScale->GetWidget()->SetValue(displayNode->GetGlyphThetaResolution());

      } 
    else 
      {
      vtkDebugWithObjectMacro(this,"Update widget null display node  ;) " << this->DiffusionRSHDisplayPropertiesNodeID);
      }

    return;
    }
  }

// TO DO: is this used?
//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::UpdateMRML()
  {

  if ( this->DiffusionRSHDisplayPropertiesNodeID )
    {
    vtkMRMLDiffusionRSHDisplayPropertiesNode *displayNode = 
      vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(
      this->DiffusionRSHDisplayPropertiesNodeID));
    if (displayNode != NULL) 
      {
      displayNode->SetGlyphGeometry(this->GlyphGeometryMap[std::string(this->GlyphGeometryMenu->GetWidget()->GetValue())]);
      displayNode->SetGlyphScaleFactor(this->GlyphScale->GetWidget()->GetValue());
      }
    else 
      {
      vtkDebugWithObjectMacro(this,"Update mrml null display node  ;) " << this->DiffusionRSHDisplayPropertiesNodeID);
      }

    return;
    }
  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::AddWidgetObservers ( ) {

  //this->Superclass::AddWidgetObservers();

  this->GlyphGeometryMenu->GetWidget()->GetMenu()->AddObserver (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphResolutionScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphAngularResolutionScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphMinMaxNormalizationButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  }

//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::RemoveWidgetObservers ( ) {

  //this->Superclass::RemoveWidgetObservers();

  this->GlyphGeometryMenu->GetWidget()->GetMenu()->RemoveObservers (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->GlyphScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphResolutionScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphAngularResolutionScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->GlyphMinMaxNormalizationButton->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  }


//---------------------------------------------------------------------------
void vtkSlicerDiffusionRSHGlyphDisplayWidget::CreateWidget ( )
  {
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }

  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  //Create dummy display properties node to init variables
  vtkMRMLDiffusionRSHDisplayPropertiesNode *propNode = 
    vtkMRMLDiffusionRSHDisplayPropertiesNode::New();

  // ---
  // DISPLAY FRAME            
  this->Frame = vtkKWFrameWithLabel::New ( );
  this->Frame->SetParent ( this->GetParent() );
  this->Frame->Create ( );
  this->Frame->SetLabelText ( "Glyph Display Properties" );
  //this->Frame->CollapseFrame ( );

  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
    this->Frame->GetWidgetName() );

  // glyph geometry menu
  vtkKWMenuButtonWithLabel *glyphMenuButton = 
    vtkKWMenuButtonWithLabel::New();

  this->GlyphGeometryMenu = glyphMenuButton;
  glyphMenuButton->SetParent( this->Frame->GetFrame() );
  glyphMenuButton->Create();

  // initialize glyph geometry menu
  int initIdx = propNode->GetFirstGlyphGeometry();
  int endIdx = propNode->GetLastGlyphGeometry();
  int currentVal = propNode->GetGlyphGeometry();
  this->GlyphGeometryMap.clear();
  int k;
  for (k=initIdx ; k<=endIdx ; k++)
    {
    propNode->SetGlyphGeometry(k);
    const char *tag = propNode->GetGlyphGeometryAsString();
    this->GlyphGeometryMap[std::string(tag)]=k;
    glyphMenuButton->GetWidget()->GetMenu()->AddRadioButton(tag);
    }
  // init to class default value
  propNode->SetGlyphGeometry(currentVal);
  glyphMenuButton->GetWidget()->SetValue(propNode->GetGlyphGeometryAsString());

  // pack glyph menu
  glyphMenuButton->SetLabelText("Glyph Type");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
    glyphMenuButton->GetWidgetName());

  this->GlyphMinMaxNormalizationButton = vtkKWCheckButton::New();
  this->GlyphMinMaxNormalizationButton->SetParent(this->Frame->GetFrame());
  this->GlyphMinMaxNormalizationButton->Create();
  this->GlyphMinMaxNormalizationButton->SelectedStateOn();
  this->GlyphMinMaxNormalizationButton->SetText("Min Max Normalization");
  this->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->GlyphMinMaxNormalizationButton->GetWidgetName());

  this->GlyphScale = vtkKWScaleWithLabel::New();
  this->GlyphScale->SetParent ( this->Frame->GetFrame() );
  this->GlyphScale->Create ( );
  this->GlyphScale->SetLabelText("Scale Factor");
  this->GlyphScale->GetWidget()->SetRange(0,200);
  this->GlyphScale->GetWidget()->SetResolution(1);
  this->GlyphScale->SetBalloonHelpString("set glyph scale value.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
    this->GlyphScale->GetWidgetName() );

  this->GlyphResolutionScale = vtkKWScaleWithLabel::New();
  this->GlyphResolutionScale->SetParent ( this->Frame->GetFrame() );
  this->GlyphResolutionScale->Create ( );
  this->GlyphResolutionScale->SetLabelText("Spacing");
  this->GlyphResolutionScale->GetWidget()->SetRange(1,50);
  this->GlyphResolutionScale->GetWidget()->SetResolution(1);
  this->GlyphResolutionScale->SetBalloonHelpString("skip step for glyphs.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
    this->GlyphResolutionScale->GetWidgetName() );

  this->GlyphAngularResolutionScale = vtkKWScaleWithLabel::New();
  this->GlyphAngularResolutionScale->SetParent ( this->Frame->GetFrame() );
  this->GlyphAngularResolutionScale->Create ( );
  this->GlyphAngularResolutionScale->SetLabelText("Glyph Detail");
  this->GlyphAngularResolutionScale->GetWidget()->SetRange(1,50);
  this->GlyphAngularResolutionScale->GetWidget()->SetResolution(1);
  this->GlyphAngularResolutionScale->SetBalloonHelpString("Angular resolution for glyphs. (Num theta)");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
    this->GlyphAngularResolutionScale->GetWidgetName() );


  // ---
  // ADVANCED FRAME            
  vtkKWFrameWithLabel *advFrame = vtkKWFrameWithLabel::New ( );
  advFrame->SetParent ( this->Frame->GetFrame() );
  advFrame->Create ( );
  advFrame->SetLabelText ( "Advanced" );
  advFrame->CollapseFrame ( );

  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
    advFrame->GetWidgetName() );

  // ---
  // Ellipsoids FRAME            
  vtkKWFrameWithLabel *ellipseFrame = vtkKWFrameWithLabel::New ( );
  ellipseFrame->SetParent ( advFrame->GetFrame() );
  ellipseFrame->Create ( );
  ellipseFrame->SetLabelText ( "Ellipsoid Glyph Display" );
  ellipseFrame->CollapseFrame ( );

  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
    ellipseFrame->GetWidgetName() );


  // add observers
  this->AddWidgetObservers();

  ellipseFrame->Delete();
  advFrame->Delete();
  propNode->Delete();
  }

void vtkSlicerDiffusionRSHGlyphDisplayWidget::SetGlyphResolution(int value)
  {
   if ( this->DiffusionRSHDisplayPropertiesNodeID )
    {
    vtkMRMLDiffusionRSHDisplayPropertiesNode *displayNode = 
      vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast(this->MRMLScene->GetNodeByID(
      this->DiffusionRSHDisplayPropertiesNodeID));
    if (displayNode != NULL) 
      {
  this->GlyphResolutionScale->GetWidget()->SetValue(value);
      }
     }
  }

