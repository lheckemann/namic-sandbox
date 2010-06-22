
#ifndef __vtkStereoCalibViewerWidget_h
#define __vtkStereoCalibViewerWidget_h



// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include "vtkStereoCalibWin32Header.h"

#include "vtkKWTopLevel.h"
#include "vtkKWRenderWidget.h"
#include "vtkRenderer.h"


class VTK_StereoCalib_EXPORT vtkStereoCalibViewerWidget :
  public vtkKWTopLevel
{
public:
  static vtkStereoCalibViewerWidget *New();  
  vtkTypeRevisionMacro(vtkStereoCalibViewerWidget ,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
  void DisplayOnSecondaryMonitor();
  // vtkKWRenderWidget* ViewerWidget; // 
  vtkKWRenderWidget* lw;       // 100603-komura
  vtkKWRenderWidget* rw;
  vtkKWRenderWidget* mw;        // 100616-komura for mixture
  void changeSecondaryMonitorSize(int, int); // 100622-komura

protected:
  vtkStereoCalibViewerWidget();
  ~vtkStereoCalibViewerWidget(); 
  virtual void CreateWidget();
  void UpdateSecondaryMonitorPoisition();

  vtkKWFrame* MainFrame; 
  int  SecondaryMonitorSize[2]; // virtual screen size in pixels
private:
  vtkStereoCalibViewerWidget(const vtkStereoCalibViewerWidget&);
  void operator=(const vtkStereoCalibViewerWidget&);
};

#endif
