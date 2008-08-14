#ifndef __vtkPerkStationSecondaryMonitor_h
#define __vtkPerkStationSecondaryMonitor_h

#include "vtkPerkStationModule.h"
// for getting display device information
#include "Windows.h"

class vtkPerkStationModuleGUI;
class vtkSlicerSliceViewer;
class vtkImageData;
class vtkImageReslice;
class vtkMatrix4x4;
class vtkTransform;
class vtkMRMLScalarVolumeNode;
class vtkKWFrame;
class vtkWin32OpenGLRenderWindow;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageMapper;
class vtkActor2D;
class vtkKWRenderWidget;

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationSecondaryMonitor
{
public:
  static vtkPerkStationSecondaryMonitor *New();  

  // Description: 
  // Get/Set GUI
  vtkPerkStationModuleGUI* GetGUI(){return this->GUI;};
  virtual void SetGUI(vtkPerkStationModuleGUI *gui){ this->GUI = gui;};

  // About the secondary monitor itself:

  // Description:
  // See if the secondary monitor is attached or not
  bool IsSecondaryMonitorActive(){ return this->DeviceActive;};

  // Description:
  // Detect the secondary monitor attached to the system, gather information about it
  void Initialize();

  // Description:
  // Get Spacing
  void GetMonitorSpacing(double & xSpacing, double & ySpacing){xSpacing = MonitorSpacing[0]; ySpacing = MonitorSpacing[1];};

  // Description:
  // Get/Set VirtialScreenCoord
  void GetVirtualScreenCoord( int & left, int & top){left = this->VirtualScreenCoord[0]; top = this->VirtualScreenCoord[1];};
  void SetVirtualScreenCoord( int left, int top){this->VirtualScreenCoord[0] = left; this->VirtualScreenCoord[1] = top;};

  // Description:
  // Get/Set ScreenSize
  void GetScreenDimensions( unsigned int & sizeX, unsigned int & sizeY){sizeX = this->ScreenSize[0]; sizeY = this->ScreenSize[1];};
  
  // Description
  // set up image data, once the volume has been loaded inside slicer
  void SetupImageData();

  // Description
  // update matrices
  void UpdateMatrices();

  // Description
  // get render window
  vtkWin32OpenGLRenderWindow *GetRenderWindow(){return this->RenderWindow;};

  // Description
  // get renderer
  vtkRenderer *GetRenderer(){ return this->Renderer;};

  // Description
  // get interactor
  vtkRenderWindowInteractor *GetRenderWindowInteractor(){return this->Interator;};

  // Description
  // Get XYToRAS matrix
  vtkMatrix4x4 *GetXYToRAS(){ return this->XYToRAS;};

  // Description
  // Get XYToSlice matrix
  vtkMatrix4x4 *GetXYToIJK(){ return this->XYToIJK;};

  // Description
  // Flip vertically, i.e., about x-axis (left stays left)
  void FlipVertical();

  // Description
  // Flip horizontally, i.e., about y-axis (left becomes right)
  void FlipHorizontal();

  // Description
  // Scale image
  void Scale(double sx, double sy, double sz);

  // Description
  // Rotate image (units degrees)
  void Rotate(double angle);

  // Description
  // Translate (translation units mm, so must be converted to pixels inside function)
  void Translate(double tx, double ty, double tz);

  // Description
  // Overlay needle guide
  void OverlayNeedleGuide();

protected:
  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  

  vtkPerkStationModuleGUI *GUI;

  // about handling the display/visualization
  vtkWin32OpenGLRenderWindow *RenderWindow;
  vtkRenderer *Renderer;
  vtkRenderWindowInteractor *Interator;
  vtkImageMapper *ImageMapper;
  vtkActor2D *ImageActor;
  
  vtkImageReslice *Reslice; // reslice/resample filter
  // matrices   
  vtkMatrix4x4 *XYToIJK; // moving from xy coordinates to volume/slice ijk
  vtkMatrix4x4 *XYToRAS; // use volume node's ijktoras matrix along with xytoijk to get a mapping of xy to ras
  vtkMatrix4x4 *CurrentTransformMatrix;
  // transform to be used for Reslice
  vtkTransform *ResliceTransform;
  vtkMRMLScalarVolumeNode *VolumeNode;

  vtkImageData *ImageData; // the image data to be displayed

  vtkMatrix4x4 *GetFlipMatrixFromDirectionCosines (vtkMatrix4x4 *directionMatrix, bool & verticalFlip, bool & horizontalFlip);
  void UpdateImageDisplay();
  
  // about the monitor
  bool DeviceActive;
  int MonitorPixelResolution[2];
  double MonitorPhysicalSizeMM[2];
  double MonitorSpacing[2];
  int VirtualScreenCoord[2];
  int ScreenSize[3];
  int ImageSize[3];
  bool VerticalFlipped;
  bool HorizontalFlipped;
private:
  vtkPerkStationSecondaryMonitor(const vtkPerkStationSecondaryMonitor&);
  void operator=(const vtkPerkStationSecondaryMonitor&);
};

#endif
