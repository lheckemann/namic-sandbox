/***************************************************************************
 * FileName      : FrameForward.cpp
 * Created       : 2008/12/10
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : For Kinematics
 * OS            : VxWorks 5.5.1
 ***************************************************************************/

#include "FrameForward.h"

//-----------------------------------------------------------------------------
//
FrameForward::FrameForward(){
  this->Init();
}

//-----------------------------------------------------------------------------
//
FrameForward::~FrameForward(){

}

//-----------------------------------------------------------------------------
//
void
FrameForward::Init(){
  //const
  L = 159.0;
  l1 = 40.0;
  l2 = 40.0;
  l3 = 80.0;
  rho = PI/2;
  epsilon = 0;
  sigma=asin(sin(epsilon)/cos(rho/2));

  theta1 = theta2 = 0;
  ax = ay = az = nx = ox = 0;
  mu = eta = lambda = psi = 0;
  t1 = t2 = t4 = 0;
  x1 = y1 = z1 = 0;
  x2 = y2 = z2 = 0;
  a = b = c = 0;
  px = py = pz = 0;

  memset(&q[0], 0, sizeof(double)*4 );

}


//-----------------------------------------------------------------------------
//
void
FrameForward::Kinematics(int JNum, double* angle, Coord_6DoF& P ){

  q[0] = angle[0];
  q[1] = angle[1];
  q[2] = angle[2] + 300.0*PI/180.0;
  q[3] = 0.0; // not used

  t1=q[1];
  t2=atan((cos(rho)*sin(q[1])-cos(q[1])*tan(q[0]))/sin(rho));
  t4=q[3];

  ax=cos(rho/2.0)*cos(sigma)*sin(t2)+cos(t2)*(cos(sigma)*sin(t1)*sin(rho/2.0)+cos(t1)*sin(sigma));
  ay=-cos(t2)*cos(rho/2.0)*sin(t1)+sin(t2)*sin(rho/2.0);
  az=cos(t1)*cos(t2)*cos(sigma)-(cos(rho/2.0)*sin(t2)+cos(t2)*sin(t1)*sin(rho/2.0))*sin(sigma);
  nx=sin(t4)*(sin(t1)*sin(sigma)-cos(t1)*cos(sigma)*sin(rho/2.0))+cos(t4)*(cos(t2)*cos(sigma)*cos(rho/2.0)-sin(t2)*(cos(t1)*sin(sigma)+sin(t1)*cos(sigma)*sin(rho/2.0)));
  ox=cos(t4)*(sin(t1)*sin(sigma)-cos(t1)*cos(sigma)*sin(rho/2.0))-sin(t4)*(cos(t2)*cos(sigma)*cos(rho/2.0)-sin(t2)*(cos(t1)*sin(sigma)+sin(t1)*cos(sigma)*sin(rho/2.0)));

  mu=atan(ax/az);
  eta=atan(-ay/az);
  lambda=L-sqrt(sq(x2)+sq(y2)+sq(z2));
  psi=atan(-ox/nx);

  theta1=q[2];
  theta2=atan(tan(eta)*sin(theta1));

  x1=(l1+l2)*cos(theta1)*cos(theta2);
  y1=(l1+l2)*sin(theta2);
  z1=-(l1+l2)*cos(theta2)*sin(theta1);

  a=sq(tan(mu))+sq(tan(eta))+1;
  b=2*(-x1*tan(mu)+y1*tan(eta)-z1);
  c=sq(x1)+sq(y1)+sq(z1)-sq(l3);

  z2=(-b+sqrt(sq(b)-4*a*c))/(2*a);
  x2=z2*tan(mu);
  y2=-z2*tan(eta);

  t3=-(L-sqrt(sq(x2)+sq(y2)+sq(z2)));

  P.x = px = t3*(cos(rho/2)*cos(sigma)*sin(t2)+cos(t2)*(cos(sigma)*sin(t1)*sin(rho/2)+cos(t1)*sin(sigma)));
  P.y = py = t3*(-cos(t2)*cos(rho/2)*sin(t1)+sin(t2)*sin(rho/2));
  P.z = pz = t3*(cos(t1)*cos(t2)*cos(sigma)-(cos(rho/2)*sin(t2)+cos(t2)*sin(t1)*sin(rho/2))*sin(sigma));

/*
  if( GetTime()%1000 == 0 )
    cout<<theta1<<" "<<theta2<<" "<<x1<<" "<<y1<<" "<<z1<<" "<<a<<" "<<b<<" "<<c<<" "<<x2<<" "<<y2<<" "<<z2<<" "
        <<t3<<" a:"<<q[0]<<" "<<q[1]<<" "<<q[2]<<" p:"<<P.x<<" "<<P.y<<" "<<P.z<<endl;
*/

}




