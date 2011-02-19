#ifndef __vtkPerkStationPlanStep_h
#define __vtkPerkStationPlanStep_h

#include "vtkPerkStationStep.h"

class vtkKWCheckButtonWithLabel;
class vtkKWLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkKWPushButton;

#include "vtkActor.h"
#include "vtkSmartPointer.h"


/**
 * GUI wizard panel for the planning workphase.
 */
class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationPlanStep 
: public vtkPerkStationStep
{
public:

  static vtkPerkStationPlanStep *New();
  vtkTypeRevisionMacro( vtkPerkStationPlanStep,vtkPerkStationStep );
  void PrintSelf( ostream& os, vtkIndent indent );

  
    // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();
  virtual void ShowTargetFirstFrame();
  virtual void ShowPlanListFrame();
  virtual void HideUserInterface();
  
  
    // Callbacks.
  void ProcessImageClickEvents( vtkObject *caller, unsigned long event, void *callData );
  void ProcessMouseMoveEvent( vtkObject *caller, unsigned long event, void *callData );
  
    // Process GUI events
  virtual void ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData );
  
  
  void SetInGUICallbackFlag( int flag ) {
    this->InGUICallbackFlag = flag;
  }
  vtkGetMacro( InGUICallbackFlag, int );
  
  virtual void Reset();
  
  virtual void UpdateGUI();
  
  
  void OnMultiColumnListUpdate( int row, int col, char * str );
  void OnMultiColumnListSelectionChanged();
  void OnSliceOffsetChanged( double offset );
  
  virtual void OverlayNeedleGuide();
  void RemoveOverlayNeedleGuide();
  
  
protected:
  
  vtkPerkStationPlanStep();
  ~vtkPerkStationPlanStep();
  
  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );

  vtkPerkStationPlanStep( const vtkPerkStationPlanStep& );
  void operator=( const vtkPerkStationPlanStep& );
  
  void AddGUIObservers();
  void RemoveGUIObservers();
  vtkCallbackCommand *GUICallbackCommand;
  
  
  // GUI callback  
  static void WizardGUICallback( vtkObject *caller, unsigned long event,
                                 void *clientData, void *callData );

  virtual void InstallCallbacks();

  virtual void Validate();

  bool DoubleEqual( double val1, double val2 );
  

    // GUI widgets ------------------------------------------------------------
  
  vtkKWFrame* TargetFirstFrame;
  vtkKWCheckButtonWithLabel* TargetFirstCheck;
  
  
  vtkKWFrame* PlanListFrame;
  vtkKWMultiColumnListWithScrollbars* PlanList;
  vtkKWPushButton* DeleteButton;
  
  vtkKWFrame *TiltInformationFrame;
  vtkKWEntryWithLabel *SystemTiltAngle;
  

  //BTX
  vtkSmartPointer< vtkActor > PlanningLineActor;
  //ETX
  
    // World coordinates for display.
  
  double WCEntryPoint[ 3 ];
  double WCTargetPoint[ 3 ];

  vtkActor* NeedleActor;

private:
    
    // State description.
  
  int InGUICallbackFlag;
  
  unsigned int NumPointsSelected;
  bool ProcessingCallback;
  bool SelectTargetFirst;

};

#endif
