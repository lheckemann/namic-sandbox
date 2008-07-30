#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <vector>

#include "RobinIF.h"

using namespace std;

RobinIF::RobinIF(char* robinhost, int port){
  
  int n;
  struct sockaddr_in serv_addr; 
  struct hostent *server;
  
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  server = gethostbyname(robinhost);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");  
  else{
    active = true;
  }  
}

void RobinIF::error(char *msg)
{
  perror(msg);
  exit(0);
}

bool RobinIF::isAlive(){
  return active;
}


void RobinIF::dataFormat(){  
  int n;  // number of data packets
  char* quat  = "\r";
  n = write(sockfd,quat,strlen(quat));
  if (n < 0) 
    error("ERROR Requesting data from Robin Host");
}

void RobinIF::dataRequest(){  
  int n;  // number of data packets
  char* data1 = "DATA 1";
  n = write(sockfd,data1,strlen(data1));
  
  if (n < 0) 
    error("ERROR Requesting data from Robin Host");
}


void RobinIF::dataRead(){
  int n;   // number of data packets
  //Waiting to determine if command is valid
  n=read(sockfd,buffer,255);
  if(n<0)
    error("ERROR link dead");
  
  printf("%s\n",buffer);
  std::string data0,data1;
  double x0,y0,z0,q0_0,q0_1,q0_2,q0_3,stat0,bt0;
  double x1,y1,z1,q1_0,q1_1,q1_2,q1_3,stat1,bt1;
  
 std:stringstream sttr(buffer);
  sttr >> data0 >> x0 >> y0 >> z0 >> q0_0 >> q0_1 >> q0_2 >> q0_3 >> stat0 >> bt0
       >> data1 >> x1 >> y1 >> z1 >> q1_0 >> q1_1 >> q1_2 >> q1_3 >> stat1 >> bt1;
  
   
        cout <<"Data 0 = "   << data0 << endl;
        cout <<"Data x0 = "  << x0 << endl;
        cout <<"Data y0 = "  << y0 << endl;
        cout <<"Data z0 = "  << z0 << endl;
        cout <<"Data q0_0 = " << q0_0 << endl;
        cout <<"Data q0_1 = " << q0_1 << endl;
        cout <<"Data q0_2 = " << q0_2 << endl;
        cout <<"Data q0_3 = " << q0_3 << endl;
        cout <<"Data 1 = "   << data1 << endl;
        cout <<"Data x1 = "  << x1 << endl;
        cout <<"Data y1 = "  << y1 << endl;
        cout <<"Data z1 = "  << z1 << endl;
        cout <<"Data q1_0 = " << q1_0 << endl;
        cout <<"Data q1_1 = " << q1_1 << endl;
        cout <<"Data q1_2 = " << q1_2 << endl;
        cout <<"Data q1_3 = " << q1_3 << endl;
        
        
        if(data0.compare("DATA0")==0){
          setPosition0(x0,y0,z0);
          setOrientation0(q0_0,q0_1,q0_2,q0_3);
        }
        if(data1.compare("DATA1")==0){
          setPosition1(x1,y1,z1);
          setOrientation0(q1_0,q1_1,q1_2,q1_3);
        }
    bzero(buffer,256);
}

vector<double> RobinIF::getOrientation(int device){
  vector<double> v(4);
  
  switch(device){
  case 0:
    v[0] = currentq0_0;
    v[1] = currentq0_1;
    v[2] = currentq0_2;
    v[3] = currentq0_3;
    break;
  case 1:
    v[0] = currentq1_0;
    v[1] = currentq1_1;
    v[2] = currentq1_2;
    v[3] = currentq1_3;
    break;
  default :
    break;
  }
  
  return v;
}

vector<double> RobinIF::getPosition(int device){
  vector<double> v(3);
  
  switch(device){
  case 0:
    v[0] = currentX0;
    v[1] = currentY0;
    v[2] = currentZ0;break;
  case 1:
    v[0] = currentX1;
    v[1] = currentY1;
    v[2] = currentZ1;
    break;
  default :
    break;
  }
  
  return v;
}

void RobinIF::setOrientation0(double q0, double q1, double q2, double q3){
  currentq0_0 = q0;
  currentq0_1 = q1;
  currentq0_2 = q2;
  currentq0_3 = q3;
}
void RobinIF::setOrientation1(double q0, double q1, double q2, double q3){
  currentq1_0 = q0;
  currentq1_1 = q1;
  currentq1_2 = q2;
  currentq1_3 = q3;
}

void RobinIF::setPosition0(double x, double y, double z){
  currentX0 = x;
  currentY0 = y;
  currentZ0 = z;
}

void RobinIF::setPosition1(double x, double y, double z){
  currentX1 = x;
  currentY1 = y;
  currentZ1 = z;
}

