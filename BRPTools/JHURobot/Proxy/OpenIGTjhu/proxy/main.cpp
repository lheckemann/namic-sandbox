// Proxy main

#include "BRPtprMessageToPipe.h"
#include "BRPtprPipe.h"


#define PORT  5678

int main(int argc, char **argv)
{

 printf("Pipe initialized\n");

 // First thread: get messages from pipe, send it to Slicer
 BRPtprPipe pipe;
 pipe.Initialize();

 while (true) {
  std::cout << "Proxy: Listening on port " << PORT << "\n";

  try {
        // Second thread: get message from Slicer, send it through pipe
        BRPtprMessageToPipe proxy;

        proxy.SetPipe(&pipe);
        pipe.SetNetwork(&proxy);

        proxy.RunServer(PORT);

  } catch (std::exception& e) {
          std::cerr << "Proxy: Exception in main: " << e.what() << "\n";
  }
  std::cout << "Proxy: Network connection was lost.\n\n";
 }

 return 0;
}


