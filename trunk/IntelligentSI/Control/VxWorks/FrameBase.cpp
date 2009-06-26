/*****************************************************************************
 * FileName      : FrameBase.cpp
 * Created       : 2008/11/2
 * LastModified  : 2009/5/
 * Author        : Hiroaki KOZUKA
 * Aim           : Frame base class for kinematics models
 ****************************************************************************/
#include "FrameBase.h"

void
FrameBase::FilterFirstOrderLag(Coord_6DoF& Pee, Coord_6DoF& Pee_1){
  const double dt = 0.001; // Simpling time [s]
  const double T  = 0.004; // [s]

  Pee.x = (dt*Pee.x + T*Pee_1.x)/(dt + T);
  Pee.y = (dt*Pee.y + T*Pee_1.y)/(dt + T);
  Pee.z = (dt*Pee.z + T*Pee_1.z)/(dt + T);
  Pee.alpha = (dt*Pee.alpha + T*Pee_1.alpha)/(dt + T);
  Pee.beta  = (dt*Pee.beta  + T*Pee_1.beta )/(dt + T);
  Pee.gamma = (dt*Pee.gamma + T*Pee_1.gamma)/(dt + T);

  Pee_1 = Pee;

}

void
FrameBase::FilterMovingAvrg(Coord_6DoF& Pee){
  static Coord_6DoF Pee_[4] = {{0,0,0,0,0,0},{0,0,0,0,0,0},
                               {0,0,0,0,0,0},{0,0,0,0,0,0}};
  MovingAvrg(4, Pee, Pee_ );
}

void
FrameBase::MovingAvrg(int num, Coord_6DoF& data, Coord_6DoF* data_){
  //
  for(int i=0;i<num;i++){
    data.x += data_[i].x;
    data.y += data_[i].y;
    data.z += data_[i].z;
    data.alpha += data_[i].alpha;
    data.beta  += data_[i].beta;
    data.gamma += data_[i].gamma;
  }

  //
  num++;
  data.x = data.x/((double)num);
  data.y = data.y/((double)num);
  data.z = data.z/((double)num);
  data.alpha = data.alpha/((double)num);
  data.beta  = data.beta/((double)num);
  data.gamma = data.gamma/((double)num);

  //
  for(int i=num-2; i>0; i--){
    data_[i] = data_[i-1];
  }
  data_[0] = data;

}




