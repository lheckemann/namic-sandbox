
#ifndef __vtkPerkStationSecondaryMonitor_h
#define __vtkPerkStationSecondaryMonitor_h

#include "vtkObject.h"
#include "vtkPerkStationModule.h"
#include "vtkSmartPointer.h"

// for getting display device information
#include "Windows.h"
#include <vector>

class vtkPerkStationModuleGUI;
class vtkSlicerSliceViewer;
class vtkImageData;
class vtkImageReslice;
class vtkMatrix4x4;
class vtkTransform;
class vtkTransformFilter;
class vtkMRMLScalarVolumeNode;
class vtkKWFrame;

#include "vtkWin32OpenGLRenderWindow.h"

class vtkImageMapToWindowLevelColors;

#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageMapper.h"

class vtkTextActor;
class vtkTextActorFlippable;
class vtkActor2D;
class vtkActor2DCollection;
class vtkActor;
class vtkKWRenderWidget;
class vtkLineSource;
class vtkActorCollection;


/**
 * This class uses the following coordinates:
 * * RAS: From slicer, in mm.
 * * IJK: Image volume indices, in voxels.
 * 
 * Slicer uses RAS coordinates, but gives the IJKToRAS transform.
 */
class
// VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationSecondaryMonitor : public vtkObject
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
  
  
  // Description:
  // Detect the secondary monitor attached to the system, gather information about it
  void Initialize();
  
  
  void GetMonitorSpacing( double & xSpacing, double & ySpacing )
  {
    xSpacing = this->MonitorPhysicalSizeMM[0] / this->MonitorPixelResolution[0];
    ySpacing = this->MonitorPhysicalSizeMM[1] / this->MonitorPixelResolution[1];
  };
  
  
  void GetVirtualScreenCoord( int & left, int & top )
  {
    left = this->VirtualScreenCoord[ 0 ];
    top = this->VirtualScreenCoord[ 1 ];
  };
  
  void SetVirtualScreenCoord( int left, int top )
  {
    this->VirtualScreenCoord[0] = left;
    this->VirtualScreenCoord[1] = top;
  };

  
  void GetPhysicalSize( double & mmX, double & mmY )
  {
    mmX = this->MonitorPhysicalSizeMM[ 0 ];
    mmY = this->MonitorPhysicalSizeMM[ 1 ];
  };
  
  void SetPhysicalSize( double mmX, double mmY )
  {
    this->MonitorPhysicalSizeMM[ 0 ] = mmX;
    this->MonitorPhysicalSizeMM[ 1 ] = mmY;
  };

  
  void GetPixelResolution( double & pixX, double & pixY )
  {
    pixX = this->MonitorPixelResolution[ 0 ];
    pixY = this->MonitorPixelResolution[ 1 ];
  };
  
  void SetPixelResolution( double pixX, double pixY )
  {
    this->MonitorPixelResolution[ 0 ] = pixX;
    this->MonitorPixelResolution[ 1 ] = pixY;
  };
  
  
  // Description:
  // Get/Set ScreenSize
  void GetScreenDimensions( unsigned int & sizeX, unsigned int & sizeY )
  {
    sizeX = this->ScreenSize[ 0 ];
    sizeY = this->ScreenSize[ 1 ];
  };
  
  // Description
  // set up image data, once the volume has been loaded inside slicer
  void SetupImageData();
  
  
  // Description
  // load calibration, which has been read in from a file already
  void LoadCalibration();

  void OverlayNeedleGuide();

  void OverlayRealTimeNeedleTip(double tipRAS[3], vtkMatrix4x4 *tranformMatrix=NULL);
  void RemoveOverlayRealTimeNeedleTip();

  // Description
  // reset calibration
  void ResetCalibration();

  // Description
  // remove overlay guide needle actor
  void RemoveOverlayNeedleGuide();

  void SetDepthPerceptionLines();  
  void RemoveDepthPerceptionLines();
  void RemoveTextActors();

  void UpdateImageDataOnSliceOffset( double sliceOffset = 0 );

  void SetRealTimeNeedleLineActorVisibility( bool v );


  
public:

  void UpdateImageDisplay();
  
  void ShowCalibrationControls( bool show );
  void ShowNeedleGuide( bool show );
  void ShowDepthPerceptionLines( bool show );
  
  
protected:
  
  vtkSmartPointer< vtkTextActorFlippable > CalibrationControlsActor;

  
protected:
  
  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  

  vtkPerkStationModuleGUI *GUI;

    // Display/visualization.
  vtkSmartPointer< vtkWin32OpenGLRenderWindow > RenderWindow;
  vtkSmartPointer< vtkRenderer >                Renderer;
  vtkSmartPointer< vtkRenderWindowInteractor >  Interactor;
  vtkSmartPointer< vtkImageMapper >             ImageMapper;
  vtkSmartPointer< vtkActor2D >                 ImageActor;
  
    // Real time needle display.
  vtkSmartPointer< vtkActor > NeedleGuideActor;
  vtkSmartPointer< vtkActor > NeedleTipActor;
  vtkSmartPointer< vtkActor > RealTimeNeedleLineActor;
  vtkSmartPointer< vtkTextActor > NeedleTipZLocationText;
  
    // Collection of depth perception lines with labels.
  vtkSmartPointer< vtkActorCollection > DepthPerceptionLines;  
  vtkSmartPointer< vtkActor2DCollection > TextActorsCollection;
  
  vtkSmartPointer< vtkTextActorFlippable > LeftSideActor;
  vtkSmartPointer< vtkTextActorFlippable > RightSideActor;
  
  vtkSmartPointer< vtkImageMapToWindowLevelColors > MapToWindowLevelColors;
  
  
    // Transformations (matrices).
  
  vtkSmartPointer< vtkMatrix4x4 > SystemStateXYToIJK;
  vtkSmartPointer< vtkMatrix4x4 > SystemStateResliceMatrix;

    
  
    // the image data to be displayed
  vtkMRMLScalarVolumeNode*  VolumeNode;
  vtkImageData*             ImageData;
  
  
  vtkMatrix4x4* GetFlipMatrixFromDirectionCosines (
      vtkMatrix4x4* directionMatrix, bool& verticalFlip, bool& horizontalFlip );
 
  
  // about the monitor
  bool DeviceActive;
  bool DisplayInitialized;
  
  double MonitorPixelResolution[ 2 ];
  double MonitorPhysicalSizeMM[ 2 ];  
  int VirtualScreenCoord[ 2 ];
  int ScreenSize[ 3 ]; // Number of pixels on the monitor.
  int ImageSize[ 3 ]; // Number of pixels on image slices, num of slices.
  
  double MeasureNeedleLengthInWorldCoordinates;
  unsigned int NumOfDepthPerceptionLines;
  bool DepthLinesInitialized;

  double CurrentTranslation[2];
  double CurrentRotation;
  bool CalibrationFromFileLoaded;

private:

  vtkPerkStationSecondaryMonitor( const vtkPerkStationSecondaryMonitor& );
  void operator=( const vtkPerkStationSecondaryMonitor& );
  
  
    // Image geometry.

public:
  
  void SetSecMonHorizontalFlip( bool flip );
  void SetSecMonVerticalFlip( bool flip );
  
  void SetRotationCenter( double center[ 2 ] );
  void SetRotation( double rotation );
  void SetTranslation( double x, double y );
  
  
private:
  
  vtkSmartPointer< vtkTransform > XYToRAS();
  
  int NumberOfMonitors; // Number of physical monitors of the computer.
  
    // Extract the displayed image from the planning image volume. 
  vtkSmartPointer< vtkTransform > ResliceTransform;
  vtkSmartPointer< vtkImageReslice > ResliceFilter;
  
    // Calibration parameters. Used in the post reslice transform.
  bool HorizontalFlip; // If true, the system design is wrong!
  bool VerticalFlip; // If true, the system design is wrong!
  double Rotation;
  double RotationCenter[ 2 ];
  double Translation[ 2 ];
  double Scale[ 2 ];
  
    // Calibration transforms.
  vtkSmartPointer< vtkTransform > SecMonFlipTransform;
  vtkSmartPointer< vtkTransform > SecMonRotateTransform;
  vtkSmartPointer< vtkTransform > SecMonTranslateTransform;
  
    // Coordinate transforms.  
  vtkSmartPointer< vtkTransform > RASToIJK;
  
    // Image slice position.
  double SliceOffsetRAS;  // In RAS coordinates.
  double SliceOffsetIJK;  // In IJK coordinates.
};

#endif
