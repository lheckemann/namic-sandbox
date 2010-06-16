#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>

#include <termios.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlClientSocket.h>

#define RESPONSE_SIZE 24
#define POS_SCALE 72/32767
#define ANGLE_SCALE 1/32767

#define STREAM "stream"
#define POINT  "point"

#define DATAFLOW POINT

int n_boards = 0; //Number of snapshots of the chessboard
int frame_step;   //Frames to be skipped
int board_w;      //Enclosed corners horizontally on the chessboard
int board_h;      //Enclosed corners vertically on the chessboard

igtl::Matrix4x4 matrix;


int retrieve_pos_ang_matrix();


//================================================================================

int opencv_camera_calibration()
{
        CvCapture* capture;

        printf("Enter the numbers of spanspots = ");
        scanf("%d",&n_boards);

        printf("\rEnter the numbers of frames to skip = ");
        scanf("%d",&frame_step);

        board_w  = 8;
        board_h  = 6;
  
        int board_total = board_w * board_h;                                //Total enclosed corners on the board
        CvSize board_sz = cvSize( board_w, board_h );

        capture = cvCreateCameraCapture( 0 );
       
        if(!capture) 
        {
                printf("\nCouldn't open the camera\n"); 
                return -1;
        }

        cvNamedWindow( "Snapshot" );
        cvNamedWindow( "Raw Video");

        //Allocate storage for the parameters according to total number of corners and number of snapshots
        CvMat* image_points      = cvCreateMat(n_boards*board_total,2,CV_32FC1);
        CvMat* object_points     = cvCreateMat(n_boards*board_total,3,CV_32FC1);
        CvMat* point_counts      = cvCreateMat(n_boards,1,CV_32SC1);
        CvMat* intrinsic_matrix  = cvCreateMat(3,3,CV_32FC1);
        CvMat* distortion_coeffs = cvCreateMat(4,1,CV_32FC1);

        CvMat* rotation_matrix   = cvCreateMat(3,3,CV_32FC1);
        CvMat* translation_vector= cvCreateMat(3,1,CV_32FC1);


        CV_MAT_ELEM(*rotation_matrix, float, 0,0) = matrix[0][0];
        CV_MAT_ELEM(*rotation_matrix, float, 1,0) = matrix[1][0];
        CV_MAT_ELEM(*rotation_matrix, float, 2,0) = matrix[2][0];
        
        CV_MAT_ELEM(*rotation_matrix, float, 0,1) = matrix[0][1];
        CV_MAT_ELEM(*rotation_matrix, float, 1,1) = matrix[1][1];
        CV_MAT_ELEM(*rotation_matrix, float, 2,1) = matrix[2][1];
   
        CV_MAT_ELEM(*rotation_matrix, float, 0,2) = matrix[0][2];
        CV_MAT_ELEM(*rotation_matrix, float, 1,2) = matrix[1][2];
        CV_MAT_ELEM(*rotation_matrix, float, 2,2) = matrix[2][2];


        CV_MAT_ELEM(*translation_vector, float, 0,0) = matrix[0][3];
        CV_MAT_ELEM(*translation_vector, float, 1,0) = matrix[1][3];
        CV_MAT_ELEM(*translation_vector, float, 2,0) = matrix[2][3]; 


        /*

        for(int i=0;i<n_boards;i++)
        {
          CV_MAT_ELEM(*rotation_matrix, float, i, 0) = matrix[0][0];
          CV_MAT_ELEM(*rotation_matrix, float, i, 1) = matrix[0][1];
          CV_MAT_ELEM(*rotation_matrix, float, i, 2) = matrix[0][2];
          CV_MAT_ELEM(*rotation_matrix, float, i, 3) = matrix[1][0];
          CV_MAT_ELEM(*rotation_matrix, float, i, 4) = matrix[1][1];
          CV_MAT_ELEM(*rotation_matrix, float, i, 5) = matrix[1][2];
          CV_MAT_ELEM(*rotation_matrix, float, i, 6) = matrix[2][0];
          CV_MAT_ELEM(*rotation_matrix, float, i, 7) = matrix[2][1];
          CV_MAT_ELEM(*rotation_matrix, float, i, 8) = matrix[2][2];

          CV_MAT_ELEM(*translation_vector, float, i, 0) = matrix[0][3];
          CV_MAT_ELEM(*translation_vector, float, i, 1) = matrix[1][3];
          CV_MAT_ELEM(*translation_vector, float, i, 2) = matrix[2][3];
         }    

         */

        //Note:
        //Intrinsic Matrix - 3x3                                              Lens Distorstion Matrix - 4x1
        //      [fx 0 cx]                                                      [k1 k2 p1 p2   k3(optional)]
        //      [0 fy cy]
        //      [0  0  1]


        CvPoint2D32f* corners = new CvPoint2D32f[ board_total ];
        int corner_count;
        int successes = 0;
        int step, frame = 0;

        IplImage *image = cvQueryFrame( capture );
        IplImage *gray_image = cvCreateImage(cvGetSize(image),8,1);                    //subpixel
 
  
        //Loop while successful captures equals total snapshots
        //Successful captures implies when all the enclosed corners are detected from a snapshot

        while(successes < n_boards)
        {
                if((frame++ % frame_step) == 0)                                                               //Skip frames
                {
                        //Find chessboard corners:
                        int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );

                        cvCvtColor(image, gray_image, CV_BGR2GRAY);                //Get Subpixel accuracy on those corners
                        cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11,11),cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

                        cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);                //Draw it
   
                        // If we got a good board, add it to our data
                        if( corner_count == board_total ) 
                        {
                         cvShowImage( "Snapshot", image );                 // show in color if we did collect the image
                                step = successes*board_total;
                                for( int i=step, j=0; j<board_total; ++i,++j ) {
                                CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
                                CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
                                CV_MAT_ELEM(*object_points,float,i,0) = (float) j/board_w;
                                CV_MAT_ELEM(*object_points,float,i,1) = (float) (j%board_w);
                                CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;
                        }
                        CV_MAT_ELEM(*point_counts, int,successes,0) = board_total;    
                        successes++;
                        printf("\r%d successful Snapshots out of %d collected.",successes,n_boards);
                        }
                        else
                        cvShowImage( "Snapshot", gray_image );                //Show Gray if we didn't collect the image
             } 
 
                //Handle pause/unpause and ESC
                int c = cvWaitKey(15);
                if(c == 'p')
                {  
                        c = 0;
                        while(c != 'p' && c != 27)
                        {
                              c = cvWaitKey(250);
                        }
                }
                if(c == 27)
                    return 0;

                image = cvQueryFrame( capture );                            //Get next image
                cvShowImage("Raw Video", image);
        } 

        //End WHILE loop with enough successful captures

        cvDestroyWindow("Snapshot");

        printf("\n\n *** Calbrating the camera now...\n");

        //Allocate matrices according to successful number of captures
        CvMat* object_points2  = cvCreateMat(successes*board_total,3,CV_32FC1);
        CvMat* image_points2   = cvCreateMat(successes*board_total,2,CV_32FC1);
        CvMat* point_counts2   = cvCreateMat(successes,1,CV_32SC1);

        //Tranfer the points to matrices
        for(int i = 0; i<successes*board_total; ++i)
        {
      CV_MAT_ELEM( *image_points2, float, i, 0)  =  CV_MAT_ELEM( *image_points, float, i, 0);
      CV_MAT_ELEM( *image_points2, float, i, 1)  =  CV_MAT_ELEM( *image_points, float, i, 1);
      CV_MAT_ELEM( *object_points2, float, i, 0) =  CV_MAT_ELEM( *object_points, float, i, 0) ;
      CV_MAT_ELEM( *object_points2, float, i, 1) =  CV_MAT_ELEM( *object_points, float, i, 1) ;
      CV_MAT_ELEM( *object_points2, float, i, 2) =  CV_MAT_ELEM( *object_points, float, i, 2) ;
        } 

        for(int i=0; i<successes; ++i)
        { 
            CV_MAT_ELEM( *point_counts2, int, i, 0) = CV_MAT_ELEM( *point_counts, int, i, 0);  //These are all the same number
        }
        cvReleaseMat(&object_points);
        cvReleaseMat(&image_points);
        cvReleaseMat(&point_counts);

        // Initialize the intrinsic matrix with both the two focal lengths in a ratio of 1.0

        CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0f;
        CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0f;

        //Calibrate the camera
        //_____________________________________________________________________________________
 
        cvCalibrateCamera2(object_points2, image_points2, point_counts2,  cvGetSize( image ), intrinsic_matrix, distortion_coeffs, NULL, NULL,0 );

        //CV_CALIB_FIX_ASPECT_RATIO
        //_____________________________________________________________________________________

        //Save values to file
        printf(" *** Calibration Done!\n\n");
        printf("Storing Intrinsics.xml and Distortions.xml files...\n");
        cvSave("Intrinsics.xml",intrinsic_matrix);
        cvSave("Distortion.xml",distortion_coeffs);
        printf("Files saved.\n\n");

        printf("Starting corrected display....");

        //Sample: load the matrices from the file
        CvMat *intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
        CvMat *distortion = (CvMat*)cvLoad("Distortion.xml");

        // Build the undistort map used for all subsequent frames.

        IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
        IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
        cvInitUndistortMap(intrinsic,distortion,mapx,mapy);

        // Run the camera to the screen, showing the raw and the undistorted image.

        //CvMat* projected_image = cvCreateMat(successes*board_total,2,CV_32FC1);
        //CvMat* rotation_vector = cvCreateMat(3,1,CV_32FC1);

        cvNamedWindow( "Undistort" );
        // cvNamedWindow( "My Test" );
        while(image) 
        {
                IplImage *t = cvCloneImage(image);
                cvShowImage( "Raw Video", image );                      // Show raw image
                cvRemap( t, image, mapx, mapy );                        // Undistort image
                cvReleaseImage(&t);
                cvShowImage("Undistort", image);                        // Show corrected image

                //cvRodrigues2(rotation_matrix,rotation_vector);

                //cvProjectPoints2(object_points2, rotation_vector, translation_vector, intrinsic, distortion, projected_image);

                //                cvShowImage("My Test", projected_image);
         

                //Handle pause/unpause and ESC
                int c = cvWaitKey(15);
                if(c == 'p')
                { 
                        c = 0;
                        while(c != 'p' && c != 27)
                        {
                                c = cvWaitKey(250);
                        }
                }
                if(c == 27)
                        break;

                image = cvQueryFrame( capture );
        } 



       return 0;
}


//===============================================================================

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

void* thread_pos_ang(void* data)
{
  retrieve_pos_ang_matrix();
}

//////////////////////////////////////////////////////////////////////////////

int retrieve_pos_ang_matrix()
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

// igtl::ClientSocket::Pointer socket;
// socket = igtl::ClientSocket::New();
// int r = socket->ConnectToServer("localhost", 18944);

// igtl::TransformMessage::Pointer transMsg;
// transMsg = igtl::TransformMessage::New();
// transMsg->SetDeviceName("NeuroEndoscope");


// if (r != 0)
// {
//   std::cout << "Connect to the server\t\t\t\t\t\t\t[Failed]" << std::endl;
// }
// else
// {
//   std::cout << "Connect to the server\t\t\t\t\t\t[Succeed]" << std::endl;  

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

          // igtl::PrintMatrix(matrix);

          //-----------------
          // Send Matrix

          // transMsg->SetMatrix(matrix);
          // transMsg->Pack();
          // socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
          // igtl::Sleep(100);
          usleep(100000);

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

    // socket->CloseSocket();
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

// } // else (Connect to Slicer successful )

printf("\n\n\n\n\n");

return 0;
}


int main() 
{
  pthread_t pos_ang_matrix_thread;

  pthread_create(&pos_ang_matrix_thread,NULL, thread_pos_ang,NULL);  

  opencv_camera_calibration();

  pthread_cancel(pos_ang_matrix_thread);

  return 0;
}


