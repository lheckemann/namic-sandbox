#ifndef __vtkSlicerSecondaryViewerWindow_h
#define __vtkSlicerSecondaryViewerWindow_h



// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include "vtkSecondaryWindowWin32Header.h"


#include "vtkSmartPointer.h"
//#include "vtkSlicerFiducialListWidget.h"
//#include "vtkSlicerROIViewerWidget.h"
#include "vtkSlicerSecondaryViewerWidget.h"

#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

class VTK_SecondaryWindow_EXPORT vtkSlicerSecondaryViewerWindow : 
  public vtkKWTopLevel
{
public:
  static vtkSlicerSecondaryViewerWindow *New();  
  vtkTypeRevisionMacro(vtkSlicerSecondaryViewerWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DisplayOnSecondaryMonitor();

  //void SetCurrentTarget(vtkTRProstateBiopsyTargetDescriptor* target);

protected:

  void UpdateSecondaryMonitorPoisition();
  virtual void CreateWidget();

  vtkSlicerSecondaryViewerWindow();
  ~vtkSlicerSecondaryViewerWindow();  

  vtkKWFrame* MainFrame;

  vtkSlicerSecondaryViewerWidget* ViewerWidget;
  //vtkSlicerFiducialListWidget* FiducialListWidget;  
  //vtkSlicerROIViewerWidget* ROIViewerWidget;
  //vtkTRProstateBiopsyRobotWidget* RobotViewerWidget;

  bool MultipleMonitorsAvailable; 
  int  SecondaryMonitorPosition[2]; // virtual screen position in pixels
  int  SecondaryMonitorSize[2]; // virtual screen size in pixels

  //BTX
  //vtkTRProstateBiopsyTargetDescriptor* CurrentTarget;
  //ETX

private:
  vtkSlicerSecondaryViewerWindow(const vtkSlicerSecondaryViewerWindow&);
  void operator=(const vtkSlicerSecondaryViewerWindow&);
};

#endif
