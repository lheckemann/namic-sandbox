/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIISAFTLowLevelIO.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $
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


#include <cisstDeviceInterface/ddiATIISAFTLowLevelIO.h>
#if CISST_HAS_ATIISAFT

extern "C" {

  unsigned short __ATIISAFT_BaseAddress;

  unsigned short temp_buffer[500];       // temp command buffer

  struct OUTPUT_INFO __output_info;
  struct SENSOR_DATA __sensor_data;
  struct CARD_DATA __card_data;
  struct PEAK_DATA __peak_data;

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   * 
   *  function:  send_command()
   *  sends a command to the card
   * 
   *  Calling parameters:
   *  bptr = pointer to command structure
   *  numwords = length of command
   * 
   * ------------------------------------------------------------------
   */

  int send_command (unsigned short *bptr, int numwords) {
    unsigned short dword, command, retcode, loop;
    /*
     * write command block to DPRAM at offset 0x000
     */
    command = *bptr;
    outw (ATIFT_CMD_BUFFER_OFFSET, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG);  // set address register to command buffer block
    for(;numwords;numwords--) {  // write command data to dual port RAM
      dword = *bptr;
      outw (dword, __ATIISAFT_BaseAddress + ATIISAFT_DATA_REG); // remember index reg increments automagically
      bptr ++;
    }
    /*
     * read MailBox at DPRAM 0x3FE to clear host interrupt flag
     */
    outw (ATIFT_CMD_ISA_TO_PC_MAILBOX, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG); // set address register to isa to pc mailbox
    inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);  // reading from data reg reset the pc interrupt flag on DSP

    /*
     * this step is not there in the manual, but exists in sample provided
     * by ATI. recheck!
     */
    outw (ATIFT_CMD_ISA_TO_PC_MAILBOX, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG); // set address register to isa to pc mailbox
    outw (command, __ATIISAFT_BaseAddress + ATIISAFT_DATA_REG); // clears the acknowledgment flag

    /*
     * write command word to Mailbox at DPRAM 0x3FF to set DSP interrupt flag
     */
    outw (ATIFT_CMD_PC_TO_ISA_MAILBOX, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG); // set address register to pc to isa mailbox
    outw (command, __ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);

    /*
     * waits for command completion (interrupt flag) from DSP
     */
    retcode = TRUE;
    for ( loop = TRUE; loop; ) {
      dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_STATUS_REG);

      // wait for interrupt flag
      if ( (dword & ATIFT_ACK_BIT) == 0 ) {          // interrupt set
        outw (ATIFT_CMD_ISA_TO_PC_MAILBOX, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG); // set address register to isa to pc mailbox
        retcode = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG); // reading from data reg reset the pc interrupt flag on DSP

        // test mailbox for command acknowledgment
        if (retcode & 0x00ff) loop=FALSE;  // test for ack
      }
    }

    /*
     * this step is not there in the manual, but exists in sample provided
     * by ATI. recheck!
     */
    outw (ATIFT_CMD_BUFFER_OFFSET, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG);  // set address register to command buffer block
    outw (0x0, __ATIISAFT_BaseAddress + ATIISAFT_DATA_REG); // enables the output

    return (retcode);
  }

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *
   *  function:  receive_command()
   *  reads numwords words from ft card starting at fifo offset 0
   *
   *  Calling parameters:
   *  bptr = pointer to command structure
   *  numwords  = length of command
   * ------------------------------------------------------------------
   */

  void receive_command (unsigned short *bptr, int numword) {
    unsigned short  dword;

    outw (ATIFT_CMD_BUFFER_OFFSET, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG); // set address register to beginning of command data block
    for(;numword;numword--) {
      dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG); // data register increments automagically
      *bptr = dword;
      bptr ++;
    }
  }

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *
   *  function: get_output_info()
   *  Sends Output_Information Command to F/T card and fills 
   *  __output_info with data
   * ------------------------------------------------------------------
   */

  int get_output_info (void) {
    unsigned short  *buf_ptr;

    __output_info.cmd_rc = ATIFT_CMD_INFO;
    __output_info.mode   = ATIFT_CMD_INFO_READ_MODE;
    buf_ptr = &__output_info.cmd_rc;
    if ( !send_command (buf_ptr, 2) ) return (FALSE);
    receive_command (buf_ptr, sizeof (struct OUTPUT_INFO)/2);
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: get_sensor_info ()
   *  Sends Sensor_Data Command to F/T card and fills 
   *  __sensor_data with data
   * ------------------------------------------------------------------
   */

  int get_sensor_data (void) {
    unsigned short  *buf_ptr;

    __sensor_data.sd_cmd_rc = ATIFT_CMD_SENSOR_DATA;
    __sensor_data.sd_mode = ATIFT_CMD_SENSOR_DATA_READ_MODE;
    buf_ptr = &__sensor_data.sd_cmd_rc;
    if ( !send_command (buf_ptr, 2) ) return (FALSE);
    receive_command (buf_ptr, sizeof (struct SENSOR_DATA)/2);
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: set_sensor_info ()
   *  Sends Sensor_Data Command to F/T card and dumps 
   *  __sensor_data into EEPROM.
   *
   * *** USE with extra care ***
   * ------------------------------------------------------------------
   */

  int set_sensor_data (struct SENSOR_DATA new_sensor_data) {
    unsigned short  *buf_ptr;

    temp_buffer[0] = ATIFT_CMD_SENSOR_DATA;
    temp_buffer[1] = ATIFT_CMD_SENSOR_DATA_READ_MODE;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, sizeof (struct SENSOR_DATA)/2);
    //new_sensor_data.sd_cmd_rc = ATIFT_CMD_SENSOR_DATA;
    //new_sensor_data.sd_mode = ATIFT_CMD_SENSOR_DATA_WRITE_MODE;
    buf_ptr = &new_sensor_data.sd_cmd_rc;
    buf_ptr[0] = ATIFT_CMD_SENSOR_DATA;
    buf_ptr[1] = ATIFT_CMD_SENSOR_DATA_WRITE_MODE;
    /*
       int i, j, k;
       printk ("ATIFT: SENSOR> %x:%x %x%x  %x%x\n",
       buf_ptr[0], buf_ptr[1], buf_ptr[2], buf_ptr[3], buf_ptr[4], buf_ptr[5]);
       k = 0;
       for (i = 0; i < 6; i++) {
       printk ("ATIFT: set_sensor_data");
       for (j = 0; j < 8; j++) {
       printk ("%lx ",
       (long)(new_sensor_data.sd_matrix[k] + (new_sensor_data.sd_matrix[k+1] << 16)));
       k += 2;
       }
       printk ("\n");
       }
     */
    if ( !send_command (buf_ptr, sizeof (struct SENSOR_DATA)/2) ) return (FALSE);
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: get_card_data ()
   *  Sends Card_Data Command to F/T card and fills 
   *  __card_data with data
   * ------------------------------------------------------------------
   */

  int get_card_data (void) {
    unsigned short  *buf_ptr;

    __card_data.cd_cmd_rc = ATIFT_CMD_CARD_DATA;
    __card_data.cd_mode = ATIFT_CMD_CARD_DATA_READ_MODE;
    buf_ptr = &__card_data.cd_cmd_rc;
    if ( !send_command (buf_ptr, 2) ) return (FALSE);
    receive_command (buf_ptr, sizeof (struct CARD_DATA)/2);
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   * Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *
   * function: get_counts_info()
   *  gets 12-bit (nominal) counts per force and torque, and units
   *
   * Calling parameters:
   *  cnts_force  = the nominal (12-bit) counts per force (default units)
   *  cnts_torque = the nominal (12-bit) counts per torque (default units)
   *  units       = the default units for the calibration (1=lbf,lbf-in;
   *      2=N,N-mm; 3=N,N-m; 4=Kg,Kg-cm; 5=Klbf,Klbf-in)
   * ------------------------------------------------------------------
   */

  int get_counts_info (unsigned short *cnts_force, unsigned short *cnts_torque, unsigned short *units) {
    unsigned short *buf_ptr;

    temp_buffer[0] = ATIFT_CMD_SENSOR_DATA;
    temp_buffer[1] = ATIFT_CMD_SENSOR_DATA_READ_MODE;
    buf_ptr = &temp_buffer[0];
    if ( !send_command (buf_ptr, 2) ) return (FALSE);
    receive_command (buf_ptr, ATIFT_CMD_SENSOR_DATA_DATA_SIZE); // number of words to read = 147
    *cnts_force = temp_buffer[ATIFT_CMD_SENSOR_DATA_CNTS_PER_FORCE_OFFSET];
    *cnts_torque = temp_buffer[ATIFT_CMD_SENSOR_DATA_CNTS_PER_TORQUE_OFFSET];
    *units = temp_buffer[ATIFT_CMD_SENSOR_DATA_UNITS_OFFSET];
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: set_new_bias ()
   *  performs a sensor bias, removing any existing loads on the sensor
   * ------------------------------------------------------------------
   */

  int set_new_bias (void) {
    temp_buffer[0] = ATIFT_CMD_BIAS_ARRAY;
    temp_buffer[1] = ATIFT_CMD_BIAS_ARRAY_NEW;
    if( !send_command (temp_buffer,2) ) return(FALSE);
    receive_command (temp_buffer, 10);
    /*
       int i;
       printk ("ATIFT: Current Bias -> [");
       for (i = 0; i < 8; i++) 
         printk ("%d, ", temp_buffer[i + 2]);
       printk ("]\n");
     */
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: set_sample_rate ()
   *    sets the sample rate, max rate = 7800Hz
   *  Calling parameters:
   *  sample_rate = the intended sample rate. Note setting a sample rate
   *  does not gurantee that it would be used for sampling. It is a
   *  suggested rate. Recheck rate using get_sensor_data if this is 
   *  critical.
   * ------------------------------------------------------------------
   */

  // PKAZ:  rename following to avoid conflict with MEI library
  int ft_set_sample_rate (unsigned short  sample_rate) {
    if (sample_rate > 7800) return FALSE;

    // read card data info
    temp_buffer[0] = ATIFT_CMD_CARD_DATA;
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_READ_MODE;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, 6);
    // write new card data
    temp_buffer[0] = ATIFT_CMD_CARD_DATA;
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_WRITE_MODE;
    temp_buffer[5] = sample_rate;
    if ( !send_command (temp_buffer, 6) ) return FALSE;
    return (TRUE);
  }

  /*
   * ------------------------------------------------------------------
   *  function: set_filter_size ()
   *    sets the number of dual cascaded IIR filter stages
   *  Calling parameters:
   *    filter_size = 0 means no filtering. max size = 5
   *    a filter size of 5 takes 11.5ms to settle for a clock
   *    speed of 2KHz. It takes about 2.9ms for a clock speed
   *    of 7.8KHz. See manual for more details.
   * ------------------------------------------------------------------
   */

  int set_filter_size (unsigned short filter_size) {
    unsigned short orig_flags;
    if (filter_size > 5) return FALSE;

    // read card data info
    temp_buffer[0] = ATIFT_CMD_CARD_DATA;
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_READ_MODE;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, 6);
    orig_flags = temp_buffer[2] & ATIFT_CMD_CARD_DATA_FILTER_MASK_INV;
    // write new card data
    temp_buffer[0] = ATIFT_CMD_CARD_DATA;
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_WRITE_MODE;
    temp_buffer[2] = orig_flags | filter_size;
    if ( !send_command (temp_buffer, 6) ) return FALSE;
    return TRUE;
  }

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *
   *  function: reset_card ()
   *  issues the reset command to the card
   * ------------------------------------------------------------------
   */

  int reset_card (void) {
    int success;

    temp_buffer[0] = ATIFT_CMD_RESET;
    success = send_command (temp_buffer, 1);
    receive_command (temp_buffer, 4);  
    /*
       printk ("ATIFT: Self-test return code -> %d\n", temp_buffer[1]);
     */
    if ( success ) return TRUE;
    else return FALSE;
  }

  int enable_card_interrupts (void) {
    temp_buffer[0] = ATIFT_CMD_CARD_DATA; 
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_READ_MODE;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, 6);
    temp_buffer[0] = ATIFT_CMD_CARD_DATA; 
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_WRITE_MODE;
    temp_buffer[4] = ATIFT_CMD_CARD_DATA_ENABLE_INTERRUPTS; 
    if ( !send_command (temp_buffer, 6) ) return FALSE;
    outw (ATIFT_CLEAR_STATUS_REGISTER, __ATIISAFT_BaseAddress + ATIISAFT_STATUS_REG); //clear status register
    return TRUE;
  }

  int disable_card_interrupts (void) {
    temp_buffer[0] = ATIFT_CMD_CARD_DATA; 
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_READ_MODE;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, 6);
    temp_buffer[0] = ATIFT_CMD_CARD_DATA; 
    temp_buffer[1] = ATIFT_CMD_CARD_DATA_WRITE_MODE;
    temp_buffer[4] = ATIFT_CMD_CARD_DATA_DISABLE_INTERRUPTS; 
    if ( !send_command(temp_buffer, 6) ) return FALSE;
    return TRUE;
  }

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *  function: read_ft_point ()
   *  reads a single f/t point into argument data
   * 
   * Calling parameters:
   *   data - an array of 8 longs to hold f/t data
   *    data[0] = sequence
   *        [1] = monitor condition
   *        [2] = fx
   *        [3] = fy
   *        [4] = fz
   *        [5] = tx
   *        [6] = ty
   *        [7] = tz
   * ------------------------------------------------------------------
   */

  int read_ft_point(long data[]) {
    int    i;
    int    dword;
    long  lword, lwordh;

    dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_STATUS_REG);
    if ( (dword & 0x0010) == 0 ) {
      // new data available. first get f/t buffer address
      // clear status register
      outw (ATIFT_CLEAR_STATUS_REGISTER, __ATIISAFT_BaseAddress + ATIISAFT_STATUS_REG);
      // read current f/t buffer location from isa to pc mailbox 0x3fe
      outw (ATIFT_CMD_ISA_TO_PC_MAILBOX, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG);
      // reset pc interrupt flag on DSP
      dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);
      // address is of the format 0xffxx where xx is offset from 0x100
      if ( (dword & 0xFF00) == 0xFF00) dword = dword & 0x01FF;
      else dword = 0x100;
      // load starting address of f/t buffer in index register
      outw (dword, __ATIISAFT_BaseAddress + ATIISAFT_ADDR_REG);
      // remember index (a.k.a address register) increments automatically
      // read 1st word = Sequence Number
      dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);
      data[0] = (unsigned int)dword;
      // read 2nd word = Monitor Condition
      dword = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);
      data[1] = (unsigned int) dword;
      /*
       * get the new F/T data in counts
       * to get actual forces and torque, the application
       * should divide the counts by the
       * counts_per_force and counts_per_torque
       * conversion factors
       */
      for(i=0;i<6;i++) {
        lword = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);
        lwordh = inw (__ATIISAFT_BaseAddress + ATIISAFT_DATA_REG);
        // convert to 32-bit long
        lword = lword + (lwordh << 16);
        data[i + 2] = lword;
      }
    }
    return 0;
  }

  /*
   * ------------------------------------------------------------------
   *  function: read_peak_values ()
   *  reads a single f/t point into argument data
   * 
   * Calling parameters:
   *   mindata - an array of 6 longs to hold min f/t data
   *   maxdata - an array of 6 longs to hold max f/t data
   *     m??data[0] = (m??)fx
   *          [1] = (m??)fy
   *          [2] = (m??)fz
   *          [3] = (m??)tx
   *          [4] = (m??)ty
   *          [5] = (m??)tz
   * ------------------------------------------------------------------
   */
  int read_peak_values (long mindata[], long maxdata[]) {
    int    i;
    long  lword, lwordh;

    temp_buffer[0] = ATIFT_CMD_GET_PEAK_VALUES; 
    temp_buffer[1] = ATIFT_CMD_PEAK_DATA_DONT_RESET;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    receive_command (temp_buffer, 26);
    for (i = 0; i < 6; i++ ) {
      lword = temp_buffer[ATIFT_CMD_PEAK_DATA_MIN_VALUES_OFFSET + 2*i];
      lwordh = temp_buffer[ATIFT_CMD_PEAK_DATA_MIN_VALUES_OFFSET + 2*i + 1];
      lword = lword + (lwordh << 16);
      mindata[i] = lword;
      lword = temp_buffer[ATIFT_CMD_PEAK_DATA_MAX_VALUES_OFFSET + 2*i];
      lwordh = temp_buffer[ATIFT_CMD_PEAK_DATA_MAX_VALUES_OFFSET + 2*i + 1];
      lword = lword + (lwordh << 16);
      maxdata[i] = lword;
    }
    return TRUE;
  }

  int reset_peak_values (void) {
    temp_buffer[0] = ATIFT_CMD_GET_PEAK_VALUES;
    temp_buffer[1] = ATIFT_CMD_PEAK_DATA_RESET;
    if ( !send_command (temp_buffer, 2) ) return FALSE;
    return TRUE;
  }

  /*
   * ------------------------------------------------------------------
   *  function: get_condition_flags ()
   *    looks at data condition flags, and returns more usable format.
   *    assumes that __card_data is populated using get_card_data ().
   * ------------------------------------------------------------------
   */
  void get_condition_flags (unsigned short *filter_size, unsigned short *is_bias_on,
      unsigned short *is_temp_compensation_on, unsigned short *is_calibration_on,
      unsigned short *is_peak_holding_on, unsigned short *is_monitoring_on,
      unsigned short *is_saturation_check_on) {
    *filter_size = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_FILTER_MASK;
    *is_bias_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_BIAS_MASK;
    *is_temp_compensation_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_TEMP_MASK;
    *is_calibration_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_CALIBRATION_MASK;
    *is_peak_holding_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_PEAK_MASK;
    *is_monitoring_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_MONITORING_MASK;
    *is_saturation_check_on = __card_data.cd_flags & ATIFT_CMD_CARD_DATA_SAT_CHK_MASK;
  }

  /*
   * ------------------------------------------------------------------
   *  Copyright 1998, 1999 ATI Industrial Automation, All rights reserved.
   *
   *  function: set_resolution()
   *  changes the resolution and/or units on a 16-bit ISA system.
   *  using zero for either parameter retains the old value.
   *  this reset the card - any other changes made to card
   *  settings would be lost!
   *
   *  Calling Parameters:
   *  new_res = Resolution to set (12-16, 0=no change)
   *  new_units = Units to set (1=lbf,lbf-in; 2=N,N-mm; 3=N,N-m;
   *        4=Kg,Kg-cm; 5=Klbf,Klbf-in; 0=no change)
   * ------------------------------------------------------------------
   */
  int set_resolution (unsigned short new_res, unsigned short new_units) {
    unsigned short cnts_force, cnts_torque;               // these variables depend on resolution
    unsigned short cnts_force_default, cnts_torque_default, units;    // these are factory-set
    unsigned short sFDiv,sTDiv;
    double  dtemp, counts_per_force, counts_per_torque;
    long temp1,temp2;
    long pow2n;
    int success;

    // unit conversion factors
    double fconv[5][5] = {
      {1.000000, 4.448222, 4.44822,  0.453592, 0.001000},
      {0.224809, 1.000000,  1.00000,  0.101972, 0.000225},
      {0.224809, 1.000000,  1.00000,  0.101972, 0.000225},
      {2.204622, 9.806650, 9.80665,  1.000000, 0.002205},
      {1000.000, 4448.222, 4448.222, 453.59241, 1.000000}
    };

    double tconv[5][5] = {
      {1.000000, 112.9848, 0.112985, 1.152124, 0.001000},
      {0.008851, 1.000000,  0.00100,  0.010197, 0.000008851},
      {8.850748, 1000.000,  1.00000,  10.197162, 0.008851},
      {0.867962, 98.06650, 0.0980665, 1.000000, 0.000868},
      {1000.000, 112984.8, 112.9848, 1152.1243, 1.000000}
    };

    get_output_info ();
    get_counts_info (&cnts_force_default, &cnts_torque_default, &units);

    if ( new_units == 0 ) new_units = units;
    if ( new_res == 0 )   new_res = __output_info.resolution;

    if ( ( new_res > 16 ) || ( new_res < 12 ) || ( new_units > 5 ) ) return FALSE; 

    dtemp = fconv[units-1][new_units-1];
    counts_per_force  = (double) cnts_force_default / dtemp;
    if ( counts_per_force == 0 ) counts_per_force = 1;
    dtemp = tconv[units-1][new_units-1];
    counts_per_torque  = (double) cnts_torque_default / dtemp;
    if ( counts_per_torque == 0 ) counts_per_torque = 1;

    // find new counts values
    // cant compute pow just like that!
    //temp1 = pow (2, new_res-12) * counts_per_force;
    //temp2 = pow (2, new_res-12) * counts_per_torque;
    pow2n = 1L;
    pow2n = pow2n << ( new_res-12 );
    temp1 = (long)(pow2n * counts_per_force);
    temp2 = (long)(pow2n * counts_per_torque);

    // round to nearest 10
    if ( temp1 > 0xffff ) temp1 = 0xffff;
    if ( temp1 > 10 ) temp1 = (temp1 / 10) * 10;

    if ( temp2 > 0xffff) temp2 = 0xffff;
    if ( temp2 > 10) temp2 = (temp2 / 10) * 10;

    cnts_force = (unsigned short)temp1;
    cnts_torque = (unsigned short)temp2;

    // check for minimum values
    if (cnts_force < counts_per_force)
      cnts_force = (unsigned short) (counts_per_force + 0.5);
    if (cnts_torque < counts_per_torque)
      cnts_torque = (unsigned short) (counts_per_torque+ 0.5);

    /*
     * calc new divisors 
     * 0.32 fractional format = 1073.7418 = 2^30/1M
     * 1073741824/1M = 1073.7418
     */
    sFDiv = (unsigned short)(((268.43545 * cnts_force) / counts_per_force + 0.5)) ;
    sTDiv = (unsigned short)(((268.43545 * cnts_torque) / counts_per_torque + 0.5));

    /*
     * this is not documented! but exists in the sample code
     */
    temp_buffer[0] = ATIFT_CMD_INFO;
    temp_buffer[1] = ATIFT_CMD_INFO_WRITE_MODE;
    temp_buffer[2] = new_units;
    temp_buffer[3] = new_res;
    temp_buffer[4] = cnts_force;
    temp_buffer[5] = cnts_torque;
    temp_buffer[6] = sFDiv;
    temp_buffer[7] = sTDiv;
    success = send_command (temp_buffer, 8);
    reset_card ();

    if ( success ) return TRUE;
    else return FALSE;
  }
} //extern "C"

#endif // CISST_HAS_ATIISAFT


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIISAFTLowLevelIO.cpp,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/07/29 20:12:40  pkaz
// ddiATIISAFTLowLevelIO: Changed set_sample_rate to avoid name conflict with MEI library.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.2  2004/04/08 06:07:01  kapoor
// Added conditional compilation
//
// Revision 1.1  2004/03/22 00:30:53  kapoor
// Added new devices.
//
// Revision 1.1.1.1  2004/03/05 21:18:16  kapoor
// Reimporting into a cvs on stomach
//
// ****************************************************************************
