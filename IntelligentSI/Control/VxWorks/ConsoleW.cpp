/******************************************************************************
 * FileName      : Console.cpp
 * Created       : 2009/01/15
 * LastModified  :
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board consloe
 *****************************************************************************/
#include "ConsoleW.h"

//using namespace std;

#define ComOutput std::cout<<"<"<<ROBOT_NAME<<">: "<<std::flush

//-----------------------------------------------------------------------------
//
ConsoleW::ConsoleW(){
  std::cout<<"Console init..."<<std::flush;

  OrderNum = 7;
  Order = new ConsoleOrder<EVENT>[OrderNum];
  Order[0].SetOrder( E_CTRL_STOP_START,"istart", sizeof("istart"), "Start Control stop mode" );
  Order[1].SetOrder( E_PRGM_EXIT,      "exit",   sizeof("exit"),   "Exit program" );
  Order[2].SetOrder( E_CTRL_RUN_START, "cstart", sizeof("cstart"), "Start Control" );
  Order[3].SetOrder( E_INIT,           "init",   sizeof("init"),   "Initialize" );
  Order[4].SetOrder( E_CTRL_STOP_STOP, "istop",  sizeof("istop"),  "Stop stop Control" );
  Order[5].SetOrder( E_CTRL_RUN_STOP,  "cstop",  sizeof("cstop"),  "Stop Control" );
  Order[6].SetOrder( E_EMERGENCY_0,    "em",     sizeof("em"),     "Emargency mode" );
  //Order[7].SetLocalOrder( "ClearScreen",   "@cls",   sizeof("@cls"),   "clear screen" );
  //Order[8].SetLocalOrder( "DefultScreen",  "@dfs",   sizeof("@dfs"),   "defult screen" );
  //Order[9].SetLocalOrder( "DisplayData",   "@d",     sizeof("@d"),     "display data" );
  TF.Body()[5] = 10;
  std::cout<<"done."<<std::endl;
  tr2 = CreateThread( "task1", (unsigned int)this, (unsigned int)ThreadFunc, 3, NULL, NULL, 0, 0, 0, 0 );
//  tr  = CreateThread( "task2", 0, (unsigned int)ThreadFunc2, 5, NULL, NULL, 0, 0, 0, 0 );
}

//-----------------------------------------------------------------------------
//
ConsoleW::~ConsoleW(){
  delete [] Order;
  DeleteThread(tr2);
  //DeleteThread(tr);
  std::cout<<"End cosole."<<std::endl;
}

//-----------------------------------------------------------------------------
//
THRD_FUNC_RETURN
ConsoleW::ThreadFunc(void* ptr){
  ConsoleW* Com = (ConsoleW*)ptr;
  EVENT E_0 = E_PRGM_START;
  EVENT E_1 = E_PRGM_START;
  std::string buffer;

  do{
    ComOutput;
    getline( std::cin, buffer );
    E_1 = Com->keyHandler(E_0, buffer);
    if(E_1 != E_0 ){
    //  if( E_1 == "ClearScreen" || E_1 == "DefultScreen")
    //    Com->SetEvent_L(E_1);
    //  else{
        SetEvent(E_1);
    //  }
    }
    E_0 = E_1;

  }while( GetEvent() != E_PRGM_EXIT );
  return 0;
}

//-----------------------------------------------------------------------------
//
THRD_FUNC_RETURN
ConsoleW::ThreadFunc2(void* ptr){
  bool scrn = true;

  unsigned long long t = 0;
  do{
    /*if(Com->GetEvent_L() == "ClearScreen"){
      scrn = false;
    }
    else if(Com->GetEvent_L() == "DefultScreen"){
      scrn = true;
    }
    */
    t++;
    if(scrn){
        std::cout<<"t:"<<t<<std::endl;
        ComOutput;
    }

    TaskDelay(1000000);
  }while( GetEvent() != E_PRGM_EXIT );
  return 0;
}

//-----------------------------------------------------------------------------
//
EVENT
ConsoleW::keyHandler(EVENT flag, std::string& buffer){
  for(int i=0; i<OrderNum; i++){
    if( buffer == Order[i].Order() ){
      flag = Order[i].Event();
      ComOutput<<Order[i].Order()<<std::endl;
      break;
    }
    else if( buffer == "" ){
      break;
    }
    else if( buffer == "help" || buffer == "h" ){
      this->helpMessage();
      break;
    }
    else{
      if( i == (OrderNum-1) ){
        ComOutput<<"Undefined symbol: "<<buffer<<"\n";
        ComOutput<<"Type 'help' for command"<<std::endl;
      }
    }
  }

  return (flag);
}

//-----------------------------------------------------------------------------
//
int
ConsoleW::helpMessage(){
  std::cout << "\n";
  std::cout<< " ****************************************** \n";
  for(int i=0; i<OrderNum; i++ ){
    std::cout<<Order[i].Help();
  }
  std::cout<< " ****************************************** \n";
  std::cout<< std::endl;
  return 1;
}

