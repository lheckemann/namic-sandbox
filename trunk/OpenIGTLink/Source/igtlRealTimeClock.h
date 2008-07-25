/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRealTimeClock.h,v $
  Language:  C++
  Date:      $Date: 2006-05-10 20:27:16 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlRealTimeClock_h
#define __igtlRealTimeClock_h

#include <igtlMacro.h>
#include <igtlObject.h>
#include <igtlObjectFactory.h>

namespace igtl
{

/** \class RealTimeClock
* \brief The RealTimeClock provides a timestamp from a real-time clock
*
* This class represents a real-time clock object
* and provides a timestamp in platform-independent format.
*
* \author Hee-Su Kim, Compute Science Dept. Kyungpook National University,
*                     ISIS Center, Georgetown University.
*/

class IGTLCommon_EXPORT RealTimeClock : public Object
{
public:
  typedef RealTimeClock Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  igtlTypeMacro(RealTimeClock, Object);

  /** Method for creation through the object factory */
  igtlNewMacro(Self);

  /** Define the type for the timestap */
  typedef double        TimeStampType;

  /** Define the type for the frequency of the clock */
  typedef double        FrequencyType;

  /** Returns a timestamp in seconds   e.g. 52.341243 seconds */
  TimeStampType GetTimeStamp() const;
  TimeStampType GetTimestamp() const
    {
    igtlWarningMacro("This call is deprecated. "
        "Its naming was not conforming to IGTL Style. "
        "Please use GetTimeStamp() instead. Note the capital S");
    return this->GetTimeStamp();
    }

  /** Returns the frequency of a clock */
  igtlGetConstMacro(Frequency, FrequencyType);

protected:

  /** constructor */
  RealTimeClock();

  /** destructor */
  virtual ~RealTimeClock();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os) const;

private:
  FrequencyType    m_Frequency;
  TimeStampType    m_Difference;
  TimeStampType    m_Origin;
};

} // end of namespace igtl

#endif  // __igtlRealTimeClock_h
