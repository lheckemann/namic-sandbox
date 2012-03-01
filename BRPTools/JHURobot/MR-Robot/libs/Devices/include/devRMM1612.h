/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*!
  \file
  \brief Defines the RMM1612 device 
  \ingroup cisstDevices
*/


#ifndef _devRMM1612_h
#define _devRMM1612_h

#include <cisstConfig.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsVector.h>

//GSF
#include <dscud.h>


/*!
  \ingroup cisstDevices
  A specific device class for RMM1612 card.
 */
class devRMM1612: public mtsComponent {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Mimimum value of DAC analog output, set by jumper on board */
    int DACRangeLow;

    /*! Maximum value of DAC analog output, set by jumper on board */
    int DACRangeHigh;

    /*! Base address of the board. */
    int BaseAddress;

    
    //GSF
    DSCB dscb;          // handle used to refer to the board
    DSCCB dsccb;        // structure containing board settings
    DSCDACS dscdacs;    // structure containing D/A conversion settings
    
public:
    /*! Constructor. Prepares the maps. */
    devRMM1612(const std::string & deviceName = "RMM1612", bool doInit = true);
    
    /*! Default destructor. Disables motor power. */
    virtual ~devRMM1612();
    
    /*! Prepares the board for reading and writing. Sets up some registers on
      the board. */
    void Configure(const std::string &filename = "");

 protected:
    //GSF
    /*! Set all channels on the DAC  */
    void SetOutputAll(const mtsDoubleVec &output);
};


CMN_DECLARE_SERVICES_INSTANTIATION(devRMM1612);


#endif // _devRMM1612_h
