// Robot Simulator

#include "BRPtprOpenTracker.h"
#include "ctfControlBase.h"

#include <stdio.h>
#include "BRPplatform.h" // pthread, pipe
#include <time.h>


int main(int argc, char *argv[])
{
 // Initialize the OpenTracker module
 BRPtprOpenTracker OpenTracker;
 
 // Start the OpenTracker threads
 OpenTracker.Initialize();

 printf("OpenTracker initialized\n");
 
 // Initialize the control object
 ctfControlBase RobotControl(8); // robot with 8 joints and 16 valves
 RobotControl.SetOpenTracker(&OpenTracker);

 printf("RobotControl initialized\n");

 float position[3] = {3,2,1};
 float orientation[4] = {7,6,5,4};
 float depthv[3] = {9,8,7};
 RobotControl.SaveActualRobotPosition(position, orientation, depthv);

 printf("Waiting for commands\n");

 // Start polling for commands
 while (1) {
 
   //MoveRobot();
   //RobotControl.SetCurrentCoordinates(p,o);

   while (OpenTracker.IsThereNewCommand()) {
     //printf("New command received!\n");
     OpenTracker.ProcessNextCommand(&RobotControl);
   }

   //pthread_yield();
 //  msleep(10);

 }
 
}
