#ifndef _ROBINIF_H_
#define _ROBINIF_H_

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

//using namespace std;

class RobinIF {  
 public:
  // Variables
  bool active;         // Valid Connection Made
  int sockfd;
  char buffer[256];    // Send/Receive Buffer -- Crappy Implementation
  
  //(Con/De)stuctor
  RobinIF(char* robinhost, int port);
  ~RobinIF(){};
  
  
  //Methods
  void dataFormat();
  void dataRequest();
  void dataRead();
  void error(char *msg);
  bool isAlive();
  std::vector<double> getOrientation(int device);
  std::vector<double> getPosition(int device);
  void setOrientation0(double q0, double q1, double q2, double q3);
  void setOrientation1(double q0, double q1, double q2, double q3);
  void setPosition0(double x, double y, double z);
  void setPosition1(double x, double y, double z);

 protected:
  double currentX0 ,currentY0 ,currentZ0; 
  double currentX1 ,currentY1 ,currentZ1;
  double currentq0_0,currentq0_1,currentq0_2,currentq0_3;
  double currentq1_0,currentq1_1,currentq1_2,currentq1_3;
};
  
#endif /* _ROBINIF_H_ */
