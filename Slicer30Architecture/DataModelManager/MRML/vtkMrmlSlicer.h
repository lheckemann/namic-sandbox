/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlSlicer.h,v $
  Date:      $Date: 2005/12/20 22:44:31 $
  Version:   $Revision: 1.42.2.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlSlicer_h
#define __vtkMrmlSlicer_h

#include <stdlib.h>

#include "vtkCamera.h"
#include "vtkImageReformatIJK.h"
#include "vtkImageReformat.h"
#include "vtkImageOverlay.h"
#include "vtkImageMapToColors.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "vtkLookupTable.h"
#include "vtkMrmlDataVolume.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkImageLabelOutline.h"
#include "vtkImageCrossHair2D.h"
#include "vtkImageZoom2D.h"
#include "vtkImageDouble2D.h"
#include "vtkIndirectLookupTable.h"
#include "vtkImageDrawROI.h"
#include "vtkStackOfPolygons.h"
#include "vtkCollection.h"
#include "vtkVoidArray.h"

#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif


class VTK_SLICER_BASE_EXPORT vtkMrmlSlicer : public vtkObject 
{
  public:
  
  // The Usual vtk class functions
  static vtkMrmlSlicer *New();
  vtkTypeMacro(vtkMrmlSlicer,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
    

  //------ Output things to be displayed in slice s: ------//

  // Description:
  // Overlay is merged fore, back, and label images, for display
  // in slice window s (where s is 0, 1, or 2)
  vtkImageData *GetOutput(int s);

  // Description:
  // Cursor is the moving cross-hair for over slice s
  vtkImageData *GetCursor(int s) ;

  // Description:
  // Active output is either the contour just drawn or the regular
  // overlay image, depending on the slice it is for.
  vtkImageData *GetActiveOutput(int s) ;

  // Description:
  // The active slice is the one last touched by the user.
  void SetActiveSlice(int s);
  vtkGetMacro(ActiveSlice, int);

  // Karl - June 2005 
  vtkSetMacro(DisplayMethod,int);
  vtkGetMacro(DisplayMethod,int);
  
  //------ Factors that affect how slices are displayed: ------//

  // Description:
  // Zoom factor
  void SetZoom(int s, vtkFloatingPointType mag);
  void SetZoom(vtkFloatingPointType mag);
  vtkFloatingPointType GetZoom(int s) ;
 
  void SetZoomNew(int s, vtkFloatingPointType mag);
  void SetZoomNew(vtkFloatingPointType mag);
  vtkFloatingPointType GetZoomNew(int s);
  void SetOriginShift(int s, vtkFloatingPointType sx, vtkFloatingPointType sy);


  // Description:
  // Zoom center
  void SetZoomCenter(int s, vtkFloatingPointType x, vtkFloatingPointType y);
  void GetZoomCenter();
  vtkGetVector2Macro(ZoomCenter0, vtkFloatingPointType);
  vtkGetVector2Macro(ZoomCenter1, vtkFloatingPointType);
  vtkGetVector2Macro(ZoomCenter2, vtkFloatingPointType);

  // Description:
  // Zoom auto center
  void SetZoomAutoCenter(int s, int yes);
  int GetZoomAutoCenter(int s) ;

  // Description:
  // Double slice size outputs 512x512 images for larger display
  // (instead of 256x256)


  // Should be moved to vtkMrmlSlicer.cxx
  void SetDouble(int s, int yes); 
  
  vtkSetMacro(DrawDoubleApproach,int);
  vtkGetMacro(DrawDoubleApproach,int);
  
  int GetDouble(int s) ;

  //Deep Copy Method
  void DeepCopy(vtkMrmlSlicer *src);

  // Description:
  // The cursor is the crosshair that moves with the mouse over the slices
  void SetShowCursor(int vis);
  void SetNumHashes(int hashes);
  void SetCursorColor(vtkFloatingPointType red, vtkFloatingPointType green, vtkFloatingPointType blue);
  void SetCursorPosition(int s, int x, int y) ;
  
  // turn on or off the cross hair intersection - if off there's a gap
    void SetCursorIntersect(int flag);
    void SetCursorIntersect(int s, int flag)  ;
    int GetCursorIntersect(int s) ;

  // Description:
  // Field of view for slices.  Also used for reformatting...
  vtkGetMacro(FieldOfView, vtkFloatingPointType);
  void SetFieldOfView(vtkFloatingPointType x);

  // Cursor Annotation
  vtkFloatingPointType GetForePixel(int s, int x, int y);
  vtkFloatingPointType GetBackPixel(int s, int x, int y);

  // Description:
  // Sets the opacity used to overlay this layer on the others
  void SetForeOpacity(vtkFloatingPointType opacity);
  vtkGetMacro(ForeOpacity, vtkFloatingPointType);

  // Description:
  // Sets whether to fade out the background even when the 
  // foreground is clearn
  void SetForeFade(int fade);
  vtkGetMacro(ForeFade, int);

  // Description:
  // Coloring label maps
  void SetLabelIndirectLUT(vtkIndirectLookupTable *lut);


  //--------- Volumes layered in the 3 slice windows -----------//

  // Description:
  // The None volume is a single slice, all 0's, used as input to 
  // the pipeline when no volume is selected.
  void SetNoneVolume(vtkMrmlDataVolume *vol);
  vtkGetObjectMacro(NoneVolume, vtkMrmlDataVolume);
  // Description:
  // The Back volume is the one displayed in the background slice layer
  void SetBackVolume(vtkMrmlDataVolume *vol);
  // Description:
  // The Fore volume is the one displayed in the foreground slice layer
  void SetForeVolume(vtkMrmlDataVolume *vol);
  // Description:
  // The Label volume is displayed in the label slice layer.
  // It is passed through a vtkImageLabelOutline filter which shows only
  // the outline of the labeled regions.
  void SetLabelVolume(vtkMrmlDataVolume *vol);

  void SetBackVolume( int s, vtkMrmlDataVolume *vol);
  void SetForeVolume( int s, vtkMrmlDataVolume *vol);
  void SetLabelVolume(int s, vtkMrmlDataVolume *vol);
  vtkMrmlDataVolume* GetBackVolume( int s) ;
  vtkMrmlDataVolume* GetForeVolume( int s) ;
  vtkMrmlDataVolume* GetLabelVolume(int s) ;


  //--------- Slice reformatting, orientation, point conversion  -----------//

  // Description:
  // Slice Orientation
  void SetOrient(int orient);
  void SetOrient(int s, int orient);
  void SetOrientString(char *str);
  void SetOrientString(int s, char *str);
  int GetOrient(int s) ;
  char *GetOrientString(int s);
  char *GetOrientList() ;

  // Description:
  // Slice Offset
  vtkFloatingPointType GetOffsetRangeLow(int s) ;
  vtkFloatingPointType GetOffsetRangeHigh(int s) ;
  void SetOffset(int s, vtkFloatingPointType offset);
  void InitOffset(int s, char *str, vtkFloatingPointType offset);
  vtkFloatingPointType GetOffset(int s) ;
  vtkFloatingPointType GetOffset(int s, char *str) ;

  // Description:
  // Matrix
  vtkMatrix4x4 *GetReformatMatrix(int s) ;
  void ComputeNTPFromCamera(vtkCamera *camera);
  void SetDirectNTP(vtkFloatingPointType nx, vtkFloatingPointType ny, vtkFloatingPointType nz,
    vtkFloatingPointType tx, vtkFloatingPointType ty, vtkFloatingPointType tz, vtkFloatingPointType px, vtkFloatingPointType py, vtkFloatingPointType pz);
  void SetDriver(int s, int d) ;
  int GetDriver(int s) ;
  double *GetP(int s);
  double *GetT(int s);
  double *GetN(int s);
  vtkGetVector3Macro(DirP, double);
  vtkGetVector3Macro(CamP, double);
  // user defined matrix
  void SetNewOrientNTP(int s, vtkFloatingPointType nx, vtkFloatingPointType ny, vtkFloatingPointType nz,
    vtkFloatingPointType tx, vtkFloatingPointType ty, vtkFloatingPointType tz, vtkFloatingPointType px, vtkFloatingPointType py, vtkFloatingPointType pz);
  // reformat matrix
  void SetReformatNTP(char *orientation, vtkFloatingPointType nx, vtkFloatingPointType ny, vtkFloatingPointType nz, vtkFloatingPointType tx, vtkFloatingPointType ty, vtkFloatingPointType tz, vtkFloatingPointType px, vtkFloatingPointType py, vtkFloatingPointType pz);

  // Points
  void SetReformatPoint(int s, int x, int y);
  void SetReformatPoint(vtkMrmlDataVolume *vol, vtkImageReformat *ref, int s, int x, int y);
  vtkGetVectorMacro(WldPoint, vtkFloatingPointType, 3);
  vtkGetVectorMacro(IjkPoint, vtkFloatingPointType, 3);
  void SetScreenPoint(int s, int x, int y);
  vtkGetVectorMacro(ReformatPoint, int, 2);
  vtkGetVectorMacro(Seed, int, 3);
  vtkGetVectorMacro(Seed2D, int, 3);


  //-------------------- Filter pipeline  -------------------------//

  // Description:
  // Convenient pipeline hook for developers.
  // First and last filters of any pipeline (or part of pipeline)
  // whose output should be displayed in a slice.
  // For example, this is used in Editor/EdThreshold.tcl for
  // dynamic thresholding.
  // This is for display only!  It can't be used to actually change
  // the volumes in the slicer.  Use the editor (vtkImageEditorEffects)
  // for that.
  void SetFirstFilter(int s, vtkImageToImageFilter *filter);
  // LastFilter is of type vtkImageSource, a superclass of 
  // both vtkImageToImage and vtkMultipleInput filters.
  void SetLastFilter(int s, vtkImageSource *filter);
  vtkImageToImageFilter* GetFirstFilter(int s) ;
  vtkImageSource* GetLastFilter(int s) ;

  // Description:
  // Whether to apply pipeline defined by first, last filter
  // to the Back slice
  vtkGetMacro(BackFilter, int);
  vtkSetMacro(BackFilter, int);
  vtkBooleanMacro(BackFilter, int);

  // Description:
  // Whether to apply pipeline defined by first, last filter
  // to the Fore slice
  vtkGetMacro(ForeFilter, int);
  vtkSetMacro(ForeFilter, int);
  vtkBooleanMacro(ForeFilter, int);

  // Description:
  // Whether to apply pipeline defined by first, last filter
  // only to Active slice or to all three slices
  vtkGetMacro(FilterActive, int);
  vtkSetMacro(FilterActive, int);
  vtkBooleanMacro(FilterActive, int);

  // Description:
  // Whether to overlay filter output on all layers 
  // or use it to replace the fore layer.
  vtkGetMacro(FilterOverlay, int);
  vtkSetMacro(FilterOverlay, int);
  vtkBooleanMacro(FilterOverlay, int);

  //-------------------- Additional Reformatting ---------------------------//
  // For developers: convenience functions that reformat volumes 
  // in the slicer.

  // Description:
  // Add a volume to the list we are reformatting.
  void AddVolumeToReformat(vtkMrmlDataVolume * v);

  // Description:
  // Call this to clear out the volumes when your module is exited.
  void RemoveAllVolumesToReformat();

  // Description:
  // Get the reformatted slice from this volume.  The volume
  // must have been added first.  Currently this reformats
  // along with the active slice in the Slicer.
  vtkImageData *GetReformatOutputFromVolume(vtkMrmlDataVolume *v) ;
  
  // Description:
  // Set reformat matrix same as that of this slice
  void ReformatVolumeLikeSlice(vtkMrmlDataVolume * v, int s);

  //-------------------- Draw ---------------------------//
  // Description:
  // For display of temporary drawing over slices.
  // 
  // This is for display only!  It can't be used to actually change
  // the volumes in the slicer.  The editor (vtkImageEditorEffects)
  // is used for that.
  //
  // Mainly Drawing is interaction with the vtkImageDrawROI object
  // PolyDraw.
  //
  void DrawSetColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b) ;
  void DrawSelectAll() ;
  void DrawDeselectAll() ;
  void DrawDeleteSelected() ;
  void DrawDeleteAll() ;
  void DrawInsert(int x, int y) ;
  void DrawShowPoints(int s) ;
  void DrawSetRadius(int r) ;
  int DrawGetRadius() ;
  void DrawInsertPoint(int x, int y) ;
  void DrawMoveInit(int x, int y) ;
  void DrawMove(int x, int y) ;
  int DrawGetNumPoints() ;
  int DrawGetNumSelectedPoints() ;
  void DrawStartSelectBox(int x, int y) ;
  void DrawDragSelectBox(int x, int y) ;
  void DrawEndSelectBox(int x, int y) ;
  vtkPoints* DrawGetPoints() 
  vtkPoints* DrawGetPointsInterpolated(int density) 
  void DrawComputeIjkPoints();
  void DrawComputeIjkPointsInterpolated(int density);
  void DrawComputeIjkPointsInterpolated(int window, int s, int p);
  vtkGetObjectMacro(DrawIjkPoints, vtkPoints);
  void DrawSetShapeToPolygon() ;
  void DrawSetShapeToLines() ;
  void DrawSetShapeToPoints() ;
  char* GetShapeString() ;

  void DrawSetSelectedPointColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b);
  void DrawSetShapeToCrosses() ;
  void DrawSetShapeToBoxes() ;
  void DrawSelectPoint(int x, int y) 
  void DrawDeselectPoint(int x, int y) 
  // (CTJ) To detect whether mouse (x,y) is "near" selected points
  int DrawIsNearSelected(int x, int y)
  
  void DrawSetClosed(int closed);
  
  void DrawSetHideSpline(int hide);
  

  void DrawSetStartMethod(void (*f)(void *), void *arg);
  void DrawSetStartMethodArgDelete(void (*f)(void *));

  vtkPoints* CopyGetPoints();
  
  void CopySetDrawPoints();

  // ---- Stack of polygons ---- //
  void StackSetPolygon(int window, int s, int d);

  vtkPoints* StackGetPoints(int window, int s);

  vtkPoints* StackGetPoints(int window, int s, int p);

  void StackSetPolygon(int window, int s, int p, int d, int closed, int preshape, int label);

  vtkPoints* RasStackSetPolygon(int window, int s, int p, int d, int closed, int preshape, int label);

  void StackRemovePolygon(int window, int s, int p);

  void RasStackRemovePolygon(int window, int s, int p);

  int StackGetNumberOfPoints(int window, int s);

  int StackGetInsertPosition(int window, int s);

  int StackGetNextInsertPosition(int window, int s, int p);

  int StackGetRetrievePosition(int window, int s);

  int StackGetNextRetrievePosition(int window, int s, int p);

  int StackGetPreshape(int window, int s, int p);

  int StackGetLabel(int window, int s, int p);

  int StackGetNumApplyable(int window, int s);

  int StackGetApplyable(int window, int s, int q);

  void StackClear(int window);

  // Necessary for calculating the ROI windowsize
  // TO DO: Add check for s
  int GetBackReformatResolution(int s);
  vtkImageDrawROI *GetImageDrawROI();


  vtkImageReformat *GetBackReformat(int s);


  //Hanifa
  vtkImageReformat *GetForeReformat(int s);
  vtkImageReformat *GetBackReformat3DView(int s);
  vtkImageReformat *GetForeReformat3DView(int s);
  vtkImageReformat *GetLabelReformat3DView(int s); 

  // Description:
  // Update any part of this class that needs it.
  // Call this if you are using the First, Last filter pipeline
  // and want it to execute.
  void Update();

  // Description:
  // 
  void ReformatModified() ;

  // Description:
  // return the version number of the compiler
 int GetCompilerVersion();
  // Description:
  // return the name of the compiler
  char *GetCompilerName();

  // Description:
  // return the vtk version
  char *GetVTKVersion();
    
    
};

#endif

