#ifndef __vtkPerkStationSecondaryMonitor_h
#define __vtkPerkStationSecondaryMonitor_h

#include "vtkObject.h"
#include "vtkPerkStationModule.h"
// for getting display device information
#include "Windows.h"
#include <vector>

class vtkPerkStationModuleGUI;
class vtkSlicerSliceViewer;
class vtkImageData;
class vtkImageReslice;
class vtkMatrix4x4;
class vtkTransform;
class vtkMRMLScalarVolumeNode;
class vtkKWFrame;
class vtkWin32OpenGLRenderWindow;
class vtkImageMapToWindowLevelColors;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageMapper;
class vtkTextActor;
class vtkActor2D;
class vtkActor2DCollection;
class vtkActor;
class vtkKWRenderWidget;
class vtkLineSource;
class vtkActorCollection;


/**
 * Creates a VTK window for the secondary monitor, and responsible for all
 * computations needed to fill it with the appropriate image slice.
 * 
 * Coordinate systems:
 *  * RAS: Slicer's system, fixed to the patient, unit is mm.
 */
class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationSecondaryMonitor
: public vtkObject
{

public:
  
  static vtkPerkStationSecondaryMonitor *New();  

  // Description: 
  // Get/Set GUI
  vtkPerkStationModuleGUI* GetGUI(){return this->GUI;};
  vtkTypeMacro(vtkPerkStationSecondaryMonitor,vtkObject);
  virtual void SetGUI(vtkPerkStationModuleGUI *gui){ this->GUI = gui;};
  
  
  // About the secondary monitor itself:

  // Description:
  // See if the secondary monitor is attached or not
  bool IsSecondaryMonitorActive(){ return this->DeviceActive;};

  void Initialize();
  
  
  // Description:
  // Get Spacing
  void GetMonitorSpacing(double & xSpacing, double & ySpacing)
  {
    xSpacing = this->MonitorPhysicalSizeMM[0]/this->MonitorPixelResolution[0];
    ySpacing = this->MonitorPhysicalSizeMM[1]/this->MonitorPixelResolution[1];
  }
  
  
  // Description:
  // Get/Set VirtialScreenCoord
  void GetVirtualScreenCoord( int & left, int & top)
  {
    left = this->VirtualScreenCoord[0];
    top = this->VirtualScreenCoord[1];
  }
  
  void SetVirtualScreenCoord( int left, int top )
  {
    this->VirtualScreenCoord[0] = left;
    this->VirtualScreenCoord[1] = top;
  }
  
  
  // Description:
  // get/set physical size of monitor
  void GetPhysicalSize(double & mmX, double & mmY)
  {
    mmX = this->MonitorPhysicalSizeMM[0];
    mmY = this->MonitorPhysicalSizeMM[1];
  }
  
  void SetPhysicalSize(double mmX, double mmY)
  {
    this->MonitorPhysicalSizeMM[0] = mmX;
    this->MonitorPhysicalSizeMM[1] = mmY;
  }

  
    // get/set physical size of monitor
  void GetPixelResolution(double & pixX, double & pixY)
  {
    pixX = this->MonitorPixelResolution[0];
    pixY = this->MonitorPixelResolution[1];
  }
    
  void SetPixelResolution(double pixX, double pixY)
  {
    this->MonitorPixelResolution[0] = pixX;
    this->MonitorPixelResolution[1] = pixY;
  }
  
  
    // Get/Set ScreenSize
  void GetScreenDimensions( unsigned int & sizeX, unsigned int & sizeY)
  {
    sizeX = this->ScreenSize[0]; sizeY = this->ScreenSize[1];
  }
  
    // set up image data, once the volume has been loaded inside slicer
  void SetupImageData();
  
    // load calibration, which has been read in from a file already
  void LoadCalibration();
  
  
  bool GetDepthLinesInitialized()
  {
    return this->DepthLinesInitialized;
  }
  
  
  // please note that this function calls UpdateImageDisplay at end of
  // recomputing matrices
  void UpdateMatrices();
  
  // reads the window/level from scalar display node, and updates the
  // rendering the image
  void UpdateImageDisplay();
  
  
  vtkWin32OpenGLRenderWindow *GetRenderWindow(){return this->RenderWindow;};
  vtkRenderer *GetRenderer(){ return this->Renderer;};
  vtkRenderWindowInteractor *GetRenderWindowInteractor(){return this->Interactor;};
  vtkMatrix4x4 *GetXYToRAS(){ return this->XYToRAS;};
  vtkMatrix4x4 *GetXYToIJK(){ return this->XYToIJK;};
  
  
    // Old interface for image geometry.
  
  void GetTranslation(double & translationX, double & translationY)
  {
    translationX = this->CurrentTranslation[0];
    translationY = this->CurrentTranslation[1];
  }
  
  void GetRotation(double & rotation)
  {
    rotation = this->CurrentRotation;
  }
  
  void FlipVertical( bool flip );
  void FlipHorizontal( bool flip );
  void Scale( double sx, double sy, double sz );
  void Rotate( double angle ); // Rotate image (units degrees)
    // Tilt image out of plane (units degrees)
  void TiltOutOfPlane( double tiltAngle, double rasCor[3] );
    // Translate (translation units mm, so must be converted to pixels
    // inside function)
  void Translate( double tx, double ty, double tz );
  
  
  
  void OverlayNeedleGuide();
  void OverlayRealTimeNeedleTip(double tipRAS[3], vtkMatrix4x4 *tranformMatrix=NULL);
  void RemoveOverlayRealTimeNeedleTip();
  
  
  void ResetCalibration();
  
  
    // remove overlay guide needle actor
  void RemoveOverlayNeedleGuide();
  
  void ResetTilt();
  
  void SetDepthPerceptionLines();  
  
  void RemoveDepthPerceptionLines();
  
  void RemoveTextActors();
  
  void UpdateImageDataOnSliceOffset(double sliceOffset=0);
  
  
    // Set visivility of RealTimeNeedleLineActor
  void SetRealTimeNeedleLineActorVisibility(bool v);
  
  
    // Image geometry.
  
  void SetSecMonHorizontalFlip( bool flip );
  void SetSecMonVerticalFlip( bool flip );
  void SetSecMonRotation( double[ 3 ] rotation );
  void SetSecMonRotationCenter( double[ 3 ] center );
  void SetSecMonTranslation( double[ 3 ] translation );
  
  
protected:

  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  

  vtkPerkStationModuleGUI *GUI;

    // about handling the display/visualization
  vtkWin32OpenGLRenderWindow *RenderWindow;
  vtkRenderer *Renderer;
  vtkRenderWindowInteractor *Interactor;
  vtkImageMapper *ImageMapper;
  vtkActor2D *ImageActor;
  
  // overlay needle guide actor
  vtkActor *NeedleGuideActor; 
  // cross-hair at real-time needle tip
  vtkActor *NeedleTipActor;
  // real-time needle vector as being tracked
  vtkActor *RealTimeNeedleLineActor;
  // needle tip z-location as tracked text actor
  vtkTextActor *NeedleTipZLocationText;
  // collection of depth perception lines
  vtkActorCollection *DepthPerceptionLines;  
  // collection of text labels on the depth perception lines
  vtkActor2DCollection *TextActorsCollection;

  vtkImageMapToWindowLevelColors *MapToWindowLevelColors;
  
  vtkImageReslice *Reslice; // reslice/resample filter
  // matrices   
  vtkMatrix4x4 *XYToIJK; // moving from xy coordinates to volume/slice ijk
  vtkMatrix4x4 *XYToRAS; // use volume node's ijktoras matrix along with xytoijk
                         // to get a mapping of xy to ras
  vtkMatrix4x4 *CurrentTransformMatrix;


  vtkMatrix4x4 *SystemStateXYToIJK;
  vtkMatrix4x4 *SystemStateResliceMatrix;

    // transform to be used for Reslice
  vtkTransform *ResliceTransform;
  
  vtkMRMLScalarVolumeNode *VolumeNode;

  vtkImageData *ImageData; // the image data to be displayed

  vtkMatrix4x4 *GetFlipMatrixFromDirectionCosines (
    vtkMatrix4x4 *directionMatrix, bool & verticalFlip, bool & horizontalFlip );
 
  
  // about the monitor
  bool DeviceActive;
  bool DisplayInitialized;
  
  int VirtualScreenCoord[2];
  bool VerticalFlipped;
  bool HorizontalFlipped;

  double MeasureNeedleLengthInWorldCoordinates;
  unsigned int NumOfDepthPerceptionLines;
  bool DepthLinesInitialized;

  double CurrentTranslation[2];
  double CurrentRotation;
  bool CalibrationFromFileLoaded;

private:
  
  vtkPerkStationSecondaryMonitor(const vtkPerkStationSecondaryMonitor&);
  void operator=(const vtkPerkStationSecondaryMonitor&);
  
  
  
    // New data structure for image geometry.
  
  vtkTransform *SecMonHorizontalFlipTransform;
  vtkTransform *SecMonVerticalFlipTranform;
  vtkTransform *SecMonRotateTransform;
  vtkTransform *SecMonTranslateTransform;
  
  bool SecMonHorizontalFlip;
  bool SecMonVerticalFlip;
  double SecMonRotation[ 3 ];
  double SecMonRotationCenter[ 3 ]; // In the RAS coordinate system.
  double SecMonTranslation[ 3 ];
  
    // Transfered from the old version.
  double MonitorPixelResolution[ 2 ];
  double MonitorPhysicalSizeMM[ 2 ];  
  int ScreenSize[ 3 ];   // Number of pixels on the monitor.
  int ImageSize[ 3 ];    // Number of pixels in the image.
  
};

#endif
