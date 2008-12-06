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

#include "vtkOpenIGTLinkIFLogic.h"


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

  // pointer to OpenIGTLinkIF Logic
  vtkOpenIGTLinkIFLogic* OpenIGTLinkIFLogic;
  

  // GUI Widgets
  vtkKWFrame *ConfigNTFrame;
  vtkKWFrame *ConnectNTFrame;
  vtkKWLoadSaveButtonWithLabel *LoadConfigButtonNT;
  vtkKWEntry *ConfigFileEntryNT;
  vtkKWCheckButton *ConnectCheckButtonNT;


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
