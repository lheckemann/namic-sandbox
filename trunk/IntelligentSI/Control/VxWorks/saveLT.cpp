/***************************************************************************
 * FileName      : save.cpp
 * Created       : 2007/10/01
 * LastModified  : 2007/10/16
 * Author        : hiroaki KOZUKA
 * Aim           : save class for data log
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "saveLT.h"

int SAVE::fd;

unsigned long long SAVE::iii=0;
unsigned long long SAVE::T=0;


SAVE::SAVE(int timeNum, double sample, int num, int saveTime){
  cout<<"Init save ..."<<endl;

  dataVL = num;

  dataNum = (unsigned long long)((double)timeNum*1000.0*(1/(sample*1000.0)));

  data = new float*[dataNum];;

  for(unsigned long long num=0; num<dataNum; num++){
    data[num] = new float[dataVL+1];
    memset( data[num], 0, sizeof(float)*(dataVL+1) );
  }

  Ts = (unsigned long long)(sample*1000);

  memset(buff, 0, 2048);

  SaveNum = (unsigned long long)((double)saveTime*1000.0*(1.0/(sample*1000.0)));

  IFMsgQID = CreateMsgQ( 200, sizeof(M_BUFF) );
  cout<<"save:"<<(int)IFMsgQID<<endl;

  char temp[] = "t[s], Px[mm], Py, Pz, destAg0[deg], dAg1, dAg2, cAg0[deg], cAg1, cAg2";
  this->Openf( temp, sizeof(temp) );

  taskSpawn( "tSaveTask", 95, VX_FP_TASK, 50000, (FUNCPTR)savef,
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
    data[(*time2)][0] = (float)(*time1)/1000.0;
    data[(*time2)][num+1] = *databuf;
  }
}


void
SAVE::Openf(char buffstrg[], int size){
  // fd = creat("occs.txt",O_WRONLY);
  fdb = fopen("occs.txt", "wb"  ); // file open on binary mode
  if(fdb == NULL)
    cout<<"file open error."<<endl;

  unsigned long long temp[3];

  temp[0] = (unsigned long long)dataVL+1;
  temp[1] = (unsigned long long)dataNum;
  temp[2] = (unsigned long long)size;
  fwrite(temp, sizeof(unsigned long long), 3, fdb );

  fwrite(buffstrg, sizeof(char), size, fdb );
  //write(fd,buffstrg, size );
}

void
SAVE::savef(void* thisClass){
  SAVE* S = (SAVE*)thisClass;

  EE_POSITION_LT_N_B Buff_N_P;

  M_BUFF Buff_m;
  memset(&Buff_m, 0, sizeof(M_BUFF) );

  int Mnum=0;
  unsigned long long dataNum = 0;

  do{
    Mnum = S->GetMsgQNum( S->IFMsgQID );

    while(Mnum){
      // Receive msg
      S->ReceiveMsgQ( S->IFMsgQID, (char*)&Buff_m, sizeof(M_BUFF) );

      if(Buff_m.header.Type == C_POSITION){
        memcpy(&Buff_N_P, &Buff_m.body[0], sizeof(EE_POSITION_N_B) );
        S->saveBuf( &Buff_m.header.Time, &dataNum, 0, (float*)&Buff_N_P.Time );
        S->saveBuf( &Buff_m.header.Time, &dataNum, 1, &Buff_N_P.x);
        S->saveBuf( &Buff_m.header.Time, &dataNum, 2, &Buff_N_P.y);
        S->saveBuf( &Buff_m.header.Time, &dataNum, 3, &Buff_N_P.z);
        dataNum++;
      }
      Mnum--;
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
      cout<<"file write"<<endl;
    }

  }while(true);
}


