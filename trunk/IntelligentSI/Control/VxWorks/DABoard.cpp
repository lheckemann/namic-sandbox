/***************************************************************************
 * FileName      : DABoard.cpp
 * Created       : 2008/12/24
 * LastModified  : 2009/
 * Author        : hiroaki kozuka
 * Aim           : DA Driver for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "DABoard.h"
#include "BoardBaseSim.h"

using namespace std;
//-----------------------------------------------------------------------------
//
DABoard::DABoard(BoardBaseSim* ptr) {
 cout<<"Init DABoard..."<<flush;
 BBSim = ptr;
  for(int unitNum=0; unitNum<board_num; unitNum++){
    memset( &Volt[unitNum][0], 0, sizeof(double)*CH_NUM );
  }
  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
DABoard::~DABoard() {
  cout<<"End DABoard"<<endl;
}

//-----------------------------------------------------------------------------
//
void
DABoard::Set(int unitNum, int ch, double data) {
  Volt[unitNum][ch] = data;
}

//-----------------------------------------------------------------------------
//
void
DABoard::Write(int unitNum, BoardType type) {
  //cout<<"Volt: "<<flush;
  for(int ch=0; ch<CH_NUM; ch++){
    //cout<<Volt[unitNum][ch]<<" "<<flush;
    BBSim->SetVolt(ch, Volt[unitNum][ch]);
  }
  //cout<<endl;
}

//-----------------------------------------------------------------------------
//
int
DABoard::CHNum(){
  return CH_NUM;
}

//-----------------------------------------------------------------------------
//
int
DABoard::BoardNum(){
  return board_num;
}

//-----------------------------------------------------------------------------
//
double*
DABoard::GetDataPtr(int unitNum, int ch){
  return &Volt[unitNum][ch];
}


