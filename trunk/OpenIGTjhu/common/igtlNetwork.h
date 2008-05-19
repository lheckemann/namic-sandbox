#ifndef _IGTL_NETWORK_
#define _IGTL_NETWORK_

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "igtlMessage.h"

using boost::asio::ip::tcp;

class igtlNetwork
{
public:
 igtlNetwork(void);
 virtual ~igtlNetwork(void);

 bool Connect(char *ip, char *port); // success?
 bool IsOpen(void);
 void StartListen(void);
 void close(void);
 void SendPacket(const igtlMessage& msg);

 void RunServer(int port);

protected:
 virtual void PacketReceived(igtlMessage *msg);

 void handle_receiving(const boost::system::error_code& error,  tcp::resolver::iterator endpoint_iterator);
 void handle_read_header(const boost::system::error_code& error);
 void handle_read_body(const boost::system::error_code& error);
 void do_close(void);
 void do_write(igtlMessage msg);
 void handle_write(const boost::system::error_code& error);

 boost::asio::io_service io_service;
 tcp::socket *socket;
 tcp::endpoint socket_endpoint;
 igtlMessage read_msg;
 igtlMessage_queue write_msgs;
 tcp::resolver::iterator s_iterator;
 tcp::resolver *resolver;
 boost::thread *listen_thread;

 void handle_accept(const boost::system::error_code& error);
 void start_server_receive(void);

 tcp::endpoint *server_endpoint;
 tcp::acceptor *acceptor;
};

#endif
