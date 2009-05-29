#ifndef __vtkTRProstateBiopsySecondaryMonitor_h
#define __vtkTRProstateBiopsySecondaryMonitor_h

// for getting display device information
#include "Windows.h"
#include "vtkTRProstateBiopsyWin32Header.h"

#include "vtkSmartPointer.h"
#include "vtkTRProstateBiopsyViewerWidget.h"
#include "vtkSlicerFiducialListWidget.h"
#include "vtkSlicerROIViewerWidget.h"

class vtkTRProstateBiopsyGUI;

#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"
class vtkTRProstateBiopsyTargetDescriptor;
class vtkTRProstateBiopsyRobotWidget;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsySecondaryMonitor : 
  public vtkKWTopLevel
{
public:
  static vtkTRProstateBiopsySecondaryMonitor *New();  
  vtkTypeRevisionMacro(vtkTRProstateBiopsySecondaryMonitor,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DisplayOnSecondaryMonitor();

  void SetCurrentTarget(vtkTRProstateBiopsyTargetDescriptor* target);

protected:

  void UpdateSecondaryMonitorPoisition();
  virtual void CreateWidget();

  vtkTRProstateBiopsySecondaryMonitor();
  ~vtkTRProstateBiopsySecondaryMonitor();  

  vtkKWFrame* MainFrame;

  vtkTRProstateBiopsyViewerWidget* ViewerWidget;
  vtkSlicerFiducialListWidget* FiducialListWidget;  
  vtkSlicerROIViewerWidget* ROIViewerWidget;
  vtkTRProstateBiopsyRobotWidget* RobotViewerWidget;

  bool MultipleMonitorsAvailable; 
  int SecondaryMonitorPosition[2]; // virtual screen position in pixels
  int SecondaryMonitorSize[2]; // virtual screen size in pixels

  //BTX
  vtkTRProstateBiopsyTargetDescriptor* CurrentTarget;
  //ETX

private:
  vtkTRProstateBiopsySecondaryMonitor(const vtkTRProstateBiopsySecondaryMonitor&);
  void operator=(const vtkTRProstateBiopsySecondaryMonitor&);
};

#endif
