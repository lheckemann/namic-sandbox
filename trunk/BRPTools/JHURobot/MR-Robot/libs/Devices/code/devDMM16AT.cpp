/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include "devDMM16AT.h"

#include <cisstCommon/cmnXMLPath.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <limits.h>


CMN_IMPLEMENT_SERVICES(devDMM16AT);


devDMM16AT::devDMM16AT(const std::string & deviceName, bool doInit):
    mtsDevice(deviceName)
{
    mtsInterfaceProvided *provided = AddInterfaceProvided("MainInterface");
    if (provided) {
        provided->AddCommandRead(&devDMM16AT::GetInputAll, this,
                                 "GetInputAll", mtsDoubleVec(16));
    }

//GSF
    // Only once per program, not per board
    if (doInit)
        dscInit( DSC_VERSION );
}

devDMM16AT::~devDMM16AT() {
    free(ADC_samples);
    dscFree();
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void devDMM16AT::Configure(const std::string &filename) {
    
    cmnXMLPath xmlConfig;
    if (filename == "") {
        CMN_LOG_CLASS_INIT_WARNING << "Warning, could not configure DMM16AT device" << std::endl;
        return;
    }
    
    struct stat st;
    if (stat(filename.c_str(), &st) < 0) {
        CMN_LOG_CLASS_INIT_WARNING << "Invalid filename!! " << filename << std::endl;
        return;
    }
    
    xmlConfig.SetInputSource(filename);
    //char path[60];
    std::string context("/Config/Device[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS_INIT_VERBOSE << "Configuring DMM16AT Board with \"" << filename << "\"" << std::endl;
    //read in the base address and axis factors
    xmlConfig.GetXMLValue(context.c_str(), "@BaseAddress", BaseAddress);
    //int SelectorSwitchPosition;
    //if (ret == false) {
    //    // try switch selector
    //    xmlConfig.GetXMLValue(context.c_str(), "@SelectorSwitchPosition", SelectorSwitchPosition);
    //    BaseAddress = 0x200 + SelectorSwitchPosition * 0x20;
    //}
    
    //xmlConfig.GetXMLValue(context.c_str(), "@StartChan", LowChannel);
    //xmlConfig.GetXMLValue(context.c_str(), "@EndChan", HighChannel);
//TEMP
    LowChannel = 0;
    HighChannel = 15;
    CMN_LOG_CLASS_INIT_VERBOSE << "BaseAddress: " << BaseAddress << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Low Channel: " << LowChannel << " ,  High Channel: " << HighChannel << std::endl;

    /////
    // Retrieve these settings from config file
    /////
    // Initialize board
    //dsccb.base_address = 0x300;
    dsccb.base_address = BaseAddress;
    dsccb.int_level = 3;
    dscInitBoard(DSC_DMM16AT, &dsccb, &dscb);

    memset(&dscadsettings, 0, sizeof(DSCADSETTINGS));

    // Initialize settings
    dscadsettings.range = RANGE_10;
    dscadsettings.polarity = BIPOLAR;
    dscadsettings.gain = GAIN_1;
    dscadsettings.load_cal = (BYTE)FALSE;
    dscadsettings.current_channel = 0;
    BYTE result = dscADSetSettings( dscb, &dscadsettings );

    memset(&dscadscan, 0, sizeof(DSCADSCAN));
    //dscadscan.low_channel = 0;
    //dscadscan.high_channel = 15;
    dscadscan.low_channel = LowChannel;
    dscadscan.high_channel = HighChannel;
    dscadscan.gain = dscadsettings.gain ;

    ADC_samples = (DSCSAMPLE*)malloc( sizeof(DSCSAMPLE) * 16 );

    CMN_LOG_CLASS_INIT_VERBOSE << "Configured DMM16AT" << std::endl;
}

//GSF
void devDMM16AT::GetInputAll(mtsDoubleVec &input) const
{
    int limit = 16;
    if (input.size() > limit)
        CMN_LOG_CLASS_INIT_VERBOSE << "GetInputAll: input vector too large, size = " << input.size() << std::endl;
    else if (input.size() < limit) {
        CMN_LOG_CLASS_INIT_VERBOSE << "GetInputAll: input vector too small, size = " << input.size() << std::endl;
        limit = input.size();
    }
    // Cast "this" to non-const pointer to call the dscud.h functions, which do not declare
    // any parameters as "const" (the last parameter to dscADScan really can't be const anyway,
    // unless it is changed to a local variable).
    devDMM16AT *this2 = const_cast<devDMM16AT*>(this);
    CMN_ASSERT(this2);

    BYTE result = dscADScan(dscb, &(this2->dscadscan), this2->ADC_samples);
#if 0
    if (result != DE_NONE)
        CMN_LOG_CLASS_INIT_VERBOSE << "dscADScan returns " << (int)result << std::endl;
#endif

    double ADC_voltage;
    int ctr=0;
    for(int i=0; i<limit; i++){
        if ( (i>=LowChannel) && (i<=HighChannel) ){
            dscADCodeToVoltage(dscb, dscadsettings, dscadscan.sample_values[ctr], &ADC_voltage);
            ctr++;
            input[i] = ADC_voltage;
        } else {
            input[i] = 0;
        }
    }
}

