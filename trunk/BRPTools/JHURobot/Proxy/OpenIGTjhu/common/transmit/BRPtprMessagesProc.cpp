#include "BRPtprMessagesProc.h"


BRPtprMessageProc::BRPtprMessageProc(void): igtlNetwork()
{
}
BRPtprMessageProc::~BRPtprMessageProc(void)
{
}

void BRPtprMessageProc::PacketReceived(igtlMessage * msg)
{
        std::cout << "Packet received: " << msg->get_header()->name << " body size: " << msg->get_header()->body_size << "\n";
}

void BRPtprMessageProc::SetUpHeader(igtlMessage & msg,BRPtprMessageCommandType cmd)
{
        /// TODO verify cmd boundary
        std::memcpy(msg.get_header()->name, BRPCommands[cmd].c_str(), BRPCommands[cmd].length() );
}

void BRPtprMessageProc::SendCommand(BRPtprMessageCommandType cmd)
{
        igtlMessage msg;
        SetUpHeader(msg, cmd);
        msg.encode_header();
    SendPacket(msg);
}

/*


 /// This buffer -> Event: Creates OpenTracker event out of a BRPtprMessageProc buffer
bool BRPtprMessageProc::CreateEventFromThis(ot::Event *event)
{
        assert(this); // it can happen!
        assert(event);

        switch (this->command) {
                case BRPtprSTART_UP:    CreateWorkphaseEvent(event, BRPtprSTART_UP); break;
                case BRPtprPLANNING:    CreateWorkphaseEvent(event, BRPtprPLANNING); break;
                case BRPtprCALIBRATION: CreateWorkphaseEvent(event, BRPtprCALIBRATION); break;
                case BRPtprTARGETING:   CreateWorkphaseEvent(event, BRPtprTARGETING); break;
                case BRPtprMANUAL:              CreateWorkphaseEvent(event, BRPtprMANUAL); break;
                case BRPtprEMERGENCY:   CreateWorkphaseEvent(event, BRPtprEMERGENCY); break;

                //case BRPtprInitializeRobot:   CreateCommandEvent(event, BRPtprInitializeRobot); break;
                case BRPtprSetZFrame:           CreateZFrameEvent(event); break;
                case BRPtprGoToCoordinate:      CreateGoToCoordinateEvent(event); break;
                case BRPtprGetCoordinates:      CreateCommandEvent(event, BRPtprGetCoordinates); break;
                case BRPtprGetRobotStatus:      CreateCommandEvent(event, BRPtprGetRobotStatus); break;
                case BRPtprActualCoordinates:   CreateActualCoordinatesEvent(event); break;
                case BRPtprActualRobotStatus:   CreateActualRobotStatusEvent(event); break;

                default:
                        printf("Command ID: %d is not implemented!\n",this->command);
                        if ( (this->command > 0) && (this->command < BRPtprMessageCommandLastCommand) )
                                printf("ID %d = %s\n",this->command, BRPCommands[this->command].c_str() );
                        assert(false);return false; // wrong command??
        }
        return true;
}


/// Event -> This buffer: Creates a BRPtprMessageProc buffer from a OpenTracker event
bool BRPtprMessageProc::CreateThisFromEvent(const ot::Event *event)
{
        assert(this);
        assert(event);

        // workphase, command or status

        if ( (!event->hasAttribute("workphase")) && (!event->hasAttribute("command")) && (!event->hasAttribute("status")) ) {
                printf("Each messsage should be workphase, command or status: %s \n", event->getPrintOut().c_str() );
                assert(false);  return false; // unknown event - not from BRPtpRobot!
        }

        if (event->hasAttribute("status")) {
                MakeThisActualRobotStatusEvent(event);
                return true;
        }

        std::string cmd;

        // we assume that this names are unique across the message types
        if ( event->hasAttribute("workphase") ) {
                cmd = event->getAttributeValueString("workphase");
        } else {
                cmd = event->getAttributeValueString("command");
        }

        for (int i=0;i<BRPtprMessageCommandLastCommand;i++) {
                //printf("Trying %s \n", BRPCommands[i].c_str());
                if ( BRPCommands[i]==cmd ) {
                        switch (i) {

                                case BRPtprSTART_UP:    MakeThisWorkphaseEvent(event, BRPtprSTART_UP); break;
                                case BRPtprPLANNING:    MakeThisWorkphaseEvent(event, BRPtprPLANNING); break;
                                case BRPtprCALIBRATION: MakeThisWorkphaseEvent(event, BRPtprCALIBRATION); break;
                                case BRPtprTARGETING:   MakeThisWorkphaseEvent(event, BRPtprTARGETING); break;
                                case BRPtprMANUAL:              MakeThisWorkphaseEvent(event, BRPtprMANUAL); break;
                                case BRPtprEMERGENCY:   MakeThisWorkphaseEvent(event, BRPtprEMERGENCY); break;

                                //case BRPtprInitializeRobot:   MakeThisCommandEvent(event, BRPtprInitializeRobot); break;
                                case BRPtprSetZFrame:           MakeThisZFrameEvent(event); break;
                                case BRPtprGoToCoordinate:      MakeThisGoToCoordinateEvent(event); break;

                                case BRPtprGetCoordinates:      MakeThisCommandEvent(event, BRPtprGetCoordinates); break;
                                case BRPtprGetRobotStatus:      MakeThisCommandEvent(event, BRPtprGetRobotStatus); break;

                                case BRPtprActualCoordinates:   MakeThisActualCoordinatesEvent(event); break;

                                default:  // wrong command??
                                        printf("BRPtprMessageProc::CreateThisFromEvent: Command ID: %d is not implemented!\n",i);
                                        if ( (i > 0) && (i < BRPtprMessageCommandLastCommand) )
                                                printf("ID %d = %s\n",i, BRPCommands[i].c_str() );
                                        assert(false);return false;
                        }
                        return true;
                }
        }
        printf("BRPtprMessageProc::CreateThisFromEvent: Command '%s' not found!\n",cmd.c_str());
        assert(false);
        return false;
}


/// Convert float to std::string
std::string BRPtprMessageProc::FloatToString(float a)
{
        std::ostringstream o;
        o << a; // returns true or false
        return o.str();
}


/// Convert int to std::string
std::string BRPtprMessageProc::IntToString(int a)
{
        std::ostringstream o;
        o << a; // returns true or false
        return o.str();
}


// Message type: "bool" "int" "long" "short" "unsigned_int" "unsigned_long" "unsigned_short" "double" "float" "string" "vector<float>"


/// Creates "event" from "buff": Workphase
void BRPtprMessageProc::CreateWorkphaseEvent(ot::Event *event, BRPtprMessageCommandType cmd)
{
        event->addAttribute("string", "workphase", BRPCommands[cmd]);
        event->timeStamp();
}

/// Creates "buffer" from "event": Workphase
void BRPtprMessageProc::MakeThisWorkphaseEvent(const ot::Event *event, BRPtprMessageCommandType cmd)
{
        this->command=BRPtprInvalidCommand;
        this->Clean(BRPtprSendBuffer);
        if (!event->hasAttribute("workphase")) { assert(false);return;  } // it must have "workphase" parameter!
        this->command=cmd;
}


/// Creates "event" from "buff": Command
void BRPtprMessageProc::CreateCommandEvent(ot::Event *event, BRPtprMessageCommandType cmd)
{
        event->addAttribute("string", "command", BRPCommands[cmd]);
        event->timeStamp();
}

/// Creates "buffer" from "event": Command
void BRPtprMessageProc::MakeThisCommandEvent(const ot::Event *event, BRPtprMessageCommandType cmd)
{
        this->command=BRPtprInvalidCommand;
        this->Clean(BRPtprSendBuffer);
        if (!event->hasAttribute("command")) { assert(false);return;  } // it must have "command" parameter!
        this->command=cmd;
}

void BRPtprMessageProc::CreateZFrameEvent(ot::Event *event)
{
        CreateCommandEvent(event, BRPtprSetZFrame);
        FLOAT_COPY3(this->position, event->getPosition());
        FLOAT_COPY4(this->orientation, event->getOrientation());
}

void BRPtprMessageProc::MakeThisZFrameEvent(const ot::Event *event)
{
        MakeThisCommandEvent(event, BRPtprSetZFrame);
        FLOAT_COPY3(event->getPosition(), this->position);
        FLOAT_COPY4(event->getOrientation(), this->orientation);
}

void BRPtprMessageProc::CreateGoToCoordinateEvent(ot::Event *event)
{
        CreateCommandEvent(event, BRPtprGoToCoordinate);
        FLOAT_COPY3(this->position, event->getPosition());
        FLOAT_COPY4(this->orientation, event->getOrientation());
}

void BRPtprMessageProc::MakeThisGoToCoordinateEvent(const ot::Event *event)
{
        MakeThisCommandEvent(event, BRPtprGoToCoordinate);
        FLOAT_COPY3(event->getPosition(), this->position);
        FLOAT_COPY4(event->getOrientation(), this->orientation);
}

void BRPtprMessageProc::CreateActualCoordinatesEvent(ot::Event *event)
{
        CreateCommandEvent(event, BRPtprActualCoordinates);
        FLOAT_COPY3(this->position, event->getPosition());
        FLOAT_COPY4(this->orientation, event->getOrientation());
        event->addAttribute("depth", ot::copyA2V(this->depth_vector,3) );
}

void BRPtprMessageProc::MakeThisActualCoordinatesEvent(const ot::Event *event)
{
        std::string cmd("depth");
        
        MakeThisCommandEvent(event, BRPtprActualCoordinates);
        if (!event->hasAttribute(cmd)) { assert(false);return;  } // it must have "command" parameter!
        FLOAT_COPY3(event->getPosition(), this->position);
        FLOAT_COPY4(event->getOrientation(), this->orientation);

        ot::Event * event2 = (ot::Event *)event; // Otherwise the next line will complain about const<->writable conversion
        FLOAT_COPY3(event2->getAttribute(cmd,std::vector<float>(3, .0f)), this->depth_vector);
}


/// Creates event from this object
void BRPtprMessageProc::CreateActualRobotStatusEvent(ot::Event *event)
{
        event->addAttribute("string", "status", BRPStatusMessages[this->status]);
        event->addAttribute("string", "message", "Message not yet implemented (proxy)");
        event->timeStamp();
}

/// Initializes this object from the event
void BRPtprMessageProc::MakeThisActualRobotStatusEvent(const ot::Event *event)
{
        this->command=BRPtprInvalidCommand;
        this->Clean(BRPtprSendBuffer);
        if (!event->hasAttribute("status")) { assert(false);return;  } // it must have "status" parameter!
        if (!event->hasAttribute("message")) { assert(false);return;  } // it must have "message" parameter!

        this->command=BRPtprActualRobotStatus;
        this->status = BRPTPRstatus_Invalid;

        std::string status_name = event->getAttributeValueString("status");

        for( BRPTPRstatusType e=BRPTPRstatus_Invalid; e <= BRPTPRstatus_Number; e = BRPTPRstatusType( e+1 ) )  {
                if (status_name == BRPStatusMessages[e]) {
                        this->status = e;
                        break;
                }
        }
        assert(this->status != BRPTPRstatus_Invalid);
         
        // !! ignore "message" for now
}
*/        



