#include "igtlNetwork.h"



/// Constructor
igtlNetwork::igtlNetwork(void)
{
        socket = NULL;
        resolver = NULL;
        listen_thread = NULL;
        server_endpoint = NULL;
        acceptor = NULL;
}


/// Destructior
igtlNetwork::~igtlNetwork(void)
{
        if (this->listen_thread) this->do_close();
        if (this->socket) { delete this->socket;this->socket=NULL; }
        if (this->resolver) { delete this->resolver;this->resolver=NULL; }

        if (this->server_endpoint) { delete this->server_endpoint;this->server_endpoint=NULL; }
        if (this->acceptor) { delete this->acceptor;this->acceptor=NULL; }
}


/// Conect to a given IP:Port
bool igtlNetwork::Connect(char *ip, char *port)
{
 do_close();

 this->resolver = new tcp::resolver(io_service);
 if (!this->resolver) return false;

 tcp::resolver::query query(ip, port);
 this->s_iterator = this->resolver->resolve(query);

 this->socket_endpoint = *s_iterator;

 this->socket = new tcp::socket(io_service);
 if (!this->socket) return false;

 this->socket->async_connect(this->socket_endpoint,
        boost::bind(&igtlNetwork::handle_receiving, this,
                boost::asio::placeholders::error, ++s_iterator));

 return this->socket->is_open();
}




/// Spawn listening thread
void igtlNetwork::StartListen(void)
{
        listen_thread = new     boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
        assert(listen_thread); /// TODO  check
}


/// Start up packet receiving
void igtlNetwork::handle_receiving(const boost::system::error_code& error,  tcp::resolver::iterator endpoint_iterator)
{
        if (!error)
    {
      boost::asio::async_read(*socket,
          boost::asio::buffer(read_msg.data(), igtlMessage::header_length),
          boost::bind(&igtlNetwork::handle_read_header, this,
            boost::asio::placeholders::error));
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
      socket->close();
      tcp::endpoint endpoint = *endpoint_iterator;
      socket->async_connect(endpoint,
          boost::bind(&igtlNetwork::handle_receiving, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
}


/// Header received - jump forward to body receiving
void igtlNetwork::handle_read_header(const boost::system::error_code& error)
 {
    if (!error && read_msg.decode_header())
    {
          // header received, now look for body
      boost::asio::async_read(*socket,
          boost::asio::buffer(read_msg.body(), read_msg.body_length()),
          boost::bind(&igtlNetwork::handle_read_body, this,
            boost::asio::placeholders::error));
    }
    else
    {
      printf("Error receiving header! Closing network connection.\n");
      do_close();
    }
}

void igtlNetwork::PacketReceived(igtlMessage *msg)
{
  // just write it out - the descendant will overwrite this anyways
  std::cout.write(msg->body(), msg->body_length());
  std::cout << " [Length: " << msg->body_length() << "]\n";
}


/// Body received. Process and jump back to header receiving.
void igtlNetwork::handle_read_body(const boost::system::error_code& error)
{
        if (!error) {
          // Packet received, process it!
          PacketReceived(&read_msg);

      // look for header again
      boost::asio::async_read(*socket,
          boost::asio::buffer(read_msg.data(), igtlMessage::header_length),
          boost::bind(&igtlNetwork::handle_read_header, this,
            boost::asio::placeholders::error));
    }
    else
    {
          printf("Error receiving body! Closing network connection. \n");
      do_close();
    }
}

/// queue close request
void igtlNetwork::close(void)
{
  io_service.post(boost::bind(&igtlNetwork::do_close, this));
}


/// Close down the connection
void igtlNetwork::do_close(void)
{
        if (this->socket) {
          this->socket->close();
          this->socket=NULL;
        }

        if (this->listen_thread) {
          //listen_thread->join();
          delete listen_thread;
          listen_thread=NULL;
        }
}

bool igtlNetwork::IsOpen(void)
{
        if (!this) return false;
        if (!this->socket) return false;
        if (this->listen_thread) return false;
        return this->socket->is_open();
}


/// Queue packet sending
void igtlNetwork::SendPacket(const igtlMessage& msg)
{
  io_service.post(boost::bind(&igtlNetwork::do_write, this, msg));
}

/// Add packet to list, start async sending
void igtlNetwork::do_write(igtlMessage msg)
{
    bool write_in_progress = !write_msgs.empty();
    write_msgs.push_back(msg);
    if (!write_in_progress) {
      boost::asio::async_write(*socket,
          boost::asio::buffer(write_msgs.front().data(),
            write_msgs.front().length()),
          boost::bind(&igtlNetwork::handle_write, this,
            boost::asio::placeholders::error));
    }
}

/// Packet sent, remove from list. If list is not empty, start sending the next one.
void igtlNetwork::handle_write(const boost::system::error_code& error)
{
    if (!error)  {
      write_msgs.pop_front();
      if (!write_msgs.empty())  {
        boost::asio::async_write(*socket,
            boost::asio::buffer(write_msgs.front().data(),
              write_msgs.front().length()),
            boost::bind(&igtlNetwork::handle_write, this,
              boost::asio::placeholders::error));
      }
    }
    else
    {
          printf("Error sending packet! Closing network connection.\n");
      do_close();
    }
}



// ------------------ Server ------------------


/// Listen on this port indefinitely
void igtlNetwork::RunServer(int port)
{
        server_endpoint = new tcp::endpoint(tcp::v4(), port);
        socket = new tcp::socket(io_service);
        assert(socket); /// TODO  check socket
        acceptor = new tcp::acceptor(io_service, *server_endpoint);
        assert(acceptor); /// TODO  check acceptor
        acceptor->async_accept( *socket,
        boost::bind(&igtlNetwork::handle_accept, this, boost::asio::placeholders::error));
        io_service.run();
}


/// accept incoming connection
void igtlNetwork::handle_accept(const boost::system::error_code& error)
{
    if (!error)
    {
      start_server_receive();
      acceptor->async_accept(*socket,
          boost::bind(&igtlNetwork::handle_accept, this, boost::asio::placeholders::error));
    }
}


/// Receiving thread
void igtlNetwork::start_server_receive(void)
{
 boost::asio::async_read(*socket,
  boost::asio::buffer(read_msg.data(), igtlMessage::header_length),
   boost::bind(&igtlNetwork::handle_read_header, this,
    boost::asio::placeholders::error));
}
