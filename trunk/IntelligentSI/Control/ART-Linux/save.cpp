/***************************************************************************
 * FileName      : save.cpp
 * Created       : 2007/10/01
 * LastModified  : 2007/10/16
 * Author        : hiroaki KOZUKA
 * Aim           : Class for saving to data log
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "save.h"

#ifndef __linux__ //if not __linux__
int SAVE::fd;
#else //if __linux__
using namespace std; 
ofstream SAVE::ofs;
#endif //if not __linux__

SAVE::SAVE(int timeNum){
  dataNum = timeNum;
  dataVL = 2;
  data1 = new double[dataNum];
  data2 = new double[dataNum];
}

SAVE::~SAVE(){
    delete [] data1;
    delete [] data2;
}

void
SAVE::saveBuf(int time, double databuf1, double databuf2,
        double databuf3,double databuf4, double databuf5){
    for(int Num=0; Num<dataVL; Num++){
  switch(Num){
   case 0: data1[time] = databuf1; break;
   case 1: data2[time] = databuf2; break;
   //case 2: data3[time] = databuf3; break;
   //case 3: data4[time] = databuf4; break;
   //case 4: data5[time] = databuf5; break;
  }
    }
}

void
SAVE::savef(){
    int buff0;
    int dataStrgLg = 8 + dataVL*7;
    char   buff[ dataStrgLg ];
    
    //logMsg("Save start....\n",0,0,0,0,0,0);
    cout<<"Save start...."<<flush;
#ifndef __linux__ //if not __linux__
    fd = creat("CC_SYSTEM.txt",O_WRONLY);
#else //if __linux__
    ofs.open("CC_SYSTEM.txt");
#endif //if not __linux__

    char buffstrg[] = "time [s], destDeg [deg], curDeg [deg]\n";
#ifndef __linux__ //if not __linux__
    write(fd,buffstrg, sizeof(buffstrg));
#else //if __linux__
    ofs.write(buffstrg, sizeof(buffstrg));
#endif //if not __linux__

    //data saved start
    for(int i=0; i<dataNum; i++){
        //time save to buf
        buff0 = i;
        for (int ii = 0; ii<6; ii++){
            if(ii<3)
          buff[6-ii] = buff0%10+48;
            else if(ii>=3)
                buff[5-ii] = buff0%10+48;
            buff0 /= 10;
        }
        buff[3] = '.';
  
        //data save to buf
        for(int j=0; j<dataVL; j++){
            int ofset =  6 + j*7;
            buff[ofset+1] = ',';
            //data input
      switch(j){
      case 0:
    buff0 = (int)(data1[i]*10);
    break;
      case 1:
    buff0 = (int)(data2[i]*10);
    break;
      }

            //plus or minus
      if(buff0<0){
          buff0 = abs(buff0);
          buff[ ofset+2 ] = '-';
            }
            else
          buff[ ofset+2 ] = '+';
            //data
            for (int ii = 0; ii<4; ii++){
                if(ii < 1)
              buff[(ofset+7)-ii] = buff0%10 + 48;
                else
                    buff[(ofset+6)-ii] = buff0%10 + 48;
                buff0 /= 10;
            }
            buff[ ofset+6 ] = '.';
        }//end for
        buff[dataStrgLg-1] = '\n';
  
#ifndef __linux__ //if not __linux__
        write( fd, buff, sizeof(char)*dataStrgLg );
#else //if __linux__
  ofs.write(buff, dataStrgLg);
#endif //if not __linux__
    }//end for
#ifndef __linux__ //if not __linux__
    close(fd);
#else //if __linux__
    ofs.close();
#endif //if not __linux__
    //delete("CC_SYSTEM.txt");
    //logMsg("Save done.\n",0,0,0,0,0,0);
    cout<<"Save done."<<endl;
}

