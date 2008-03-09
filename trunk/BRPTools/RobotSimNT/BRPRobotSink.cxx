#include <OpenTracker/input/BRPRobotSink.h>
#include <OpenTracker/input/BRPRobotModule.h>

#ifdef WIN32
#include <windows.h>
#include <OpenTracker/misc/SPLOT_Win.h>
#endif

#include <iostream>



namespace ot {

  BRPRobotSink::BRPRobotSink()
  {
    position.resize(3);
    orientation.resize(4);

    position[0] = position[1] = position[2] = 0.0;
    orientation[0] = orientation[1] = orientation[2] = orientation[3] = 0.0;
    workphase = BRPRobotModule::WP_STARTUP;
  }

  BRPRobotSink::~BRPRobotSink()
  {
  }

  void BRPRobotSink::onEventGenerated( Event& event, Node& generator)
  {

    std::cerr << "BRPRobotSink::onEventGenerated()" << std::endl;


    if (event.hasAttribute("workphase"))
      {
        std::string strWorkphase = event.getAttribute<std::string>("workphase", "");
        std::cerr << "workphase = " << strWorkphase << std::endl;
        if (strWorkphase.compare("START_UP") == 0)
          {
            workphase = BRPRobotModule::WP_STARTUP;
          }
        else if (strWorkphase.compare("PLANNING") == 0)
          {
            workphase = BRPRobotModule::WP_PLANNING;
          }
        else if (strWorkphase.compare("CALIBRATION") == 0)
          {
            workphase = BRPRobotModule::WP_CALIBRATION;
          }
        else if (strWorkphase.compare("TARGETING") == 0)
          {
            workphase = BRPRobotModule::WP_TARGETING;
          }
        else if (strWorkphase.compare("MANUAL") == 0)
          {
            workphase = BRPRobotModule::WP_MANUAL;
          }
        else if (strWorkphase.compare("EMERGENCY") == 0)
          {
            workphase = BRPRobotModule::WP_EMERGENCY;
          }
      }


    std::string command = "";
    if (event.hasAttribute("command"))
      {
        command = event.getAttribute<std::string>("command", "");
      }

    if (command.compare("TARGET"))
      {
        if (event.hasAttribute("position"))
          {
            for(int i = 0; i < 3; i ++)
              position[i]=event.getPosition()[i];
          }
        
        if (event.hasAttribute("orientation"))
          {
            for  (int i = 0; i < 4; i ++) {
              orientation[i]= event.getOrientation()[i];
            }
          }
      }
    else if (command.compare("ZFRAME"))
      {
        if (event.hasAttribute("position"))
          {
            for(int i = 0; i < 3; i ++)
              zframe_position[i]=event.getPosition()[i];
          }
        
        if (event.hasAttribute("orientation"))
          {
            for  (int i = 0; i < 4; i ++) {
              zframe_orientation[i]= event.getOrientation()[i];
            }
          }
      }
      

    std::cerr << "BRPRobotSink = " << position[0]
              << ", " << position[1]
              << ", " << position[2] << std::endl;
    
  }


} // end of namespace ot
