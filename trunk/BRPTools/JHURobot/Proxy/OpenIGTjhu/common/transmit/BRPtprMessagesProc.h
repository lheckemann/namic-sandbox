/** @file
  \brief BRPtprMessageProc(essing): Create struct from network packet and vice versa. igtlMessage <-> BRPtprMessageStructType

  IGT Client (Slicer) <-> Network <-> IGT Server (Proxy) <-----pipe----> Robot Controller
  It is important to run NTP in both OpenTracker sides! (TimeStamp is in local time)
*/ 



#ifndef _BRPtprMessagesProc_h_
#define _BRPtprMessagesProc_h_

//#include "BRPTPRInterface.h"
#include "BRPtprMessages.h"
#include "igtlNetwork.h"

class BRPtprMessageProc: public igtlNetwork
{
public:
 BRPtprMessageProc(void);
 virtual ~BRPtprMessageProc(void);
 void SendCommand(BRPtprMessageCommandType cmd);

protected:
 virtual void PacketReceived(igtlMessage * msg);
 void SetUpHeader(igtlMessage & msg,BRPtprMessageCommandType cmd);

};

/*

/// BRPtprMessageStructType extension - virtual functions can't be used! It will segfault your process...
class BRPtprMessageProc: public BRPtprMessageStructType
{
public:
        BRPtprMessageProc(BRPtprBufferType BufferType);

        bool PackToIGTPacket();
        bool UnpackFromIGTPacket(const );
 
        bool CreateEventFromThis(ot::Event *event);        // this buffer -> event
        bool CreateThisFromEvent(const ot::Event *event); // event -> this buffer

protected:
        std::string FloatToString(float a);
        std::string IntToString(int a);

        void CreateWorkphaseEvent(ot::Event *event, BRPtprMessageCommandType cmd);
        void CreateCommandEvent(ot::Event *event, BRPtprMessageCommandType cmd);

        void CreateZFrameEvent(ot::Event *event);
        void CreateGoToCoordinateEvent(ot::Event *event);
        void CreateActualCoordinatesEvent(ot::Event *event);
        void CreateActualRobotStatusEvent(ot::Event *event);

        void MakeThisWorkphaseEvent(const ot::Event *event, BRPtprMessageCommandType cmd);
        void MakeThisCommandEvent(const ot::Event *event, BRPtprMessageCommandType cmd);

        void MakeThisZFrameEvent(const ot::Event *event);
        void MakeThisGoToCoordinateEvent(const ot::Event *event);
        void MakeThisActualCoordinatesEvent(const ot::Event *event);
        void MakeThisActualRobotStatusEvent(const ot::Event *event);
};
*/

#endif // _BRPtprMessagesProc_h_

