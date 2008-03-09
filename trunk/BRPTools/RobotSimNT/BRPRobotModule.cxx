#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4290)
#endif

#include <OpenTracker/input/BRPRobotModule.h>
#include <OpenTracker/core/Context.h>
#include <iostream>


namespace ot {

//------------------------------------------------------------------------------
// In following macro, module registration function named
// "registerModuleBRPRobotModule()"  is defined.
// This would be called from SPLModule class.
//------------------------------------------------------------------------------
OT_MODULE_REGISTER_FUNC(BRPRobotModule){
  OT_MODULE_REGISTRATION_DEFAULT(BRPRobotModule, "BRPRobotConfig" );
}


//------------------------------------------------------------------------------
// FUNCTION: BRPRobotModlue()
//
// Constructor of this class
//------------------------------------------------------------------------------
BRPRobotModule::BRPRobotModule() 
{
  source    = NULL;
  sink      = NULL;
}


//------------------------------------------------------------------------------
// FUNCTION: BRPRobotModlue()
//
// Destructor of this class
//------------------------------------------------------------------------------
BRPRobotModule::~BRPRobotModule()
{

}


//------------------------------------------------------------------------------
// FUNCTION: createNode()
//
// This function creates node for "Sink" and "Source".
// Called when NaviTrack finds a name of node in XML file during starting up.
// Note that this implementation does not allow to have multiple nodes.
// Use std::vector to manage multiple nodes.
//------------------------------------------------------------------------------
ot::Node * BRPRobotModule::createNode( const std::string& name,  ot::StringTable& attributes)
{
  if( name.compare("BRPRobotSink") == 0 )
    {
      std::string strName=attributes.get("name");
      std::cout << "BRPRobotModule::createNode(): creating a BRPRobotSink node" << std::endl;
      std::cout << "BRPRobotModule::createNode(): attribute\"name\" is " << strName << std::endl;
      sink =  new BRPRobotSink();
      return sink;
    }
  if(name.compare("BRPRobotSource") == 0 )
    {
      std::string strName=attributes.get("name");
      std::cout << "BRPRobotModule::createNode(): creating a BRPRobotSource node" << std::endl;
      std::cout << "BRPRobotModule::createNode(): attribute\"name\" is " << strName << std::endl;
      source = new BRPRobotSource(strName);
      return source;
    }
  return NULL;
}


//------------------------------------------------------------------------------
// FUNCTION: pushState
//
// This function updates trackerinfo for all sources of this module
//------------------------------------------------------------------------------
void BRPRobotModule::pushEvent()
{
  static short* pix = NULL;

  if(source)
    {
//      std::cout << "BRPRobotModule::pushEvent() is called." << std::endl;
//      source->event.getPosition()[0] += 0.01;
//      source->event.getPosition()[1] += 0.02;
//      source->event.getPosition()[2] += 0.03;
//      source->event.getOrientation()[0] += 0.01;
//      source->event.getOrientation()[1] += 0.02;
//      source->event.getOrientation()[2] += 0.03;
//      source->event.getOrientation()[3] += 0.04;
//      
    }
}
  
  
//------------------------------------------------------------------------------
// FUNCTION: init()
//
// This function initializes the module.
//------------------------------------------------------------------------------
void BRPRobotModule::init(StringTable& attributes, ConfigNode * localTree)
{
  std::cout << "BRPRobotModule::init() is called." << std::endl;

  std::string strName=attributes.get("name");
  std::cout << "BRPRobotModule::init(): attribute \"name\" is " 
            << strName << std::endl;
}


//------------------------------------------------------------------------------
// FUNCTION: setTracker()
//
// This function updates trackerinfo for all sources of this module
//------------------------------------------------------------------------------
void BRPRobotModule::setTracker(std::vector<float> pos,std::vector<float> quat)
{
  std::cout << "BRPRobotModule::setTracker() is called." << std::endl;

  if (pos.size() != 3 || quat.size() != 4) {
    std::cout << "BRPRobotModule::setTracker(): illegal vector size." << std::endl;
    return;
  }

  if (source!=NULL)
    {
      ot::Event *event = new ot::Event();
      event->setAttribute("position",pos);
      event->setAttribute("orientation",quat);
      event->getButton()=0;
      event->getConfidence()=1.0;
      source->changed=1;
      event->timeStamp();
      source->updateObservers( *event );
    }
}


//------------------------------------------------------------------------------
// FUNCTION: sendCurrentWorkphase()
//
// This function get tracker information from sink node
//------------------------------------------------------------------------------
void BRPRobotModule::sendCurrentWorkphase(int current)
{
  if (source != NULL)
    {
      char* status = NULL;
      if (current == WP_STARTUP)          status = "START_UP";
      else if (current == WP_PLANNING)    status = "PLANNING";
      else if (current == WP_CALIBRATION) status = "CALIBRATION";
      else if (current == WP_TARGETING)   status = "TARGETING";
      else if (current == WP_MANUAL)      status = "MANUAL";
      else if (current == WP_EMERGENCY)   status = "EMERGENCY";

      if (status)
        {
          ot::Event *event = new ot::Event();
          event->setAttribute("status", std::string(status));
          event->getConfidence()=1.0;
          source->changed=1;
          event->timeStamp();
          source->updateObservers( *event );
        }
    }
}



} // end of "namespace ot"





