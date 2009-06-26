/***************************************************************************
 * FileName      : FrameInverse.cpp
 * Created       : 2008/12/10
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : For inverse kinematics
 * OS            : VxWorks 5.5.1
 ***************************************************************************/

#include "FrameInverse.h"

//-----------------------------------------------------------------------------
//
FrameInverse::FrameInverse(){
  this->Init();
}

//-----------------------------------------------------------------------------
//
FrameInverse::~FrameInverse(){

}

//-----------------------------------------------------------------------------
//
void
FrameInverse::Init(){
  //const
  L = 159;
  l1 = 40;
  l2 = 40;
  l3 = 80;
  rho = PI/2;
  epsilon = 0;
  sigma=asin(sin(epsilon)/cos(rho/2));

  px = py = pz = 0;
  mu = eta = lambda = psi = 0;
  t1 = t2 = t4 = 0;
  alpha = beta = 0;
  x2 = y2 = z2=0;
  kappa1 = kappa2 = 0;
  a = b = c = d = e = 0;
  x1 = y1 = z1 = 0;

  memset(&q[0], 0, sizeof(double)*4 );

}


//-----------------------------------------------------------------------------
//
double
FrameInverse::getAngle(int i){
  return  (180*q[i]/PI);
}




/*
//-----------------------------------------------------------------------------
//
void
FrameForward::limitAngle(double* angle){

}
*/

//-----------------------------------------------------------------------------
//
void
FrameInverse::InvKinematics(Coord_6DoF &P, int JNum, double* angle ){
  px = P.x;
  py = P.y;
  pz = P.z;
  psi = P.gamma*PI/180;

  eta=atan(-py/pz);
  mu=atan(px/pz);
  lambda=sqrt(sq(px)+sq(py)+sq(pz));

  t1=atan((tan(eta)*cos(rho/2)-sin(sigma)*sin(rho/2)+tan(mu)*cos(sigma)*sin(rho/2))/(tan(mu)*sin(sigma)+cos(sigma)));
  t2=atan((cos(t1)*(-tan(eta)*sin(rho/2)-sin(sigma)*cos(rho/2)+tan(mu)*cos(sigma)*cos(rho/2)))/(tan(mu)*sin(sigma)+cos(sigma)));

  alpha=acos((sq(l1+l2)+sq(L-lambda)-sq(l3))/(2*(l1+l2)*(L-lambda)));

  x2=(-px*(L-lambda))/(lambda);
  y2=(-py*(L-lambda))/(lambda);
  z2=(-pz*(L-lambda))/(lambda);

  kappa1=(l1+l2)*(L-lambda)*cos(alpha);
  kappa2=y2*tan(eta)-z2;

  a=1+sq((x2*tan(eta))/kappa2)+sq(x2/kappa2);
  b=(2*x2/kappa2)*(-kappa2+(y2-(kappa1*tan(eta))/kappa2)*tan(eta)-(z2+kappa1/kappa2));
  c=sq(x2)+sq(y2-(kappa1*tan(eta))/kappa2)+sq(z2+kappa1/kappa2)-sq(l3);
  d=-sin(sigma)*cos(t1)*sin(t2)+cos(sigma)*(cos(rho/2)*cos(t2)-sin(rho/2)*sin(t1)*sin(t2));
  e=sin(sigma)*sin(t1)-cos(sigma)*sin(rho/2)*cos(t1);

  t4 = psi;//atan2( (e+d*tan(psi)), (d-e*tan(psi)) );

  x1=(-b+sqrt(sq(b)-4*a*c))/(2*a);
  y1=-((x1*x2-kappa1)/kappa2)*tan(eta);
  z1=(x1*x2-kappa1)/kappa2;

  if(x1<0){
    q[2] = -( PI - asin(z1/sqrt(sq(x1)+sq(z1))) ) + PI*60/180;  // x1<0
  }
  else{
    q[2] = -asin( z1/sqrt(sq(x1)+sq(z1)) ) + PI*60/180;      // x1>=0
  }

  q[0]=atan((cos(rho)*sin(t1)-sin(rho)*tan(t2))/cos(t1));
  q[1]=t1;
  // q[2]= -asin( z1/sqrt(sq(x1)+sq(z1)) ) + PI*46.3/180;
  q[3]=t4;

//  cout<<"a"<<a<<" b"<<b<<" c"<<c<<" test"<<(sq(b) - 4*a*c)<<flush;
//  cout<<" x2:"<<x2<<" z2:"<<z2<<" x1:"<<x1<<" z1:"<<z1<<endl;

  for(int i=0; i<JNum; i++){
    angle[i] = q[i];
  }
}


/*
void
FrameInverse::InvKinematics( const EE_POSITION &P, int JNum, double* angle ){
  px = P.x;
  py = P.y;
  pz = P.z;

  eta = atan2(-py, pz);
  mu = atan2(px, pz);
  lambda = sqrt(sq(px) + sq(py) + sq(pz));

  t1 = atan2((tan(eta)*cos(rho/2) - sin(sigma)*sin(rho/2) + tan(mu)*cos(sigma)*sin(rho/2)), (tan(mu)*sin(sigma)+cos(sigma)));
  t2 = atan2((cos(t1)*(-tan(eta)*sin(rho/2) - sin(sigma)*cos(rho/2) + tan(mu)*cos(sigma)*cos(rho/2))), (tan(mu)*sin(sigma) + cos(sigma)));

  alpha = acos((sq(l1+l2) + sq(L - lambda) - sq(l3))/(2*(l1 + l2)*(L-lambda)));
  beta = acos((sq(l1+l2) + sq(l3) - sq(L - lambda))/(2*(l1 + l2)*l3));

  x2 = (-px*(L - lambda))/(lambda);
  y2 = (-py*(L - lambda))/(lambda);
  z2 = (-pz*(L - lambda))/(lambda);

  kappa1 = (l1 + l2)*(L - lambda)*cos(alpha);
  kappa2 = (l1 + l2)*l3*cos(beta);

  a = sq(y2*tan(eta) - z2)+sq(x2)*(1 + sq(tan(eta)));
  b = (y2*tan(eta) - z2)*(2*kappa1 - sq(x2)) + sq(x2)*(y2*tan(eta) - z2);
  c = kappa1*(kappa1 - sq(x2)) - sq(x2)*kappa2;
  d = -sin(sigma)*cos(t1)*sin(t2) + cos(sigma)*(cos(rho/2)*cos(t2) - sin(rho/2)*sin(t1)*sin(t2));
  e = sin(sigma)*sin(t1) - cos(sigma)*sin(rho/2)*cos(t1);

//  t4 = atan2( (e+d*tan(psi)), (d-e*tan(psi)) );

  if(px < 0) {z1 = (-b - sqrt(sq(b) - 4*a*c))/(2*a);}
  else {z1 = (-b + sqrt(sq(b) - 4*a*c))/(2*a);}

  y1 = -z1*tan(eta);
  x1 = (x2 + sqrt(sq(x2) - 4*(y1*(y1 - y2) + z1*(z1 - z2) - kappa2)))/2;

  q[0] = atan2( (cos(rho)*sin(t1) - sin(rho)*tan(t2)), cos(t1) );
  q[1] = t1;
//  q[2] = -asin(z1/sqrt(sq(x1) + sq(z1)));
  q[2] = -atan2(z1, x1);
  q[3] = t4;
//  cout<<"a"<<a<<" b"<<b<<" c"<<c<<" test"<<(sq(b) - 4*a*c)<<flush;
//  cout<<" x2:"<<x2<<" z2:"<<z2<<" x1:"<<x1<<" z1:"<<z1<<endl;

  for(int i=0; i<4; i++){
    angle[i] = 180*q[i]/PI;
  }

}

*/


