
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
class vtkCollection;
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



//
// This class uses the following coordinates:
// * RAS:     From Slicer, in mm.
// * IJK:     Image volume as in DICOM, in voxels. Directions different from RAS.
// * Overlay: Virtual image that appears after reflection on the overlay glass.
//            Units in pixels. Origin at the lower left corner of the overlayed
//            image. First coordinate goes to the right from the operator point of view.
//            Second coordinate goes up from the operator poitn of view.
//            On typical overlay hardware, this is horizontally flipped
//            compared to the second monitor screen coordinate system.
// 
// Second monitor (this) displayes the same image slice, which is displayed
// in the "Red" viewport of Slicer. It transforms the image to match the
// displayed size with the physical size of the image, and to match the
// manual calibration (translation and rotation) to the patient.
//
class
vtkPerkStationSecondaryMonitor
: public vtkObject
{
public:
  
  static vtkPerkStationSecondaryMonitor *New();  
  vtkTypeMacro( vtkPerkStationSecondaryMonitor, vtkObject );
  
  
  void Initialize();     // Detect the secondary monitor, gather information about it.
  void SetupImageData(); // set up image data, once the volume has been loaded inside slicer
  
  
    // Get/Set methods about display geometry. --------------------------------
  
  vtkGetMacro( DeviceActive, bool );
  vtkGetMacro( Initialized, bool );
  vtkGetMacro( ImageLoaded, bool );
  
  void GetMonitorSpacing( double& resolutionX, double& resoltionY );
  
  vtkGetVectorMacro( VirtualScreenCoord, int, 2 );
  vtkSetVectorMacro( VirtualScreenCoord, int, 2 );
  
  vtkGetVectorMacro( MonitorSizePixel, int, 2 );
  vtkSetVectorMacro( MonitorSizePixel, int, 2 );
  
  vtkGetVectorMacro( MonitorSizeMm, double, 2 );
  vtkSetVectorMacro( MonitorSizeMm, double, 2 );
  
  
    // Show / hide visual guides on the second monitor. -----------------------
    
  void OverlayRealTimeNeedleTip( double tipRAS[3], vtkMatrix4x4 *tranformMatrix = NULL );
  void SetRealTimeNeedleLineActorVisibility( bool v );
  void RemoveOverlayRealTimeNeedleTip();
  
  void OverlayNeedleGuide();
  
  void ShowCalibrationControls( bool show );
  void ShowDepthPerceptionLines( bool show );
  
  void CreateDepthPerceptionLines();  
  void RemoveDepthPerceptionLines();
  
  
    // Call these functions to update image on the second monitor. ------------
  
  void UpdateImageDataOnSliceOffset( double sliceOffset = 0 );
  void UpdateImageDisplay();
  
  
  // References to other objects. =============================================
  
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
  
  // ==========================================================================
  
  
protected:
  
  vtkPerkStationSecondaryMonitor();
  ~vtkPerkStationSecondaryMonitor();  
  
  vtkPerkStationSecondaryMonitor( const vtkPerkStationSecondaryMonitor& );
  void operator=( const vtkPerkStationSecondaryMonitor& );
  
  
private:
  
  //BTX
  
  
  // Internal helper functions. -----------------------------------------------
  
  void UpdateCornerPositions();
  void UpdateVisibilities( bool& displayEmpty );   // Only to be called from UpdateImageDisplay().
  void PointOverlayToRenderer( double* pointDisplay, double* pointWorld );
  
  vtkSmartPointer< vtkTransform > OverlayToRAS();
  vtkSmartPointer< vtkTransform > OverlayToIJK();
  
  
  
  // Visual components --------------------------------------------------------
  
    // Display / visualization pipeline.
  
  vtkSmartPointer< vtkWin32OpenGLRenderWindow >     RenderWindow;
  vtkSmartPointer< vtkRenderer >                    Renderer;
  vtkSmartPointer< vtkRenderWindowInteractor >      Interactor;
  vtkSmartPointer< vtkImageMapper >                 ImageMapper;
  vtkActor2D*                                       ImageActor;
  vtkSmartPointer< vtkImageData >                   EmptyImage;
  vtkSmartPointer< vtkImageMapToWindowLevelColors > MapToWindowLevelColors;
  
    // Real time needle display.
  
  vtkSmartPointer< vtkActor >       NeedleGuideActor;
  vtkSmartPointer< vtkActor >       NeedleTipActor;
  vtkSmartPointer< vtkActor >       RealTimeNeedleLineActor;
  vtkSmartPointer< vtkTextActor >   NeedleTipZLocationText;
  
    // Guide of keys for calibration.
  
  vtkSmartPointer< vtkTextActorFlippable > CalibrationControlsActor;
  
    // Text guides for calibration and insertion.
  
  vtkSmartPointer< vtkTextActorFlippable > TablePositionActor;
  vtkSmartPointer< vtkTextActorFlippable > CalibrationNameActor;
  vtkSmartPointer< vtkTextActorFlippable > PlanNameActor;
  
  vtkSmartPointer< vtkTextActorFlippable > LeftSideActor;
  vtkSmartPointer< vtkTextActorFlippable > RightSideActor;
  
  vtkSmartPointer< vtkTextActorFlippable > WorkflowActor;
  
    // Depth perception lines and needle measure. For Insertion workphase.
  
  vtkSmartPointer< vtkActorCollection >    DepthPerceptionLines;
  vtkSmartPointer< vtkCollection >         DepthPerceptionTexts;
  double                                   DepthPerceptionLinesIncrement;
  unsigned int                             NumOfDepthPerceptionLines;
  
  vtkSmartPointer< vtkActor >              DepthMeasureLineActor;
  vtkSmartPointer< vtkTextActorFlippable > DepthMeasureTextActor;
  
  
  
  // Parameters for display on the second monitor. ----------------------------
  
  bool    DeviceActive;
  bool    Initialized;
  bool    ImageLoaded;
  int     NumberOfMonitors; // Number of physical monitors of the computer.
  
  double  MonitorSizeMm[ 2 ];
  int     MonitorSizePixel[ 2 ];
  int     VirtualScreenCoord[ 2 ];
  int     ImageSizeVoxel[ 3 ];    // Number of pixels on image slices, num of slices.
  
  
  double UpperRightCorner[ 2 ];
  double UpperLeftCorner[ 2 ];
  double LowerRightCorner[ 2 ];
  double LowerLeftCorner[ 2 ];
  
  
  vtkImageData* ImageData;
  
  double SliceOffsetRAS;
  double SliceOffsetIJK;
  
  
    // Extract the displayed image from the planning image volume. 
  vtkSmartPointer< vtkTransform >    ResliceTransform;
  vtkSmartPointer< vtkImageReslice > ResliceFilter;
  
  
  
  // Calibration transforms. --------------------------------------------------
  
  vtkSmartPointer< vtkTransform > SecMonFlipTransform;
  vtkSmartPointer< vtkTransform > SecMonRotateTransform;
  vtkSmartPointer< vtkTransform > SecMonTranslateTransform;
  
  vtkSmartPointer< vtkTransform > RASToIJK;
  
  
  //ETX
  
};


#endif
