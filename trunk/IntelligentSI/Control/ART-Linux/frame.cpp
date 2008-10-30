/***************************************************************************
 * FileName      : frame.cpp
 * Created       : 2007/12/17
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : Computed inverse/kinematics
 * OS            : VxWorks 5.5.1
 ***************************************************************************/

#include "frame.h"

#define sq(x) ((x)*(x))

FRAME::FRAME(double rad_deg){
  
  if(rad_deg == 360){
    UNIT_ANGLE = 180.0/PI;
  }
  else if(rad_deg == PI){
    UNIT_ANGLE = 1;
  }
  else{
    UNIT_ANGLE = 0;
  }
  jNum = 4;
  //OriginPee = *Pee_origin;

  angleInfo = new ANGLE_INFO[jNum];

  angleInfo[0].Init(45,-45, 10.1559);
  angleInfo[1].Init(45,-45, -10.1559);
  angleInfo[2].Init(45,-45, 86.896006);//75.8781);//100.952);
  angleInfo[3].Init( 0, 0, 0);
  
  angleBef[0] = 0;
  angleBef[1] = 0;
  angleBef[2] = 0;
  angleBef[3] = 0;
  
  
  limitPeeMax.x =  20;
  limitPeeMin.x = -20;
  limitPeeMax.y =  20;
  limitPeeMin.y = -20;
  limitPeeMax.z = -60;
  limitPeeMin.z = -110;
  
  limitPeeMax.alpha = 0;
  limitPeeMin.alpha = 0;
  limitPeeMax.beta  = 0;
  limitPeeMin.beta  = 0;
  limitPeeMax.gamma = 0;
  limitPeeMin.gamma = 0;
}

FRAME::~FRAME(){

}


const TRANSFORM*
FRAME::getTransform(){
  return 0;
}

const double*
FRAME::getAngles(){
  return 0;//q_inv;
}

void
FRAME::limitPosition(EE_POSITION* Pee){

}

void
FRAME::calculateKinematics( const double *angle, EE_POSITION *Pee){
  /*
  if( jNum != sizeof(angle)/sizeof(double) )  return;
  
  t1 = q[1]/UNIT_ANGLE;
  t2 = atan2( cos(rho)*sin(t1)-cos(t1)*tan(q[0]/UNIT_ANGLE), sin(rho) );
  t3 = q[2];
  //q[2] = sqrt( sq(destPee.x) + sq(destPee.y) + sq(destPee.z) );
  
  Pee->x = T[0][3] = -t3*( cos(t2) * ( cos(t1)*sin(sigma) + cos(sigma)*sin(rho/2)*sin(t1) )
             + cos(rho/2)*cos(sigma)*sin(t2) );
  
  Pee->y = T[1][3] = -t3*( sin(rho/2)*sin(t2) - cos(rho/2)*cos(t2)*sin( t1) );
  
  Pee->z = T[2][3] = -t3*( cos(t2) * ( cos(sigma)*cos(t1) - sin(rho/2)*sin(sigma)*sin(t1) )
             - cos(rho/2)*sin(sigma)*sin(t2) );
  T[3][3] = 1;
  */
}

int
FRAME::calculateInvKinematics(const EE_POSITION *Pee, double *angle){

  double T[4][4] = {{1, 0, 0, Pee->x},
          {0, 1, 0, Pee->y},
          {0, 0, 1, Pee->z},
          {0, 0, 0, 1}};
  
  //limit position
  if(T[0][3] > limitPeeMax.x)
    T[0][3] = limitPeeMax.x;
  else if(T[0][3] < limitPeeMin.x)
    T[0][3] = limitPeeMin.x;
  
  if(T[1][3] > limitPeeMax.y)
    T[1][3] = limitPeeMax.y;
  else if(T[1][3] < limitPeeMin.y)
    T[1][3] = limitPeeMin.y;
  
  if(T[2][3]> limitPeeMax.z)
    T[2][3] = limitPeeMax.z;
  else if(T[2][3] < limitPeeMin.z)
    T[2][3] = limitPeeMin.z;
  
  double epsilon = 10*PI/180;
  double rho = 90*PI/180;
  
  double L = 180.5, L0 = 88.819, L1 = 200, L2 = 120;
  
  double q1,q2,q3,q4;
  double t1,t2,t3,t4;
  double eta,lambda, mu, sigma, psi;
  double A,B;
  
  sigma = asin( sin(epsilon)/cos(rho/2) );
  eta = atan( -T[1][3]/T[2][3] );//acos( z/sqrt(sq(y)+sq(z)) );
  mu  = atan( T[0][3]/T[2][3] );//acos( z/sqrt(sq(x)+sq(z)) );
  lambda = sqrt( sq(T[0][3]) + sq(T[1][3]) + sq(T[2][3]) );
  psi = atan2( -T[0][1], T[0][0] );
  
  double L3 = L - lambda;
  
  t1 = atan2( (tan(eta)*cos(rho/2) - sin(sigma)*sin(rho/2) + tan(mu)*cos(sigma)*cos(rho/2)),
          (tan(mu)*sin(sigma) + cos(sigma)) );
  
  t2 = atan2( (cos(t1)*(-tan(eta)*sin(rho/2) - sin(sigma)*cos(rho/2) + tan(mu)*cos(sigma)*cos(rho/2) )),
          (tan(mu)*sin(sigma) + cos(sigma)) );

  t3 = -(-4*L1*L3*cos(mu+sigma)+sqrt(16*sq(L1)*sq(L3)*sq(cos(mu+sigma))-4*(sq(L0-L1)-sq(L2)+sq(L3)+2*(-L0+L1)*
     L3*sin(mu+sigma))*(sq(L0+L1)-sq(L2)+sq(L3)-2*(L0+L1)*L3*sin(mu+sigma))))/
    (2*(sq(L0-L1)-sq(L2)+sq(L3)+2*(-L0+L1)*L3*sin(mu+sigma)));
  
  A = -sin(sigma)*cos(t1)*sin(t2) + cos(sigma)*( cos(rho/2)*cos(t2) - sin(rho/2)*sin(t1)*sin(t2) );
  B = sin(sigma)*sin(t1)-cos(sigma)*sin(rho/2)*cos(t1);
  
  t4 = atan2( B + A*tan(psi), A - B*tan(psi));
  
  q2 = t1;
  q1 = atan2( -sin(rho)*tan(t2) + cos(rho)*sin(q2), cos(q2) );
  q3 = 2*atan(t3);
  q4 = t4;
  
  angle[0] = 180*q1/PI - angleInfo[0].Offset;
  angle[1] = 180*q2/PI - angleInfo[1].Offset;
  angle[2] = 180*q3/PI - angleInfo[2].Offset;
  angle[3] = 180*q4/PI - angleInfo[3].Offset;
  
  //angle limit
  for(int i=0; i<4; i++){
    if(angle[i] > angleInfo[i].limitMax){
      angle[i] = angleInfo[i].limitMax;
    }
    else if(angle[i] < angleInfo[i].limitMin){
      angle[i] = angleInfo[i].limitMin;
    }
    else if( (angle[i] <= angleInfo[i].limitMax) && (angle[i] >= angleInfo[i].limitMin) ){
      //no operate (TODO: means no change to angle[i], right?)
    }
    else{
      angle[i] = angleBef[i];
    }
    angleBef[i] = angle[i];
  }
  
  return 0;
}

