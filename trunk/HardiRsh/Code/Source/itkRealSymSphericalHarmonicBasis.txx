#ifndef __itkRealSymSphericalHarmonicBasis_txx
#define __itkRealSymSphericalHarmonicBasis_txx

#include "vnl/vnl_math.h"
#include <cmath>

#define USE_WIGNER_ROTATION 2

#ifndef USE_WIGNER_ROTATION
#include "vnl/algo/vnl_qr.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#endif

#include "itkMatrix.h"

namespace itk
{

//Public methods
template < unsigned int Order > 
const unsigned int 
RealSymSphericalHarmonicBasis< Order >
::GetJ(int l,int m)
{
  return 1 + m + l * (l+1) / 2;
}

template < unsigned int Order > 
const typename  RealSymSphericalHarmonicBasis< Order >::LmVector
RealSymSphericalHarmonicBasis< Order >
::GetLM(int j)
{
  const int l = 2 * (int) ( ((1 + vcl_sqrt(8 * j - 7)) / 2) / 2);
  const int m = j - 1 - l * (l+1) / 2;
  typename RealSymSphericalHarmonicBasis< Order >::LmVector retVal;

  retVal[0] = l;
  retVal[1] = m;

  return retVal;
}

/**
 * Compute the real spherical harmonics the traditional way.
 * Not sure how this stacks up agains other computation stratagies.
 */
template < unsigned int Order > 
double
RealSymSphericalHarmonicBasis< Order >
::Y( int j, double theta, double phi )
{

  LmVector vec = GetLM(j);
  const int l = vec[0];
  const int m = vec[1];
  
  if( m == 0 ) /// Y_l^0
    return K(l,0) * LegendreP(l,m,vcl_cos(theta));
  else if( m < 0 ) /// sqrt2 re(y_l^m)
    return vnl_math::sqrt2 * K(l,m) * vcl_cos(m*phi) * LegendreP(l,m,vcl_cos(theta));
  else ///(m > 0) sqrt2 im(y_l^m)
    return vnl_math::sqrt2* K(l,m) * vcl_sin(m*phi) * LegendreP(l,m,vcl_cos(theta));
}

//Begin Private methods
/**
 * Nomalization factor for the spherical harmonics...
 * vcl_sqrt( ( (2*l+1) * factorial(l-m) ) / ( 4*( vnl_math::pi ) * factorial(l+m) ) );
 * 
 * Use a speed up to compute factorial(l-m) / factorial(l+m)
 * 
 * No Overflow is checked but this should be more robust then using the
 * Factorial method.
 */
template < unsigned int Order > 
double
RealSymSphericalHarmonicBasis< Order >
::K( int l, int m )
{
  double f = 1; //if m=0
  if (m > 0)
  {
    for(int i=l-m+1; i<l+m+1; i++)
    {
      f /= i; 
    }
  }
  else
  {
    for(int i=l+m+1; i<l-m+1; i++)
      {
        f *= i; 
      }
  }
  return vcl_sqrt( ( (2*l+1) / ( 4*( vnl_math::pi ) ) * f ) );
}

template < unsigned int Order >
template < typename TMatrixValueType > 
const 
typename RealSymSphericalHarmonicBasis< Order >::RshRotationMatixType
RealSymSphericalHarmonicBasis< Order >
::ComputeRotationMatrix( const Matrix<TMatrixValueType, 3u, 3u> & in )
{
  
  const typename Matrix<TMatrixValueType, 3u, 3u>::InternalMatrixType m = in.GetTranspose();
  

  #if (USE_WIGNER_ROTATION==1)
  //Convert rotation matrix m in zyz euler angles..
  double alpha;
  double beta;
  double gamma;

  if ( m(2,2) == 1.0 ) //beta is 0 so alpha and gamma are indistiguishable
  {
    beta = 0;
    alpha = 0;
    gamma = atan2(-m(0,1),m(0,0));;
  }
  else if ( m(2,2) == -1.0 ) //beta is Pi
  {
    beta = vnl_math::pi;
    itkGenericExceptionMacro("Was hopping this wouldn't happen");
  }
  else
  {
    beta  = acos(m(2,2));
    alpha = atan2(m(2,1),-m(2,0));
    gamma = atan2(m(1,2),m(0,2));
  }


  const RshRotationMatixType zRotGamma      = ComputeZRotationMatrix(gamma);

  RshRotationMatixType results = zRotGamma;
  if ( gamma == 0) results.SetIdentity(); 

  if ( beta != 0)
  {
    results = GetDMatMinusPiOver2()
      * ComputeZRotationMatrix(beta) * GetDMatPiOver2() * results;
  }

  if ( beta != 0)
  {
    results = ComputeZRotationMatrix(alpha) * results;
  }

  ///It may be prudent to re normalize here...
#elif (USE_WIGNER_ROTATION==2)
  typedef typename RshRotationMatixType::ValueType            MatrixValueType;

  //Convert rotation matrix m in zyz euler angles..
  double alpha;
  double beta;
  double gamma;
  
  if ( m(2,2) == 1.0 ) //beta is 0 so alpha and gamma are indistiguishable
  {
    beta = 0;
    alpha = 0;
    gamma = atan2(-m(0,1),m(0,0));;
  }
  else if ( m(2,2) == -1.0 ) //beta is Pi
  {
    beta = vnl_math::pi;
    itkGenericExceptionMacro("Was hopping this wouldn't happen");
  }
  else
  {
    beta  = acos(m(2,2));
    alpha = atan2(m(2,1),-m(2,0));
    gamma = atan2(m(1,2),m(0,2));
  }


  RshRotationMatixType M3; // M3 = zRotAlpha * GetDMatMinusPiOver2() * zRotBeta
  RshRotationMatixType M1; // M1 = GetDMatPiOver2() * zRotGamma

  const RshRotationMatixType P2 = GetDMatPiOver2();
  const RshRotationMatixType P1 = GetDMatMinusPiOver2();
  
  //Fill in M1 and M3
  unsigned blockIndex = 0;

  int ip1Sign; // (-1)^(i+1)
  int jp1Sign; // (-1)^(j+1)

  for (int l=0;l<=static_cast<int>(Order);l+=2)
  {
    ip1Sign = -1; // start at (-1)^(0+1)
    for (int i = 0; i< 2*l+1; ++i)
    {
      jp1Sign = -1; // (-1)^(j+1)
      for (int j = 0; j< 2*l+1; ++j)
      {
        if ( (i==l) )
        {
          if (j==l)
          {
            M1(blockIndex+i,blockIndex+j) = 
                    static_cast<MatrixValueType>(P2(blockIndex+i,blockIndex+j));

            M3(blockIndex+i,blockIndex+j) = 
                    static_cast<MatrixValueType>(P1(blockIndex+i,blockIndex+j));
                    
          }
          else
          {
            M1(blockIndex+i,blockIndex+j) = static_cast<MatrixValueType>
              ( vcl_cos( (j-l) * gamma) * P2(blockIndex+i,blockIndex+j) +
                jp1Sign * vcl_sin( (l-j) * gamma) * P2(blockIndex+i,blockIndex+2*l-j)
              );
              
            M3(blockIndex+i,blockIndex+j) = static_cast<MatrixValueType>
              ( vcl_cos( (j-l) * beta) * P1(blockIndex+i,blockIndex+j) +
                jp1Sign * vcl_sin( (l-j) * beta) * P1(blockIndex+i,blockIndex+2*l-j)
              );
              
          }
        }
        else //i != l
        {
          M1(blockIndex+i,blockIndex+j) = static_cast<MatrixValueType>
              ( vcl_cos( (j-l) * gamma) * P2(blockIndex+i,blockIndex+j) +
                jp1Sign * vcl_sin( (l-j) * gamma) * P2(blockIndex+i,blockIndex+2*l-j)
              );
          
          M3(blockIndex+i,blockIndex+j) = static_cast<MatrixValueType>(
            vcl_cos( (i-l) * alpha) * ( vcl_cos( (j-l) * beta) * P1(blockIndex+i,blockIndex+j)
                     + jp1Sign * vcl_sin( (l-j) * beta) * P1(blockIndex+i,blockIndex+2*l-j) )
            + ip1Sign * vcl_sin( (i-l) * alpha) * ( vcl_cos( (j-l) * beta) * P1(blockIndex+2*l-i,blockIndex+j)
                     + jp1Sign * vcl_sin( (l-j) * beta) * P1(blockIndex+2*l-i,blockIndex+2*l-j) )
            );        
            
        }
        jp1Sign *= -1;
      }
      ip1Sign *= -1;
    }
    //Incerment the block index.
    blockIndex += 2*l + 1;
  }

  //~ const RshRotationMatixType zRotBeta       = ComputeZRotationMatrix(beta);
  //~ const RshRotationMatixType zRotAlpha      = ComputeZRotationMatrix(alpha);
  
  //  M3 = zRotAlpha * GetDMatMinusPiOver2() * zRotBeta;
  
  //~ std::cerr << M3 << std::endl;
  //~ std::cerr << zRotAlpha * GetDMatMinusPiOver2() * zRotBeta << std::endl;
  
  RshRotationMatixType results = M3 * M1;
  
  
#else
  
  //how many grads to use!
  unsigned int nGrads = 2 * Dimensions;
  
  //Compute evaluate this on a bunch of points!!!
  GradientDirectionContainerPointer initGrads
                                  = ComputeGradDirections(nGrads);

  RshBasisMatrixType initBasis = ComputeRshBasis(initGrads);
  
  //Now Rotate the initGrads
  //We want a full copy of the container at initGrads
  GradientDirectionContainerType::Pointer
              rotatedGrads = GradientDirectionContainerType::New();
  
  GradientDirectionType dir,rDir;

  for (unsigned int g = 0; g < initGrads->size(); ++g)
  {
    dir[0] = initGrads->GetElement(g)[0];
    dir[1] = initGrads->GetElement(g)[1];
    dir[2] = initGrads->GetElement(g)[2];
    
    //init the rotated direction
    rDir.fill(0);
    
    //Perform the rotation
    //rdir = m * dir;
    for (unsigned int r=0;r<3;++r)
    {
      for (unsigned int c=0;c<3;++c)
      {
        rDir[r] += m(r,c) * dir[c];
      }
    }
    rotatedGrads->InsertElement(g,rDir/rDir.two_norm());
  }

    
  RshBasisMatrixType rotBasis = ComputeRshBasis(rotatedGrads);

  //vnl_matrix_inverse uses svd, would like something faster!
  //vnl_matirx_inverse has problems compiling!
  //const RshRotationMatixType results = vnl_matrix_inverse<double>(rotBasis) * initBasis;

  vnl_svd<double> solver(initBasis);
  const RshRotationMatixType results = solver.inverse() * rotBasis;

#endif

  return results;
}


template < unsigned int Order > 
const
typename RealSymSphericalHarmonicBasis< Order >::RshRotationMatixType
RealSymSphericalHarmonicBasis< Order >
::ComputeZRotationMatrix(double alpha)
{
  typedef typename RshRotationMatixType::ValueType            MatrixValueType;
  RshRotationMatixType results;

  if (alpha ==0)
  {
    results.SetIdentity();
  }
  else
  {
    unsigned blockIndex = 0;

    for (int l=0;l<=static_cast<int>(Order);l+=2)
    {
      for (int m = -l;m<=l;++m)
      {
        if (m == 0)
        {
          results(blockIndex+l,blockIndex+l) = static_cast<MatrixValueType>(1);
        }
        else
        {
          results(blockIndex+l+m,blockIndex+l+m) =
            static_cast<MatrixValueType>(cos(m*alpha));

          int sign = 1;
          if (vcl_abs(m+1) % 2 == 1) sign = -1;
          results(blockIndex+l+m,blockIndex+l-m) =
            static_cast<MatrixValueType>(sign * sin(m*alpha));

        }
      }
      //Incerment the block index.
      blockIndex += 2*l + 1;
    }
  }
  return results;
}

template < unsigned int Order > 
void
RealSymSphericalHarmonicBasis< Order >
::Initialize()
{
#ifdef USE_WIGNER_ROTATION
  RshRotationMatixType ryPlus,ryMinus; 
  ryPlus.Fill(0.0);
  ryMinus.Fill(0.0);

  /** First compute Ry(pi/2) and Ry(-pi/2) */
  int lMax = Order;
  int bigI = 0;

  for(int l=0; l<lMax+1; l+=2)
  {
    for(int mp=-l; mp<l+1; mp++)
    {
      for(int m=-l; m<l+1; m++)
      {
        //computeSomeSigns
        int sign_m =    ( vcl_abs(m)     % 2 == 1) ? -1 :1;     // (-1)^m
        int sign_mp =   ( vcl_abs(mp)    % 2 == 1) ? -1 :1;     // (-1)^mp
        int sign_mpm =  ( vcl_abs(mp+m)  % 2 == 1) ? -1 :1;     // (-1)^(m+mp)
        int sign_m1 =   ( vcl_abs((m+1)) % 2 == 1) ? -1 :1;     // (-1)^(m+1)

        //Matrix indices
        int mpi = bigI + mp + l;
        int mi = bigI + m + l;
        double plusVal;
        if (m == 0)
        {
          if (mp==0)
          {
            plusVal = computeWignerLittleDPlus(l, mp, m);
          }
          else if (mp<0)
          {
            plusVal = vnl_math::sqrt2 * sign_mp * computeWignerLittleDPlus(l, -mp, m);
          }
          else //mp>0
          {
            plusVal = 0.0;
          }
        }
        else if (m < 0)
        {
          if (mp==0)
          {
            plusVal =vnl_math::sqrt2 * sign_m * computeWignerLittleDPlus(l, mp, -m);
          }
          else if (mp<0)
          {
            plusVal = sign_mpm * computeWignerLittleDPlus(l, -mp, -m)
                              + sign_mp * computeWignerLittleDPlus(l, -mp, m);
          }
          else //mp>0
          {
            plusVal = 0.0;
          }
        }
        else //m >0
        {
        if (mp==0)
          {
            plusVal = 0.0;
          }
          else if (mp<0)
          {
            plusVal = 0.0;
          }
          else //mp>0
          {
            plusVal = computeWignerLittleDPlus(l, mp, m) + sign_m1 * computeWignerLittleDPlus(l,mp,-m);
          }
        }
        
        ryPlus(mpi,mi) = plusVal;
        
        //Determine (-1)^(m-mp)
        int sign = 1;
        if (vcl_abs(m-mp) % 2 == 1) sign = -1;
        ryMinus(mpi,mi) = sign * plusVal;
 
      }
    }
    bigI+= 2 * l+1;
    
  }

  const RshRotationMatixType zPlus       = ComputeZRotationMatrix(vnl_math::pi_over_2);
  const RshRotationMatixType zMinus      = ComputeZRotationMatrix(-vnl_math::pi_over_2);  
  m_DMatPiOver2       = ryPlus * zPlus;
  m_DMatMinusPiOver2  = zMinus * ryMinus;
#endif  
}

/****EDMONDS implementaiton */
template < unsigned int Order > 
const
double
RealSymSphericalHarmonicBasis< Order >
::computeWignerLittleDPlus(int l, int mp, int m)
{
  //Determine (-1)^(l-mp)
  int sign = 1;
  if ( vcl_abs(l-mp) % 2 == 1) sign = -1;

  //Compute the normalization factor
  double f;
  
  f = 1; //if m==mp
  if (mp>m)
  {
    for(int i=l+m+1; i<=l+mp; i++)
    {
      f *= i;
    }
    for(int i=l-mp+1; i<=l-m; i++)
    {
      f /= i;
    }
  }
  else if (m>mp)
  {
    for(int i=l-m+1; i<=l-mp; i++)
    {
      f *= i;
    }
    for(int i=l+mp+1; i<=l+m; i++)
    {
      f /= i;
    }
  }

  //Compute bounds on the summation
  int sMin = ( 0 > -(mp+m) ) ? 0 : -(mp+m); // Max[0, -(mp + m)]
  int sMax = ( l-mp < l-m ) ? l-mp : l-m;   // Min[l - mp, l - m];
  
  double sum = 0;
  for(int s= sMin; s<= sMax; s++) //Not that s > 0 so no need for vcl_abs(s) in (-1)^s
  {
    if (s % 2 == 1) // (-1)^s == -1
    {
      sum -= binomialCoeff(l + m, l - mp - s)*binomialCoeff(l - m, s);
    }
    else            // (-1)^s == +1;
    {
      sum += binomialCoeff(l + m, l - mp - s)*binomialCoeff(l - m, s);
    }
  }

  return sign * vcl_sqrt(f) / vcl_pow(2.0,l) * sum;
}

/**Getters and Setters */
#if 0
template < unsigned int Order > 
void
RealSymSphericalHarmonicBasis< Order >
::SetDMatPiOver2(RshRotationMatixType inMat)
{
  m_DMatPiOver2 = inMat;
}
  
template < unsigned int Order > 
void
RealSymSphericalHarmonicBasis< Order >
::SetDMatMinusPiOver2(RshRotationMatixType inMat)
{
  m_DMatMinusPiOver2 = inMat;
}

#endif

template < unsigned int Order > 
const
typename RealSymSphericalHarmonicBasis< Order >::RshRotationMatixType
RealSymSphericalHarmonicBasis< Order >
::GetDMatPiOver2() const
{
  return m_DMatPiOver2;
}

template < unsigned int Order > 
const
typename RealSymSphericalHarmonicBasis< Order >::RshRotationMatixType
RealSymSphericalHarmonicBasis< Order >
::GetDMatMinusPiOver2() const
{
  return m_DMatMinusPiOver2;
}

}


#endif
