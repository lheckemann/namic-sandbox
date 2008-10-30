/***************************************************************************
 * FileName      : main.cpp
 * Created       : 2007/09/10
 * LastModified  : 2007/09/20
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "main.h"

#ifndef __linux__ 
void Main(){
#else // __linux__
int main(){
#endif // not __linux__

    slaveRobot  = new ROBOT();
    delete slaveRobot;

#ifdef __linux__ 
    return 0;
#endif // __linux__
}

