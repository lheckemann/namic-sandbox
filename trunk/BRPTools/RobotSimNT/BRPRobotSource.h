#ifndef __BRP_ROBOT_SOURCE_H__
#define __BRP_ROBOT_SOURCE_H__

#include <OpenTracker/OpenTracker.h>
#include <string>

namespace ot {

  class BRPRobotSource : public Node
  {
  private:
    
  public:
    
    std::string StationName;
    
  public:
    BRPRobotSource(std::string stationName) : changed(0) {};
    virtual int isEventGenerator() {return 1;};
    int changed;
    
    Event event;
  };
    
} // end of namespace ot

#endif // __BRP_ROBOT_SOURCE_H__
