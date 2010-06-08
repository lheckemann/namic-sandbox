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
  vtkTypeRevisionMacro(vtkPerkStationPlanStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Reimplement the superclass's method (see vtkKWWizardStep).
  
  virtual void ShowUserInterface();
  virtual void ShowTargetFirstFrame();
  virtual void ShowPlanListFrame();
  

    // Description:
    // Callbacks.
  void ProcessImageClickEvents( vtkObject *caller,
                                unsigned long event,
                                void *callData );
  
  void ProcessMouseMoveEvent( vtkObject *caller,
                              unsigned long event,
                              void *callData );
  
  
    // Description:
    // Process GUI events
  virtual void ProcessGUIEvents( vtkObject *caller,
                                 unsigned long event,
                                 void *callData );
  
  
  virtual void OverlayNeedleGuide();
  
  virtual void Reset();
  
  void RemoveOverlayNeedleGuide();
  
  void PopulateControlsOnLoadPlanning();

protected:
  
  vtkPerkStationPlanStep();
  ~vtkPerkStationPlanStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  
  // Description:
  // GUI callback  
  static void WizardGUICallback( vtkObject *caller,
                                 unsigned long event,
                                 void *clientData,
                                 void *callData );

  virtual void InstallCallbacks();

  virtual void Validate();

  void ResetControls();
  bool DoubleEqual( double val1, double val2 );
  

  // GUI widgets --------------------------------------------------------------
  
  vtkKWFrame* TargetFirstFrame;
  vtkKWCheckButtonWithLabel* TargetFirstCheck;
  
  // entry point RAS
  // information to be had from the user
  
  vtkKWFrame *EntryPointFrame;
  vtkKWLabel *EntryPointLabel;
  vtkKWEntrySet      *EntryPoint;
  
  vtkKWFrame*    TargetPointFrame;
  vtkKWLabel*    TargetPointLabel;  
  vtkKWEntrySet* TargetPoint;
  
  vtkKWFrame* PlanListFrame;
  vtkKWMultiColumnListWithScrollbars* PlanList;
  vtkKWPushButton* DeleteButton;
  
  vtkKWFrame *TiltInformationFrame;
  vtkKWEntryWithLabel *SystemTiltAngle;
  

  //BTX
  vtkSmartPointer< vtkActor > PlanningLineActor;
  //ETX
  
  double WCEntryPoint[3];
  double WCTargetPoint[3];

  bool EntryTargetAcquired;
  unsigned int ClickNumber;
  vtkActor *NeedleActor;
private:
  bool ProcessingCallback;
  bool SelectTargetFirst;

  vtkPerkStationPlanStep( const vtkPerkStationPlanStep& );
  void operator=( const vtkPerkStationPlanStep& );
};

#endif
