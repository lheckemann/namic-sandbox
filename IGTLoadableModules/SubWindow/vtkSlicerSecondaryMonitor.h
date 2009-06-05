#ifndef __vtkSlicerSecondaryMonitor_h
#define __vtkSlicerSecondaryMonitor_h



// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include "vtkSubWindowWin32Header.h"


#include "vtkSmartPointer.h"
#include "vtkSlicerFiducialListWidget.h"
#include "vtkSlicerROIViewerWidget.h"

#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

class VTK_SubWindow_EXPORT vtkSlicerSecondaryMonitor : 
  public vtkKWTopLevel
{
public:
  static vtkSlicerSecondaryMonitor *New();  
  vtkTypeRevisionMacro(vtkSlicerSecondaryMonitor,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DisplayOnSecondaryMonitor();

  //void SetCurrentTarget(vtkTRProstateBiopsyTargetDescriptor* target);

protected:

  void UpdateSecondaryMonitorPoisition();
  virtual void CreateWidget();

  vtkSlicerSecondaryMonitor();
  ~vtkSlicerSecondaryMonitor();  

  vtkKWFrame* MainFrame;

  //vtkTRProstateBiopsyViewerWidget* ViewerWidget;
  vtkSlicerFiducialListWidget* FiducialListWidget;  
  vtkSlicerROIViewerWidget* ROIViewerWidget;
  //vtkTRProstateBiopsyRobotWidget* RobotViewerWidget;

  bool MultipleMonitorsAvailable; 
  int SecondaryMonitorPosition[2]; // virtual screen position in pixels
  int SecondaryMonitorSize[2]; // virtual screen size in pixels

  //BTX
  //vtkTRProstateBiopsyTargetDescriptor* CurrentTarget;
  //ETX

private:
  vtkSlicerSecondaryMonitor(const vtkSlicerSecondaryMonitor&);
  void operator=(const vtkSlicerSecondaryMonitor&);
};

#endif
