//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrLocatorClient.cpp,v $
// $Revision: 1.6 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:47 $
//====================================================================


//====================================================================
// Description: 
//    Message interface to communicate with MRTS.
//====================================================================

#include <iostream>
#include <strings.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
//#include <poll.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <wchar.h>

//#include <sys/ioctl.h>
//#include <sys/errno.h>

#include <sys/timeb.h>

#include "MrsvrLocatorClient.h"

//const char* MrsvrLocatorClient::statusStr[] = {
//  "Disconnected",
//  "Connected"
//};

using namespace std;


//----------------------------------------------------------------------
// Constructor & destructor
//----------------------------------------------------------------------

MrsvrLocatorClient::MrsvrLocatorClient() : MrsvrThread()
{
  serverFd = -1;
  eventOnSignalReceive = 0;
  setTriggerMode(MrsvrThread::COUNT);
  //enableTrigger();
  //  triggerMode   = LOCAL;
  loopInterval = DEFAULT_LSERVER_INTERVAL;
  locStatus = MrsvrLocatorClient::LOCATOR_SENSOR_BLOCKED;

//  isRun           = 0;
//  mtc             = new MRTrackingClient();
//  mrtsHostname[0] = '\0';
//  mrtsPort        = DEFAULT_MRTS_PORT;
//  status          = MRTS_DISCONNECTED;
//  interval        = DEFAULT_MRTS_INTERVAL;
//
//  setPoint        = new MrsvrRASWriter(SHM_RAS_SETPOINT);
}


MrsvrLocatorClient::MrsvrLocatorClient(const char* hostname, int port)
  : MrsvrThread()
{
  serverFd = -1;
  locStatus = MrsvrLocatorClient::LOCATOR_SENSOR_BLOCKED;

  setServer(hostname, port);
  //setTriggerMode(Thread::COUNT);

}

MrsvrLocatorClient::~MrsvrLocatorClient()
{
//  if (mtc) 
//    delete mtc;
}


//----------------------------------------------------------------------
// Private functions
//----------------------------------------------------------------------
int MrsvrLocatorClient::getPos(int timeout)
{
  uint32_t  tmp;
  uint32_t* pos;
  size_t    s;

#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
  cerr << "MrsvrLocatorClient::getSignal()" << endl;
#endif

  pos = (uint32_t*) latestPos;

  for (int i = 0; i < 9; i ++) {
    s = readn(&tmp, sizeof(uint32_t), timeout);
    if ((int)s < 0) {
      return -2; /* disconnected */
    }
    if (s != sizeof(uint32_t)) {
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
      cerr << "MrsvrLocatorClient::getPos(): can't get data." << endl;
#endif
      return 0;
    }
    pos[i] = (int)ntohl(tmp);
  }

  return 1;
}


size_t MrsvrLocatorClient::readn(void *vptr, size_t n)
{
  size_t  nleft;
  size_t  nread;
  char    *ptr;
  
  ptr = (char*)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(serverFd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        return ((size_t) -1);
    } else if (nread == 0) {
      return (size_t) -1;            /* EOF */
    }
    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft);     /* return >= 0 */
}


size_t MrsvrLocatorClient::readn(void *vptr, size_t n, int msec)
  //
  // Read n bytes data from the server.
  // If the specified time is over, automatically quit the function.
  // msec < 0 wait data forever.
  //
{
  struct pollfd  pfd;
  size_t         nleft;
  size_t         nread;
  char           *ptr;
  int            r;
  
  pfd.fd     = serverFd;
  pfd.events = POLLIN;

  ptr = (char*)vptr;
  nleft = n;
  while(nleft > 0 && serverFd > 0) {
    //cerr << "polling ...";
    r = poll(&pfd, 1, msec);
    //cerr << "done. " << r << endl;
    if (r > 0 && pfd.revents & POLLIN) {
      if ((nread = read(serverFd, ptr, nleft)) < 0) {
        if (errno == EINTR)
          nread = 0;
        else
          return ((size_t) -1);
      } else if (nread == 0) {/* disconnected */
        return ((size_t) -1);
      }
      nleft -= nread;
      ptr   += nread;
    } else if (r == 0) {    // time out
      break;
    } else if ( r < 0 ) {
      cerr << "ERROR in polling." << endl;
    }
  }
  return (n - nleft);     /* return >= 0 */
}


size_t MrsvrLocatorClient::writen(const void *vptr, size_t n)
{
  size_t     nleft;
  size_t     nwritten;
  const char *ptr;
  
  ptr = (char*)vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(serverFd, ptr, nleft)) <= 0) {
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


int MrsvrLocatorClient::sendMsg(LIPMSG msg)
{
  size_t  s;
  LIPMSG b;

  b = htons(msg);
  s = writen(&b, sizeof(LIPMSG));
  if (s != sizeof(LIPMSG)) {
    return 0;
  } else {
    return 1;
  }
}


int MrsvrLocatorClient::getMsg()
{
  size_t s;
  LIPMSG b;
  
  s = readn(&b, sizeof(LIPMSG));
  if ((int)s < 0) {
    return -2; /* disconnected */
  }
  if (s != sizeof(LIPMSG)) {
    return -1;  //timeout
  }
  return (int)ntohs(b);
}


int MrsvrLocatorClient::getMsg(int timeout)
{
  size_t s;
  LIPMSG b;

  s = readn(&b, sizeof(LIPMSG), timeout);
  //cerr << "getMsg(): size = " << s << endl;
  if ((int)s < 0) {
    return -2; /* disconnected */
  }
  if (s != sizeof(LIPMSG)) {
    return -1;  //timeout or error
  }
  return (int)ntohs(b);
}


//----------------------------------------------------------------------
// Public functions
//----------------------------------------------------------------------

void MrsvrLocatorClient::setServer(const char* hostname, int port)
{
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
  cerr << "MrsvrLocatorClient::setServer(): " << hostname << endl;
#endif

  // get IP address by hostname
  serverHostent = gethostbyname(hostname);
  serverPort    = port;
  if(serverHostent->h_addrtype != AF_INET) {
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
    cerr << "    Not an IP host." << endl;
#endif
    return;
  }
  serverIpAddr = (struct in_addr *)serverHostent->h_addr_list[0];
}


void MrsvrLocatorClient::setServer(const wchar_t* hostname, int port)
{
  char cname[128];
  const wchar_t*  pchar;
  
  pchar = hostname;
  wcsrtombs(cname, &pchar, 128, NULL);
  setServer(cname, port);
  //mrtsapp->sacsClient.run();
}


int MrsvrLocatorClient::connectToServer()
{
  int msg;

  // create socket
  serverFd = socket(AF_INET, SOCK_STREAM, 0);
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = serverIpAddr->s_addr;
  clientAddr.sin_port = htons(serverPort);
  clientLen = sizeof(clientAddr);
  
  if(connect(serverFd, (struct sockaddr *)&clientAddr, clientLen) == -1) {
    cerr << "MrsvrLocatorClient::connect(): Failed connecting to the server." << endl;
    serverFd = -1;
    return MrsvrLocatorClient::FAILURE;
  }
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
  cerr << "MrsvrLocatorClient::connect(): Waiting server response ..." << endl;
#endif

  // receive server response
  msg = getMsg(5000);
  if (msg != SERVER_HELO || serverFd < 0) {
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
    cerr << "Server Error" << endl;
#endif
    closeConnection();
    return -1;
  }
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
  cerr << "MrsvrLocatorClient::connect(): received HELO message." << endl;
#endif
  
  // check if log directory exists
  struct stat sb;
  stat("loc_data", &sb);
  if (errno == ENOENT) { /* the directory doesn't exist */
    mkdir("loc_data", 0755);
  }

  static time_t ct = 0;
  struct tm ctm;

  time(&ct);
  localtime_r(&ct, &ctm);
  char filename[256];
  sprintf(filename, "loc_data/%04d-%02d-%02d_%02d-%02d-%02d.txt",
          ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday,
          ctm.tm_hour, ctm.tm_min, ctm.tm_sec);

  locfp = fopen(filename, "a");

  enableTrigger();

  return MrsvrLocatorClient::SUCCESS;
}


int MrsvrLocatorClient::closeConnection()
{
  close(serverFd);
  serverFd = -1;
  disableTrigger();
  pthread_mutex_destroy(&mtxLatestPos);
}

int MrsvrLocatorClient::disconnect()
{
  stop();
  closeConnection();
 
  return MrsvrLocatorClient::SUCCESS;
}


void MrsvrLocatorClient::process()
{
  int msg;

#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
  cout << "MrsvrLocatorClient::process(): starting Locator Server client thread..." << endl;
#endif
  pthread_mutex_init(&mtxLatestPos, NULL);

  while (1) {
    lseek(serverFd, 0, SEEK_END);
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
    cerr << "MrsvrLocatorClient::process(): waiting trigger .....  ";
#endif

    /*** Trigger mode: LOCAL ***/
    //waitForTrigger();
    usleep(loopInterval);
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
    cerr << "MrsvrLocatorClient::process(): Trigger pulled." << endl;
#endif
    int r = sendMsg(GET_POS);
    if (r < 0) {
      closeConnection();
      threadExit();
    }
    
    msg = getMsg(5000);
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
    cerr << "MrsvrLocatorClient::process(): getMsg(): msg is " << msg << endl;
#endif

    if (msg == SEND_POS) {
      // Start critical section for latestPos[]
      pthread_mutex_lock(&mtxLatestPos);
      getPos();
      locStatus = MrsvrLocatorClient::LOCATOR_SENSOR_OK;
      // End of critical section for latestPos[]
      pthread_mutex_unlock(&mtxLatestPos);
    } else if (msg == SERVER_BLOCK) {
      // Start critical section for latestPos[]
      pthread_mutex_lock(&mtxLatestPos);
      getPos();
      locStatus = MrsvrLocatorClient::LOCATOR_SENSOR_BLOCKED;
      // End of critical section for latestPos[]
      pthread_mutex_unlock(&mtxLatestPos);
    } else if (msg < -1 ){ // disconnected
      closeConnection();
      threadExit();
    } else {
#ifdef _DEBUG_MRSVR_LOCATOR_CLIENT
      cerr << "MrsvrLocatorClient::process(): illegal message from Locator Server  " << endl;
#endif
    }
  }
}

int MrsvrLocatorClient::getLatestPos(float pos[9])
// substitute locator position and orientatin and
// return locator status
// pos[9] = { px, py, pz, nx, ny, nz, tx, ty, tz };
{
  int r;
  static int c = 0;
  pthread_mutex_lock(&mtxLatestPos);
  for (int i = 0;  i < 9; i ++)
    pos[i] = latestPos[i];
  r = locStatus;
  pthread_mutex_unlock(&mtxLatestPos);

  if (locfp != NULL) {
    // get time
    struct timeb tmb;
    struct tm ctm;

    ftime(&tmb);
    localtime_r(&tmb.time, &ctm);
    fprintf(locfp, "%d, %d, %d, ", tmb.time, tmb.millitm, r);
    fprintf(locfp, "%f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            pos[0], pos[1], pos[2],
            pos[3], pos[4], pos[5],
            pos[6], pos[7], pos[8]);
    
    c = (c + 1) % 100;
    if (c == 0) {
      fflush(locfp);
    }
  }
  
  return r;
}



