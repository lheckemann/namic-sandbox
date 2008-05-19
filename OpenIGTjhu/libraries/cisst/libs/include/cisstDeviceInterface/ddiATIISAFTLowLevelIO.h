/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIISAFTLowLevelIO.h,v 1.5 2006/02/24 19:32:46 pkaz Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


/*!
  \file
  \brief Defines the low level IOs for the ISA ATI Force/Torque sensor.
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiATIISAFTLowLevelIO_h
#define _ddiATIISAFTLowLevelIO_h

#include <cisstConfig.h>
#if CISST_HAS_ATIISAFT || defined DOXYGEN

#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
#include <conio.h>
#define outw(data, address)   outpw(address, data)
#define inw(address)          inpw(address)
#else
#include <sys/io.h>
#include <unistd.h>
#endif

/* REAL dumb, but cant help it */
#define TRUE     1
#define FALSE     0

/* I/O Registers Offsets */
#define ATIISAFT_ADDR_REG       0x0
#define ATIISAFT_DATA_REG       0x2
#define ATIISAFT_STATUS_REG       0x4

/* ATI F/T Commands */

#define  ATIFT_CMD_SENSOR_DATA    0x0100
#define  ATIFT_CMD_CARD_DATA    0x0200
#define  ATIFT_CMD_BIAS_ARRAY    0x0300
#define  ATIFT_CMD_GET_PEAK_VALUES  0x0400
#define  ATIFT_CMD_RESET    0x0500
#define  ATIFT_CMD_CALIBRATION_MATRIX  0x0600
#define  ATIFT_CMD_MONITOR    0x0700
#define  ATIFT_CMD_IO      0x0800
#define  ATIFT_CMD_INFO      0x0A00

/* ATI F/T Command/Outputs Offset */
#define  ATIFT_CMD_BUFFER_OFFSET  0x00
#define  ATIFT_CMD_ISA_TO_PC_MAILBOX  0x3FE
#define  ATIFT_CMD_PC_TO_ISA_MAILBOX  0x3FF

/* some masks -- RTFM ATI ISA F/T-16 Card Hardware Interface */
#define  ATIFT_ACK_BIT      0x0010
#define  ATIFT_INTERRUPT_BIT    0x0080
#define  ATIFT_CLEAR_STATUS_REGISTER  0x01

/* some command specific constants */
/* Command: Sensor Data */
#define  ATIFT_CMD_SENSOR_DATA_READ_MODE   0x00
#define  ATIFT_CMD_SENSOR_DATA_WRITE_MODE   0x0F
#define  ATIFT_CMD_SENSOR_DATA_DATA_SIZE   147
#define  ATIFT_CMD_SENSOR_DATA_CNTS_PER_FORCE_OFFSET   139
#define  ATIFT_CMD_SENSOR_DATA_CNTS_PER_TORQUE_OFFSET   140
#define  ATIFT_CMD_SENSOR_DATA_UNITS_OFFSET   145

/* Command: Card Data */
#define  ATIFT_CMD_CARD_DATA_READ_MODE     0x00
#define  ATIFT_CMD_CARD_DATA_WRITE_MODE     0x01
#define  ATIFT_CMD_CARD_DATA_DISABLE_INTERRUPTS  0x00
#define ATIFT_CMD_CARD_DATA_ENABLE_INTERRUPTS  0x01
#define ATIFT_CMD_CARD_DATA_FILTER_MASK_INV  0xFFF8
#define ATIFT_CMD_CARD_DATA_FILTER_MASK    0x0007
#define ATIFT_CMD_CARD_DATA_BIAS_MASK    0x0008
#define ATIFT_CMD_CARD_DATA_TEMP_MASK    0x0010
#define ATIFT_CMD_CARD_DATA_CALIBRATION_MASK  0x0020
#define ATIFT_CMD_CARD_DATA_PEAK_MASK    0x0040
#define ATIFT_CMD_CARD_DATA_MONITORING_MASK  0x0080
#define ATIFT_CMD_CARD_DATA_SAT_CHK_MASK  0x0100

/* Command: Bias Array */
#define ATIFT_CMD_BIAS_ARRAY_READ     0x00
#define ATIFT_CMD_BIAS_ARRAY_NEW    0x01
#define ATIFT_CMD_BIAS_ARRAY_WRITE     0x02

/* Command: Get Peak Values */
#define ATIFT_CMD_PEAK_DATA_DONT_RESET     0x00
#define ATIFT_CMD_PEAK_DATA_RESET     0x01
#define ATIFT_CMD_PEAK_DATA_MIN_VALUES_OFFSET   2
#define ATIFT_CMD_PEAK_DATA_MAX_VALUES_OFFSET   14

/* Command: Output Information */
#define ATIFT_CMD_INFO_READ_MODE    0x00
#define ATIFT_CMD_INFO_WRITE_MODE     0x0F

/*
  RTFM ATI ISA F/T-16 Card Hardware Interface for why, how, etc.
  these structures exist and what they do
 */
extern "C" {
  extern unsigned short __ATIISAFT_BaseAddress;

  extern struct OUTPUT_INFO __output_info;
  extern struct SENSOR_DATA __sensor_data;
  extern struct CARD_DATA __card_data;
  extern struct PEAK_DATA __peak_data;

  struct OUTPUT_INFO {
    unsigned short  cmd_rc;
    unsigned short  mode;
    unsigned short  units;
    unsigned short  resolution;
    unsigned short  cnts_per_force;
    unsigned short  cnts_per_torque;
    unsigned short  cpf_div; // reserved for internal use
    unsigned short  cpt_div; // reserved for internal use
  };

  struct CARD_DATA {
    unsigned short cd_cmd_rc;
    unsigned short cd_mode;
    unsigned short cd_flags;
    unsigned short cd_mask;
    unsigned short cd_interrupts;
    unsigned short cd_sample_rate;
    unsigned short cd_model;
    unsigned short cd_microcode_version;
    unsigned short cd_serial_number;
  };

  struct SENSOR_DATA {
    unsigned short sd_cmd_rc;
    unsigned short sd_mode;
    unsigned short sd_matrix[96];       // 6x8x 2words
    unsigned short sd_sbias[8];
    signed short sd_Mm[8];
    signed short sd_Bm[8];
    signed short sd_Mb[8];
    signed short sd_Bb[8];
    unsigned short sd_ngages;         // offset 138
    unsigned short sd_counts_lb;
    unsigned short sd_counts_lb_in;
    unsigned short sd_force;
    unsigned short sd_torque;
    unsigned short sd_temp_max;
    unsigned short sd_temp_min;
    unsigned short sd_units;
    unsigned short sd_serial;
  };

  struct PEAK_DATA {
    long mindata[6];
    long maxdata[6];
  };

  int  send_command (unsigned short *bptr, int numwords);
  void  receive_command (unsigned short *bptr, int numword);
  int  get_output_info (void);
  int  get_card_data (void);
  /* *DO NOT* use these two if get_output_info suffices, use a lot more I/O */
  int  get_sensor_data (void);
  int  get_counts_info (unsigned short *cnts_force, unsigned short *cnts_torque, unsigned short *units);
  int  set_new_bias (void);
  // PKAZ:  renamed following to avoid conflict with MEI library
  int  ft_set_sample_rate (unsigned short sample_rate);
  int  set_filter_size (unsigned short filer_size);
  int  set_resolution (unsigned short new_resolution, unsigned short new_units);
  int  reset_card (void);
  int  enable_card_interrupts (void);
  int  disable_card_interrupts (void);
  int  read_ft_point (long data[]);
  int  read_peak_values (long mindata[], long maxdata[]);
  int  reset_peak_values (void);
  /*
      this functions is used to set a new calibration matrix in the EEPROM.
    */
  int  set_sensor_data (struct SENSOR_DATA new_sensor_data);
  /*
      the functions below are helper functions. they are to be used with or on results
      of one of the functions above
    */
  void  get_condition_flags (unsigned short *filter_size, unsigned short *is_bias_on,
      unsigned short *is_temp_compensation_on, unsigned short *is_calibration_on,
      unsigned short *is_peak_holding_on, unsigned short *is_monitoring_on,
      unsigned short *is_saturation_check_on);
} // extern "C"


#endif // CISST_HAS_ATIISAFT

#endif // _ddiATIISAFTLowLevelIO_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIISAFTLowLevelIO.h,v $
// Revision 1.5  2006/02/24 19:32:46  pkaz
// ddiATIISAFTLowLevelIO.h:  fixed to compile on Windows.
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/07/29 20:12:40  pkaz
// ddiATIISAFTLowLevelIO: Changed set_sample_rate to avoid name conflict with MEI library.
//
// Revision 1.2  2004/05/27 19:34:22  anton
// Added || defined DOXYGEN to all #ifdef CISST_HAS_XYZ so that doxygen can
// create the documentation.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.2  2004/04/08 06:04:25  kapoor
// Added conditional compilation
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
