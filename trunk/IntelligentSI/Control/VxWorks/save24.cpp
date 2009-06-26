/***************************************************************************
 * FileName      : save.cpp
 * Created       : 2007/10/01
 * LastModified  : 2007/10/16
 * Author        : hiroaki KOZUKA
 * Aim           : save class for data log
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "save24.h"

unsigned long long SAVE::iii=0;
unsigned long long SAVE::T=0;


SAVE::SAVE(int timeNum, double sample, int num, int saveTime){
  cout<<"Init save ..."<<endl;

  dataVL = 15;

//  dataNum = (unsigned long long)((double)timeNum*(1/(sample*1000.0)));

  dataNum = 100;

  data = new float*[dataNum];

  for(unsigned long long num=0; num<dataNum; num++){
    data[num] = new float[dataVL+1];
    memset( data[num], 0, sizeof(float)*(dataVL+1) );
  }

  Ts = (unsigned long long)(sample*1000);

  memset(buff, 0, 2048);

//  SaveNum = (unsigned long long)((double)saveTime*(1.0/(sample*1000.0)));
  SaveNum = 50;

  InterfaceDataBaseSet IFDBS;
  IFMsgQID = CreateMsgQ( 200, sizeof(BUFF_M) );
  SetSMsgQID( IFDBS.GetSDType("POSITION"), 50, sizeof(BUFF_M), 500, IFMsgQID);
  SetSMsgQID( IFDBS.GetSDType("C_POSITION"), 50, sizeof(BUFF_M), 500, IFMsgQID);
  SetSMsgQID( IFDBS.GetSDType("DEST_ANGLES"), 50, sizeof(BUFF_M), 500, IFMsgQID);
  SetSMsgQID( IFDBS.GetSDType("CUR_ANGLES"), 50, sizeof(BUFF_M), 500, IFMsgQID);
  SetSMsgQID( IFDBS.GetSDType("TIMESTAMP"), 50, sizeof(BUFF_M), 500, IFMsgQID);
  SetSMsgQID( IFDBS.GetSDType("TIME_TRNSCT"), 50, sizeof(BUFF_M), 500, IFMsgQID);

  cout<<"save:"<<(int)IFMsgQID<<endl;

  char temp[] = "t[s], destPx[mm], Py, Pz, curPx, Py, Pz, destAg0[deg], dAg1, dAg2, cAg0[deg], cAg1, cAg2, stamp[ms], IFL_Time[ns], MainL_Time[ns]";

  this->Openf( temp, sizeof(temp) );

  taskSpawn( "tSaveTask", 92, VX_FP_TASK, 50000, (FUNCPTR)savef,
             (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0 );

  cout<<"save done. dataNum:"<<dataNum<<endl;
}

SAVE::~SAVE(){

  taskDelete( taskNameToId("tSaveTask") );
  DeleteMsgQ(IFMsgQID);

  fclose(fdb);

  for(int i=0;i<dataVL;i++)
    delete [] data[i];
  delete [] data;
  iii=0;
  T=0;

  cout<<"Save done."<<endl;
}

//-----------------------------------------------------------------------------
//
InterfaceBase::IF_MSG_Q_ID
SAVE::GetMsgQID(){
  return IFMsgQID;
}

void
SAVE::saveBuf( const unsigned long long* time1, const unsigned long long* time2,
               int num, const float* databuf ){
  if( num>dataVL ){
    return;
  }
  else if((*time2) >= dataNum ){
    return;
  }
  else{
    data[(*time2)][0] = (float)(*time1);
    data[(*time2)][num+1] = *databuf;
  }
}


void
SAVE::Openf(char buffstrg[], int size){

  fdb = fopen("occs.txt", "wb"  ); // file open on binary mode
  if(fdb == NULL)
    cout<<"file open error."<<endl;

  unsigned long long temp[3];
  temp[0] = (unsigned long long)dataVL+1;
  temp[1] = (unsigned long long)dataNum;
  temp[2] = (unsigned long long)size;

  fwrite(temp, sizeof(unsigned long long), 3, fdb );

  fwrite(buffstrg, sizeof(char), size, fdb );
}

void
SAVE::savef(void* thisClass){
  SAVE* S = (SAVE*)thisClass;
  InterfaceDataBaseSet IFDBS;

  EE_POSITION_N Buff_N_P;
  EE_POSITION_N Buff_N_cP;
  float angle[8]={0};
  unsigned long long stamp[2]={0};
  float stmp=0;

  BUFF_M Buff_m;
  memset(&Buff_m, 0, sizeof(BUFF_M) );

  unsigned long long dataNum = 0;
  unsigned long long T[6]={10, 11, 12, 13,14,15};

  double time = 0;
  double time_1 = 0;

  do{
    S->ReceiveMsgQ( S->IFMsgQID, (char*)&Buff_m, sizeof(BUFF_M) );
      if(Buff_m.Header.Type == IFDBS.GetSDType("POSITION")){
        memcpy(&Buff_N_P, &Buff_m.Body[0], sizeof(EE_POSITION_N) );
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 0, &Buff_N_P.x);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 1, &Buff_N_P.y);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 2, &Buff_N_P.z);
        T[0] = Buff_m.Header.Time;
      }
      else if(Buff_m.Header.Type == IFDBS.GetSDType("C_POSITION")){
        memcpy(&Buff_N_cP, &Buff_m.Body[0], sizeof(EE_POSITION_N) );
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 3, &Buff_N_cP.x);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 4, &Buff_N_cP.y);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 5, &Buff_N_cP.z);
        T[1] = Buff_m.Header.Time;
      }
      else if(Buff_m.Header.Type == IFDBS.GetSDType("DEST_ANGLES")){
        memcpy(&angle[0], &Buff_m.Body[0], sizeof(float)*3 );
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 6, &angle[0]);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 7, &angle[1]);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 8, &angle[2]);
        T[2] = Buff_m.Header.Time;
      }
      else if(Buff_m.Header.Type == IFDBS.GetSDType("CUR_ANGLES")){
        memcpy(&angle[3], &Buff_m.Body[0], sizeof(float)*3 );
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 9, &angle[3]);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 10, &angle[4]);
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 11, &angle[5]);
        T[3] = Buff_m.Header.Time;
      }
      /*
      else if(Buff_m.Header.Type == IFDBS.GetSDType("TIMESTAMP") ){
        memcpy(&stamp, &Buff_m.Body, sizeof(unsigned long long)*2 );
        stmp = (float)stamp[0];
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 12, &stmp);
        stmp = (float)stamp[1];
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 13, &stmp);
        T[4] = Buff_m.Header.Time;
      }
      else if(Buff_m.Header.Type == IFDBS.GetSDType("TIME_TRNSCT") ){
        memcpy(&stmp, &Buff_m.Body, sizeof(float) );
        S->saveBuf( &Buff_m.Header.Time, &dataNum, 14, &stmp);
        T[5] = Buff_m.Header.Time;
      }
      */
      if( T[0] == T[1] && T[0] == T[2] &&T[0]==T[3] ){
        dataNum++;
      }

    if( dataNum >= S->SaveNum ){
      //data save start
      int dNum=0;
      for(unsigned long long count=0; dataNum>0; dataNum--, count++){
        dNum = fwrite( S->data[count], sizeof(float), (S->dataVL+1), S->fdb);
        if(dNum!= (S->dataVL+1) ){
          cout<<"File error."<<endl;
        }
      }

    }

  }while(true);
}

//
double
SAVE::TimeGet(){
  UINT32 tbu, tbl;
  vxTimeBaseGet(&tbu, &tbl);
  return (tbu * 4294967296.0 + tbl)/25000000.0;
}
