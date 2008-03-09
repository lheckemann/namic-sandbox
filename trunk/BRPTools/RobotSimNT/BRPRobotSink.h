#ifndef __MY_TUTORIAL_SINK_H__
#define __MY_TUTORIAL_SINK_H__

#include <OpenTracker/OpenTracker.h>

namespace ot {
  
  class BRPRobotSink : public ot::Node
  {
  private:
    std::vector<float> position;
    std::vector<float> orientation;

    std::vector<float> zframe_position;
    std::vector<float> zframe_orientation;

    int workphase;

    // This node has two bufferes for received data.

  public:
    
    BRPRobotSink();
    ~BRPRobotSink();
    
  public:
    
    virtual int isEventGenerator() {return 1;};
    virtual void onEventGenerated(Event&, Node&);

    inline void getTracker(std::vector<float>& pos, std::vector<float>& ori)
    {
      for (int i = 0; i < 3; i ++) pos[i] = position[i];
      for (int i = 0; i < 4; i ++) ori[i] = orientation[i];
    }

    inline void getZTracker(std::vector<float>& pos, std::vector<float>& ori)
    {
      for (int i = 0; i < 3; i ++) pos[i] = zframe_position[i];
      for (int i = 0; i < 4; i ++) ori[i] = zframe_orientation[i];
    }

    inline int getLastWorkPhaseCmd() {
      return workphase;
    }
      
  };

} // end of namespace ot

#endif // end of __MY_TUTORIAL_SINK_H
