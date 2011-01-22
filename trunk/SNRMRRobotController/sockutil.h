/*====================================================================

 Socket Utilities

 Copyright (c) ATRE Lab. The Univ. of Tokyo, 2004.
 Department of Mechano-Informatics,
 Graduate School of Information Science and Technology,
 The University of Tokyo.

======================================================================
 Author      : Junichi Tokuda (junichi@atre.t.u-tokyo.ac.jp)
 Description : Read signal data from BAM and send to MRTS.
======================================================================
 $RCSfile: sacsutil.h,v $
 $Revision: 1.2 $
 $Author: junichi $
 $Date: 2005/06/30 08:41:00 $
====================================================================*/


#ifndef _INC_SOCKUTIL_H
#define _INC_SOCKUTIL_H

#include "btypes.h"

size_t readn(int fd, void *vptr, size_t n);
size_t readnt(int fd, void *vptr, size_t n, int msec);
size_t readnct(int fd, void *vptr, size_t n, char term, int msec);
size_t writen(int fd, const void *vptr, size_t n);


/* file I/O with simple buffer routines (sb)*/

#define SB_BUFFER_SIZE  256

typedef struct {
  int   fd;
  char  buffer[SB_BUFFER_SIZE];
  char* begin;
  char* end;
} Sbfd;


Sbfd*  sb_new(int fd);
void   sb_del(Sbfd* sbfd);
int    sb_readn(Sbfd* sbfd, void *vptr, size_t n);
int    sb_readnt(Sbfd* sbfd, void *vptr, size_t n, int ms);
int    sb_readnct(Sbfd* sbfd, void *vptr, size_t n, char term, int msec);


/* Following functions allow to specify a same array for dest and src. */
void ahtonl(uint32_t* dest, uint32_t* src, int n);
void antohl(uint32_t* dest, uint32_t* src, int n);
void ahtons(uint16_t* dest, uint16_t* src, int n);
void antohs(uint16_t* dest, uint16_t* src, int n);

/*
int sendMsg(int fd, SACSMSG msg);
int getMsg(int fd);

int sendFloat(int fd, float data);
int sendUInt32(int fd, uint32_t data);
int sendInt32(int fd, int data);
*/

#endif
