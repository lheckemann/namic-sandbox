#ifndef __vtkProstateNavConfigurationStep_h
#define __vtkProstateNavConfigurationStep_h

#include "vtkProstateNavStep.h"
#include "vtkCommand.h"

class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrame;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWLabel;

class VTK_PROSTATENAV_EXPORT vtkProstateNavConfigurationStep :
  public vtkProstateNavStep
{
public:
  static vtkProstateNavConfigurationStep *New();
  vtkTypeRevisionMacro(vtkProstateNavConfigurationStep,vtkProstateNavStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

protected:
  vtkProstateNavConfigurationStep();
  ~vtkProstateNavConfigurationStep();

  // GUI Widgets
  vtkKWFrame *RobotFrame;
  vtkKWLabel *RobotLabel1;
  vtkKWLabel *RobotLabel2;
  vtkKWEntry *RobotAddressEntry;
  vtkKWEntry *RobotPortEntry;
  vtkKWPushButton *RobotConnectButton;
  
  vtkKWFrame *ScannerFrame;
  vtkKWLabel *ScannerLabel1;
  vtkKWLabel *ScannerLabel2;
  vtkKWEntry *ScannerAddressEntry;
  vtkKWEntry *ScannerPortEntry;
  vtkKWPushButton *ScannerConnectButton;
  
private:
  vtkProstateNavConfigurationStep(const vtkProstateNavConfigurationStep&);
  void operator=(const vtkProstateNavConfigurationStep&);
};

#endif
