/***************************************************************************
 * FileName      : main.cpp
 * Created       : 2007/09/10
 * LastModified  : 2007/09/20
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "main.h"

void Main(){
    slaveRobot  = new ROBOT();
  delete slaveRobot;
}
