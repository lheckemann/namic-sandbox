/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*!
  \file
  \brief Defines the DMM16AT device 
  \ingroup cisstDevices
*/


#ifndef _devDMM16AT_h
#define _devDMM16AT_h

#include <cisstConfig.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsVector.h>

//GSF
#include <dscud.h>


/*!
  \ingroup cisstDevices
  A specific device class for DMM16AT card.
 */
class devDMM16AT: public mtsComponent {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Starting number of ADC channel used on the board. */
    int LowChannel;

    /*! Ending number of ADC channel used on the board. */
    int HighChannel;

    /*! Base address of the board. */
    int BaseAddress;

    //GSF
    DSCSAMPLE* ADC_samples;
    DSCB dscb;      // handle used to refer to the board
    DSCCB dsccb;    // structure containing board settings
    DSCADSCAN dscadscan;         // structure containing A/D scan settings
    DSCADSETTINGS dscadsettings; // structure containing A/D conversion settings
    
public:

    /*! Constructor. Prepares the maps. */
    devDMM16AT(const std::string & deviceName = "DMM16AT", bool doInit = true);
    
    /*! Default destructor. Disables motor power. */
    virtual ~devDMM16AT();
    
    /*! Prepares the board for reading and writing. Sets up some registers on
      the board. */
    void Configure(const std::string &filename = "");

 protected:

    //GSF
    /*! Capture all channels on the ADC.
        Return the 16 voltages. */
    void GetInputAll(mtsDoubleVec &input) const;
};


CMN_DECLARE_SERVICES_INSTANTIATION(devDMM16AT);


#endif // _devDMM16AT_h
