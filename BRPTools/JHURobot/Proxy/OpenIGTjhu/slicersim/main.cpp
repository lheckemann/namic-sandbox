// Slicer Simulator

#include "BRPtprMessagesProc.h"
#include "BRPtprMessages.h"
#include "BRPplatform.h"

#define HOST "127.0.0.1"
#define PORT  "5678"

int main(int argc, char **argv)
{
 char *help_mess = "\n1-startup 2-planning 3-calibration 4-targeting 5-manual 6-emergency\n z-zframe s-getStatus c-getCoord p-GoTo q-Quit\n"; 

 printf("Navigation software simulator\n");
 bool lEnd = false;

 try {

        BRPtprMessageProc simulator;

        while (!lEnd) {

                if (!simulator.Connect(HOST, PORT)) {
                        std::cout << "Error setting up TCP handler!\n";
                }

                sleep(1);

                if (!simulator.IsOpen() ) {
                        std::cout << "Can't connect to Robot Proxy " << "[" << HOST << ":" << PORT << "]\n\n Is the Proxy running?\n(waiting 4 sec)\n";
                        //exit(55);
                        sleep(4);
                        continue;
                }
                simulator.StartListen();

                std::cout << "Connected to Robot Proxy " << "[" << HOST << ":" << PORT << "]\n";
                printf("Key loop\n");

                
                char cKey;
                while (!lEnd) {
                        
                        std::cout << help_mess;
                        std::cin >> cKey;
                        //if (!simulator.IsOpen() ) { std::cout << "Socket closed?\n\n\n"; break;}

                        switch (cKey) {
                                case '1': simulator.SendCommand(COMM_BRPTPR_START_UP); break;
                                case '2': simulator.SendCommand(COMM_BRPTPR_PLANNING); break;
                                case '3': simulator.SendCommand(COMM_BRPTPR_CALIBRATION); break;
                                case '4': simulator.SendCommand(COMM_BRPTPR_TARGETTING); break;
                                case '5': simulator.SendCommand(COMM_BRPTPR_MANUAL); break;
                                case '6': simulator.SendCommand(COMM_BRPTPR_EMERGENCY); break;
                                case 'c': simulator.SendCommand(COMM_BRPTPR_GET_POSITION); break;
                                case 's': simulator.SendCommand(COMM_BRPTPR_GET_STATUS); break;
                                case 'q': lEnd=true;break;

                                default: std::cout << "(unknown key)\n";
                        }
                } // waiting for key

                simulator.close();

                std::cout << "Connection closed\n";

        }
 } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
 }

 



 return 0;
}  




