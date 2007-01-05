/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkTimeStampTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 01:23:28 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include "igstkTimeStamp.h"

int igstkTimeStampTest( int, char * [] )
{

  try
    {
    const double tolerance = 0.01; // Platform dependent ??
    
    typedef igstk::TimeStamp  TimeStampType;

    const double millisecondsToExpire = 2000.0;

    TimeStampType stamp;
    
    std::cout << "Testing the first Set" << std::endl;

    { // convenience block for local variable declarations.

    stamp.SetStartTimeNowAndExpireAfter( millisecondsToExpire );

    std::cout << stamp << std::endl;

    const double startingTime   = stamp.GetStartTime();
    const double expirationTime = stamp.GetExpirationTime();
    
    const double expectedExpirationTime = startingTime + millisecondsToExpire;

    if( fabs( expectedExpirationTime - expirationTime ) > tolerance )
      {
      std::cerr << "Error in expiration time" << std::endl;
      std::cerr << "Expected expiration time = " << expectedExpirationTime << std::endl;
      std::cerr << "Actual   expiration time = " << expirationTime << std::endl;
      return EXIT_FAILURE;
      }

    const double scheduledRenderTime1 = startingTime - 1000.0;
    const double scheduledRenderTime2 = startingTime - 1.0;
    const double scheduledRenderTime3 = startingTime + 1.0;
    const double scheduledRenderTime4 = startingTime + millisecondsToExpire / 2.0;
    const double scheduledRenderTime5 = expirationTime - 1.0;
    const double scheduledRenderTime6 = expirationTime + 1.0;
    const double scheduledRenderTime7 = expirationTime + 1000.0;

    if( stamp.IsValidAtTime( scheduledRenderTime1 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime2 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime3 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime4 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime5 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime6 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime7 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    } // end of first block
    
    std::cout << "Testing the second Set" << std::endl;

    { // convenience block for local variable declarations.

    stamp.SetStartTimeNowAndExpireAfter( millisecondsToExpire );

    const double startingTime   = stamp.GetStartTime();
    const double expirationTime = stamp.GetExpirationTime();
    
    const double expectedExpirationTime = startingTime + millisecondsToExpire;

    if( fabs( expectedExpirationTime - expirationTime ) > tolerance )
      {
      std::cerr << "Error in expiration time" << std::endl;
      std::cerr << "Expected expiration time = " << expectedExpirationTime << std::endl;
      std::cerr << "Actual   expiration time = " << expirationTime << std::endl;
      return EXIT_FAILURE;
      }

    const double scheduledRenderTime1 = startingTime - 1000.0;
    const double scheduledRenderTime2 = startingTime - 1.0;
    const double scheduledRenderTime3 = startingTime + 1.0;
    const double scheduledRenderTime4 = startingTime + millisecondsToExpire / 2.0;
    const double scheduledRenderTime5 = expirationTime - 1.0;
    const double scheduledRenderTime6 = expirationTime + 1.0;
    const double scheduledRenderTime7 = expirationTime + 1000.0;

    if( stamp.IsValidAtTime( scheduledRenderTime1 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime2 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime3 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime4 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( !stamp.IsValidAtTime( scheduledRenderTime5 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'true', but returned 'false'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime6 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    if( stamp.IsValidAtTime( scheduledRenderTime7 ) )
      {
      std::cerr << "Error in IsValidAtTime()." << std::endl; 
      std::cerr << "Expected value was 'false', but returned 'true'" << std::endl;
      return EXIT_FAILURE;
      }

    } // end of first block


    }
  catch(...)
    {
    std::cerr << "Exception catched !!!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED ! " << std::endl;

  return EXIT_SUCCESS;
}


