#include "BRPtprMessageToPipe.h"


BRPtprMessageToPipe::BRPtprMessageToPipe(void): BRPtprMessageProc()
{
}
BRPtprMessageToPipe::~BRPtprMessageToPipe(void)
{
}

void BRPtprMessageToPipe::PacketReceived(igtlMessage *msg)
{

        std::cout << "Packet received: " << msg->get_header()->name << " body size: " << msg->get_header()->body_size << "\n";
        if (pipe) {
                pipe->QueueCommand(msg);
        }

        //send it to pipe?
}


