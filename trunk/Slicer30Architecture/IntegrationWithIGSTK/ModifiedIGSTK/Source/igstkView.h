/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkView.h,v $
  Language:  C++
  Date:      $Date: 2006/01/04 13:41:43 $
  Version:   $Revision: 1.24 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _igstkView_h
#define _igstkView_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// VTK headers
#include <vtkRenderWindowInteractor.h>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkIndent.h"
#include "vtkWorldPointPicker.h"
#include "vtkCommand.h"

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkPulseGenerator.h"
#include "igstkObjectRepresentation.h"   
#include "igstkEvents.h"   

namespace igstk{


/** \class View
 * 
 * \brief Base class for all View2D and View3D classes that display scenes in a
 * VTK controlled window attached to the GUI.
 *
 * \ingroup Object
 */

class View : public Fl_Gl_Window, public vtkRenderWindowInteractor 
{

public:
    
  typedef View      Self;
  typedef vtkRenderWindowInteractor Superclass;

  igstkTypeMacro( View, vtkRenderWindowInteractor );
  
  /** Set the desired frequency for refreshing the view. It is not worth to
   * attempt to go faster than your monitor, nor more than double than your
   * trackers */
  void RequestSetRefreshRate( double frequency );
  
  /** Add an observer to this View class */
  void AddObserver( const ::itk::EventObject & event, ::itk::Command * observer );
  
  /** Object representation types */
  typedef ObjectRepresentation::Pointer     ObjectPointer;
  typedef std::list< ObjectPointer >        ObjectListType; 
  typedef ObjectListType::iterator          ObjectListIterator;
  typedef ObjectListType::const_iterator    ObjectListConstIterator;

  /** Add an object representation to the list of children and associate it
   * with a specific view. */ 
  void RequestAddObject( ObjectRepresentation* object ); 

  /** Remove the object passed as arguments from the list of children, only if
   * it is associated to a particular view. */ 
  void RequestRemoveObject( ObjectRepresentation* object ); 

  /** Request to save a screen shot into a file. The file format MUST be PNG
   * in order to have lossless compression. This method will trigger an extra
   * rendering of the scene in order to ensure that the image is fresh.
   * */
  void RequestSaveScreenShot( const std::string & filename );

   /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
 
  void Print(std::ostream& os);

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

protected:

  View( int x, int y, int w, int h, const char *l="");
  virtual ~View( void );

  // vtkRenderWindowInteractor overrides made protected in order to prevent
  // users from using these methods.
  void Initialize();
  void Enable();
  void Disable();
  void SetRenderWindow(vtkRenderWindow *aren);
  void UpdateSize(int x, int y);
  void ResetCameraProcessing();
  void DisableInteractionsProcessing();
  void EnableInteractionsProcessing();

public:
  
  /** Update the display in order to render the new content of the scene*/
  void Update();
 
  /** Disable user interactions with the window via mouse and keyboard */
  void RequestDisableInteractions();

  /** Enable user interactions with the window via mouse and keyboard */
  void RequestEnableInteractions();

  /** Request to return the camera to a known position */
  void RequestResetCamera();
  
  /** Request Start the periodic refreshing of the view */
  void RequestStart();

  /** Request Stopping the periodic refreshing of the view */
  void RequestStop();

protected:
  
  // Fl_Gl_Window overrides
  void flush(void);
  void draw( void );
  void resize( int x, int y, int w, int h );
  virtual int  handle( int event );
   
  /** Get the vtk RenderWindow */
  vtkRenderWindow* GetRenderWindow() {return m_RenderWindow;}
    
  /** Get the vtk Renderer */
  vtkRenderer* GetRenderer() {return m_Renderer;}
    
  /** Default Camera */
  vtkCamera             * m_Camera;
 
private:
  
  vtkRenderWindow       * m_RenderWindow;
  vtkRenderer           * m_Renderer;

  typedef vtkWorldPointPicker  PickerType;
  PickerType            * m_PointPicker;
  
  bool                    m_InteractionHandling;

  
  /** Member variables for holding temptative arguments of functions.
   *  This is needed for implementing a layer of security that decouples
   *  user invokations from the actual state of this class */
  vtkProp            * m_ActorToBeAdded;
  vtkProp            * m_ActorToBeRemoved;
  
  typedef itk::SimpleMemberCommand< Self >   ObserverType;

  PulseGenerator::Pointer   m_PulseGenerator;
  ObserverType::Pointer     m_PulseObserver;
  ::itk::Object::Pointer    m_Reporter;

  /** List of the children object plug to the spatial object. */
  ObjectListType m_Objects; 

private:

  /** Methods that will only be invoked by the State Machine */

  /** Add and remove vtk Actors. Intended to be called only by the state
   * machine */
  void AddActorProcessing();
  void RemoveActorProcessing();

  /** Add and remove RepresentationObject classes */
  void AddObjectProcessing();
  void RemoveObjectProcessing();

  /** Method that will refresh the view.. and the GUI */
  void RefreshRender();

  void RequestAddActor( vtkProp * actor );
  void RequestRemoveActor( vtkProp * actor );
  
  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Save a screenshot of the current rendered scene */
  void SaveScreenShotProcessing();
  
  /** Reports when a filename for the screen shot is not valid */
  void ReportInvalidScreenShotFileNameProcessing();

  /** This should be called by the state machine */
  void StartProcessing();
  void StopProcessing();
  

private:
  
  // Arguments for methods to be invoked by the state machine.
  //
  ObjectRepresentation::Pointer m_ObjectToBeAdded;
  ObjectRepresentation::Pointer m_ObjectToBeRemoved;
  ObjectListType::iterator      m_IteratorToObjectToBeRemoved;
  std::string                   m_ScreenShotFileName;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidAddActor );
  igstkDeclareInputMacro( NullAddActor );
  igstkDeclareInputMacro( ValidRemoveActor );
  igstkDeclareInputMacro( NullRemoveActor );
  igstkDeclareInputMacro( ValidAddObject );
  igstkDeclareInputMacro( NullAddObject );
  igstkDeclareInputMacro( ExistingAddObject );
  igstkDeclareInputMacro( ValidRemoveObject );
  igstkDeclareInputMacro( InexistingRemoveObject );
  igstkDeclareInputMacro( NullRemoveObject );
  igstkDeclareInputMacro( ResetCamera );
  igstkDeclareInputMacro( EnableInteractions );
  igstkDeclareInputMacro( DisableInteractions );
  igstkDeclareInputMacro( StartRefreshing );
  igstkDeclareInputMacro( StopRefreshing );
  igstkDeclareInputMacro( ValidScreenShotFileName );
  igstkDeclareInputMacro( InvalidScreenShotFileName );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( Refreshing );

};

std::ostream& operator<<(std::ostream& os, View& o);

} // end namespace igstk

#endif
