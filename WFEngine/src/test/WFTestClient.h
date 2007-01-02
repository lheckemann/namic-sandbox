#ifndef WFTESTCLIENT_H_
#define WFTESTCLIENT_H_

namespace nmWFTestClient
{

class WFTestClient
{
public:
 static WFTestClient *New();
 
protected:
 WFTestClient();
 virtual ~WFTestClient();
 
 int initializeInterface();
};

}

#endif /*WFTESTCLIENT_H_*/
