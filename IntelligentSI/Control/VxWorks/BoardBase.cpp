/******************************************************************************
 * FileName      : BoardBase.cpp
 * Created       : 2008/12/2
 * LastModified  : 2008/12/
 * Author        : hiroaki KOZUKA
 * Aim           : board driver base class
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#include "BoardBase.h"

BoardBase::BoardBase(){

}

BoardBase::~BoardBase(){
}

void
BoardBase::Write(int unitNum){
}

void
BoardBase::Write(int unitNum, BoardType type){
}

void
BoardBase::Read(int unitNum){
}

void
BoardBase::Read(int unitNum, BoardType type){
}


