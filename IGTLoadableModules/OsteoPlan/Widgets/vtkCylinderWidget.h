/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCylinderWidget.h
  Language:  C++
  RCS:   $Id: vtkCylinderWidget.h 1.3 2005/01/18 12:53:55 Goodwin Exp Goodwin $

  Copyright (c) 2003-2004 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.

=========================================================================*/
// .NAME vtkCylinderWidget - orthogonal hexahedron 3D widget
// .SECTION Description
// This 3D widget defines a region of interest that is represented by an
// arbitrarily oriented hexahedron with interior face angles of 90 degrees
// (orthogonal faces). The object creates 3 handles that can be moused on and
// manipulated. The first two correspond to the two faces, the third is in
// the center of the hexahedron.
//
// A nice feature of the object is that the vtkCylinderWidget, like any 3D widget,
// will work with the current interactor style. That is, if vtkCylinderWidget does
// not handle an event, then all other registered observers (including the
// interactor style) have an opportunity to process the event. Otherwise, the
// vtkCylinderWidget will terminate the processing of the event that it handles.
//
// To use this object, just invoke SetInteractor() with the argument of the
// method a vtkRenderWindowInteractor.  You may also wish to invoke
// "PlaceWidget()" to initially position the widget. The interactor will act
// normally until the "i" key (for "interactor") is pressed, at which point the
// vtkCylinderWidget will appear. (See superclass documentation for information
// about changing this behavior.) By grabbing the six face handles (use the
// left mouse button), faces can be moved. By grabbing the center handle
// (with the left mouse button), the entire hexahedron can be
// translated. (Translation can also be employed by using the
// "shift-left-mouse-button" combination inside of the widget.) Scaling is
// achieved by using the right mouse button "up" the render window (makes the
// widget bigger) or "down" the render window (makes the widget smaller). To
// rotate vtkCylinderWidget, pick a face (but not a face handle) and move the left
// mouse. (Note: the mouse button must be held down during manipulation.)
// Events that occur outside of the widget (i.e., no part of the widget is
// picked) are propagated to any other registered obsevers (such as the
// interaction style).  Turn off the widget by pressing the "i" key again.
// (See the superclass documentation on key press activiation.)
//
// The vtkCylinderWidget is very flexible. It can be used to select, cut, clip, or
// perform any other operation that depends on an implicit function (use the
// GetPlanes() method); or it can be used to transform objects using a linear
// transformation (use the GetTransform() method). Typical usage of the
// widget is to make use of the StartInteractionEvent, InteractionEvent, and
// EndInteractionEvent events. The InteractionEvent is called on mouse
// motion; the other two events are called on button down and button up 
// (either left or right button).
//
// Some additional features of this class include the ability to control the
// rendered properties of the widget. You can set the properties of the
// selected and unselected representations of the parts of the widget. For
// example, you can set the property for the handles, faces, and outline in
// their normal and selected states.

// .SECTION Caveats
// Note that handles can be picked even when they are "behind" other actors.
// This is an intended feature and not a bug.
//
// The cylinder widget can be oriented by specifying a transformation matrix.
// This transformation is applied to the initial bounding box as defined by
// the PlaceWidget() method. DO NOT ASSUME that the transformation is applied
// to a unit box centered at the origin; this is wrong!

// .SECTION See Also
// vtk3DWidget vtkPointWidget vtkLineWidget vtkPlaneWidget 
// vtkImplicitPlaneWidget vtkImagePlaneWidget

#ifndef __vtkCylinderWidget_h
#define __vtkCylinderWidget_h

#include "vtkOsteoPlanWin32Header.h"

#include "vtkPolyDataSourceWidget.h"
//#include "vtkBioengConfigure.h" // Include configuration header.
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"

class vtkActor;
class vtkCellPicker;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkProp;
class vtkProperty;
class vtkSphereSource;
class vtkTransform;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkPlane;
class vtkCylinder;
class vtkPolyDataNormals;
class vtkPlane;
class vtkConeSource;
class vtkLineSource;

class VTK_OsteoPlan_EXPORT vtkCylinderWidget : public vtkPolyDataSourceWidget
{
public:
  // Description:
  // Instantiate the object.
  static vtkCylinderWidget *New();

  vtkTypeRevisionMacro(vtkCylinderWidget,vtk3DWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax, double ymin, double ymax, 
                   double zmin, double zmax)
    {this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}

  // Description:
  // Set/Get the InsideOut flag. When off, the normals point out of the
  // box. When on, the normals point into the hexahedron.  InsideOut
  // is off by default.
  vtkSetMacro(InsideOut,int);
  vtkGetMacro(InsideOut,int);
  vtkBooleanMacro(InsideOut,int);

  // Description:
  // Get the direction of the cylinder
  vtkGetVectorMacro(Direction,double,3);
  
  // Description:
  // Get the height of the cylinder
  vtkGetMacro(Height,double);
  void SetHeight(double height){Cylinder->SetHeight(height);};
  
  // Description:
  // Get the radius of the cylinder
  vtkGetMacro(Radius,double);
  void SetRadius(double radius){Cylinder->SetRadius(radius);};

  // Description:
  // Grab the polydata (including points) that define the box widget. The
  // polydata consists of 6 quadrilateral faces and 15 points. The first
  // eight points define the eight corner vertices; the next six define the
  // -x,+x, -y,+y, -z,+z face points; and the final point (the 15th out of 15
  // points) defines the center of the hexahedron. These point values are
  // guaranteed to be up-to-date when either the InteractionEvent or
  // EndInteractionEvent events are invoked. The user provides the
  // vtkPolyData and the points and cells are added to it.
  void GetPolyData(vtkPolyData *pd);
  
  // Description:
  // Satisfies superclass API.  This returns a pointer to the underlying
  // PolyData.  Make changes to this before calling the initial PlaceWidget()
  // to have the initial placement follow suit.  Or, make changes after the
  // widget has been initialised and call UpdatePlacement() to realise.
  //vtkPolyDataSource* GetPolyDataSource();

  // Description:
  // Get the handle properties (the little balls are the handles). The 
  // properties of the handles when selected and normal can be 
  // set.
  vtkGetObjectMacro(HandleProperty,vtkProperty);
  vtkGetObjectMacro(SelectedHandleProperty,vtkProperty);

  // Description:
  // Switches handles (the cones) on or off by manipulating the actor
  // visibility.
  void HandlesOn();
  void HandlesOff();
  
    // Description:
  // Switches cylinder on or off by manipulating the actor
  // visibility. The widget can then be used as a vector widget!!
  void CylinderOn();
  void CylinderOff();
  
  // Description:
  // Get an implicit representation of the cylinder
  void GetCylinder(vtkCylinder *cylinder);
 
  // Description:
  // Get an implicit representation of the the first end plane
  void GetPlane1(vtkPlane *plane1);
  
  // Description:
  // Get an implicit representation of the second end plane
  void GetPlane2(vtkPlane *plane2);
  
  // Description:
  // Set/Get the position of first end point.
  void SetPoint1(double x, double y, double z);
  void SetPoint1(double x[3]) 
    {this->SetPoint1(x[0], x[1], x[2]); }
  double* GetPoint1() 
    {return this->ConeSource->GetCenter();}
  void GetPoint1(double xyz[3]) 
    {this->ConeSource->GetCenter(xyz);}

  // Description:
  // Set position of other end point.
  void SetPoint2(double x, double y, double z);
  void SetPoint2(double x[3]) 
    {this->SetPoint2(x[0], x[1], x[2]);}
  double* GetPoint2() 
    {return this->ConeSource2->GetCenter();}
  void GetPoint2(double xyz[3]) 
    {this->ConeSource2->GetCenter(xyz);}

  // Description:
  // Satisfies superclass API.  This returns a pointer to the underlying
  // PolyData.  Make changes to this before calling the initial PlaceWidget()
  // to have the initial placement follow suit.  Or, make changes after the
  // widget has been initialised and call UpdatePlacement() to realise.
  vtkPolyDataAlgorithm* GetPolyDataAlgorithm();
     
  // Description:
  // Satisfies superclass API.  This will change the state of the widget to
  // match changes that have been made to the underlying PolyDataSource
  void UpdatePlacement(void);
  
  // Description:
  // Control the behavior of the widget. Translation, rotation, and
  // scaling can all be enabled and disabled.
  vtkSetMacro(TranslationEnabled,int);
  vtkGetMacro(TranslationEnabled,int);
  vtkBooleanMacro(TranslationEnabled,int);
  vtkSetMacro(ScalingEnabled,int);
  vtkGetMacro(ScalingEnabled,int);
  vtkBooleanMacro(ScalingEnabled,int);
  vtkSetMacro(RotationEnabled,int);
  vtkGetMacro(RotationEnabled,int);
  vtkBooleanMacro(RotationEnabled,int);
  
  // Description:
  // Enable/disable mouse interaction so the widget remains on display.
  void SetInteraction(int interact);
  vtkGetMacro(Interaction,int);
  vtkBooleanMacro(Interaction,int);

  virtual void SizeHandles();
  virtual void PositionHandles();

protected:
  vtkCylinderWidget();
  ~vtkCylinderWidget();

//BTX - manage the state of the widget
  int State;
  enum WidgetState
  {
    Start=0,
    Moving,
    Scaling,
    Outside
  };
//ETX
    
  // Handles the events
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);
                            
  // internal utility method that adds observers to the RenderWindowInteractor
  // so that our ProcessEvents is eventually called.  this method is called
  // by SetEnabled as well as SetInteraction
  void AddObservers();

  // ProcessEvents() dispatches to these methods.
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  
  // the geometry
  vtkCylinderSource *Cylinder;
  vtkPolyDataNormals *CylNormals;
  vtkActor          *CylActor;
  vtkPolyDataMapper *CylMapper;
  vtkPolyData       *CylPolyData;
  
  // glyphs representing hot spots (e.g., handles)
  vtkActor          *Handle;
  vtkPolyDataMapper *HandleMapper;
  vtkSphereSource   *HandleGeometry;
  
  // the normal cone
  vtkActor          *ConeActor;
  vtkPolyDataMapper *ConeMapper;
  vtkConeSource     *ConeSource;

  // the normal line
  vtkActor          *LineActor;
  vtkPolyDataMapper *LineMapper;
  vtkLineSource     *LineSource;

  // the normal cone
  vtkActor          *ConeActor2;
  vtkPolyDataMapper *ConeMapper2;
  vtkConeSource     *ConeSource2;

  // the normal line
  vtkActor          *LineActor2;
  vtkPolyDataMapper *LineMapper2;
  vtkLineSource     *LineSource2;


  int HighlightHandle(vtkProp *prop); //returns cell id
  void HighlightOutline(int highlight);

  

  // Do the picking
  vtkCellPicker *HandlePicker;
  vtkCellPicker *CylPicker;
  vtkActor *CurrentHandle;
  
  // Methods to manipulate the cylinder.
  virtual void Translate(double *p1, double *p2);
  virtual void Scale(double *p1, double *p2, int X, int Y);
  virtual void Rotate(int X, int Y, double *p1, double *p2, double *vpn);
  void Elongate(double *p1, double *p2);
  
  // Transform the points 
  vtkTransform *RotationTransform;
  vtkTransform *TranslationTransform;
  vtkTransform *Transform;
  vtkTransformPolyDataFilter *TransformFilter;
  
  
  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *HandleProperty;
  vtkProperty *SelectedHandleProperty;
  vtkProperty *OutlineProperty;
  vtkProperty *SelectedOutlineProperty;
  vtkProperty *FaceProperty;
  vtkProperty *SelectedFaceProperty;
  
  void CreateDefaultProperties();
  
  // Control the orientation of the normals
  int InsideOut;
  
  double Direction[3];
  double Height;
  double Radius;
  
  // Control whether scaling, rotation, and translation are supported
  int TranslationEnabled;
  int ScalingEnabled;
  int RotationEnabled;
  int   Interaction; // Is the widget responsive to mouse events  
private:
  vtkCylinderWidget(const vtkCylinderWidget&);  //Not implemented
  void operator=(const vtkCylinderWidget&);  //Not implemented
};

#endif
