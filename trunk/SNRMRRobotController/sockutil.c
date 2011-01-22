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
 $RCSfile: sacsutil.c,v $
 $Revision: 1.4 $
 $Author: junichi $
 $Date: 2005/08/02 05:08:35 $
====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
/* #include <poll.h> */
#include <sys/poll.h>
#include <pthread.h>

#include "sockutil.h"

size_t readn(int fd, void *vptr, size_t n)
{
  size_t  nleft;
  size_t  nread;
  char   *ptr;
  
  ptr = (char*)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        nread = 0;
      } else {
        return ((size_t) -1);
      }
    } else if (nread == 0) {
      return (size_t) -1;            /* EOF */
    }
    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft);     /* return >= 0 */
}


size_t readnt(int fd, void *vptr, size_t n, int msec) /* with timeout */
{
  struct pollfd  pfd;
  size_t         nleft;
  size_t         nread;
  char           *ptr;
  int            r;
  
  pfd.fd     = fd;
  pfd.events = POLLIN;

  ptr = (char*)vptr;
  nleft = n;
  while(nleft > 0) {
    /* cerr << "polling ..."; */
    r = poll(&pfd, 1, msec);
    /* cerr << "done. " << r << endl;*/
    if (r > 0 && pfd.revents & POLLIN) {
      if ((nread = read(fd, ptr, nleft)) < 0) {
        if (errno == EINTR)
          nread = 0;
        else
          return ((size_t) -1);
      } else if (nread == 0) {/* disconnected */
        return ((size_t) -1);
        /*break;*/
      }
      nleft -= nread;
      ptr   += nread;
    } else if (r == 0) {    /* time out */
      break;
    } else if ( r < 0 ) {
      /* printf(stderr, "ERROR in polling.\n");*/
    }
  }
  return (n - nleft);     /* return >= 0 */
}

size_t readnct(int fd, void *vptr, size_t n, char term, int msec)
{
  struct pollfd  pfd;
  size_t         nleft;
  size_t         nread;
  char           *ptr;
  char           *ptr2;
  int            r;
  int            ts;
  char           *pt;
  
  pfd.fd     = fd;
  pfd.events = POLLIN;

  ptr = (char*)vptr;
  nleft = n;
  while(nleft > 0) {
    /* cerr << "polling ..."; */
    r = poll(&pfd, 1, msec);
    /* cerr << "done. " << r << endl;*/
    if (r > 0 && pfd.revents & POLLIN) {
      if ((nread = read(fd, ptr, nleft)) < 0) {
        if (errno == EINTR)
          nread = 0;
        else
          return ((size_t) -1);
      } else if (nread == 0) {/* disconnected */
        return ((size_t) -1);
      }
      ptr2  = ptr;
      ptr   += nread;
      /* find a byte that equals to 'term' */
      while (ptr2 < ptr) {
        if (*ptr2 == term) {
          *ptr2 = '\0';
          return (n-nleft);
        }
        ptr2 ++;
        nleft --;
      }
    } else if (r == 0) {    /* time out */
      *ptr = '\0';
      break;
    } else if ( r < 0 ) {
      /*printf(stderr, "ERROR in polling.\n");*/
      return -1;
    }
  }
  return (n - nleft);     /* return >= 0 */
}


size_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  size_t nwritten;
  const char *ptr;
  
  ptr = (char*)vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;   /* and call write() again */
      else
        return ((size_t)-1);    /* error */
    }
    
    nleft -= nwritten;
    ptr += nwritten;
  }
  return (n);
}


Sbfd* sb_new(int fd)
{
  Sbfd* p;

  p = (Sbfd*) malloc(sizeof(Sbfd));
  p->fd = fd;
  p->begin = p->buffer;
  p->end   = p->buffer;

  return p;
}


void sb_del(Sbfd* sbfd)
{
  free(sbfd);
}


int sb_readn(Sbfd* sbfd, void *vptr, size_t n)
{
  int     fd;
  size_t  nleft;
  size_t  nread;
  char    *ptr;

  fd = sbfd->fd;
  /* if data remains in buffer */
  nread = (long int)sbfd->end - (long int)sbfd->begin;
  if (nread == 0) {
    ptr = (char*)vptr;
    nleft = n;
  } else if (nread > n) {
    memcpy(vptr, sbfd->begin, n);
    sbfd->begin += n;
    return n;
  } else {
    ptr = (char*)vptr;
    memcpy(ptr, sbfd->begin, nread);
    ptr += nread;
    nleft = n - nread;
    sbfd->begin = sbfd->buffer;
    sbfd->end   = sbfd->buffer;
  }

  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        nread = 0;
      } else {
        return ((size_t) -1);
      }
    } else if (nread == 0) {
      return (size_t) -1;            /* EOF */
    }
    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft);     /* return >= 0 */
}


int sb_readnt(Sbfd* sbfd, void *vptr, size_t n, int msec)
{
  int            fd;
  struct pollfd  pfd;
  size_t         nleft;
  size_t         nread;
  char           *ptr;
  int            r;

  fd         = sbfd->fd;
  pfd.fd     = fd;
  pfd.events = POLLIN;

  /* if data remains in buffer */
  nread = (long int)sbfd->end - (long int)sbfd->begin;
  if (nread == 0) {
    ptr = (char*)vptr;
    nleft = n;
  } else if (nread > n) {
    memcpy(vptr, sbfd->begin, n);
    sbfd->begin += n;
    return n;
  } else {
    ptr = (char*)vptr;
    memcpy(ptr, sbfd->begin, nread);
    ptr += nread;
    nleft = n - nread;
    sbfd->begin = sbfd->buffer;
    sbfd->end   = sbfd->buffer;
  }

  while(nleft > 0) {
    /* cerr << "polling ..."; */
    r = poll(&pfd, 1, msec);
    /* cerr << "done. " << r << endl;*/
    if (r > 0 && pfd.revents & POLLIN) {
      if ((nread = read(fd, ptr, nleft)) < 0) {
        if (errno == EINTR)
          nread = 0;
        else
          return ((size_t) -1);
      } else if (nread == 0) {/* disconnected */
        return ((size_t) -1);
        /*break;*/
      }
      nleft -= nread;
      ptr   += nread;
    } else if (r == 0) {    /* time out */
      break;
    } else if ( r < 0 ) {
      /* printf(stderr, "ERROR in polling.\n");*/
    }
  }
  return (n - nleft);     /* return >= 0 */
}


int sb_readnct(Sbfd* sbfd, void *vptr, size_t n, char term, int msec)
{
  int            fd;
  struct pollfd  pfd;
  size_t         nleft;
  size_t         nread;
  char           *ptr;
  char           *ptr2;
  int            r;
  int            ts;
  char           *pt;
  size_t         maxread;

  fd         = sbfd->fd;
  pfd.fd     = fd;
  pfd.events = POLLIN;

  /* if data remains in buffer */
  nread = (long int)sbfd->end - (long int)sbfd->begin;
  if (nread == 0) {
    ptr = (char*)vptr;
    nleft = n;
  } else {
    nread = 0;
    ptr   = (char*)vptr;
    ptr2  = sbfd->begin;
    while (ptr2 < sbfd->end && nread < n) {
      *ptr = *ptr2;
      if (*ptr2 == term) {
        *ptr = '\0';
        sbfd->begin = ptr2 + 1;
        return nread;
      }
      ptr2 ++;
      ptr ++;
      nread ++;
    }
    if (n == nread) {
      return nread;
    } else { /*ptr2 == sbfd->end*/
      sbfd->begin = sbfd->buffer;
      sbfd->end   = sbfd->buffer;
    }
    nleft = n - nread;
  }

  while(nleft > 0) {
    /* cerr << "polling ..."; */
    r = poll(&pfd, 1, msec);
    /* cerr << "done. " << r << endl;*/
    if (r > 0 && pfd.revents & POLLIN) {
      maxread = (nleft>SB_BUFFER_SIZE)?SB_BUFFER_SIZE:nleft;
      if ((nread = read(fd, ptr, maxread)) < 0) {
        if (errno == EINTR)
          nread = 0;
        else
          return -1;
      } else if (nread == 0) {/* disconnected */
        return -1;
      }
      ptr2  = ptr;
      ptr   += nread;
      /* find a byte that equals to 'term' */
      while (ptr2 < ptr) {
        if (*ptr2 == term) {
          *ptr2 = '\0';
          ptr2 ++;
          if (ptr2 < ptr) { /* copy bytes after 'term' to buffer */
            nread = (long int)ptr-(long int)ptr2;
            memcpy(sbfd->begin, ptr2, nread);
            sbfd->end += nread;
          }
          return (n-nleft);
        }
        ptr2 ++;
        nleft --;
      }
    } else if (r == 0) {    /* time out */
      *ptr = '\0';
      break;
    } else if ( r < 0 ) {
      /*printf(stderr, "ERROR in polling.\n");*/
      return -1;
    }
  }
  return (n - nleft);     /* return >= 0 */
}



void ahtonl(uint32_t* dest, uint32_t* src, int n)
{
  uint32_t* end;

  end = dest;
  end += n;

  while (dest < end) {
    *dest = htonl(*src);
    dest ++;
    src ++;
  }
}


void antohl(uint32_t* dest, uint32_t* src, int n)
{
  uint32_t* end;

  end = dest;
  end += n;

  while (dest < end) {
    *dest = ntohl(*src);
    dest ++;
    src ++;
  }
}


void ahtons(uint16_t* dest, uint16_t* src, int n)
{
  uint16_t* end;

  end = dest;
  end += n;

  while (dest < end) {
    *dest = htons(*src);
    dest ++;
    src ++;
  }
}

void antohs(uint16_t* dest, uint16_t* src, int n)
{
  uint16_t* end;

  end = dest;
  end += n;

  while (dest < end) {
    *dest = ntohs(*src);
    dest ++;
    src ++;
  }
}



