// For the Proxy
// Sends the received images through the pipe

// Second thread: get message from Slicer, send it through pipe

#ifndef _BRPtprMessagesToPipe_h_
#define _BRPtprMessagesToPipe_h_

#include "BRPtprMessagesProc.h"
#include "BRPtprMessages.h"
#include "igtlNetwork.h"
#include "BRPtprPipe.h"


class BRPtprMessageToPipe: public BRPtprMessageProc
{
public:
 BRPtprMessageToPipe(void);
 virtual ~BRPtprMessageToPipe(void);
 void SetPipe(BRPtprPipe *p) {pipe=p;};
protected:
 virtual void PacketReceived(igtlMessage * msg);
 BRPtprPipe *pipe;

};



#endif
