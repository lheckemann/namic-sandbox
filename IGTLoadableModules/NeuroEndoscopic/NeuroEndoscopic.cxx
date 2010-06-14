#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <termios.h>

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlClientSocket.h>

#define RESPONSE_SIZE 24
#define POS_SCALE 72/32767
#define ANGLE_SCALE 1/32767

#define STREAM "stream"
#define POINT  "point"

#define DATAFLOW POINT

///////////////////////////////////////////////////////////////////////////////
//from pg 210 of 3D Guidance medSAFE Installation and Operation Guide manual (Ascension)

static void
unpack( unsigned char *buffer, short *output, int size )
{
  int i,j;

    for( i = 0, j = 0; i < size; i += 2 ) {
        //shift ls
        buffer[ i ] = buffer[ i ] << 1;

        output[ j++ ] = ((buffer[ i + 1 ] << 8) | buffer[ i ]) << 1;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Thread to control input keyboard, to leave the while loop

void* keybord_input(void* data)
{
  bool *exit_while = (bool*)data;

  while(getchar() != 'q'){}
 
  (*exit_while) = true;

}

//////////////////////////////////////////////////////////////////////////////

int main()
{

//-----------------
// Open RS232 Communication (via USB Converter)

int usb_device = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);

//-----------------
// Define commands 

char position = 0x56;
char reset = 0x62;
char run = 0x46;
char stream = 0x40;
char examine = 0x4F;
char tracker_status = 0x00;
char point = 0x42;
char pos_ang = 0x59;
char sleep = 0x47;
char first_sensor = 0xF1;
char pos_matrix = 0x5A;

char fast_stream_speed = 0x51;
char medium_fast_stream_speed = 0x52;
char medium_slow_stream_speed = 0x53;
char slow_stream_speed = 0x54;

//-----------------
// Buffer to receive results of command sent

unsigned char buffer[RESPONSE_SIZE] = {0};
short unpack_coordinates[RESPONSE_SIZE] = {0};

//-----------------
// Termios structure to configure RS232 port

struct termios oldtio,newtio;

//-----------------
// Initialize variables

int loop=0;
bool exit_while = false;

//-----------------
// Thread

pthread_t thread;

//-----------------
// Clear terminal

system("clear");

//-----------------
// Open connection to localhost:18944

igtl::ClientSocket::Pointer socket;
socket = igtl::ClientSocket::New();
int r = socket->ConnectToServer("localhost", 18944);

igtl::TransformMessage::Pointer transMsg;
transMsg = igtl::TransformMessage::New();
transMsg->SetDeviceName("NeuroEndoscope");


if (r != 0)
{
  std::cout << "Connect to the server\t\t\t\t\t\t\t[Failed]" << std::endl;
}
else
{
  std::cout << "Connect to the server\t\t\t\t\t\t[Succeed]" << std::endl;  

  if(usb_device < 0) 
  {
    printf("Open USB Device\t\t\t\t\t\t\t[Failed]\n");
  }
  else
  {

   //-----------------
   // Slicer connection and RS232 opened successfully

    printf("Open USB Device\t\t\t\t\t\t\t[Succeed]\n");

   //-----------------
   // Use termios to configure RS232 communication port
   //-----------------
   // 115200 bauds, 8 bits, 1 stop bit, no parity, no flow control

    tcgetattr(usb_device,&oldtio); /* save current port settings */
        
    newtio.c_cflag = B115200 | CS8;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
        
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
         
    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = RESPONSE_SIZE;   /* blocking read until RESPONSE_SIZE chars received */
        
    tcflush(usb_device, TCIFLUSH);
    tcsetattr(usb_device,TCSANOW,&newtio);


    //-----------------
    // Reset system

    write(usb_device,&reset,1);
    write(usb_device,&run,1);

    //-----------------
    // Slow down the data coming from the sensor (stream)
    // If too fast, data are not read correctly

    write(usb_device,&slow_stream_speed,1);

    //-----------------
    // Let some time to be sure the system has restarted (1s)
   
    usleep(1000000);

    //-----------------
    // Configure sensor to retrieve position and angle (matrix format)
    
    if (write(usb_device,&pos_matrix,1) != 1)
    {
      printf("Write on USB Device\t\t\t\t\t\t[Failed]\n");
    }
    else
    {
      printf("Write on USB Device\t\t\t\t\t\t[Succeed]\n");

      //-----------------
      // Used first sensor, and ask a stream of data (instead of asking data each time (point)) 
      // Choose between stream and point (below)    

      write(usb_device,&first_sensor,1);
      if(!strcmp(DATAFLOW,STREAM))
      {
      write(usb_device,&stream,1);
      }

      pthread_create(&thread, NULL, keybord_input, &exit_while);

      while(!exit_while)//loop <= 80)
      {

        //-----------------
        // Send command to retrieve some data
        // Choose between stream (above) and point
        if(!strcmp(DATAFLOW,POINT))
        { 
        write(usb_device,&point,1);
        }

        //-----------------
        // Read data

        int bytes_read = read(usb_device,buffer,RESPONSE_SIZE);
        if( bytes_read != RESPONSE_SIZE)
        {
          printf("Read on USB Device [%d]\t\t\t\t\t\t[Failed]\n",bytes_read);
        }
        else
        {
          
          //-----------------
          // Shift bits as describe in the documentation

          unpack(buffer,unpack_coordinates,RESPONSE_SIZE);
 
          //-----------------
          // Create OpenIGTLink matrix to send, with data

          igtl::Matrix4x4 matrix;
            
             //-----------------
             // Angles

          matrix[0][0] = (float)unpack_coordinates[3]*ANGLE_SCALE;
          matrix[1][0] = (float)unpack_coordinates[4]*ANGLE_SCALE;
          matrix[2][0] = (float)unpack_coordinates[5]*ANGLE_SCALE;
          matrix[3][0] = (float)0;

          matrix[0][1] = (float)unpack_coordinates[6]*ANGLE_SCALE;
          matrix[1][1] = (float)unpack_coordinates[7]*ANGLE_SCALE;
          matrix[2][1] = (float)unpack_coordinates[8]*ANGLE_SCALE;
          matrix[3][1] = (float)0;

          matrix[0][2] = (float)unpack_coordinates[9]*ANGLE_SCALE;
          matrix[1][2] = (float)unpack_coordinates[10]*ANGLE_SCALE;
          matrix[2][2] = (float)unpack_coordinates[11]*ANGLE_SCALE;
          matrix[3][2] = (float)0;

            //-----------------
            // Position

          matrix[0][3] = unpack_coordinates[0]*POS_SCALE*5;
          matrix[1][3] = unpack_coordinates[1]*POS_SCALE*5;
          matrix[2][3] = unpack_coordinates[2]*POS_SCALE*5;
          matrix[3][3] = 1;

          //-----------------
          // Print created matrix in the terminal

          igtl::PrintMatrix(matrix);

          //-----------------
          // Send Matrix

          transMsg->SetMatrix(matrix);
          transMsg->Pack();
          socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
          igtl::Sleep(100);

        } // else (Read successful)

        loop++;
      } // while

      pthread_cancel(thread);

    } // else (Write on device successful)


    //-----------------
    // Reset and stop 

    write(usb_device,&reset,1);
    write(usb_device,&sleep,1);

    //-----------------
    // Close connection

    socket->CloseSocket();
    tcsetattr(usb_device,TCSANOW,&oldtio);

    //-----------------
    // Close RS232 Communication (via USB)

    int closed_usb_device = close(usb_device);
    if(closed_usb_device == -1)
    {
      printf("Close USB Device\t\t\t\t\t\t[Failed]\n");
    }
    else
    {
      printf("Close USB Device\t\t\t\t\t\t[Succeed]\n");
    }

  } // else (Open device successful) 

} // else (Connect to Slicer successful )

printf("\n\n\n\n\n");

return 0;
}
