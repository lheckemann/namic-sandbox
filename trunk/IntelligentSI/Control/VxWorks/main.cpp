#include "main.h"

using namespace std;

void
Main(){
  InterfaceBase IFB;
  occo Oj;
  Oj.Init();
  IFB.Init();

  InterfaceMasterR IFMsR;
  InterfaceSlicer IFS;

  Robot* Rt = new Robot(4);
  ConsoleW Con;

  Rt->SetAngOrigin(0, 0.0);
  Rt->SetAngOrigin(1, 0.0);
  Rt->SetAngOrigin(2, 0.0);
  Rt->SetAngOrigin(3, 0.0);
  Rt->SetPosOrigin(0.0, 0.0, -20.4359, 0.0, 0.0, 0.0);

  Joint* J0 = new Joint( Rt->DrPtr(), 0);
  Joint* J1 = new Joint( Rt->DrPtr(), 1);
  Joint* J2 = new Joint( Rt->DrPtr(), 2);
  Joint* J3 = new Joint( Rt->DrPtr(), 3);

  FrameInverse FI;
  FrameForward FF;
  Motor Mt;
  Encoder Enc;

  ACP560 Da;
  ACP420 Cnt;

  Mt.SetDvcNum(4);
  Enc.SetDvcNum(4);

  Mt.SetBoardNum(1);
  Mt.SetBoard(0, &Da, 1);

  Enc.SetBoardNum(1);
  Enc.SetBoard(0, &Cnt, 1);

  Enc.InitParam( 0, 1, 1000, 50 );
  Enc.InitParam( 1, 1, 1000, 50 );
  Enc.InitParam( 2, -1, 1000, 50 );
  Enc.InitParam( 3, 1, 256, 1 );

  Mt.InitParam( 0, -1, 10, -10, 0.07143, 50 );
  Mt.InitParam( 1, -1, 10, -10, 0.07143, 50 );
  Mt.InitParam( 2, 1, 10, -10, 0.07143, 50 );
  Mt.InitParam( 3, -1, 10, -10, 0.08333, 10 );

  Mt.Connect(0, Da.GetDataPtr(1,0) );
  Mt.Connect(1, Da.GetDataPtr(1,1) );
  Mt.Connect(2, Da.GetDataPtr(1,2) );
  Mt.Connect(3, Da.GetDataPtr(1,3) );

  Enc.Connect(0, Cnt.GetCntDataPtr(1,0) );
  Enc.Connect(1, Cnt.GetCntDataPtr(1,1) );
  Enc.Connect(2, Cnt.GetCntDataPtr(1,2) );
  Enc.Connect(3, Cnt.GetCntDataPtr(1,3) );


  Rt->DrPtr()->SetDeviceNum(Driver::ANGLE_SENSOR, 1);
  Rt->DrPtr()->SetDevice(Driver::ANGLE_SENSOR, 0, &Enc);

  Rt->DrPtr()->SetDeviceNum(Driver::ACTUATOR, 1);
  Rt->DrPtr()->SetDevice(Driver::ACTUATOR, 0, &Mt);

  Rt->DrPtr()->SetDID(0, Driver::ANGLE_SENSOR, 0, 0);
  Rt->DrPtr()->SetDID(1, Driver::ANGLE_SENSOR, 0, 1);
  Rt->DrPtr()->SetDID(2, Driver::ANGLE_SENSOR, 0, 2);
  Rt->DrPtr()->SetDID(3, Driver::ANGLE_SENSOR, 0, 3);

  Rt->DrPtr()->SetDID(0, Driver::ACTUATOR, 0, 0);
  Rt->DrPtr()->SetDID(1, Driver::ACTUATOR, 0, 1);
  Rt->DrPtr()->SetDID(2, Driver::ACTUATOR, 0, 2);
  Rt->DrPtr()->SetDID(3, Driver::ACTUATOR, 0, 3);

  J0->SetCtrlParam( 60, 0, 0.001 );
  J1->SetCtrlParam( 60, 0, 0.001 );
  J2->SetCtrlParam( 60, 0, 0.001 );
  J3->SetCtrlParam( 1, 0.03, 0.01 );

  Rt->SetJoint(0, J0);
  Rt->SetJoint(1, J1);
  Rt->SetJoint(2, J2);
  Rt->SetJoint(3, J3);

  Rt->SetFrameInv(&FI);
  Rt->SetFrameFor(&FF);

  taskSpawn("tRobotMain", 91, VX_FP_TASK, 100000,
            (FUNCPTR)Rt->RobotMain, (int)Rt,0,0,0,0,0,0,0,0,0);

  do{
    Oj.TaskDelay(1000000);
  }while(Oj.GetEvent() != E_PRGM_EXIT);

  taskDelete( taskNameToId("tRobotMain"));

  delete Rt;
  IFB.Delete();

  delete J3;
  delete J2;
  delete J1;
  delete J0;

  Oj.Delete();

  fReader();
}

