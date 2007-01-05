/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkViewRefreshRateTest.cxx,v $
  Language:  C++
  Date:      $Date: 2006/01/04 13:41:43 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// 
//  The purpose of this test is to verify that a view is actually refreshed
//  at a rate close to the one that was requested by the application developer.
//
//


#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "itkRealTimeClock.h"

#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"

namespace ViewRefreshRateTest
{
  class ViewObserver : public ::itk::Command 
  {
  public:
    typedef  ViewObserver   Self;
    typedef  ::itk::Command    Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    ViewObserver() 
      {
      m_PulseCounter = 0;
      m_NumberOfPulsesToStop = 10;
      m_Form = 0;
      m_View = 0;
      }
  public:

    void SetForm( Fl_Window * form )
      {
      m_Form = form;
      }

    void SetEndFlag( bool * end )
      {
      m_End = end;
      }

    void Execute(const itk::Object *caller, const itk::EventObject & event)
      {
      std::cerr << "Execute( const * ) should not be called" << std::endl;         
      }

    void SetView( ::igstk::View * view )
    {
    m_View = view;
    if( m_View )
      {
      m_View->AddObserver( ::igstk::RefreshEvent(), this );
      }
    }

    void SetNumberOfPulsesToStop( unsigned long number )
      {
      m_NumberOfPulsesToStop = number;
      }

    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
      if( ::igstk::RefreshEvent().CheckEvent( &event ) )
        {
        m_PulseCounter++;

        if( m_PulseCounter > m_NumberOfPulsesToStop )
          {
          if( m_View )
            {
            m_View->RequestStop();
            } 
          else
            {
            std::cerr << "View pointer is NULL " << std::endl;
            }
          if( m_Form )
            {
            m_Form->hide();
            }
          *m_End = true;
          return;
          }
        }
      }
  private:
    unsigned long       m_PulseCounter;
    unsigned long       m_NumberOfPulsesToStop;
    Fl_Window          *m_Form;
    ::igstk::View      *m_View;
    bool *              m_End;
  };



}


int igstkViewRefreshRateTest( int, char * [] )
{

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool result = true;

  try
    {

    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(601,301,"View3D Test");
    
    View2DType * view2D = new View2DType( 10,10,280,280,"2D View");
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");

    form->end();
    // End of the GUI creation

    form->show();
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
 
    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );
    
    view2D->RequestDisableInteractions();
    view3D->RequestDisableInteractions();

    bool bEnd = false;

    typedef ViewRefreshRateTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view2D );
    viewObserver->SetForm( form );
    viewObserver->SetEndFlag( &bEnd );

    // Exercise the code for resizing the window
    form->resize(100, 100, 600, 300);

    const float refreshRate = 20;
    const float expectedNumberOfSeconds = 20;
    const unsigned long numberOfPulsesToStop = 
      static_cast< unsigned long >( refreshRate * expectedNumberOfSeconds );

    // Set the refresh rate for the view that we are validating
    view2D->RequestSetRefreshRate( refreshRate );

    // Make it compete with another view that refreshes at double the rate.
    view3D->RequestSetRefreshRate( refreshRate * 2 );

    viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
    
    view2D->RequestStart();
    view3D->RequestStart();

    // Manually trigger the first redraw in order to discard any initialization
    // time of the graphics.
    Fl::check();

    // Now go for the actual measurement of the refresh rate
    itk::RealTimeClock::Pointer realTimeClock = itk::RealTimeClock::New();
    
    const double beginTime = realTimeClock->GetTimeStamp();
    
    while(1)
      {
      Fl::wait(0.1);
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }


    const double endTime   = realTimeClock->GetTimeStamp();

    const double secondsElapsed = endTime - beginTime;

    const float actualRate = numberOfPulsesToStop / secondsElapsed;

    std::cout << "selected refresh rate   = " << refreshRate << std::endl;
    std::cout << "actual   refresh rate   = " << actualRate  << std::endl;

    //
    //   Give it a tolerance of 1%
    //
    if( fabs( actualRate - refreshRate ) / refreshRate > 0.01 )
      {
      std::cerr << "Refresh Rate did not match the expected value" << std::endl;
      std::cerr << "secondsElapsed          = " << secondsElapsed << std::endl;
      std::cerr << "expectedNumberOfSeconds = " << expectedNumberOfSeconds << std::endl;
      result = false ;
      }

  
    // at this point the observer should have hid the form

    delete view2D;
    delete view3D;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    result = false;
    }



  if( !result )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


