
#ifndef __vtkPerkStationSecondaryMonitor_h
#define __vtkPerkStationSecondaryMonitor_h


#include "vtkImageMapper.h"
#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkWin32OpenGLRenderWindow.h"

#include "vtkPerkStationModule.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkTextActorFlippable.h"

  // for getting display device information
#include "Windows.h"

#include <vector>

class vtkActor;
class vtkActor2D;
class vtkActor2DCollection;
class vtkActorCollection;
class vtkImageData;
class vtkImageMapToWindowLevelColors;
class vtkImageReslice;
class vtkLineSource;
class vtkMatrix4x4;
class vtkMRMLScalarVolumeNode;
class vtkPerkStationModuleGUI;
class vtkSlicerSliceViewer;
class vtkTransform;
class vtkTransformFilter;
class vtkKWFrame;
class vtkTextActor;
class vtkKWRenderWidget;


/**
 * This class uses the following coordinates:
 * * RAS: From slicer, in mm.
 * * IJK: Image volume, in voxels. Directions different from RAS.
 * * XY: Units in pixels. Origin at the lower left corner of the overlayed
 *       image. On typical overlay hardware, this is horizontally flipped
 *       compared to the second monitor screen coordinate system.
 * 
 * Second monitor (this) displayes the same image slice, which is displayed
 * in the "Red" viewport of slicer. It transforms the image to match the
 * displayed size with the physical size of the image, and to match the
 * manual calibration (translation and rotation) to the patient.
 */
class
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
  
  
  // Detect the secondary monitor attached to the system,
  // gather information about it
  void Initialize();
  
  
  void GetMonitorSpacing( double & xSpacing, double & ySpacing ) {
    xSpacing = this->MonitorPhysicalSizeMM[0] / this->MonitorPixelResolution[0];
    ySpacing = this->MonitorPhysicalSizeMM[1] / this->MonitorPixelResolution[1];
  };
  
  
  void GetVirtualScreenCoord( int & left, int & top ) {
    left = this->VirtualScreenCoord[ 0 ];
    top = this->VirtualScreenCoord[ 1 ];
  };
  
  void SetVirtualScreenCoord( int left, int top ) {
    this->VirtualScreenCoord[ 0 ] = left;
    this->VirtualScreenCoord[ 1 ] = top;
  };
  
  
  void GetPhysicalSize( double & mmX, double & mmY ) {
    mmX = this->MonitorPhysicalSizeMM[ 0 ];
    mmY = this->MonitorPhysicalSizeMM[ 1 ];
  };
  
  void SetPhysicalSize( double mmX, double mmY ) {
    this->MonitorPhysicalSizeMM[ 0 ] = mmX;
    this->MonitorPhysicalSizeMM[ 1 ] = mmY;
    this->UpdateImageDisplay();
  };
  
  
  void GetPixelResolution( double & pixX, double & pixY ) {
    pixX = this->MonitorPixelResolution[ 0 ];
    pixY = this->MonitorPixelResolution[ 1 ];
  };
  
  void SetPixelResolution( double pixX, double pixY ) {
    this->MonitorPixelResolution[ 0 ] = pixX;
    this->MonitorPixelResolution[ 1 ] = pixY;
    this->UpdateCornerPositions();
  };
  
  
  // Description:
  // Get/Set ScreenSize
  void GetScreenDimensions( unsigned int & sizeX, unsigned int & sizeY ) {
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
  void OverlayRealTimeNeedleTip( double tipRAS[3],
                                 vtkMatrix4x4 *tranformMatrix = NULL );
  void RemoveOverlayRealTimeNeedleTip();

  
  void ResetCalibration();

  
  void RemoveOverlayNeedleGuide();
  
  void SetDepthPerceptionLines();  
  void RemoveDepthPerceptionLines();
  void RemoveTextActors();
  
  void UpdateImageDataOnSliceOffset( double sliceOffset = 0 );

  void SetRealTimeNeedleLineActorVisibility( bool v );


  void UpdateImageDisplay();
  
  void ShowCalibrationControls( bool show );
  void ShowNeedleGuide( bool show );
  void ShowDepthPerceptionLines( bool show );
  
  
protected:
  
  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  
  
  vtkPerkStationModuleGUI *GUI;
  
  
  // Visual components --------------------------------------------------------
  
    // Display/visualization.
  vtkSmartPointer< vtkWin32OpenGLRenderWindow > RenderWindow;
  vtkSmartPointer< vtkRenderer >                Renderer;
  vtkSmartPointer< vtkRenderWindowInteractor >  Interactor;
  vtkSmartPointer< vtkImageMapper >             ImageMapper;
  vtkSmartPointer< vtkActor2D >                 ImageActor;
  
    // Real time needle display.
  vtkSmartPointer< vtkActor >       NeedleGuideActor;
  vtkSmartPointer< vtkActor >       NeedleTipActor;
  vtkSmartPointer< vtkActor >       RealTimeNeedleLineActor;
  vtkSmartPointer< vtkTextActor >   NeedleTipZLocationText;
  
    // Collection of depth perception lines and needle measure.
  vtkSmartPointer< vtkActorCollection >    DepthPerceptionLines;  
  vtkSmartPointer< vtkActor2DCollection >  TextActorsCollection;
  vtkSmartPointer< vtkTextActorFlippable > MeasureDigitsActor;
  
    // Guide of keys for calibration.
  vtkSmartPointer< vtkTextActorFlippable > CalibrationControlsActor;
  
  vtkSmartPointer< vtkTextActorFlippable > TablePositionActor;
  
    // Left/Right side letters.
  vtkSmartPointer< vtkTextActorFlippable > LeftSideActor;
  vtkSmartPointer< vtkTextActorFlippable > RightSideActor;
  
  // --------------------------------------------------------------------------
  
  
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
  
  
  void UpdateCornerPositions();
  
  double UpperRightCorner[ 2 ];
  double UpperLeftCorner[ 2 ];
  double LowerRightCorner[ 2 ];
  double LowerLeftCorner[ 2 ];
  
  
    // Image geometry.

public:
  
  void SetHorizontalFlip( bool flip );
  void SetVerticalFlip( bool flip );
  
  void SetRotationCenter( double center[ 2 ] );
  
  void GetRotation( double& angle );
  void SetRotation( double angle );
  
  void GetTranslation( double& x, double& y );
  void SetTranslation( double x, double y );
  
  void UpdateCalibration();
  
  
private:
  
  vtkSmartPointer< vtkTransform > XYToRAS();
  vtkSmartPointer< vtkTransform > XYToIJK();
  
  int NumberOfMonitors; // Number of physical monitors of the computer.
  
    // Extract the displayed image from the planning image volume. 
  vtkSmartPointer< vtkTransform > ResliceTransform;
  vtkSmartPointer< vtkImageReslice > ResliceFilter;
  
    // Calibration parameters. Used in the post reslice transform.
  bool HorizontalFlip;
  bool VerticalFlip;
  double Rotation;
  double RotationCenter[ 2 ];
  double Translation[ 2 ];
  double Scale[ 2 ];
  PatientPosition Position;
  
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
