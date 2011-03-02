
#ifndef __vtkPerkStationSecondaryMonitor_h
#define __vtkPerkStationSecondaryMonitor_h


#include "vtkImageMapper.h"
#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkWin32OpenGLRenderWindow.h"

#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationModule.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkTextActorFlippable.h"

#include "Windows.h"  // for getting display device information

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
class vtkSlicerApplicationGUI;
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
  vtkTypeMacro( vtkPerkStationSecondaryMonitor, vtkObject );
  
  
  bool IsSecondaryMonitorActive() { return this->DeviceActive; };
  
  void Initialize();     // Detect the secondary monitor, gather information about it.
  
  void SetupImageData(); // set up image data, once the volume has been loaded inside slicer
  
  
    // Get/Set methods about display geometry. --------------------------------
  
  void GetMonitorSpacing( double & xSpacing, double & ySpacing ) {
    xSpacing = this->Scale[ 0 ];  ySpacing = this->Scale[ 1 ];  };
  
  void GetVirtualScreenCoord( int & left, int & top ) {
    left = this->VirtualScreenCoord[ 0 ];  top = this->VirtualScreenCoord[ 1 ];  };
  
  void SetVirtualScreenCoord( int left, int top ) {
    this->VirtualScreenCoord[ 0 ] = left;  this->VirtualScreenCoord[ 1 ] = top;  };
  
  void GetPixelResolution( double & pixX, double & pixY ) {
    pixX = this->MonitorPixelResolution[ 0 ];   pixY = this->MonitorPixelResolution[ 1 ];  };
  
  void SetPixelResolution( double pixX, double pixY ) {
    this->MonitorPixelResolution[ 0 ] = pixX;   this->MonitorPixelResolution[ 1 ] = pixY;  this->UpdateCornerPositions(); };
  
  void GetScreenDimensions( unsigned int & sizeX, unsigned int & sizeY ) {
    sizeX = this->ScreenSize[ 0 ];   sizeY = this->ScreenSize[ 1 ]; };
  
  
    // Show / hide visual guides on the second monitor. -----------------------
    
  void OverlayRealTimeNeedleTip( double tipRAS[3], vtkMatrix4x4 *tranformMatrix = NULL );
  void SetRealTimeNeedleLineActorVisibility( bool v );
  void RemoveOverlayRealTimeNeedleTip();
  
  void OverlayNeedleGuide();
  void RemoveOverlayNeedleGuide();
  
  void SetDepthPerceptionLines();  
  void RemoveDepthPerceptionLines();
  
  void RemoveTextActors();
  
  void ShowCalibrationControls( bool show );
  void ShowNeedleGuide( bool show );
  void ShowDepthPerceptionLines( bool show );
  
  
    // Call these functions to update image on the second monitor. ------------
  
  void UpdateImageDataOnSliceOffset( double sliceOffset = 0 );
  void UpdateImageDisplay();
  
  
    // Reference to the module GUI. -------------------------------------------

public:
  void SetPerkStationModuleGUI( vtkPerkStationModuleGUI* gui );
private:
  vtkPerkStationModuleGUI* PerkStationModuleGUI;
  
  
    // Reference to the module node. Not observed. ----------------------------
    // Actual volume node shoudl be requested from this node.

public:
  vtkGetStringMacro( PerkStationModuleNodeID );
  vtkSetStringMacro( PerkStationModuleNodeID );
  vtkMRMLPerkStationModuleNode* GetPerkStationModuleNode();
  vtkMRMLScalarVolumeNode* GetActiveVolumeNode();  // Convenience.
private:
  char* PerkStationModuleNodeID;  
  
  
protected:
  
  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  
  
  
  // Visual components --------------------------------------------------------
  //BTX
  
    // Display/visualization.
  vtkSmartPointer< vtkWin32OpenGLRenderWindow > RenderWindow;
  vtkSmartPointer< vtkRenderer >                Renderer;
  vtkSmartPointer< vtkRenderWindowInteractor >  Interactor;
  vtkSmartPointer< vtkImageMapper >             ImageMapper;
  vtkActor2D*                                   ImageActor;
  
    // Real time needle display.
  vtkSmartPointer< vtkActor >       NeedleGuideActor;
  vtkSmartPointer< vtkActor >       NeedleTipActor;
  vtkSmartPointer< vtkActor >       RealTimeNeedleLineActor;
  vtkSmartPointer< vtkTextActor >   NeedleTipZLocationText;
  
    // Collection of depth perception lines and needle measure.
  vtkSmartPointer< vtkActorCollection >    DepthPerceptionLines;  
  vtkActor2DCollection*                    TextActorsCollection;
  vtkSmartPointer< vtkTextActorFlippable > MeasureDigitsActor;
  
    // Guide of keys for calibration.
  vtkSmartPointer< vtkTextActorFlippable > CalibrationControlsActor;
  
  vtkSmartPointer< vtkTextActorFlippable > TablePositionActor;
  vtkSmartPointer< vtkTextActorFlippable > CalibrationNameActor;
  
    // Left/Right side letters.
  vtkSmartPointer< vtkTextActorFlippable > LeftSideActor;
  vtkSmartPointer< vtkTextActorFlippable > RightSideActor;
  
  // --------------------------------------------------------------------------
  
  vtkSmartPointer< vtkImageMapToWindowLevelColors > MapToWindowLevelColors;
  
  
  // vtkMatrix4x4* GetFlipMatrixFromDirectionCosines ( vtkMatrix4x4* directionMatrix, bool& verticalFlip, bool& horizontalFlip );
 
  
    // Parameters for display on the second monitor. --------------------------
  
  bool   DeviceActive;
  bool   DisplayInitialized;
  
  double MonitorPixelResolution[ 2 ]; 
  int    VirtualScreenCoord[ 2 ];
  int    ScreenSize[ 3 ]; // Number of pixels on the monitor.
  int    ImageSize[ 3 ];  // Number of pixels on image slices, num of slices.
  
  double        MeasureNeedleLengthInWorldCoordinates;
  unsigned int  NumOfDepthPerceptionLines;
  bool          DepthLinesInitialized;

  
private:

  vtkPerkStationSecondaryMonitor( const vtkPerkStationSecondaryMonitor& );
  void operator=( const vtkPerkStationSecondaryMonitor& );
  
  
  void UpdateCornerPositions();
  
  double UpperRightCorner[ 2 ];
  double UpperLeftCorner[ 2 ];
  double LowerRightCorner[ 2 ];
  double LowerLeftCorner[ 2 ];
  
  
    // Image geometry and display. --------------------------------------------
  
  vtkImageData* ImageData;
  
  //BTX
  vtkSmartPointer< vtkTransform > XYToRAS();
  vtkSmartPointer< vtkTransform > XYToIJK();
  //ETX
  
  int NumberOfMonitors; // Number of physical monitors of the computer.
  
  //BTX
    // Extract the displayed image from the planning image volume. 
  vtkSmartPointer< vtkTransform >    ResliceTransform;
  vtkSmartPointer< vtkImageReslice > ResliceFilter;
  
    // Calibration transforms.
  vtkSmartPointer< vtkTransform > SecMonFlipTransform;
  vtkSmartPointer< vtkTransform > SecMonRotateTransform;
  vtkSmartPointer< vtkTransform > SecMonTranslateTransform;
  
  vtkSmartPointer< vtkTransform > RASToIJK;
  //ETX
  
  double Scale[ 2 ];  // Scale used for display.
  
  
    // Image slice position.
    
  double SliceOffsetRAS;  // In RAS coordinates.
  double SliceOffsetIJK;  // In IJK coordinates.
};

#endif
