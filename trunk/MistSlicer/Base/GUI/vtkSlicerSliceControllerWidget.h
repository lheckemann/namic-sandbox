#ifndef __vtkSlicerSliceControllerWidget_h
#define __vtkSlicerSliceControllerWidget_h

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkKWCompositeWidget.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWMenuButtonWithSpinButtonsWithLabel.h"
#include "vtkKWMenuButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWFrame.h"
#include "vtkKWTopLevel.h"
#include "vtkKWEntry.h"

#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerWidget.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerSlicesControlIcons.h"
#include "vtkSlicerToolbarIcons.h"
#include "vtkSlicerVisibilityIcons.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLSliceCompositeNode.h"

#include "vtkSlicerSliceLogic.h"

class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerSliceControllerWidget : public vtkSlicerWidget
{
    
public:
  static vtkSlicerSliceControllerWidget* New ( );
  vtkTypeRevisionMacro ( vtkSlicerSliceControllerWidget, vtkKWCompositeWidget );
  void PrintSelf (ostream& os, vtkIndent indent);
    
  // Description:
  // Get/Set the Widgets in this composite widget.
  vtkGetObjectMacro ( OffsetScale, vtkKWScaleWithEntry );
  vtkGetObjectMacro ( OrientationSelector, vtkKWMenuButtonWithSpinButtonsWithLabel );
  vtkGetObjectMacro ( ForegroundSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( BackgroundSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( LabelSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( VisibilityToggle, vtkKWPushButton );
  vtkGetObjectMacro ( LoadDataButton, vtkKWPushButton );
  vtkGetObjectMacro ( LinkButton, vtkKWPushButton );
  vtkGetObjectMacro ( VisibilityIcons, vtkSlicerVisibilityIcons );
  vtkGetObjectMacro ( ViewConfigureIcons, vtkSlicerToolbarIcons );
  vtkGetObjectMacro ( SliceControlIcons, vtkSlicerSlicesControlIcons);
  vtkGetObjectMacro ( ScaleFrame, vtkKWFrame );
  vtkGetObjectMacro ( IconFrame, vtkKWFrame );
  vtkGetObjectMacro ( LabelOpacityButton, vtkKWPushButton );
  vtkGetObjectMacro ( LabelOpacityScale, vtkKWScaleWithEntry );
  vtkGetObjectMacro ( LabelOpacityToggleButton, vtkKWPushButton );
  vtkGetObjectMacro ( LabelOpacityTopLevel, vtkKWTopLevel );
  vtkGetObjectMacro ( LabelOutlineToggleButton, vtkKWPushButton );
  vtkGetObjectMacro ( LightboxTopLevel, vtkKWTopLevel );
  vtkGetObjectMacro ( FitToWindowButton, vtkKWPushButton );
  vtkGetObjectMacro ( VolumeDisplayMenuButton, vtkKWMenuButton );
  vtkGetObjectMacro ( LightboxButton, vtkKWMenuButton );
  vtkGetObjectMacro ( LightboxRowsEntry, vtkKWEntry );
  vtkGetObjectMacro ( LightboxColumnsEntry, vtkKWEntry );
  vtkGetObjectMacro ( LightboxApplyButton, vtkKWPushButton);
  vtkGetObjectMacro ( OrientationMenuButton, vtkKWMenuButton );
  vtkGetObjectMacro ( ForegroundMenuButton, vtkKWMenuButton );
  vtkGetObjectMacro ( BackgroundMenuButton, vtkKWMenuButton );
  vtkGetObjectMacro ( LabelMenuButton, vtkKWMenuButton );
  vtkGetObjectMacro ( MoreMenuButton, vtkKWMenuButton );
  vtkGetMacro ( LightboxRows, int );
  vtkSetMacro ( LightboxRows, int );
  vtkGetMacro ( LightboxColumns, int );  
  vtkSetMacro ( LightboxColumns, int );

  void RemoveWidgetObservers ( );
  void AddWidgetObservers ( );
  
  // Description:
  // Get/Set the Nodes
  vtkGetObjectMacro ( SliceCompositeNode, vtkMRMLSliceCompositeNode );
  void SetSliceCompositeNode (vtkMRMLSliceCompositeNode *scnode)
    {
    vtkSetAndObserveMRMLNodeMacro(this->SliceCompositeNode, scnode );
    }
  vtkGetObjectMacro ( SliceNode, vtkMRMLSliceNode );
  void SetSliceNode (vtkMRMLSliceNode *snode)
    {
    vtkSetAndObserveMRMLNodeMacro(this->SliceNode, snode );
    }

  // Description:
  // updates FG and BG layer menus when Slice Logic is updated.
  virtual void UpdateLayerMenus();

  // Description:
  // slice logic controlling the slice to be manipulated
  vtkGetObjectMacro ( SliceLogic, vtkSlicerSliceLogic );
  void SetAndObserveSliceLogic (vtkSlicerSliceLogic *sLogic)
    {
    // treat the logic as a MRML node (it will be passing on changes to volumes and layers)
    vtkSetAndObserveMRMLNodeMacro(this->SliceLogic, sLogic );
    }

  // Description:
  // TODO: Use this flag to determine how to display
  // the SliceControllerWidget.
  vtkGetMacro ( ControllerStyle, int );
  vtkSetMacro ( ControllerStyle, int );
  
  // Description:
  // respond to events from subwidgets of this widget
  void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // respond to changes in the mrml scene
  void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // respond to changes in the slice logic
  void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Description:
  // applies a background color to the controller
  virtual void ApplyColorCode ( double *c );
  virtual void ToggleSlicesLink  ( );
  virtual void LinkAllSlices();
  virtual void UnlinkAllSlices();
  virtual int AllSlicesLinked ( );
  virtual void HideLabelOpacityScaleAndEntry ( );
  virtual void HideLabelOpacityScaleAndEntry ( int link );
  virtual void PopUpLabelOpacityScaleAndEntry ( int link );
  virtual void PopUpLabelOpacityScaleAndEntry ( );
  virtual void HideLightboxCustomLayoutFrame ( );
  virtual void PopUpLightboxCustomLayoutFrame ( );
  
  // Description:
  // Shrink/Expand the widget
  virtual void Shrink();
  virtual void Expand();

  virtual void UpdateOrientation ( int link );
  virtual void UpdateForegroundLayer ( int link );
  virtual void UpdateBackgroundLayer ( int link );
  virtual void UpdateLabelLayer ( int link );
  //virtual void RaiseVolumeDisplayPanel ( char *id );
  virtual void FitSliceToBackground ( int link );
  virtual void FitSliceToBackground ( );
  virtual void ToggleReformatWidget ( int link );
  virtual void ToggleReformatWidget ( );
  virtual void ToggleLabelOutline ( int link );
  virtual void ToggleLabelOutline ( );

  // Description:
  // Configure the slice viewer layout in linked mode for all CompareX slice viewers
  // This only works in CompareView layout mode
  virtual void SliceViewerLayoutConfig(int nRows, int nColumns);

  //BTX
  enum 
  {
    ExpandEvent = 15000,
    ShrinkEvent
  };
  //ETX

protected:
  vtkSlicerSliceControllerWidget ( );
  virtual ~vtkSlicerSliceControllerWidget ( );

  // Description:
  // Create the widget.
  virtual void CreateWidget( );

  // TODO: hook this up
  int ControllerStyle;

  vtkKWFrame *ContainerFrame;

  //
  // Slice controller subwidgets
  //
  vtkKWScaleWithEntry *OffsetScale;
  vtkKWMenuButtonWithSpinButtonsWithLabel *OrientationSelector;
  vtkSlicerNodeSelectorWidget *ForegroundSelector;
  vtkSlicerNodeSelectorWidget *BackgroundSelector;
  vtkSlicerNodeSelectorWidget *LabelSelector;
  
  //
  // MenuButtons next to selectors with drop-down
  // menu of options for each layer.
  vtkKWMenuButton *OrientationMenuButton;
  vtkKWMenuButton *ForegroundMenuButton;
  vtkKWMenuButton *BackgroundMenuButton;
  vtkKWMenuButton *LabelMenuButton;

  vtkKWPushButton *VisibilityToggle;
  vtkKWPushButton *LoadDataButton;
  vtkKWPushButton *LinkButton;
  vtkKWPushButton *LabelOpacityButton;
  vtkKWScaleWithEntry *LabelOpacityScale;
  vtkKWPushButton *LabelOpacityToggleButton;
  vtkKWTopLevel *LabelOpacityTopLevel;
  vtkKWPushButton *LabelOutlineToggleButton;

  vtkKWTopLevel *LightboxTopLevel;
  vtkSlicerVisibilityIcons *VisibilityIcons;
  vtkSlicerToolbarIcons *ViewConfigureIcons;
  vtkSlicerSlicesControlIcons *SliceControlIcons;
  vtkKWFrame *ScaleFrame;
  vtkKWFrame *IconFrame;
  vtkKWPushButton *ColorCodeButton;
  vtkKWPushButton *FitToWindowButton;
  vtkKWMenuButton *VolumeDisplayMenuButton;
  vtkKWMenuButton *LightboxButton;
  vtkKWEntry *LightboxRowsEntry;
  vtkKWEntry *LightboxColumnsEntry;
  vtkKWPushButton *LightboxApplyButton;
  vtkKWMenuButton *MoreMenuButton;

  int LightboxRows;
  int LightboxColumns;

  float LastLabelOpacity;
  
  //
  // Nodes
  //
  vtkMRMLSliceNode *SliceNode;
  vtkMRMLSliceCompositeNode *SliceCompositeNode;

  //
  // Logic
  //
  vtkSlicerSliceLogic *SliceLogic;


private:
  vtkSlicerSliceControllerWidget (const vtkSlicerSliceControllerWidget &); //Not implemented
  void operator=(const vtkSlicerSliceControllerWidget &);         //Not implemented

};

#endif

