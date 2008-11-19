#ifndef __vtkTRProstateBiopsySecondaryMonitor_h
#define __vtkTRProstateBiopsySecondaryMonitor_h



// for getting display device information
#include "Windows.h"
#include "vtkTRProstateBiopsyWin32Header.h"


class vtkTRProstateBiopsyGUI;
class vtkSlicerSliceViewer;
class vtkImageData;
class vtkImageReslice;
class vtkMatrix4x4;
class vtkTransform;
class vtkMRMLScalarVolumeNode;
class vtkWin32OpenGLRenderWindow;
class vtkImageMapToWindowLevelColors;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageMapper;
class vtkActor2D;
class vtkActor;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsySecondaryMonitor
{
public:
  static vtkTRProstateBiopsySecondaryMonitor *New();  

  // Description: 
  // Get/Set GUI
  vtkTRProstateBiopsyGUI* GetGUI(){return this->GUI;};
  virtual void SetGUI(vtkTRProstateBiopsyGUI *gui){ this->GUI = gui;};

  virtual void SetVolumeNode(vtkMRMLScalarVolumeNode *volume){this->VolumeNode = volume;};

  // About the secondary monitor itself:

  // Description:
  // See if the secondary monitor is attached or not
  bool IsSecondaryMonitorActive(){ return this->DeviceActive;};

  // Description:
  // Detect the secondary monitor attached to the system, gather information about it
  void Initialize();

  // Description:
  // Get Spacing
  //void GetMonitorSpacing(double & xSpacing, double & ySpacing){xSpacing = this->MonitorPhysicalSizeMM[0]/this->MonitorPixelResolution[0]; ySpacing = this->MonitorPhysicalSizeMM[1]/this->MonitorPixelResolution[1];};

  // Description:
  // Get/Set VirtialScreenCoord
  void GetVirtualScreenCoord( int & left, int & top){left = this->VirtualScreenCoord[0]; top = this->VirtualScreenCoord[1];};
  void SetVirtualScreenCoord( int left, int top){this->VirtualScreenCoord[0] = left; this->VirtualScreenCoord[1] = top;};

  
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
  // Scale image
  void Scale(double sx, double sy, double sz);

  // Description
  // reset calibration
  void ResetCalibration();

  // Description
  // remove overlay guide needle actor
  void RemoveOverlayNeedleGuide();

protected:
  vtkTRProstateBiopsySecondaryMonitor();
  ~vtkTRProstateBiopsySecondaryMonitor();  

  vtkTRProstateBiopsyGUI *GUI;

  // about handling the display/visualization
  vtkWin32OpenGLRenderWindow *RenderWindow;
  vtkRenderer *Renderer;
  vtkRenderWindowInteractor *Interator;
  vtkImageMapper *ImageMapper;
  vtkActor2D *ImageActor;
  vtkActor *NeedleActor;  
  vtkImageMapToWindowLevelColors *MapToWindowLevelColors;
  
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
  bool DisplayInitialized; // indicate whether the matrices have been set up properly
  int VirtualScreenCoord[2];
  int ScreenSize[3];
  int ImageSize[3];
  bool VerticalFlipped;
  bool HorizontalFlipped;

private:
  vtkTRProstateBiopsySecondaryMonitor(const vtkTRProstateBiopsySecondaryMonitor&);
  void operator=(const vtkTRProstateBiopsySecondaryMonitor&);
};

#endif
