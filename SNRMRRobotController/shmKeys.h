//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: shmKeys.h,v $
// $Revision: 1.4 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:15 $
//====================================================================


//====================================================================
// Description:
//    Definitions of common shared memory keys.
//====================================================================


#ifndef _INC_SHM_KEYS
#define _INC_SHM_KEYS

#define SHM_TARGET_LOG_POS      (key_t)30534
#define SHM_TARGET_PHYS_POS     (key_t)30535
#define SHM_CURRENT_LOG_POS     (key_t)30536
#define SHM_CURRENT_PHYS_POS    (key_t)30537

#define SHM_STATUS              (key_t)30435
#define SHM_COMMAND             (key_t)30431
#define SHM_LOG                 (key_t)30432
#define SHM_RAS_CURRENT         (key_t)30433
#define SHM_RAS_SETPOINT        (key_t)30434


#endif //_INC_SHM_KEYS
