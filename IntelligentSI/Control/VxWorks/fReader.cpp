/*
 * For reading binary data file
 *
 *
 *
 */

#include "fReader.h"

void
fReader(){
  FILE* fdb = fopen("occs.txt", "rb" ); // file open on binary mode
  FILE* fda = fopen("occsA_0.txt", "w" ); // file open on ascii mode

  float data[DATA_TYPE_NUM] = {0};
  char buff[DATA_BUFF_SIZE] = {0};
  unsigned long long temp[3]={0};
  size_t fNum=0;

  if(fdb == NULL )
    cout<<"Binary file open error."<<endl;

  if(fda == NULL )
    cout<<"ASCII file open error."<<endl;

  // Read header data
  fNum = fread(&temp, sizeof(unsigned long long ), 3, fdb );

  if(fNum < 3){
    cout<<"File error:1."<<endl;
    fclose(fdb);
    fclose(fda);
    return; // EOF
  }

  const unsigned int dataTypeNum = temp[0];

  if( DATA_TYPE_NUM < dataTypeNum ){
    cout<<"File error:2."<<endl;
    fclose(fdb);
    fclose(fda);
    return;
  }

  if( DATA_BUFF_SIZE < temp[2]){
    cout<<"File error:3."<<endl;
    fclose(fdb);
    fclose(fda);
    return;
  }

  fNum = fread(buff, sizeof(char), temp[2], fdb );

  if(fNum < temp[2]){
    cout<<"File error:4."<<endl;
    fclose(fdb);
    fclose(fda);
    return; // EOF
  }

  buff[ temp[2] ] = '\n';
  fwrite(buff, sizeof(char), temp[2]+1, fda);
  memset(buff, 0, DATA_BUFF_SIZE );

  cout<<"TypeNum:"<<temp[0]<<endl;

  int num=0;
  char* ptr;

  for( unsigned long long i=0; ; i++){
    num = 0;

    // read data
    fNum = fread(&data, sizeof(float), dataTypeNum, fdb );

    if( ferror(fdb)){
      cout<<"File error:"<<i+5<<" fNum:"<<fNum<<"."<<endl;
      break; // EOF
    }
    else if(feof(fdb)){
      cout<<"EOF: "<<i<<endl;
      break;
    }
    else if( fNum <= dataTypeNum && fNum>0 ){
      //data save to buf
      for(unsigned int j=0; j<fNum; j++){
        ptr = (char*)(buff+num);
        num += sprintf(ptr, "%f,", data[j] );

        if(num>= (DATA_BUFF_SIZE-5) ){
          cout<<"buff error."<<endl;
          break;
        }
      }
    }
/*    else{
      //data save to buf
      for(unsigned int j=0; j<dataTypeNum; j++){
        ptr = (char*)(buff+num);

        if(j == (dataTypeNum-1) )
          num += sprintf(ptr, "%f", data[j] );
        else
          num += sprintf(ptr, "%f,", data[j] );


        if(num>= (DATA_BUFF_SIZE-5) ){
          cout<<"buff error."<<endl;
          break;
        }
      }
    }
*/
    // return
    ptr = (char*)(buff+num);
    num += sprintf(ptr, "\n");

    //write to file
    if( num < DATA_BUFF_SIZE ){
      fwrite( buff, sizeof(char), num, fda );
    }

    // New file open
    if( i>0 && i%FILE_MAX_LINE_NUM == 0){
      fclose(fda);
      static int fdaNum = 1;
      char NextName[20]={0};
      sprintf(NextName, "occsA_%d.txt", fdaNum);
      fda = fopen( NextName, "w" );
      fdaNum++;
    }

  }

  // file close
  fclose(fdb);
  fclose(fda);
  cout<<"End fReader."<<endl;
}

