#ifndef __BRP_ROBOT_MODULE_H__
#define __BRP_ROBOT_MODULE_H__

#include <OpenTracker/OpenTracker.h>
#include <OpenTracker/dllinclude.h>
#include <OpenTracker/input/SPLModules.h>
#include <OpenTracker/input/BRPRobotSink.h>
#include <OpenTracker/input/BRPRobotSource.h>

#include <OpenTracker/types/Image.h>

#include <string>

namespace ot {

class OPENTRACKER_API BRPRobotModule : public Module, public NodeFactory
{
 public:
  // workphases
  enum {
    WP_STARTUP,
    WP_PLANNING,
    WP_CALIBRATION,
    WP_TARGETING,
    WP_MANUAL,
    WP_EMERGENCY
  };

 public:
  
  // Constructor and destructor
  BRPRobotModule();
  virtual ~BRPRobotModule();
  
  Node* createNode(const std::string& name,  ot::StringTable& attributes);
  void  pushEvent();
  void  pullEvent() {};
  void  init(StringTable&, ConfigNode *);

  void  setTracker(std::vector<float> pos,std::vector<float> quat);

  void  sendCurrentWorkphase(int current);

  
  inline BRPRobotSink* getSink() { return sink; };

 private:
  
  BRPRobotSink*   sink;
  BRPRobotSource* source;
  
  friend class  BRPRobotSink;
  friend class  BRPRobotSource;
};

OT_MODULE(BRPRobotModule);


} // end of namespace ot

#endif // __BRP_ROBOT_MODULE_H__
