//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMessageServer.cpp,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description: 
//    Message interface for MRI guided robot control system.
//====================================================================

#include "MrsvrMessageServer.h"

const char* MrsvrMessageServer::svrStatusStr[] = {
  "Sleeping",
  "Waiting",
  "Connected",
};

#define MSG_SVR_NUM_MODE  8

const char* MrsvrMessageServer::robotModeStr[] = {
  "STOP",
  "M.CLB",
  "A.CLB",
  "MOVETO",
  "PAUSE",
  "MANUAL",
  "RCM",
  "RESET",
};

MrsvrMessageServer::MrsvrMessageServer(int port) : MrsvrThread()
{
  this->port = port;
  init();
  pthread_mutex_init(&mtxCommand, NULL);
}


MrsvrMessageServer::~MrsvrMessageServer()
{
  
}

void MrsvrMessageServer::init()
{
  serverSockFD = -1;
  masterSockFD = -1;

  msgMode    = new MrsvMsgMode;
  msgCmd     = new MrsvMsgCommand;
  sndMsgPos  = new MrsvMsgPosition;
  rcvMsgPos  = new MrsvMsgPosition;

  currentPos  = new MrsvrRASWriter(SHM_RAS_CURRENT);
  setPoint    = new MrsvrRASWriter(SHM_RAS_SETPOINT);
  robotStatus = new MrsvrStatusReader(SHM_STATUS);

  permissions[MSG_CHANGE_MODE]      = PERMIT_MASTER;
  permissions[MSG_SEND_COMMAND]     = PERMIT_MASTER;
  permissions[MSG_SEND_POSITION]    = PERMIT_MASTER;
  permissions[MSG_SEND_SYSINFO]     = PERMIT_MASTER;
  permissions[MSG_REQUEST_POSITION] = PERMIT_OTHERS|PERMIT_MASTER;

  remoteOS[0] = '\0';
  remoteSoftware[0] = '\0';

  masterBufferedFD = NULL;
  fSetTarget = false;
  nextRobotMode    = -1;
}


void MrsvrMessageServer::process()
{
  int newfd;
  fd_set readfds, testfds;
  struct timeval timeout;

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(port);
  serverLen = sizeof(serverAddr);

  // Create socket for server
  if ((serverSockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
#ifdef DEBUG    
    perror("MrsvrMessageServer::process():ERROR: socket()");
#endif // DEBUG
    return;
  }
  
  const int one = 1;
  setsockopt(serverSockFD, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
  if ((bind(serverSockFD, (struct sockaddr *)&serverAddr, serverLen)) < 0) {
#ifdef DEBUG
    perror("MrsvrMessageServer::process():ERROR: bind()");
#endif // DEBUG
    return;
  }

  listen(serverSockFD, NUM_CLIENT_WAIT);
  // Wait for service request and start service on request.
  FD_ZERO(&readfds);
  FD_SET(serverSockFD, &readfds);
  while(1) {
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    testfds = readfds;
    if(select(FD_SETSIZE, &testfds, NULL, NULL, &timeout) < 0) {
      perror("ERROR: select(). \n");
      close(serverSockFD);
      return;
    } else if (FD_ISSET(serverSockFD, &testfds)) {
      FD_CLR(serverSockFD, &testfds);
      if ((newfd = accept(serverSockFD, 0, 0)) < 0) {
        perror("accept");
        close(serverSockFD);
        return;
      }
      fprintf(stderr, "accepted.\n");

      FD_SET(newfd, &readfds);
      if (masterSockFD < 0) {
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
        printf("Master process connected to the robot module.\n");
        fflush(stdout);
#endif
        fprintf(stderr, "masterSockFD = %d.\n", newfd);
        //----- get client infomation -----
        unsigned int len = sizeof(peer_sin);
        if (getpeername(newfd, (struct sockaddr*)&peer_sin, &len) != 0) {
          fprintf(stderr, "getpeername() failed.\n");
          close(newfd);
          FD_CLR(newfd, &readfds);
        } else {
          fprintf(stderr, "getpeername() is done.\n");
          peer_host = gethostbyaddr((char*)&peer_sin.sin_addr.s_addr,
                                    sizeof(peer_sin.sin_addr), AF_INET);
          /*
          fprintf(stderr, "peer = %s.\n", peer_host->h_name);
          */
          time(&startTime);
          masterSockFD = newfd;
          masterBufferedFD = sb_new(masterSockFD);
        }
      }
    }
    /*
     *  MEMO -- by J.Tokuda on Sep 20, 2007
     *  The module has been designed to manage multiple clients,
     *  but this seems unnecessary.
     *  The new communication protocol specialized for one-client system
     *  is now implemented in onRcvMsgMaster() and onRcvMsg() will be 
     *  discarded in future.
     */
    for (int i = 0; i < FD_SETSIZE; i ++) {
      // process
      if (FD_ISSET(i, &testfds)) {
        int r;
        if (i == masterSockFD) {
          r = onRcvMsgMaster(masterBufferedFD);
        } else {
          r = onRcvMsg(i);
        }
        if (r < 0) {
          fprintf(stderr, "NO MESSAGE.\n");
          close(i);
          FD_CLR(i, &readfds);
          if (i == masterSockFD) {
            masterSockFD = -1;
            sb_del(masterBufferedFD);
            masterBufferedFD = NULL;
          }
        }
      }
    }
  }
}


void MrsvrMessageServer::stop()
{
  MrsvrThread::stop();
  /*
  fd_set readfds, testfds;
  for (int i = 0; i < FD_SETSIZE; i ++) {
    if (FD_ISSET(i, &testfds)) {
      close(i);
      FD_CLR(i, &readfds);
      if (i == masterSockFD) {
        masterSockFD = -1;
      }
    }
  }
  */
  masterSockFD = -1;
  if (serverSockFD > 0) {
    close(serverSockFD);
    serverSockFD = -1;
  }
  init();
}


msgType MrsvrMessageServer::sendMsgType(int sockfd, msgType mt)
{
  if (write(sockfd, &mt, sizeof(msgType)) == sizeof(msgType)) {
    return mt;
  } else {
    return -1;
  }
}

msgType MrsvrMessageServer::sendMsgPosition(int sockfd, MrsvMsgPosition* mp)
{
  if (write(sockfd, mp, sizeof(MrsvMsgPosition)) ==
      sizeof(MrsvMsgPosition)) {
    return MSG_SEND_POSITION;
  } else {
    return -1;
  }
}


int MrsvrMessageServer::onRcvMsgMaster(Sbfd* bfd)
{
  int s;
  char buf[256];

  s = sb_readnct(bfd, buf, 127, '\n', 1000);
  fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): Message: %s  Size: %d\n\n", buf, s);
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
  fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): Message: %s\n", buf);
#endif
  if (s < 0) {
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
    fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): disconneced\n");
#endif 
    return -1;
  }
  char cmd[256];
  if (strncmp("SET_TARGET", buf, 10) == 0) {
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
    fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): SET_TARGET command recieved\n");
#endif 
    float x, y, z;
    double target[3];
    sscanf(buf, "%s %f %f %f", cmd, &x, &y, &z);
    target[0] = x; target[1] = y; target[2] = z;
    //fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): target = (%f, %f, %f)\n", target[0], target[1], target[2]);
    int result = setTarget(target);
    char* ret;
    if (result == TARGET_ACCEPTED) {
      /*buf = "TARGET ACCEPTED";*/
      strcpy(buf, "TARGET ACCEPTED");
    } else if (result == TARGET_OUT_OF_RANGE) {
      /*buf = "TARGET OUT_OF_RANGE";*/
      strcpy(buf, "TARGET OUT_OF_RANGE");
    }
  } else if (strncmp("SET_MODE", buf, 8) == 0) {
    char param[128];
    sscanf(buf, "%s %s", cmd, param);
    if (setMode(param) >= 0) {
      sprintf(buf, "MODE OK\n");
    } else {
      sprintf(buf, "MODE ERROR\n");
    }
  } else if (strncmp("GET_STATUS", buf, 10) == 0) {
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
    fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster(): GET_STATUS command recieved\n");
#endif 
    int mode;
    int outrange;
    int lock[NUM_ACTUATORS];
    char *str_mode = "";
    getRobotStatus(&mode, &outrange, lock);
    if (mode >= 0 && mode < MSG_SVR_NUM_MODE) {
      sprintf(buf, "STATUS %s %d %d %d %d\n",
              MrsvrMessageServer::robotModeStr[mode],
              outrange, lock[0], lock[1], lock[2]);
    } else {
      sprintf(buf, "STATUS UNDEFINED %d %d %d %d\n",
              outrange, lock[0], lock[1], lock[2]);
    }
  } else {
    sprintf(buf, "UNDEFINED\n");
  }
  writen(bfd->fd, buf, strlen(buf));

  return 1;
}


int MrsvrMessageServer::onRcvMsg(int sockfd)
  // This function will be discarded in future.
  // This function returns the same value as for client,
  // or returns -1, if the connection is already expired.
{
  
  msgType mt;
  unsigned int readSize;

  printf("MrsvrMessageServer::onRcvMsg()\n");
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
  printf("MrsvrMessageServer::onRcvMsg()\n");
#endif 
  if (read(sockfd, &mt, sizeof(msgType)) < (int)sizeof(msgType)) {
    // connection closed or error
    return -1;
  }
    
  printf("  MssageType = %d\n", mt);
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
  printf("  MssageType = %d\n", mt);
#endif
  if (((sockfd==masterSockFD) && 
       (permissions[mt]&PERMIT_MASTER == 0)) ||
      ((sockfd!=masterSockFD) &&
       (permissions[mt]&PERMIT_OTHERS == 0))) { // if permission denied
    char buf[256];
    int n_size=256;
    while(n_size>=256) {                    // clear stream
      n_size = read(sockfd, buf, 256);  
    }
    return sendMsgType(sockfd, MSG_RETURN_DENY);
  }

  if ((readSize = read(sockfd, rcvBuf, MESSAGE_BUFFER_SIZE)) <= 0) { 
    // if disconnected
    return -1;
  }

  switch(mt) {
  case MSG_CHANGE_MODE:
    if (readSize >= sizeof(MrsvMsgMode)) {
      memcpy(msgMode, rcvBuf, sizeof(MrsvMsgMode));
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      printf("msgMode->modeNumber = %d\n", msgMode->modeNumber);
#endif
      return sendMsgType(sockfd, MSG_RETURN_SUCCESS);
    }
    break;

  case MSG_SEND_COMMAND:
    if (readSize >= sizeof(MrsvMsgCommand)) {
      memcpy(msgCmd, rcvBuf, sizeof(MrsvMsgCommand));
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      printf("msgCmd->target  = %d\n", msgCmd->target);
      printf("msgCmd->command = %d\n", msgCmd->command);
#endif
      return sendMsgType(sockfd, MSG_RETURN_SUCCESS);
    }
    break;

  case MSG_SEND_POSITION:
    if (readSize >= sizeof(MrsvMsgPosition)) {
      //memcpy(rcvMsgPos, rcvBuf, sizeof(MsgPosition));
      MrsvMsgPosition* p = (MrsvMsgPosition*)rcvBuf;
      setPoint->setNR(p->nx);
      setPoint->setNA(p->ny);
      setPoint->setNS(p->nz);
      setPoint->setTR(p->tx);
      setPoint->setTA(p->ty);
      setPoint->setTS(p->tz);
      setPoint->setPR(p->px);
      setPoint->setPA(p->py);
      setPoint->setPS(p->pz);
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      DUMP_MSG_POSITION(p);
#endif

      return sendMsgType(sockfd, MSG_RETURN_SUCCESS);
    }
    break;

  case MSG_SEND_SYSINFO:
    if (readSize >= sizeof(MrsvMsgSysInfo)) {
      MrsvMsgSysInfo* p = (MrsvMsgSysInfo*)rcvBuf;
      strncpy(remoteOS, p->os, MESSAGE_MAX_INFO_STR-1);
      remoteOS[MESSAGE_MAX_INFO_STR-1] = '\0';
      strncpy(remoteSoftware, p->software, MESSAGE_MAX_INFO_STR-1);
      remoteSoftware[MESSAGE_MAX_INFO_STR-1] = '\0';
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      printf("Remote OS       = %s\n", remoteOS);
      printf("Remote software = %s\n", remoteSoftware);
#endif
      return sendMsgType(sockfd, MSG_RETURN_SUCCESS);
    }
    break;
  case MSG_REQUEST_POSITION:
    //read(sockfd, sndMsgPos, sizeof(MrsvMsgPosition));
    if (currentPos != NULL) {
      sndMsgPos->nx = (int)currentPos->getNR();
      sndMsgPos->ny = (int)currentPos->getNA();
      sndMsgPos->nz = (int)currentPos->getNS();
      sndMsgPos->tx = (int)currentPos->getTR();
      sndMsgPos->ty = (int)currentPos->getTA();
      sndMsgPos->tz = (int)currentPos->getTS();
      sndMsgPos->px = (int)currentPos->getPR();
      sndMsgPos->py = (int)currentPos->getPA();
      sndMsgPos->pz = (int)currentPos->getPS();
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      DUMP_MSG_POSITION(sndMsgPos);
#endif
      sendMsgType(sockfd, MSG_SEND_POSITION);
      return sendMsgPosition(sockfd, sndMsgPos);
    }
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
    printf("Failed to send position parameters.\n");
#endif
    break;
  default:
    break;
  }

  return sendMsgType(sockfd, MSG_RETURN_ERROR);
}


int MrsvrMessageServer::getSvrStatus()
{
  if (getStatus() == MrsvrThread::RUN) {
    if (masterSockFD < 0 ) {
      return SVR_WAIT;
    } else {
      return SVR_CONNECTED;
    }
  } else {
    return SVR_STOP;
  }
}


const char* MrsvrMessageServer::getSvrStatusStr()
{
  if (getStatus() == MrsvrThread::RUN) {
    if (masterSockFD < 0 ) {
      return svrStatusStr[SVR_WAIT];
    } else {
      return svrStatusStr[SVR_CONNECTED];
    }
  } else {
    return svrStatusStr[SVR_STOP];
  }
}


long MrsvrMessageServer::getConnectionTime()
{
  time_t current;

  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    time(&current);
    return (long)(current - startTime);
  } else {
    return 0;
  }
}


const char* MrsvrMessageServer::getRemoteOS()
{
  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    return remoteOS;
  } else {
    return "----";
  }
}


const char* MrsvrMessageServer::getRemoteSoftware()
{
  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    return remoteSoftware;
  } else {
    return "----";
  }
}


const char* MrsvrMessageServer::getRemoteHost()
{
  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    if (peer_host) {
      return peer_host->h_name;
    } else {
      return "----";
    }
  } else {
    return "----";
  }
}


const char* MrsvrMessageServer::getRemoteIP()
{
  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    return inet_ntoa(peer_sin.sin_addr);
  } else {
    return "--.--.--.--";
  }
}


int MrsvrMessageServer::getRemotePort()
{ 
  if (getStatus() == MrsvrThread::RUN &&
      masterSockFD > 0){
    return (int)ntohs(peer_sin.sin_port); 
  } else {
    return (int)0;
  }
}


bool MrsvrMessageServer::getTarget(double* target)
{
  pthread_mutex_lock(&mtxCommand);  
  if (fSetTarget == false) {
    pthread_mutex_unlock(&mtxCommand);  
    return false;
  }
  fSetTarget = false;
  for (int i = 0; i < 3; i ++) {
    target[i] = targetPosition[i];
  }
  pthread_mutex_unlock(&mtxCommand);  
  return true;
}


/*
bool MrsvrMessageServer::getStop()
{
  bool r;
  pthread_mutex_lock(&mtxCommand);  
  r = fStop;
  fStop = false;
  pthread_mutex_unlock(&mtxCommand);  
  return r;
}
*/

bool MrsvrMessageServer::getMode(int* next)
{
  bool r;
  *next = -1;
  pthread_mutex_lock(&mtxCommand);  
  if (nextRobotMode < 0) {
    r = false;
  } else {
    r = true;
    *next = nextRobotMode;
  }
  pthread_mutex_unlock(&mtxCommand);  
  return r;
}



int MrsvrMessageServer::setTarget(double* target)
{
  pthread_mutex_lock(&mtxCommand);  
  for (int i = 0; i < 3; i ++)  {
    targetPosition[i] = target[i];
  }
  fSetTarget = true;
  pthread_mutex_unlock(&mtxCommand);
  //
  // The received target should be validated based on
  // physical condition. This will be implemented in future.
  //
  return TARGET_ACCEPTED;
}


int MrsvrMessageServer::setMode(const char* param)
{
  int mode = -1;
  pthread_mutex_lock(&mtxCommand);
  for (int i = 0; i < MSG_SVR_NUM_MODE; i ++) {
    //fprintf(stderr, "MrsvrMessageServer::setMode(): compare %s vs %s",
    //MrsvrMessageServer::robotModeStr[i], param);
    if (strncmp(MrsvrMessageServer::robotModeStr[i], param, 
                strlen(MrsvrMessageServer::robotModeStr[i])) == 0) {
      mode = i;
      break;
    }
  }
  if (mode >= 0) {
    nextRobotMode = mode;
  }

  pthread_mutex_unlock(&mtxCommand);  

  return mode;
}


void MrsvrMessageServer::getRobotStatus(int* mode, int* outrange, int* lock)
{
  *mode     = robotStatus->getMode();
  *outrange = 0;
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    if (robotStatus->getOutOfRange(i) != 0) {
      *outrange = 1;
    }
  }
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    if (robotStatus->getActuatorLockStatus(i)) {
      lock[i] = 1;
    } else {
      lock[i] = 0;
    }
  }
}

