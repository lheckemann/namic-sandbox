# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>

using namespace std;

# include "prob.h"

//******************************************************************************

double angle_cdf ( double x, int n )

//******************************************************************************
//
//  Purpose:
//
//    ANGLE_CDF evaluates the Angle CDF.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Reuven Rubinstein,
//    Monte Carlo Optimization, Simulation and Sensitivity of Queueing Networks,
//    Wiley, 1986.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, integer N, the spatial dimension.
//    N must be at least 2.
//
//    Output, real CDF, the value of the CDF.
//
{
# define PI 3.14159265358979323

  double cdf;
  double zero = 0.0;

  if ( n < 2 )
  {
    cout << "\n";
    cout << "ANGLE_CDF - Fatal error!\n";
    cout << "  N must be at least 2.\n";
    cout << "  The input value of N = " << n << "\n";
    exit ( 1 );
  }

  if ( x < 0.0 )
  {
    cdf = 0.0;
  }
  else if ( PI < x )
  {
    cdf = 1.0;
  }
  else if ( n == 2 )
  {
    cdf = x / PI;
  }
  else
  {
    cdf = sin_power_int ( zero, x, n )
      * gamma ( ( double ) ( n ) / 2.0 ) 
      / ( sqrt ( PI ) * gamma ( ( double ) ( n - 1 ) / 2.0 ) );
  }

  return cdf;
# undef PI
}
//******************************************************************************

double angle_mean ( int n )

//******************************************************************************
//
//  Purpose:
//
//    ANGLE_MEAN returns the mean of the Angle PDF.
//
//  Modified:
//
//    02 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the spatial dimension.
//    N must be at least 2.
//
//    Output, double ANGLE_MEAN, the mean of the PDF.
//
{
# define PI 3.14159265358979323

  double mean;

  mean = PI / 2.0;

  return mean;
# undef PI
}
//******************************************************************************

double angle_pdf ( double x, int n )

//******************************************************************************
//
//  Purpose:
//
//    ANGLE_PDF evaluates the Angle PDF.
//
//  Discussion:
//
//    X is an angle between 0 and PI, corresponding to the angle
//    made in an N-dimensional space, between a fixed line passing 
//    through the origin, and an arbitrary line that also passes
//    through the origin, which is specified by a choosing any point
//    on the N-dimensional sphere with uniform probability.
//
//  Formula:
//
//    PDF(X) = ( sin ( X ) )**(N-2) * Gamma ( N / 2 )
//             / ( sqrt ( PI ) * Gamma ( ( N - 1 ) / 2 ) )
//
//    PDF(X) = 1 / PI if N = 2.
//
//  Modified:
//
//    02 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Reuven Rubinstein,
//    Monte Carlo Optimization, Simulation and Sensitivity of Queueing Networks,
//    Wiley, 1986.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, integer N, the spatial dimension.
//    N must be at least 2.
//
//    Output, real PDF, the value of the PDF.
//
{
# define PI 3.14159265358979323

  double pdf;

  if ( n < 2 )
  {
    cout << "\n";
    cout << "ANGLE_PDF - Fatal error!\n";
    cout << "  N must be at least 2.\n";
    cout << "  The input value of N = " << n << "\n";
    exit ( 1 );
  }

  if ( x < 0.0 || PI < x )
  {
    pdf = 0.0;
  }
  else if ( n == 2 )
  {
    pdf = 1.0 / PI;
  }
  else
  {
    pdf = pow ( ( sin ( x ) ), ( n - 2 ) )
      * gamma ( ( double ) ( n ) / 2.0 ) 
      / ( sqrt ( PI ) * gamma ( ( double ) ( n - 1 ) / 2.0 ) );
  }

  return pdf;
# undef PI
}
//******************************************************************************

double anglit_cdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_CDF evaluates the Anglit CDF.
//
//  Modified:
//
//    29 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Output, double ANGLIT_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <  - 0.25 * d_pi ( ) )
  {
    cdf = 0.0;
  }
  else if ( x < 0.25 * d_pi ( ) )
  {
    cdf = 0.5 - 0.5 * cos ( 2.0 * x + d_pi ( ) / 2.0 );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double anglit_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_CDF_INV inverts the Anglit CDF.
//
//  Modified:
//
//    29 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Output, double ANGLIT_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "ANGLIT_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = 0.5 * ( acos ( 1.0 - 2.0 * cdf ) - d_pi ( ) / 2.0 );

  return x;
}
//******************************************************************************

double anglit_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_MEAN returns the mean of the Anglit PDF.
//
//  Modified:
//
//    28 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double ANGLIT_MEAN, the mean of the PDF.
//
{
  return 0.0;
}
//******************************************************************************

double anglit_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_PDF evaluates the Anglit PDF.
//
//  Formula:
//
//    PDF(X) = SIN ( 2 * X + PI / 2 ) for -PI/4 <= X <= PI/4
//
//  Modified:
//
//    28 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Output, double ANGLIT_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= - 0.25 * d_pi ( ) || 0.25 * d_pi ( ) <= x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = sin ( 2.0 * x + 0.25 * d_pi ( ) );
  }

  return pdf;
}
//******************************************************************************

double anglit_sample ( int *seed )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_SAMPLE samples the Anglit PDF.
//
//  Modified:
//
//    28 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double *X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = anglit_cdf_inv ( cdf );

  return x;
}
//******************************************************************************

double anglit_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    ANGLIT_VARIANCE returns the variance of the Anglit PDF.
// 
//  Discussion:
//
//    Variance = 
//      Integral ( -PI/4 <= X <= PI/4 ) X**2 * SIN ( 2 * X + PI / 2 ) 
//
//    Antiderivative = 
//      0.5 * X * SIN ( 2 * X + PI / 2 )
//      + ( 0.25 - 0.5 * X**2 ) * COS ( 2 * X + PI / 2 )
//
//  Modified:
//
//    29 December 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double ANGLIT_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 0.0625 * d_pi ( ) * d_pi ( ) - 0.5;

  return variance;
}
//******************************************************************************

double arcsin_cdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_CDF evaluates the Arcsin CDF.
//
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Output, double ARCSIN_CDF, the value of the CDF.
//
{
# define PI 3.14159265358979323

  double cdf;

  if ( x <= -a )
  {
    cdf = 0.0;
  }
  else if ( x < a )
  {
    cdf = 0.5 + asin ( x / a ) / PI;
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
# undef PI
}
//*****************************************************************************

double arcsin_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_CDF_INV inverts the Arcsin CDF.
//
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Output, double ARCSIN_CDF_INV, the corresponding argument.
//
{
# define PI 3.14159265358979323
  
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "ARCSIN_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a * sin ( PI * ( cdf - 0.5 ) );

  return x;
# undef PI
}
//******************************************************************************

bool arcsin_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_CHECK checks the parameter of the Arcsin CDF.
//
//  Modified:
//
//    27 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, bool ARCSIN_CHECK, is TRUE if the data is acceptable.
{
  if ( a <= 0.0 )
  {
    return false;
  }
  else
  {
    return true;
  }
}
//*****************************************************************************

double arcsin_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_MEAN returns the mean of the Arcsin PDF.
//
// 
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean = 0.0;

  return mean;
}
//*****************************************************************************

double arcsin_pdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_PDF evaluates the Arcsin PDF.
//
//  Discussion:
//
//    The LOGISTIC EQUATION has the form:
//
//      X(N+1) = 4.0 * LAMBDA * ( 1.0 - X(N) ).
//
//    where 0 < LAMBDA <= 1.  This nonlinear difference equation maps
//    the unit interval into itself, and is a simple example of a system
//    exhibiting chaotic behavior.  Ulam and von Neumann studied the
//    logistic equation with LAMBDA = 1, and showed that iterates of the
//    function generated a sequence of pseudorandom numbers with 
//    the Arcsin probability density function.
//
//    The derived sequence
//
//      Y(N) = ( 2 / PI ) * Arcsin ( SQRT ( X(N) ) )
//
//    is a pseudorandom sequence with the uniform probability density
//    function on [0,1].  For certain starting values, such as X(0) = 0, 0.75,
//    or 1.0, the sequence degenerates into a constant sequence, and for
//    values very near these, the sequence takes a while before becoming
//    chaotic.
//
//  Formula:
//
//    PDF(X) = 1 / ( PI * Sqrt ( A * A - X * X ) )
//
//  Reference:
//
//    Daniel Zwillinger and Stephen Kokoska,
//    CRC Standard Probability and Statistics Tables and Formulae,
//    Chapman and Hall/CRC, 2000, pages 114-115.
//
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 < X < 1.0.
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Output, double ARCSIN_PDF, the value of the PDF.
//
{
# define PI 3.14159265358979323

  double pdf;

  if ( a <= 0 )
  {
    cout << "\n";
    cout << "ARCSIN_PDF - Fatal error!\n";
    cout << "  Parameter A <= 0.\n";
    exit ( 1 );
  }

  if ( x <= -a || a <= x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 1.0 / ( PI * sqrt ( a * a - x * x ) );
  }

  return pdf;

# undef PI
}
//*****************************************************************************

double arcsin_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    ARCSIN_SAMPLE samples the Arcsin PDF.
//
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double ARCSIN_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = arcsin_cdf_inv ( cdf, a );

  return x;
}
//*****************************************************************************

double arcsin_variance ( double a )

//*****************************************************************************
//
//  Purpose:
//
//    ARCSIN_VARIANCE returns the variance of the Arcsin PDF.
//
//  Modified:
//
//    20 March 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the CDF.
//    A must be positive.
//
//    Output, double ARCSIN_VARIANCE, the variance of the PDF.
//
{
  double variance = a * a / 2.0;

  return variance;
}
//******************************************************************************

double benford_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    BENFORD_PDF returns the Benford probability of one or more significant digits.
//
//  Discussion:
//
//    Benford's law is an empirical formula explaining the observed
//    distribution of initial digits in lists culled from newspapers,
//    tax forms, stock market prices, and so on.  It predicts the observed
//    high frequency of the initial digit 1, for instance.
//
//    Note that the probabilities of digits 1 through 9 are guaranteed
//    to add up to 1, since
//      LOG10 ( 2/1 ) + LOG10 ( 3/2) + LOG10 ( 4/3 ) + ... + LOG10 ( 10/9 )
//      = LOG10 ( 2/1 * 3/2 * 4/3 * ... * 10/9 ) = LOG10 ( 10 ) = 1.
//
//  Formula:
//
//    PDF(X) = LOG10 ( ( X + 1 ) / X ).
//
//  Reference:
//
//    F Benford,
//    The Law of Anomalous Numbers,
//    Proceedings of the American Philosophical Society,
//    Volume 78, pages 551-572, 1938.
//
//    T P Hill,
//    The First Digit Phenomenon,
//    American Scientist,
//    Volume 86, July/August 1998, pages 358 - 363.
//
//    R Raimi,
//    The Peculiar Distribution of First Digits,
//    Scientific American,
//    December 1969, pages 109-119.
//
//  Modified:
//
//    13 August 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the string of significant digits to be checked.
//    If X is 1, then we are asking for the Benford probability that
//    a value will have first digit 1.  If X is 123, we are asking for
//    the probability that the first three digits will be 123, and so on.
//
//    Output, double BENFORD_PDF, the Benford probability that an item taken
//    from a real world distribution will have the initial digits X.
//
{
  double pdf;

  if ( x <= 0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = log10 ( ( double ) ( x + 1 ) / ( double ) ( x ) );
  }

  return pdf;
}
//******************************************************************************

double bernoulli_cdf ( int x, double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_CDF evaluates the Bernoulli CDF.
//
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the number of successes on a single trial.
//    X = 0 or 1.
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double BERNOULLI_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < 0 )
  {
    cdf = 0.0;
  }
  else if ( x == 0 )
  {
    cdf = 1.0 - a;
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

int bernoulli_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_CDF_INV inverts the Bernoulli CDF.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, the parameter of the PDF.
//    0.0 <= A <= 1.0.
//
//    Output, int BERNOULLI_CDF_INV, the corresponding argument.
//
{
  int x;
//
  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "BERNOULLI_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf <= 1.0 - a )
  {
    x = 0;
  }
  else
  {
    x = 1;
  }

  return x;
}
//******************************************************************************

bool bernoulli_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_CHECK checks the parameter of the Bernoulli CDF.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 <= A <= 1.0.
//
//    Output, bool BERNOULLI_CHECK, is TRUE if the data is acceptable.
{
  if ( a < 0.0 || 1.0 < a )
  {
    return false;
  }
  else
  {
    return true;
  }
}
//******************************************************************************

double bernoulli_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_MEAN returns the mean of the Bernoulli PDF.
// 
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success.
//    0.0 <= A <= 1.0.
//
//    Output, double BERNOULLI_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double bernoulli_pdf ( int x, double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_PDF evaluates the Bernoulli PDF.
//
//  Formula:
//
//    PDF(X)(A) = A**X * ( 1.0 - A )**( X - 1 )
// 
//    X = 0 or 1.
//
//  Discussion:
//
//    The Bernoulli PDF describes the simple case in which a single trial 
//    is carried out, with two possible outcomes, called "success" and 
//    "failure"; the probability of success is A.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the number of successes on a single trial.
//    X = 0 or 1.
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double BERNOULLI_PDF, the value of the PDF.
//
{
  double pdf;
//
  if ( x < 0 )
  {
    pdf = 0.0;
  }
  else if ( x == 0 )
  {
    pdf = 1.0 - a;
  }
  else if ( x == 1 )
  {
    pdf = a;
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
}
//******************************************************************************

int bernoulli_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_SAMPLE samples the Bernoulli PDF.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int BERNOULLI_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = bernoulli_cdf_inv ( cdf, a );

  return x;
}
//******************************************************************************

double bernoulli_variance ( double a )

//******************************************************************************
//
//  Purpose:
//
//    BERNOULLI_VARIANCE returns the variance of the Bernoulli PDF.
// 
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double BERNOULLI_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = a * ( 1.0 - a );

  return variance;
}
//******************************************************************************

double bessel_i0 ( double arg )

//******************************************************************************
//
//  Purpose:
//
//    BESSEL_I0 evaluates the modified Bessel function I0.
//
//  Discussion:
//
//    The main computation evaluates slightly modified forms of
//    minimax approximations generated by Blair and Edwards, Chalk
//    River (Atomic Energy of Canada Limited) Report AECL-4928,
//    October, 1974.  This transportable program is patterned after
//    the machine dependent FUNPACK packet NATSI0, but cannot match
//    that version for efficiency or accuracy.  This version uses
//    rational functions that theoretically approximate I-SUB-0(X)
//    to at least 18 significant decimal digits.  
//
//  Machine dependent constants:
//
//    beta   = Radix for the floating-point system
//    maxexp = Smallest power of beta that overflows
//    XSMALL = Positive argument such that 1.0 - X = 1.0 to
//             machine precision for all ABS(X) .LE. XSMALL.
//    XMAX =   Largest argument acceptable to BESI0;  Solution to
//             equation:
//               W(X) * (1+1/(8*X)+9/(128*X**2) = beta**maxexp
//             where  W(X) = EXP(X)/sqrt(2*PI*X)
//
//    Approximate values for some important machines are:
//
//                             beta       maxexp       XSMALL
//
//    CRAY-1        (S.P.)       2         8191       3.55D-15
//    Cyber 180/855
//      under NOS   (S.P.)       2         1070       3.55D-15
//    IEEE (IBM/XT,
//      SUN, etc.)  (S.P.)       2          128       2.98D-8
//    IEEE (IBM/XT,
//      SUN, etc.)  (D.P.)       2         1024       5.55D-17
//    IBM 3033      (D.P.)      16           63       6.95D-18
//    VAX           (S.P.)       2          127       2.98D-8
//    VAX D-Format  (D.P.)       2          127       6.95D-18
//    VAX G-Format  (D.P.)       2         1023       5.55D-17
//
//
//                                  XMAX
//
//    CRAY-1        (S.P.)       5682.810
//    Cyber 180/855
//      under NOS   (S.P.)       745.893
//    IEEE (IBM/XT,
//      SUN, etc.)  (S.P.)        91.900
//    IEEE (IBM/XT,
//      SUN, etc.)  (D.P.)       713.986
//    IBM 3033      (D.P.)       178.182
//    VAX           (S.P.)        91.203
//    VAX D-Format  (D.P.)        91.203
//    VAX G-Format  (D.P.)       713.293
//
//  Author: 
//
//    W. J. Cody and L. Stoltz,
//    Mathematics and Computer Science Division,
//    Argonne National Laboratory,
//    Argonne, Illinois, 60439.
//
//  Parameters:
//
//    Input, double ARG, the argument.
//
//    Output, double BESSEL_I0, the value of the modified Bessel function
//    of the first kind.
//
{
  double a;
  double b;
  double exp40 = 2.353852668370199854E+17;
  int i;
  double p[15] = {
    -5.2487866627945699800E-18, 
    -1.5982226675653184646E-14, 
    -2.6843448573468483278E-11, 
    -3.0517226450451067446E-08, 
    -2.5172644670688975051E-05, 
    -1.5453977791786851041E-02, 
    -7.0935347449210549190E+00, 
    -2.4125195876041896775E+03, 
    -5.9545626019847898221E+05, 
    -1.0313066708737980747E+08, 
    -1.1912746104985237192E+10, 
    -8.4925101247114157499E+11, 
    -3.2940087627407749166E+13, 
    -5.5050369673018427753E+14, 
    -2.2335582639474375249E+15 };
  double pp[8] = { 
    -3.9843750000000000000E-01, 
     2.9205384596336793945E+00, 
    -2.4708469169133954315E+00, 
     4.7914889422856814203E-01, 
    -3.7384991926068969150E-03, 
    -2.6801520353328635310E-03, 
     9.9168777670983678974E-05, 
    -2.1877128189032726730E-06 };
  double q[5] = { 
    -3.7277560179962773046E+03, 
     6.5158506418655165707E+06, 
    -6.5626560740833869295E+09, 
     3.7604188704092954661E+12, 
    -9.7087946179594019126E+14 };
  double qq[7] = { 
    -3.1446690275135491500E+01, 
     8.5539563258012929600E+01, 
    -6.0228002066743340583E+01, 
     1.3982595353892851542E+01, 
    -1.1151759188741312645E+00, 
     3.2547697594819615062E-02, 
    -5.5194330231005480228E-04 };
  double rec15 = 6.6666666666666666666E-02;
  double sump;
  double sumq;
  double value;
  double x;
  double xmax = 91.9;
  double xsmall = 2.98E-08;
  double xx;

  x = fabs ( arg );

  if ( x < xsmall )
  {
    value = 1.0;
  }
  else if ( x < 15.0 )
  {
//
//  XSMALL <= ABS(ARG) < 15.0
//
    xx = x * x;
    sump = p[0];
    for ( i = 1; i < 15; i++ )
    {
      sump = sump * xx + p[i];
    }

    xx = xx - 225.0;
    sumq = (((( 
           xx + q[0] ) 
         * xx + q[1] ) 
         * xx + q[2] ) 
         * xx + q[3] ) 
         * xx + q[4];

    value = sump / sumq;
  }
  else if ( 15.0 <= x )
  {
    if ( xmax < x )
    {
      value = d_huge ( );
    }
    else
    {
//
//  15.0 <= ABS(ARG)
//
      xx = 1.0 / x - rec15;

      sump = ((((((  
                  pp[0] 
           * xx + pp[1] ) 
           * xx + pp[2] ) 
           * xx + pp[3] ) 
           * xx + pp[4] ) 
           * xx + pp[5] ) 
           * xx + pp[6] ) 
           * xx + pp[7];

      sumq = (((((( 
             xx + qq[0] ) 
           * xx + qq[1] ) 
           * xx + qq[2] ) 
           * xx + qq[3] ) 
           * xx + qq[4] ) 
           * xx + qq[5] ) 
           * xx + qq[6];

      value = sump / sumq;
//
//  Calculation reformulated to avoid premature overflow.
//
      if ( x <= xmax - 15.0 )
      {
        a = exp ( x );
        b = 1.0;
      }
      else
      {
        a = exp ( x - 40.0 );
        b = exp40;
      }

      value = ( ( value * a - pp[0] * a ) / sqrt ( x ) ) * b;
    }
  }

  return value;
}
//******************************************************************************

void bessel_i0_values ( int *n_data, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    BESSEL_I0_VALUES returns some values of the I0 Bessel function.
//
//  Discussion:
//
//    The modified Bessel functions In(Z) and Kn(Z) are solutions of
//    the differential equation
//
//      Z^2 W'' + Z * W' - ( Z^2 + N^2 ) * W = 0.
//
//    The modified Bessel function I0(Z) corresponds to N = 0.
//
//    In Mathematica, the function can be evaluated by:
//
//      BesselI[0,x]
//
//  Modified:
//
//    20 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
//
# define N_MAX 20

  double fx_vec[N_MAX] = { 
     0.1000000000000000E+01,  
     0.1010025027795146E+01,  
     0.1040401782229341E+01,  
     0.1092045364317340E+01,  
     0.1166514922869803E+01,  
     0.1266065877752008E+01,  
     0.1393725584134064E+01,  
     0.1553395099731217E+01,  
     0.1749980639738909E+01,  
     0.1989559356618051E+01,  
     0.2279585302336067E+01,  
     0.3289839144050123E+01,  
     0.4880792585865024E+01,  
     0.7378203432225480E+01,  
     0.1130192195213633E+02,  
     0.1748117185560928E+02,  
     0.2723987182360445E+02,  
     0.6723440697647798E+02,  
     0.4275641157218048E+03,  
     0.2815716628466254E+04 };

  double x_vec[N_MAX] = { 
     0.00E+00,  
     0.20E+00,  
     0.40E+00,  
     0.60E+00,  
     0.80E+00,  
     0.10E+01,  
     0.12E+01,  
     0.14E+01,  
     0.16E+01,  
     0.18E+01,  
     0.20E+01,  
     0.25E+01,  
     0.30E+01,  
     0.35E+01,  
     0.40E+01,  
     0.45E+01,  
     0.50E+01,  
     0.60E+01,  
     0.80E+01,  
     0.10E+02 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double bessel_i1 ( double arg )

//******************************************************************************
//
//  Purpose:
//
//    BESSEL_I1 evaluates the Bessel I function of order I.
//
//  Discussion:
//
//    The main computation evaluates slightly modified forms of
//    minimax approximations generated by Blair and Edwards.
//    This transportable program is patterned after the machine-dependent
//    FUNPACK packet NATSI1, but cannot match that version for efficiency
//    or accuracy.  This version uses rational functions that theoretically
//    approximate I-SUB-1(X) to at least 18 significant decimal digits.
//    The accuracy achieved depends on the arithmetic system, the compiler,
//    the intrinsic functions, and proper selection of the machine-dependent
//    constants.
//
//  Machine-dependent constants:
//
//    beta   = Radix for the floating-point system.
//    maxexp = Smallest power of beta that overflows.
//    XMAX =   Largest argument acceptable to BESI1;  Solution to
//             equation:
//               EXP(X) * (1-3/(8*X)) / SQRT(2*PI*X) = beta**maxexp
//
//
//    Approximate values for some important machines are:
//
//                            beta       maxexp    XMAX
//
//    CRAY-1        (S.P.)       2         8191    5682.810
//    Cyber 180/855
//      under NOS   (S.P.)       2         1070     745.894
//    IEEE (IBM/XT,
//      SUN, etc.)  (S.P.)       2          128      91.906
//    IEEE (IBM/XT,
//      SUN, etc.)  (D.P.)       2         1024     713.987
//    IBM 3033      (D.P.)      16           63     178.185
//    VAX           (S.P.)       2          127      91.209
//    VAX D-Format  (D.P.)       2          127      91.209
//    VAX G-Format  (D.P.)       2         1023     713.293
//
//  Modified:
//
//    28 October 2004
//
//  Author:
//
//    W. J. Cody and L. Stoltz,
//    Mathematics and Computer Science Division,
//    Argonne National Laboratory,
//    Argonne, IL  60439.
//
//  Reference:
//
//    Blair and Edwards,
//    Chalk River Report AECL-4928,
//    Atomic Energy of Canada, Limited,
//    October, 1974.
//
//  Parameters:
//
//    Input, double ARG, the argument.
//
//    Output, double BESSEL_I1, the value of the Bessel
//    I1 function.
//
{
  double a;
  double b;
  double exp40 = 2.353852668370199854E+17;
  double forty = 40.0E+00;
  double half = 0.5E+00;
  int j;
  double one = 1.0E+00;
  double one5 = 15.0E+00;
  double p[15] = {
    -1.9705291802535139930E-19, 
    -6.5245515583151902910E-16, 
    -1.1928788903603238754E-12, 
    -1.4831904935994647675E-09, 
    -1.3466829827635152875E-06, 
    -9.1746443287817501309E-04, 
    -4.7207090827310162436E-01, 
    -1.8225946631657315931E+02, 
    -5.1894091982308017540E+04, 
    -1.0588550724769347106E+07, 
    -1.4828267606612366099E+09, 
    -1.3357437682275493024E+11, 
    -6.9876779648010090070E+12, 
    -1.7732037840791591320E+14, 
    -1.4577180278143463643E+15 };
  double pbar = 3.98437500E-01;
  double pp[8] = { 
    -6.0437159056137600000E-02, 
     4.5748122901933459000E-01, 
    -4.2843766903304806403E-01, 
     9.7356000150886612134E-02, 
    -3.2457723974465568321E-03, 
    -3.6395264712121795296E-04, 
     1.6258661867440836395E-05, 
    -3.6347578404608223492E-07 };
  double q[5] = { 
    -4.0076864679904189921E+03, 
     7.4810580356655069138E+06, 
    -8.0059518998619764991E+09, 
     4.8544714258273622913E+12, 
    -1.3218168307321442305E+15 };
  double qq[6] = {
    -3.8806586721556593450E+00, 
     3.2593714889036996297E+00, 
    -8.5017476463217924408E-01, 
     7.4212010813186530069E-02, 
    -2.2835624489492512649E-03, 
     3.7510433111922824643E-05 };
  double rec15 = 6.6666666666666666666E-02;
  double sump;
  double sumq;
  double two25 = 225.0E+00;
  double value;
  double x;
  double xmax = 713.987E+00;
  double xx;
  double zero = 0.0E+00;
//
  x = fabs ( arg );
//
//  ABS(ARG) < EPSILON ( ARG )
//
  if ( x < d_epsilon ( ) )
  {
    value = half * x;
  }
//
//  EPSILON ( ARG ) <= ABS(ARG) < 15.0
//
  else if ( x < one5 )
  {
    xx = x * x;
    sump = p[0];
    for ( j = 1; j < 15; j++ )
    {
      sump = sump * xx + p[j];
    }

    xx = xx - two25;

    sumq = (((( 
          xx + q[0] 
      ) * xx + q[1] 
      ) * xx + q[2] 
      ) * xx + q[3] 
      ) * xx + q[4];

    value = ( sump / sumq ) * x;
  }
  else if ( xmax < x )
  {
    value = d_huge ( );
  }
//
//  15.0 <= ABS(ARG)
//
  else
  {
    xx = one / x - rec15;

    sump = ((((((    
               pp[0] 
        * xx + pp[1] 
      ) * xx + pp[2] 
      ) * xx + pp[3] 
      ) * xx + pp[4] 
      ) * xx + pp[5] 
      ) * xx + pp[6] 
      ) * xx + pp[7];

    sumq = (((((
          xx + qq[0]
      ) * xx + qq[1]
      ) * xx + qq[2]
      ) * xx + qq[3]
      ) * xx + qq[4]
      ) * xx + qq[5];

    value = sump / sumq;

    if ( xmax - one5 < x )
    {
      a = exp ( x - forty );
      b = exp40;
    }
    else
    {
      a = exp ( x );
      b = one;
    }
    value = ( ( value * a + pbar * a ) / sqrt ( x ) ) * b;
  }

  if ( arg < zero )
  {
    value = -value;
  }

  return value;
}
//******************************************************************************

void bessel_i1_values ( int *n_data, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    BESSEL_I1_VALUES returns some values of the I1 Bessel function.
//
//  Discussion:
//
//    The modified Bessel functions In(Z) and Kn(Z) are solutions of
//    the differential equation
//
//      Z^2 W'' + Z * W' - ( Z^2 + N^2 ) * W = 0.
//
//    In Mathematica, the function can be evaluated by:
//
//      BesselI[1,x]
//
//  Modified:
//
//    20 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 20

  double fx_vec[N_MAX] = { 
     0.0000000000000000E+00,  
     0.1005008340281251E+00,  
     0.2040267557335706E+00,  
     0.3137040256049221E+00,  
     0.4328648026206398E+00,  
     0.5651591039924850E+00,  
     0.7146779415526431E+00,  
     0.8860919814143274E+00,  
     0.1084810635129880E+01,   
     0.1317167230391899E+01,  
     0.1590636854637329E+01,  
     0.2516716245288698E+01,  
     0.3953370217402609E+01,  
     0.6205834922258365E+01,  
     0.9759465153704450E+01,  
     0.1538922275373592E+02,  
     0.2433564214245053E+02,  
     0.6134193677764024E+02,  
     0.3998731367825601E+03,  
     0.2670988303701255E+04 };

  double x_vec[N_MAX] = { 
     0.00E+00,  
     0.20E+00,  
     0.40E+00,  
     0.60E+00,  
     0.80E+00,  
     0.10E+01,  
     0.12E+01,  
     0.14E+01,  
     0.16E+01,  
     0.18E+01,  
     0.20E+01,  
     0.25E+01,  
     0.30E+01,  
     0.35E+01,  
     0.40E+01,  
     0.45E+01,  
     0.50E+01,  
     0.60E+01,  
     0.80E+01,  
     0.10E+02 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double beta ( double x, double y )

//******************************************************************************
//
//  Purpose:
//
//    BETA returns the value of the Beta function.
//
//  Formula:
//
//    BETA(X,Y) = ( GAMMA(X) * GAMMA(Y) ) / GAMMA(X+Y)
//
//  Properties:
//
//    BETA(X,Y) = BETA(Y,X).
//    BETA(X,Y) = Integral ( 0 <= T <= 1 ) T**(X-1) (1-T)**(Y-1) dT.
//    BETA(X,Y) = GAMMA(X) * GAMMA(Y) / GAMMA(X+Y)
//
//  Restrictions:
//
//    Both X and Y must be greater than 0.
//
//  Modified:
//
//    09 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the two parameters that define the Beta function.
//    X and Y must be greater than 0.
//
//    Output, double BETA, the value of the Beta function.
//
{
  if ( x <= 0.0 || y <= 0.0 )
  {
    cout << "\n";
    cout << "BETA - Fatal error!\n";
    cout << "  Both X and Y must be greater than 0.\n";
    exit ( 1 );
  }

  return ( exp ( gamma_log ( x ) + gamma_log ( y ) - gamma_log ( x + y ) ) );
}
//******************************************************************************

double beta_binomial_cdf ( int x, double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_CDF evaluates the Beta Binomial CDF.
//
//  Discussion:
//
//    A simple summing approach is used.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the CDF.
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Output, double BETA_BINOMIAL_CDF, the value of the CDF.
//
{
  double cdf;
  double pdf;
  int y;

  if ( x < 0 )
  {
    cdf = 0.0;
  }
  else if ( x < c )
  {
    cdf = 0.0;
    for ( y = 0; y <= x; y++ )
    {
      pdf = beta ( a + ( double ) y, b + ( double ) ( c - y) ) 
        / ( ( double ) ( c + 1 ) 
         * beta ( ( double ) ( y + 1 ), ( double ) ( c - y + 1 ) ) 
         * beta ( a, b ) );
      cdf = cdf + pdf;
    }
  }
  else if ( c <= x )
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

int beta_binomial_cdf_inv ( double cdf, double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_CDF_INV inverts the Beta Binomial CDF.
//
//  Discussion:
//
//    A simple discrete approach is used.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Output, int BETA_BINOMIAL_CDF_INV, the smallest X whose cumulative 
//    density function is greater than or equal to CDF.
//
{
  double cum;
  double pdf;
  int x;
  int y;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "BETA_BINOMIAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cum = 0.0;

  for ( y = 0; y <= c; y++ )
  {
    pdf = beta ( a + ( double ) ( y ), 
      b + ( double ) ( c - y ) ) / ( ( double ) ( c + 1 ) 
      * beta ( ( double ) ( y + 1 ), 
      ( double ) ( c - y + 1 ) ) * beta ( a, b ) );

    cum = cum + pdf;

    if ( cdf <= cum )
    {
      x = y;
      return x;
    }

  }

  x = c;

  return x;
}
//******************************************************************************

bool beta_binomial_check ( double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_CHECK checks the parameters of the Beta Binomial PDF.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Output, bool BETA_BINOMIAL_CHECK, is TRUE if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_BINOMIAL_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_BINOMIAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c < 0 )
  {
    cout << "\n";
    cout << "BETA_BINOMIAL_CHECK - Fatal error!\n";
    cout << "  C < 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double beta_binomial_mean ( double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_MEAN returns the mean of the Beta Binomial PDF.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= N.
//
//    Output, double BETA_BINOMIAL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( double ) ( c ) * a / ( a + b );

  return mean;
}
//******************************************************************************

double beta_binomial_pdf ( int x, double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_PDF evaluates the Beta Binomial PDF.
//
//  Formula:
//
//    PDF(X)(A,B,C) = Beta(A+X,B+C-X) 
//      / ( (C+1) * Beta(X+1,C-X+1) * Beta(A,B) )  for 0 <= X <= C.
//
//    This PDF can be reformulated as:
//
//      The beta binomial probability density function for X successes
//      out of N trials is
//
//      PDF2(X)( N, MU, THETA ) =
//        C(N,X) * Product ( 0 <= R <= X - 1 ) ( MU + R * THETA )
//               * Product ( 0 <= R <= N - X - 1 ) ( 1 - MU + R * THETA )
//               / Product ( 0 <= R <= N - 1 )  ( 1 + R * THETA )
//
//      where 
//
//        C(N,X) is the combinatorial coefficient;
//        MU is the expectation of the underlying Beta distribution;
//        THETA is a shape parameter.  
//
//      A THETA value of 0 ( or A+B --> Infinity ) results in the binomial 
//      distribution:
//
//        PDF2(X) ( N, MU, 0 ) = C(N,X) * MU**X * ( 1 - MU )**(N-X)
//
//    Given A, B, C for PDF, then the equivalent PDF2 has:
//
//      N     = C
//      MU    = A / ( A + B )
//      THETA = 1 / ( A + B )
//
//    Given N, MU, THETA for PDF2, the equivalent PDF has:
//
//      A = MU / THETA
//      B = ( 1 - MU ) / THETA
//      C = N
//
//  Discussion:
//
//    BETA_BINOMIAL_PDF(X)(1,1,C) = UNIFORM_DISCRETE_PDF(X)(0,C-1)
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Output, double BETA_BINOMIAL_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0 )
  {

    pdf = 0.0;
  }
  else if ( x <= c )
  {
    pdf = beta ( a + ( double ) ( x ), b + ( double ) ( c - x ) ) 
      / ( ( double ) ( c + 1 ) 
      * beta ( ( double ) ( x + 1 ), 
      ( double ) ( c - x + 1 ) ) * beta ( a, b ) );
  }
  else if ( c < x )
  {
    pdf = 0.0;
  }

  return pdf;
}
//******************************************************************************

int beta_binomial_sample ( double a, double b, int c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_SAMPLE samples the Beta Binomial CDF.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int X, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = beta_binomial_cdf_inv ( cdf, a, b, c );

  return x;
}
//******************************************************************************

double beta_binomial_variance ( double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    BETA_BINOMIAL_VARIANCE returns the variance of the Beta Binomial PDF.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input, int C, a parameter of the PDF.
//    0 <= C.
//
//    Output, double BETA_BINOMIAL_VARIANCE, the variance of the PDF.
//
{
  double variance;
//
  variance = ( ( double ) ( c ) * a * b ) 
    * ( a + b + ( double ) ( c ) ) 
    / ( ( a + b ) * ( a + b ) * ( a + b + 1.0 ) );

  return variance;
}
//******************************************************************************

double beta_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_CDF evaluates the Beta CDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double BETA_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else if ( x <= 1.0 )
  {
    cdf = beta_inc ( a, b, x );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double beta_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_CDF_INV inverts the Beta CDF.
//
//  Modified:
//
//    22 October 2004
//
//  Reference:
//
//    Abernathy and Smith,
//    Algorithm 724,
//    ACM Transactions on Mathematical Software,
//    Volume 19, Number 4, December 1993, pages 481-483.
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double BETA_CDF_INV, the argument of the CDF.
//
{
# define MAXK 20

  double bcoeff;
  double cdf_x;
  double d[MAXK * (MAXK-1)];
  double error = 0.0001;
  double errapp = 0.01;
  int i;
  int j;
  int k;
  int loopct;
  double pdf_x;
  double q;
  double s1;
  double s2;
  double sum2;
  double t;
  double tail;
  double x;
  double xold;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "BETA_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }
//
//  Estimate the solution.
//
  x = a / ( a + b );

  xold = 0.0;
  loopct = 2;

  while ( errapp <= fabs ( ( x - xold ) / x ) && loopct != 0 )
  {
    xold = x;
    loopct = loopct - 1;
//
//  CDF_X = PROB { BETA(A,B) <= X }.
//  Q = ( CDF - CDF_X ) / PDF_X.
//
    cdf_x = beta_cdf ( x, a, b );

    pdf_x = beta_pdf ( x, a, b );

    q = ( cdf - cdf_x ) / pdf_x;
//
//  D(N,K) = C(N,K) * Q**(N+K-1) / (N-1)!
//
    t = 1.0 - x;
    s1 = q * ( b - 1.0 ) / t;
    s2 = q * ( 1.0 - a ) / x;
    d[2-1+0*MAXK] = s1 + s2;
    tail = d[2-1+0*MAXK] * q / 2.0;
    x = x + q + tail;

    k = 3;

    while ( error < fabs ( tail / x ) && k <= MAXK )
    {
//
//  Find D(2,K-2).
//
      s1 = q * ( ( double ) ( k ) - 2.0 ) * s1 / t;
      s2 = q * ( 2.0 - ( double ) ( k ) ) * s2 / x;
      d[2-1+(k-2)*MAXK] = s1 + s2;
//
//  Find D(3,K-3), D(4,K-4), D(5,K-5), ... , D(K-1,1).
//
      for ( i = 3; i <= k-1; i++ )
      {
        sum2 = d[2-1+0*MAXK] * d[i-2+(k-i)*MAXK];
        bcoeff = 1.0;

        for ( j = 1; j <= k - i; j++ )
        {
          bcoeff = ( bcoeff * ( double ) ( k - i - j + 1 ) ) 
            / ( double ) ( j );
          sum2 = sum2 + bcoeff * d[2-1+j*MAXK] * d[i-2+(k-i-j)*MAXK];
        }
        d[i-1+(k-i)*MAXK] = sum2 + d[i-2+(k-i+1)*MAXK] / ( double ) ( i - 1 );
      }
//
//  Compute D(K,0) and use it to expand the series.
//
      d[k-1+0*MAXK] = d[2-1+0*MAXK] * d[k-2+0*MAXK] + d[k-2+1*MAXK] 
        / ( double ) ( k - 1 );
      tail = d[k-1+0*MAXK] * q / ( double ) ( k );
      x = x + tail;
//
//  Check for divergence.
//
      if ( x <= 0.0 || 1.0 <= x )
      {
        cout << " \n";
        cout << "BETA_CDF_INV - Fatal error!\n";
        cout << "  The series has diverged.\n";
        x = -1.0;
        return x;
      }
      k = k + 1;
    }
  }
  return x;
# undef MAXK
}
//******************************************************************************

bool beta_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_CHECK checks the parameters of the Beta PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, bool BETA_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

void beta_cdf_values ( int *n_data, double *a, double *b, double *x, 
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    BETA_CDF_VALUES returns some values of the Beta CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = BetaDistribution [ a, b ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    02 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Karl Pearson,
//    Tables of the Incomplete Beta Function,
//    Cambridge University Press, 1968.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *A, B, the parameters of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double a_vec[N_MAX] = { 
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.10E+01,  
      0.20E+01,  
      0.30E+01,  
      0.40E+01,  
      0.50E+01 };

  double b_vec[N_MAX] = { 
      0.50E+00,  
      0.50E+00,  
      0.50E+00,  
      0.50E+00,  
      0.20E+01,  
      0.30E+01,  
      0.40E+01,  
      0.50E+01,  
      0.20E+01,  
      0.20E+01,  
      0.20E+01,  
      0.20E+01 };

  double fx_vec[N_MAX] = { 
     0.5131670194948620E-01,  
     0.1055728090000841E+00,  
     0.1633399734659245E+00,  
     0.2254033307585166E+00,  
     0.3600000000000000E+00,  
     0.4880000000000000E+00,  
     0.5904000000000000E+00,  
     0.6723200000000000E+00,  
     0.2160000000000000E+00,  
     0.8370000000000000E-01,  
     0.3078000000000000E-01,  
     0.1093500000000000E-01 };

  double x_vec[N_MAX] = { 
     0.10E+00,  
     0.20E+00,  
     0.30E+00,  
     0.40E+00,  
     0.20E+00,  
     0.20E+00,  
     0.20E+00,  
     0.20E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0.0;
    *b = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *b = b_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double beta_inc ( double a, double b, double x )

//******************************************************************************
//
//  Purpose:
//
//    BETA_INC returns the value of the incomplete Beta function.
//
//  Discussion:
//
//    This calculation requires an iteration.  In some cases, the iteration
//    may not converge rapidly, or may become inaccurate.
//
//  Formula:
//
//    BETA_INC(A,B,X)
//
//      =   Integral ( 0 <= T <= X ) T**(A-1) (1-T)**(B-1) dT
//        / Integral ( 0 <= T <= 1 ) T**(A-1) (1-T)**(B-1) dT
//
//      =   Integral ( 0 <= T <= X ) T**(A-1) (1-T)**(B-1) dT
//        / BETA(A,B)
//
//  Modified:
//
//    10 October 2004
//
//  Reference:
//
//    Majumder and Bhattacharjee,
//    Algorithm AS63,
//    Applied Statistics,
//    1973, volume 22, number 3.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the function.
//    0.0 < A,
//    0.0 < B.
//
//    Input, double X, the argument of the function.
//    Normally, 0.0 <= X <= 1.0.
//
//    Output, double BETA_INC, the value of the function.
//
{
  double cx;
  int i;
  int it;
  int it_max = 1000;
  bool indx;
  int ns;
  double pp;
  double psq;
  double qq;
  double rx;
  double temp;
  double term;
  double tol = 1.0E-07;
  double value;
  double xx;

  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_INC - Fatal error!\n";
    cout << "  A <= 0.\n";
    exit ( 1 );
  }

  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "BETA_INC - Fatal error!\n";
    cout << "  B <= 0.\n";
    exit ( 1 );
  }

  if ( x <= 0.0 )
  {
    value = 0.0;
    return value;
  }
  else if ( 1.0 <= x )
  {
    value = 1.0;
    return value;
  }
//
//  Change tail if necessary and determine S.
//
  psq = a + b;

  if ( a < ( a + b ) * x )
  {
    xx = 1.0 - x;
    cx = x;
    pp = b;
    qq = a;
    indx = true;
  }
  else
  {
    xx = x;
    cx = 1.0 - x;
    pp = a;
    qq = b;
    indx = false;
  }

  term = 1.0;
  i = 1;
  value = 1.0;

  ns = ( int ) ( qq + cx * ( a + b ) );
//
//  Use Soper's reduction formulas.
//
  rx = xx / cx;

  temp = qq - ( double ) i;
  if ( ns == 0 )
  {
    rx = xx;
  }

  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    if ( it_max < it )
    {
      cout << "\n";
      cout << "BETA_INC - Fatal error!\n";
      cout << "  Maximum number of iterations exceeded!\n";
      cout << "  IT_MAX = " << it_max << "\n";
      exit ( 1 );
    }

    term = term * temp * rx / ( pp + ( double ) ( i ) );
    value = value + term;
    temp = fabs ( term );

    if ( temp <= tol && temp <= tol * value )
    {
      break;
    }

    i = i + 1;
    ns = ns - 1;

    if ( 0 <= ns )
    {
      temp = qq - ( double ) i;
      if ( ns == 0 )
      {
        rx = xx;
      }
    }
    else
    {
      temp = psq;
      psq = psq + 1.0;
    }
  }
//
//  Finish calculation.
//
  value = value * exp ( pp * log ( xx ) 
    + ( qq - 1.0 ) * log ( cx ) ) / ( beta ( a, b ) * pp );

  if ( indx )
  {
    value = 1.0 - value;
  }

  return value;
}
//******************************************************************************

void beta_inc_values ( int *n_data, double *a, double *b, double *x, 
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    BETA_INC_VALUES returns some values of the incomplete Beta function.
//
//  Discussion:
//
//    The incomplete Beta function may be written
//
//      BETA_INC(A,B,X) = Integral (0 to X) T**(A-1) * (1-T)**(B-1) dT
//                      / Integral (0 to 1) T**(A-1) * (1-T)**(B-1) dT
//
//    Thus,
//
//      BETA_INC(A,B,0.0) = 0.0;
//      BETA_INC(A,B,1.0) = 1.0
//
//    In Mathematica, the function can be evaluated by:
//
//      BETA[X,A,B] / BETA[A,B]
//
//  Modified:
//
//    09 June 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Karl Pearson,
//    Tables of the Incomplete Beta Function,
//    Cambridge University Press, 1968.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *A, B, the parameters of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 30

  double a_vec[N_MAX] = { 
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      1.0E+00,  
      1.0E+00,  
      1.0E+00,  
      1.0E+00,  
      1.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      5.5E+00,  
     10.0E+00,  
     10.0E+00,  
     10.0E+00,  
     10.0E+00,  
     20.0E+00,  
     20.0E+00,  
     20.0E+00,  
     20.0E+00,  
     20.0E+00,  
     30.0E+00,  
     30.0E+00,  
     40.0E+00 };

  double b_vec[N_MAX] = { 
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      0.5E+00,  
      1.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      2.0E+00,  
      5.0E+00,  
      0.5E+00,  
      5.0E+00,  
      5.0E+00,  
     10.0E+00,  
      5.0E+00,  
     10.0E+00,  
     10.0E+00,  
     20.0E+00,  
     20.0E+00,  
     10.0E+00,  
     10.0E+00,  
     20.0E+00 };

  double fx_vec[N_MAX] = { 
     0.6376856085851985E-01,  
     0.2048327646991335E+00,  
     0.1000000000000000E+01,  
     0.0000000000000000E+00,  
     0.5012562893380045E-02,  
     0.5131670194948620E-01,  
     0.2928932188134525E+00,  
     0.5000000000000000E+00,  
     0.2800000000000000E-01,  
     0.1040000000000000E+00,  
     0.2160000000000000E+00,  
     0.3520000000000000E+00,  
     0.5000000000000000E+00,  
     0.6480000000000000E+00,  
     0.7840000000000000E+00,  
     0.8960000000000000E+00,  
     0.9720000000000000E+00,  
     0.4361908850559777E+00,  
     0.1516409096347099E+00,  
     0.8978271484375000E-01,  
     0.1000000000000000E+01,  
     0.5000000000000000E+00,  
     0.4598773297575791E+00,  
     0.2146816102371739E+00,  
     0.9507364826957875E+00,  
     0.5000000000000000E+00,  
     0.8979413687105918E+00,  
     0.2241297491808366E+00,  
     0.7586405487192086E+00,  
     0.7001783247477069E+00 };

  double x_vec[N_MAX] = { 
     0.01E+00,  
     0.10E+00,  
     1.00E+00,  
     0.00E+00,  
     0.01E+00,  
     0.10E+00,  
     0.50E+00,  
     0.50E+00,  
     0.10E+00,  
     0.20E+00,  
     0.30E+00,  
     0.40E+00,  
     0.50E+00,  
     0.60E+00,  
     0.70E+00,  
     0.80E+00,  
     0.90E+00,  
     0.50E+00,  
     0.90E+00,  
     0.50E+00,  
     1.00E+00,  
     0.50E+00,  
     0.80E+00,  
     0.60E+00,  
     0.80E+00,  
     0.50E+00,  
     0.60E+00,  
     0.70E+00,  
     0.80E+00,  
     0.70E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0.0;
    *b = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *b = b_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double beta_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_MEAN returns the mean of the Beta PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double BETA_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a / ( a + b );

  return mean;
}
//******************************************************************************

double beta_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_PDF evaluates the Beta PDF.
//
//  Formula:
//
//    PDF(A,B;X) = X**(A-1) * (1-X)**(B-1) / BETA(A,B).
//
//  Discussion:
//
//    A = B = 1 yields the Uniform distribution on [0,1].
//    A = B = 1/2 yields the Arcsin distribution.
//        B = 1 yields the power function distribution.
//    A = B -> Infinity tends to the Normal distribution.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double BETA_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 || 1.0 < x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = pow ( x, ( a - 1.0 ) ) * pow ( ( 1.0 - x ), ( b - 1.0 ) ) / beta ( a, b );
  }

  return pdf;
}
//******************************************************************************

double beta_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    BETA_SAMPLE samples the Beta PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    William Kennedy and James Gentle,
//    Algorithm BN,
//    Statistical Computing,
//    Dekker, 1980.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input/output, int SEED, a seed for the random number generator.
//
//    Output, double BETA_SAMPLE, a sample of the PDF.
//
{
  double mu;
  double stdev;
  double test;
  double u;
  double x;
  double y;

  mu = ( a - 1.0 ) / ( a + b - 2.0 );
  stdev = 0.5 / sqrt ( a + b - 2.0 );

  for ( ; ; )
  {
    y = normal_01_sample ( seed );

    x = mu + stdev * y;

    if ( x < 0.0 || 1.0 < x )
    {
      continue;
    }

    u = d_uniform_01 ( seed );

    test =     ( a - 1.0 )     * log (         x   / ( a - 1.0 ) ) 
             + ( b - 1.0 )     * log ( ( 1.0 - x ) / ( b - 1.0 ) ) 
             + ( a + b - 2.0 ) * log ( a + b - 2.0 ) + 0.5 * y * y;

    if ( log ( u ) <= test )
    {
      break;
    }

  }

  return x;
}
//******************************************************************************

double beta_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BETA_VARIANCE returns the variance of the Beta PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double BETA_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( a * b ) / ( ( a + b ) * ( a + b ) * ( 1.0 + a + b ) );

  return variance;
}
//******************************************************************************

double binomial_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_CDF evaluates the Binomial CDF.
//
//  Definition:
//
//    CDF(X)(A,B) is the probability of at most X successes in A trials,
//    given that the probability of success on a single trial is B.
//
//  Discussion:
//
//    A sequence of trials with fixed probability of success on
//    any trial is known as a sequence of Bernoulli trials.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the desired number of successes.
//    0 <= X <= A.
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, double CDF, the value of the CDF.
//
{
  int cnk;
  double cdf;
  int j;
  double pr;

  if ( x < 0 )
  {
    cdf = 0.0;
  }
  else if ( a <= x )
  {
    cdf = 1.0;
  }
  else if ( b == 0.0 )
  {
    cdf = 1.0;
  }
  else if ( b == 1.0 )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 0.0;

    for ( j = 0; j <= x; j++ )
    {
      cnk = binomial_coef ( a, j );

      pr = ( double ) ( cnk ) * pow ( b, j ) * pow ( ( 1.0 - b ), ( a - j ) );

      cdf = cdf + pr;

    }

  }

  return cdf;
}
//******************************************************************************

void binomial_cdf_values ( int *n_data, int *a, double *b, int *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_CDF_VALUES returns some values of the binomial CDF.
//
//  Discussion:
//
//    CDF(X)(A,B) is the probability of at most X successes in A trials,
//    given that the probability of success on a single trial is B.
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = BinomialDistribution [ n, p ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    15 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//    Daniel Zwillinger,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition, CRC Press, 1996, pages 651-652.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *A, a parameter of the function.
//
//    Output, double *B, a parameter of the function.
//
//    Output, int *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 17

  int a_vec[N_MAX] = { 
     2,  2,  2,  2, 
     2,  4,  4,  4, 
     4, 10, 10, 10, 
    10, 10, 10, 10, 
    10 };

  double b_vec[N_MAX] = { 
     0.05E+00,  
     0.05E+00,  
     0.05E+00,  
     0.50E+00,  
     0.50E+00,  
     0.25E+00,  
     0.25E+00,  
     0.25E+00,  
     0.25E+00,  
     0.05E+00,  
     0.10E+00,  
     0.15E+00,  
     0.20E+00,  
     0.25E+00,  
     0.30E+00,  
     0.40E+00,  
     0.50E+00  };

  double fx_vec[N_MAX] = { 
     0.9025000000000000E+00,  
     0.9975000000000000E+00,  
     0.1000000000000000E+01,  
     0.2500000000000000E+00,  
     0.7500000000000000E+00,  
     0.3164062500000000E+00,  
     0.7382812500000000E+00,  
     0.9492187500000000E+00,  
     0.9960937500000000E+00,  
     0.9999363101685547E+00,  
     0.9983650626000000E+00,  
     0.9901259090013672E+00,  
     0.9672065024000000E+00,  
     0.9218730926513672E+00,  
     0.8497316674000000E+00,  
     0.6331032576000000E+00,  
     0.3769531250000000E+00 };

  int x_vec[N_MAX] = { 
     0, 1, 2, 0, 
     1, 0, 1, 2, 
     3, 4, 4, 4, 
     4, 4, 4, 4, 
     4 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }
 
  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0;
    *b = 0.0;
    *x = 0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *b = b_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double binomial_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_CDF_INV inverts the Binomial CDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, int X, the corresponding argument.
//
{
  double cdf2;
  double pdf;
  int x;
  int x2;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "BINOMIAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cdf2 = 0.0;

  for ( x2 = 0; x2 <= a; x2++ )
  {
    pdf = binomial_pdf ( x2, a, b );

    cdf2 = cdf2 + pdf;

    if ( cdf <= cdf2 )
    {
      x = x2;
      return x;
    }

  }

  return x;
}
//******************************************************************************

bool binomial_check ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_CHECK checks the parameter of the Binomial PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, bool BINOMIAL_CHECK, is true if the parameters are legal.
//
{
  if ( a < 1 )
  {
    cout << "\n";
    cout << "BINOMIAL_CHECK - Fatal error!\n";
    cout << "  A < 1.\n";
    return false;
  }

  if ( b < 0.0 || 1.0 < b )
  {
    cout << "\n";
    cout << "BINOMIAL_CHECK - Fatal error!\n";
    cout << "  B < 0 or 1 < B.\n";
    return false;
  }

  return true;
}
//******************************************************************************

int binomial_coef ( int n, int k )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_COEF computes the Binomial coefficient C(N,K).
//
//  Discussion:
//
//    The value is calculated in such a way as to avoid overflow and
//    roundoff.  The calculation is done in integer arithmetic.
//
//  Formula:
//
//    CNK = C(N,K) = N! / ( K! * (N-K)! )
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    M L Wolfson and H V Wright,
//    Combinatorial of M Things Taken N at a Time,
//    ACM Algorithm 160,
//    Communications of the ACM,
//    April, 1963.
//
//  Parameters:
//
//    Input, int N, K, are the values of N and K.
//
//    Output, int CNK, the number of combinations of N
//    things taken K at a time.
//
{
  int cnk;
  int i;
  int mn;
  int mx;

  mn = i_min ( k, n-k );

  if ( mn < 0 )
  {
    cnk = 0;
  }
  else if ( mn == 0 )
  {
    cnk = 1;
  }
  else
  {
    mx = i_max ( k, n-k );
    cnk = mx + 1;

    for ( i = 2; i <= mn; i++ )
    {
      cnk = ( cnk * ( mx + i ) ) / i;
    }
  }

  return cnk;
}
//******************************************************************************

double binomial_coef_log ( int n, int k )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_COEF_LOG computes the logarithm of the Binomial coefficient.
//
//  Formula:
//
//    CNK_LOG = LOG ( C(N,K) ) = LOG ( N! / ( K! * (N-K)! ) ).
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, K, are the values of N and K.
//
//    Output, double CNK_LOG, the logarithm of C(N,K).
//
{
  double cnk_log;

  cnk_log = factorial_log ( n ) - factorial_log ( k ) - factorial_log ( n - k );

  return cnk_log;
}
//******************************************************************************

double binomial_mean ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_MEAN returns the mean of the Binomial PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, double BINOMIAL_MEAN, the expected value of the number of
//    successes in A trials.
//
{
  double mean;

  mean = ( double ) ( a ) * b;

  return mean;
}
//******************************************************************************

double binomial_pdf ( int x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_PDF evaluates the Binomial PDF.
//
//  Definition:
//
//    PDF(A,B;X) is the probability of exactly X successes in A trials,
//    given that the probability of success on a single trial is B.
//
//  Formula:
//
//    PDF(A,B;X) = C(N,X) * B**X * ( 1.0 - B )**( A - X )
//
//  Discussion:
//
//    Binomial_PDF(1,B;X) = Bernoulli_PDF(B;X).
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the desired number of successes.
//    0 <= X <= A.
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, double BINOMIAL_PDF, the value of the PDF.
//
{
  int cnk;
  double pdf;

  if ( a < 1 )
  {
    pdf = 0.0;
  }
  else if ( x < 0 || a < x )
  {
    pdf = 0.0;
  }
  else if ( b == 0.0 )
  {
    if ( x == 0 )
    {
      pdf = 1.0;
    }
    else
    {
      pdf = 0.0;
    }
  }
  else if ( b == 1.0 )
  {
    if ( x == a )
    {
      pdf = 1.0;
    }
    else
    {
      pdf = 0.0;
    }
  }
  else
  {
    cnk = binomial_coef ( a, x );

    pdf = ( double ) ( cnk ) * pow ( b, x ) * pow ( ( 1.0 - b ), ( a - x ) );
  }

  return pdf;
}
//******************************************************************************

int binomial_sample ( int a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_SAMPLE samples the Binomial PDF.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    William Kennedy and James Gentle,
//    Algorithm BU,
//    Statistical Computing,
//    Dekker, 1980.
//
//  Parameters:
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int BINOMIAL_SAMPLE, a sample of the PDF.
//
{
  int i;
  double u;
  int x;

  x = 0;

  for ( i = 1; i <= a; i++ )
  {
    u = d_uniform_01 ( seed );

    if ( u <= b )
    {
      x = x + 1;
    }

  }

  return x;
}
//******************************************************************************

double binomial_variance ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    BINOMIAL_VARIANCE returns the variance of the Binomial PDF.
//
//  Modified:
//
//    28 January 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//    1 <= A.
//
//    Input, double B, the probability of success on one trial.
//    0.0 <= B <= 1.0.
//
//    Output, double BINOMIAL_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( double ) ( a ) * b * ( 1.0 - b );

  return variance;
}
//*******************************************************************************

double bradford_cdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_CDF evaluates the Bradford CDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, double BRADFORD_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else if ( x <= b )
  {
    cdf = log ( 1.0 + c * ( x - a ) / ( b - a ) ) / log ( c + 1.0 );
  }
  else if ( b < x )
  {
    cdf = 1.0;
  }

  return cdf;
}
//*******************************************************************************

double bradford_cdf_inv ( double cdf, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_CDF_INV inverts the Bradford CDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, double BRADFORD_CDF_INV, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "BRADFORD_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf <= 0.0 )
  {
    x = a;
  }
  else if ( cdf < 1.0 )
  {
    x = a + ( b - a ) * ( pow ( ( c + 1.0 ), cdf ) - 1.0 ) / c;
  }
  else if ( 1.0 <= cdf )
  {
    x = b;
  }

  return x;
}
//*******************************************************************************

bool bradford_check ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_CHECK checks the parameters of the Bradford PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, bool BRADFORD_CHECK, is true if the parameters are legal.
//
{
  if ( b <= a )
  {
    cout << " \n";
    cout << "BRADFORD_CHECK - Fatal error!\n";
    cout << "  B <= A.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "BRADFORD_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double bradford_mean ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_MEAN returns the mean of the Bradford PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, double BRADFORD_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( c * ( b - a ) + log ( c + 1.0 ) * ( a * ( c + 1.0 ) - b ) ) 
    / ( c * log ( c + 1.0 ) );

  return mean;
}
//*******************************************************************************

double bradford_pdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_PDF evaluates the Bradford PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = 
//      C / ( ( C * ( X - A ) + B - A ) * log ( C + 1 ) )
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, double BRADFORD_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else if ( x <= b )
  {
    pdf = c / ( ( c * ( x - a ) + b - a ) * log ( c + 1.0 ) );
  }
  else if ( b < x )
  {
    pdf = 0.0;
  }

  return pdf;
}
//*******************************************************************************

double bradford_sample ( double a, double b, double c, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_SAMPLE samples the Bradford PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double BRADFORD_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = a + ( b - a ) * ( pow ( ( c + 1.0 ), cdf ) - 1.0 ) / c;

  return x;
}
//*******************************************************************************

double bradford_variance ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    BRADFORD_VARIANCE returns the variance of the Bradford PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A < B,
//    0.0 < C.
//
//    Output, double BRADFORD_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( b - a ) * ( b - a ) * 
    ( c * ( log ( c + 1.0 ) - 2.0 ) + 2.0 * log ( c + 1.0 ) ) 
    / ( 2.0 * c * pow ( ( log ( c + 1.0 ) ), 2 ) );

  return variance;
}
//*******************************************************************************

double burr_cdf ( double x, double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_CDF evaluates the Burr CDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double BURR_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 / pow ( ( 1.0 + pow ( ( b / ( x - a ) ), c )  ), d );
  }

  return cdf;
}
//*******************************************************************************

double burr_cdf_inv ( double cdf, double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_CDF_INV inverts the Burr CDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double BURR_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "BURR_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a + b / pow ( ( pow ( ( 1.0 / cdf ), (1.0 / d ) ) - 1.0 ), ( 1.0 / c ) );

  return x;
}
//*******************************************************************************

bool burr_check ( double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_CHECK checks the parameters of the Burr CDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, bool BURR_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "BURR_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c <= 0 )
  {
    cout << " \n";
    cout << "BURR_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double burr_mean ( double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
///   BURR_MEAN returns the mean of the Burr PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double BURR_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * gamma ( 1.0 - 1.0 / c ) * gamma ( d + 1.0 / c ) / gamma ( d );

  return mean;
}
//*******************************************************************************

double burr_pdf ( double x, double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_PDF evaluates the Burr PDF.
//
//  Formula:
//
//    PDF(A,B,C,D;X) = ( C * D / B ) * ( ( X - A ) / B )**( - C - 1 )
//      * ( 1 + ( ( X - A ) / B )**( - C ) )**( - D - 1 )
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    M E Johnson,
//    Multivariate Statistical Simulation,
//    Wiley, New York, 1987.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double BURR_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = ( c * d / b ) * pow ( y, ( - c - 1.0 ) ) 
      * pow ( ( 1.0 + pow ( y, ( - c ) ) ), ( - d - 1.0 ) );

  }

  return pdf;
}
//*******************************************************************************

double burr_sample ( double a, double b, double c, double d, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_SAMPLE samples the Burr PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double BURR_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = burr_cdf_inv ( cdf, a, b, c, d );

  return x;
}
//*******************************************************************************

double burr_variance ( double a, double b, double c, double d )

//*******************************************************************************
//
//  Purpose:
//
//    BURR_VARIANCE returns the variance of the Burr PDF.
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, D, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double BURR_VARIANCE, the variance of the PDF.
//
{
  double k;
  double variance;

  if ( c <= 2.0 )
  {
    cout << " \n";
    cout << "BURR_VARIANCE - Warning!\n";
    cout << "  Variance undefined for C <= 2.\n";
    variance = d_huge ( );
  }
  else
  {
    k = gamma ( d ) * gamma ( 1.0 - 2.0 / c ) * gamma ( d + 2.0 / c ) 
      - pow ( ( gamma ( 1.0 - 1.0 / c ) * gamma ( d + 1.0 / c ) ), 2 );
  
    variance = k * b * b / pow ( gamma ( d ), 2 );
  }

  return variance;
}
//*******************************************************************************

double cauchy_cdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_CDF evaluates the Cauchy CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;
  double y;

  y = ( x - a ) / b;

  cdf = 0.5 + atan ( y ) / PI;

  return cdf;
# undef PI
}
//*******************************************************************************

double cauchy_cdf_inv ( double cdf, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_CDF_INV inverts the Cauchy CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double X, the corresponding argument.
//
{
# define PI 3.141592653589793

  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "CAUCHY_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a + b * tan ( PI * ( cdf - 0.5 ) );

  return x;
# undef PI
}
//******************************************************************************

void cauchy_cdf_values ( int *n_data, double *mu, double *sigma, double *x,
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    CAUCHY_CDF_VALUES returns some values of the Cauchy CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = CauchyDistribution [ mu, sigma ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    05 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *SIGMA, the variance of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.8524163823495667E+00,  
     0.9220208696226307E+00,  
     0.9474315432887466E+00,  
     0.6475836176504333E+00,  
     0.6024163823495667E+00,  
     0.5779791303773693E+00,  
     0.5628329581890012E+00,  
     0.6475836176504333E+00,  
     0.5000000000000000E+00,  
     0.3524163823495667E+00,  
     0.2500000000000000E+00 };

  double mu_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double sigma_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *sigma = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *sigma = sigma_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool cauchy_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_CHECK checks the parameters of the Cauchy CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool CAUCHY_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "CAUCHY_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double cauchy_mean ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_MEAN returns the mean of the Cauchy PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//*******************************************************************************

double cauchy_pdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_PDF evaluates the Cauchy PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 1 / ( PI * B * ( 1 + ( ( X - A ) / B )**2 ) )
//
//  Discussion:
//
//    The Cauchy PDF is also known as the Breit-Wigner PDF.  It
//    has some unusual properties.  In particular, the integrals for the
//    expected value and higher order moments are "singular", in the
//    sense that the limiting values do not exist.  A result can be
//    obtained if the upper and lower limits of integration are set
//    equal to +T and -T, and the limit as T=>INFINITY is taken, but
//    this is a very weak and unreliable sort of limit.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  y = ( x - a ) / b;

  pdf = 1.0 / ( PI * b * ( 1.0 + y * y ) );

  return pdf;
# undef PI
}
//*******************************************************************************

double cauchy_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_SAMPLE samples the Cauchy PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = cauchy_cdf_inv ( cdf, a, b );

  return x;
}
//*******************************************************************************

double cauchy_variance ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CAUCHY_VARIANCE returns the variance of the Cauchy PDF.
//
//  Discussion:
//
//    The variance of the Cauchy PDF is not well defined.  This routine
//    is made available for completeness only, and simply returns
//    a "very large" number.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double VARIANCE, the mean of the PDF.
//
{
  double variance;

  variance = d_huge ( );

  return variance;
}
//*******************************************************************************

double chi_cdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_CDF evaluates the Chi CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  double p2;
  double x2;
  double y;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;
    x2 = 0.5 * y * y;
    p2 = 0.5 * c;

    cdf = gamma_inc ( p2, x2 );
  }

  return cdf;
}
//*******************************************************************************

double chi_cdf_inv ( double cdf, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_CDF_INV inverts the Chi CDF.
//
//  Discussion:
//
//    A simple bisection method is used.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double X, the corresponding argument of the CDF.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "CHI_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = a;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
    return x;
  }

  x1 = a;
  cdf1 = 0.0;

  x2 = a + 1.0;

  for ( ; ; )
  {
    cdf2 = chi_cdf ( x2, a, b, c );

    if ( cdf < cdf2 )
    {
      break;
    }

    x2 = a + 2.0 * ( x2 - a );
  }
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = chi_cdf ( x3, a, b, c );

    if ( abs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "CHI_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      return x;
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }

  }

  return x;
}
//*******************************************************************************

bool chi_check ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_CHECK checks the parameters of the Chi CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, bool CHI_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "CHI_CHECK - Fatal error!\n";
    cout << "  B <= 0.0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "CHI_CHECK - Fatal error!\n";
    cout << "  C <= 0.0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double chi_mean ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_MEAN returns the mean of the Chi PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double MEAN, the mean value.
//
{
  double mean;

  mean = a + sqrt ( 2.0 ) * b * gamma ( 0.5 * ( c + 1.0 ) ) / gamma ( 0.5 * c ); 

  return mean;
}
//*******************************************************************************

double chi_pdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_PDF evaluates the Chi PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = EXP ( - 0.5 * ( ( X - A ) / B )**2 ) 
//      * ( ( X - A ) / B )**( C - 1 ) /
//      ( 2**( 0.5 * C - 1 ) * B * GAMMA ( 0.5 * C ) )
//      
//  Discussion:
//
//    CHI(A,B,1) is the Half Normal PDF;
//    CHI(0,B,2) is the Rayleigh PDF;
//    CHI(0,B,3) is the Maxwell PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = exp ( - 0.5 * y * y ) * pow ( y, ( c - 1.0 ) ) / 
      ( pow ( 2.0, ( 0.5 * c - 1.0 ) ) * b * gamma ( 0.5 * c ) );
  }

  return pdf;
}
//*******************************************************************************

double chi_sample ( double a, double b, double c, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SAMPLE samples the Chi PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double x;

  x = chi_square_sample ( c, seed );

  x = a + b * sqrt ( x );

  return x;
}
//*******************************************************************************

double chi_variance ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_VARIANCE returns the variance of the Chi PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b * ( c - 2.0 * 
    pow ( ( gamma ( 0.5 * ( c + 1.0 ) ) / gamma ( 0.5 * c ) ), 2 ) );

  return variance;
}
//*******************************************************************************

double chi_square_cdf ( double x, double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_CDF evaluates the Chi squared CDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the value of the random deviate.
//
//    Input, double A, the parameter of the distribution, usually
//    the number of degrees of freedom.
//
//    Output, double CDF, the value of the CDF.
//
{
  double a2;
  double b2;
  double c2;
  double cdf;
  double x2;

  x2 = 0.5 * x;
  a2 = 0.0;
  b2 = 1.0;
  c2 = 0.5 * a;

  cdf = gamma_cdf ( x2, a2, b2, c2 );

  return cdf;
}
//*******************************************************************************

double chi_square_cdf_inv ( double cdf, double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_CDF_INV inverts the Chi squared PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Reference:
//
//    Best and Roberts,
//    The Percentage Points of the Chi-Squared Distribution,
//    Algorithm AS 91,
//    Applied Statistics,
//    Volume 24, Number ?, pages 385-390, 1975.
//
//  Parameters:
//
//    Input, double CDF, a value of the chi-squared cumulative
//    probability density function.
//    0.000002 <= CDF <= 0.999998.
//
//    Input, double A, the parameter of the chi-squared 
//    probability density function.  0 < A.
//
//    Output, double X, the value of the chi-squared random deviate
//    with the property that the probability that a chi-squared random
//    deviate with parameter A is less than or equal to X is CDF.
//
{
  double a2;
  double aa = 0.6931471806;
  double b;
  double c;
  double c1 = 0.01;
  double c2 = 0.222222;
  double c3 = 0.32;
  double c4 = 0.4;
  double c5 = 1.24;
  double c6 = 2.2;
  double c7 = 4.67;
  double c8 = 6.66;
  double c9 = 6.73;
  double c10 = 13.32;
  double c11 = 60.0;
  double c12 = 70.0;
  double c13 = 84.0;
  double c14 = 105.0;
  double c15 = 120.0;
  double c16 = 127.0;
  double c17 = 140.0;
  double c18 = 175.0;
  double c19 = 210.0;
  double c20 = 252.0;
  double c21 = 264.0;
  double c22 = 294.0;
  double c23 = 346.0;
  double c24 = 420.0;
  double c25 = 462.0;
  double c26 = 606.0;
  double c27 = 672.0;
  double c28 = 707.0;
  double c29 = 735.0;
  double c30 = 889.0;
  double c31 = 932.0;
  double c32 = 966.0;
  double c33 = 1141.0;
  double c34 = 1182.0;
  double c35 = 1278.0;
  double c36 = 1740.0;
  double c37 = 2520.0;
  double c38 = 5040.0;
  double cdf_max = 0.999998;
  double cdf_min = 0.000002;
  double ch;
  double e = 0.0000005;
  double g;
  int i;
  int it_max = 20;
  double p1;
  double p2;
  double q;
  double s1;
  double s2;
  double s3;
  double s4;
  double s5;
  double s6;
  double t;
  double x;
  double x2;
  double xx;
//
  if ( cdf < cdf_min )
  {
    x = -1.0;
    cout << " \n";
    cout << "CHI_SQUARE_CDF_INV - Fatal error!\n";
    cout << "  CDF < CDF_MIN.\n";
    exit ( 1 );
  }

  if ( cdf_max < cdf )
  {
    x = -1.0;
    cout << " \n";
    cout << "CHI_SQUARE_CDF_INV - Fatal error!\n";
    cout << "  CDF_MAX < CDF.\n";
    exit ( 1 );
  }

  xx = 0.5 * a;
  c = xx - 1.0;
//
//  Compute Log ( Gamma ( A/2 ) ).
//
  g = gamma_log ( a / 2.0 );
//
//  Starting approximation for small chi-squared.
//
  if ( a < -c5 * log ( cdf ) )
  {
    ch = pow ( ( cdf * xx * exp ( g + xx * aa ) ), ( 1.0 / xx ) );

    if ( ch < e )
    {
      x = ch;
      return x;
    }
  }
//
//  Starting approximation for A less than or equal to 0.32.
//
  else if ( a <= c3 )
  {
    ch = c4;
    a2 = log ( 1.0 - cdf );

    for ( ; ; )
    {
      q = ch;
      p1 = 1.0 + ch * ( c7 + ch );
      p2 = ch * ( c9 + ch * ( c8 + ch ) );

      t = - 0.5 + ( c7 + 2.0 * ch ) / p1 - ( c9 + ch * ( c10 + 3.0 * ch ) ) / p2;

      ch = ch - ( 1.0 - exp ( a2 + g + 0.5 * ch + c * aa ) * p2 / p1 ) / t;

      if ( fabs ( q / ch - 1.0 ) <= c1 )
      {
        break;
      }
    }
  }
//
//  Call to algorithm AS 111.
//  Note that P has been tested above.
//  AS 241 could be used as an alternative.
//
  else
  {
    x2 = normal_01_cdf_inv ( cdf );
//
//  Starting approximation using Wilson and Hilferty estimate.
//
    p1 = c2 / a;
    ch = a * pow ( ( x2 * sqrt ( p1 ) + 1.0 - p1 ), 3 );
//
//  Starting approximation for P tending to 1.
//
    if ( c6 * a + 6.0 < ch )
    {
      ch = - 2.0 * ( log ( 1.0 - cdf ) - c * log ( 0.5 * ch ) + g );
    }
  }
//
//  Call to algorithm AS 239 and calculation of seven term Taylor series.
//
  for ( i = 1; i <= it_max; i++ )
  {
    q = ch;
    p1 = 0.5 * ch;
    p2 = cdf - gamma_inc ( xx, p1 );
    t = p2 * exp ( xx * aa + g + p1 - c * log ( ch ) );
    b = t / ch;
    a2 = 0.5 * t - b * c;

    s1 = ( c19 + a2 
       * ( c17 + a2 
       * ( c14 + a2 
       * ( c13 + a2 
       * ( c12 + a2 
       *   c11 ) ) ) ) ) / c24;

    s2 = ( c24 + a2 
       * ( c29 + a2 
       * ( c32 + a2 
       * ( c33 + a2 
       *   c35 ) ) ) ) / c37;

    s3 = ( c19 + a2 
       * ( c25 + a2 
       * ( c28 + a2 
       *   c31 ) ) ) / c37;

    s4 = ( c20 + a2 
       * ( c27 + a2 
       *   c34 ) + c 
       * ( c22 + a2 
       * ( c30 + a2 
       *   c36 ) ) ) / c38;

    s5 = ( c13 + c21 * a2 + c * ( c18 + c26 * a2 ) ) / c37;

    s6 = ( c15 + c * ( c23 + c16 * c ) ) / c38;

    ch = ch + t * ( 1.0 + 0.5 * t * s1 - b * c 
      * ( s1 - b 
      * ( s2 - b 
      * ( s3 - b 
      * ( s4 - b 
      * ( s5 - b 
      *   s6 ) ) ) ) ) );

    if ( e < fabs ( q / ch - 1.0 ) )
    {
      x = ch;
      return x;
    }

  }

  x = ch;
  cout << " \n";
  cout << "CHI_SQUARE_CDF_INV - Warning!\n";
  cout << "  Convergence not reached.\n";

  return x;
}
//******************************************************************************

void chi_square_cdf_values ( int *n_data, int *a, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_CDF_VALUES returns some values of the Chi-Square CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = ChiSquareDistribution [ df ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    09 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *A, the parameter of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
//
# define N_MAX 21

  int a_vec[N_MAX] = { 
     1,  2,  1,  2, 
     1,  2,  3,  4, 
     1,  2,  3,  4, 
     5,  3,  3,  3, 
     3,  3, 10, 10, 
    10 };

  double fx_vec[N_MAX] = { 
     0.7965567455405796E-01,  
     0.4987520807317687E-02,   
     0.1124629160182849E+00,  
     0.9950166250831946E-02,  
     0.4729107431344619E+00,   
     0.1812692469220181E+00,   
     0.5975750516063926E-01,   
     0.1752309630642177E-01,   
     0.6826894921370859E+00,   
     0.3934693402873666E+00,   
     0.1987480430987992E+00,   
     0.9020401043104986E-01,   
     0.3743422675270363E-01,   
     0.4275932955291202E+00,   
     0.6083748237289110E+00,   
     0.7385358700508894E+00,   
     0.8282028557032669E+00,   
     0.8883897749052874E+00,   
     0.1721156299558408E-03,   
     0.3659846827343712E-02,   
     0.1857593622214067E-01 };

  double x_vec[N_MAX] = { 
     0.01E+00,   
     0.01E+00,    
     0.02E+00,   
     0.02E+00,   
     0.40E+00,   
     0.40E+00,   
     0.40E+00,   
     0.40E+00,   
     1.00E+00,   
     1.00E+00,   
     1.00E+00,   
     1.00E+00,   
     1.00E+00,   
     2.00E+00,   
     3.00E+00,   
     4.00E+00,   
     5.00E+00,   
     6.00E+00,   
     1.00E+00,   
     2.00E+00,   
     3.00E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool chi_square_check ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_CHECK checks the parameter of the central Chi squared PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the distribution.
//    1 <= A.
//
//    Output, bool CHI_SQUARE_CHECK, is true if the parameters are legal.
//
{
  if ( a < 1.0 )
  {
    cout << " \n";
    cout << "CHI_SQUARE_CHECK - Fatal error!\n";
    cout << "  A < 1.0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double chi_square_mean ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_MEAN returns the mean of the central Chi squared PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the distribution.
//    1 <= A.
//
//    Output, double MEAN, the mean value.
//
{
  double mean;

  mean = a;

  return mean;
}
//*******************************************************************************

double chi_square_pdf ( double x, double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_PDF evaluates the central Chi squared PDF.
//
//  Formula:
//
//    PDF(A;X) = 
//      EXP ( - X / 2 ) * X**((A-2)/2) / ( 2**(A/2) * GAMMA ( A/2 ) )
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X 
//
//    Input, double A, the parameter of the PDF.
//    1 <= A.
//
//    Output, double PDF, the value of the PDF.
//
{
  double b;
  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    b = a / 2.0;
    pdf = exp ( -0.5 * x ) * pow ( x, ( b - 1.0 ) ) / ( pow ( 2.0, b ) * gamma ( b ) );
  }

  return pdf;
}
//*******************************************************************************

double chi_square_sample ( double a, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_SAMPLE samples the central Chi squared PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1 <= A.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double a2;
  double b2;
  double c2;
  int i;
  int it_max = 100;
  int n;
  double x;
  double x2;

  n = ( int ) a;

  if ( ( double ) ( n ) == a && n <= it_max )
  {
    x = 0.0;
    for ( i = 1; i <= n; i++ )
    {
      x2 = normal_01_sample ( seed );
      x = x + x2 * x2;
    }
  }
  else
  {
    a2 = 0.0;
    b2 = 1.0;
    c2 = a / 2.0;

    x = gamma_sample ( a2, b2, c2, seed );

    x = 2.0 * x;
  }

  return x;
}
//*******************************************************************************

double chi_square_variance ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_VARIANCE returns the variance of the central Chi squared PDF.
//
//  Modified:
//
//    12 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the distribution.
//    1 <= A.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 2.0 * a;

  return variance;
}
//******************************************************************************

void chi_square_noncentral_cdf_values ( int *n_data, int *df, double *lambda, 
  double *x, double *cdf )

//******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_NONCENTRAL_CDF_VALUES returns values of the noncentral chi CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = NoncentralChiSquareDistribution [ df, lambda ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    30 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *DF, the number of degrees of freedom.
//
//    Output, double *LAMBDA, the noncentrality parameter.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *CDF, the noncentral chi CDF.
//
{
# define N_MAX 28

  double cdf_vec[N_MAX] = { 
     0.8399444269398261E+00,  
     0.6959060300435139E+00,  
     0.5350879697078847E+00,  
     0.7647841496310313E+00,  
     0.6206436532195436E+00,  
     0.4691667375373180E+00,  
     0.3070884345937569E+00,  
     0.2203818092990903E+00,  
     0.1500251895581519E+00,  
     0.3071163194335791E-02,  
     0.1763982670131894E-02,  
     0.9816792594625022E-03,  
     0.1651753140866208E-01,  
     0.2023419573950451E-03,  
     0.4984476352854074E-06,  
     0.1513252400654827E-01,  
     0.2090414910614367E-02,  
     0.2465021206048452E-03,  
     0.2636835050342939E-01,  
     0.1857983220079215E-01,  
     0.1305736595486640E-01,  
     0.5838039534819351E-01,  
     0.4249784402463712E-01,  
     0.3082137716021596E-01,  
     0.1057878223400849E+00,  
     0.7940842984598509E-01,  
     0.5932010895599639E-01,  
     0.2110395656918684E+00 };
 
  int df_vec[N_MAX] = { 
      1,   2,   3, 
      1,   2,   3, 
      1,   2,   3, 
      1,   2,   3, 
     60,  80, 100, 
      1,   2,   3, 
     10,  10,  10, 
     10,  10,  10, 
     10,  10,  10, 
      8 };

  double lambda_vec[N_MAX] = { 
     0.5E+00,  
     0.5E+00,  
     0.5E+00,  
     1.0E+00,  
     1.0E+00,  
     1.0E+00,  
     5.0E+00,  
     5.0E+00,  
     5.0E+00,  
    20.0E+00,  
    20.0E+00,  
    20.0E+00,  
    30.0E+00,  
    30.0E+00,  
    30.0E+00,  
     5.0E+00,  
     5.0E+00,  
     5.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00,  
     0.5E+00 };

  double x_vec[N_MAX] = { 
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
      3.000E+00,  
     60.000E+00,  
     60.000E+00,  
     60.000E+00,  
      0.050E+00,  
      0.050E+00,  
      0.050E+00,  
      4.000E+00,  
      4.000E+00,  
      4.000E+00,  
      5.000E+00,  
      5.000E+00,  
      5.000E+00,  
      6.000E+00,  
      6.000E+00,  
      6.000E+00,  
      5.000E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *lambda = 0.0;
    *df = 0;
    *cdf = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *lambda = lambda_vec[*n_data-1];
    *df = df_vec[*n_data-1];
    *cdf = cdf_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

double chi_square_noncentral_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_NONCENTRAL_CHECK checks the parameters of the noncentral Chi Squared PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the parameter of the PDF.
//    1.0 <= A.
//
//    Input, double B, the noncentrality parameter of the PDF.
//    0.0 <= B.
//
//    Output, bool CHI_SQUARE_NONCENTRAL_CHECK, is true if the parameters
//    are legal.
//
{
  if ( a < 1.0 )
  {
    cout << " \n";
    cout << "CHI_SQUARE_NONCENTRAL_CHECK - Fatal error!\n";
    cout << "  A < 1.\n";
    return false;
  }

  if ( b < 0.0 )
  {
    cout << " \n";
    cout << "CHI_SQUARE_NONCENTRAL_CHECK - Fatal error!\n";
    cout << "  B < 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double chi_square_noncentral_mean ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_NONCENTRAL_MEAN returns the mean of the noncentral Chi squared PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the parameter of the PDF.
//    1.0 <= A.
//
//    Input, double B, the noncentrality parameter of the PDF.
//    0.0 <= B.
//
//    Output, double MEAN, the mean value.
//
{
  double mean;

  mean = a + b;

  return mean;
}
//*******************************************************************************

double chi_square_noncentral_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_NONCENTRAL_SAMPLE samples the noncentral Chi squared PDF.
//
//  Modified:
//
//    30 November 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the parameter of the PDF.
//    1.0 <= A.
//
//    Input, double B, the noncentrality parameter of the PDF.
//    0.0 <= B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double a1;
  double a2;
  double b2;
  double x;
  double x1;
  double x2;

  a1 = a - 1.0;

  x1 = chi_square_sample ( a1, seed );

  a2 = sqrt ( b );
  b2 = 1.0;
  x2 = normal_sample ( a2, b2, seed );

  x = x1 + x2 * x2;

  return x;
}
//*******************************************************************************

double chi_square_noncentral_variance ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    CHI_SQUARE_NONCENTRAL_VARIANCE returns the variance of the noncentral Chi squared PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1 <= A.
//
//    Input, double B, the noncentrality parameter of the PDF.
//    0.0 <= B.
//
//    Output, double VARIANCE, the variance value.
//
{
  double variance;

  variance = 2.0 * ( a + 2.0 * b );

  return variance;
}
//*******************************************************************************

double *circle_sample ( double a, double b, double c, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CIRCLE_SAMPLE samples points from a circle.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the circle.
//    The circle is centered at (A,B) and has radius C.
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double CIRCLE_SAMPLE[2], a sampled point of the circle.
//
{
# define PI 3.141592653589793

  double angle;
  double radius_frac;
  double *x;

  x = new double[2];

  radius_frac = sqrt ( d_uniform_01 ( seed ) );

  angle = 2.0 * PI * d_uniform_01 ( seed );

  x[0] = a + c * radius_frac * cos ( angle );
  x[1] = b + c * radius_frac * sin ( angle );

  return x;

# undef PI
}
//*******************************************************************************

double *circular_normal_01_mean ( void )

//*******************************************************************************
//
//  Purpose:
//
//    CIRCULAR_NORMAL_01_MEAN returns the mean of the Circular Normal 01 PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double CIRCULAR_01_MEAN[2], the mean of the PDF.
//
{
  double *mean;

  mean = new double[2];

  mean[0] = 0.0;
  mean[1] = 0.0;

  return mean;
}
//*******************************************************************************

double circular_normal_01_pdf ( double x[2] )

//*******************************************************************************
//
//  Purpose:
//
//    CIRCULAR_NORMAL_01_PDF evaluates the Circular Normal 01 PDF.
//
//  Formula:
//
//    PDF(X) = EXP ( - 0.5 * ( X(1)**2 + X(2)**2 ) ) / ( 2 * PI )
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X[2], the argument of the PDF.
//
//    Output, double CIRCULAR_NORMAL_01_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  pdf = exp ( - 0.5 * ( x[0] * x[0] + x[1] * x[1] ) ) / ( 2.0 * PI );

  return pdf;
# undef PI
}
//*******************************************************************************

double *circular_normal_01_sample ( int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    CIRCULAR_NORMAL_01_SAMPLE samples the Circular Normal 01 PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double CIRCULAR_NORMAL_01_SAMPLE[2], a sample of the PDF.
//
{
# define PI 3.141592653589793

  double v1;
  double v2;
  double *x;

  x = new double[2];

  v1 = d_uniform_01 ( seed );
  v2 = d_uniform_01 ( seed );

  x[0] = sqrt ( - 2.0 * log ( v1 ) ) * cos ( 2.0 * PI * v2 );
  x[1] = sqrt ( - 2.0 * log ( v1 ) ) * sin ( 2.0 * PI * v2 );

  return x;

# undef PI
}
//*******************************************************************************

double *circular_normal_01_variance ( void )

//*******************************************************************************
//
//  Purpose:
//
//    CIRCULAR_NORMAL_01_VARIANCE returns the variance of the Circular Normal 01 PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double CIRCULAR_NORMAL_01_VARIANCE[2], the variance of the PDF.
//
{
//
  double *variance;

  variance = new double[2];

  variance[0] = 1.0;
  variance[1] = 1.0;

  return variance;
}
//*******************************************************************************

double cosine_cdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_CDF evaluates the Cosine CDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, double CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;
  double y;

  if ( x <= a - PI * b )
  {
    cdf = 0.0;
  }
  else if ( x <= a + PI * b )
  {
    y = ( x - a ) / b;

    cdf = 0.5 + ( y + sin ( y ) ) / ( 2.0 * PI );
  }
  else if ( a + PI * b < x )
  {
    cdf = 1.0;
  }

  return cdf;
# undef PI
}
//*******************************************************************************

double cosine_cdf_inv ( double cdf, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_CDF_INV inverts the Cosine CDF.
//
//  Discussion:
//
//    A simple bisection method is used on the interval 
//    [ A - PI * B, A + PI * B ].
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double X, the corresponding argument of the CDF.
//
{
# define PI 3.141592653589793

  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "COSINE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = a - PI * b;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = a + PI * b;
    return x;
  }

  x1 = a - PI * b;
  cdf1 = 0.0;

  x2 = a + PI * b;
  cdf2 = 1.0;
//
//  Now use bisection.
//
  it = 0;

  for ( it = 1; it <= it_max; it++ )
  {
    x3 = 0.5 * ( x1 + x2 );
    cdf3 = cosine_cdf ( x3, a, b );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      return x;
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }

  }

  cout << " \n";
  cout << "COSINE_CDF_INV - Fatal error!\n";
  cout << "  Iteration limit exceeded.\n";

  exit ( 1 );
  return x;
# undef PI
}
//*******************************************************************************

bool cosine_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_CHECK checks the parameters of the Cosine CDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, bool COSINE_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "COSINE_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double cosine_mean ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_MEAN returns the mean of the Cosine PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//*******************************************************************************

double cosine_pdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_PDF evaluates the Cosine PDF.
//
//  Formula:
//
//    PDF(A,B;X) = ( 1 / ( 2 * PI * B ) ) * COS ( ( X - A ) / B )
//    for A - PI * B <= X <= A + PI * B
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  if ( x < a - PI * b )
  {
    pdf = 0.0;
  }
  else if ( x <= a + PI * b )
  {
    y = ( x - a ) / b;

    pdf = 1.0 / ( 2.0 * PI * b ) * cos ( y );
  }
  else if ( a + PI * b < x )
  {
    pdf = 0.0;
  }

  return pdf;
# undef PI
}
//*******************************************************************************

double cosine_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_SAMPLE samples the Cosine PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = cosine_cdf_inv ( cdf, a, b );

  return x;
}
//*******************************************************************************

double cosine_variance ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    COSINE_VARIANCE returns the variance of the Cosine PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = ( PI * PI / 3.0 - 2.0 ) * b * b;

  return variance;
# undef PI
}
//*******************************************************************************

double coupon_mean ( int j, int n )

//*******************************************************************************
//
//  Purpose:
//
//    COUPON_MEAN returns the mean of the Coupon PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int J, the number of distinct coupons to be collected.
//    J must be between 1 and N.
//
//    Input, int N, the number of distinct coupons.
//
//    Output, double COUPON_MEAN, the mean number of coupons that 
//    must be collected in order to just get J distinct kinds.
//
{
  int i;
  double mean;

  if ( n < j )
  {
    cout << " \n";
    cout << "COUPON_MEAN - Fatal error!\n";
    cout << "  Number of distinct coupons desired must be no more\n";
    cout << "  than the total number of distinct coupons.\n";
    exit ( 1 );
  }

  mean = 0.0;

  for ( i = 1; i <= j; i++ )
  {
    mean = mean + 1.0 / ( double ) ( n - i + 1 );
  }
  mean = mean * ( double ) ( n );

  return mean;
}
//*******************************************************************************

void coupon_simulate ( int n_type, int *seed, int coupon[], int *n_coupon )

//*******************************************************************************
//
//  Purpose:
//
//    COUPON_SIMULATE simulates the coupon collector's problem.
//
//  Discussion:
//
//    The coupon collector needs to collect one of each of N_TYPE
//    coupons.  The collector may draw one coupon on each trial,
//    and takes as many trials as necessary to complete the task.
//    On each trial, the probability of picking any particular type
//    of coupon is always 1 / N_TYPE.
//
//    The most interesting question is, what is the expected number
//    of drawings necessary to complete the collection?
//    how does this number vary as N_TYPE increases?  What is the
//    distribution of the numbers of each type of coupon in a typical 
//    collection when it is just completed?
//
//    As N increases, the number of coupons necessary to be 
//    collected in order to get a complete set in any simulation 
//    strongly tends to the value N_TYPE * LOG ( N_TYPE ).
//
//    If N_TYPE is 1, the simulation ends with a single drawing.
//
//    If N_TYPE is 2, then we may call the coupon taken on the first drawing 
//    a "Head", say, and the process then is similar to the question of the 
//    length, plus one, of a run of Heads or Tails in coin flipping.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N_TYPE, the number of types of coupons.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int COUPON[N_TYPE], the number of coupons of each type
//    that were collected during the simulation.
//
//    Output, int *N_COUPON, the total number of coupons collected.
//
{
  int i;
  int max_coupon = 2000;
  int straight;

  for ( i = 0; i < n_type; i++ )
  {
    coupon[i] = 0;
  }

  straight = 0;
  *n_coupon = 0;
//
//  Draw another coupon.
//
  while ( *n_coupon < max_coupon )
  {
    i = i_random ( 1, n_type, seed );
//
//  Increment the number of I coupons.
// 
    coupon[i-1] = coupon[i-1] + 1;
    *n_coupon = *n_coupon + 1;
//
//  If I is the next one we needed, increase STRAIGHT by 1.
//
    if ( i == straight + 1 )
    {
      for ( ; ; )
      {
        straight = straight + 1;
//
//  If STRAIGHT = N_TYPE, we have all of them.
//
        if ( n_type <= straight )
        {
          return;
        }
//
//  If the next coupon has not been collected, our straight is over.
//
        if ( coupon[straight] <= 0 )
        {
          break;
        }
      }
    }
  }

  cout << " \n";
  cout << "COUPON_SIMULATE - Fatal error!\n";
  cout << "  Maximum number of coupons drawn without success.\n";

  exit ( 1 );
}
//*******************************************************************************

double coupon_variance ( int j, int n )

//*******************************************************************************
//
//  Purpose:
//
//    COUPON_VARIANCE returns the variance of the Coupon PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int J, the number of distinct coupons to be collected.
//    J must be between 1 and N.
//
//    Input, int N, the number of distinct coupons.
//
//    Output, double COUPON_VARIANCE, the variance of the number of
//    coupons that must be collected in order to just get J distinct kinds.
//
{
  int i;
  double variance;

  if ( n < j )
  {
    cout << " \n";
    cout << "COUPON_VARIANCE - Fatal error!\n";
    cout << "  Number of distinct coupons desired must be no more\n";
    cout << "  than the total number of distinct coupons.\n";
    exit ( 1 );
  }

  variance = 0.0;
  for ( i = 1; i <= j; i++ )
  {
    variance = variance + ( double ) ( i - 1 ) / 
      pow ( double ( n - i + 1 ), 2 );
  }
  variance = variance * ( double ) ( n );

  return variance;
}
//*******************************************************************************

double csc ( double theta )

//*******************************************************************************
//
//  Purpose:
//
//    CSC returns the cosecant of X.
//
//  Definition:
//
//    CSC ( THETA ) = 1.0 / SIN ( THETA )
//
//  Discussion:
//
//    CSC is not a built-in function in FORTRAN, and occasionally it
//    is handier, or more concise, to be able to refer to it directly
//    rather than through its definition in terms of the sine function.
//
//  Modified:
//
//    01 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double THETA, the angle, in radians, whose cosecant is desired.
//    It must be the case that SIN ( THETA ) is not zero.
//
//    Output, double CSC, the cosecant of THETA.
//
{
  double value;

  value = sin ( theta );

  if ( value == 0.0 )
  {
    cout << " \n";
    cout << "CSC - Fatal error!\n";
    cout << "  CSC undefined for THETA = " << theta << "\n";
    exit ( 1 );
  }

  value = 1.0 / value;

  return value;
}
//******************************************************************************

double d_epsilon ( void )

//******************************************************************************
//
//  Purpose:
//
//    D_EPSILON returns the round off unit for floating arithmetic.
//
//  Discussion:
//
//    D_EPSILON is a number R which is a power of 2 with the property that,
//    to the precision of the computer's arithmetic,
//      1 < 1 + R
//    but 
//      1 = ( 1 + R / 2 )
//
//  Modified:
//
//    06 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double D_EPSILON, the floating point round-off unit.
//
{
  double r;

  r = 1.0;

  while ( 1.0 < ( double ) ( 1.0 + r )  )
  {
    r = r / 2.0;
  }

  return ( 2.0 * r );
}
//**********************************************************************

double d_factorial ( int n )

//**********************************************************************
//
//  Purpose:
//
//    D_FACTORIAL returns N!.
//
//  Definition:
//
//    N! = Product ( 1 <= I <= N ) I
//
//  Modified:
//
//    02 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the argument of the factorial function.
//    0 <= N.
//
//    Output, double D_FACTORIAL, the factorial of N.
//
{
  double fact;
  int i;
//
//  Check.
//
  if ( n < 0 )
  {
    cout << "\n";
    cout << "D_FACTORIAL - Fatal error!\n";
    cout << "  N < 0.\n";
    return (0.0);
  }

  fact = 1.0;

  for ( i = 2; i <= n; i++ )
  {
    fact = fact * ( double ) i;
  }

  return fact;
}
//******************************************************************************

double d_huge ( void )

//******************************************************************************
//
//  Purpose:
//
//    D_HUGE returns a "huge" real value, usually the largest legal real.
//
//  Discussion:
//
//    HUGE_VAL is the largest representable legal real number, and is usually
//    defined in math.h, or sometimes in stdlib.h.
//
//  Modified:
//
//    08 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double D_HUGE, a "huge" real value.
//
{
  return HUGE_VAL;
}
//*********************************************************************

double d_max ( double x, double y )

//*********************************************************************
//
//  Purpose:
//
//    D_MAX returns the maximum of two double precision values.
//
//  Modified:
//
//    18 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the quantities to compare.
//
//    Output, double D_MAX, the maximum of X and Y.
//
{
  if ( y < x )
  {
    return x;
  } 
  else
  {
    return y;
  }
}
//*********************************************************************

double d_min ( double x, double y )

//*********************************************************************
//
//  Purpose:
//
//    D_MIN returns the minimum of two double precision values.
//
//  Modified:
//
//    31 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the quantities to compare.
//
//    Output, double D_MIN, the minimum of X and Y.
//
{
  if ( y < x )
  {
    return y;
  } 
  else
  {
    return x;
  }
}
//******************************************************************************

double d_modp ( double x, double y )

//******************************************************************************
//
//  Purpose:
//
//    D_MODP returns the nonnegative remainder of real division.
//
//  Formula:
//
//    If
//      REM = D_MODP ( X, Y )
//      RMULT = ( X - REM ) / Y
//    then
//      X = Y * RMULT + REM
//    where REM is always nonnegative.
//
//  Comments:
//
//    The MOD function computes a result with the same sign as the
//    quantity being divided.  Thus, suppose you had an angle A,
//    and you wanted to ensure that it was between 0 and 360.
//    Then mod(A,360.0) would do, if A was positive, but if A
//    was negative, your result would be between -360 and 0.
//
//    On the other hand, D_MODP(A,360.0) is between 0 and 360, always.
//
//  Examples:
//
//        I         J     MOD  D_MODP   D_MODP Factorization
//
//      107        50       7       7    107 =  2 *  50 + 7
//      107       -50       7       7    107 = -2 * -50 + 7
//     -107        50      -7      43   -107 = -3 *  50 + 43
//     -107       -50      -7      43   -107 =  3 * -50 + 43
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the number to be divided.
//
//    Input, double Y, the number that divides X.
//
//    Output, double D_MODP, the nonnegative remainder when X is divided by Y.
//
{
  double value;

  if ( y == 0.0E+00 )
  {
    cout << "\n";
    cout << "D_MODP - Fatal error!\n";
    cout << "  D_MODP ( X, Y ) called with Y = " << y << "\n";
    exit ( 1 );
  }

  value = x - ( ( double ) ( ( int ) ( x / y ) ) ) * y;

  if ( value < 0.0E+00 )
  {
    value = value + fabs ( y );
  }

  return value;
}
//******************************************************************************

int d_nint ( double x )

//******************************************************************************
//
//  Purpose:
//
//    D_NINT returns the nearest integer to a double precision value.
//
//  Examples:
//
//        X         D_NINT
//
//      1.3         1
//      1.4         1
//      1.5         1 or 2
//      1.6         2
//      0.0         0
//     -0.7        -1
//     -1.1        -1
//     -1.6        -2
//
//  Modified:
//
//    26 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the value.
//
//    Output, int D_NINT, the nearest integer to X.
//
{
  int s;

  if ( x < 0.0 )
  {
    s = -1;
  }
  else
  {
    s = 1;
  }

  return ( s * ( int ) ( fabs ( x ) + 0.5 ) );
}
//******************************************************************************

double d_pi ( void )

//******************************************************************************
//
//  Purpose:
//
//    D_PI returns the value of PI.
//
//  Modified:
//
//    06 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double D_PI, the value of PI.
//
{
  return 3.14159265358979323;
}
//****************************************************************************

double d_random ( double rlo, double rhi, int *seed )

//****************************************************************************
//
//  Purpose:
//
//    D_RANDOM returns a random real in a given range.
//
//  Modified:
//
//    04 February 2000
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double RLO, RHI, the minimum and maximum values.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double D_RANDOM, the randomly chosen value.
//
{
  double t;

  t = d_uniform_01 ( seed );

  return ( 1.0 - t ) * rlo + t * rhi;
}
//******************************************************************************

int d_roundup ( double r )

//******************************************************************************
//
//  Purpose:
//
//    D_ROUNDUP rounds a real value "up" to the nearest integer.
//
//  Examples:
//
//    R     D_ROUNDUP
//
//   -1.1  -1
//   -1.0  -1
//   -0.9   0
//    0.0   0
//    5.0   5
//    5.1   6
//    5.9   6
//    6.0   6
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double R, the real value to be rounded up.
//
//    Output, int D_ROUNDUP, the rounded value.
//
{
  int value;

  value = int ( r );

  if ( ( double ) value < r )
  {
    value = value + 1;
  }
  
  return value;
}
//*********************************************************************

double d_sign ( double x )

//*********************************************************************
//
//  Purpose:
//
//    D_SIGN returns the sign of a double precision number.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the number whose sign is desired.
//
//    Output, double D_SIGN, the sign of X.
//
{
  if ( x < 0.0E+00 )
  {
    return ( -1.0E+00 );
  } 
  else
  {
    return ( 1.0E+00 );
  }
}
//******************************************************************************

double d_uniform_01 ( int *seed )

//******************************************************************************
//
//  Purpose:
//
//    D_UNIFORM_01 is a portable pseudorandom number generator.
//
//  Discussion:
//
//    This routine implements the recursion
//
//      seed = 16807 * seed mod ( 2**31 - 1 )
//      unif = seed / ( 2**31 - 1 )
//
//    The integer arithmetic never requires more than 32 bits,
//    including a sign bit.
//
//  Modified:
//
//    11 August 2004
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, L E Schrage,
//    A Guide to Simulation,
//    Springer Verlag, pages 201-202, 1983.
//
//    Bennett Fox,
//    Algorithm 647:
//    Implementation and Relative Efficiency of Quasirandom
//    Sequence Generators,
//    ACM Transactions on Mathematical Software,
//    Volume 12, Number 4, pages 362-376, 1986.
//
//  Parameters:
//
//    Input/output, int *SEED, the "seed" value.  Normally, this
//    value should not be 0.  On output, SEED has been updated.
//
//    Output, double D_UNIFORM_01, a new pseudorandom variate, strictly between
//    0 and 1.
//
{
  int k;
  double r;

  k = *seed / 127773;

  *seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

  if ( *seed < 0 )
  {
    *seed = *seed + 2147483647;
  }
//
//  Although SEED can be represented exactly as a 32 bit integer,
//  it generally cannot be represented exactly as a 32 bit real number!
//
  r = ( double ) ( *seed ) * 4.656612875E-10;

  return r;
}
//*******************************************************************************

double deranged_cdf ( int x, int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_CDF evaluates the Deranged CDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the maximum number of items in their correct places.
//    0 <= X <= A.
//
//    Input, int A, the number of items.
//    1 <= A.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  int cnk;
  int dnmk;
  int nfact;
  int sum2;
  int x2;

  if ( x < 0 || a < x )
  {
    cdf = 0.0;
  }
  else
  {
    sum2 = 0;
    for ( x2 = 0; x2 <= x; x2++ )
    {
      cnk = binomial_coef ( a, x2 );
      dnmk = deranged_enum ( a - x2 );
      sum2 = sum2 + cnk * dnmk;
    }
    nfact = i_factorial ( a );
    cdf = ( double ) ( sum2 ) / ( double ) ( nfact );
  }

  return cdf;
}
//*******************************************************************************

int deranged_cdf_inv ( double cdf, int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_CDF_INV inverts the Deranged CDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, int A, the number of items.
//    1 <= A.
//
//    Output, int X, the corresponding argument.
//
{
  double cdf2;
  double pdf;
  int x;
  int x2;
//
  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "DERANGED_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cdf2 = 0.0;

  for ( x2 = 0; x2 <= a; x2++ )
  {
    pdf = deranged_pdf ( x2, a );

    cdf2 = cdf2 + pdf;

    if ( cdf <= cdf2 )
    {
      x = x2;
      return x;
    }

  }

  x = a;

  return x;
}
//*******************************************************************************

bool deranged_check ( int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_CHECK checks the parameter of the Deranged PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the total number of items.
//    1 <= A.
//
//    Output, bool DERANGED_CHECK, is true if the parameters are legal.
//
{
  if ( a < 1 )
  {
    cout << " \n";
    cout << "DERANGED_CHECK - Fatal error!\n";
    cout << "  A < 1.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

int deranged_enum ( int n )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_ENUM returns the number of derangements of N objects.
//
//  Definition:
//
//    A derangement of N objects is a permutation with no fixed
//    points.  If we symbolize the permutation operation by "P",
//    then for a derangment, P(I) is never equal to I.
//
//  Recursion:
//
//      D(0) = 1
//      D(1) = 0
//      D(2) = 1
//      D(N) = (N-1) * ( D(N-1) + D(N-2) )
//
//    or
//
//      D(0) = 1
//      D(1) = 0
//      D(N) = N * D(N-1) + (-1)**N
//
//  Formula:
//
//    D(N) = N! * ( 1 - 1/1! + 1/2! - 1/3! ... 1/N! )
//
//    Based on the inclusion/exclusion law.
//
//  Comments:
//
//    D(N) is the number of ways of placing N non-attacking rooks on 
//    an N by N chessboard with one diagonal deleted.
//
//    Limit ( N -> Infinity ) D(N)/N! = 1 / e.
//
//    The number of permutations with exactly K items in the right
//    place is COMB(N,K) * D(N-K).
//
//  First values:
//
//     N         D(N)
//     0           1
//     1           0
//     2           1
//     3           2
//     4           9
//     5          44
//     6         265
//     7        1854
//     8       14833
//     9      133496
//    10     1334961
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of objects to be permuted.
//
//    Output, int DERANGED_ENUM, the number of derangements of N objects.
//
{
  int dn;
  int dnm1;
  int dnm2;
  int i;

  if ( n < 0 )
  {
    dn = 0;
  }
  else if ( n == 0 )
  {
    dn = 1;
  }
  else if ( n == 1 )
  {
    dn = 0;
  }
  else if ( n == 2 )
  {
    dn = 1;
  }
  else
  {
    dnm1 = 0;
    dn = 1;
    
    for ( i = 3; i <= n; i++ )
    {
      dnm2 = dnm1;
      dnm1 = dn;
      dn = ( i - 1 ) * ( dnm1 + dnm2 );
    }
          
  }
  
  return dn;
}
//*******************************************************************************

double deranged_mean ( int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_MEAN returns the mean of the Deranged CDF.
//
//  Discussion:
//
//    The mean is computed by straightforward summation.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of items.
//    1 <= A.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;
  double pdf;
  int x;

  mean = 0.0;
  for ( x = 1; x <= a; x++ )
  {
    pdf = deranged_pdf ( x, a );
    mean = mean + pdf * x;
  }

  return mean;
}
//*******************************************************************************

double deranged_pdf ( int x, int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_PDF evaluates the Deranged PDF.
//
//  Definition:
//
//    PDF(A;X) is the probability that exactly X items will occur in
//    their proper place after a random permutation of A items.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the number of items in their correct places.
//    0 <= X <= A.
//
//    Input, int A, the total number of items.
//    1 <= A.
//
//    Output, double PDF, the value of the PDF.
//
{
  int cnk;
  int dnmk;
  int nfact;
  double pdf;

  if ( x < 0 || a < x )
  {
    pdf = 0.0;
  }
  else
  {
    cnk = binomial_coef ( a, x );
    dnmk = deranged_enum ( a-x );
    nfact = i_factorial ( a );
    pdf = ( double ) ( cnk * dnmk ) / ( double ) ( nfact );
  }

  return pdf;
}
//*******************************************************************************

int deranged_sample ( int a, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_SAMPLE samples the Deranged PDF.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of items.
//    1 <= A.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int X, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = deranged_cdf_inv ( cdf, a );

  return x;
}
//*******************************************************************************

double deranged_variance ( int a )

//*******************************************************************************
//
//  Purpose:
//
//    DERANGED_VARIANCE returns the variance of the Deranged CDF.
//
//  Discussion:
//
//    The variance is computed by straightforward summation.
//
//  Modified:
//
//    13 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of items.
//    1 <= A.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double mean;
  double pdf;
  int x;
  double variance;

  mean = deranged_mean ( a );

  variance = 0.0;
  for ( x = 1; x <= a; x++ )
  {
    pdf = deranged_pdf ( x, a );
    variance = variance + pdf * pow ( ( x - mean ), 2 );
  }

  return variance;
}
//*******************************************************************************

double digamma ( double x )

//*******************************************************************************
//
//  Purpose:
//
//    DIGAMMA calculates the digamma or Psi function.
//
//  Discussion:
//
//    DiGamma ( X ) = d ( log ( Gamma ( X ) ) ) / dX
//
//  Modified:
//
//    14 October 2004
//
//  Reference:
//
//    J Bernardo,
//    Psi ( Digamma ) Function,
//    Algorithm AS 103,
//    Applied Statistics,
//    Volume 25, Number 3, pages 315-317, 1976.
//
//  Parameters:
//
//    Input, double X, the argument of the digamma function.
//    0 < X.
//
//    Output, double DIGAMMA, the value of the digamma function at X.
//
{
  double c = 8.5;
  double d1 = -0.5772156649;
  double r;
  double s = 0.00001;
  double s3 = 0.08333333333;
  double s4 = 0.0083333333333;
  double s5 = 0.003968253968;
  double value;
  double y;
//
//  The argument must be positive.
//
  if ( x <= 0.0 )
  {
    value = 0.0;
    cout << " \n";
    cout << "DIGAMMA - Fatal error!\n";
    cout << "  X <= 0.\n";
    exit ( 1 );
  }
//
//  Use approximation if argument <= S.
//
  else if ( x <= s )
  {
    value = d1 - 1.0 / x;
  }
//
//  Reduce the argument to DIGAMMA(X + N) where C <= (X + N).
//
  else
  {
    value = 0.0;
    y = x;

    while ( y < c )
    {
      value = value - 1.0 / y;
      y = y + 1.0;
    }
//
//  Use Stirling's (actually de Moivre's) expansion if C < argument.
//
    r = 1.0 / ( y * y );
    value = value + log ( y ) - 0.5 / y - r * ( s3 - r * ( s4 - r * s5 ) );
  }

  return value;
}
//*******************************************************************************

double dipole_cdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    DIPOLE_CDF evaluates the Dipole CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A is arbitrary, but represents an angle, so only 0 <= A <= 2 * PI 
//    is interesting, and -1.0 <= B <= 1.0.
//
//    Output, double DIPOLE_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;

  cdf = 0.5 + ( 1.0 / PI ) * atan ( x ) + b * b * ( x * cos ( 2.0 * a ) 
    - sin ( 2.0 * a ) ) / ( PI * ( 1.0 + x * x ) );

  return cdf;
# undef PI
}
//*******************************************************************************

double dipole_cdf_inv ( double cdf, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    DIPOLE_CDF_INV inverts the Dipole CDF.
//
//  Discussion:
//
//    A simple bisection method is used.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    -1.0 <= B <= 1.0.
//
//    Output, double DIPOLE_CDF_INV, the corresponding argument of the CDF.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "DIPOLE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = - d_huge ( );
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
    return x;
  }
//
//  Seek X1 < X < X2.
//
  x1 = - 1.0;

  for ( ; ; )
  {
    cdf1 = dipole_cdf ( x1, a, b );

    if ( cdf1 <= cdf )
    {
      break;
    }
    x1 = 2.0 * x1;
  }

  x2 = 1.0;

  for ( ; ; )
  {
    cdf2 = dipole_cdf ( x2, a, b );

    if ( cdf <= cdf2 )
    {
      break;
    }
    x2 = 2.0 * x2;
  }
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = dipole_cdf ( x3, a, b );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "DIPOLE_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      exit ( 1 );
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }

  }

  return x;
}
//*******************************************************************************

bool dipole_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    DIPOLE_CHECK checks the parameters of the Dipole CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A is arbitrary, but represents an angle, so only 0 <= A <= 2 * PI 
//    is interesting, and -1.0 <= B <= 1.0.
//
//    Output, bool DIPOLE_CHECK, is true if the parameters are legal.
//
{
  if ( b < -1.0 || 1.0 < b )
  {
    cout << " \n";
    cout << "DIPOLE_CHECK - Fatal error!\n";
    cout << "  -1.0 <= B <= 1.0 is required.\n";
    cout << "  The input B = " << b << "\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double dipole_pdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    DIPOLE_PDF evaluates the Dipole PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 
//        1 / ( PI * ( 1 + X**2 ) )
//      + B**2 * ( ( 1 - X**2 ) * cos ( 2 * A ) + 2.0 * X * sin ( 2 * A ) )
//      / ( PI * ( 1 + X )**2 )
//
//  Discussion:
//
//    Densities of this kind commonly occur in the analysis of resonant
//    scattering of elementary particles.
//
//    DIPOLE_PDF(A,0;X) = CAUCHY_PDF(A;X)
//    A = 0, B = 1 yields the single channel dipole distribution.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Robert Knop,
//    Algorithm 441,
//    ACM Transactions on Mathematical Software.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A is arbitrary, but represents an angle, so only 0 <= A <= 2 * PI 
//      is interesting,
//    and -1.0 <= B <= 1.0.
//
//    Output, double DIPOLE_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  pdf = 1.0 / ( PI * ( 1.0 + x * x ) ) 
    + b * b * ( ( 1.0 - x * x ) * cos ( 2.0 * a )
    + 2.0 * x * sin ( 2.0 * x ) ) 
    / ( PI * ( 1.0 + x * x ) * ( 1.0 + x * x ) );

  return pdf;
# undef PI
}
//*******************************************************************************

double dipole_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    DIPOLE_SAMPLE samples the Dipole PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Robert Knop,
//    Algorithm 441,
//    ACM Transactions on Mathematical Software.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A is arbitrary, but represents an angle, so only 0 <= A <= 2 * PI 
//      is interesting,
//    and -1.0 <= B <= 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double a2;
  double b2;
  double c2;
  double x;
  double *xc;
//
//  Find (X1,X2) at random in a circle.
//
  a2 = b * sin ( a );
  b2 = b * cos ( a );
  c2 = 1.0;

  xc = circle_sample ( a2, b2, c2, seed );
//
//  The dipole variate is the ratio X1 / X2.
//
  x = xc[0] / xc[1];

  delete [] xc;

  return x;
}
//*******************************************************************************

bool dirichlet_check ( int n, double a[] )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_CHECK checks the parameters of the Dirichlet PDF.
//
//  Modified:
//
//    30 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components.
//
//    Input, double A[N], the probabilities for each component.
//    Each A[I] should be positive.
//
//    Output, bool DIRICHLET_CHECK, is true if the parameters are legal.
//
{
  int i;
  bool positive;

  positive = false;

  for ( i = 0; i < n; i++ )
  {
    if ( a[i] <= 0.0 )
    {
      cout << " \n";
      cout << "DIRICHLET_CHECK - Fatal error!\n";
      cout << "  A[" << i << "] <= 0.\n";
      cout << "  A[" << i << "] = " << a[i] << ".\n";
      return false;
    }
    else if ( 0.0 < a[i] )
    {
      positive = true;
    }
  }

  if ( !positive )
  {
    cout << " \n";
    cout << "DIRICHLET_CHECK - Fatal error!\n";
    cout << "  All parameters are zero!\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double *dirichlet_mean ( int n, double a[] )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MEAN returns the means of the Dirichlet PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components.
//
//    Input, double A[N], the probabilities for each component.
//    Each A[I] should be positive.
//
//    Output, double DIRICHLET_MEAN[N], the means of the PDF.
//
{
  int i;
  double *mean;

  mean = new double[n];

  for ( i = 0; i < n; i++ )
  {
    mean[i] = a[i];
  }

  dvec_unit_sum ( n, mean );

  return mean;
}
//******************************************************************************

double *dirichlet_moment2 ( int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MOMENT2 returns the second moments of the Dirichlet PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components.
//
//    Input, double A[N], the probabilities for each component.
//    Each A(I) should be positive.
//
//    Output, double DIRICHLET_MOMENT[N*N], the second moments of the PDF.
//
{
  double a_sum;
  double *m2;
  int i;
  int j;

  m2 = new double[n*n];

  a_sum = dvec_sum ( n, a );

  for ( i = 0; i < n; i++ )
  {
    for ( j = 0; j < n; j++ )
    {
      if ( i == j )
      {
        m2[i+j*n] = a[i] * ( a[i] + 1.0 ) / ( a_sum * ( a_sum + 1.0 ) );
      }
      else
      {
        m2[i+j*n] = a[i] * a[j] / ( a_sum * ( a_sum + 1.0 ) );
      }
    }
  }

  return m2;
}
//******************************************************************************

double dirichlet_pdf ( double x[], int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_PDF evaluates the Dirichlet PDF.
//
//  Definition:
//
//    PDF(N,A;X) = Product ( 1 <= I <= N ) X(I)**( A(I) - 1 ) 
//      * Gamma ( A_SUM ) / A_PROD
//
//    where 
//
//      0 < A(I) for all I;
//      0 <= X(I) for all I;
//      Sum ( 1 <= I <= N ) X(I) = 1;
//      A_SUM = Sum ( 1 <= I <= N ) A(I).
//      A_PROD = Product ( 1 <= I <= N ) Gamma ( A(I) )
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X(N), the argument of the PDF.  Each X(I) should
//    be greater than 0.0, and the X(I)'s must add up to 1.0.
//
//    Input, int N, the number of components.
//
//    Input, double A(N), the probabilities for each component.
//    Each A(I) should be positive.
//
//    Output, double PDF, the value of the PDF.
//
{
  double a_prod;
  double a_sum;
  int i;
  double pdf;
  double tol = 0.0001;
  double x_sum;

  for ( i = 0; i < n; i++ )
  {
    if ( x[i] <= 0.0 )
    {
      cout << " \n";
      cout << "DIRICHLET_PDF - Fatal error!\n";
      cout << "  X(I) <= 0.\n";
      exit ( 1 );
    }
  }

  x_sum = dvec_sum ( n, x );

  if ( tol < fabs ( x_sum - 1.0 ) )
  {
    cout << " \n";
    cout << "DIRICHLET_PDF - Fatal error!\n";
    cout << "  SUM X(I) =/= 1.\n";
    exit ( 1 );
  }

  a_sum = dvec_sum ( n, a );

  a_prod = 1.0;
  for ( i = 0; i < n; i++ )
  {
    a_prod = a_prod * gamma ( a[i] );
  }

  pdf = gamma ( a_sum ) / a_prod;

  for ( i = 0; i < n; i++ )
  {
    pdf = pdf * pow ( x[i], a[i] - 1.0 );
  }

  return pdf;
}
//******************************************************************************

double *dirichlet_sample ( int n, double a[], int *seed )

//******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_SAMPLE samples the Dirichlet PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 169.
//
//  Parameters:
//
//    Input, int N, the number of components.
//
//    Input, double A(N), the probabilities for each component.
//    Each A(I) should be positive.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double DIRICHLET_SAMPLE(N), a sample of the PDF.  The entries 
//    of X should sum to 1.
//
{
  double a2;
  double b2;
  double c2;
  int i;
  double *x;

  x = new double[n];

  a2 = 0.0;
  b2 = 1.0;

  for ( i = 0; i < n; i++ )
  {
    c2 = a[i];
    x[i] = gamma_sample ( a2, b2, c2, seed );
  }
//
//  Rescale the vector to have unit sum.
//
  dvec_unit_sum ( n, x );

  return x;
}
//******************************************************************************

double *dirichlet_variance ( int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_VARIANCE returns the variances of the Dirichlet PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components.
//
//    Input, double A(N), the probabilities for each component.
//    Each A(I) should be positive.
//
//    Output, double DIRICHLET_VARIANCE(N), the variances of the PDF.
//
{
  double a_sum;
  int i;
  double *variance;

  variance = new double[n];

  a_sum = dvec_sum ( n, a );

  for ( i = 0; i < n; i++ )
  {
    variance[i] = a[i] * ( a_sum - a[i] ) / ( a_sum * a_sum * ( a_sum + 1.0 ) );
  }

  return variance;
}
//*******************************************************************************

bool dirichlet_mix_check ( int comp_num, int elem_num, double a[], 
  double comp_weight[] )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MIX_CHECK checks the parameters of a Dirichlet mixture PDF.
//
//  Modified:
//
//    30 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int COMP_NUM, the number of components in the Dirichlet
//    mixture density, that is, the number of distinct Dirichlet PDF's
//    that are mixed together.
//
//    Input, int ELEM_NUM, the number of elements of an observation.
//
//    Input, double A[ELEM_NUM*COMP_NUM], the probabilities 
//    for element ELEM_NUM in component COMP_NUM.
//    Each A[I,J] should be positive.
//
//    Input, double COMP_WEIGHT[COMP_NUM], the mixture weights of the densities.
//    These do not need to be normalized.  The weight of a given component is
//    the relative probability that that component will be used to generate
//    the sample.
//
//    Output, bool DIRICHLET_MIX_CHECK, is true if the parameters are legal.
//
{
  int comp_i;
  int elem_i;
  bool positive;

  for ( comp_i = 0; comp_i < comp_num; comp_i++ )
  {
    for ( elem_i = 0; elem_i < elem_num; elem_i++ )
    {
      if ( a[elem_i+comp_i*elem_num] <= 0.0 )
      {
        cout << " \n";
        cout << "DIRICHLET_MIX_CHECK - Fatal error!\n";
        cout << "  A(ELEM,COMP) <= 0.\n";
        cout << "  COMP = " << comp_i << "\n";
        cout << "  ELEM = " << elem_i << "\n";
        cout << "  A(COMP,ELEM) = " << a[elem_i+comp_i*elem_num] << "\n";
        return false;
      }
    }
  }

  positive = false;

  for ( comp_i = 0; comp_i < comp_num; comp_i++ )
  {
    if ( comp_weight[comp_i] < 0.0 )
    {
      cout << " \n";
      cout << "DIRICHLET_MIX_CHECK - Fatal error!\n";
      cout << "  COMP_WEIGHT(COMP) < 0.\n";
      cout << "  COMP = " << comp_i << "\n";
      cout << "  COMP_WEIGHT(COMP) = " << comp_weight[comp_i] << "\n";
      return false;
    }
    else if ( 0.0 < comp_weight[comp_i] )
    {
      positive = true;
    }
  }

  if ( !positive )
  {
    cout << " \n";
    cout << "DIRICHLET_MIX_CHECK - Fatal error!\n";
    cout << "  All component weights are zero.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double *dirichlet_mix_mean ( int comp_num, int elem_num, double a[], 
  double comp_weight[] )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MIX_MEAN returns the means of a Dirichlet mixture PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int COMP_NUM, the number of components in the Dirichlet
//    mixture density, that is, the number of distinct Dirichlet PDF's
//    that are mixed together.
//
//    Input, int ELEM_NUM, the number of elements of an observation.
//
//    Input, double A[ELEM_NUM*COMP_NUM], the probabilities for
//    element ELEM_NUM in component COMP_NUM.
//    Each A[I,J] should be positive.
//
//    Input, double COMP_WEIGHT[COMP_NUM], the mixture weights of the densities.
//    These do not need to be normalized.  The weight of a given component is
//    the relative probability that that component will be used to generate
//    the sample.
//
//    Output, double DIRICHLET_MIX_MEAN[ELEM_NUM], the means for each element.
//
{
  double *a_vec;
  int comp_i;
  double *comp_mean;
  double comp_weight_sum;
  int elem_i;
  double *mean;

  a_vec = new double[elem_num];
  mean = new double[elem_num];

  comp_weight_sum = 0.0;
  for ( comp_i = 0; comp_i < comp_num; comp_i++ )
  {
    comp_weight_sum = comp_weight_sum + comp_weight[comp_i];
  }

  for ( elem_i = 0; elem_i < elem_num; elem_i++ )
  {
    mean[elem_i] = 0.0;
  }

  for ( comp_i = 0; comp_i < comp_num; comp_i++ )
  {
    for ( elem_i = 0; elem_i < elem_num; elem_i++ )
    {
      a_vec[elem_i] = a[elem_i+comp_i*elem_num];
    }
    comp_mean = dirichlet_mean ( elem_num, a_vec );
    for ( elem_i = 0; elem_i < elem_num; elem_i++ )
    {
      mean[elem_i] = mean[elem_i] + comp_weight[comp_i] * comp_mean[elem_i];
    }
    delete [] comp_mean;
  }

  for ( elem_i = 0; elem_i < elem_num; elem_i++ )
  {
    mean[elem_i] = mean[elem_i] / comp_weight_sum;
  }

  delete [] a_vec;
  delete [] comp_mean;

  return mean;
}
//*******************************************************************************

double dirichlet_mix_pdf ( double x[], int comp_num, int elem_num, double a[], 
  double comp_weight[] )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MIX_PDF evaluates a Dirichlet mixture PDF.
//
//  Discussion:
//
//    The PDF is a weighted sum of Dirichlet PDF's.  Each PDF is a 
//    "component", with an associated weight.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X[ELEM_NUM], the argument of the PDF.
//
//    Input, int COMP_NUM, the number of components in the Dirichlet
//    mixture density, that is, the number of distinct Dirichlet PDF's
//    that are mixed together.
//
//    Input, int ELEM_NUM, the number of elements of an observation.
//
//    Input, double A[ELEM_NUM*COMP_NUM], the probabilities for
//    element ELEM_NUM in component COMP_NUM.
//    Each A[I,J] should be positive.
//
//    Input, double COMP_WEIGHT[COMP_NUM], the mixture weights of the densities.
//    These do not need to be normalized.  The weight of a given component is
//    the relative probability that that component will be used to generate
//    the sample.
//
//    Output, double DIRICHLET_MIX_PDF, the value of the PDF.
//
{
  double *a_vec;
  double comp_pdf;
  double comp_weight_sum;
  int i;
  int j;
  double pdf;

  a_vec = new double[elem_num];

  comp_weight_sum = 0.0;
  for ( j = 0; j < comp_num; j++ )
  {
    comp_weight_sum = comp_weight_sum + comp_weight[j];
  }

  pdf = 0.0;
  for ( j = 0; j < comp_num; j++ )
  {
    for ( i = 0; i < elem_num; i++ )
    {
      a_vec[i] = a[i+j*elem_num];
    }
    comp_pdf = dirichlet_pdf ( x, elem_num, a_vec );

    pdf = pdf + comp_weight[j] * comp_pdf / comp_weight_sum;
  }

  delete [] a_vec;

  return pdf;
}
//*******************************************************************************

double *dirichlet_mix_sample ( int comp_num, int elem_num, double a[], 
  double comp_weight[], int *seed, int *comp )

//*******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MIX_SAMPLE samples a Dirichlet mixture PDF.
//
//  Modified:
//
//    30 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int COMP_NUM, the number of components in the Dirichlet
//    mixture density, that is, the number of distinct Dirichlet PDF's
//    that are mixed together.
//
//    Input, int ELEM_NUM, the number of elements of an observation.
//
//    Input, double A[ELEM_NUM*COMP_NUM], the probabilities for
//    element ELEM_NUM in component COMP_NUM.
//    Each A[I,J] should be positive.
//
//    Input, double COMP_WEIGHT[COMP_NUM], the mixture weights of the densities.
//    These do not need to be normalized.  The weight of a given component is
//    the relative probability that that component will be used to generate
//    the sample.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int *COMP, the index of the component of the Dirichlet
//    mixture that was chosen to generate the sample.
//
//    Output, double DIRICHLET_MIX_SAMPLE[ELEM_NUM], a sample of the PDF.
//
{
  double *a_vec;
  int elem_i;
  double *x;

  a_vec = new double[elem_num];
//
//  Choose a particular density component COMP.
//
  *comp = discrete_sample ( comp_num, comp_weight, seed );
//
//  Sample the density number COMP.
//
  for ( elem_i = 0; elem_i < elem_num; elem_i++ )
  {
    a_vec[elem_i] = a[elem_i+(*comp-1)*elem_num];
  }

  x = dirichlet_sample ( elem_num, a_vec, seed );

  delete [] a_vec;

  return x;
}
//******************************************************************************

double dirichlet_multinomial_pdf ( int x[], int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    DIRICHLET_MULTINOMIAL_PDF evaluates a Dirichlet Multinomial PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = Comb(A,B,X) * ( Gamma(C_Sum) / Gamma(C_Sum+A) )
//      Product ( 1 <= I <= B ) Gamma(C(I)+X(I)) / Gamma(C(I))
//
//    where:
//
//      Comb(A,B,X) is the multinomial coefficient C( A; X(1), X(2), ..., X(B) ),
//      C_Sum = Sum ( 1 <= I <= B ) C(I)
//
//  Modified:
//
//    19 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Kenneth Lange,
//    Mathematical and Statistical Methods for Genetic Analysis,
//    Springer, 1997, page 45.
//
//  Parameters:
//
//    Input, int X[B]; X[I] counts the number of occurrences of
//    outcome I, out of the total of A trials.
//
//    Input, int A, the total number of trials.
//
//    Input, int B, the number of different possible outcomes on
//    one trial.
//
//    Input, double C[B]; C[I] is the Dirichlet parameter associated
//    with outcome I.
//
//    Output, double DIRICHLET_MULTINOMIAL_PDF, the value of the Dirichlet
//     multinomial PDF.
//
{
  double c_sum;
  int i;
  double pdf;
  double pdf_log;

  c_sum = dvec_sum ( b, c );

  pdf_log = - gamma_log ( c_sum + ( double ) ( a ) ) + gamma_log ( c_sum ) 
    + gamma_log ( ( double ) ( a + 1 ) );

  for ( i = 0; i < b; i++ )
  {
    pdf_log = pdf_log + gamma_log ( c[i] + ( double ) ( x[i] ) ) 
      - gamma_log ( c[i] ) - gamma_log ( ( double ) ( x[i] + 1 ) );
  }

  pdf = exp ( pdf_log );

  return pdf;
}
//*******************************************************************************

double discrete_cdf ( int x, int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_CDF evaluates the Discrete CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the item whose probability is desired.
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of outcomes
//    1 through A.  Each entry must be nonnegative.
//
//    Output, double DISCRETE_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < 1 )
  {
    cdf = 0.0;
  }
  else if ( x < a )
  {
    cdf = dvec_sum ( x, b ) / dvec_sum ( a, b );
  }
  else if ( a <= x )
  {
    cdf = 1.0;
  }

  return cdf;
}
//*******************************************************************************

int discrete_cdf_inv ( double cdf, int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_CDF_INV inverts the Discrete CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of outcomes 
//    1 through A.  Each entry must be nonnegative.
//
//    Output, int DISCRETE_CDF_INV, the corresponding argument for which
//    CDF(X-1) < CDF <= CDF(X)
//
{
  double b_sum;
  double cum;
  int j;
  int x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "DISCRETE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  b_sum = dvec_sum ( a, b );

  cum = 0.0;

  for ( j = 1; j <= a; j++ )
  {
    cum = cum + b[j-1] / b_sum;

    if ( cdf <= cum )
    {
      x = j;
      return x;
    }
  }

  x = a;
  
  return x;
}
//*******************************************************************************

bool discrete_check ( int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_CHECK checks the parameters of the Discrete CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of 
//    outcomes 1 through A.  Each entry must be nonnegative.
//
//    Output, bool DISCRETE_CHECK, is true if the parameters are legal.
//
{
  double b_sum;
  int j;

  for ( j = 0; j < a; j++ )
  {
    if ( b[j] < 0.0 )
    {
      cout << " \n";
      cout << "DISCRETE_CHECK - Fatal error!\n";
      cout << "  Negative probabilities not allowed.\n";
      return false;
    }
  }

  b_sum = dvec_sum ( a, b );

  if ( b_sum == 0.0 )
  {
    cout << " \n";
    cout << "DISCRETE_CHECK - Fatal error!\n";
    cout << "  Total probablity is zero.\n";
    return false;
  }
  
  return true;
}
//*******************************************************************************

double discrete_mean ( int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_MEAN evaluates the mean of the Discrete PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of
//    outcomes 1 through A.  Each entry must be nonnegative.
//
//    Output, double DISCRETE_MEAN, the mean of the PDF.
//
{
  double b_sum;
  int j;
  double mean;

  b_sum = dvec_sum ( a, b );

  mean = 0.0;
  for ( j = 0; j < a; j++ )
  {
    mean = mean + ( double ) ( j ) * b[j];
  }

  mean = mean / b_sum;

  return mean;
}
//*******************************************************************************

double discrete_pdf ( int x, int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_PDF evaluates the Discrete PDF.
//
//  Formula:
//
//    PDF(A,B;X) = B(X) if 1 <= X <= A
//                = 0    otherwise
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the item whose probability is desired.
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of 
//    outcomes 1 through A.  Each entry must be nonnegative.
//
//    Output, double DISCRETE_PDF, the value of the PDF.
//
{
  double b_sum;
  double pdf;

  b_sum = dvec_sum ( a, b );

  if ( 1 <= x && x <= a )
  {
    pdf = b[x-1] / b_sum;
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
}
//*******************************************************************************

int discrete_sample ( int a, double b[], int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_SAMPLE samples the Discrete PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of 
//    outcomes 1 through A.  Each entry must be nonnegative.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int DISCRETE_SAMPLE, a sample of the PDF.
//
{
  double b_sum;
  double cdf;
  int x;

  b_sum = dvec_sum ( a, b );

  cdf = d_uniform_01 ( seed );

  x = discrete_cdf_inv ( cdf, a, b );

  return x;
}
//*******************************************************************************

double discrete_variance ( int a, double b[] )

//*******************************************************************************
//
//  Purpose:
//
//    DISCRETE_VARIANCE evaluates the variance of the Discrete PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of probabilities assigned.
//
//    Input, double B[A], the relative probabilities of 
//    outcomes 1 through A.  Each entry must be nonnegative.
//
//    Output, double DISCRETE_VARIANCE, the variance of the PDF.
//
{
  double b_sum;
  int j;
  double mean;
  double variance;

  b_sum = dvec_sum ( a, b );

  mean = 0.0;
  for ( j = 1; j <= a; j++ )
  {
    mean = mean + ( double ) ( j ) * b[j-1];
  }

  mean = mean / b_sum;

  variance = 0.0;
  for ( j = 1; j <= a; j++ )
  {
    variance = variance + b[j-1] * pow ( ( double ) j - mean, 2 ); 
  }

  variance = variance / b_sum;

  return variance;
}
//******************************************************************************

void dmat_print ( int m, int n, double a[], char *title )

//******************************************************************************
//
//  Purpose:
//
//    DMAT_PRINT prints a double precision matrix, with an optional title.
//
//  Discussion:
//
//    The doubly dimensioned array A is treated as a one dimensional vector,
//    stored by COLUMNS.  Entry A(I,J) is stored as A[I+J*M]
//
//  Modified:
//
//    29 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, the number of rows in A.
//
//    Input, int N, the number of columns in A.
//
//    Input, double A[M*N], the M by N matrix.
//
//    Input, char *TITLE, a title to be printed.
//
{
  dmat_print_some ( m, n, a, 1, 1, m, n, title );

  return;
}
//******************************************************************************

void dmat_print_some ( int m, int n, double a[], int ilo, int jlo, int ihi, 
  int jhi, char *title )

//******************************************************************************
//
//  Purpose:
//
//    DMAT_PRINT_SOME prints some of a double precision matrix.
//
//  Modified:
//
//    09 April 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, the number of rows of the matrix.
//    M must be positive.
//
//    Input, int N, the number of columns of the matrix.
//    N must be positive.
//
//    Input, double A[M*N], the matrix.
//
//    Input, int ILO, JLO, IHI, JHI, designate the first row and
//    column, and the last row and column to be printed.
//
//    Input, char *TITLE, a title for the matrix.
{
# define INCX 5

  int i;
  int i2hi;
  int i2lo;
  int j;
  int j2hi;
  int j2lo;

  if ( 0 < s_len_trim ( title ) )
  {
    cout << "\n";
    cout << title << "\n";
  }
//
//  Print the columns of the matrix, in strips of 5.
//
  for ( j2lo = jlo; j2lo <= jhi; j2lo = j2lo + INCX )
  {
    j2hi = j2lo + INCX - 1;
    j2hi = i_min ( j2hi, n );
    j2hi = i_min ( j2hi, jhi );

    cout << "\n";
//
//  For each column J in the current range...
//
//  Write the header.
//
    cout << "  Col:    ";
    for ( j = j2lo; j <= j2hi; j++ )
    {
      cout << setw(7) << j << "       ";
    }
    cout << "\n";
    cout << "  Row\n";
    cout << "  ---\n";
//
//  Determine the range of the rows in this strip.
//
    i2lo = i_max ( ilo, 1 );
    i2hi = i_min ( ihi, m );

    for ( i = i2lo; i <= i2hi; i++ )
    {
//
//  Print out (up to) 5 entries in row I, that lie in the current strip.
//
      cout << setw(5) << i << "  ";
      for ( j = j2lo; j <= j2hi; j++ )
      {
        cout << setw(12) << a[i-1+(j-1)*m] << "  ";
      }
      cout << "\n";
    }

  }

  cout << "\n";

  return;
# undef INCX
}
//******************************************************************************

double *drow_max ( int m, int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DROW_MAX returns the maximums of a double precision array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MAX =
//      3
//      7
//
//  Modified:
//
//    29 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, double A[M*N], the array to be examined.
//
//    Output, double DROW_MAX[M], the maximums of the rows.
//
{
  int i;
  int j;
  double *amax;

  amax = new double[m];

  for ( i = 0; i < m; i++ )
  {
    amax[i] = a[i+0*m];

    for ( j = 1; j < n; j++ )
    {
      if ( amax[i] < a[i+j*m] )
      {
        amax[i] = a[i+j*m];
      }
    }
  }

  return amax;
}
//******************************************************************************

double *drow_mean ( int m, int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DROW_MEAN returns the means of a double precision array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MEAN =
//      2
//      5
//
//  Modified:
//
//    29 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, double A[M*N], the array to be examined.
//
//    Output, double DROW_MEAN[M], the means, or averages, of the rows.
//
{
  int i;
  int j;
  double *mean;

  mean = new double[m];

  for ( i = 0; i < m; i++ )
  {
    mean[i] = 0.0;
    for ( j = 0; j < n; j++ )
    {
      mean[i] = mean[i] + a[i+j*m];
    }
    mean[i] = mean[i] / ( double ) ( n );
  }

  return mean;
}
//******************************************************************************

double *drow_min ( int m, int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DROW_MIN returns the minimums of a double precision array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MIN =
//      1
//      2
//
//  Modified:
//
//    29 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, double A[M*N], the array to be examined.
//
//    Output, double DROW_MIN[M], the minimums of the rows.
//
{
  int i;
  int j;
  double *amin;

  amin = new double[m];

  for ( i = 0; i < m; i++ )
  {
    amin[i] = a[i+0*m];
    for ( j = 1; j < n; j++ )
    {
      if ( a[i+j*m] < amin[i] )
      {
        amin[i] = a[i+j*m];
      }
    }
  }

  return amin;
}
//******************************************************************************

double *drow_variance ( int m, int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DROW_VARIANCE returns the variances of double precision array of rows.
//
//  Modified:
//
//    29 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, double A[M*N], the array whose variances are desired.
//
//    Output, double DROW_VARIANCE[M], the variances of the rows.
//
{
  int i;
  int j;
  double mean;
  double *variance;

  variance = new double[m];

  for ( i = 0; i < m; i++ )
  {
    mean = 0.0;
    for ( j = 0; j < n; j++ )
    {
      mean = mean + a[i+j*m];
    }
    mean = mean / ( double ) ( n );

    variance[i] = 0.0;
    for ( j = 0; j < n; j++ )
    {
      variance[i] = variance[i] + pow ( ( a[i+j*m] - mean ), 2 );
    }

    if ( 1 < n )
    {
      variance[i] = variance[i] / ( double ) ( n - 1 );
    }
    else
    {
      variance[i] = 0.0;
    }

  }

  return variance;
}
//**********************************************************************

double dvec_max ( int n, double *dvec )

//**********************************************************************
//
//  Purpose:
//
//    DVEC_MAX returns the value of the maximum element in a real array.
//
//  Modified:
//
//    15 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the array.
//
//    Input, double *RVEC, a pointer to the first entry of the array.
//
//    Output, double DVEC_MAX, the value of the maximum element.  This
//    is set to 0.0 if N <= 0.
//
{
  int i;
  double rmax;
  double *dvec_pointer;

  if ( n <= 0 ) 
  {
    return 0.0;
  }

  for ( i = 0; i < n; i++ ) 
  {
    if ( i == 0 ) 
    {
      rmax = *dvec;
      dvec_pointer = dvec;
    }
    else
    {
      dvec_pointer++;
      if ( rmax < *dvec_pointer ) 
      {
        rmax = *dvec_pointer;
      }
    }
  }

  return rmax;
  
}
//**********************************************************************

double dvec_mean ( int n, double x[] )

//**********************************************************************
//
//  Purpose:
//
//    DVEC_MEAN returns the mean of a real vector.
//
//  Modified:
//
//    01 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, double X[N], the vector whose mean is desired.
//
//    Output, double DVEC_MEAN, the mean, or average, of the vector entries.
//
{
  int i;
  double mean;

  mean = 0.0;
  for ( i = 0; i < n; i++ )
  {
    mean = mean + x[i];
  }

  mean = mean / ( double ) n;

  return mean;
}
//**********************************************************************

double dvec_min ( int n, double *dvec )

//**********************************************************************
//
//  Purpose:
//
//    DVEC_MIN returns the value of the minimum element in a real array.
//
//  Modified:
//
//    15 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the array.
//
//    Input, double *RVEC, a pointer to the first entry of the array.
//
//    Output, double DVEC_MIN, the value of the minimum element.  This
//    is set to 0.0 if N <= 0.
//
{
  int i;
  double rmin;
  double *dvec_pointer;

  if ( n <= 0 ) 
  {
    return 0.0;
  }

  for ( i = 0; i < n; i++ ) 
  {
    if ( i == 0 ) 
    {
      rmin = *dvec;
      dvec_pointer = dvec;
    }
    else 
    {
      dvec_pointer++;
      if ( *dvec_pointer < rmin ) 
      {
        rmin = *dvec_pointer;
      }
    }
  }

  return rmin;
  
}
//********************************************************************

void dvec_print ( int n, double a[], char *title )

//********************************************************************
//
//  Purpose:
//
//    DVEC_PRINT prints a real vector.
//
//  Modified:
//
//    16 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components of the vector.
//
//    Input, double A[N], the vector to be printed.
//
//    Input, char *TITLE, a title to be printed first.
//    TITLE may be blank.
//
{
  int i;

  if ( 0 < s_len_trim ( title ) )
  {
    cout << "\n";
    cout << title << "\n";
  }

  cout << "\n";
  for ( i = 0; i <= n-1; i++ ) 
  {
    cout << setw(6)  << i + 1 << "  " 
         << setw(14) << a[i]  << "\n";
  }

  return;
}
//**********************************************************************

double *dvec_random ( int n, double alo, double ahi, int *seed )

//**********************************************************************
//
//  Purpose:
//
//    DVEC_RANDOM returns a random double precision vector in a given range.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, double ALO, AHI, the range allowed for the entries.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double DVEC_RANDOM[N], the vector of randomly chosen integers.
//
{
  double *a;
  int i;

  a = new double[n];

  for ( i = 0; i < n; i++ )
  {
    a[i] = d_random ( alo, ahi, seed );
  }

  return a; 
}
//****************************************************************************

double dvec_sum ( int n, double a[] )

//****************************************************************************
//
//  Purpose:
//
//    DVEC_SUM returns the sum of a double precision vector.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, double A[N], the vector.
//
//    Output, double DVEC_SUM, the sum of the vector.
//
{
  int i;
  float sum;

  sum = 0.0E+00;
  for ( i = 0; i < n; i++ )
  {
    sum = sum + a[i];
  }

  return sum;
}
//******************************************************************************

double *dvec_uniform_01 ( int n, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    DVEC_UNIFORM_01 fills a double precision vector with pseudorandom values.
//
//  Discussion:
//
//    This routine implements the recursion
//
//      seed = 16807 * seed mod ( 2**31 - 1 )
//      unif = seed / ( 2**31 - 1 )
//
//    The integer arithmetic never requires more than 32 bits,
//    including a sign bit.
//
//  Modified:
//
//    19 August 2004
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, L E Schrage,
//    A Guide to Simulation,
//    Springer Verlag, pages 201-202, 1983.
//
//    Bennett Fox,
//    Algorithm 647:
//    Implementation and Relative Efficiency of Quasirandom
//    Sequence Generators,
//    ACM Transactions on Mathematical Software,
//    Volume 12, Number 4, pages 362-376, 1986.
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double DVEC_UNIFORM_01[N], the vector of pseudorandom values.
//
{
  int i;
  int k;
  double *r;

  r = new double[n];

  for ( i = 0; i < n; i++ )
  {
    k = *seed / 127773;

    *seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

    if ( *seed < 0 )
    {
      *seed = *seed + 2147483647;
    }

    r[i] = ( double ) ( *seed ) * 4.656612875E-10;
  }

  return r;
}
//******************************************************************************

void dvec_unit_sum ( int n, double a[] )

//******************************************************************************
//
//  Purpose:
//
//    DVEC_UNIT_SUM normalizes a double precision vector to have unit sum.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, integer N, the number of entries in the vector.
//
//    Input/output, double A[N], the vector to be normalized.
//    On output, the entries of A should have unit sum.  However, if
//    the input vector has zero sum, the routine halts.
//
{
  double a_sum;
  int i;

  a_sum = 0.0;
  for ( i = 0; i < n; i++ )
  {
    a_sum = a_sum + a[i];
  }

  if ( a_sum == 0.0 )
  {
    cout << "\n";
    cout << "DVEC_UNIT_SUM - Fatal error!\n";
    cout << "  The vector entries sum to 0.\n";
    exit ( 1 );
  }

  for ( i = 0; i < n; i++ )
  {
    a[i] = a[i] / a_sum;
  }

  return;
}

//**********************************************************************

double dvec_variance ( int n, double x[] )

//**********************************************************************
//
//  Purpose:
//
//    DVEC_VARIANCE returns the variance of a real vector.
//
//  Modified:
//
//    01 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, double X[N], the vector whose variance is desired.
//
//    Output, double DVEC_VARIANCE, the variance of the vector entries.
//
{
  int i;
  double mean;
  double variance;

  mean = dvec_mean ( n, x );

  variance = 0.0;
  for ( i = 0; i < n; i++ )
  {
    variance = variance + ( x[i] - mean ) * ( x[i] - mean );
  }

  if ( 1 < n )
  {
    variance = variance / ( double ) ( n - 1 );
  }
  else
  {
    variance = 0.0;
  }

  return variance;
}
//******************************************************************************

double e_constant ( void )

//******************************************************************************
//
//  Purpose:
//
//    E_CONSTANT returns the value of E.
//
//  Discussion:
//
//    "E" was named in honor of Euler, but is known as Napier's constant.
//
//  Modified:
//
//    17 April 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double E_CONSTANT, the base of the natural logarithm system.
//
{
  double value = 2.71828182845904523536028747135266249775724709369995E+00;

  return value;
}
//*******************************************************************************

double empirical_discrete_cdf ( double x, int a, double b[], double c[] )

//*******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_CDF evaluates the Empirical Discrete CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B[A], the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C[A], the values.
//    The values must be distinct and in ascending order.
//
//    Output, double EMPIRICAL_DISCRETE_CDF, the value of the CDF.
//
{
  double bsum;
  double cdf;
  int i;

  cdf = 0.0;

  bsum = dvec_sum ( a, b );

  for ( i = 1; i <= a; i++ )
  {
    if ( x < c[i-1] )
    {
      return cdf;
    }
    cdf = cdf + b[i-1] / bsum;
  }

  return cdf;
}
//******************************************************************************

double empirical_discrete_cdf_inv ( double cdf, int a, double b[], double c[] )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_CDF_INV inverts the Empirical Discrete CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B(A), the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C(A), the values.
//    The values must be distinct and in ascending order.
//
//    Output, double EMPIRICAL_DISCRETE_CDF_INV, the smallest argument 
//    whose CDF is greater than or equal to CDF.
//
{
  double bsum;
  double cdf2;
  int i;
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "EMPIRICAL_DISCRETE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  bsum = dvec_sum ( a, b );

  x = c[0];
  cdf2 = b[0] / bsum;

  for ( i = 1; i < a; i++ )
  {
    if ( cdf <= cdf2 )
    {
      return x;
    }

    x = c[i];
    cdf2 = cdf2 + b[i] / bsum;
  }

  return x;
}
//******************************************************************************

bool empirical_discrete_check ( int a, double b[], double c[] )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_CHECK checks the parameters of the Empirical Discrete CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B[A], the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C[A], the values.
//    The values must be distinct and in ascending order.
//
//    Output, bool EMPIRICAL_DISCRETE_CHECK, is true if the parameters
//    are legal.
//
{
  int i;
  int j;
  bool positive;

  if ( a <= 0 )
  {
    cout << " \n";
    cout << "EMPIRICAL_DISCRETE_CHECK - Fatal error!\n";
    cout << "  A must be positive.\n";
    cout << "  Input A = " << a << "\n";
    cout << "  A is the number of weights.\n";
    return false;
  }

  for ( i = 0; i < a; i++ )
  {
    if ( b[i] < 0.0 )
    {
      cout << " \n";
      cout << "EMPIRICAL_DISCRETE_CHECK - Fatal error!\n";
      cout << "  B[" << i << "] < 0.\n";
      cout << "  But all B values must be nonnegative.\n";
      return false;
    }
  }

  positive = false;

  for ( i = 0; i < a; i++ )
  {
    if ( 0.0 < b[i] )
    {
      positive = true;
    }
  }

  if ( !positive )
  {
    cout << " \n";
    cout << "EMPIRICAL_DISCRETE_CHECK - Fatal error!\n";
    cout << "  All B(*) = 0.\n";
    cout << "  But at least one B values must be nonzero.\n";
    return false;
  }

  for ( i = 0; i < a; i++ )
  {
    for ( j = i+1; j < a; j++ )
    {
      if ( c[i] == c[j] )
      {
        cout << " \n";
        cout << "EMPIRICAL_DISCRETE_CHECK - Fatal error!\n";
        cout << "  All values C must be unique.\n";
        cout << "  But at least two values are identical.\n";
        return false;
      }
    }
  }

  for ( i = 0; i < a-1; i++ )
  {
    if ( c[i+1] < c[i] )
    {
      cout << " \n";
      cout << "EMPIRICAL_DISCRETE_CHECK - Fatal error!\n";
      cout << "  The values in C must be in ascending order.\n";
      return false;
    }
  }

  return true;
}
//******************************************************************************

double empirical_discrete_mean ( int a, double b[], double c[] )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_MEAN returns the mean of the Empirical Discrete PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B(A), the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C(A), the values.
//    The values must be distinct and in ascending order.
//
//    Output, double EMPIRICAL_DISCRETE_MEAN, the mean of the PDF.
//
{
  int i;
  double mean;

  mean = 0.0;
  for ( i = 0; i < a; i++ )
  {
    mean = mean + b[i] * c[i];
  }
  mean = mean / dvec_sum ( a, b );

  return mean;
}
//******************************************************************************

double empirical_discrete_pdf ( double x, int a, double b[], double c[] )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_PDF evaluates the Empirical Discrete PDF.
//
//  Discussion:
//
//    A set of A values C(1:A) are assigned nonnegative weights B(1:A),
//    with at least one B nonzero.  The probability of C(I) is the
//    value of B(I) divided by the sum of the weights.  
//
//    The C's must be distinct, and given in ascending order.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B(A), the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C(A), the values.
//    The values must be distinct and in ascending order.
//
//    Output, double EMPIRICAL_DISCRETE_PDF, the value of the PDF.
//
{
  int i;
  double pdf;

  for ( i = 0; i <= a; i++ )
  {
    if ( x == c[i] )
    {
      pdf = b[i] / dvec_sum ( a, b );
      return pdf;
    }
  }

  pdf = 0.0;

  return pdf;
}
//******************************************************************************

double empirical_discrete_sample ( int a, double b[], double c[], int *seed )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_SAMPLE samples the Empirical Discrete PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B(A), the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C(A), the values.
//    The values must be distinct and in ascending order.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double EMPIRICAL_DISCRETE_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = empirical_discrete_cdf_inv ( cdf, a, b, c );

  return x;
}
//******************************************************************************

double empirical_discrete_variance ( int a, double b[], double c[] )

//******************************************************************************
//
//  Purpose:
//
//    EMPIRICAL_DISCRETE_VARIANCE returns the variance of the Empirical Discrete PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of values.
//    0 < A.
//
//    Input, double B(A), the weights of each value.
//    0 <= B(1:A) and at least one value is nonzero.
//
//    Input, double C(A), the values.
//    The values must be distinct and in ascending order.
//
//    Output, double EMPIRICAL_DISCRETE_VARIANCE, the variance of the PDF.
//
{
  double bsum;
  int i;
  double mean;
  double variance;

  bsum = dvec_sum ( a, b );

  mean = empirical_discrete_mean ( a, b, c );

  variance = 0.0;

  for ( i = 0; i < a; i++ )
  {
    variance = variance + ( b[i] / bsum ) * pow ( c[i] - mean, 2 );
  }

  return variance;
}
//*******************************************************************************

double erlang_cdf ( double x, double a, double b, int c )

//*******************************************************************************
//
//  Purpose:
//
//    ERLANG_CDF evaluates the Erlang CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, double ERLANG_CDF, the value of the CDF.
//
{
  double cdf;
  double p2;
  double x2;

  if ( x < a )
  {
    cdf = 0.0;
  }
  else
  {
    x2 = ( x - a ) / b;
    p2 = ( double ) ( c );

    cdf = gamma_inc ( p2, x2 );
  }

  return cdf;
}
//******************************************************************************

double erlang_cdf_inv ( double cdf, double a, double b, int c )

//******************************************************************************
//
//  Purpose:
//
//    ERLANG_CDF_INV inverts the Erlang CDF.
//
//  Discussion:
//
//    A simple bisection method is used.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, double ERLANG_CDF_INV, the corresponding argument of the CDF.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "ERLANG_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = a;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
    return x;
  }

  x1 = a;
  cdf1 = 0.0;

  x2 = a + 1.0;

  for ( ; ; )
  {
    cdf2 = erlang_cdf ( x2, a, b, c );

    if ( cdf < cdf2 )
    {
      break;
    }
    x2 = a + 2.0 * ( x2 - a );
  }
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = erlang_cdf ( x3, a, b, c );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "ERLANG_CDF_INV - Warning!\n";
      cout << "  Iteration limit exceeded.\n";
      return x;
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }
  }
  return x;
}
//*******************************************************************************

bool erlang_check ( double a, double b, int c )

//*******************************************************************************
//
//  Purpose:
//
//    ERLANG_CHECK checks the parameters of the Erlang PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, bool ERLANG_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "ERLANG_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  if ( c <= 0 )
  {
    cout << " \n";
    cout << "ERLANG_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double erlang_mean ( double a, double b, int c )

//*******************************************************************************
//
//  Purpose:
//
//    ERLANG_MEAN returns the mean of the Erlang PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, double ERLANG_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * ( double ) ( c );

  return mean;
}
//******************************************************************************

double erlang_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    ERLANG_PDF evaluates the Erlang PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = ( ( X - A ) / B )**( C - 1 ) 
//      / ( B * Gamma ( C ) * EXP ( ( X - A ) / B ) )
//
//    for 0 < B, 0 < C integer, A <= X.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, double ERLANG_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = pow ( y, c - 1 ) / ( b * d_factorial ( c - 1 ) * exp ( y ) );
  }

  return pdf;
}
//******************************************************************************

double erlang_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    ERLANG_SAMPLE samples the Erlang PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double a2;
  double b2;
  int i;
  double x;
  double x2;

  a2 = 0.0;
  b2 = b;
  x = a;

  for ( i = 1; i <= c; i++ )
  {
    x2 = exponential_sample ( a2, b2, seed );
    x = x + x2;
  }

  return x;
}
//******************************************************************************

double erlang_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    ERLANG_VARIANCE returns the variance of the Erlang PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, int C, the parameters of the PDF.
//    0.0 < B.
//    0 < C.
//
//    Output, double ERLANG_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance =  b * b * ( double ) ( c );

  return variance;
}
//******************************************************************************

double erf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    ERF evaluates the error function.
//
//  Definition:
//
//    ERF(X) = ( 2 / sqrt ( PI ) ) * Integral ( 0 <= T <= X ) EXP ( - T**2 ) dT.
//
//  Properties:
//
//    Limit ( X -> -Infinity ) ERF(X) =          -1.0;
//                             ERF(0) =           0.0;
//                             ERF(0.476936...) = 0.5;
//    Limit ( X -> +Infinity ) ERF(X) =          +1.0.
//
//    0.5 * ( ERF(X/sqrt(2)) + 1 ) = Normal_01_CDF(X)
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    W J Cody,
//    Mathematics and Computer Science Division,
//    Argonne National Laboratory,
//    Argonne, Illinois, 60439.
//
//  Reference:
//
//    W J Cody,
//    "Rational Chebyshev Approximations for the Error Function",
//    Mathematics of Computation, 
//    1969, pages 631-638.
//
//  Parameters:
//
//    Input, double X, the argument of the error function.
//
//    Output, double ERF, the value of the error function.
//
{
  double a[5] = {
    3.16112374387056560E+00, 
    1.13864154151050156E+02, 
    3.77485237685302021E+02, 
    3.20937758913846947E+03, 
    1.85777706184603153E-01 };
  double b[4] = { 
    2.36012909523441209E+01, 
    2.44024637934444173E+02, 
    1.28261652607737228E+03, 
    2.84423683343917062E+03 };
  double c[9] = { 
    5.64188496988670089E-01, 
    8.88314979438837594E+00, 
    6.61191906371416295E+01, 
    2.98635138197400131E+02, 
    8.81952221241769090E+02, 
    1.71204761263407058E+03, 
    2.05107837782607147E+03, 
    1.23033935479799725E+03, 
    2.15311535474403846E-08 };
  double d[8] = { 
    1.57449261107098347E+01, 
    1.17693950891312499E+02, 
    5.37181101862009858E+02, 
    1.62138957456669019E+03, 
    3.29079923573345963E+03, 
    4.36261909014324716E+03, 
    3.43936767414372164E+03, 
    1.23033935480374942E+03 };
  double del;
  double erfxd;
  int i;
  double p[6] = {
    3.05326634961232344E-01, 
    3.60344899949804439E-01, 
    1.25781726111229246E-01, 
    1.60837851487422766E-02, 
    6.58749161529837803E-04, 
    1.63153871373020978E-02 };
  double q[5] = {
    2.56852019228982242, 
    1.87295284992346047, 
    5.27905102951428412E-01, 
    6.05183413124413191E-02, 
    2.33520497626869185E-03 };
  double sqrpi = 0.56418958354775628695;
  double thresh = 0.46875;
  double xabs;
  double xbig = 26.543;
  double xden;
  double xnum;
  double xsmall = 1.11E-16;
  double xsq;

  xabs = fabs ( ( x ) );
//
//  Evaluate ERF(X) for |X| <= 0.46875.
//
  if ( xabs <= thresh )
  {
    if ( xsmall < xabs )
    {
      xsq = xabs * xabs;
    }
    else
    {
      xsq = 0.0;
    }

    xnum = a[4] * xsq;
    xden = xsq;

    for ( i = 0; i < 3; i++ )
    {
      xnum = ( xnum + a[i] ) * xsq;
      xden = ( xden + b[i] ) * xsq;
    }

    erfxd = x * ( xnum + a[3] ) / ( xden + b[3] );
  }
//
//  Evaluate ERFC(X) for 0.46875 <= |X| <= 4.0.
//
  else if ( xabs <= 4.0 )
  {
    xnum = c[8] * xabs;
    xden = xabs;
    for ( i = 0; i < 7; i++ )
    {
      xnum = ( xnum + c[i] ) * xabs;
      xden = ( xden + d[i] ) * xabs;
    }

    erfxd = ( xnum + c[7] ) / ( xden + d[7] );
    xsq = ( ( double ) ( ( int ) ( xabs * 16.0 ) ) ) / 16.0;
    del = ( xabs - xsq ) * ( xabs + xsq );
    erfxd = exp ( - xsq * xsq ) * exp ( -del ) * erfxd;

    erfxd = ( 0.5 - erfxd ) + 0.5;

    if ( x < 0.0 )
    {
      erfxd = -erfxd;
    }
  }
//
//  Evaluate ERFC(X) for 4.0 < |X|.
//
  else
  {
    if ( xbig <= xabs )
    {
      if ( 0.0 < x )
      {
        erfxd = 1.0;
      }
      else
      {
        erfxd = - 1.0;
      }
    }
    else
    {
      xsq = 1.0 / ( xabs * xabs );

      xnum = p[5] * xsq;
      xden = xsq;

      for ( i = 0; i < 4; i++ )
      {
        xnum = ( xnum + p[i] ) * xsq;
        xden = ( xden + q[i] ) * xsq;
      }

      erfxd = xsq * ( xnum + p[4] ) / ( xden + q[4] );
      erfxd = ( sqrpi - erfxd ) / xabs;
      xsq = ( ( double ) ( ( int ) ( xabs * 16.0 ) ) ) / 16.0;
      del = ( xabs - xsq ) * ( xabs + xsq );
      erfxd = exp ( - xsq * xsq ) * exp ( - del ) * erfxd;

      erfxd = ( 0.5 - erfxd ) + 0.5;

      if ( x < 0.0 )
      {
        erfxd = -erfxd;
      }
    }
  }

  return erfxd;
}
//*******************************************************************************

double euler_constant ( void )

//*******************************************************************************
//
//  Purpose:
//
//    EULER_CONSTANT returns the value of the Euler-Mascheroni constant.
//
//  Discussion:
//
//    The Euler-Mascheroni constant is often denoted by a lower-case
//    Gamma.  Gamma is defined as
//
//      Gamma = limit ( M -> Infinity ) 
//        ( Sum ( 1 <= N <= M ) 1 / N ) - Log ( M )
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double EULER_CONSTANT, the value of the 
//    Euler-Mascheroni constant.
//
{
  double value = 0.577215664901532860606512090082402431042;

  return value;
}
//*******************************************************************************

double exponential_01_cdf ( double x )

//*******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_CDF evaluates the Exponential 01 CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Output, double EXPONENTIAL_01_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - exp ( - x );
  }

  return cdf;
}
//******************************************************************************

double exponential_01_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_CDF_INV inverts the Exponential 01 CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Output, double EXPONENTIAL_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "EXPONENTIAL_01_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = - log ( 1.0 - cdf );

  return x;
}
//******************************************************************************

double exponential_01_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_MEAN returns the mean of the Exponential 01 PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double EXPONENTIAL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = 1.0;

  return mean;
}
//******************************************************************************

double exponential_01_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_PDF evaluates the Exponential 01 PDF.
//
//  Formula:
//
//    PDF(X) = EXP ( - X )
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = exp ( - x );
  }

  return pdf;
}
//******************************************************************************

double exponential_01_sample ( int *seed )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_SAMPLE samples the Exponential PDF with parameter 1.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = - log ( 1.0 - cdf );

  return x;
}
//******************************************************************************

double exponential_01_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_01_VARIANCE returns the variance of the Exponential 01 PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 1.0;

  return variance;
}
//*******************************************************************************

double exponential_cdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_CDF evaluates the Exponential CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, double EXPONENTIAL_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - exp ( ( a - x ) / b );
  }

  return cdf;
}
//******************************************************************************

double exponential_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_CDF_INV inverts the Exponential CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXPONENTIAL_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "EXPONENTIAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a - b * log ( 1.0 - cdf );

  return x;
}
//******************************************************************************

void exponential_cdf_values ( int *n_data, double *lambda, double *x, 
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_CDF_VALUES returns some values of the Exponential CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = ExponentialDistribution [ lambda ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    29 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *LAMBDA, the parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 9

  double fx_vec[N_MAX] = { 
     0.3934693402873666E+00,  
     0.6321205588285577E+00,  
     0.7768698398515702E+00,  
     0.8646647167633873E+00,  
     0.8646647167633873E+00,  
     0.9816843611112658E+00,  
     0.9975212478233336E+00,  
     0.9996645373720975E+00,  
     0.9999546000702375E+00 };

  double lambda_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,    
     0.5000000000000000E+00,    
     0.5000000000000000E+00,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *lambda = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *lambda = lambda_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool exponential_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_CHECK checks the parameters of the Exponential CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, bool EXPONENTIAL_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "EXPONENTIAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  return true;
}
//******************************************************************************

double exponential_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_MEAN returns the mean of the Exponential PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXPONENTIAL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b;

  return mean;
}
//******************************************************************************

double exponential_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_PDF evaluates the Exponential PDF.
//
//  Formula:
//
//    PDF(A,B;X) = ( 1 / B ) * EXP ( ( A - X ) / B )
//
//  Discussion:
//
//    The time interval between two Poisson events is a random 
//    variable with the Exponential PDF.  The parameter B is the
//    average interval between events.
//
//    In another context, the Exponential PDF is related to
//    the Boltzmann distribution, which describes the relative 
//    probability of finding a system, which is in thermal equilibrium 
//    at absolute temperature T, in a given state having energy E.  
//    The relative probability is
//
//      Boltzmann_Relative_Probability(E,T) = exp ( - E / ( k * T ) ),
//
//    where k is the Boltzmann constant, 
//
//      k = 1.38 * 10**(-23) joules / degree Kelvin
//
//    and normalization requires a determination of the possible
//    energy states of the system.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXPONENTIAL_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < a )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = ( 1.0 / b ) * exp ( ( a - x ) / b );
  }

  return pdf;
}
//******************************************************************************

double exponential_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_SAMPLE samples the Exponential PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double EXPONENTIAL_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = exponential_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double exponential_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXPONENTIAL_VARIANCE returns the variance of the Exponential PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXPONENTIAL_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b;

  return variance;
}
//******************************************************************************

double extreme_values_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_CDF evaluates the Extreme Values CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0D+00 < B.
//
//    Output, double EXTREME_VALUES_CDF, the value of the CDF.
//
{
  double cdf;
  double y;

  y = ( x - a ) / b;

  cdf = exp ( - exp ( - y ) );

  return cdf;
}
//******************************************************************************

double extreme_values_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_CDF_INV inverts the Extreme Values CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXTREME_VALUES_CDF_INV, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "EXTREME_VALUES_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a - b * log ( - log ( cdf ) );

  return x;
}
//******************************************************************************

void extreme_values_cdf_values ( int *n_data, double *alpha, double *beta, 
  double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_CDF_VALUES returns some values of the Extreme Values CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = ExtremeValuesDistribution [ alpha, beta ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    05 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *ALPHA, the first parameter of the distribution.
//
//    Output, double *BETA, the second parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double alpha_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 };

  double beta_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double fx_vec[N_MAX] = { 
     0.3678794411714423E+00,  
     0.8734230184931166E+00,  
     0.9818510730616665E+00,  
     0.9975243173927525E+00,  
     0.5452392118926051E+00,  
     0.4884435800065159E+00,  
     0.4589560693076638E+00,  
     0.4409910259429826E+00,  
     0.5452392118926051E+00,  
     0.3678794411714423E+00,  
     0.1922956455479649E+00,  
     0.6598803584531254E-01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *alpha = 0.0;
    *beta = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *alpha = alpha_vec[*n_data-1];
    *beta = beta_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool extreme_values_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_CHECK checks the parameters of the Extreme Values CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool EXTREME_VALUES_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "EXTREME_VALUES_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double extreme_values_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_MEAN returns the mean of the Extreme Values PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXTREME_VALUES_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * euler_constant ( );

  return mean;
}
//******************************************************************************

double extreme_values_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_PDF evaluates the Extreme Values PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 
//      ( 1 / B ) * exp ( ( A - X ) / B - exp ( ( A - X ) / B  ) ).
//
//  Discussion:
//
//    The Extreme Values PDF is also known as the Fisher-Tippet PDF
//    and the Log-Weibull PDF.
//
//    The special case A = 0 and B = 1 is the Gumbel PDF.
//
//    The Extreme Values PDF is the limiting distribution for the
//    smallest or largest value in a large sample drawn from
//    any of a great variety of distributions.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Eric Weisstein, editor,
//    CRC Concise Encylopedia of Mathematics,
//    CRC Press, 1998.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXTREME_VALUES_PDF, the value of the PDF.
//
{
  double pdf;

  pdf = ( 1.0 / b ) * exp ( ( a - x ) / b - exp ( ( a - x ) / b ) );

  return pdf;
}
//******************************************************************************

double extreme_values_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_SAMPLE samples the Extreme Values PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double EXTREME_VALUES_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = extreme_values_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double extreme_values_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    EXTREME_VALUES_VARIANCE returns the variance of the Extreme Values PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double EXTREME_VALUES_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = PI * PI * b * b / 6.0;

  return variance;
# undef PI
}
//******************************************************************************

double f_cdf ( double x, int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_CDF evaluates the F central CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Formula 26.5.28
//    Abramowitz and Stegun,
//    Handbook of Mathematical Functions.
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//
//    Output, double F_CDF, the value of the CDF.
//
{
  double arg1;
  double arg2;
  double arg3;
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    arg1 = 0.5 * ( double ) ( n );
    arg2 = 0.5 * ( double ) ( m );
    arg3 = ( double ) ( n ) / ( ( double ) ( n ) + ( double ) ( m ) * x );

    cdf = beta_inc ( arg1, arg2, arg3 );

  }

  return cdf;
}
//******************************************************************************

void f_cdf_values ( int *n_data, int *a, int *b, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    F_CDF_VALUES returns some values of the F CDF test function.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = FRatioDistribution [ dfn, dfd ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    11 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *A, int B, the parameters of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 20

  int a_vec[N_MAX] = { 
    1, 
    1, 
    5, 
    1, 
    2, 
    4, 
    1, 
    6, 
    8, 
    1, 
    3, 
    6, 
    1, 
    1, 
    1, 
    1, 
    2, 
    3, 
    4, 
    5 };

  int b_vec[N_MAX] = { 
     1, 
     5, 
     1, 
     5, 
    10, 
    20, 
     5, 
     6, 
    16, 
     5, 
    10, 
    12, 
     5, 
     5, 
     5, 
     5, 
     5, 
     5, 
     5,   
     5 };

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.4999714850534485E+00,  
     0.4996034370170990E+00,  
     0.7496993658293228E+00,  
     0.7504656462757382E+00,  
     0.7514156325324275E+00,  
     0.8999867031372156E+00,  
     0.8997127554259699E+00,  
     0.9002845660853669E+00,  
     0.9500248817817622E+00,  
     0.9500574946122442E+00,  
     0.9501926400000000E+00,  
     0.9750133887312993E+00,  
     0.9900022327445249E+00,  
     0.9949977837872073E+00,  
     0.9989999621122122E+00,  
     0.5687988496283079E+00,  
     0.5351452100063650E+00,  
     0.5143428032407864E+00,  
     0.5000000000000000E+00 };

  double x_vec[N_MAX] = { 
      1.00E+00,  
      0.528E+00,  
      1.89E+00,  
      1.69E+00,  
      1.60E+00,  
      1.47E+00,  
      4.06E+00,  
      3.05E+00,  
      2.09E+00,  
      6.61E+00,  
      3.71E+00,  
      3.00E+00,  
     10.01E+00,  
     16.26E+00,  
     22.78E+00,  
     47.18E+00,  
      1.00E+00,  
      1.00E+00,  
      1.00E+00,  
      1.00E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0;
    *b = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *b = b_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool f_check ( int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_CHECK checks the parameters of the F PDF.
//
//  Modified:
//
//    25 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//
//    Output, bool F_CHECK, is true if the parameters are legal.
//
{
  if ( m <= 0 )
  {
    cout << "\n";
    cout << "F_CHECK - Fatal error!\n";
    cout << "  M <= 0.\n";
    return false;
  }

  if ( n <= 0 )
  {
    cout << "\n";
    cout << "F_CHECK - Fatal error!\n";
    cout << "  N <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double f_mean ( int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_MEAN returns the mean of the F central PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//    Note, however, that the mean is not defined unless 3 <= N.
//
//    Output, double F_MEAN, the mean of the PDF.
//
{
  double mean;

  if ( n < 3 )
  {
    cout << " \n";
    cout << "F_MEAN - Fatal error!\n";
    cout << "  The mean is not defined for N < 3.\n";
    exit ( 1 );
  }

  mean = ( double ) ( n ) / ( double ) ( n - 2 );

  return mean;
}
//******************************************************************************

double f_pdf ( double x, int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_PDF evaluates the F central PDF.
//
//  Formula:
//
//    PDF(M,N;X) = M**(M/2) * X**((M-2)/2)
//      / ( Beta(M/2,N/2) * N**(M/2) * ( 1 + (M/N) * X )**((M+N)/2)
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//
//    Output, double F_PDF, the value of the PDF.
//
{
  double a;
  double b;
  double bot1;
  double bot2;
  double pdf;
  double top;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    a = ( double ) ( m );
    b = ( double ) ( n );

    top = sqrt ( pow ( a, m ) * pow ( b, n ) * pow ( x, m - 2 ) );
    bot1 = beta ( a / 2.0, b / 2.0 ) ;
    bot2 = sqrt ( pow ( b + a * x, m + n ) );

    pdf = top / ( bot1 * bot2 );

  }

  return pdf;
}
//******************************************************************************

double f_sample ( int m, int n, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    F_SAMPLE samples the F central PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double F_SAMPLE, a sample of the PDF.
//
{
  double a;
  double x;
  double xm;
  double xn;

  a = ( double ) ( m );
  xm = chi_square_sample ( a, seed );

  a = ( double ) ( n );
  xn = chi_square_sample ( a, seed );

  x = ( double ) ( n ) * xm / ( ( double ) ( m ) * xn );

  return x;
}
//******************************************************************************

double f_variance ( int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_VARIANCE returns the variance of the F central PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//    Note, however, that the variance is not defined unless 5 <= N.
//
//    Output, double F_VARIANCE, the variance of the PDF.
//
{
  double variance;

  if ( n < 5 )
  {
    cout << " \n";
    cout << "F_VARIANCE - Fatal error!\n";
    cout << "  The variance is not defined for N < 5.\n";
    exit ( 1 );
  }

  variance = ( double ) ( 2 * n * n * ( m + n - 2 ) ) / 
    ( double ) ( m * ( n - 2 ) * ( n - 2 ) * ( n - 4 ) );

  return variance;
}
//******************************************************************************

void f_noncentral_cdf_values ( int *n_data, int *n1, int *n2, double *lambda, 
  double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    F_NONCENTRAL_CDF_VALUES returns some values of the F CDF test function.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = NoncentralFRatioDistribution [ n1, n2, lambda ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    30 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *N1, int *N2, the numerator and denominator
//    degrees of freedom.
//
//    Output, double *LAMBDA, the noncentrality parameter.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 22

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.6367825323508774E+00,  
     0.5840916116305482E+00,  
     0.3234431872392788E+00,  
     0.4501187879813550E+00,  
     0.6078881441188312E+00,  
     0.7059275551414605E+00,  
     0.7721782003263727E+00,  
     0.8191049017635072E+00,  
     0.3170348430749965E+00,  
     0.4327218008454471E+00,  
     0.4502696915707327E+00,  
     0.4261881186594096E+00,  
     0.6753687206341544E+00,  
     0.4229108778879005E+00,  
     0.6927667261228938E+00,  
     0.3632174676491226E+00,  
     0.4210054012695865E+00,  
     0.4266672258818927E+00,  
     0.4464016600524644E+00,  
     0.8445888579504827E+00,  
     0.4339300273343604E+00 };

  double lambda_vec[N_MAX] = { 
     0.00E+00,  
     0.00E+00,  
     0.25E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     2.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     2.00E+00,  
     1.00E+00,  
     1.00E+00,  
     0.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00 };

  int n1_vec[N_MAX] = { 
     1,  1,  1,  1, 
     1,  1,  1,  1, 
     1,  1,  2,  2, 
     3,  3,  4,  4, 
     5,  5,  6,  6, 
     8, 16 };

  int n2_vec[N_MAX] = { 
     1,  5,  5,  5, 
     5,  5,  5,  5, 
     5,  5,  5, 10, 
     5,  5,  5,  5, 
     1,  5,  6, 12, 
    16,  8 };

  double x_vec[N_MAX] = { 
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     0.50E+00,  
     1.00E+00,  
     2.00E+00,  
     3.00E+00,  
     4.00E+00,  
     5.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     2.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     2.00E+00,  
     2.00E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *n1 = 0;
    *n2 = 0;
    *lambda = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *n1 = n1_vec[*n_data-1];
    *n2 = n2_vec[*n_data-1];
    *lambda = lambda_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool f_noncentral_check ( double a, int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_NONCENTRAL_CHECK checks the parameters of the F noncentral PDF.
//
//  Modified:
//
//    30 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, a parameter of the PDF.
//
//    Input, int M, N, the parameters of the PDF.
//    1 <= M,
//    1 <= N.
//
//    Output, bool F_NONCENTRAL_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "F_NONCENTRAL_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    exit ( 1 );
  }

  if ( m <= 0 )
  {
    cout << "\n";
    cout << "F_NONCENTRAL_CHECK - Fatal error!\n";
    cout << "  M <= 0.\n";
    return false;
  }

  if ( n <= 0 )
  {
    cout << "\n";
    cout << "F_NONCENTRAL_CHECK - Fatal error!\n";
    cout << "  N <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double f_noncentral_mean ( double a, int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_NONCENTRAL_MEAN returns the mean of the F noncentral PDF.
//
//  Modified:
//
//    26 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, a parameter of the PDF.
//
//    Input, int M, N, parameters of the PDF.
//    1 <= M,
//    1 <= N.
//    Note, however, that the mean is not defined unless 3 <= N.
//
//    Output, double F_NONCENTAL_MEAN, the mean of the PDF.
//
{
  double mean;

  if ( n < 3 )
  {
    cout << " \n";
    cout << "F_NONCENTRAL_MEAN - Fatal error!\n";
    cout << "  The mean is not defined for N < 3.\n";
    exit ( 1 );
  }

  mean = ( ( double ) ( m ) + a ) * ( double ) ( n ) 
    / ( ( double ) ( m ) * ( double ) ( n - 2 ) );

  return mean;
}
//******************************************************************************

double f_noncentral_variance ( double a, int m, int n )

//******************************************************************************
//
//  Purpose:
//
//    F_NONCENTRAL_VARIANCE returns the variance of the F noncentral PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, a parameter of the PDF.
//
//    Input, int M, N, parameters of the PDF.
//    1 <= M,
//    1 <= N.
//    Note, however, that the variance is not defined unless 5 <= N.
//
//    Output, double F_NONCENTRAL_VARIANCE, the variance of the PDF.
//
{
  double mr;
  double nr;
  double variance;

  if ( n < 5 )
  {
    cout << " \n";
    cout << "F_NONCENTRAL_VARIANCE - Fatal error!\n";
    cout << "  The variance is not defined for N < 5.\n";
    exit ( 1 );
  }

  mr = ( double ) ( m );
  nr = ( double ) ( n );

  variance = ( ( mr + a ) * ( mr + a ) + 2.0 * ( mr + a ) * nr * nr ) 
    / ( ( nr - 2.0 ) * ( nr - 4.0 ) * mr * mr ) 
    - ( mr + a ) * ( mr + a ) * nr * nr 
    / ( ( nr - 2.0 ) * ( nr - 2.0 ) * mr * mr );

  return variance;
}
//******************************************************************************

double factorial_log ( int n )

//******************************************************************************
//
//  Purpose:
//
//    FACTORIAL_LOG returns the logarithm of N!.
//
//  Definition:
//
//    N! = Product ( 1 <= I <= N ) I
//
//  Method:
//
//    N! = Gamma(N+1).
//
//  Modified:
//
//    11 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the argument of the function.
//    0 <= N.
//
//    Output, double FACTORIAL_LOG, the logarithm of N!.
//
{
  int i;
  double value;

  if ( n < 0 )
  {
    cout << "\n";
    cout << "FACTORIAL_LOG - Fatal error!\n";
    cout << "  N < 0.\n";
    exit ( 1 );
  }

  value = 0.0;

  for ( i = 2; i <= n; i++ )
  {
    value = value + log ( ( double ) ( i ) );
  }

  return value;
}
//******************************************************************************

double factorial_stirling ( int n )

//******************************************************************************
//
//  Purpose:
//
//    FACTORIAL_STIRLING computes Stirling's approximation to N!.
//
//  Definition:
//
//    N! = Product ( 1 <= I <= N ) I
//
//    Stirling ( N ) = sqrt ( 2 * PI * N ) * ( N / E )**N * E**(1/(12*N) )
//
//  Discussion:
//
//    This routine returns the raw approximation for all nonnegative
//    values of N.  If N is less than 0, the value is returned as 0,
//    and if N is 0, the value of 1 is returned.  In all other cases,
//    Stirling's formula is used.
//
//  Modified:
//
//    01 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the argument of the function.
//
//    Output, double FACTORIAL_STIRLING, an approximation to N!.
//
{
# define PI 3.141592653589793

  double e_natural = 2.71828182845904523536028747135266249775724709369995;
  double value;

  if ( n < 0 )
  {
    value = 0.0;
  }
  else if ( n == 0 )
  {
    value = 1.0;
  }
  else
  {
    value = sqrt ( 2.0 * PI * ( double ) ( n ) ) 
      * pow ( ( double ) ( n ) / e_natural, n ) 
      * exp ( 1.0 / ( double ) ( 12 * n ) );
  }

  return value;
# undef PI
}
//******************************************************************************

double fisk_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_CDF evaluates the Fisk CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double FISK_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 / ( 1.0 + pow ( ( b / ( x - a ) ), c ) );
  }

  return cdf;
}
//******************************************************************************

double fisk_cdf_inv ( double cdf, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_CDF_INV inverts the Fisk CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double FISK_CDF_INV, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "FISK_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf <= 0.0 )
  {
    x = a;
  }
  else if ( cdf < 1.0 )
  {
    x = a + b * pow ( cdf / ( 1.0 - cdf ), 1.0 / c );
  }
  else if ( 1.0 <= cdf )
  {
    x = d_huge ( );
  }

  return x;
}
//******************************************************************************

bool fisk_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_CHECK checks the parameters of the Fisk PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, bool FISK_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "FISK_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "FISK_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double fisk_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_MEAN returns the mean of the Fisk PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double FISK_MEAN, the mean of the PDF.
//
{
# define PI 3.141592653589793

  double mean;

  if ( c <= 1.0 )
  {
    cout << " \n";
    cout << "FISK_MEAN - Fatal error!\n";
    cout << "  No mean defined for C <= 1.0\n";
    exit ( 1 );
  }

  mean = a + PI * ( b / c ) * csc ( PI / c );

  return mean;
# undef PI
}
//******************************************************************************

double fisk_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_PDF evaluates the Fisk PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = 
//      ( C / B ) * ( ( X - A ) / B )**( C - 1 ) /
//      ( 1 + ( ( X - A ) / B )**C )**2
//
//  Discussion:
//
//    The Fisk PDF is also known as the Log Logistic PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double FISK_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = ( c / b ) * pow ( y, ( c - 1.0 ) ) / pow ( ( 1.0 + pow ( y, c ) ), 2 );
  }

  return pdf;
}
//******************************************************************************

double fisk_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    FISK_SAMPLE samples the Fisk PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double FISK_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = fisk_cdf_inv ( cdf, a, b, c );

  return x;
}
//******************************************************************************

double fisk_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    FISK_VARIANCE returns the variance of the Fisk PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double FISK_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double g;
  double variance;

  if ( c <= 2.0 )
  {
    cout << " \n";
    cout << "FISK_VARIANCE - Fatal error!\n";
    cout << "  No variance defined for C <= 2.0\n";
    exit ( 1 );
  }

  g = PI / c;

  variance = b * b * ( 2.0 * g * csc ( 2.0 * g ) - pow ( ( g * csc ( g ) ), 2 ) );

  return variance;
# undef PI
}
//******************************************************************************

double folded_normal_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_CDF evaluates the Folded Normal CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//    0.0 <= X.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, double FOLDED_NORMAL_CDF, the value of the CDF.
//
{
  double cdf;
  double cdf1;
  double cdf2;
  double x1;
  double x2;

  if ( x < 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    x1 = ( x - a ) / b;
    cdf1 = normal_01_cdf ( x1 );
    x2 = ( - x - a ) / b;
    cdf2 = normal_01_cdf ( x2 );
    cdf = cdf1 - cdf2;
  }

  return cdf;
}
//******************************************************************************

double folded_normal_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_CDF_INV inverts the Folded Normal CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, double FOLDED_NORMAL_CDF_INV, the argument of the CDF.
//    0.0 <= X.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;
  double xa;
  double xb;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "FOLDED_NORMAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = 0.0;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
    return x;
  }
//
//  Find X1, for which the value of CDF will be too small.
//
  if ( 0.0 <= a )
  {
    x1 = normal_cdf_inv ( cdf, a, b );
  }
  else
  {
    x1 = normal_cdf_inv ( cdf, -a, b );
  }
  x1 = d_max ( x1, 0.0 );
  cdf1 = folded_normal_cdf ( x1, a, b );
//
//  Find X2, for which the value of CDF will be too big.
//
  cdf2 = ( 1.0 - cdf ) / 2.0;

  xa = normal_cdf_inv ( cdf2, a, b );
  xb = normal_cdf_inv ( cdf2, -a, b );
  x2 = d_max ( fabs ( xa ), fabs ( xb ) );
  cdf2 = folded_normal_cdf ( x2, a, b );
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = folded_normal_cdf ( x3, a, b );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "FOLDED_NORMAL_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      exit ( 1 );
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }
  }

  return x;
}
//******************************************************************************

bool folded_normal_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_CHECK checks the parameters of the Folded Normal CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, bool FOLDED_NORMAL_CHECK, is true if the parameters are legal.
//
{
  if ( a < 0.0 )
  {
    cout << " \n";
    cout << "FOLDED_NORMAL_CHECK - Fatal error!\n";
    cout << "  A < 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "FOLDED_NORMAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double folded_normal_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_MEAN returns the mean of the Folded Normal PDF.
// 
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, double FOLDED_NORMAL_MEAN, the mean of the PDF.
//
{
# define PI 3.141592653589793

  double a2;
  double cdf;
  double mean;

  a2 = a / b;

  cdf = normal_01_cdf ( a2 );

  mean = b * sqrt ( 2.0 / PI ) * exp ( - 0.5 * a2 * a2 ) 
    - a * ( 1.0 - 2.0 * cdf );

  return mean;
# undef PI
}
//******************************************************************************

double folded_normal_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_PDF evaluates the Folded Normal PDF.
//
//  Formula:
//
//    PDF(A;X) = sqrt ( 2 / PI ) * ( 1 / B ) * COSH ( A * X / B**2 )
//      * EXP ( - 0.5 * ( X**2 + A**2 ) / B**2 )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, double FOLDED_NORMAL_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = sqrt ( 2.0 / PI ) * ( 1.0 / b ) * cosh ( a * x / b / b ) 
      * exp ( - 0.5 * ( x * x + a * a ) / b / b );
  }

  return pdf;
# undef PI
}
//******************************************************************************

double folded_normal_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_SAMPLE samples the Folded Normal PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double FOLDED_NORMAL_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );
  
  x = folded_normal_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double folded_normal_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    FOLDED_NORMAL_VARIANCE returns the variance of the Folded Normal PDF.
// 
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A,
//    0.0 < B.
//
//    Output, double FOLDED_NORMAL_VARIANCE, the variance of the PDF.
//
{
  double mean;
  double variance;

  mean = folded_normal_mean ( a, b );

  variance = a * a + b * b - mean * mean;

  return variance;
}
//******************************************************************************

double gamma_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_CDF evaluates the Gamma CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B, 
//    0.0 < C.
//
//    Output, double GAMMA_CDF, the value of the CDF.
//
{
  double cdf;
  double p2;
  double x2;

  x2 = ( x - a ) / b;
  p2 = c;

  cdf = gamma_inc ( p2, x2 );

  return cdf;
}
//******************************************************************************

void gamma_cdf_values ( int *n_data, double *mu, double *sigma, double *x,
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_CDF_VALUES returns some values of the Gamma CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = GammaDistribution [ mu, sigma ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    05 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *SIGMA, the variance of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double fx_vec[N_MAX] = { 
     0.8646647167633873E+00,  
     0.9816843611112658E+00,  
     0.9975212478233336E+00,  
     0.9996645373720975E+00,  
     0.6321205588285577E+00,  
     0.4865828809674080E+00,  
     0.3934693402873666E+00,  
     0.3296799539643607E+00,  
     0.4421745996289254E+00,  
     0.1911531694619419E+00,  
     0.6564245437845009E-01,  
     0.1857593622214067E-01 };

  double mu_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double sigma_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *sigma = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *sigma = sigma_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool gamma_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_CHECK checks the parameters of the Gamma PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, bool GAMMA_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "GAMMA_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    cout << "  B = " << b << "\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "GAMMA_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    cout << "  C = " << c << "\n";
    return false;
  }

  return true;
}
//******************************************************************************

double gamma_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_MEAN returns the mean of the Gamma PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double GAMMA_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * c;

  return mean;
}
//******************************************************************************

double gamma_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_PDF evaluates the Gamma PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = EXP ( - ( X - A ) / B ) * ( ( X - A ) / B )**(C-1) 
//      / ( B * GAMMA ( C ) )
//
//  Discussion:
//
//    GAMMA_PDF(A,B,C;X), where C is an integer, is the Erlang PDF.
//    GAMMA_PDF(A,B,1;X) is the Exponential PDF.
//    GAMMA_PDF(0,2,C/2;X) is the Chi Squared PDF with C degrees of freedom.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B, 
//    0.0 < C.
//
//    Output, double GAMMA_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = pow ( y, c - 1.0 ) / ( b * gamma ( c ) * exp ( y ) );
  }

  return pdf;
}
//******************************************************************************

double gamma_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_SAMPLE samples the Gamma PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Reference:
//
//    J H Ahrens and U Dieter,
//    Generating Gamma Variates by a Modified Rejection Technique,
//    Communications of the ACM, 
//    Volume 25, Number 1, January 1982, pages 47 - 54.
//
//    J H Ahrens and U Dieter,
//    Computer Methods for Sampling from Gamma, Beta, Poisson and
//      Binomial Distributions.
//    Computing, Volume 12, 1974, pages 223 - 246.
//
//    J H Ahrens, K D Kohrt, and U Dieter,
//    Algorithm 599,
//    ACM Transactions on Mathematical Software,
//    Volume 9, Number 2, June 1983, pages 255-257.
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B, 
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double a1 =   0.3333333;
  double a2 = - 0.2500030;
  double a3 =   0.2000062;
  double a4 = - 0.1662921;
  double a5 =   0.1423657;
  double a6 = - 0.1367177;
  double a7 =   0.1233795;
  double bcoef;
  double co;
  double d;
  double e;
  double e1 = 1.0;
  double e2 = 0.4999897;
  double e3 = 0.1668290;
  double e4 = 0.0407753;
  double e5 = 0.0102930;
  double euler = 2.71828182845904;
  double p;
  double q;
  double q0;
  double q1 =   0.04166669;
  double q2 =   0.02083148;
  double q3 =   0.00801191;
  double q4 =   0.00144121;
  double q5 = - 0.00007388;
  double q6 =   0.00024511;
  double q7 =   0.00024240;
  double r;
  double s;
  double si;
  double s2;
  double t;
  double u;
  double v;
  double w;
  double x;
//
//  Allow C = 0.
//
  if ( c == 0.0 )
  {
    x = a;
    return x;
  }
//
//  C < 1.
//
  if ( c < 1.0 )
  {
    for ( ; ; )
    {
      u = d_uniform_01 ( seed );
      t = 1.0 + c / euler;
      p = u * t;

      s = exponential_01_sample ( seed );

      if ( p < 1.0 )
      {
        x = exp ( log ( p ) / c );
        if ( x <= s )
        {
          break;
        }
      }
      else
      {
        x = - log ( ( t - p ) / c );
        if ( ( 1.0 - c ) * log ( x ) <= s )
        {
          break;
        }
      }
    }

    x = a + b * x;
    return x;
  }
//
//  1 <= C.
//
  else
  {
    s2 = c - 0.5;
    s = sqrt ( c - 0.5 );
    d = sqrt ( 32.0 ) - 12.0 * sqrt ( c - 0.5 );

    t = normal_01_sample ( seed );
    x = pow ( ( sqrt ( c - 0.5 ) + 0.5 * t ), 2 );

    if ( 0.0 <= t )
    {
      x = a + b * x;
      return x;
    }

    u = d_uniform_01 ( seed );

    if ( d * u <= t * t * t )
    {
      x = a + b * x;
      return x;
    }

    r = 1.0 / c;

    q0 = ( ( ( ( ( ( 
           q7   * r 
         + q6 ) * r 
         + q5 ) * r 
         + q4 ) * r 
         + q3 ) * r 
         + q2 ) * r 
         + q1 ) * r;

    if ( c <= 3.686 )
    {
      bcoef = 0.463 + s - 0.178 * s2;
      si = 1.235;
      co = 0.195 / s - 0.079 + 0.016 * s;
    }
    else if ( c <= 13.022 )
    {
      bcoef = 1.654 + 0.0076 * s2;
      si = 1.68 / s + 0.275;
      co = 0.062 / s + 0.024;
    }
    else
    {
      bcoef = 1.77;
      si = 0.75;
      co = 0.1515 / s;
    }

    if ( 0.0 < sqrt ( c - 0.5 ) + 0.5 * t )
    {
      v = 0.5 * t / s;

      if ( 0.25 < fabs ( v ) )
      {
        q = q0 - s * t + 0.25 * t * t + 2.0 * s2 * log ( 1.0 + v );
      }
      else
      {
        q = q0 + 0.5 * t * t * ( ( ( ( ( ( 
               a7   * v 
             + a6 ) * v 
             + a5 ) * v 
             + a4 ) * v 
             + a3 ) * v 
             + a2 ) * v 
             + a1 ) * v;
      }

      if ( log ( 1.0 - u ) <= q )
      {
        x = a + b * x;
        return x;
      }
    }

    for ( ; ; )
    {
      e = exponential_01_sample ( seed );

      u = d_uniform_01 ( seed );

      u = 2.0 * u - 1.0;
      t = bcoef + fabs ( si * e ) * d_sign ( u );

      if ( -0.7187449 <= t )
      {
        v = 0.5 * t / s;

        if ( 0.25 < fabs ( v ) )
        {
          q = q0 - s * t + 0.25 * t * t + 2.0 * s2 * log ( 1.0 + v );
        }
        else
        {
          q = q0 + 0.5 * t * t * ( ( ( ( ( ( 
               a7   * v 
             + a6 ) * v 
             + a5 ) * v 
             + a4 ) * v 
             + a3 ) * v 
             + a2 ) * v 
             + a1 ) * v;
        }

        if ( 0.0 < q )
        {
          if ( 0.5 < q )
          {
            w = exp ( q ) - 1.0; 
          }
          else
          {
            w = ( ( ( ( 
                    e5   * q 
                  + e4 ) * q 
                  + e3 ) * q 
                  + e2 ) * q 
                  + e1 ) * q;
          }

          if ( co * fabs ( u ) <= w * exp ( e - 0.5 * t * t ) )
          {
            x = a + b * pow ( s + 0.5 * t, 2 );
            return x;
          }
        }
      }
    }
  }
}
//******************************************************************************

double gamma_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_VARIANCE returns the variance of the Gamma PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b * c;

  return variance;
}
//******************************************************************************

double gamma ( double x )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA calculates the Gamma function for a real argument X.
//
//  Definition:
//
//    GAMMA(X) = Integral ( 0 <= T <= Infinity ) T**(X-1) EXP(-T) DT
//
//  Recursion:
//
//    GAMMA(X+1) = X * GAMMA(X)
//
//  Special values:
//
//    GAMMA(0.5) = SQRT(PI)
//    If N is a positive integer, GAMMA(N+1) = N!, the standard factorial.
//
//  Discussion:
//
//    Computation is based on an algorithm outlined in reference 1.
//    The program uses rational functions that approximate the GAMMA
//    function to at least 20 significant decimal digits.  Coefficients
//    for the approximation over the interval (1,2) are unpublished.
//    Those for the approximation for X .GE. 12 are from reference 2.
//    The accuracy achieved depends on the arithmetic system, the
//    compiler, the intrinsic functions, and proper selection of the
//    machine-dependent constants.
//
//  Machine-dependent constants:
//
//    BETA: radix for the floating-point representation.
//    MAXEXP: the smallest positive power of BETA that overflows.
//    XBIG: the largest argument for which GAMMA(X) is representable
//      in the machine, i.e., the solution to the equation
//      GAMMA(XBIG) = BETA**MAXEXP.
//    XMININ: the smallest positive floating-point number such that
//      1/XMININ is machine representable.
//
//    Approximate values for some important machines are:
//
//                               BETA       MAXEXP        XBIG
//
//    CRAY-1         (S.P.)        2         8191        966.961
//    Cyber 180/855
//      under NOS    (S.P.)        2         1070        177.803
//    IEEE (IBM/XT,
//      SUN, etc.)   (S.P.)        2          128        35.040
//    IEEE (IBM/XT,
//      SUN, etc.)   (D.P.)        2         1024        171.624
//    IBM 3033       (D.P.)       16           63        57.574
//    VAX D-Format   (D.P.)        2          127        34.844
//    VAX G-Format   (D.P.)        2         1023        171.489
//
//                              XMININ
//
//    CRAY-1         (S.P.)   1.84E-2466
//    Cyber 180/855
//      under NOS    (S.P.)   3.14E-294
//    IEEE (IBM/XT,
//      SUN, etc.)   (S.P.)   1.18E-38
//    IEEE (IBM/XT,
//      SUN, etc.)   (D.P.)   2.23D-308
//    IBM 3033       (D.P.)   1.39D-76
//    VAX D-Format   (D.P.)   5.88D-39
//    VAX G-Format   (D.P.)   1.12D-308
//
//  Author: 
//
//    W. J. Cody and L. Stoltz,
//    Applied Mathematics Division,
//    Argonne National Laboratory,
//    Argonne, Illinois, 60439.
//
//  Reference: 
//
//    W J Cody,
//    "An Overview of Software Development for Special Functions", 
//    Lecture Notes in Mathematics, 506, 
//    Numerical Analysis Dundee, 1975, 
//    G. A. Watson (ed.),
//    Springer Verlag, Berlin, 1976.
//
//    Hart et al,
//    Computer Approximations, 
//    Wiley and sons, New York, 1968.
//
//  Parameters:
//
//    Input, double X, the argument of the function.
//
//    Output, double GAMMA, the value of the function. 
//    The computation is believed to be free of underflow and overflow.
//
{
  double c[7] = {
    -1.910444077728E-03, 
     8.4171387781295E-04, 
    -5.952379913043012E-04, 
     7.93650793500350248E-04, 
    -2.777777777777681622553E-03, 
     8.333333333333333331554247E-02, 
     5.7083835261E-03 };
  double fact;
  int i;
  int n;
  double p[8] = {
    -1.71618513886549492533811E+00, 
     2.47656508055759199108314E+01, 
    -3.79804256470945635097577E+02, 
     6.29331155312818442661052E+02, 
     8.66966202790413211295064E+02, 
    -3.14512729688483675254357E+04, 
    -3.61444134186911729807069E+04, 
     6.64561438202405440627855E+04 };
  bool parity;
  double pi = 
    3.141592653589793E+00;
  double q[8] = {
    -3.08402300119738975254353E+01, 
     3.15350626979604161529144E+02,
    -1.01515636749021914166146E+03,
     -3.10777167157231109440444E+03, 
     2.25381184209801510330112E+04, 
     4.75584627752788110767815E+03, 
    -1.34659959864969306392456E+05, 
    -1.15132259675553483497211E+05 };
  double sqrtpi = 0.9189385332046727417803297E+00;
  double sum2;
  double value;
  double xbig = 35.040E+00;
  double xden;
  double xminin = 1.18E-38;
  double xnum;
  double y;
  double y1;
  double ysq;
  double z;
//
  parity = false;
  fact = 1.0;
  n = 0;
  y = x;
//
//  Argument is negative.
//
  if ( y <= 0.0 )
  {
    y = -x;
    y1 = ( double ) ( ( int ) ( y ) );
    value = y - y1;

    if ( value != 0.0 )
    {
      if ( y1 != ( double ) ( ( int ) ( y1 * 0.5 ) ) * 2.0 )
      {
        parity = true;
      }

      fact = -pi / sin ( pi * value );
      y = y + 1.0;
    }
    else
    {
      value = d_huge ( );
      return value;
    }

  }
//
//  Argument < EPS
//
  if ( y < d_epsilon ( ) )
  {
    if ( xminin <= y )
    {
      value = 1.0 / y;
    }
    else
    {
      value = d_huge ( );
      return value;
    }
  }
  else if ( y < 12.0 )
  {
    y1 = y;
//
//  0.0 < argument < 1.0
//
    if ( y < 1.0 )
    {
      z = y;
      y = y + 1.0;
    }
//
//  1.0 < argument < 12.0, reduce argument if necessary.
//
    else
    {
      n = int ( y ) - 1;
      y = y - ( double ) ( n );
      z = y - 1.0;
    }
//
//  Evaluate approximation for 1.0 < argument < 2.0.
//
    xnum = 0.0;
    xden = 1.0;
    for ( i = 0; i < 8; i++ )
    {
      xnum = ( xnum + p[i] ) * z;
      xden = xden * z + q[i];
    }

    value = xnum / xden + 1.0;
//
//  Adjust result for case  0.0 < argument < 1.0.
//
    if ( y1 < y )
    {
      value = value / y1;
    }
//
//  Adjust result for case  2.0 < argument < 12.0.
//
    else if ( y < y1 )
    {
      for ( i = 1; i <= n; i++ )
      {
        value = value * y;
        y = y + 1.0;
      }

    }
  }
//
//  Evaluate for 12 <= argument.
//
  else
  {
    if ( y <= xbig )
    {
      ysq = y * y;
      sum2 = c[6];
      for ( i = 0; i < 6; i++ )
      {
        sum2 = sum2 / ysq + c[i];
      }
      sum2 = sum2 / y - y + sqrtpi;
      sum2 = sum2 + ( y - 0.5 ) * log ( y );
      value = exp ( sum2 );
    }
    else
    {
      value = d_huge ( );
      return value;

    }

  }
//
//  Final adjustments and return.
//
  if ( parity )
  {
    value = -value;
  }

  if ( fact != 1.0 )
  {
    value = fact / value;
  }

  return value;
}
//******************************************************************************

double gamma_inc ( double p, double x )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_INC computes the incomplete Gamma function.
//
//  Definition:
//
//    GAMMA_INC(P,X) = Integral ( 0 <= T <= X ) T**(P-1) EXP(-T) DT / GAMMA(P).
//
//  Discussion:
//
//    GAMMA_INC(P,       0) = 0, 
//    GAMMA_INC(P,Infinity) = 1.
//
//  Modified:
//
//    16 October 2004
//
//  Reference:
//
//    B L Shea,
//    Chi-squared and Incomplete Gamma Integral,
//    Algorithm AS239,
//    Applied Statistics,
//    Volume 37, Number 3, 1988, pages 466-473.
//
//  Parameters:
//
//    Input, double P, the exponent parameter.
//    0.0 < P.
//
//    Input, double X, the integral limit parameter.
//    If X is less than or equal to 0, GAMMA_INC is returned as 0.
//
//    Output, double GAMMA_INC, the value of the function.
//
{
  double a;
  double arg;
  double b;
  double c;
  double cdf;
  double exp_arg_min = -88.0E+00;
  double overflow = 1.0E+37;
  double plimit = 1000.0E+00;
  double pn1;
  double pn2;
  double pn3;
  double pn4;
  double pn5;
  double pn6;
  double rn;
  double value;
  double tol = 1.0E-07;
  double xbig = 1.0E+08;

  value = 0.0;

  if ( p <= 0.0 )
  {
    cout << " \n";
    cout << "GAMMA_INC - Fatal error!\n";
    cout << "  Parameter P <= 0.\n";
    exit ( 1 );
  }

  if ( x <= 0.0 )
  {
    value = 0.0;
    return value;
  }
//
//  Use a normal approximation if PLIMIT < P.
//
  if ( plimit < p )
  {
    pn1 = 3.0 * sqrt ( p ) * ( pow ( x / p, 1.0 / 3.0 )
      + 1.0 / ( 9.0 * p ) - 1.0 );
    value = normal_01_cdf ( pn1 );
    return value;
  }
//
//  Is X extremely large compared to P?
//
  if ( xbig < x )
  {
    value = 1.0;
    return value;
  }
//
//  Use Pearson's series expansion.
//  (P is not large enough to force overflow in the log of Gamma.
//
  if ( x <= 1.0 || x < p )
  {
    arg = p * log ( x ) - x - gamma_log ( p + 1.0 );
    c = 1.0;
    value = 1.0;
    a = p;

    for ( ; ; )
    {
      a = a + 1.0;
      c = c * x / a;
      value = value + c;

      if ( c <= tol )
      {
        break;
      }
    }

    arg = arg + log ( value );

    if ( exp_arg_min <= arg )
    {
      value = exp ( arg );
    }
    else
    {
      value = 0.0;
    }
  }
//
//  Use a continued fraction expansion.
//
  else
  {
    arg = p * log ( x ) - x - gamma_log ( p );
    a = 1.0 - p;
    b = a + x + 1.0;
    c = 0.0;
    pn1 = 1.0;
    pn2 = x;
    pn3 = x + 1.0;
    pn4 = x * b;
    value = pn3 / pn4;

    for ( ; ; )
    {
      a = a + 1.0;
      b = b + 2.0;
      c = c + 1.0;
      pn5 = b * pn3 - a * c * pn1;
      pn6 = b * pn4 - a * c * pn2;

      if ( 0.0 < fabs ( pn6 ) )
      {
        rn = pn5 / pn6;

        if ( fabs ( value - rn ) <= d_min ( tol, tol * rn ) )
        {
          arg = arg + log ( value );

          if ( exp_arg_min <= arg )
          {
            value = 1.0 - exp ( arg );
          }
          else
          {
            value = 1.0;
          }

          return value;
        }
        value = rn;
      }
      pn1 = pn3;
      pn2 = pn4;
      pn3 = pn5;
      pn4 = pn6;
//
//  Rescale terms in continued fraction if terms are large.
//
      if ( overflow <= fabs ( pn5 ) )
      {
        pn1 = pn1 / overflow;
        pn2 = pn2 / overflow;
        pn3 = pn3 / overflow;
        pn4 = pn4 / overflow;
      }
    }
  }

  return value;
}
//******************************************************************************

void gamma_inc_values ( int *n_data, double *a, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_INC_VALUES returns some values of the incomplete Gamma function.
//
//  Discussion:
//
//    The (normalized) incomplete Gamma function P(A,X) is defined as:
//
//      PN(A,X) = 1/Gamma(A) * Integral ( 0 <= T <= X ) T**(A-1) * exp(-T) dT.
//
//    With this definition, for all A and X,
//
//      0 <= PN(A,X) <= 1
//
//    and
//
//      PN(A,INFINITY) = 1.0
//
//    In Mathematica, the function can be evaluated by:
//
//      1 - GammaRegularized[A,X]
//
//  Modified:
//
//    28 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *A, the parameter of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 20

  double a_vec[N_MAX] = { 
     0.10E+00,  
     0.10E+00,  
     0.10E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.10E+01,  
     0.10E+01,  
     0.10E+01,  
     0.11E+01,  
     0.11E+01,  
     0.11E+01,  
     0.20E+01,  
     0.20E+01,  
     0.20E+01,  
     0.60E+01,  
     0.60E+01,  
     0.11E+02,  
     0.26E+02,  
     0.41E+02  };

  double fx_vec[N_MAX] = { 
     0.7382350532339351E+00,  
     0.9083579897300343E+00,  
     0.9886559833621947E+00,  
     0.3014646416966613E+00,  
     0.7793286380801532E+00,  
     0.9918490284064973E+00,  
     0.9516258196404043E-01,  
     0.6321205588285577E+00,  
     0.9932620530009145E+00,  
     0.7205974576054322E-01,  
     0.5891809618706485E+00,  
     0.9915368159845525E+00,  
     0.1392920235749422E+00,  
     0.7768698398515702E+00,  
     0.9990881180344455E+00,  
     0.4202103819530612E-01,  
     0.9796589705830716E+00,  
     0.9226039842296429E+00,  
     0.4470785799755852E+00,  
     0.7444549220718699E+00 };

  double x_vec[N_MAX] = { 
     0.30E-01,  
     0.30E+00,  
     0.15E+01,  
     0.75E-01,  
     0.75E+00,  
     0.35E+01,  
     0.10E+00,  
     0.10E+01,  
     0.50E+01,  
     0.10E+00,   
     0.10E+01,  
     0.50E+01,  
     0.15E+00,  
     0.15E+01,  
     0.70E+01,  
     0.25E+01,  
     0.12E+02,  
     0.16E+02,  
     0.25E+02,  
     0.45E+02 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double gamma_log ( double x )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_LOG calculates the natural logarithm of GAMMA ( X ) for positive X.
//
//  Discussion:
//
//    Computation is based on an algorithm outlined in references 1 and 2.
//    The program uses rational functions that theoretically approximate
//    LOG(GAMMA(X)) to at least 18 significant decimal digits.  The
//    approximation for 12 < X is from reference 3, while approximations
//    for X < 12.0 are similar to those in reference 1, but are unpublished.
//    The accuracy achieved depends on the arithmetic system, the compiler,
//    intrinsic functions, and proper selection of the machine-dependent
//    constants.
//
//  Modified:
//
//    12 May 2003
//
//  Author:
//
//    W. J. Cody and L. Stoltz
//    Argonne National Laboratory
//
//  Reference:
//
//    # 1)
//    W. J. Cody and K. E. Hillstrom,
//    Chebyshev Approximations for the Natural Logarithm of the Gamma Function,
//    Mathematics of Computation,
//    Volume 21, 1967, pages 198-203.
//
//    # 2)
//    K. E. Hillstrom,
//    ANL/AMD Program ANLC366S, DGAMMA/DLGAMA,
//    May 1969.
//
//    # 3)
//    Hart, Et. Al.,
//    Computer Approximations,
//    Wiley and sons, New York, 1968.
//
//  Parameters:
//
//    Input, double X, the argument of the Gamma function.  X must be positive.
//
//    Output, double GAMMA_LOG, the logarithm of the Gamma function of X.
//    If X <= 0.0, or if overflow would occur, the program returns the
//    value XINF, the largest representable floating point number.
//
//*******************************************************************************
//
//  Explanation of machine-dependent constants
//
//  BETA   - radix for the floating-point representation.
//
//  MAXEXP - the smallest positive power of BETA that overflows.
//
//  XBIG   - largest argument for which LN(GAMMA(X)) is representable
//           in the machine, i.e., the solution to the equation
//             LN(GAMMA(XBIG)) = BETA**MAXEXP.
//
//  FRTBIG - Rough estimate of the fourth root of XBIG
//
//
//  Approximate values for some important machines are:
//
//                            BETA      MAXEXP         XBIG
//
//  CRAY-1        (S.P.)        2        8191       9.62E+2461
//  Cyber 180/855
//    under NOS   (S.P.)        2        1070       1.72E+319
//  IEEE (IBM/XT,
//    SUN, etc.)  (S.P.)        2         128       4.08E+36
//  IEEE (IBM/XT,
//    SUN, etc.)  (D.P.)        2        1024       2.55D+305
//  IBM 3033      (D.P.)       16          63       4.29D+73
//  VAX D-Format  (D.P.)        2         127       2.05D+36
//  VAX G-Format  (D.P.)        2        1023       1.28D+305
//
//
//                           FRTBIG
//
//  CRAY-1        (S.P.)   3.13E+615
//  Cyber 180/855
//    under NOS   (S.P.)   6.44E+79
//  IEEE (IBM/XT,
//    SUN, etc.)  (S.P.)   1.42E+9
//  IEEE (IBM/XT,
//    SUN, etc.)  (D.P.)   2.25D+76
//  IBM 3033      (D.P.)   2.56D+18
//  VAX D-Format  (D.P.)   1.20D+9
//  VAX G-Format  (D.P.)   1.89D+76
//
{
  double c[7] = {
    -1.910444077728E-03, 
     8.4171387781295E-04, 
    -5.952379913043012E-04, 
     7.93650793500350248E-04, 
    -2.777777777777681622553E-03, 
     8.333333333333333331554247E-02, 
     5.7083835261E-03 };
  double corr;
  double d1 = - 5.772156649015328605195174E-01;
  double d2 =   4.227843350984671393993777E-01;
  double d4 =   1.791759469228055000094023E+00;
  double frtbig = 1.42E+09;
  int i;
  double p1[8] = {
    4.945235359296727046734888E+00, 
    2.018112620856775083915565E+02, 
    2.290838373831346393026739E+03, 
    1.131967205903380828685045E+04, 
    2.855724635671635335736389E+04, 
    3.848496228443793359990269E+04, 
    2.637748787624195437963534E+04, 
    7.225813979700288197698961E+03 };
  double p2[8] = {
    4.974607845568932035012064E+00, 
    5.424138599891070494101986E+02, 
    1.550693864978364947665077E+04, 
    1.847932904445632425417223E+05, 
    1.088204769468828767498470E+06, 
    3.338152967987029735917223E+06, 
    5.106661678927352456275255E+06, 
    3.074109054850539556250927E+06 };
  double p4[8] = {
    1.474502166059939948905062E+04, 
    2.426813369486704502836312E+06, 
    1.214755574045093227939592E+08, 
    2.663432449630976949898078E+09, 
    2.940378956634553899906876E+010,
    1.702665737765398868392998E+011,
    4.926125793377430887588120E+011, 
    5.606251856223951465078242E+011 };
  double pnt68 = 0.6796875E+00;
  double q1[8] = {
    6.748212550303777196073036E+01, 
    1.113332393857199323513008E+03, 
    7.738757056935398733233834E+03, 
    2.763987074403340708898585E+04, 
    5.499310206226157329794414E+04, 
    6.161122180066002127833352E+04, 
    3.635127591501940507276287E+04, 
    8.785536302431013170870835E+03 };
  double q2[8] = {
    1.830328399370592604055942E+02, 
    7.765049321445005871323047E+03, 
    1.331903827966074194402448E+05, 
    1.136705821321969608938755E+06, 
    5.267964117437946917577538E+06, 
    1.346701454311101692290052E+07, 
    1.782736530353274213975932E+07, 
    9.533095591844353613395747E+06 };
  double q4[8] = {
    2.690530175870899333379843E+03, 
    6.393885654300092398984238E+05, 
    4.135599930241388052042842E+07, 
    1.120872109616147941376570E+09, 
    1.488613728678813811542398E+010, 
    1.016803586272438228077304E+011, 
    3.417476345507377132798597E+011, 
    4.463158187419713286462081E+011 };
  double res;
  double sqrtpi = 0.9189385332046727417803297E+00;
  double xbig = 4.08E+36;
  double xden;
  double xm1;
  double xm2;
  double xm4;
  double xnum;
  double xsq;
//
//  Return immediately if the argument is out of range.
//
  if ( x <= 0.0 || xbig < x )
  {
    return d_huge ( );
  }

  if ( x <= d_epsilon ( ) )
  {
    res = - log ( x );
  }
  else if ( x <= 1.5 )
  {
    if ( x < pnt68 )
    {
      corr = - log ( x );
      xm1 = x;
    }
    else
    {
      corr = 0.0;
      xm1 = ( x - 0.5 ) - 0.5;
    }

    if ( x <= 0.5 || pnt68 <= x )
    {
      xden = 1.0;
      xnum = 0.0;

      for ( i = 0; i < 8; i++ )
      {
        xnum = xnum * xm1 + p1[i];
        xden = xden * xm1 + q1[i];
      }

      res = corr + ( xm1 * ( d1 + xm1 * ( xnum / xden ) ) );
    }
    else
    {
      xm2 = ( x - 0.5 ) - 0.5;
      xden = 1.0;
      xnum = 0.0;
      for ( i = 0; i < 8; i++ )
      {
        xnum = xnum * xm2 + p2[i];
        xden = xden * xm2 + q2[i];
      }

      res = corr + xm2 * ( d2 + xm2 * ( xnum / xden ) );

    }
  }
  else if ( x <= 4.0 )
  {
    xm2 = x - 2.0;
    xden = 1.0;
    xnum = 0.0;
    for ( i = 0; i < 8; i++ )
    {
      xnum = xnum * xm2 + p2[i];
      xden = xden * xm2 + q2[i];
    }

    res = xm2 * ( d2 + xm2 * ( xnum / xden ) );
  }
  else if ( x <= 12.0 )
  {
    xm4 = x - 4.0;
    xden = - 1.0;
    xnum = 0.0;
    for ( i = 0; i < 8; i++ )
    {
      xnum = xnum * xm4 + p4[i];
      xden = xden * xm4 + q4[i];
    }

    res = d4 + xm4 * ( xnum / xden );
  }
  else
  {
    res = 0.0;

    if ( x <= frtbig )
    {

      res = c[6];
      xsq = x * x;

      for ( i = 0; i < 6; i++ )
      {
        res = res / xsq + c[i];
      }

    }

    res = res / x;
    corr = log ( x );
    res = res + sqrtpi - 0.5 * corr;
    res = res + x * ( corr - 1.0 );

  }

  return res;
}
//******************************************************************************

double gamma_log_int ( int n )

//******************************************************************************
//
//  Purpose:
//
//    GAMMA_LOG_INT computes the logarithm of Gamma of an integer N.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the argument of the logarithm of the Gamma function.  
//    0 < N.
//
//    Output, double GAMMA_LOG_INT, the logarithm of 
//    the Gamma function of N.
//
{
  double value;

  if ( n <= 0 )
  {
    cout << " \n";
    cout << "GAMMA_LOG_INT - Fatal error!\n";
    cout << "  Illegal input value of N = " << n << "\n";
    cout << "  But N must be strictly positive.\n";
    exit ( 1 );
  }

  value = gamma_log ( ( double ) ( n ) );

  return value;
}
//******************************************************************************

double genlogistic_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_CDF evaluates the Generalized Logistic CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  double y;

  y = ( x - a ) / b;

  cdf = 1.0 / pow ( ( 1.0 + exp ( - y ) ), c );

  return cdf;
}
//******************************************************************************

double genlogistic_cdf_inv ( double cdf, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_CDF_INV inverts the Generalized Logistic CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double X, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "GENLOGISTIC_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = - d_huge ( );
  }
  else if ( cdf < 1.0 )
  {
    x = a - b * log ( pow ( cdf, - 1.0 / c ) - 1.0 );
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
  }

  return x;
}
//******************************************************************************

bool genlogistic_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_CHECK checks the parameters of the Generalized Logistic CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, bool GENLOGISTIC_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "GENLOGISTIC_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "GENLOGISTIC_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double genlogistic_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_MEAN returns the mean of the Generalized Logistic PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * ( euler_constant ( ) + digamma ( c ) );

  return mean;
}
//******************************************************************************

double genlogistic_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_PDF evaluates the Generalized Logistic PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = ( C / B ) * EXP ( ( A - X ) / B ) /
//      ( ( 1 + EXP ( ( A - X ) / B ) )**(C+1) )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  y = ( x - a ) / b;

  pdf = ( c / b ) * exp ( - y ) / pow ( 1.0 + exp ( - y ), c + 1.0 );

  return pdf;
}
//*******************************************************************************

double genlogistic_sample ( double a, double b, double c, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_SAMPLE samples the Generalized Logistic PDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double GENLOGISTIC_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = genlogistic_cdf_inv ( cdf, a, b, c );

  return x;
}
//******************************************************************************

double genlogistic_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    GENLOGISTIC_VARIANCE returns the variance of the Generalized Logistic PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = b * b * ( PI * PI / 6.0 + trigamma ( c ) );

  return variance;
# undef PI
}
//******************************************************************************

double geometric_cdf ( int x, double a )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_CDF evaluates the Geometric CDF.
//
//  Definition:
//
//    CDF(X,P) is the probability that there will be at least one
//    successful trial in the first X Bernoulli trials, given that
//    the probability of success in a single trial is P.
//
//  Modified:
//
//    28 January 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the maximum number of trials.
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double GEOMETRIC_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0 )
  {
    cdf = 0.0;
  }
  else if ( a == 0.0 )
  {
    cdf = 0.0;
  }
  else if ( a == 1.0 )
  {
    cdf = 1.0;
  }
  else
  {
    cdf = 1.0 - pow ( ( 1.0 - a ), x );
  }

  return cdf;
}
//******************************************************************************

int geometric_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_CDF_INV inverts the Geometric CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, int GEOMETRIC_CDF_INV, the corresponding value of X.
//
{
  int x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "GEOMETRIC_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( a == 1.0 )
  {
    x = 1;
  }
  else if ( a == 0.0 )
  {
    x = d_huge ( );
  }
  else
  {
    x = 1 + int ( log ( 1.0 - cdf ) / log ( 1.0 - a ) );
  }
 
  return x;
}
//******************************************************************************

void geometric_cdf_values ( int *n_data, int *x, double *p, double *cdf )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_CDF_VALUES returns values of the geometric CDF.
//
//  Discussion:
//
//    The geometric or Pascal probability density function gives the 
//    probability that the first success will happen on the X-th Bernoulli 
//    trial, given that the probability of a success on a single trial is P.
//
//    The value of CDF ( X, P ) is the probability that the first success
//    will happen on or before the X-th trial.
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = GeometricDistribution [ p ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    21 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//    Daniel Zwillinger and Stephen Kokoska,
//    CRC Standard Probability and Statistics Tables and Formulae,
//    Chapman and Hall / CRC Press, 2000.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *X, the number of trials.
//
//    Output, double *P, the probability of success 
//    on one trial.
//
//    Output, double *CDF, the cumulative density function.
//
{
# define N_MAX 14

  double cdf_vec[N_MAX] = { 
     0.1900000000000000E+00,  
     0.2710000000000000E+00,  
     0.3439000000000000E+00,  
     0.6861894039100000E+00,  
     0.3600000000000000E+00,  
     0.4880000000000000E+00,  
     0.5904000000000000E+00,  
     0.9141006540800000E+00,  
     0.7599000000000000E+00,  
     0.8704000000000000E+00,  
     0.9375000000000000E+00,  
     0.9843750000000000E+00,  
     0.9995117187500000E+00,  
     0.9999000000000000E+00 };

  double p_vec[N_MAX] = { 
     0.1E+00,  
     0.1E+00,  
     0.1E+00,  
     0.1E+00,  
     0.2E+00,  
     0.2E+00,  
     0.2E+00,  
     0.2E+00,  
     0.3E+00,  
     0.4E+00,  
     0.5E+00,  
     0.5E+00,  
     0.5E+00,  
     0.9E+00 };

  int x_vec[N_MAX] = { 
    1,  2,  3, 10, 1, 
    2,  3, 10,  3, 3,  
    3,  5, 10,  3 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0;
    *p = 0.0;
    *cdf = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *p = p_vec[*n_data-1];
    *cdf = cdf_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool geometric_check ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_CHECK checks the parameter of the Geometric CDF.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, bool GEOMETRIC_CHECK, is true if the parameters are legal.
//
{
  if ( a < 0.0 || 1.0 < a )
  {
    cout << " \n";
    cout << "GEOMETRIC_CHECK - Fatal error!\n";
    cout << "  A < 0 or 1 < A.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double geometric_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_MEAN returns the mean of the Geometric PDF.
//
//  Discussion:
//
//    MEAN is the expected value of the number of trials required
//    to obtain a single success.
// 
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = 1.0 / a;

  return mean;
}
//*******************************************************************************

double geometric_pdf ( int x, double a )

//*******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_PDF evaluates the Geometric PDF.
//
//  Formula:
//
//    PDF(A;X) = A * ( 1 - A )**(X-1)
//
//  Definition:
//
//    PDF(A;X) is the probability that exactly X Bernoulli trials, each
//    with probability of success A, will be required to achieve 
//    a single success.
//
//  Modified:
//
//    15 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the number of trials.
//    0 < X
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;
//
//  Special cases.
//
  if ( x < 1 )
  {
    pdf = 0.0;
  }
  else if ( a == 0.0 )
  {
    pdf = 0.0;
  }
  else if ( a == 1.0 )
  {
    if ( x == 1 )
    {
      pdf = 1.0;
    }
    else
    {
      pdf = 0.0;
    }
  }
  else
  {
    pdf = a * pow ( ( 1.0 - a ), ( x - 1 ) );

  }

  return pdf;
}
//******************************************************************************

double geometric_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_SAMPLE samples the Geometric PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int GEOMETRIC_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = geometric_cdf_inv ( cdf, a );

  return x;
}
//******************************************************************************

double geometric_variance ( double a )

//******************************************************************************
//
//  Purpose:
//
//    GEOMETRIC_VARIANCE returns the variance of the Geometric PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the probability of success on one trial.
//    0.0 <= A <= 1.0.
//
//    Output, double GEOMETRIC_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( 1.0 - a ) / ( a * a );

  return variance;
}
//******************************************************************************

int get_seed ( void )

//******************************************************************************
//
//  Purpose:
//
//    GET_SEED returns a random seed for the random number generator.
//
//  Modified:
//
//    15 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, int GET_SEED, a random seed value.
//
{
# define I_MAX 2147483647
  time_t clock;
  int i;
  int ihour;
  int imin;
  int isec;
  int seed;
  struct tm *lt;
  time_t tloc;
//
//  If the internal seed is 0, generate a value based on the time.
//
  clock = time ( &tloc );
  lt = localtime ( &clock );
//
//  Hours is 1, 2, ..., 12.
//
  ihour = lt->tm_hour;

  if ( 12 < ihour )
  {
    ihour = ihour - 12;
  }
//
//  Move Hours to 0, 1, ..., 11
//
  ihour = ihour - 1;

  imin = lt->tm_min;

  isec = lt->tm_sec;

  seed = isec + 60 * ( imin + 60 * ihour );
//
//  We want values in [1,43200], not [0,43199].
//
  seed = seed + 1;
//
//  Remap SEED from [1,43200] to [1,IMAX].
//
  seed = ( int ) 
    ( ( ( double ) seed )
    * ( ( double ) I_MAX ) / ( 60.0 * 60.0 * 12.0 ) );
//
//  Never use a seed of 0.
//
  if ( seed == 0 )
  {
    seed = 1;
  }

  return seed;
#undef I_MAX
}
//******************************************************************************

double gompertz_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    GOMPERTZ_CDF evaluates the Gompertz CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Johnson, Kotz, and Balakrishnan,
//    Continuous Univariate Distributions, Volume 2, second edition,
//    Wiley, 1994, pages 25-26.
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    1 < A, 0 < B.
//
//    Output, double GOMPERTZ_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - exp ( - b * ( pow ( a, x ) - 1.0 ) / log ( a ) );
  }

  return cdf;
}
//******************************************************************************

double gompertz_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    GOMPERTZ_CDF_INV inverts the Gompertz CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Johnson, Kotz, and Balakrishnan,
//    Continuous Univariate Distributions, Volume 2, second edition,
//    Wiley, 1994, pages 25-26.
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    1 < A, 0 < B.
//
//    Output, double GOMPERTZ_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "GOMPERTZ_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf < 1.0 )
  {
    x = log ( 1.0 - log ( 1.0 - cdf ) * log ( a ) / b  ) / log ( a );
  }
  else
  {
    x = d_huge ( );
  }

  return x;
}
//******************************************************************************

bool gompertz_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    GOMPERTZ_CHECK checks the parameters of the Gompertz PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Johnson, Kotz, and Balakrishnan,
//    Continuous Univariate Distributions, Volume 2, second edition,
//    Wiley, 1994, pages 25-26.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1 < A, 0 < B.
//
//    Output, bool GOMPERTZ_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 1.0 )
  {
    cout << " \n";
    cout << "GOMPERTZ_CHECK - Fatal error!\n";
    cout << "  A <= 1.0!\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "GOMPERTZ_CHECK - Fatal error!\n";
    cout << "  B <= 0.0!\n";
    return false;
  }

  return true;
}
//******************************************************************************

double gompertz_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    GOMPERTZ_PDF evaluates the Gompertz PDF.
//
//  Formula:
//
//    PDF(A,B;X) = B * A**X / exp ( B * ( A**X - 1 ) / log ( A ) )     
//
//    for
//
//      0.0 <= X
//      1.0 <  A 
//      0.0 <  B 
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Johnson, Kotz, and Balakrishnan,
//    Continuous Univariate Distributions, Volume 2, second edition,
//    Wiley, 1994, pages 25-26.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    1 < A, 0 < B.
//
//    Output, double GOMPERTZ_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else if ( 1.0 < a )
  {
    pdf = exp ( log ( b ) + x * log ( a ) 
      - ( b / log ( a ) ) * ( pow ( a, x ) - 1.0 ) );
  }

  return pdf;
}
//******************************************************************************

double gompertz_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    GOMPERTZ_SAMPLE samples the Gompertz PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1 < A, 0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double GOMPERTZ_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = gompertz_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double gumbel_cdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_CDF evaluates the Gumbel CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Output, double GUMBEL_CDF, the value of the CDF.
//
{
  double cdf;

  cdf = exp ( - exp ( - x ) );

  return cdf;
}
//******************************************************************************

double gumbel_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_CDF_INV inverts the Gumbel CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Output, double GUMBEL_CDF_INV, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "GUMBEL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x =  - log ( - log ( cdf ) );

  return x;
}
//******************************************************************************

double gumbel_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_MEAN returns the mean of the Gumbel PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double GUMBEL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = euler_constant ( );

  return mean;
}
//******************************************************************************

double gumbel_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_PDF evaluates the Gumbel PDF.
//
//  Formula:
//
//    PDF(X) = EXP ( - X - EXP ( - X  ) ).
//
//  Discussion:
//
//    GUMBEL_PDF(X) = EXTREME_PDF(0,1;X)
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Eric Weisstein, editor,
//    CRC Concise Encylopedia of Mathematics,
//    CRC Press, 1998.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Output, double GUMBEL_PDF, the value of the PDF.
//
{
  double pdf;

  pdf = exp ( - x - exp ( - x ) );

  return pdf;
}
//******************************************************************************

double gumbel_sample ( int *seed )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_SAMPLE samples the Gumbel PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double GUMBEL_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = gumbel_cdf_inv ( cdf );

  return x;
}
//******************************************************************************

double gumbel_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    GUMBEL_VARIANCE returns the variance of the Gumbel PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double GUMBEL_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = PI * PI / 6.0;

  return variance;
# undef PI
}
//******************************************************************************

double half_normal_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_CDF evaluates the Half Normal CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double HALF_NORMAL_CDF, the value of the CDF.
//
{
  double cdf;
  double cdf2;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else
  {
    cdf2 = normal_cdf ( x, a, b ); 
    cdf = 2.0 * cdf2 - 1.0;
  }

  return cdf;
}
//******************************************************************************

double half_normal_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_CDF_INV inverts the Half Normal CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double HALF_NORMAL_CDF_INV, the corresponding argument.
//
{
  double cdf2;
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "HALF_NORMAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cdf2 = 0.5 * ( cdf + 1.0 );

  x = normal_cdf_inv ( cdf2, a, b );

  return x;
}
//******************************************************************************

bool half_normal_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_CHECK checks the parameters of the Half Normal PDF.
// 
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool HALF_NORMAL_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "HALF_NORMAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double half_normal_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_MEAN returns the mean of the Half Normal PDF.
// 
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double HALF_NORMAL_MEAN, the mean of the PDF.
//
{
# define PI 3.141592653589793

  double mean;

  mean = a + b * sqrt ( 2.0 / PI );

  return mean;
# undef PI
}
//******************************************************************************

double half_normal_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_PDF evaluates the Half Normal PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 
//      sqrt ( 2 / PI ) * ( 1 / B ) * exp ( - 0.5 * ( ( X - A ) / B )**2 )
//
//    for A <= X
//
//  Discussion:
//
//    The Half Normal PDF is a special case of both the Chi PDF and the
//    Folded Normal PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double HALF_NORMAL_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = sqrt ( 2.0 / PI ) * ( 1.0 / b ) * exp ( - 0.5 * y * y );

  }

  return pdf;
# undef PI
}
//******************************************************************************

double half_normal_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_SAMPLE samples the Half Normal PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double HALF_NORMAL_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );
  
  x = half_normal_cdf_inv ( cdf, a, b );
  
  return x;
}
//******************************************************************************

double half_normal_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    HALF_NORMAL_VARIANCE returns the variance of the Half Normal PDF.
// 
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double HALF_NORMAL_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = b * b * ( 1.0 - 2.0 / PI );

  return variance;
# undef PI
}
//******************************************************************************

double hypergeometric_cdf ( int x, int n, int m, int l )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_CDF evaluates the Hypergeometric CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the CDF.
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Output, double HYPERGEOMETRIC_CDF, the value of the CDF.
//
{
  double cdf;
  double c1_log;
  double c2_log;
  double pdf;
  int x2;

  c1_log = binomial_coef_log ( l - m, n );
  c2_log = binomial_coef_log ( l, n );

  pdf = exp ( c1_log - c2_log );
  cdf = pdf;

  for ( x2 = 0; x2 <= x - 1; x2++ )
  {
    pdf = pdf * ( double ) ( ( m - x2 ) * ( n - x2 ) ) 
      / ( double ) ( ( x2 + 1 ) * ( l - m - n + x2 + 1 ) );

    cdf = cdf + pdf;
  }

  return cdf;
}
//******************************************************************************

void hypergeometric_cdf_values ( int *n_data, int *sam, int *suc, int *pop, 
  int *n, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_CDF_VALUES returns some values of the hypergeometric CDF.
//
//  Discussion:
//
//    CDF(X)(A,B) is the probability of at most X successes in A trials,
//    given that the probability of success on a single trial is B.
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = HypergeometricDistribution [ sam, suc, pop ]
//      CDF [ dist, n ]
//
//  Modified:
//
//    05 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//    Daniel Zwillinger,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition, CRC Press, 1996, pages 651-652.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *SAM, int *SUC, int *POP, the sample size, 
//    success size, and population parameters of the function.
//
//    Output, int *N, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 16

  double fx_vec[N_MAX] = { 
     0.6001858177500578E-01,  
     0.2615284665839845E+00,  
     0.6695237889132748E+00,  
     0.1000000000000000E+01,  
     0.1000000000000000E+01,  
     0.5332595856827856E+00,  
     0.1819495964117640E+00,  
     0.4448047017527730E-01,  
     0.9999991751316731E+00,  
     0.9926860896560750E+00,  
     0.8410799901444538E+00,  
     0.3459800113391901E+00,  
     0.0000000000000000E+00,  
     0.2088888139634505E-02,  
     0.3876752992448843E+00,  
     0.9135215248834896E+00 };

  int n_vec[N_MAX] = { 
     7,  8,  9, 10, 
     6,  6,  6,  6, 
     6,  6,  6,  6, 
     0,  0,  0,  0 };

  int pop_vec[N_MAX] = { 
    100, 100, 100, 100, 
    100, 100, 100, 100, 
    100, 100, 100, 100, 
    90,  200, 1000, 10000 };

  int sam_vec[N_MAX] = { 
    10, 10, 10, 10, 
     6,  7,  8,  9, 
    10, 10, 10, 10, 
    10, 10, 10, 10 };

  int suc_vec[N_MAX] = { 
    90, 90, 90, 90, 
    90, 90, 90, 90, 
    10, 30, 50, 70, 
    90, 90, 90, 90 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }
 
  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *sam = 0;
    *suc = 0;
    *pop = 0;
    *n = 0;
    *fx = 0.0;
  }
  else
  {
    *sam = sam_vec[*n_data-1];
    *suc = suc_vec[*n_data-1];
    *pop = pop_vec[*n_data-1];
    *n = n_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool hypergeometric_check ( int n, int m, int l )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_CHECK checks the parameters of the Hypergeometric CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Output, bool HYPERGEOMETRIC_CHECK, is true if the parameters are legal.
//
{
  if ( n < 0 || l < n )
  {
    cout << " \n";
    cout << "HYPERGEOMETRIC_CHECK - Fatal error!\n";
    cout << "  Input N is out of range.\n";
    return false;
  }

  if ( m < 0 || l < m )
  {
    cout << " \n";
    cout << "HYPERGEOMETRIC_CHECK - Fatal error!\n";
    cout << "  Input M is out of range.\n";
    return false;
  }

  if ( l < 0 )
  {
    cout << " \n";
    cout << "HYPERGEOMETRIC_CHECK - Fatal error!\n";
    cout << "  Input L is out of range.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double hypergeometric_mean ( int n, int m, int l )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_MEAN returns the mean of the Hypergeometric PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Output, double HYPERGEOMETRIC_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( double ) ( n * m ) / ( double ) ( l );

  return mean;
}
//******************************************************************************

double hypergeometric_pdf ( int x, int n, int m, int l )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_PDF evaluates the Hypergeometric PDF.
//
//  Formula:
//
//    PDF(N,M,L;X) = C(M,X) * C(L-M,N-X) / C(L,N).
//
//  Definition:
//
//    PDF(N,M,L;X) is the probability of drawing X white balls in a 
//    single random sample of size N from a population containing 
//    M white balls and a total of L balls.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the desired number of white balls.
//    0 <= X <= N, usually, although any value of X can be given.
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Output, double HYPERGEOMETRIC_PDF, the probability of exactly K white balls.
//
{
  double c1;
  double c2;
  double c3;
  double pdf;
  double pdf_log;
//
//  Special cases.
//
  if ( x < 0 )
  {
    pdf = 1.0;
  }
  else if ( n < x )
  {
    pdf = 0.0;
  }
  else if ( m < x )
  {
    pdf = 0.0;
  }
  else if ( l < x )
  {
    pdf = 0.0;
  }
  else if ( n == 0 )
  {
    if ( x == 0 )
    {
      pdf = 1.0;
    }
    else
    {
      pdf = 0.0;
    }
  }
  else
  {
    c1 = binomial_coef_log ( m, x );
    c2 = binomial_coef_log ( l-m, n-x );
    c3 = binomial_coef_log ( l, n );

    pdf_log = c1 + c2 - c3;

    pdf = exp ( pdf_log );

  }

  return pdf;
}
//******************************************************************************

int hypergeometric_sample ( int n, int m, int l, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_SAMPLE samples the Hypergeometric PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 165.
//
//  Parameters:
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int HYPERGEOMETRIC_SAMPLE, a sample of the PDF.
//
{
  double a;
  double b;
  double c1_log;
  double c2_log;
  double u;
  int x;

  c1_log = binomial_coef_log ( l - m, n );
  c2_log = binomial_coef_log ( l, n );

  a = exp ( c1_log - c2_log );
  b = a;

  u = d_uniform_01 ( seed );

  x = 0;

  while ( a < u )
  {
    b = b * ( double ) ( ( m - x ) * ( n - x ) ) 
      / ( double ) ( ( x + 1 ) * ( l - m - n + x + 1 ) );

    a = a + b;

    x = x + 1;

  }

  return x;
}
//******************************************************************************

double hypergeometric_variance ( int n, int m, int l )

//******************************************************************************
//
//  Purpose:
//
//    HYPERGEOMETRIC_VARIANCE returns the variance of the Hypergeometric PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of balls selected.
//    0 <= N <= L.
//
//    Input, int M, the number of white balls in the population.
//    0 <= M <= L.
//
//    Input, int L, the number of balls to select from.
//    0 <= L.
//
//    Output, double HYPERGEOMETRIC_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( double ) ( n * m * ( l - m ) * ( l - n ) ) 
    / ( double ) ( l * l * ( l - 1 ) );

  return variance;
}
//**********************************************************************

int i_factorial ( int n )

//**********************************************************************
//
//  Purpose:
//
//    I_FACTORIAL returns N!.
//
//  Definition:
//
//    N! = Product ( 1 <= I <= N ) I
//
//  Modified:
//
//    02 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the argument of the factorial function.
//    0 <= N.
//
//    Output, int I_FACTORIAL, the factorial of N.
//
{
  int fact;
  int i;
//
//  Check.
//
  if ( n < 0 )
  {
    cout << "\n";
    cout << "I_FACTORIAL - Fatal error!\n";
    cout << "  N < 0.\n";
    return 0;
  }

  fact = 1;

  for ( i = 2; i <= n; i++ )
  {
    fact = fact * i;
  }

  return fact;
}
//******************************************************************************

int i_huge ( void )

//******************************************************************************
//
//  Purpose:
//
//    I_HUGE returns a "huge" integer value, usually the largest legal signed int.
//
//  Modified:
//
//    16 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, int I_HUGE, a "huge" integer.
//
{
  return 2147483647;
}
//****************************************************************************

int i_max ( int i1, int i2 )

//****************************************************************************
//
//  Purpose:
//
//    I_MAX returns the maximum of two integers.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, are two integers to be compared.
//
//    Output, int I_MAX, the larger of I1 and I2.
//
//
{
  if ( i2 < i1 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//****************************************************************************

int i_min ( int i1, int i2 )

//****************************************************************************
//
//  Purpose:
//
//    I_MIN returns the smaller of two integers.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, two integers to be compared.
//
//    Output, int I_MIN, the smaller of I1 and I2.
//
{
  if ( i1 < i2 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//********************************************************************

int i_random ( int ilo, int ihi, int *seed )

//********************************************************************
//
//  Purpose:
//
//    I_RANDOM returns a random integer in a given range.
//
//  Modified:
//
//    03 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int ILO, IHI, the minimum and maximum values acceptable
//    for I.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int I_RANDOM, the randomly chosen integer.
//
{
  int i;
//
  i = ilo + ( int ) ( d_uniform_01 ( seed ) * ( double ) ( ihi + 1 - ilo ) );
//
//  In case of oddball events at the boundary, enforce the limits.
//
  if ( i < ilo )
  {
    i = ilo;
  }

  if ( ihi < i )
  {
    i = ihi;
  }

  return i;
}
//******************************************************************************

double inverse_gaussian_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_CDF evaluates the Inverse Gaussian CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//    0.0 < X.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double INVERSE_GAUSSIAN_CDF, the value of the CDF.
//
{
  double cdf;
  double cdf1;
  double cdf2;
  double x1;
  double x2;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    x1 = sqrt ( b / x ) * ( x - a ) / a;
    cdf1 = normal_01_cdf ( x1 );

    x2 = - sqrt ( b / x ) * ( x + a ) / a;
    cdf2 = normal_01_cdf ( x2 );

    cdf = cdf1 + exp ( 2.0 * b / a ) * cdf2;
  }

  return cdf;
}
//******************************************************************************

bool inverse_gaussian_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_CHECK checks the parameters of the Inverse Gaussian CDF.
//
//  Modified:
//
//    22 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, bool INVERSE_GAUSSIAN_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "INVERSE_GAUSSIAN_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "INVERSE_GAUSSIAN_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double inverse_gaussian_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_MEAN returns the mean of the Inverse Gaussian PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double INVERSE_GAUSSIAN_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double inverse_gaussian_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_PDF evaluates the Inverse Gaussian PDF.
//
//  Discussion:
//
//    The Inverse Gaussian PDF is also known as the Wald PDF
//    and the Inverse Normal PDF.
//
//  Formula:
//
//    PDF(A,B;X) 
//      = sqrt ( B / ( 2 * PI * X**3 ) ) 
//        * exp ( - B * ( X - A )**2 / ( 2.0 * A**2 * X ) )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 < X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double INVERSE_GAUSSIAN_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  if ( x <= 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = sqrt ( b / ( 2.0 * PI * pow ( x, 3 ) ) ) * 
      exp ( - b * ( x - a ) * ( x - a ) / ( 2.0 * a * a * x ) );
  }

  return pdf;
# undef PI
}
//******************************************************************************

double inverse_gaussian_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_SAMPLE samples the Inverse Gaussian PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double INVERSE_GAUSSIAN_SAMPLE, a sample of the PDF.
//
{
  double phi;
  double t;
  double u;
  double x;
  double y;
  double z;

  phi = b / a;
  z = normal_01_sample ( seed );
  y = z * z;

  t = 1.0 + 0.5 * ( y - sqrt ( 4.0 * phi * y + y * y ) ) / phi;
  u = d_uniform_01 ( seed );

  if ( u * ( 1.0 + t ) <= 1.0 )
  {
    x = a * t;
  }
  else
  {
    x = a / t;
  }

  return x;
}
//******************************************************************************

double inverse_gaussian_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    INVERSE_GAUSSIAN_VARIANCE returns the variance of the Inverse Gaussian PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double INVERSE_GAUSSIAN_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = a * a * a / b;

  return variance;
}
//******************************************************************************

int *irow_max ( int m, int n, int a[] )

//******************************************************************************
//
//  Purpose:
//
//    IROW_MAX returns the maximums of an integer array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MAX =
//      3
//      7
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, int A[M*N], the array to be examined.
//
//    Output, int IROW_AMAX[M], the maximums of the rows.
//
{
  int i;
  int j;
  int *amax;

  amax = new int[m];

  for ( i = 0; i < m; i++ )
  {
    amax[i] = a[i+0*m];

    for ( j = 1; j < n; j++ )
    {
      if ( amax[i] < a[i+j*m] )
      {
        amax[i] = a[i+j*m];
      }
    }
  }

  return amax;
}
//******************************************************************************

double *irow_mean ( int m, int n, int a[] )

//******************************************************************************
//
//  Purpose:
//
//    IROW_MEAN returns the means of an integer array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MEAN =
//      2
//      5
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, int A[M*N], the array to be examined.
//
//    Output, double IROW_MEAN[M], the means, or averages, of the rows.
//
{
  int i;
  int j;
  double *mean;

  mean = new double[m];

  for ( i = 0; i < m; i++ )
  {
    mean[i] = 0.0;
    for ( j = 0; j < n; j++ )
    {
      mean[i] = mean[i] + ( double ) a[i+j*m];
    }
    mean[i] = mean[i] / ( double ) ( n );
  }

  return mean;
}
//******************************************************************************

int *irow_min ( int m, int n, int a[] )

//******************************************************************************
//
//  Purpose:
//
//    IROW_MIN returns the minimums of an integer array of rows.
//
//  Example:
//
//    A =
//      1  2  3
//      2  6  7
//
//    MIN =
//      1
//      2
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, int A[M*N], the array to be examined.
//
//    Output, int IROW_AMIN[M], the minimums of the rows.
//
{
  int i;
  int j;
  int *amin;

  amin = new int[m];

  for ( i = 0; i < m; i++ )
  {
    amin[i] = a[i+0*m];
    for ( j = 1; j < n; j++ )
    {
      if ( a[i+j*m] < amin[i] )
      {
        amin[i] = a[i+j*m];
      }
    }
  }

  return amin;
}
//******************************************************************************

double *irow_variance ( int m, int n, int a[] )

//******************************************************************************
//
//  Purpose:
//
//    IROW_VARIANCE returns the variances of an integer array of rows.
//
//  Modified:
//
//    10 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, N, the number of rows and columns in the array.
//
//    Input, int A[M*N], the array whose variances are desired.
//
//    Output, double IROW_VARIANCE[M], the variances of the rows.
//
{
  int i;
  int j;
  double mean;
  double *variance;

  variance = new double[m];

  for ( i = 0; i < m; i++ )
  {
    mean = 0.0;
    for ( j = 0; j < n; j++ )
    {
      mean = mean + ( double ) a[i+j*m];
    }
    mean = mean / ( double ) ( n );

    variance[i] = 0.0;
    for ( j = 0; j < n; j++ )
    {
      variance[i] = variance[i] + pow ( ( ( double ) a[i+j*m] - mean ), 2 );
    }

    if ( 1 < n )
    {
      variance[i] = variance[i] / ( double ) ( n - 1 );
    }
    else
    {
      variance[i] = 0.0;
    }
  }

  return variance;
}
//**********************************************************************

int ivec_max ( int n, int x[] )

//**********************************************************************
//
//  Purpose:
//
//    IVEC_MAX returns the maximum of an integer vector.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, int X[N], the vector whose maximum is desired.
//
//    Output, int IVEC_MAX, the maximum of the vector entries.
//
{
  int i;
  int value;
  
  value = x[0];
  for ( i = 1; i < n; i++ )
  {
    if ( value < x[i] )
    {
      value = x[i];
    }
  }

  return value;
}
//**********************************************************************

double ivec_mean ( int n, int x[] )

//**********************************************************************
//
//  Purpose:
//
//    IVEC_MEAN returns the mean of an integer vector.
//
//  Modified:
//
//    01 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, int X[N], the vector whose mean is desired.
//
//    Output, double IVEC_MEAN, the mean, or average, of the vector entries.
//
{
  int i;
  double mean;

  mean = 0.0;
  for ( i = 0; i < n; i++ )
  {
    mean = mean + ( double ) x[i];
  }

  mean = mean / ( double ) n;

  return mean;
}
//**********************************************************************

int ivec_min ( int n, int x[] )

//**********************************************************************
//
//  Purpose:
//
//    IVEC_MIN returns the minimum of an integer vector.
//
//  Modified:
//
//    22 May 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, int X[N], the vector whose minimum is desired.
//
//    Output, int IVEC_MIN, the minimum of the vector entries.
//
{
  int i;
  int value;
  
  value = x[0];
  for ( i = 1; i < n; i++ )
  {
    if ( x[i] < value )
    {
      value = x[i];
    }
  }

  return value;
}
//********************************************************************

void ivec_print ( int n, int a[], char *title )

//********************************************************************
//
//  Purpose:
//
//    IVEC_PRINT prints an integer vector.
//
//  Modified:
//
//    25 February 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components of the vector.
//
//    Input, int A[N], the vector to be printed.
//
//    Input, char *TITLE, a title to be printed first.
//    TITLE may be blank.
//
{
  int i;

  if ( s_len_trim ( title ) != 0 )
  {
    cout << "\n";
    cout << title << "\n";
  }

  cout << "\n";
  for ( i = 0; i <= n-1; i++ ) 
  {
    cout << setw(6) << i << "  " << setw(8) << a[i] << "\n";
  }

  return;
}
//*********************************************************************

int ivec_sum ( int n, int a[] )

//*********************************************************************
//
//  Purpose:
//
//    IVEC_SUM sums the entries of an integer vector.
//
//  Example:
//
//    Input:
//
//      A = ( 1, 2, 3, 4 )
//
//    Output:
//
//      IVEC_SUM = 10
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, int A[N], the vector to be summed.
//
//    Output, int IVEC_SUM, the sum of the entries of A.
//
{
  int i;
  int sum;

  sum = 0;
  for ( i = 0; i < n; i++ )
  {
    sum = sum + a[i];
  }

  return sum;
}
//**********************************************************************

double ivec_variance ( int n, int x[] )

//**********************************************************************
//
//  Purpose:
//
//    IVEC_VARIANCE returns the variance of an int vector.
//
//  Modified:
//
//    01 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input, int X[N], the vector whose variance is desired.
//
//    Output, double IVEC_VARIANCE, the variance of the vector entries.
//
{
  int i;
  double mean;
  double variance;

  mean = ivec_mean ( n, x );

  variance = 0.0;
  for ( i = 0; i < n; i++ )
  {
    variance = variance + ( ( double ) x[i] - mean ) * ( ( double ) x[i] - mean );
  }

  if ( 1 < n )
  {
    variance = variance / ( double ) ( n - 1 );
  }
  else
  {
    variance = 0.0;
  }

  return variance;
}
//******************************************************************************

void laplace_cdf_values ( int *n_data, double *mu, double *beta, double *x,
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_CDF_VALUES returns some values of the Laplace CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = LaplaceDistribution [ mu, beta ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    28 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *BETA, the shape parameter.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double beta_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,  
     0.1000000000000000E+01,  
     0.1000000000000000E+01,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.8160602794142788E+00,  
     0.9323323583816937E+00,  
     0.9751064658160680E+00,  
     0.6967346701436833E+00,  
     0.6417343447131054E+00,  
     0.6105996084642976E+00,  
     0.5906346234610091E+00,  
     0.5000000000000000E+00,  
     0.3032653298563167E+00,  
     0.1839397205857212E+00,  
     0.1115650800742149E+00 };

  double mu_vec[N_MAX] = { 
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.0000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01 }; 

  double x_vec[N_MAX] = { 
     0.0000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *beta = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *beta = beta_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double laplace_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_CDF evaluates the Laplace CDF.
//
//  Modified:
//
//    16 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LAPLACE_CDF, the value of the PDF.
//
{
  double cdf;
  double y;

  y = ( x - a ) / b;

  if ( x <= a )
  {
    cdf = 0.5 * exp ( y );
  }
  else 
  {
    cdf = 1.0 - 0.5 * exp ( - y );
  }

  return cdf;
}
//******************************************************************************

double laplace_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_CDF_INV inverts the Laplace CDF.
//
//  Modified:
//
//    17 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LAPLACE_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "LAPLACE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf <= 0.5 )
  {
    x = a + b * log ( 2.0 * cdf );
  }
  else
  {
    x = a - b * log ( 2.0 * ( 1.0 - cdf ) );
  }

  return x;
}

//******************************************************************************

bool laplace_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_CHECK checks the parameters of the Laplace PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool LAPLACE_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "LAPLACE_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double laplace_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_MEAN returns the mean of the Laplace PDF.
//
//  Modified:
//
//    15 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LAPLACE_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double laplace_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_PDF evaluates the Laplace PDF.
//
//  Formula:
//
//    PDF(A,B;X) = exp ( - abs ( X - A ) / B ) / ( 2 * B )
//
//  Discussion:
//
//    The Laplace PDF is also known as the Double Exponential PDF.
//
//  Modified:
//
//    09 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LAPLACE_PDF, the value of the PDF.
//
{
  double pdf;

  pdf = exp ( - fabs ( x - a ) / b ) / ( 2.0 * b );

  return pdf;
}
//******************************************************************************

double laplace_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_SAMPLE samples the Laplace PDF.
//
//  Modified:
//
//    16 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double LAPLACE_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = laplace_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double laplace_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LAPLACE_VARIANCE returns the variance of the Laplace PDF.
//
//  Modified:
//
//    15 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LAPLACE_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 2.0 * b * b;

  return variance;
}
//******************************************************************************

double lerch ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    LERCH estimates the Lerch transcendent function.
//
//  Definition:
//
//    The Lerch transcendent function is defined as:
//
//      LERCH ( A, B, C ) = Sum ( 0 <= K < Infinity ) A**K / ( C + K )**B
//
//    excluding any term with ( C + K ) = 0.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Eric Weisstein, editor,
//    CRC Concise Encylopedia of Mathematics,
//    CRC Press, 1998.
//
//  Thanks:
//
//    Oscar van Vlijmen
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the function. 
//
//    Output, double LERCH, an approximation to the Lerch
//    transcendent function.
//
{
  double a_k;
  int k;
  double sum2;
  double sum2_old;

  sum2 = 0.0;
  k = 0;
  a_k = 1.0;

  for ( ; ; )
  {
    sum2_old = sum2;

    if ( c + ( double ) ( k ) == 0.0 )
    {
      k = k + 1;
      a_k = a_k * a;
      continue;
    }
    
    sum2 = sum2 + a_k / pow ( c + ( double ) ( k ), b );

    if ( sum2 <= sum2_old )
    {
      break;
    }

    k = k + 1;
    a_k = a_k * a;

  }

  return sum2;
}
//*******************************************************************************

double log_normal_cdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_CDF evaluates the Lognormal CDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 < X.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  double logx;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    logx = log ( x );
  
    cdf = normal_cdf ( logx, a, b );
  }

  return cdf;
}
//*******************************************************************************

double log_normal_cdf_inv ( double cdf, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_CDF_INV inverts the Lognormal CDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input, double X, the corresponding argument.
//
{
  double logx;
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "LOG_NORMAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  logx = normal_cdf_inv ( cdf, a, b );

  x = exp ( logx );

  return x;
}
//******************************************************************************

void log_normal_cdf_values ( int *n_data, double *mu, double *sigma, 
  double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_CDF_VALUES returns some values of the Log Normal CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = LogNormalDistribution [ mu, sigma ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    28 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *SIGMA, the shape parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double fx_vec[N_MAX] = { 
     0.2275013194817921E-01,  
     0.2697049307349095E+00,  
     0.5781741008028732E+00,  
     0.7801170895122241E+00,  
     0.4390310097476894E+00,  
     0.4592655190218048E+00,  
     0.4694258497695908E+00,  
     0.4755320473858733E+00,  
     0.3261051056816658E+00,  
     0.1708799040927608E+00,  
     0.7343256357952060E-01,  
     0.2554673736161761E-01 };

  double mu_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double sigma_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *sigma = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *sigma = sigma_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool log_normal_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_CHECK checks the parameters of the Lognormal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool LOG_NORMAL_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "LOG_NORMAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double log_normal_mean ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_MEAN returns the mean of the Lognormal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = exp ( a + 0.5 * b * b );

  return mean;
}
//*******************************************************************************

double log_normal_pdf ( double x, double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_PDF evaluates the Lognormal PDF.
//
//  Formula:
//
//    PDF(A,B;X) 
//      = exp ( - 0.5 * ( ( log ( X ) - A ) / B )**2 ) 
//        / ( B * X * sqrt ( 2 * PI ) )
//
//  Discussion:
//
//    The Lognormal PDF is also known as the Cobb-Douglas PDF,
//    and as the Antilog_normal PDF.
//
//    The Lognormal PDF describes a variable X whose logarithm
//    is normally distributed.
//
//    The special case A = 0, B = 1 is known as Gilbrat's PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 < X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  if ( x <= 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( log ( x ) - a ) / b;
    pdf = exp ( -0.5 * y * y ) / ( b * x * sqrt ( 2.0 * PI ) );
  }

  return pdf;
# undef PI
}
//*******************************************************************************

double log_normal_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_SAMPLE samples the Lognormal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = log_normal_cdf_inv ( cdf, a, b );

  return x;
}
//*******************************************************************************

double log_normal_variance ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    LOG_NORMAL_VARIANCE returns the variance of the Lognormal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = exp ( 2.0 * a + b * b ) * ( exp ( b * b ) - 1.0 );

  return variance;
}
//******************************************************************************

double log_series_cdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_CDF evaluates the Logarithmic Series CDF.
//
//  Discussion:
//
//    Simple summation is used, with a recursion to generate successive
//    values of the PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Thanks:
//
//    Oscar van Vlijmen
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//    0 < X 
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, double LOG_SERIES_CDF, the value of the CDF.
//
{
  double cdf;
  double pdf;
  int x2;

  cdf = 0.0;

  for ( x2 = 1; x2 <= x; x2++ )
  {
    if ( x2 == 1 )
    {
      pdf = - a / log ( 1.0 - a );
    }
    else
    {
      pdf = ( double ) ( x2 - 1 ) * a * pdf / ( double ) ( x2 );
    }

    cdf = cdf + pdf;
  }

  return cdf;
}
//******************************************************************************

int log_series_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_CDF_INV inverts the Logarithmic Series CDF.
//
//  Discussion:
//
//    Simple summation is used.  The only protection against an
//    infinite loop caused by roundoff is that X cannot be larger 
//    than 1000.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, double LOG_SERIES_CDF_INV, the argument X of the CDF for which
//    CDF(X-1) <= CDF <= CDF(X).
//
{
  double cdf2;
  double pdf;
  int x;
  int xmax = 1000;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "LOG_SERIES_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cdf2 = 0.0;
  x = 1;

  while ( cdf2 < cdf && x < xmax )
  {
    if ( x == 1 )
    {
      pdf = - a / log ( 1.0 - a );
    }
    else
    {
      pdf = ( double ) ( x - 1 ) * a * pdf / ( double ) ( x );
    }

    cdf2 = cdf2 + pdf;

    x = x + 1;
  }

  return x;
}
//******************************************************************************

void log_series_cdf_values ( int *n_data, double *t, int *n, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_CDF_VALUES returns some values of the log series CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = LogSeriesDistribution [ t ]
//      CDF [ dist, n ]
//
//  Modified:
//
//    27 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *T, the parameter of the function.
//
//    Output, int *N, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 29

  double fx_vec[N_MAX] = { 
     0.9491221581029903E+00,  
     0.9433541128559735E+00,  
     0.9361094611773272E+00,  
     0.9267370278044118E+00,  
     0.9141358246245129E+00,  
     0.8962840235449100E+00,  
     0.8690148741955517E+00,  
     0.8221011541254772E+00,  
     0.7213475204444817E+00,  
     0.6068261510845583E+00,  
     0.5410106403333613E+00,  
     0.4970679476476894E+00,  
     0.4650921887927060E+00,  
     0.4404842934597863E+00,  
     0.4207860535926143E+00,  
     0.4045507673897055E+00,  
     0.3908650337129266E+00,  
     0.2149757685421097E+00,  
     0.0000000000000000E+00,  
     0.2149757685421097E+00,  
     0.3213887739704539E+00,  
     0.3916213575531612E+00,  
     0.4437690508633213E+00,  
     0.4850700239649681E+00,  
     0.5191433267738267E+00,  
     0.5480569580144867E+00,  
     0.5731033910767085E+00,  
     0.5951442521714636E+00,  
     0.6147826594068904E+00 };

  int n_vec[N_MAX] = { 
     1, 1, 1, 1, 1, 
     1, 1, 1, 1, 1, 
     1, 1, 1, 1, 1, 
     1, 1, 1, 0, 1, 
     2, 3, 4, 5, 6, 
     7, 8, 9, 10 };

  double t_vec[N_MAX] = { 
     0.1000000000000000E+00,  
     0.1111111111111111E+00,  
     0.1250000000000000E+00,  
     0.1428571428571429E+00,  
     0.1666666666666667E+00,  
     0.2000000000000000E+00,  
     0.2500000000000000E+00,  
     0.3333333333333333E+00,  
     0.5000000000000000E+00,  
     0.6666666666666667E+00,  
     0.7500000000000000E+00,  
     0.8000000000000000E+00,  
     0.8333333333333333E+00,  
     0.8571485714857149E+00,  
     0.8750000000000000E+00,  
     0.8888888888888889E+00,  
     0.9000000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00,  
     0.9900000000000000E+00  };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }
 
  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *t = 0.0;
    *n = 0;
    *fx = 0.0;
  }
  else
  {
    *t = t_vec[*n_data-1];
    *n = n_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool log_series_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_CHECK checks the parameter of the Logarithmic Series PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, bool LOG_SERIES_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 || 1.0 <= a )
  {
    cout << " \n";
    cout << "LOG_SERIES_CHECK - Fatal error!\n";
    cout << "  A <= 0.0 or 1.0 <= A\n";
    return false;
  }

  return true;
}
//******************************************************************************

double log_series_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_MEAN returns the mean of the Logarithmic Series PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, double LOG_SERIES_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = - a / ( ( 1.0 - a ) * log ( 1.0 - a ) );

  return mean;
}
//******************************************************************************

double log_series_pdf ( int x, double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_PDF evaluates the Logarithmic Series PDF.
//
//  Formula:
//
//    PDF(A;X) = - A**X / ( X * log ( 1 - A ) )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//    0 < X
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, double LOG_SERIES_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= 0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = - pow ( a, x ) / ( ( double ) ( x ) * log ( 1.0 - a ) );
  }

  return pdf;
}
//******************************************************************************

int log_series_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_SAMPLE samples the Logarithmic Series PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Luc Devroye,
//    Non-Uniform Random Variate Generation,
//    Springer-Verlag, New York, 1986, page 547.
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int LOG_SERIES_SAMPLE, a sample of the PDF.
//
{
  double u;
  double v;
  int x;

  u = d_uniform_01 ( seed );
  v = d_uniform_01 ( seed );

  x = ( int ) ( 1.0 + log ( v ) / ( log ( 1.0 - pow ( 1.0 - a, u ) ) ) );

  return x;
}
//******************************************************************************

double log_series_variance ( double a )

//******************************************************************************
//
//  Purpose:
//
//    LOG_SERIES_VARIANCE returns the variance of the Logarithmic Series PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A < 1.0.
//
//    Output, double LOG_SERIES_VARIANCE, the variance of the PDF.
//
{
  double alpha;
  double variance;

  alpha = - 1.0 / log ( 1.0 - a );

  variance = a * alpha * ( 1.0 - alpha * a ) / pow ( ( 1.0 - a ), 2 );

  return variance;
}
//******************************************************************************

double log_uniform_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_CDF evaluates the Log Uniform CDF.
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else if ( x < b )
  {
    cdf = ( log ( x ) - log ( a ) ) / ( log ( b ) - log ( a ) );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double log_uniform_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_CDF_INV inverts the Log Uniform CDF.
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double X, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "LOG_UNIFORM_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a * exp ( ( log ( b ) - log ( a ) ) * cdf );

  return x;
}
//******************************************************************************

bool log_uniform_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_CHECK checks the parameters of the Log Uniform CDF.
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1.0 < A < B.
//
//    Output, bool LOG_UNIFORM_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 1.0 )
  {
    cout << "\n";
    cout << "LOG_UNIFORM_CHECK - Fatal error!\n";
    cout << "  A <= 1.\n";
    return false;
  }

  if ( b <= a )
  {
    cout << "\n";
    cout << "LOG_UNIFORM_CHECK - Fatal error!\n";
    cout << "  B <= A.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double log_uniform_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_MEAN returns the mean of the Log Uniform PDF.
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1.0 < A < B.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( b - a ) / ( log ( b ) - log ( a ) );

  return mean;
}
//******************************************************************************

double log_uniform_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_PDF evaluates the Log Uniform PDF.
//
//  Discussion:
//
//    PDF(A,B;X) = ( log ( B ) - log ( A ) ) / X  for A <= X <= B
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    1.0 < A < B.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < a )
  {
    pdf = 0.0;
  }
  else if ( x <= b )
  {
    pdf = 1.0 / ( x * ( log ( b ) - log ( a ) ) );
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
}
//******************************************************************************

double log_uniform_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_SAMPLE samples the Log Uniform PDF.
//
//  Modified:
//
//    20 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1.0 < A < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = log_uniform_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double log_uniform_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOG_UNIFORM_VARIANCE returns the variance of the Log Uniform PDF.
//
//  Modified:
//
//    01 November 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    1.0 < A < B.
//
//    Output, double LOG_UNIFORM_VARIANCE, the variance of the PDF.
//
{
  double mean;
  double variance;

  mean = log_uniform_mean ( a, b );

  variance = 
    ( ( 0.5 * b * b - 2.0 * mean * b + mean * mean * log ( b ) ) 
    - ( 0.5 * a * a - 2.0 * mean * a + mean * mean * log ( a ) ) ) 
    / ( log ( b ) - log ( a ) );

  return variance;
}
//******************************************************************************

double logistic_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_CDF evaluates the Logistic CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LOGISTIC_CDF, the value of the CDF.
//
{
  double cdf;

  cdf = 1.0 / ( 1.0 + exp ( ( a - x ) / b ) );

  return cdf;
}
//******************************************************************************

double logistic_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_CDF_INV inverts the Logistic CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LOGISTIC_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "LOGISTIC_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a - b * log ( ( 1.0 - cdf ) / cdf );

  return x;
}
//******************************************************************************

void logistic_cdf_values ( int *n_data, double *mu, double *beta, double *x,
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_CDF_VALUES returns some values of the Logistic CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = LogisticDistribution [ mu, beta ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    30 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *BETA, the shape parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double beta_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.8807970779778824E+00,  
     0.9820137900379084E+00,  
     0.9975273768433652E+00,  
     0.6224593312018546E+00,  
     0.5825702064623147E+00,  
     0.5621765008857981E+00,  
     0.5498339973124779E+00,  
     0.6224593312018546E+00,  
     0.5000000000000000E+00,  
     0.3775406687981454E+00,  
     0.2689414213699951E+00 };

  double mu_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *beta = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *beta = beta_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool logistic_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_CHECK checks the parameters of the Logistic CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool LOGISTIC_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "LOGISTIC_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double logistic_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_MEAN returns the mean of the Logistic PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LOGISTIC_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double logistic_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_PDF evaluates the Logistic PDF.
//
//  Formula:
//
//    PDF(A,B;X) = exp ( ( A - X ) / B ) /
//      ( B * ( 1 + exp ( ( A - X ) / B ) )**2 )
//
//  Discussion:
//
//    The Logistic PDF is also known as the Sech-Squared PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LOGISTIC_PDF, the value of the PDF.
//
{
  double pdf;
  double temp;

  temp = exp ( ( a - x ) / b );

  pdf = temp / ( b * ( 1.0 + temp ) * ( 1.0 + temp ) );

  return pdf;
}
//******************************************************************************

double logistic_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_SAMPLE samples the Logistic PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double LOGISTIC_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = logistic_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double logistic_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    LOGISTIC_VARIANCE returns the variance of the Logistic PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double LOGISTIC_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = PI * PI * b * b / 3.0;

  return variance;
# undef PI
}
//******************************************************************************

double lorentz_cdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_CDF evaluates the Lorentz CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Output, double LORENTZ_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;

  cdf = 0.5 + atan ( x ) / PI;

  return cdf;
# undef PI
}
//******************************************************************************

double lorentz_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_CDF_INV inverts the Lorentz CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Output, double LORENTZ_CDF_INV, the corresponding argument.
//
{
# define PI 3.141592653589793

  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "LORENTZ_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = tan ( PI * ( cdf - 0.5 ) );

  return x;
# undef PI
}
//******************************************************************************

double lorentz_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_MEAN returns the mean of the Lorentz PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double LORENTZ_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = 0.0;

  return mean;
}
//******************************************************************************

double lorentz_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_PDF evaluates the Lorentz PDF.
//
//  Formula:
//
//    PDF(X) = 1 / ( PI * ( 1 + X**2 ) )
//
//  Discussion:
//
//    The chief interest of the Lorentz PDF is that it is easily
//    inverted, and can be used to dominate other PDF's in an
//    acceptance/rejection method.
//
//    LORENTZ_PDF(X) = CAUCHY_PDF(0,1;X)
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Output, double LORENTZ_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  pdf = 1.0 / ( PI * ( 1.0 + x * x ) );

  return pdf;
# undef PI
}
//******************************************************************************

double lorentz_sample ( int *seed )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_SAMPLE samples the Lorentz PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double LORENTZ_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = lorentz_cdf_inv ( cdf );

  return x;
}
//******************************************************************************

double lorentz_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    LORENTZ_VARIANCE returns the variance of the Lorentz PDF.
//
//  Discussion:
//
//    The variance of the Lorentz PDF is not well defined.  This routine
//    is made available for completeness only, and simply returns
//    a "very large" number.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double LORENTZ_VARIANCE, the mean of the PDF.
//
{
  double variance;

  variance = d_huge ( );

  return variance;
}
//******************************************************************************

double maxwell_cdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_CDF evaluates the Maxwell CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double MAXWELL_CDF, the value of the CDF.
//
{
  double cdf;
  double p2;
  double x2;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    x2 = x / a;
    p2 = 1.5;

    cdf = gamma_inc ( p2, x2 );
  }

  return cdf;
}
//******************************************************************************

double maxwell_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_CDF_INV inverts the Maxwell CDF.
//
//  Discussion:
//
//    A simple bisection method is used.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double MAXWELL_CDF_INV, the corresponding argument of the CDF.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "MAXWELL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = 0.0;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
    return x;
  }

  x1 = 0.0;
  cdf1 = 0.0;

  x2 = 1.0;

  for ( ; ; )
  {
    cdf2 = maxwell_cdf ( x2, a );

    if ( cdf < cdf2 )
    {
      break;
    }

    x2 = 2.0 * x2;

    if ( 1000000.0 < x2 )
    {
      cout << " \n";
      cout << "MAXWELL_CDF_INV - Fatal error!\n";
      cout << "  Initial bracketing effort fails.\n";
      exit ( 1 );
    }
  }
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = maxwell_cdf ( x3, a );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "MAXWELL_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      exit ( 1 );
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }
  }

  return x;
}
//******************************************************************************

bool maxwell_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_CHECK checks the parameters of the Maxwell CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, bool MAXWELL_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "MAXWELL_CHECK - Fatal error!\n";
    cout << "  A <= 0.0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double maxwell_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_MEAN returns the mean of the Maxwell PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double MAXWELL_MEAN, the mean value.
//
{
  double mean;

  mean = sqrt ( 2.0 ) * a * gamma ( 2.0 ) / gamma ( 1.5 );

  return mean;
}
//******************************************************************************

double maxwell_pdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_PDF evaluates the Maxwell PDF.
//
//  Formula:
//
//    PDF(A;X) = EXP ( - 0.5 * ( X / A )**2 ) * ( X / A )**2 /
//      ( sqrt ( 2 ) * A * GAMMA ( 1.5 ) )
//      
//  Discussion:
//
//    MAXWELL_PDF(A;X) = CHI_PDF(0,A,3;X)
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0 < X
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double MAXWELL_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    y = x / a;

    pdf = exp ( - 0.5 * y * y ) * y * y / ( sqrt ( 2.0 ) * a * gamma ( 1.5 ) );
  }

  return pdf;
}
//******************************************************************************

double maxwell_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_SAMPLE samples the Maxwell PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double MAXWELL_SAMPLE, a sample of the PDF.
//
{
  double a2;
  double x;

  a2 = 3.0;
  x = chi_square_sample ( a2, seed );

  x = a * sqrt ( x );

  return x;
}
//******************************************************************************

double maxwell_variance ( double a )

//******************************************************************************
//
//  Purpose:
//
//    MAXWELL_VARIANCE returns the variance of the Maxwell PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double MAXWELL_VARIANCE, the variance of the PDF.
//
{
  double temp;
  double variance;

  temp = gamma ( 2.0 ) / gamma ( 1.5 );

  variance = a * a * ( 3.0 - 2.0 * temp * temp );

  return variance;
}
//******************************************************************************

bool multicoef_check ( int nfactor, int factor[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTICOEF_CHECK checks the parameters of the multinomial coefficient.
//
//  Modified:
//
//    22 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int NFACTOR, the number of factors.
//    1 <= NFACTOR.
//
//    Input, int FACTOR(NFACTOR), contains the factors.
//    0.0 <= FACTOR(I).
//
//    Output, bool MULTICOEF_CHECK, is true if the parameters are legal.
//
{
  int i;

  if ( nfactor < 1 )
  {
    cout << " \n";
    cout << "MULTICOEF_CHECK - Fatal error!\n";
    cout << "  NFACTOR < 1.\n";
    return false;
  }

  for ( i = 0; i < nfactor; i++ )
  {
    if ( factor[i] < 0 )
    {
      cout << " \n";
      cout << "MULTICOEF_CHECK - Fatal error\n";
      cout << "  Factor[" << i << "] = " << factor[i] << "\n";
      cout << "  But this value must be nonnegative.\n";
      return false;
    }

  }

  return true;
}
//******************************************************************************

int multinomial_coef1 ( int nfactor, int factor[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_COEF1 computes a Multinomial coefficient.
//
//  Definition:
//
//    The multinomial coefficient is a generalization of the binomial
//    coefficient.  It may be interpreted as the number of combinations of
//    N objects, where FACTOR(1) objects are indistinguishable of type 1,
//    ... and FACTOR(NFACTOR) are indistinguishable of type NFACTOR,
//    and N is the sum of FACTOR(1) through FACTOR(NFACTOR).
//
//  Formula:
//
//    NCOMB = N! / ( FACTOR(1)! FACTOR(2)! ... FACTOR(NFACTOR)! )
//
//  Method:
//
//    The log of the gamma function is used, to avoid overflow.
//
//  Modified:
//
//    02 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int NFACTOR, the number of factors.
//    1 <= NFACTOR.
//
//    Input, int FACTOR(NFACTOR), contains the factors.
//    0.0 <= FACTOR(I).
//
//    Output, int MULTINOMIAL_COEF1, the value of the multinomial coefficient.
//
{
  double facn;
  int i;
  int n;
  int ncomb;

  if (  !multicoef_check ( nfactor, factor ) )
  {
    cout << " \n";
    cout << "MULTINOMIAL_COEF1 - Fatal error!\n";
    cout << "  MULTICOEF_CHECK failed.\n";
    ncomb = -i_huge ( );
    return ncomb;
  }
//
//  The factors sum to N.
//
  n = ivec_sum ( nfactor, factor );

  facn = factorial_log ( n );

  for ( i = 0; i < nfactor; i++ )
  {
    facn = facn - factorial_log ( factor[i] );
  }

  ncomb = d_nint ( exp ( facn ) );

  return ncomb;
}
//******************************************************************************

int multinomial_coef2 ( int nfactor, int factor[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_COEF2 computes a Multinomial coefficient.
//
//  Definition:
//
//    The multinomial coefficient is a generalization of the binomial
//    coefficient.  It may be interpreted as the number of combinations of
//    N objects, where FACTOR(1) objects are indistinguishable of type 1,
//    ... and FACTOR(NFACTOR) are indistinguishable of type NFACTOR,
//    and N is the sum of FACTOR(1) through FACTOR(NFACTOR).
//
//  Formula:
//
//    NCOMB = N! / ( FACTOR(1)! FACTOR(2)! ... FACTOR(NFACTOR)! )
//
//  Method:
//
//    A direct method is used, which should be exact.  However, there
//    is a possibility of intermediate overflow of the result.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int NFACTOR, the number of factors.
//    1 <= NFACTOR.
//
//    Input, int FACTOR[NFACTOR], contains the factors.
//    0 <= FACTOR(I).
//
//    Output, int MULTINOMIAL_COEF2, the value of the multinomial coefficient.
//
{
  int i;
  int j;
  int k;
  int ncomb;

  if ( !multicoef_check ( nfactor, factor ) )
  {
    cout << " \n";
    cout << "MULTINOMIAL_COEF2 - Fatal error!\n";
    cout << "  MULTICOEF_CHECK failed.\n";
    ncomb = - i_huge ( );
    return ncomb;
  }

  ncomb = 1;
  k = 0;

  for ( i = 0; i < nfactor; i++ )
  {
    for ( j = 1; j <= factor[i]; j++ )
    {
      k = k + 1;
      ncomb = ( ncomb * k ) / j;
    }
  }

  return ncomb;
}
//******************************************************************************

bool multinomial_check ( int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_CHECK checks the parameters of the Multinomial PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//
//    Input, int B, the number of outcomes possible on one trial.
//    1 <= B.
//
//    Input, double C(B).  C(I) is the probability of outcome I on
//    any trial.
//    0.0 <= C(I) <= 1.0,
//    Sum ( 1 <= I <= B ) C(I) = 1.0.
//
//    Output, bool MULTINOMIAL_CHECK, is true if the parameters are legal.
//
{
  double c_sum;
  int i;

  if ( b < 1 )
  {
    cout << " \n";
    cout << "MULTINOMIAL_CHECK - Fatal error!\n";
    cout << "  B < 1.\n";
    return false;
  }

  for ( i = 0; i < b; i++ )
  {
    if ( c[i] < 0.0 || 1.0 < c[i] )
    {
      cout << " \n";
      cout << "MULTINOMIAL_CHECK - Fatal error!\n";
      cout << "  Input C(I) is out of range.\n";
      return false;
    }
  }

  c_sum = dvec_sum ( b, c );

  if ( 0.0001 < fabs ( 1.0 - c_sum ) )
  {
    cout << " \n";
    cout << "MULTINOMIAL_CHECK - Fatal error!\n";
    cout << "  The probabilities do not sum to 1.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double *multinomial_covariance ( int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_COVARIANCE returns the covariances of the Multinomial PDF.
//
//  Modified:
//
//    14 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//
//    Input, int B, the number of outcomes possible on one trial.
//    1 <= B.
//
//    Input, double C(B).  C(I) is the probability of outcome I on
//    any trial.
//    0.0 <= C(I) <= 1.0,
//    SUM ( 1 <= I <= B) C(I) = 1.0.
//
//    Output, double MULTINOMIAL_COVARIANCE[B*B], the covariance matrix.
//
{
  double *covariance;
  int i;
  int j;

  covariance = new double[b*b];

  for ( i = 0; i < b; i++)
  {
    for ( j = 0; j < b; j++ )
    {
      if ( i == j )
      {
        covariance[i+j*b] = ( double ) ( a ) * c[i] * ( 1.0 - c[i] );
      }
      else
      {
        covariance[i+j*b] = - ( double ) ( a ) * c[i] * c[j];
      }
    }
  }

  return covariance;
}
//******************************************************************************

double *multinomial_mean ( int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_MEAN returns the means of the Multinomial PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//
//    Input, int B, the number of outcomes possible on one trial.
//    1 <= B.
//
//    Input, double C(B).  C(I) is the probability of outcome I on
//    any trial.
//    0.0 <= C(I) <= 1.0,
//    SUM ( 1 <= I <= B) C(I) = 1.0.
//
//    Output, double MEAN(B), MEAN(I) is the expected value of the 
//    number of outcome I in N trials.
//
{
  int i;
  double *mean;

  mean = new double[b];

  for ( i = 0; i < b; i++ )
  {
    mean[i] = ( double ) ( a ) * c[i];
  }

  return mean;
}
//******************************************************************************

double multinomial_pdf ( int x[], int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_PDF computes a Multinomial PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = Comb(A,B,X) * Product ( 1 <= I <= B ) C(I)**X(I)
//
//    where Comb(A,B,X) is the multinomial coefficient
//      C( A; X(1), X(2), ..., X(B) )
//
//  Discussion:
//
//    PDF(A,B,C;X) is the probability that in A trials there
//    will be exactly X(I) occurrences of event I, whose probability
//    on one trial is C(I), for I from 1 to B.
//
//    As soon as A or B gets large, the number of possible X's explodes,
//    and the probability of any particular X can become extremely small.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X[B]; X(I) counts the number of occurrences of
//    outcome I, out of the total of A trials.
//
//    Input, int A, the total number of trials.
//
//    Input, int B, the number of different possible outcomes on 
//    one trial.
//
//    Input, double C[B]; C(I) is the probability of outcome I on 
//    any one trial.
//
//    Output, double MULTINOMIAL_PDF, the value of the multinomial PDF.
//
{
  int i;
  double pdf;
  double pdf_log;
//
//  To try to avoid overflow, do the calculation in terms of logarithms.
//  Note that Gamma(A+1) = A factorial.
//
  pdf_log = gamma_log ( ( double ) ( a + 1 ) );

  for ( i = 0; i < b; i++ )
  {
    pdf_log = pdf_log + x[i] * log ( c[i] ) 
      - gamma_log ( ( double ) ( x[i] + 1 ) );
  }

  pdf = exp ( pdf_log );

  return pdf;
}
//******************************************************************************

int *multinomial_sample ( int a, int b, double c[], int *seed )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_SAMPLE samples the Multinomial PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Luc Devroye,
//    Non-Uniform Random Variate Generation,
//    Springer-Verlag, New York, 1986, page 559.
//
//  Parameters:
//
//    Input, int A, the total number of trials.
//    0 <= A.
//
//    Input, int B, the number of outcomes possible on one trial.
//    1 <= B.
//
//    Input, double C[B].  C(I) is the probability of outcome I on
//    any trial.
//    0.0 <= C(I) <= 1.0,
//    SUM ( 1 <= I <= B) C(I) = 1.0.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int X[B]; X(I) is the number of
//    occurrences of event I during the N trials.
//
{
  int i;
  int ifactor;
  int ntot;
  double prob;
  double sum2;
  int *x;

  x = new int[b];
  ntot = a;

  sum2 = 1.0;

  for ( i = 0; i < b; i++ )
  {
    x[i] = 0;
  }

  for ( ifactor = 0; i < b-1; i++ )
  {
    prob = c[ifactor] / sum2;
//
//  Generate a binomial random deviate for NTOT trials with 
//  single trial success probability PROB.
//
    x[ifactor] = binomial_sample ( ntot, prob, seed );

    ntot = ntot - x[ifactor];
    if ( ntot <= 0 )
    {
      return x;
    }

    sum2 = sum2 - c[ifactor];
  }
//
//  The last factor gets what's left.
//
  x[b-1] = ntot;

  return x;
}
//******************************************************************************

double *multinomial_variance ( int a, int b, double c[] )

//******************************************************************************
//
//  Purpose:
//
//    MULTINOMIAL_VARIANCE returns the variances of the Multinomial PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, the number of trials.
//
//    Input, int B, the number of outcomes possible on one trial.
//    1 <= B.
//
//    Input, double C[B].  C(I) is the probability of outcome I on
//    any trial.
//    0.0 <= C(I) <= 1.0,
//    SUM ( 1 <= I <= B ) C(I) = 1.0.
//
//    Output, double VARIANCE(B), VARIANCE(I) is the variance of the 
//    total number of events of type I.
//
{
  int i;
  double *variance;

  variance = new double[b];

  for ( i = 0; i < b; i++ )
  {
    variance[i] = ( double ) ( a ) * c[i] * ( 1.0 - c[i] );
  }

  return variance;
}
//******************************************************************************

double *multivariate_normal_sample ( int n, double mean[], double covar_factor[],
  int *seed )

//******************************************************************************
//
//  Purpose:
//
//    MULTIVARIATE_NORMAL_SAMPLE samples the Multivariate Normal PDF.
//
//  Discussion:
//
//    PDF ( Mean(1:N), S(1:N,1:N); X(1:N) ) = 1 / ( 2 * PI * det ( S ) )^(N/2) 
//      * exp ( - ( X - Mean )' * inverse ( S ) * ( X - Mean ) / 2 )
//
//    Here, 
//
//      X is the argument vector of length N,
//      Mean is the mean vector of length N,
//      S is an N by N positive definite symmetric covariance matrix.
//
//    The properties of S guarantee that it has a lower triangular
//    matrix L, the Cholesky factor, such that S = L * L'.  It is the
//    matrix L, rather than S, that is required by this routine.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 167.
//
//  Parameters:
//
//    Input, int N, the spatial dimension.
//
//    Input, double MEAN[N], the mean vector.
// 
//    Input, double COVAR_FACTOR[N*N], the lower triangular Cholesky
//    factor L of the covariance matrix S.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double MULTIVARIATE_NORMAL_SAMPLE[N], a sample point
//    of the distribution.
//
{
  int i;
  int j;
  double *x;
  double z;

  x = new double[n];

  for ( i = 0; i < n; i++ )
  {
    z = normal_01_sample ( seed );

    x[i] = mean[i];

    for ( j = 0; j <= i; j++ )
    {
      x[i] = x[i] + covar_factor[i+j*n] * z;
    }
  }

  return x;
}
//******************************************************************************

double nakagami_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    NAKAGAMI_CDF evaluates the Nakagami CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B
//    0.0 < C.
//
//    Output, double NAKAGAMI_CDF, the value of the CDF.
//
{
  double cdf;
  double p2;
  double x2;
  double y;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else if ( 0.0 < x )
  {
    y = ( x - a ) / b;
    x2 = c * y * y;
    p2 = c;

    cdf = gamma_inc ( p2, x2 );
  }

  return cdf;
}
//******************************************************************************

bool nakagami_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    NAKAGAMI_CHECK checks the parameters of the Nakagami PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, bool NAKAGAMI_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "NAKAGAMI_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "NAKAGAMI_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double nakagami_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    NAKAGAMI_MEAN returns the mean of the Nakagami PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B
//    0.0 < C
//
//    Output, double NAKAGAMI_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a + b * gamma ( c + 0.5 ) / ( sqrt ( c ) * gamma ( c ) );

  return mean;
}
//******************************************************************************

double nakagami_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    NAKAGAMI_PDF evaluates the Nakagami PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B
//    0.0 < C.
//
//    Output, double NAKAGAMI_PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x <= 0.0 )
  {
    pdf = 0.0;
  }
  else if ( 0.0 < x )
  {
    y = ( x - a ) / b;

    pdf = 2.0 * pow ( c, c ) / ( b * gamma ( c ) ) * pow ( y, ( 2.0 * c - 1.0 ) )
      * exp ( - c * y * y );

  }

  return pdf;
}
//******************************************************************************

double nakagami_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    NAKAGAMI_VARIANCE returns the variance of the Nakagami PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B
//    0.0 < C
//
//    Output, double NAKAGAMI_VARIANCE, the variance of the PDF.
//
{
  double t1;
  double t2;
  double variance;

  t1 = gamma ( c + 0.5 );
  t2 = gamma ( c );

  variance = b * b * ( 1.0 - t1 * t1 / ( c * t2 * t2 ) );

  return variance;
}
//******************************************************************************

double negative_binomial_cdf ( int x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_CDF evaluates the Negative Binomial CDF.
//
//  Discussion:
//
//    A simple summing approach is used.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the CDF.
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Output, double NEGATIVE_BINOMIAL_CDF, the value of the CDF.
//
{
  double cdf;
  int cnk;
  double pdf;
  int y;

  cdf = 0.0;

  for ( y = a; y <= x; y++ )
  {
    cnk = binomial_coef ( y-1, a-1 );

    pdf = ( double ) ( cnk ) * pow ( b, a ) * pow ( 1.0 - b, y - a );

    cdf = cdf + pdf;
  }

  return cdf;
}
//******************************************************************************

int negative_binomial_cdf_inv ( double cdf, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_CDF_INV inverts the Negative Binomial CDF.
//
//  Discussion:
//
//    A simple discrete approach is used.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Output, int NEGATIVE_BINOMIAL_CDF_INV, the smallest X whose cumulative 
//    density function is greater than or equal to CDF.
//
{
  double cum;
  double pdf;
  int x;
  int x_max = 1000;
//
  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "NEGATIVE_BINOMIAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  cum = 0.0;

  x = a;

  for ( ; ; )
  {
    pdf = negative_binomial_pdf ( x, a, b );

    cum = cum + pdf;

    if ( cdf <= cum || x_max <= x )
    {
      break;
    }
    x = x + 1;
  }

  return x;
}
//******************************************************************************

void negative_binomial_cdf_values ( int *n_data, int *f, int *s, double *p, 
  double *cdf )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_CDF_VALUES returns values of the negative binomial CDF.
//
//  Discussion:
//
//    Assume that a coin has a probability P of coming up heads on
//    any one trial.  Suppose that we plan to flip the coin until we
//    achieve a total of S heads.  If we let F represent the number of
//    tails that occur in this process, then the value of F satisfies
//    a negative binomial PDF:
//
//      PDF(F,S,P) = Choose ( F from F+S-1 ) * P**S * (1-P)**F
//
//    The negative binomial CDF is the probability that there are F or
//    fewer failures upon the attainment of the S-th success.  Thus,
//
//      CDF(F,S,P) = sum ( 0 <= G <= F ) PDF(G,S,P)
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = NegativeBinomialDistribution [ s, p ]
//      CDF [ dist, f ]
//
//  Modified:
//
//    24 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    F C Powell,
//    Statistical Tables for Sociology, Biology and Physical Sciences,
//    Cambridge University Press, 1982.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *F, the maximum number of failures.
//
//    Output, int *S, the number of successes.
//
//    Output, double *P, the probability of a success on one trial.
//
//    Output, double *CDF, the probability of at most F failures 
//    before the S-th success.
//
{
# define N_MAX 27

  double cdf_vec[N_MAX] = { 
     0.6367187500000000E+00,  
     0.3632812500000000E+00,  
     0.1445312500000000E+00,  
     0.5000000000000000E+00,  
     0.2265625000000000E+00,  
     0.6250000000000000E-01,  
     0.3437500000000000E+00,  
     0.1093750000000000E+00,  
     0.1562500000000000E-01,  
     0.1792000000000000E+00,  
     0.4096000000000000E-01,  
     0.4096000000000000E-02,  
     0.7047000000000000E-01,  
     0.1093500000000000E-01,  
     0.7290000000000000E-03,  
     0.9861587127990000E+00,  
     0.9149749500510000E+00,  
     0.7471846521450000E+00,  
     0.8499053647030009E+00,  
     0.5497160941090026E+00,  
     0.2662040052146710E+00,  
     0.6513215599000000E+00,  
     0.2639010709000000E+00,  
     0.7019082640000000E-01,  
     0.1000000000000000E+01,  
     0.1990000000000000E-01,  
     0.1000000000000000E-03 };

  int f_vec[N_MAX] = { 
     4,  3,  2, 
     3,  2,  1, 
     2,  1,  0, 
     2,  1,  0, 
     2,  1,  0, 
    11, 10,  9, 
    17, 16, 15, 
     9,  8,  7, 
     2,  1,  0 };

  double p_vec[N_MAX] = { 
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     0.40E+00,  
     0.40E+00,  
     0.40E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00,  
     0.30E+00,  
     0.10E+00,  
     0.10E+00,  
     0.10E+00,  
     0.10E+00,  
     0.10E+00,  
     0.10E+00,  
     0.10E-01,  
     0.10E-01,  
     0.10E-01 };

  int s_vec[N_MAX] = { 
    4, 5, 6, 
    4, 5, 6, 
    4, 5, 6, 
    4, 5, 6, 
    4, 5, 6, 
    1, 2, 3, 
    1, 2, 3, 
    1, 2, 3, 
    0, 1, 2 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *f = 0;
    *s = 0;
    *p = 0.0;
    *cdf = 0.0;
  }
  else
  {
    *f = f_vec[*n_data-1];
    *s = s_vec[*n_data-1];
    *p = p_vec[*n_data-1];
    *cdf = cdf_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool negative_binomial_check ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_CHECK checks the parameters of the Negative Binomial PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Output, bool NEGATIVE_BINOMIAL_CHECK, is true if the 
//    parameters are legal.
//
{
  if ( a < 0 )
  {
    cout << " \n";
    cout << "NEGATIVE_BINOMIAL_CHECK - Fatal error!\n";
    cout << "  A < 0.\n";
    return false;
  }

  if ( b <= 0.0 || 1.0 < b )
  {
    cout << " \n";
    cout << "NEGATIVE_BINOMIAL_CHECK - Fatal error!\n";
    cout << "  B <= 0 or 1 < B.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double negative_binomial_mean ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_MEAN returns the mean of the Negative Binomial PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Output, double NEGATIVE_BINOMIAL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( double ) ( a ) / b;

  return mean;
}
//******************************************************************************

double negative_binomial_pdf ( int x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_PDF evaluates the Negative Binomial PDF.
//
//  Formula:
//
//    PDF(A,B;X) = C(X-1,A-1) * B**A * ( 1 - B )**(X-A)
//
//  Discussion:
//
//    PDF(A,B;X) is the probability that the A-th success will
//    occur on the X-th trial, given that the probability
//    of a success on a single trial is B.
//
//    The Negative Binomial PDF is also known as the Pascal PDF or 
//    the "Polya" PDF.
//
//    NEGATIVE_BINOMIAL_PDF(1,B;X) = GEOMETRIC_PDF(B;X)
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the number of trials.
//    A <= X.
//
//    Input, int A, the number of successes required.
//    0 <= A <= X, normally.
//
//    Input, double B, the probability of a success on a single trial.
//    0.0 < B <= 1.0.
//
//    Output, double NEGATIVE_BINOMIAL_PDF, the value of the PDF.
//
{
  int cnk;
  double pdf;

  if ( x < a )
  {
    pdf = 0.0;
  }
  else
  {
    cnk = binomial_coef ( x-1, a-1 );

    pdf = ( double ) ( cnk ) * pow ( b, a ) * pow ( 1.0 - b, x - a );
  }

  return pdf;
}
//******************************************************************************

int negative_binomial_sample ( int a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_SAMPLE samples the Negative Binomial PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int NEGATIVE_BINOMIAL_SAMPLE, a sample of the PDF.
//
{
  int num_success;
  double r;
  int x;

  if ( b == 1.0 )
  {
    x = a;
    return x;
  }
  else if ( b == 0.0 )
  {
    x = i_huge ( );
    return x;
  }

  x = 0;
  num_success = 0;

  while ( num_success < a )
  {
    x = x + 1;
    r = d_uniform_01 ( seed );

    if ( r <= b )
    {
      num_success = num_success + 1;
    }

  }

  return x;
}
//******************************************************************************

double negative_binomial_variance ( int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NEGATIVE_BINOMIAL_VARIANCE returns the variance of the Negative Binomial PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, double B, parameters of the PDF.
//    0 <= A,
//    0 < B <= 1.
//
//    Output, double NEGATIVE_BINOMIAL_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( double ) ( a ) * ( 1.0 - b ) / ( b * b );

  return variance;
}
//******************************************************************************

double normal_01_cdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_CDF evaluates the Normal 01 CDF.
//
//  Modified:
//
//    10 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference: 
//
//    A G Adams,
//    Areas Under the Normal Curve,
//    Algorithm 39, 
//    Computer j., 
//    Volume 12, pages 197-198, 1969.
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Output, double CDF, the value of the CDF.
//
{
  double a1 = 0.398942280444E+00;
  double a2 = 0.399903438504E+00;
  double a3 = 5.75885480458E+00;
  double a4 = 29.8213557808E+00;
  double a5 = 2.62433121679E+00;
  double a6 = 48.6959930692E+00;
  double a7 = 5.92885724438E+00;
  double b0 = 0.398942280385E+00;
  double b1 = 3.8052E-08;
  double b2 = 1.00000615302E+00;
  double b3 = 3.98064794E-04;
  double b4 = 1.98615381364E+00;
  double b5 = 0.151679116635E+00;
  double b6 = 5.29330324926E+00;
  double b7 = 4.8385912808E+00;
  double b8 = 15.1508972451E+00;
  double b9 = 0.742380924027E+00;
  double b10 = 30.789933034E+00;
  double b11 = 3.99019417011E+00;
  double cdf;
  double q;
  double y;
//
//  |X| <= 1.28.
//
  if ( fabs ( x ) <= 1.28 )
  {
    y = 0.5 * x * x;

    q = 0.5 - fabs ( x ) * ( a1 - a2 * y / ( y + a3 - a4 / ( y + a5 
      + a6 / ( y + a7 ) ) ) );
//
//  1.28 < |X| <= 12.7
//
  }
  else if ( fabs ( x ) <= 12.7 )
  {
    y = 0.5 * x * x;

    q = exp ( - y ) * b0 / ( fabs ( x ) - b1 
      + b2 / ( fabs ( x ) + b3 
      + b4 / ( fabs ( x ) - b5 
      + b6 / ( fabs ( x ) + b7 
      - b8 / ( fabs ( x ) + b9 
      + b10 / ( fabs ( x ) + b11 ) ) ) ) ) );
//
//  12.7 < |X|
//
  }
  else
  {
    q = 0.0;
  }
//
//  Take account of negative X.
//
  if ( x < 0.0 )
  {
    cdf = q;
  }
  else
  {
    cdf = 1.0 - q;
  }

  return cdf;
}
//******************************************************************************

double normal_01_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_CDF_INV inverts the Normal 01 CDF.
//
//  Modified:
//
//    18 September 2004
//
//  Reference:
//
//    J D Beasley and S G Springer,
//    The Percentage Points of the Normal Distribution,
//    Algorithm AS 111, 
//    Applied Statistics, 
//    Volume 26, pages 118-121, 1977.
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double X, the corresponding argument.
//
{
  double a0 =   2.50662823884E+00;
  double a1 = -18.61500062529E+00;
  double a2 =  41.39119773534E+00;
  double a3 = -25.44106049637E+00;
  double b1 =  -8.47351093090E+00;
  double b2 =  23.08336743743E+00;
  double b3 = -21.06224101826E+00;
  double b4 =   3.13082909833E+00;
  double c0 =  -2.78718931138E+00;
  double c1 =  -2.29796479134E+00;
  double c2 =   4.85014127135E+00;
  double c3 =   2.32121276858E+00;
  double d1 =   3.54388924762E+00;
  double d2 =   1.63706781897E+00;
  double q;
  double r;
  double x;
//
  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "NORMAL_01_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = -d_huge ( );
    return x;
  }

  if ( cdf == 1.0 )
  {
    x = d_huge ( );
    return x;
  }

  q = cdf - 0.5;

  q = d_min ( q,  0.5 );
  q = d_max ( q, -0.5 );
//
//  0.08 < CDF < 0.92
//
  if ( fabs ( q ) <= 0.42 )
  {
    r = q * q;

    x = q * ( ( ( 
           a3   * r 
         + a2 ) * r 
         + a1 ) * r 
         + a0 ) / ( ( ( ( 
           b4   * r 
         + b3 ) * r 
         + b2 ) * r 
         + b1 ) * r + 1.0 );
//
//  CDF < 0.08 or 0.92 < CDF.
//
  }
  else
  {
    r = d_min ( cdf, 1.0 - cdf );
    r = d_max ( r, 0.0 );
    r = d_min ( r, 1.0 );

    r = sqrt ( - log ( r ) );

    x = ( ( ( 
           c3   * r 
         + c2 ) * r 
         + c1 ) * r 
         + c0 ) / ( ( 
           d2   * r 
         + d1 ) * r + 1.0 );

    if ( q < 0.0 )
    {
      x = -x;
    }

  }

  return x;
}
//******************************************************************************

void normal_01_cdf_values ( int *n_data, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_CDF_VALUES returns some values of the Normal 01 CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = NormalDistribution [ 0, 1 ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    28 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 17

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.5398278372770290E+00,  
     0.5792597094391030E+00,  
     0.6179114221889526E+00,  
     0.6554217416103242E+00,  
     0.6914624612740131E+00,  
     0.7257468822499270E+00,  
     0.7580363477769270E+00,  
     0.7881446014166033E+00,  
     0.8159398746532405E+00,  
     0.8413447460685429E+00,  
     0.9331927987311419E+00,  
     0.9772498680518208E+00,  
     0.9937903346742239E+00,  
     0.9986501019683699E+00,  
     0.9997673709209645E+00,  
     0.9999683287581669E+00 };

  double x_vec[N_MAX] = { 
     0.0000000000000000E+00,    
     0.1000000000000000E+00,  
     0.2000000000000000E+00,  
     0.3000000000000000E+00,  
     0.4000000000000000E+00,  
     0.5000000000000000E+00,  
     0.6000000000000000E+00,  
     0.7000000000000000E+00,  
     0.8000000000000000E+00,  
     0.9000000000000000E+00,  
     0.1000000000000000E+01,  
     0.1500000000000000E+01,  
     0.2000000000000000E+01,  
     0.2500000000000000E+01,  
     0.3000000000000000E+01,  
     0.3500000000000000E+01,  
     0.4000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double normal_01_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_MEAN returns the mean of the Normal 01 PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = 0.0;

  return mean;
}
//******************************************************************************

double normal_01_pdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_PDF evaluates the Normal 01 PDF.
//
//  Discussion:
//
//    The Normal 01 PDF is also called the "Standard Normal" PDF, or
//    the Normal PDF with 0 mean and variance 1.
//
//  Formula:
//
//    PDF(X) = exp ( - 0.5 * X**2 ) / sqrt ( 2 * PI )
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  pdf = exp ( -0.5 * x * x ) / sqrt ( 2.0 * PI );

  return pdf;
# undef PI
}
//**********************************************************************

double normal_01_sample ( int *seed )

//**********************************************************************
//
//  Purpose:
//
//    NORMAL_01_SAMPLE samples the standard normal probability distribution.
//
//  Discussion:
//
//    The standard normal probability distribution function (PDF) has 
//    mean 0 and standard deviation 1.
//
//  Method:
//
//    The Box-Muller method is used, which is efficient, but 
//    generates two values at a time.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double NORMAL_01_SAMPLE, a normally distributed random value.
//
{
# define PI 3.141592653589793

  double r1;
  double r2;
  static int used = -1;
  double x;
  static double y = 0.0;

  if ( used == -1 )
  {
    used = 0;
  }
//
//  If we've used an even number of values so far, generate two more, return one,
//  and save one.
//
  if ( ( used % 2 )== 0 )
  {
    for ( ; ; )
    {
      r1 = d_uniform_01 ( seed );
      if ( r1 != 0.0 )
      {
        break;
      }
    }

    r2 = d_uniform_01 ( seed );

    x = sqrt ( -2.0 * log ( r1 ) ) * cos ( 2.0 * PI * r2 );
    y = sqrt ( -2.0 * log ( r1 ) ) * sin ( 2.0 * PI * r2 );
  }
  else
  {

    x = y;

  }

  used = used + 1;

  return x;
# undef PI
}
//******************************************************************************

double normal_01_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_VARIANCE returns the variance of the Normal 01 PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 1.0;

  return variance;
}
//******************************************************************************

double *normal_01_vector ( int n, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_01_VECTOR samples the standard normal probability distribution.
//
//  Discussion:
//
//    The standard normal probability distribution function (PDF) has
//    mean 0 and standard deviation 1.
//
//    This routine can generate a vector of values on one call.  It
//    has the feature that it should provide the same results
//    in the same order no matter how we break up the task.
//
//    Before calling this routine, the user may call RANDOM_SEED
//    in order to set the seed of the random number generator.
//
//  Method:
//
//    The Box-Muller method is used, which is efficient, but
//    generates an even number of values each time.  On any call
//    to this routine, an even number of new values are generated.
//    Depending on the situation, one value may be left over.
//    In that case, it is saved for the next call.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of values desired.  If N is negative,
//    then the code will flush its internal memory; in particular,
//    if there is a saved value to be used on the next call, it is
//    instead discarded.  This is useful if the user has reset the
//    random number seed, for instance.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X(N), a sample of the standard normal PDF.
//
//  Local parameters:
//
//    Local, int MADE, records the number of values that have
//    been computed.  On input with negative N, this value overwrites
//    the return value of N, so the user can get an accounting of
//    how much work has been done.
//
//    Local, real R(N+1), is used to store some uniform random values.
//    Its dimension is N+1, but really it is only needed to be the
//    smallest even number greater than or equal to N.
//
//    Local, int SAVED, is 0 or 1 depending on whether there is a
//    single saved value left over from the previous call.
//
//    Local, int X_LO, X_HI, records the range of entries of
//    X that we need to compute.  This starts off as 1:N, but is adjusted
//    if we have a saved value that can be immediately stored in X(1),
//    and so on.
//
//    Local, real Y, the value saved from the previous call, if
//    SAVED is 1.
//
{
# define PI 3.141592653589793

  int i;
  int m;
  static int made = 0;
  double *r;
  static int saved = 0;
  double *x;
  int x_hi;
  int x_lo;
  static double y = 0.0;

  x = new double[n];
//
//  I'd like to allow the user to reset the internal data.
//  But this won't work properly if we have a saved value Y.
//  I'm making a crock option that allows the user to signal
//  explicitly that any internal memory should be flushed,
//  by passing in a negative value for N.
//
  if ( n < 0 )
  {
    made = 0;
    saved = 0;
    y = 0.0;
    return NULL;
  }
  else if ( n == 0 )
  {
    return NULL;
  }
//
//  Record the range of X we need to fill in.
//
  x_lo = 1;
  x_hi = n;
//
//  Use up the old value, if we have it.
//
  if ( saved == 1 )
  {
    x[0] = y;
    saved = 0;
    x_lo = 2;
  }
//
//  Maybe we don't need any more values.
//
  if ( x_hi - x_lo + 1 == 0 )
  {
  }
//
//  If we need just one new value, do that here to avoid null arrays.
//
  else if ( x_hi - x_lo + 1 == 1 )
  {
    r = dvec_uniform_01 ( 2, seed );

    x[x_hi-1] = sqrt ( -2.0 * log ( r[0] ) ) * cos ( 2.0 * PI * r[1] );
    y =         sqrt ( -2.0 * log ( r[0] ) ) * sin ( 2.0 * PI * r[1] );

    saved = 1;

    made = made + 2;

    delete [] r;
  }
//
//  If we require an even number of values, that's easy.
//
  else if ( ( x_hi - x_lo + 1 ) % 2 == 0 )
  {
    m = ( x_hi - x_lo + 1 ) / 2;

    r = dvec_uniform_01 ( 2*m, seed );

    for ( i = 0; i <= 2*m-2; i = i + 2 )
    {
      x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
      x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
    }
    made = made + x_hi - x_lo + 1;

    delete [] r;
  }
//
//  If we require an odd number of values, we generate an even number,
//  and handle the last pair specially, storing one in X(N), and
//  saving the other for later.
//
  else
  {
    x_hi = x_hi - 1;

    m = ( x_hi - x_lo + 1 ) / 2 + 1;

    r = dvec_uniform_01 ( 2*m, seed );

    for ( i = 0; i <= 2*m-4; i = i + 2 )
    {
      x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
      x[x_lo+i  ] = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );
    }

    i = 2*m - 2;

    x[x_lo+i-1] = sqrt ( -2.0 * log ( r[i] ) ) * cos ( 2.0 * PI * r[i+1] );
    y           = sqrt ( -2.0 * log ( r[i] ) ) * sin ( 2.0 * PI * r[i+1] );

    saved = 1;

    made = made + x_hi - x_lo + 2;

    delete [] r;
  }

  return x;
# undef PI
}
//******************************************************************************

double normal_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_CDF evaluates the Normal CDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  double y;

  y = ( x - a ) / b;

  cdf = normal_01_cdf ( y );

  return cdf;
}
//******************************************************************************

double normal_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_CDF_INV inverts the Normal CDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Algorithm AS 111, 
//    Applied Statistics, 
//    Volume 26, pages 118-121, 1977.
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double X, the corresponding argument.
//
{
  double x;
  double x2;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "NORMAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x2 = normal_01_cdf_inv ( cdf );

  x = a + b * x2;

  return x;
}
//******************************************************************************

void normal_cdf_values ( int *n_data, double *mu, double *sigma, double *x, 
  double *fx )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_CDF_VALUES returns some values of the Normal CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = NormalDistribution [ mu, sigma ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    05 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *MU, the mean of the distribution.
//
//    Output, double *SIGMA, the variance of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.9772498680518208E+00,  
     0.9999683287581669E+00,  
     0.9999999990134124E+00,  
     0.6914624612740131E+00,  
     0.6305586598182364E+00,  
     0.5987063256829237E+00,  
     0.5792597094391030E+00,  
     0.6914624612740131E+00,  
     0.5000000000000000E+00,  
     0.3085375387259869E+00,  
     0.1586552539314571E+00 };

  double mu_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double sigma_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *mu = 0.0;
    *sigma = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *mu = mu_vec[*n_data-1];
    *sigma = sigma_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool normal_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_CHECK checks the parameters of the Normal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, bool NORMAL_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "NORMAL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double normal_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_MEAN returns the mean of the Normal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double normal_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_PDF evaluates the Normal PDF.
//
//  Formula:
//
//    PDF(A,B;X) 
//      = exp ( - 0.5 * ( ( X - A ) / B )**2 )
//      / ( B * SQRT ( 2 * PI ) )
//
//  Discussion:
//
//    The normal PDF is also known as the Gaussian PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  y = ( x - a ) / b;

  pdf = exp ( -0.5 * y * y )  / ( b * sqrt ( 2.0 * PI ) );

  return pdf;
# undef PI
}
//******************************************************************************

double normal_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_SAMPLE samples the Normal PDF.
//
//  Method:
//
//    The Box-Muller method is used.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
# define PI 3.141592653589793

  int static iset = -1;
  double v1;
  double v2;
  double x;
  double static y = 0.0;

  if ( iset == -1 )
  {
    iset = 0;
  }

  if ( iset == 0 )
  {
    for ( ; ; )
    {
      v1 = d_uniform_01 ( seed );

      if ( v1 != 0.0 ) 
      {
        break;
      }
    }
    
    v2 = d_uniform_01 ( seed );

    x = a + b * sqrt ( - 2.0 * log ( v1 ) ) * cos ( 2.0 * PI * v2 );
    y = a + b * sqrt ( - 2.0 * log ( v1 ) ) * sin ( 2.0 * PI * v2 );

    iset = 1;
  }
  else
  {
    x = y;
    iset = 0;

  }

  return x;
# undef PI
}
//******************************************************************************

double normal_variance ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    NORMAL_VARIANCE returns the variance of the Normal PDF.
//
//  Modified:
//
//    19 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b;

  return variance;
}
//******************************************************************************

double *normal_vector ( int n, double mean, double dev, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    NORMAL_VECTOR samples the normal probability distribution.
//
//  Discussion:
//
//    The normal probability distribution function (PDF) has
//    a user-specified mean and standard deviation.
//
//    This routine can generate a vector of values on one call.  It
//    has the feature that it should provide the same results
//    in the same order no matter how we break up the task.
//
//    Before calling this routine, the user may call RANDOM_SEED
//    in order to set the seed of the random number generator.
//
//  Method:
//
//    The Box-Muller method is used, which is efficient, but
//    generates an even number of values each time.  On any call
//    to this routine, an even number of new values are generated.
//    Depending on the situation, one value may be left over.
//    In that case, it is saved for the next call.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of values desired.  If N is negative,
//    then the code will flush its internal memory; in particular,
//    if there is a saved value to be used on the next call, it is
//    instead discarded.  This is useful if the user has reset the
//    random number seed, for instance.
//
//    Input, double MEAN, the desired mean value.
//
//    Input, double DEV, the desired standard deviation.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double NORMAL_VECTOR[N], a sample of the normal PDF.
//
{
# define PI 3.141592653589793

  int i;
  double *x;

  x = normal_01_vector ( n, seed );

  for ( i = 0; i < n; i++ )
  {
    x[i] = mean + dev * x[i];
  }

  return x;
# undef PI
}
//******************************************************************************

double pareto_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_CDF evaluates the Pareto CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PARETO_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < a )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - pow ( ( a / x ), b );
  }

  return cdf;
}
//******************************************************************************

double pareto_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_CDF_INV inverts the Pareto CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PARETO_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "PARETO_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a / pow ( 1.0 - cdf, 1.0 / b );

  return x;
}
//******************************************************************************

bool pareto_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_CHECK checks the parameters of the Pareto CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, bool PARETO_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "PARETO_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "PARETO_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double pareto_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_MEAN returns the mean of the Pareto PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PARETO_MEAN, the mean of the PDF.
//
{
  double mean;

  if ( b <= 1.0 )
  {
    cout << " \n";
    cout << "PARETO_MEAN - Fatal error!\n";
    cout << "  For B <= 1, the mean does not exist.\n";
    mean = 0.0;
    return mean;
  }

  mean = b * a / ( b - 1.0 );

  return mean;
}
//******************************************************************************

double pareto_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_PDF evaluates the Pareto PDF.
//
//  Formula:
//
//    PDF(A,B;X) = B * A**B / X**(B+1).
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A.
//    0.0 < B.
//
//    Output, double PARETO_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < a )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = b * pow ( a, b ) / pow ( x, ( b + 1.0 ) );
  }

  return pdf;
}
//******************************************************************************

double pareto_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_SAMPLE samples the Pareto PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double PARETO_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = pareto_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double pareto_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PARETO_VARIANCE returns the variance of the Pareto PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PARETO_VARIANCE, the variance of the PDF.
//
{
  double variance;

  if ( b <= 2.0 )
  {
    cout << " \n";
    cout << "PARETO_VARIANCE - Warning!\n";
    cout << "  For B <= 2, the variance does not exist.\n";
    variance = 0.0;
    return variance;
  }

  variance = a * a * b / ( pow ( ( b - 1.0 ), 2 ) * ( b - 2.0 ) );

  return variance;
}
//******************************************************************************

bool pearson_05_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    PEARSON_05_CHECK checks the parameters of the Pearson 5 PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, bool PEARSON_05_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "PEARSON_05_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "PEARSON_05_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double pearson_05_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    PEARSON_05_MEAN evaluates the mean of the Pearson 5 PDF.
//
//  Discussion:
//
//    The mean is undefined for B <= 1.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double PEARSON_05_MEAN, the mean of the PDF.
//
{
  double mean;
  
  if ( b <= 1.0 )
  {
    cout << " \n";
    cout << "PEARSON_05_MEAN - Warning!\n";
    cout << "  MEAN undefined for B <= 1.\n";
    mean = c;
    return mean;
  }

  mean = c + a / ( b - 1.0 );
  
  return mean;
}
//******************************************************************************

double pearson_05_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    PEARSON_05_PDF evaluates the Pearson 5 PDF.
//
//  Formula:
//
//    PDF(A,B;X) = A**B * ( X - C )**(-B-1) 
//      * exp ( - A / ( X - C ) ) / Gamma ( B )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    C < X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= c )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = pow ( a, b ) * pow ( x - c, - b - 1.0 ) 
      * exp ( - a / ( x - c ) ) / gamma ( b );
  }

  return pdf;
}
//******************************************************************************

double pearson_05_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    PEARSON_05_SAMPLE samples the Pearson 5 PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double PEARSON_05_SAMPLE, a sample of the PDF.
//
{
  double a2;
  double b2;
  double c2;
  double x;
  double x2;

  a2 = 0.0;
  b2 = b;
  c2 = 1.0 / a;

  x2 = gamma_sample ( a2, b2, c2, seed );

  x = c + 1.0 / x2;
  
  return x;
}
//******************************************************************************

bool planck_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PLANCK_CHECK checks the parameters of the Planck PDF.
//
//  Modified:
//
//    26 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, bool PLANCK_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "PLANCK_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "PLANCK_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double planck_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PLANCK_MEAN returns the mean of the Planck PDF.
//
//  Modified:
//
//    26 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PLANCK_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( b + 1.0 ) * zeta ( b + 2.0 ) / zeta ( b + 1.0 );

  return mean;
}
//******************************************************************************

double planck_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PLANCK_PDF evaluates the Planck PDF.
//
//  Discussion:
//
//    The Planck PDF has the form
//
//      PDF(A,B;X) = A**(B+1) * X**B / ( exp ( A * X ) - 1 ) / K
//
//    where K is the normalization constant, and has the value
//
//      K = Gamma ( B + 1 ) * Zeta ( B + 1 ).
//
//    The original Planck distribution governed the frequencies in
//    blackbody radiation at a given temperature T, and has the form
//
//      PDF(A;X) = K * X**3 / ( exp ( A * X ) - 1 )
//
//    where 
//
//      K = 15 / PI**4.
//
//    Thus, in terms of the Planck PDF, the original Planck distribution
//    has A = 1, B = 3.
//
//  Modified:
//
//    26 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PLANCK_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double k;
  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    k = gamma ( b + 1.0 ) * zeta ( b + 1.0 );
    pdf = pow ( a, b + 1.0 ) * pow ( x, b ) / ( exp ( a * x ) - 1.0 ) / k;
  }

  return pdf;
# undef PI
}
//******************************************************************************

double planck_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    PLANCK_SAMPLE samples the Planck PDF.
//
//  Modified:
//
//    26 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Luc Devroye,
//    Non-Uniform Random Variate Generation,
//    Springer Verlag, 1986, pages 552.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double PLANCK_SAMPLE, a sample of the PDF.
//
{
  double a2;
  double b2;
  double c2;
  double g;
  double x;
  int z;
//
  a2 = 0.0;
  b2 = 1.0;
  c2 = b + 1.0;

  g = gamma_sample ( a2, b2, c2, seed );

  z = zipf_sample ( c2, seed );

  x = g / ( a * ( double ) ( z ) );

  return x;
}
//******************************************************************************

double planck_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    PLANCK_VARIANCE returns the variance of the Planck PDF.
//
//  Modified:
//
//    26 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A,
//    0.0 < B.
//
//    Output, double PLANCK_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 0.0;

  return variance;
}
//******************************************************************************

double point_distance_1d_pdf ( double x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POINT_DISTANCE_1D_PDF evaluates the point distance PDF in 1D.
//
//  Discussion:
//
//    It is assumed that a set of points has been generated in 1D
//    according to a Poisson process.  The number of points in a region 
//    of size LENGTH is a Poisson variate with mean value B * LENGTH.
//
//    For a point chosen at random, we may now find the nearest
//    Poisson point, the second nearest and so on.  We are interested
//    in the PDF that governs the expected behavior of the distances
//    of rank A = 1, 2, 3, ... with Poisson density B.
//
//    Note that this PDF is a form of the Gamma PDF.???
//
//  Formula:
//
//    PDF(A,B;X) = B**A * X**( A - 1 ) * EXP ( - B * X ) / ( A - 1 )!
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X.
//
//    Input, int A, indicates the degree of nearness of the point.
//    A = 1 means the nearest point, A = 2 the second nearest, and so on.
//    0 < A.
//
//    Input, double B, the point density.  0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( a < 1 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_1D_PDF - Fatal error!\n";
    cout << "  Input parameter A < 1.\n";
    exit ( 1 );
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_1D_PDF - Fatal error!\n";
    cout << "  Input parameter B <= 0.0.\n";
    exit ( 1 );
  }

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = pow ( b, a ) * pow ( x, a - 1 ) * exp ( - b * x ) 
      / d_factorial ( a - 1 );
  }

  return pdf;
}
//******************************************************************************

double point_distance_2d_pdf ( double x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POINT_DISTANCE_2D_PDF evaluates the point distance PDF in 2D.
//
//  Discussion:
//
//    It is assumed that a set of points has been generated in 2D
//    according to a Poisson process.  The number of points in a region 
//    of size AREA is a Poisson variate with mean value B * AREA.
//
//    For a point chosen at random, we may now find the nearest
//    Poisson point, the second nearest and so on.  We are interested
//    in the PDF that governs the expected behavior of the distances
//    of rank A = 1, 2, 3, ... with Poisson density B.
//
//  Formula:
//
//    PDF(A,B;X) = 2 * ( B * PI )**A * X**( 2 * A - 1 ) 
//      * EXP ( - B * PI * X * X ) / ( A - 1 )!
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Daniel Zwillinger, editor,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition,
//    CRC Press, 1996, pages 579.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X.
//
//    Input, int A, indicates the degree of nearness of the point.
//    A = 1 means the nearest point, A = 2 the second nearest, and so on.
//    0 < A.
//
//    Input, double B, the point density.  0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  if ( a < 1 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_2D_PDF - Fatal error!\n";
    cout << "  Input parameter A < 1.\n";
    exit ( 1 );
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_2D_PDF - Fatal error!\n";
    cout << "  Input parameter B <= 0.0.\n";
    exit ( 1 );
  }

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 2.0 * pow ( b * PI, a ) * pow ( x, 2 * a - 1 )  
      * exp ( - b * PI * x * x ) / d_factorial ( a - 1 );
  }

  return pdf;
# undef PI
}
//******************************************************************************

double point_distance_3d_pdf ( double x, int a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POINT_DISTANCE_3D_PDF evaluates the point distance PDF in the 3D.
//
//  Discussion:
//
//    It is assumed that a set of points has been generated in 3D
//    according to a Poisson process.  The number of points in a region 
//    of size VOLUME is a Poisson variate with mean value B * VOLUME.
//
//    For a point chosen at random, we may now find the nearest
//    Poisson point, the second nearest and so on.  We are interested
//    in the PDF that governs the expected behavior of the distances
//    of rank A = 1, 2, 3, ... with Poisson density B.
//
//  Formula:
//
//    PDF(A,B;X) = 3 * ( (4/3) * B * PI )**A * X**( 3 * A - 1 ) 
//      * EXP ( - (4/3) * B * PI * X * X * X ) / ( A - 1 )!
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Daniel Zwillinger, editor,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition,
//    CRC Press, 1996, pages 580.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X.
//
//    Input, int A, indicates the degree of nearness of the point.
//    A = 1 means the nearest point, A = 2 the second nearest, and so on.
//    0 < A.
//
//    Input, double B, the Poisson point density.  0.0 < B.
//
//    Output, double PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;

  if ( a < 1 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_3D_PDF - Fatal error!\n";
    cout << "  Input parameter A < 1.\n";
    exit ( 1 );
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "POINT_DISTANCE_3D_PDF - Fatal error!\n";
    cout << "  Input parameter B <= 0.0.\n";
    exit ( 1 );
  }

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 3.0 * pow ( ( ( 4.0 / 3.0 ) * b * PI ), a ) 
      * pow ( x, 3 * a - 1 ) * exp ( - ( 4.0 / 3.0 ) * b * PI * x * x * x ) 
      / d_factorial ( a - 1 );
  }

  return pdf;
# undef PI
}
//**********************************************************************

double poisson_cdf ( int k, double a )

//**********************************************************************
//
//  Purpose:
//
//    POISSON_CDF evaluates the Poisson CDF.
//
//  Definition:
//
//    CDF(K,A) is the probability that the number of events observed
//    in a unit time period will be no greater than K, given that the 
//    expected number of events in a unit time period is A.
//
//  Modified:
//
//    28 January 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int K, the argument of the CDF.
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double POISSON_CDF, the value of the CDF.
//
{
  double cdf;
  int i;
  double last;
  double next;
//
//  Check.
//
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "POISSON_CDF - Fatal error!\n";
    cout << "  A <= 0.\n";
    return 0;
  }
//
//  Special cases.
//
  if ( k < 0 )
  {
    cdf = 0.0;
    return cdf;
  }
//
//  General case.
//
  next = exp ( - a );
  cdf = next;

  for ( i = 1; i <= k; i++ )
  {
    last = next;
    next = last * a / ( double ) i;
    cdf = cdf + next;
  }

  return cdf;
}
//******************************************************************************

int poisson_cdf_inv ( double cdf, double a )

//******************************************************************************
//
//  Purpose:
//
//    POISSON_CDF_INV inverts the Poisson CDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, a value of the CDF.
//    0 <= CDF < 1.
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, int POISSON_CDF_INV, the corresponding argument.
//
{
  int i;
  double last;
  double newval;
  double sum2;
  double sumold;
  int x;
  int xmax = 100;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "POISSON_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }
//
//  Now simply start at X = 0, and find the first value for which
//  CDF(X-1) <= CDF <= CDF(X).
//
  sum2 = 0.0;

  for ( i = 0; i <= xmax; i++ )
  {
    sumold = sum2;

    if ( i == 0 )
    {
      newval = exp ( - a );
      sum2 = newval;
    }
    else
    {
      last = newval;
      newval = last * a / ( double ) ( i );
      sum2 = sum2 + newval;
    }

    if ( sumold <= cdf && cdf <= sum2 )
    {
      x = i;
      return x;
    }
  }

  cout << " \n";
  cout << "POISSON_CDF_INV - Warning!\n";
  cout << "  Exceeded XMAX = " << xmax << "\n";

  x = xmax;

  return x;
}
//******************************************************************************

void poisson_cdf_values ( int *n_data, double *a, int *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    POISSON_CDF_VALUES returns some values of the Poisson CDF.
//
//  Discussion:
//
//    CDF(X)(A) is the probability of at most X successes in unit time,
//    given that the expected mean number of successes is A.
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`DiscreteDistributions`]
//      dist = PoissonDistribution [ a ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    20 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//    Daniel Zwillinger,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition, CRC Press, 1996, pages 653-658.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *A, the parameter of the function.
//
//    Output, int *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 21

  double a_vec[N_MAX] = { 
     0.02E+00,  
     0.10E+00,  
     0.10E+00,  
     0.50E+00,  
     0.50E+00,  
     0.50E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     1.00E+00,  
     2.00E+00,  
     2.00E+00,  
     2.00E+00,  
     2.00E+00,  
     5.00E+00,  
     5.00E+00,  
     5.00E+00,  
     5.00E+00,  
     5.00E+00,  
     5.00E+00,  
     5.00E+00 };

  double fx_vec[N_MAX] = { 
     0.9801986733067553E+00,  
     0.9048374180359596E+00,  
     0.9953211598395555E+00,  
     0.6065306597126334E+00,  
     0.9097959895689501E+00,  
     0.9856123220330293E+00,  
     0.3678794411714423E+00,  
     0.7357588823428846E+00,  
     0.9196986029286058E+00,  
     0.9810118431238462E+00,  
     0.1353352832366127E+00,  
     0.4060058497098381E+00,  
     0.6766764161830635E+00,  
     0.8571234604985470E+00,  
     0.6737946999085467E-02,  
     0.4042768199451280E-01,  
     0.1246520194830811E+00,  
     0.2650259152973617E+00,  
     0.4404932850652124E+00,  
     0.6159606548330631E+00,  
     0.7621834629729387E+00 };

  int x_vec[N_MAX] = { 
     0, 0, 1, 0, 
     1, 2, 0, 1, 
     2, 3, 0, 1, 
     2, 3, 0, 1, 
     2, 3, 4, 5, 
     6 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0.0;
    *x = 0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool poisson_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    POISSON_CHECK checks the parameter of the Poisson PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, bool POISSON_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "POISSON_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  return true;
}
//**********************************************************************

double poisson_mean ( double a )

//**********************************************************************
//
//  Purpose:
//
//    POISSON_MEAN returns the mean of the Poisson PDF.
//
//  Modified:
//
//    01 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double POISSON_MEAN, the mean of the PDF.
//
{
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "POISSON_MEAN - Fatal error!\n" ;
    cout << "  A <= 0.\n";
    return 0;
  }
  
  return a;

}
//**********************************************************************

double poisson_pdf ( int k, double a )

//**********************************************************************
//
//  Purpose:
//
//    POISSON_PDF evaluates the Poisson PDF.
//
//  Discussion:
//
//    PDF(K,A) is the probability that the number of events observed
//    in a unit time period will be K, given the expected number 
//    of events in a unit time.
//
//    The parameter A is the expected number of events per unit time.
//
//    The Poisson PDF is a discrete version of the exponential PDF.
//
//    The time interval between two Poisson events is a random 
//    variable with the exponential PDF.
//
//  Modified:
//
//    01 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int K, the argument of the PDF.
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double POISSON_PDF, the value of the PDF.
//
{
  double pdf;
//
//  Check.
//
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "POISSON_PDF - Fatal error!\n";
    cout << "  A <= 0.\n";
    return (0.0);
  }

  pdf = exp ( -a ) * pow ( a, ( double ) k ) / d_factorial ( k );

  return pdf;
}
//**********************************************************************

int poisson_sample ( double a, int *seed )

//**********************************************************************
//
//  Purpose:
//
//    POISSON_SAMPLE samples the Poisson PDF.
//
//  Modified:
//
//    02 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Input/output, int *SEED, the random number generator seed.
//
//    Output, int POISSON_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  int i;
  int KMAX = 100;
  double last;
  double next;
  double sum;
  double sumold;
//
//  Check.
//
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "POISSON_SAMPLE - Fatal error!\n";
    cout << "  A <= 0.\n";
    return 0;
  }
//
//  Pick a random value of CDF.
//
  cdf = uniform_01_sample ( seed );
//
//  Now simply start at K = 0, and find the first value for which
//  CDF(K-1) <= CDF <= CDF(K).
//
  sum = 0.0;

  for ( i = 0; i <= KMAX; i++ )
  {
    sumold = sum;

    if ( i == 0 )
    {
      next = exp ( -a );
      sum = next;
    }
    else
    {
      last = next;
      next = last * a / ( double ) i;
      sum = sum + next;
    }

    if ( sumold <= cdf && cdf <= sum )
    {
      return i;
    }

  }

  cout << "\n";
  cout << "POISSON_SAMPLE - Warning!\n";
  cout << "  Exceeded KMAX = " << KMAX << "\n";

  return KMAX;
}
//**********************************************************************

double poisson_variance ( double a )

//**********************************************************************
//
//  Purpose:
//
//    POISSON_VARIANCE returns the variance of the Poisson PDF.
//
//  Modified:
//
//    01 February 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double POISSON_VARIANCE, the variance of the PDF.
//
{
//
//  Check.
//
  if ( a <= 0.0 )
  {
    cout << "\n";
    cout << "POISSON_VARIANCE - Fatal error!\n";
    cout << "  A <= 0.\n";
    return (0.0);
  }

  return a;
}
//******************************************************************************

double power_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_CDF evaluates the Power CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B,
//
//    Output, double POWER_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else if ( x <= b )
  {
    cdf = pow ( ( x / b ), a );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double power_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_CDF_INV inverts the Power CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double POWER_CDF_INV, the argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "POWER_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }
  
  if ( cdf == 0.0 )
  {
    x = 0.0;
  }
  else if ( cdf < 1.0 )
  {
    x = b * exp ( log ( cdf ) / a );
  }
  else 
  {
    x = b;
  }

  return x;
}
//******************************************************************************

bool power_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_CHECK checks the parameter of the Power PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, bool POWER_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "POWER_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "POWER_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double power_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_MEAN returns the mean of the Power PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double POWER_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a * b / ( a + 1.0 );

  return mean;
}
//******************************************************************************

double power_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_PDF evaluates the Power PDF.
//
//  Formula:
//
//    PDF(A;X) = (A/B) * (X/B)**(A-1)
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Daniel Zwillinger and Stephen Kokoska,
//    CRC Standard Probability and Statistics Tables and Formulae,
//    Chapman and Hall/CRC, 2000, pages 152-153.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X <= B.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double POWER_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 || b < x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = ( a / b ) * pow ( x / b, a - 1.0 );
  }

  return pdf;
}
//******************************************************************************

double power_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    POWER_SAMPLE samples the Power PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double POWER_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = power_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double power_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    POWER_VARIANCE returns the variance of the Power PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A, 0.0 < B.
//
//    Output, double POWER_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b * a / ( ( a + 1.0 ) * ( a + 1.0 ) * ( a + 2.0 ) );

  return variance;
}
//******************************************************************************

void psi_values ( int *n_data, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    PSI_VALUES returns some values of the Psi or Digamma function.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      PolyGamma[x]
//
//    or
//
//      Polygamma[0,x]
//
//    PSI(X) = d ln ( Gamma ( X ) ) / d X = Gamma'(X) / Gamma(X)
//
//    PSI(1) = -Euler's constant.
//
//    PSI(X+1) = PSI(X) + 1 / X.
//
//  Modified:
//
//    17 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 11

  double fx_vec[N_MAX] = { 
     -0.5772156649015329E+00,  
     -0.4237549404110768E+00,  
     -0.2890398965921883E+00,  
     -0.1691908888667997E+00,  
     -0.6138454458511615E-01,  
      0.3648997397857652E-01,  
      0.1260474527734763E+00,  
      0.2085478748734940E+00,  
      0.2849914332938615E+00,  
      0.3561841611640597E+00,  
      0.4227843350984671E+00 };

  double x_vec[N_MAX] = { 
     1.0E+00,  
     1.1E+00,  
     1.2E+00,  
     1.3E+00,  
     1.4E+00,  
     1.5E+00,  
     1.6E+00,  
     1.7E+00,  
     1.8E+00,  
     1.9E+00,  
     2.0E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double rayleigh_cdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_CDF evaluates the Rayleigh CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//    0.0 <= X.
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, double RAYLEIGH_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < 0.0 )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - exp ( - x * x / ( 2.0 * a * a ) );
  }

  return cdf;
}
//******************************************************************************

double rayleigh_cdf_inv ( double cdf, double a )

//*******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_CDF_INV inverts the Rayleigh CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, double RAYLEIGH_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "RAYLEIGH_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = sqrt ( - 2.0 * a * a * log ( 1.0 - cdf ) );

  return x;
}
//******************************************************************************

void rayleigh_cdf_values ( int *n_data, double *sigma, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_CDF_VALUES returns some values of the Rayleigh CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = RayleighDistribution [ sigma ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    01 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *SIGMA, the shape parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 9

  double fx_vec[N_MAX] = { 
     0.8646647167633873E+00,  
     0.9996645373720975E+00,  
     0.9999999847700203E+00,  
     0.999999999999987E+00,  
     0.8646647167633873E+00,  
     0.3934693402873666E+00,  
     0.1992625970831920E+00,  
     0.1175030974154046E+00,  
     0.7688365361336422E-01 };

  double sigma_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.1000000000000000E+01,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *sigma = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *sigma = sigma_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

bool rayleigh_check ( double a )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_CHECK checks the parameter of the Rayleigh PDF.
//
//  Modified:
//
//    16 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, bool RAYLEIGH_CHECK, is true if the parameter is legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "RAYLEIGH_CHECK - Fatal error!\n";
    cout << "  A <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double rayleigh_mean ( double a )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_MEAN returns the mean of the Rayleigh PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Output, double RAYLEIGH_MEAN, the mean of the PDF.
//
{
# define PI 3.141592653589793

  double mean;

  mean = a * sqrt ( 0.5 * PI );

  return mean;
# undef PI
}
//******************************************************************************

double rayleigh_pdf ( double x, double a )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_PDF evaluates the Rayleigh PDF.
//
//  Formula:
//
//    PDF(A;X) = ( X / A**2 ) * EXP ( - X**2 / ( 2 * A**2 ) )
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X
//
//    Input, double A, the parameter of the PDF.
//    0 < A.
//
//    Output, double RAYLEIGH_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = ( x / ( a * a ) ) * exp ( - x * x / ( 2.0 * a * a ) );
  }

  return pdf;
}
//******************************************************************************

double rayleigh_sample ( double a, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_SAMPLE samples the Rayleigh PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    0.0 < A.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double RAYLEIGH_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = rayleigh_cdf_inv ( cdf, a );

  return x;
}
//******************************************************************************

double rayleigh_variance ( double a )

//******************************************************************************
//
//  Purpose:
//
//    RAYLEIGH_VARIANCE returns the variance of the Rayleigh PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameters of the PDF.
//    0.0 < A.
//
//    Output, double RAYLEIGH_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = 2.0 * a * a * ( 1.0 - 0.25 * PI );

  return variance;
# undef PI
}
//******************************************************************************

double reciprocal_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_CDF evaluates the Reciprocal CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, double RECIPROCAL_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= 0.0 )
  {
    cdf = 0.0;
  }
  else if ( 0.0 < x )
  {
    cdf = log ( a / x ) / log ( a / b );
  }

  return cdf;
}
//******************************************************************************

double reciprocal_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_CDF_INV inverts the Reciprocal CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, double RECIPROCAL_CDF_INV, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "RECIPROCAL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = 0.0;
  }
  else if ( 0.0 < cdf )
  {
    x = pow ( b, cdf ) / pow ( a, ( cdf - 1.0 ) );
  }
 
  return x;
}
//******************************************************************************

bool reciprocal_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_CHECK checks the parameters of the Reciprocal CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, bool RECIPROCAL_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 0.0 )
  {
    cout << " \n";
    cout << "RECIPROCAL_CHECK - Fatal error!\n";
    cout << "  A <= 0.0\n";
    return false;
  }

  if ( b < a )
  {
    cout << " \n";
    cout << "RECIPROCAL_CHECK - Fatal error!\n";
    cout << "  B < A\n";
    return false;
  }

  return true;
}
//******************************************************************************

double reciprocal_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_MEAN returns the mean of the Reciprocal PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, double RECIPROCAL_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = ( a - b ) / log ( a / b );

  return mean;
}
//******************************************************************************

double reciprocal_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_PDF evaluates the Reciprocal PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 1.0 / ( X * LOG ( B / A ) )
//    for 0.0 <= X
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, double RECIPROCAL_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= 0.0 )
  {
    pdf = 0.0;
  }
  else if ( 0.0 < x )
  {
    pdf = 1.0 / ( x * log ( b / a ) );
  }

  return pdf;
}
//******************************************************************************

double reciprocal_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_SAMPLE samples the Reciprocal PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double RECIPROCAL_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = pow ( b, cdf ) / pow ( a, ( cdf - 1.0 ) );

  return x;
}
//******************************************************************************

double reciprocal_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    RECIPROCAL_VARIANCE returns the variance of the Reciprocal PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < A <= B.
//
//    Output, double RECIPROCAL_VARIANCE, the variance of the PDF.
//
{
  double d;
  double variance;

  d = log ( a / b );

  variance = ( a - b )* ( a * ( d - 2.0 ) + b * ( d + 2.0 ) ) 
    / ( 2.0 * d * d );

  return variance;
}
//******************************************************************************

int s_len_trim ( char *s )

//******************************************************************************
//
//  Purpose:
//
//    S_LEN_TRIM returns the length of a string to the last nonblank.
//
//  Modified:
//
//    26 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a pointer to a string.
//
//    Output, int S_LEN_TRIM, the length of the string to the last nonblank.
//    If S_LEN_TRIM is 0, then the string is entirely blank.
//
{
  int n;
  char* t;

  n = strlen ( s );
  t = s + strlen ( s ) - 1;

  while ( 0 < n ) 
  {
    if ( *t != ' ' )
    {
      return n;
    }
    t--;
    n--;
  }

  return n;
}
//******************************************************************************

double sech ( double x )

//******************************************************************************
//
//  Purpose:
//
//    SECH returns the hyperbolic secant.
//
//  Definition:
//
//    SECH ( X ) = 1.0 / COSH ( X ) = 2.0 / ( EXP ( X ) + EXP ( - X ) )
//
//  Discussion:
//
//    SECH is not a built-in function in FORTRAN, and occasionally it
//    is handier, or more concise, to be able to refer to it directly
//    rather than through its definition in terms of the sine function.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument.
//
//    Output, double SECH, the hyperbolic secant of X.
//
{
  double value;

  value = 1.0 / cosh ( x );

  return value;
}
//******************************************************************************

double sech_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SECH_CDF evaluates the Hyperbolic Secant CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, double SECH_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;
  double y;

  y = ( x - a ) / b;

  cdf = 2.0 * atan ( exp ( y ) ) / PI;

  return cdf;
# undef PI
}
//******************************************************************************

double sech_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SECH_CDF_INV inverts the Hyperbolic Secant CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SECH_CDF_INV, the corresponding argument of the CDF.
//
{
# define PI 3.141592653589793

  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "SECH_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = - d_huge (  );
  }
  else if ( cdf < 1.0 )
  {
    x = a + b * log ( tan ( 0.5 * PI * cdf ) );
  }
  else if ( 1.0 == cdf )
  {
    x = d_huge ( );
  }

  return x;
# undef PI
}
//*******************************************************************************

bool sech_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    SECH_CHECK checks the parameters of the Hyperbolic Secant CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, bool SECH_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "SECH_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  return true;
}
//******************************************************************************

double sech_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SECH_MEAN returns the mean of the Hyperbolic Secant PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SECH_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double sech_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SECH_PDF evaluates the Hypebolic Secant PDF.
//
//  Formula:
//
//    PDF(A,B;X) = sech ( ( X - A ) / B ) / ( PI * B )
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SECH_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  y = ( x - a ) / b;

  pdf = sech ( y ) / ( PI * b );

  return pdf;
# undef PI
}
//******************************************************************************

double sech_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    SECH_SAMPLE samples the Hyperbolic Secant PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double SECH_SAMPLE, a sample of the PDF.
//
{
# define PI 3.141592653589793

  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = a + b * log ( tan ( 0.5 * PI * cdf ) );

  return x;
# undef PI
}
//******************************************************************************

double sech_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SECH_VARIANCE returns the variance of the Hyperbolic Secant PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SECH_VARIANCE, the variance of the PDF.
//
{
# define PI 3.141592653589793

  double variance;

  variance = 0.25 * PI * PI * b * b;

  return variance;
# undef PI
}
//******************************************************************************

double semicircular_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_CDF evaluates the Semicircular CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, double SEMICIRCULAR_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double cdf;
  double y;

  if ( x <= a - b )
  {
    cdf = 0.0;
  }
  else if ( x <= a + b )
  {
    y = ( x - a ) / b;

    cdf = 0.5 + ( y * sqrt ( 1.0 - y * y ) + asin ( y ) ) / PI;
  }
  else if ( a + b < x )
  {
    cdf = 1.0;
  }

  return cdf;
# undef PI
}
//******************************************************************************

double semicircular_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_CDF_INV inverts the Semicircular CDF.
//
//  Discussion:
//
//    A simple bisection method is used on the interval [ A - B, A + B ].
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SEMICIRCULAR_CDF_INV, the corresponding argument
//    of the CDF.
//
{
  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "SEMICIRCULAR_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = a - b;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = a + b;
    return x;
  }

  x1 = a - b;
  cdf1 = 0.0;

  x2 = a + b;
  cdf2 = 1.0;
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = semicircular_cdf ( x3, a, b );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "SEMICIRCULAR_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      exit ( 1 );
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }

  }

  return x;
}
//*******************************************************************************

bool semicircular_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_CHECK checks the parameters of the Semicircular CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameter of the PDF.
//    0.0 < B.
//
//    Output, bool SEMICIRCULAR_CHECK, is true if the parameters are legal.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "SEMICIRCULAR_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  return true;
}
//******************************************************************************

double semicircular_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_MEAN returns the mean of the Semicircular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SEMICIRCULAR_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double semicircular_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_PDF evaluates the Semicircular PDF.
//
//  Formula:
//
//    PDF(A,B;X) = ( 2 / ( B * PI ) ) * SQRT ( 1 - ( ( X - A ) / B )**2 )
//    for A - B <= X <= A + B
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SEMICIRCULAR_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  if ( x < a - b )
  {
    pdf = 0.0;
  }
  else if ( x <= a + b )
  {
    y = ( x - a ) / b;

    pdf = 2.0 / ( b * PI ) * sqrt ( 1.0 - y * y );
  }
  else if ( a + b < x )
  {
    pdf = 0.0;
  }

  return pdf;
# undef PI
}
//******************************************************************************

double semicircular_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_SAMPLE samples the Semicircular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double SEMICIRCULAR_SAMPLE, a sample of the PDF.
//
{
# define PI 3.141592653589793

  double angle;
  double radius;
  double x;

  radius = d_uniform_01 ( seed );
  radius = b * sqrt ( radius );
  angle = PI * d_uniform_01 ( seed );
  x = a + radius * cos ( angle );

  return x;
# undef PI
}
//******************************************************************************

double semicircular_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    SEMICIRCULAR_VARIANCE returns the variance of the Semicircular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 < B.
//
//    Output, double SEMICIRCULAR_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = b * b / 4.0;

  return variance;
}
//******************************************************************************

double sin_power_int ( double a, double b, int n )

//******************************************************************************
//
//  Purpose:
//
//    SIN_POWER_INT evaluates the sine power integral.
//
//  Discussion:
//
//    The function is defined by
//
//      SIN_POWER_INT(A,B,N) = Integral ( A <= T <= B ) ( sin ( t ))^n dt
//
//    The algorithm uses the following fact:
//
//      Integral sin^n ( t ) = (1/n) * (
//        sin^(n-1)(t) * cos(t) + ( n-1 ) * Integral sin^(n-2) ( t ) dt )
//
//  Modified:
//
//    02 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters
//
//    Input, double A, B, the limits of integration.
//
//    Input, integer N, the power of the sine function.
//
//    Output, double SIN_POWER_INT, the value of the integral.
//
{
  double ca;
  double cb;
  int m;
  int mlo;
  double sa;
  double sb;
  double value;
//
  if ( n < 0 )
  {
    cout << "\n";
    cout << "SIN_POWER_INT - Fatal error!\n";
    cout << "  Power N < 0.\n";
    value = 0.0;
    exit ( 1 );
  }

  sa = sin ( a );
  sb = sin ( b );
  ca = cos ( a );
  cb = cos ( b );

  if ( ( n % 2 ) == 0 )
  {
    value = b - a;
    mlo = 2;
  }
  else
  {
    value = ca - cb;
    mlo = 3;
  }

  for ( m = mlo; m <= n; m = m + 2 )
  {
    value = ( ( double ) ( m - 1 ) * value 
      + pow ( sa, (m-1) ) * ca - pow ( sb, (m-1) ) * cb ) 
      / ( double ) ( m );
  }

  return value;
}
//******************************************************************************

double student_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_CDF evaluates the central Student T CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double STUDENT_CDF, the value of the CDF.
//
{
  double a2;
  double b2;
  double c2;
  double cdf;
  double y;

  y = ( x - a ) / b;

  a2 = 0.5 * c;
  b2 = 0.5;
  c2 = c / ( c + y * y );

  if ( y <= 0.0 )
  {
    cdf = 0.5 * beta_inc ( a2, b2, c2 );
  }
  else
  {
    cdf = 1.0 - 0.5 * beta_inc ( a2, b2, c2 );
  }

  return cdf;
}
//******************************************************************************

void student_cdf_values ( int *n_data, int *a, double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_CDF_VALUES returns some values of the Student CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = StudentTDistribution [ df ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    21 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *A, the parameter of the function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 13

  int a_vec[N_MAX] = { 
    1, 2, 3, 4, 
    5, 2, 5, 2, 
    5, 2, 3, 4, 
    5 };

  double fx_vec[N_MAX] = { 
     0.6000231200328521E+00,  
     0.6001080279134390E+00,  
     0.6001150934648930E+00,  
     0.6000995134721354E+00,  
     0.5999341989834830E+00,  
     0.7498859393137811E+00,  
     0.7500879487671045E+00,  
     0.9500004222186464E+00,  
     0.9499969138365968E+00,  
     0.9900012348724744E+00,  
     0.9900017619355059E+00,  
     0.9900004567580596E+00,  
     0.9900007637471291E+00 };

  double x_vec[N_MAX] = { 
     0.325E+00,  
     0.289E+00,  
     0.277E+00,  
     0.271E+00,  
     0.267E+00,  
     0.816E+00,  
     0.727E+00,  
     2.920E+00,  
     2.015E+00,  
     6.965E+00,  
     4.541E+00,  
     3.747E+00,  
     3.365E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *a = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *a = a_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool student_check ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    STUDENT_CHECK checks the parameter of the central Student T CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
{
  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "STUDENT_CHECK - Fatal error!\n";
    cout << "  B must be greater than 0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << " \n";
    cout << "STUDENT_CHECK - Fatal error!\n";
    cout << "  C must be greater than 0.\n";
    return false;
  }
  
  return true;
}
//******************************************************************************

double student_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_MEAN returns the mean of the central Student T PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameter of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double STUDENT_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;
  
  return mean;
}
//******************************************************************************

double student_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_PDF evaluates the central Student T PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = Gamma ( (C+1)/2 ) /
//      ( Gamma ( C / 2 ) * Sqrt ( PI * C ) 
//      * ( 1 + ((X-A)/B)**2/C )**(C + 1/2 ) )
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Output, double STUDENT_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double pdf;
  double y;

  y = ( x - a ) / b;

  pdf = gamma ( 0.5 * ( c + 1.0 ) ) / ( sqrt ( PI * c ) 
    * gamma ( 0.5 * c ) 
    * sqrt ( pow ( ( 1.0 + y * y / c ), ( 2 * c + 1.0 ) ) ) );
  
  return pdf;
# undef PI
}
//******************************************************************************

double student_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_SAMPLE samples the central Student T PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0 < B,
//    0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double STUDENT_SAMPLE, a sample of the PDF.
//
{
  double a2;
  double b2;
  double x;
  double x2;
  double x3;

  if ( c < 3.0 )
  {
    cout << " \n";
    cout << "STUDENT_SAMPLE - Fatal error!\n";
    cout << "  Sampling fails for C < 3.\n";
    exit ( 1 );
  }

  a2 = 0.0;
  b2 = c  / ( c - 2.0 );

  x2 = normal_sample ( a2, b2, seed );

  x3 = chi_square_sample ( c, seed );
  x3 = x3 * c / ( c - 2.0 );

  x = a + b * x2 * sqrt ( c ) / x3;

  return x;
}
//******************************************************************************

double student_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_VARIANCE returns the variance of the central Student T PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameter of the PDF.
//    0 < B,
//    0 < C.
//    However, the variance is not defined unless 2 < C
//
//    Output, double STUDENT_VARIANCE, the variance of the PDF.
//
{
  double variance;

  if ( c < 3.0 )
  {
    cout << " \n";
    cout << "STUDENT_VARIANCE - Fatal error!\n";
    cout << "  Variance not defined for C <= 2.\n";
    exit ( 1 );
  }

  variance = b * b * c / ( c - 2.0 );
  
  return variance;
}
//******************************************************************************

double student_noncentral_cdf ( double x, int idf, double d )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_NONCENTRAL_CDF evaluates the noncentral Student T CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Reference:
//
//    Algorithm AS 5,
//    Applied Statistics,
//    Volume 17, 1968, page 193.
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, int IDF, the number of degrees of freedom.
//
//    Input, double D, the noncentrality parameter.
//
//    Output, double STUDENT_NONCENTRAL_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double a;
  int a_max = 100;
  double ak;
  double b;
  double cdf;
  double cdf2;
  double drb;
  double emin = 12.5;
  double f;
  double fk;
  double fmkm1;
  double fmkm2;
  int k;
  double sum2;
  double temp;

  f = ( double ) idf;

  if ( idf == 1 )
  {
    a = x / sqrt ( f );
    b = f / ( f + x * x );
    drb = d * sqrt ( b );

    cdf2 = normal_01_cdf ( drb );
    cdf = 1.0 - cdf2 + 2.0 * tfn ( drb, a );
  }
  else if ( idf <= a_max )
  {
    a = x / sqrt ( f );
    b = f / ( f + x * x );
    drb = d * sqrt ( b );
    sum2 = 0.0;

    fmkm2 = 0.0;
    if ( fabs ( drb ) < emin )
    {
      cdf2 = normal_01_cdf ( a * drb );
      fmkm2 = a * sqrt ( b ) * exp ( - 0.5 * drb * drb ) * cdf2 
        / sqrt ( 2.0 * PI );
    }

    fmkm1 = b * d * a * fmkm2;
    if ( fabs ( d ) < emin )
    {
      fmkm1 = fmkm1 + 0.5 * b * a * exp ( - 0.5 * d * d ) / PI;
    }

    if ( idf % 2 == 0 )
    {
      sum2 = fmkm2;
    }
    else
    {
      sum2 = fmkm1;
    }

    ak = 1.0;

    for ( k = 2; k <= idf - 2; k = k + 2 )
    {
      fk = ( double ) ( k );

      fmkm2 = b * ( d * a * ak * fmkm1 + fmkm2 ) * ( fk - 1.0 ) / fk;

      ak = 1.0 / ( ak * ( fk - 1.0 ) );
      fmkm1 = b * ( d * a * ak * fmkm2 + fmkm1 ) * fk / ( fk + 1.0 );

      if ( idf % 2 == 0 )
      {
        sum2 = sum2 + fmkm2;
      }
      else
      {
        sum2 = sum2 + fmkm1;
      }

      ak = 1.0 / ( ak * fk );

    }

    if ( idf % 2 == 0 )
    {
      cdf2 = normal_01_cdf ( d );
      cdf = 1.0 - cdf2 + sum2 * sqrt ( 2.0 * PI );
    }
    else
    {
      cdf2 = normal_01_cdf ( drb );
      cdf = 1.0 - cdf2 + 2.0 * ( sum2 + tfn ( drb, a ) );
    }
  }
//
//  Normal approximation.
//
  else
  {
    a = sqrt ( 0.5 * f ) * exp ( gamma_log ( 0.5 * ( f - 1.0 ) ) 
      - gamma_log ( 0.5 * f ) ) * d;

    temp = ( x - a ) / sqrt ( f * ( 1.0 + d * d ) / ( f - 2.0 ) - a * a );

    cdf2 = normal_01_cdf ( temp );
    cdf = cdf2;
  }

  return cdf;
# undef PI
}
//******************************************************************************

void student_noncentral_cdf_values ( int *n_data, int *df, double *lambda, 
  double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    STUDENT_NONCENTRAL_CDF_VALUES returns values of the noncentral Student CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = NoncentralStudentTDistribution [ df, lambda ]
//      CDF [ dist, x ]
//
//    Mathematica seems to have some difficulty computing this function
//    to the desired number of digits.
//
//  Modified:
//
//    01 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, int *DF, double *LAMBDA, the parameters of the
//    function.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 30

  int df_vec[N_MAX] = { 
     1,  2,  3, 
     1,  2,  3, 
     1,  2,  3, 
     1,  2,  3, 
     1,  2,  3, 
    15, 20, 25, 
     1,  2,  3, 
    10, 10, 10, 
    10, 10, 10, 
    10, 10, 10 };

  double fx_vec[N_MAX] = { 
     0.8975836176504333E+00,  
     0.9522670169E+00,  
     0.9711655571887813E+00,  
     0.8231218864E+00,  
     0.9049021510E+00,  
     0.9363471834E+00,  
     0.7301025986E+00,  
     0.8335594263E+00,  
     0.8774010255E+00,  
     0.5248571617E+00,  
     0.6293856597E+00,  
     0.6800271741E+00,  
     0.20590131975E+00,  
     0.2112148916E+00,  
     0.2074730718E+00,  
     0.9981130072E+00,  
     0.9994873850E+00,  
     0.9998391562E+00,  
     0.168610566972E+00,  
     0.16967950985E+00,  
     0.1701041003E+00,  
     0.9247683363E+00,  
     0.7483139269E+00,  
     0.4659802096E+00,  
     0.9761872541E+00,  
     0.8979689357E+00,  
     0.7181904627E+00,  
     0.9923658945E+00,  
     0.9610341649E+00,  
     0.8688007350E+00 };

  double lambda_vec[N_MAX] = { 
     0.0E+00,  
     0.0E+00,  
     0.0E+00,  
     0.5E+00,  
     0.5E+00,  
     0.5E+00,  
     1.0E+00,  
     1.0E+00,  
     1.0E+00,  
     2.0E+00,  
     2.0E+00,  
     2.0E+00,  
     4.0E+00,  
     4.0E+00,  
     4.0E+00,  
     7.0E+00,  
     7.0E+00,  
     7.0E+00,  
     1.0E+00,  
     1.0E+00,  
     1.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00,  
     2.0E+00,  
     3.0E+00,  
     4.0E+00 };

  double x_vec[N_MAX] = { 
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
      3.00E+00,  
     15.00E+00,  
     15.00E+00,  
     15.00E+00,  
      0.05E+00,  
      0.05E+00,  
      0.05E+00,  
      4.00E+00,  
      4.00E+00,  
      4.00E+00,  
      5.00E+00,  
      5.00E+00,  
      5.00E+00,  
      6.00E+00,  
      6.00E+00,  
      6.00E+00 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *df = 0;
    *lambda = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *df = df_vec[*n_data-1];
    *lambda = lambda_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//******************************************************************************

double tfn ( double x, double fx )

//******************************************************************************
//
//  Purpose:
//
//    TFN calculates the T function of Owen.
//
//  Discussion:
//
//    Owen's T function is useful for computation of the bivariate normal
//    distribution and the distribution of a skewed normal distribution.
//
//    Although it was originally formulated in terms of the bivariate
//    normal function, the function can be defined more directly as
//
//      T(H,A) = 1 / ( 2 * PI ) * 
//        Integral ( 0 <= X <= A ) e^(H^2*(1+X^2)/2) / (1+X^2) dX
//
//  Modified:
//
//    18 October 2004
//
//  Reference:
//
//    D B Owen,
//    Tables for computing the bivariate normal distribution,
//    Annals of Mathematical Statistics,
//    Volume 27, pages 1075-1090, 1956.
//
//    Algorithm AS 76,
//    Applied Statistics,
//    Volume 23, Number 3, 1974.
//
//  Parameters:
//
//    Input, double X, FX, the arguments of the T function.
//
//    Output, double TFN, the value of the T function.
//
{
# define NGAUSS 5

  double fxs;
  int i;
  double r1;
  double r2;
  double rt;
  double tp = 0.159155;
  double tv1 = 1.0E-35;
  double tv2 = 15.0;
  double tv3 = 15.0;
  double tv4 = 1.0E-05;
  double u[NGAUSS] = {
    0.0744372, 
    0.2166977, 
    0.3397048, 
    0.4325317, 
    0.4869533 };
  double value;
  double weight[NGAUSS] = {
    0.1477621, 
    0.1346334, 
    0.1095432, 
    0.0747257, 
    0.0333357 };
  double x1;
  double x2;
  double xs;
//
//  Test for X near zero.
//
  if ( fabs ( x ) < tv1 )
  {
    value = tp * atan ( fx );
  }
//
//  Test for large values of abs(X).
//
  else if ( tv2 < fabs ( x ) )
  {
    value = 0.0;
  }
//
//  Test for FX near zero.
//
  else if ( fabs ( fx ) < tv1 )
  {
    value = 0.0;
  }
//
//  Test whether abs(FX) is so large that it must be truncated.
//
  else 
  {
    xs = - 0.5 * x * x;
    x2 = fx;
    fxs = fx * fx;
//
//  Computation of truncation point by Newton iteration.
//
    if ( tv3 <= log ( 1.0 + fxs ) - xs * fxs )
    {
      x1 = 0.5 * fx;
      fxs = 0.25 * fxs;

      for ( ; ; )
      {
        rt = fxs + 1.0;
        x2 = x1 + ( xs * fxs + tv3 - log ( rt ) ) 
          / ( 2.0 * x1 * ( 1.0 / rt - xs ) );
        fxs = x2 * x2;

        if ( fabs ( x2 - x1 ) < tv4 )
        {
          break;
        }
        x1 = x2;
      }
    }
//
//  Gaussian quadrature.
//
    rt = 0.0;
    for ( i = 0; i < NGAUSS; i++ )
    {
      r1 = 1.0 + fxs * pow ( ( 0.5 + u[i] ), 2 );
      r2 = 1.0 + fxs * pow ( ( 0.5 - u[i] ), 2 );
      rt = rt + weight[i] * ( exp ( xs * r1 ) / r1 + exp ( xs * r2 ) / r2 );
    }
    value = rt * x2 * tp;
  }

  return value;
# undef NGAUSS
}
//**********************************************************************

void timestamp ( void )

//**********************************************************************
//
//  Purpose:
//
//    TIMESTAMP prints the current YMDHMS date as a time stamp.
//
//  Example:
//
//    May 31 2001 09:45:54 AM
//
//  Modified:
//
//    24 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    None
//
{
#define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  cout << time_buffer << "\n";

  return;
#undef TIME_SIZE
}
//******************************************************************************

double triangle_cdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_CDF evaluates the Triangle CDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, double TRIANGLE_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else if ( x <= b )
  {
    if ( a == b )
    {
      cdf = 0.0;
    }
    else
    {
      cdf = ( x - a ) * ( x - a ) / ( b - a ) / ( c - a );
    }
  }
  else if ( x <= c )
  {
    cdf = ( b - a ) / ( c - a ) 
        + ( 2.0 * c - b - x ) * ( x - b ) / ( c - b ) / ( c - a );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double triangle_cdf_inv ( double cdf, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_CDF_INV inverts the Triangle CDF.
//
//  Modified:
//
//    17 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, double TRIANGLE_X, the corresponding argument.
//
{
  double cdf_mid;
  double d;
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "TRIANGLE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  d = 2.0 / ( c - a );
  cdf_mid = 0.5 * d * ( b - a );

  if ( cdf <= cdf_mid )
  {
    x = a + sqrt ( cdf * ( b - a ) * ( c - a ) );
  }
  else
  {
    x = c - sqrt ( ( c - b ) * ( ( c - b ) - ( cdf - cdf_mid ) * ( c - a ) ) );
  }

  return x;
}
//******************************************************************************

bool triangle_check ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_CHECK checks the parameters of the Triangle CDF.
//
//  Modified:
//
//    17 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, bool TRIANGLE_CHECK, is true if the parameters are legal.
//
{
  if ( b < a )
  {
    cout << "\n";
    cout << "TRIANGLE_CHECK - Fatal error!\n";
    cout << "  B < A.\n";
    return false;
  }

  if ( c < b )
  {
    cout << "\n";
    cout << "TRIANGLE_CHECK - Fatal error!\n";
    cout << "  C < B.\n";
    return false;
  }

  if ( a == c )
  {
    cout << "\n";
    cout << "TRIANGLE_CHECK - Fatal error!\n";
    cout << "  A == C.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double triangle_mean ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_MEAN returns the mean of the Triangle PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, double TRIANGLE_MEAN, the mean of the discrete uniform PDF.
//
{
  double mean;

  mean = a + ( c + b - 2.0 * a ) / 3.0;

  return mean;
}
//******************************************************************************

double triangle_pdf ( double x, double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_PDF evaluates the Triangle PDF.
//
//  Discussion:
//
//    Given points A <= B <= C, the probability is 0 to the left of A,
//    rises linearly to a maximum of 2/(C-A) at B, drops linearly to zero
//    at C, and is zero for all values greater than C.
//
//  Formula:
//
//    PDF(A,B,C;X)
//      = 2 * ( X - A ) / ( B - A ) / ( C - A ) for A <= X <= B
//      = 2 * ( C - X ) / ( C - B ) / ( C - A ) for B <= X <= C.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, double TRIANGLE_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else if ( x <= b )
  {
    if ( a == b )
    {
      pdf = 0.0;
    }
    else
    {
      pdf = 2.0 * ( x - a ) / ( b - a ) / ( c - a );
    }
  }
  else if ( x <= c )
  {
    if ( b == c )
    {
      pdf = 0.0;
    }
    else
    {
      pdf = 2.0 * ( c - x ) / ( c - b ) / ( c - a );
    }
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
}
//******************************************************************************

double triangle_sample ( double a, double b, double c, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_SAMPLE samples the Triangle PDF.
//
//  Modified:
//
//    17 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = triangle_cdf_inv ( cdf, a, b, c );

  return x;
}
//******************************************************************************

double triangle_variance ( double a, double b, double c )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGLE_VARIANCE returns the variance of the Triangle PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    A <= B <= C and A < C.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double h;
  double variance;

  variance = ( ( c - a ) * ( c - a ) 
             - ( c - a ) * ( b - a )
             + ( b - a ) * ( b - a ) ) / 18.0;

  return variance;
}
//******************************************************************************

double triangular_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_CDF evaluates the Triangular CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double TRIANGULAR_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x <= a )
  {
    cdf = 0.0;
  }
  else if ( x <= 0.5 * ( a + b ) )
  {
    cdf = 2.0 * ( x * x - 2.0 * a * x + a * a ) / pow ( ( b - a ), 2 );
  }
  else if ( x <= b )
  {
    cdf = 0.5 + ( - 2.0 * x * x + 4.0 * b * x + 0.5 * a * a 
      - a * b - 1.5 * b * b ) / pow ( ( b - a ), 2 );
  }
  else
  {
    cdf = 1.0;
  }

  return cdf;
}
//******************************************************************************

double triangular_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_CDF_INV inverts the Triangular CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double TRIANGULAR_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "TRIANGULAR_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf <= 0.5 )
  {
    x = a + 0.5 * ( b - a ) * sqrt ( 2.0 * cdf );
  }
  else
  {
    x = b - 0.5 * ( b - a ) * sqrt ( 2.0 * ( 1.0 - cdf ) );
  }

  return x;
}
//*******************************************************************************

bool triangular_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_CHECK checks the parameters of the Triangular CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, bool TRIANGULAR_CHECK, is true if the parameters are legal.
//
{
  if ( b <= a )
  {
    cout << " \n";
    cout << "TRIANGULAR_CHECK - Fatal error!\n";
    cout << "  B <= A.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double triangular_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_MEAN returns the mean of the Triangular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double TRIANGULAR_MEAN, the mean of the discrete PDF.
//
{
  double mean;

  mean = 0.5 * ( a + b );

  return mean;
}
//******************************************************************************

double triangular_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_PDF evaluates the Triangular PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 4 * ( X - A ) / ( B - A )**2 for A <= X <= (A+B)/2
//                = 4 * ( B - X ) / ( B - A )**2 for (A+B)/2 <= X <= B.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double TRIANGULAR_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x <= a )
  {
    pdf = 0.0;
  }
  else if ( x <= 0.5 * ( a + b ) )
  {
    pdf = 4.0 * ( x - a ) / ( b - a ) / ( b - a );
  }
  else if ( x <= b )
  {
    pdf = 4.0 * ( b - x ) / ( b - a ) / ( b - a );
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
}
//******************************************************************************

double triangular_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_SAMPLE samples the Triangular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double TRIANGULAR_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = triangular_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double triangular_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    TRIANGULAR_VARIANCE returns the variance of the Triangular PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double TRIANGULAR_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( b - a ) * ( b - a ) / 24.0;

  return variance;
}
//******************************************************************************

double trigamma ( double x )

//******************************************************************************
//
//  Purpose:
//
//    TRIGAMMA calculates the TriGamma function.
//
//  Discussion:
//
//    TriGamma(x) = d**2 log ( Gamma ( x ) ) / dx**2.
//
//  Modified:
//
//    03 January 2000
//
//  Reference:
//
//    B Schneider,
//    Trigamma Function,
//    Algorithm AS 121,
//    Applied Statistics, 
//    Volume 27, Number 1, page 97-99, 1978.
//
//  Parameters:
//
//    Input, double X, the argument of the trigamma function.
//    0 < X.
//
//    Output, double TRIGAMMA, the value of the 
//    trigamma function at X.
//
{
  double a = 0.0001;
  double b = 5.0;
  double b2 =   1.0 / 6.0;
  double b4 = - 1.0 / 30.0;
  double b6 =   1.0 / 42.0;
  double b8 = - 1.0 / 30.0;
  double value;
  double y;
  double z;
//
//  1): If X is not positive, fail.
//
  if ( x <= 0.0 )
  {
    value = 0.0;
    cout << " \n";
    cout << "TRIGAMMA - Fatal error!\n";
    cout << "  X <= 0.\n";
    exit ( 1 );
  }
//
//  2): If X is smaller than A, use a small value approximation.
//
  else if ( x <= a )
  {
    value = 1.0 / x / x;
  }
//
//  3): Otherwise, increase the argument to B <= ( X + I ).
//
  else
  {
    z = x;
    value = 0.0;

    while ( z < b )
    {
      value = value + 1.0 / z / z;
      z = z + 1.0;
    }
//
//  ...and then apply an asymptotic formula.
//
    y = 1.0 / z / z;

    value = value + 0.5 * 
            y + ( 1.0 
          + y * ( b2 
          + y * ( b4 
          + y * ( b6 
          + y *   b8 )))) / z;
  }

  return value;
}
//******************************************************************************

double uniform_01_cdf ( double x )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_CDF evaluates the Uniform 01 CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Output, double UNIFORM_01_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < 0.0 )
  {
    cdf = 0.0;
  }
  else if ( 1.0 < x )
  {
    cdf = 1.0;
  }
  else
  {
    cdf = x;
  }

  return cdf;
}
//******************************************************************************

double uniform_01_cdf_inv ( double cdf )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_CDF_INV inverts the Uniform 01 CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Output, double UNIFORM_01_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "UNIFORM_01_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = cdf;

  return x;
}
//******************************************************************************

double uniform_01_mean ( void )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_MEAN returns the mean of the Uniform 01 PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double UNIFORM_01_MEAN, the mean of the discrete uniform PDF.
//
{
  double mean;

  mean = 0.5;

  return mean;
}
//*******************************************************************************

double uniform_01_pdf ( double x )

//*******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_PDF evaluates the Uniform 01 PDF.
//
//  Formula:
//
//    PDF(X) = 1 for 0 <= X <= 1
//           = 0 otherwise
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    0.0 <= X <= 1.0.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0.0 || 1.0 < x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 1.0;
  }

  return pdf;
}
//**********************************************************************

double uniform_01_sample ( int *seed )

//**********************************************************************
//
//  Purpose:
//
//    UNIFORM_01_SAMPLE is a random number generator.
//
//  Formula:
//
//    SEED = SEED * (7**5) mod (2**31 - 1)
//    UNIFORM_01_SAMPLE = SEED * / ( 2**31 - 1 )
//
//  Parameters:
//
//    Input/output, int *SEED, the integer "seed" used to generate
//    the output random number, and updated in preparation for the
//    next one.  *SEED should not be zero.
//
//    Output, double UNIFORM_01_SAMPLE, a random value between 0 and 1.
//
//  Local Parameters:
//
//    Local, int IA = 7**5.
//
//    Local, int IB = 2**15.
//
//    Local, int IB16 = 2**16.
//
//    Local, int IP = 2**31 - 1.
//
{
  static int ia = 16807;
  static int ib15 = 32768;
  static int ib16 = 65536;
  static int ip = 2147483647;
  int iprhi;
  int ixhi;
  int k;
  int leftlo;
  int loxa;
  double temp;
//
//  Don't let SEED be 0.
//
  if ( *seed == 0 )
  {
    *seed = ip;
  }
//
//  Get the 15 high order bits of SEED.
//
  ixhi = *seed / ib16;
//
//  Get the 16 low bits of SEED and form the low product.
//
  loxa = ( *seed - ixhi * ib16 ) * ia;
//
//  Get the 15 high order bits of the low product.
//
  leftlo = loxa / ib16;
//
//  Form the 31 highest bits of the full product.
//
  iprhi = ixhi * ia + leftlo;
//
//  Get overflow past the 31st bit of full product.
//
  k = iprhi / ib15;
//
//  Assemble all the parts and presubtract IP.  The parentheses are essential.
//
  *seed = ( ( ( loxa - leftlo * ib16 ) - ip ) +
    ( iprhi - k * ib15 ) * ib16 ) + k;
//
//  Add IP back in if necessary.
//
  if ( *seed < 0 )
  {
    *seed = *seed + ip;
  }
//
//  Multiply by 1 / (2**31-1).
//
  temp = ( ( double ) *seed ) * 4.656612875E-10;

  return ( temp );
}
//******************************************************************************

double uniform_01_variance ( void )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_VARIANCE returns the variance of the Uniform 01 PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double UNIFORM_01_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 1.0 / 12.0;

  return variance;
}
//******************************************************************************

double *uniform_01_order_sample ( int n, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_01_ORDER_SAMPLE samples the Uniform 01 Order PDF.
//
//  Discussion:
//
//    In effect, this routine simply generates N samples of the
//    Uniform 01 PDF; but it generates them in order.  (Actually,
//    it generates them in descending order, but stores them in
//    the array in ascending order).  This saves the work of
//    sorting the results.  Moreover, if the order statistics
//    for another PDF are desired, and the inverse CDF is available,
//    then the desired values may be generated, presorted, by
//    calling this routine and using the results as input to the
//    inverse CDF routine.
//
//  Modified:
//
//    27 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 168.
//
//  Parameters:
//
//    Input, int N, the number of elements in the sample.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double UNIFORM_01_ORDER_SAMPLE[N], N samples of the Uniform 01 PDF, in
//    ascending order.
//
{
  int i;
  double u;
  double v;
  double *x;

  x = new double[n];

  v = 1.0;

  for ( i = n-1; 0 <= i; i-- )
  {
    u = d_uniform_01 ( seed );
    v = v * pow ( u, 1.0 / ( double ) ( i + 1 ) );
    x[i] = v;
  }

  return x;
}
//******************************************************************************

double uniform_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_CDF evaluates the Uniform CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double UNIFORM_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < a )
  {
    cdf = 0.0;
  }
  else if ( b < x )
  {
    cdf = 1.0;
  }
  else
  {
    cdf = ( x - a ) / ( b - a );
  }

  return cdf;
}
//******************************************************************************

double uniform_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_CDF_INV inverts the Uniform CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double UNIFORM_CDF_INV, the corresponding argument.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "UNIFORM_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a + ( b - a ) * cdf;

  return x;
}
//******************************************************************************

bool uniform_check ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_CHECK checks the parameters of the Uniform CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, bool UNIFORM_CHECK, is true if the parameters are legal.
//
{
  if ( b <= a )
  {
    cout << " \n";
    cout << "UNIFORM_CHECK - Fatal error!\n";
    cout << "  B <= A.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double uniform_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_MEAN returns the mean of the Uniform PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double UNIFORM_MEAN, the mean of the discrete uniform PDF.
//
{
  double mean;

  mean = 0.5 * ( a + b );

  return mean;
}
//******************************************************************************

double uniform_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_PDF evaluates the Uniform PDF.
//
//  Discussion:
//
//    The Uniform PDF is also known as the "Rectangular" or "de Moivre" PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 1 / ( B - A ) for A <= X <= B
//               = 0 otherwise
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double UNIFORM_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < a || b < x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 1.0 / ( b - a );
  }

  return pdf;
}
//******************************************************************************

double uniform_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_SAMPLE samples the Uniform PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double UNIFORM_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = uniform_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double uniform_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_VARIANCE returns the variance of the Uniform PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    A < B.
//
//    Output, double UNIFORM_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( b - a ) * ( b - a ) / 12.0;

  return variance;
}
//******************************************************************************

double uniform_discrete_cdf ( int x, int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_CDF evaluates the Uniform Discrete CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the CDF.
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, double UNIFORM_DISCRETE_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < a )
  {
    cdf = 0.0;
  }
  else if ( b < x )
  {
    cdf = 1.0;
  }
  else
  {
    cdf = ( double ) ( x + 1 - a ) / ( double ) ( b + 1 - a );
  }

  return cdf;
}
//******************************************************************************

int uniform_discrete_cdf_inv ( double cdf, int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_CDF_INV inverts the Uniform Discrete CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, int UNIFORM_DISCRETE_CDF_INV, the smallest argument whose 
//    CDF is greater than or equal to CDF.
//
{
  double a2;
  double b2;
  int x;
  double x2;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "UNIFORM_DISCRETE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  a2 = ( double ) ( a ) - 0.5;
  b2 = ( double ) ( b ) + 0.5;
  x2 = a + cdf * ( b2 - a2 );

  x = d_nint ( x2 );

  x = d_max ( x, a );
  x = d_min ( x, b );

  return x;
}
//******************************************************************************

bool uniform_discrete_check ( int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_CHECK checks the parameters of the Uniform discrete CDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, bool UNIFORM_DISCRETE_CHECK, is true if the parameters 
//    are legal.
//
{
  if ( b < a )
  {
    cout << " \n";
    cout << "UNIFORM_DISCRETE_CHECK - Fatal error!\n";
    cout << "  B < A.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double uniform_discrete_mean ( int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_MEAN returns the mean of the Uniform discrete PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, double UNIFORM_DISCRETE_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = 0.5 * ( double ) ( a + b );

  return mean;
}
//******************************************************************************

double uniform_discrete_pdf ( int x, int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_PDF evaluates the Uniform discrete PDF.
//
//  Discussion:
//
//    The Uniform Discrete PDF is also known as the "Rectangular"
//    Discrete PDF.
//
//  Formula:
//
//    PDF(A,B;X) = 1 / ( B + 1 - A ) for A <= X <= B. 
//
//    The parameters define the interval of integers
//    for which the PDF is nonzero.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, double UNIFORM_DISCRETE_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < a || b < x )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 1.0 / ( double ) ( b + 1 - a );
  }

  return pdf;
}
//******************************************************************************

int uniform_discrete_sample ( int a, int b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_SAMPLE samples the Uniform discrete PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int UNIFORM_DISCRETE_SAMPLE, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = uniform_discrete_cdf_inv ( cdf, a, b );

  return x;
}
//******************************************************************************

double uniform_discrete_variance ( int a, int b )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_DISCRETE_VARIANCE returns the variance of the Uniform discrete PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int A, B, the parameters of the PDF.
//    A <= B.
//
//    Output, double UNIFORM_DISCRETE_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = ( double ) ( ( b + 1 - a ) * ( b + 1 - a ) - 1 ) / 12.0;

  return variance;
}
//******************************************************************************

double *uniform_nsphere_sample ( int n, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    UNIFORM_NSPHERE_SAMPLE samples the Uniform Unit Sphere PDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 168.
//
//  Parameters:
//
//    Input, int N, the dimension of the sphere.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double UNIFORM_NSPHERE_SAMPLE[N], a point on the unit N 
//    sphere, chosen with a uniform probability.
//
{
  int i;
  double sum;
  double *x;

  x = new double[n];

  for ( i = 0; i < n; i++ )
  {
    x[i] = normal_01_sample ( seed );
  }

  sum = 0.0;
  for ( i = 0; i < n; i++ )
  {
    sum = sum + x[i] * x[i];
  }
  sum = sqrt ( sum );

  for ( i = 0; i < n; i++ )
  {
    x[i] = x[i] / sum;
  }

  return x;
}
//******************************************************************************

double von_mises_cdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_CDF evaluates the Von Mises CDF.
//
//  Modified:
//
//    05 April 1999
//
//  Reference:
//
//    Geoffrey Hill,
//    ACM TOMS Algorithm 518,
//    Incomplete Bessel Function I0: The Von Mises Distribution,
//    ACM Transactions on Mathematical Software,
//    Volume 3, Number 3, September 1977, pages 279-284.
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//    A - PI <= X <= A + PI.
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, double VON_MISES_CDF, the value of the CDF.
//
{
# define PI 3.141592653589793

  double a1 = 12.0;
  double a2 = 0.8;
  double a3 = 8.0;
  double a4 = 1.0;
  double arg;
  double c;
  double c1 = 56.0;
  double cdf;
  double ck = 10.5;
  double cn;
  double erfx;
  int ip;
  int n;
  double p;
  double r;
  double s;
  double sn;
  double u;
  double v;
  double y;
  double z;
//
//  We expect -PI <= X - A <= PI.
//
  if ( x - a <= -PI )
  {
    cdf = 0.0;
    return cdf;
  }

  if ( PI <= x - a )
  {
    cdf = 1.0;
    return cdf;
  }
//
//  Convert the angle (X - A) modulo 2 PI to the range ( 0, 2 * PI ). 
//
  z = b;

  u = d_modp ( x - a + PI, 2.0 * PI );

  if ( u < 0.0 )
  {
    u = u + 2.0 * PI;
  }

  y = u - PI;
//
//  For small B, sum IP terms by backwards recursion.
//
  if ( z <= ck )
  {
    v = 0.0;

    if ( 0.0 < z )
    {
      ip = z * a2 - a3 / ( z + a4 ) + a1;
      p = ( double ) ( ip );
      s = sin ( y );
      c = cos ( y );
      y = p * y;
      sn = sin ( y );
      cn = cos ( y );
      r = 0.0;
      z = 2.0 / z;

      for ( n = 2; n <= ip; n++ )
      {
        p = p - 1.0;
        y = sn;
        sn = sn * c - cn * s;
        cn = cn * c + y * s;
        r = 1.0 / ( p * z + r );
        v = ( sn / p + v ) * r;
      }
    }
    cdf = ( u * 0.5 + v ) / PI;
  }
//
//  For large B, compute the normal approximation and left tail.
//
  else
  {
    c = 24.0 * z;
    v = c - c1;
    r = sqrt ( ( 54.0 / ( 347.0 / v + 26.0 - c ) - 6.0 + c ) / 12.0 );
    z = sin ( 0.5 * y ) * r;
    s = 2.0 * z * z;
    v = v - s + 3.0;
    y = ( c - s - s - 16.0 ) / 3.0;
    y = ( ( s + 1.75 ) * s + 83.5 ) / v - y;
    arg = z * ( 1.0 - s / y / y );
    erfx = erf ( arg );
    cdf = 0.5 * erfx + 0.5;
  }

  cdf = d_max ( cdf, 0.0 );
  cdf = d_min ( cdf, 1.0 );

  return cdf;
# undef PI
}
//******************************************************************************

double von_mises_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_CDF_INV inverts the Von Mises CDF.
//
//  Discussion:
//
//    A simple bisection method is used on the interval [ A - PI, A + PI ].
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, double VON_MISES_CDF_INV, the corresponding argument of the CDF.
//    A - PI <= X <= A + PI.
//
{
# define PI 3.141592653589793

  double cdf1;
  double cdf2;
  double cdf3;
  int it;
  int it_max = 100;
  double tol = 0.0001;
  double x;
  double x1;
  double x2;
  double x3;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "VON_MISES_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  if ( cdf == 0.0 )
  {
    x = a - PI;
    return x;
  }
  else if ( 1.0 == cdf )
  {
    x = a + PI;
    return x;
  }

  x1 = a - PI;
  cdf1 = 0.0;

  x2 = a + PI;
  cdf2 = 1.0;
//
//  Now use bisection.
//
  it = 0;

  for ( ; ; )
  {
    it = it + 1;

    x3 = 0.5 * ( x1 + x2 );
    cdf3 = von_mises_cdf ( x3, a, b );

    if ( fabs ( cdf3 - cdf ) < tol )
    {
      x = x3;
      break;
    }

    if ( it_max < it )
    {
      cout << " \n";
      cout << "VON_MISES_CDF_INV - Fatal error!\n";
      cout << "  Iteration limit exceeded.\n";
      exit ( 1 );
    }

    if ( ( cdf3 <= cdf && cdf1 <= cdf ) || ( cdf <= cdf3 && cdf <= cdf1 ) )
    {
      x1 = x3;
      cdf1 = cdf3;
    }
    else
    {
      x2 = x3;
      cdf2 = cdf3;
    }
  }

  return x;
# undef PI
}
//*******************************************************************************

bool von_mises_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    VON_MISES_CHECK checks the parameters of the Von Mises PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, bool VON_MISES_CHECK, is true if the parameters are legal.
//
{
# define PI 3.141592653589793

  if ( a < - PI || PI < a )
  {
    cout << " \n";
    cout << "VON_MISES_CHECK - Fatal error!\n";
    cout << "  A < -PI or PI < A.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "VON_MISES_CHECK - Fatal error!\n";
    cout << "  B <= 0.0\n";
    return false;
  }

  return true;
# undef PI
}
//******************************************************************************

double von_mises_mean ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_MEAN returns the mean of the Von Mises PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, double VON_MISES_MEAN, the mean of the PDF.
//
{
  double mean;

  mean = a;

  return mean;
}
//******************************************************************************

double von_mises_pdf ( double x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_PDF evaluates the Von Mises PDF.
//
//  Formula:
//
//    PDF(A,B;X) = EXP ( B * COS ( X - A ) ) / ( 2 * PI * I0(B) )
//
//    where:
// 
//      I0(*) is the modified Bessel function of the first
//      kind of order 0.
//
//    The von Mises distribution for points on the unit circle is
//    analogous to the normal distribution of points on a line.
//    The variable X is interpreted as a deviation from the angle A,
//    with B controlling the amount of dispersion.
//
//  Modified:
//
//    27 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Jerry Banks, editor,
//    Handbook of Simulation,
//    Engineering and Management Press Books, 1998, page 160.
//
//    D J Best and N I Fisher,
//    Efficient Simulation of the von Mises Distribution,
//    Applied Statistics,
//    Volume 28, Number 2, pages 152-157.
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A - PI <= X <= A + PI.
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, double VON_MISES_PDF, the value of the PDF.
//
{
# define PI 3.141592653589793

  double i0b;
  double pdf;

  if ( x < a - PI )
  {
    pdf = 0.0;
  }
  else if ( x <= a + PI )
  {
    pdf = exp ( b * cos ( x - a ) ) / ( 2.0 * PI * bessel_i0 ( b ) );
  }
  else
  {
    pdf = 0.0;
  }

  return pdf;
# undef PI
}
//******************************************************************************

double von_mises_sample ( double a, double b, int *seed )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_SAMPLE samples the Von Mises PDF.
//
//  Modified:
//
//    17 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    D J Best and N I Fisher,
//    Efficient Simulation of the von Mises Distribution,
//    Applied Statistics,
//    Volume 28, Number 2, pages 152-157.
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
# define PI 3.141592653589793

  double c;
  double f;
  double r;
  double rho;
  double tau;
  double u1;
  double u2;
  double u3;
  double x;
  double z;

  tau = 1.0 + sqrt ( 1.0 + 4.0 * b * b );
  rho = ( tau - sqrt ( 2.0 * tau ) ) / ( 2.0 * b );
  r = ( 1.0 + rho * rho ) / ( 2.0 * rho );

  for ( ; ; )
  {
    u1 = d_uniform_01 ( seed );
    z = cos ( PI * u1 );
    f = ( 1.0 + r * z ) / ( r + z );
    c = b * ( r - f );

    u2 = d_uniform_01 ( seed );

    if ( u2 < c * ( 2.0 - c ) )
    {
      break;
    }

    if ( c <= log ( c / u2 ) + 1.0 )
    {
      break;
    }

  }

  u3 = d_uniform_01 ( seed );

  x = a + d_sign ( u3 - 0.5 ) * acos ( f );

  return x;
# undef PI
}
//******************************************************************************

double von_mises_variance ( double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    VON_MISES_VARIANCE returns the variance of the Von Mises PDF.
//
//  Modified:
//
//    27 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    -PI <= A <= PI,
//    0.0 < B.
//
//    Output, double VON_MISES_VARIANCE, the variance of the PDF.
//
{
  double variance;

  variance = 1.0 - bessel_i1 ( b ) / bessel_i0 ( b );

  return variance;
}
//*******************************************************************************

double weibull_cdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_CDF evaluates the Weibull CDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the CDF.
//    A <= X.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double CDF, the value of the CDF.
//
{
  double cdf;
  double y;

  if ( x < a )
  {
    cdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;
    cdf = 1.0 - 1.0 / exp ( pow ( y, c ) );
  }

  return cdf;
}
//*******************************************************************************

double weibull_cdf_inv ( double cdf, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_CDF_INV inverts the Weibull CDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 < CDF < 1.0.
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double X, the corresponding argument of the CDF.
//
{
  double x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << "\n";
    cout << "WEIBULL_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = a + b * pow ( ( - log ( 1.0 - cdf ) ), ( 1.0 / c ) );

  return x;
}
//******************************************************************************

void weibull_cdf_values ( int *n_data, double *alpha, double *beta, 
  double *x, double *fx )

//******************************************************************************
//
//  Purpose:
//
//    WEIBULL_CDF_VALUES returns some values of the Weibull CDF.
//
//  Discussion:
//
//    In Mathematica, the function can be evaluated by:
//
//      Needs["Statistics`ContinuousDistributions`"]
//      dist = WeibullDistribution [ alpha, beta ]
//      CDF [ dist, x ]
//
//  Modified:
//
//    31 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    US Department of Commerce, 1964.
//
//    Stephen Wolfram,
//    The Mathematica Book,
//    Fourth Edition,
//    Wolfram Media / Cambridge University Press, 1999.
//
//  Parameters:
//
//    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the
//    first call.  On each call, the routine increments N_DATA by 1, and
//    returns the corresponding data; when there is no more data, the
//    output value of N_DATA will be 0 again.
//
//    Output, double *ALPHA, the first parameter of the distribution.
//
//    Output, double *BETA, the second parameter of the distribution.
//
//    Output, double *X, the argument of the function.
//
//    Output, double *FX, the value of the function.
//
{
# define N_MAX 12

  double alpha_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.5000000000000000E+01 }; 

  double beta_vec[N_MAX] = { 
     0.5000000000000000E+00,    
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.5000000000000000E+00,  
     0.2000000000000000E+01,  
     0.3000000000000000E+01,  
     0.4000000000000000E+01,  
     0.5000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01,  
     0.2000000000000000E+01 };

  double fx_vec[N_MAX] = { 
     0.8646647167633873E+00,  
     0.9816843611112658E+00,  
     0.9975212478233336E+00,  
     0.9996645373720975E+00,  
     0.6321205588285577E+00,  
     0.4865828809674080E+00,  
     0.3934693402873666E+00,  
     0.3296799539643607E+00,  
     0.8946007754381357E+00,  
     0.9657818816883340E+00,  
     0.9936702845725143E+00,  
     0.9994964109502630E+00 };

  double x_vec[N_MAX] = { 
     0.1000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.4000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.2000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01,    
     0.3000000000000000E+01 };
//
  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *alpha = 0.0;
    *beta = 0.0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *alpha = alpha_vec[*n_data-1];
    *beta = beta_vec[*n_data-1];
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}
//*******************************************************************************

bool weibull_check ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_CHECK checks the parameters of the Weibull CDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, bool WEIBULL_CHECK, is true if the parameters are legal.
//
{

  if ( b <= 0.0 )
  {
    cout << "\n";
    cout << "WEIBULL_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  if ( c <= 0.0 )
  {
    cout << "\n";
    cout << "WEIBULL_CHECK - Fatal error!\n";
    cout << "  C <= 0.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double weibull_mean ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_MEAN returns the mean of the Weibull PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double MEAN, the mean of the PDF.
//
{
  double mean;

  mean = b * gamma ( ( c + 1.0 ) / c ) + a;

  return mean;
}
//*******************************************************************************

double weibull_pdf ( double x, double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_PDF evaluates the Weibull PDF.
//
//  Formula:
//
//    PDF(A,B,C;X) = ( C / B ) * ( ( X - A ) / B )**( C - 1 ) 
//     * EXP ( - ( ( X - A ) / B )**C ).
//
//  Discussion:
//
//    The Weibull PDF is also known as the Frechet PDF.
//
//    WEIBULL_PDF(A,B,1;X) is the Exponential PDF.
//
//    WEIBULL_PDF(0,1,2;X) is the Rayleigh PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the argument of the PDF.
//    A <= X
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;
  double y;

  if ( x < a )
  {
    pdf = 0.0;
  }
  else
  {
    y = ( x - a ) / b;

    pdf = ( c / b ) * pow ( y, ( c - 1.0 ) )  / exp ( pow ( y, c ) );
  }

  return pdf;
}
//*******************************************************************************

double weibull_sample ( double a, double b, double c, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_SAMPLE samples the Weibull PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, double X, a sample of the PDF.
//
{
  double cdf;
  double x;

  cdf = d_uniform_01 ( seed );

  x = weibull_cdf_inv ( cdf, a, b, c );

  return x;
}
//*******************************************************************************

double weibull_variance ( double a, double b, double c )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_VARIANCE returns the variance of the Weibull PDF.
//
//  Modified:
//
//    18 September 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, C, the parameters of the PDF.
//    0.0 < B,
//    0.0 < C.
//
//    Output, double VARIANCE, the variance of the PDF.
//
{
  double g1;
  double g2;
  double variance;

  g1 = gamma ( ( c + 2.0 ) / c );
  g2 = gamma ( ( c + 1.0 ) / c );

  variance = b * b * ( g1 - g2 * g2 );

  return variance;
}
//******************************************************************************

double weibull_discrete_cdf ( int x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    WEIBULL_DISCRETE_CDF evaluates the Discrete Weibull CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the CDF.
//    0 <= X.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A <= 1.0,
//    0.0 < B.
//
//    Output, double WEIBULL_DISCRETE_CDF, the value of the CDF.
//
{
  double cdf;

  if ( x < 0 )
  {
    cdf = 0.0;
  }
  else
  {
    cdf = 1.0 - pow ( 1.0 - a, pow ( x + 1, b ) );
  }

  return cdf;
}
//******************************************************************************

int weibull_discrete_cdf_inv ( double cdf, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    WEIBULL_DISCRETE_CDF_INV inverts the Discrete Weibull CDF.
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double CDF, the value of the CDF.
//    0.0 <= CDF <= 1.0.
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A <= 1.0,
//    0.0 < B.
//
//    Output, int WEIBULL_DISCRETE_CDF_INV, the corresponding argument.
//
{
  int x;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    cout << " \n";
    cout << "WEIBULL_DISCRETE_CDF_INV - Fatal error!\n";
    cout << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  x = d_roundup ( 
    pow ( log ( 1.0 - cdf ) / log ( 1.0 - a ), 1.0 / b ) - 1.0 );

  return x;
}
//*******************************************************************************

bool weibull_discrete_check ( double a, double b )

//*******************************************************************************
//
//  Purpose:
//
//    WEIBULL_DISCRETE_CHECK checks the parameters of the discrete Weibull CDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A <= 1.0,
//    0.0 < B.
//
//    Output, bool WEIBULL_DISCRETE_CHECK, is true if the parameters
//    are legal.
//
{
  if ( a < 0.0 || 1.0 < a )
  {
    cout << " \n";
    cout << "WEIBULL_DISCRETE_CHECK - Fatal error!\n";
    cout << "  A < 0 or 1 < A.\n";
    return false;
  }

  if ( b <= 0.0 )
  {
    cout << " \n";
    cout << "WEIBULL_DISCRETE_CHECK - Fatal error!\n";
    cout << "  B <= 0.\n";
    return false;
  }

  return true;
}
//******************************************************************************

double weibull_discrete_pdf ( int x, double a, double b )

//******************************************************************************
//
//  Purpose:
//
//    WEIBULL_DISCRETE_PDF evaluates the discrete Weibull PDF.
//
//  Formula:
//
//    PDF(A,B;X) = ( 1 - A )**X**B - ( 1 - A )**(X+1)**B.
//
//  Discussion:
//
//    WEIBULL_DISCRETE_PDF(A,1;X) = GEOMETRIC_PDF(A;X)
//
//  Modified:
//
//    18 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//    0 <= X
//
//    Input, double A, B, the parameters that define the PDF.
//    0 <= A <= 1,
//    0 < B.
//
//    Output, double WEIBULL_DISCRETE_PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 0 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = pow ( 1.0 - a , pow ( x, b ) ) - pow ( 1.0 - a, pow ( x + 1, b ) );
  }

  return pdf;
}
//*******************************************************************************

int weibull_discrete_sample ( double a, double b, int *seed )

//*******************************************************************************
//
//  Purpose:  
//
//    WEIBULL_DISCRETE_SAMPLE samples the discrete Weibull PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, B, the parameters of the PDF.
//    0.0 <= A <= 1.0,
//    0.0 < B.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int X, a sample of the PDF.
//
{
  double cdf;
  int x;

  cdf = d_uniform_01 ( seed );

  x = weibull_discrete_cdf_inv ( cdf, a, b );

  return x;
}
//*******************************************************************************

double zeta ( double p )

//*******************************************************************************
//
//  Purpose:
//
//    ZETA estimates the Riemann Zeta function.
//
//  Definition:
//
//    For 1 < P, the Riemann Zeta function is defined as:
//
//      ZETA ( P ) = Sum ( 1 <= N < Infinity ) 1 / N**P
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Daniel Zwillinger, editor,
//    CRC Standard Mathematical Tables and Formulae,
//    30th Edition,
//    CRC Press, 1996.
//
//  Parameters:
//
//    Input, double P, the power to which the integers are raised.
//    P must be greater than 1.  For integral P up to 20, a
//    precomputed value of ZETA is returned; otherwise the infinite
//    sum is approximated.
//
//    Output, double ZETA, an approximation to the Riemann 
//    Zeta function.
//
{
# define PI 3.141592653589793

  int n;
  double value;
  double zsum;
  double zsum_old;

  if ( p <= 1.0 )
  {
    cout << " \n";
    cout << "ZETA - Fatal error!\n";
    cout << "  Exponent P <= 1.0.\n";
    exit ( 1 );
  }
  else if ( p == 2.0 )
  {
    value = pow ( PI, 2 ) / 6.0;
  }
  else if ( p == 3.0 )
  {
    value = 1.2020569032;
  }
  else if ( p == 4.0 )
  {
    value = pow ( PI, 4 ) / 90.0;
  }
  else if ( p == 5.0 )
  {
    value = 1.0369277551;
  }
  else if ( p == 6.0 )
  {
    value = pow ( PI, 6 ) / 945.0;
  }
  else if ( p == 7.0 )
  {
    value = 1.0083492774;
  }
  else if ( p == 8.0 )
  {
    value = pow ( PI, 8 ) / 9450.0;
  }
  else if ( p == 9.0 )
  {
    value = 1.0020083928;
  }
  else if ( p == 10.0 )
  {
    value = pow ( PI, 10 ) / 93555.0;
  }
  else if ( p == 11.0 )
  {
    value = 1.0004941886;
  }
  else if ( p == 12.0 )
  {
    value = 1.0002460866;
  }
  else if ( p == 13.0 )
  {
    value = 1.0001227133;
  }
  else if ( p == 14.0 )
  {
    value = 1.0000612482;
  }
  else if ( p == 15.0 )
  {
    value = 1.0000305882;
  }
  else if ( p == 16.0 )
  {
    value = 1.0000152823;
  }
  else if ( p == 17.0 )
  {
    value = 1.0000076372;
  }
  else if ( p == 18.0 )
  {
    value = 1.0000038173;
  }
  else if ( p == 19.0 )
  {
    value = 1.0000019082;
  }
  else if ( p == 20.0 )
  {
    value = 1.0000009540;
  }
  else
  {
    zsum = 0.0;
    n = 0;

    for ( ; ; )
    {
      n = n + 1;
      zsum_old = zsum;
      zsum = zsum + 1.0 / pow ( ( double ) n, p );
      if ( zsum <= zsum_old )
      {
        break;
      }
    }
    value = zsum;
  }

  return value;
# undef PI
}
//*******************************************************************************

double zipf_cdf ( int x, double a )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_CDF evaluates the Zipf CDF.
//
//  Discussion:
//
//    Simple summation is used.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//    1 <= N
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Output, double CDF, the value of the CDF.
//
{
  double c;
  double cdf;
  double pdf;
  int y;

  if ( x < 1 )
  {
    cdf = 0.0;
  }
  else
  {
    c = zeta ( a );

    cdf = 0.0;
    for ( y = 1; y <= x; y++ )
    {
      pdf = 1.0 / pow ( y, a ) / c;
      cdf = cdf + pdf;
    }

  }

  return cdf;
}
//*******************************************************************************

bool zipf_check ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_CHECK checks the parameter of the Zipf PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Output, bool ZIPF_CHECK, is true if the parameters are legal.
//
{
  if ( a <= 1.0 )
  {
    cout << " \n";
    cout << "ZIPF_CHECK - Fatal error!\n";
    cout << "  A <= 1.\n";
    return false;
  }

  return true;
}
//*******************************************************************************

double zipf_mean ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_MEAN returns the mean of the Zipf PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Output, double ZIPF_MEAN, the mean of the PDF.
//    The mean is only defined for 2 < A.
//
{
  double mean;

  if ( a <= 2.0 )
  {
    cout << " \n";
    cout << "ZIPF_MEAN - Fatal error!\n";
    cout << "  No mean defined for A <= 2.\n";
    exit ( 1 );
  }

  mean = zeta ( a - 1.0 ) / zeta ( a );

  return mean;
}
//*******************************************************************************

double zipf_pdf ( int x, double a )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_PDF evaluates the Zipf PDF.
//
//  Formula:
//
//    PDF(A;X) = ( 1 / X**A ) / C
//
//    where the normalizing constant is chosen so that
//
//    C = Sum ( 1 <= I < Infinity ) 1 / I**A.
//
//  Discussion:
//
//    From observation, the frequency of different words in long
//    sequences of text seems to follow the Zipf PDF, with
//    parameter A slightly greater than 1.  The Zipf PDF is sometimes
//    known as the "discrete Pareto" PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int X, the argument of the PDF.
//    1 <= N
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Output, double PDF, the value of the PDF.
//
{
  double pdf;

  if ( x < 1 )
  {
    pdf = 0.0;
  }
  else
  {
    pdf = 1.0 / pow ( x, a ) / zeta ( a );
  }

  return pdf;
}
//*******************************************************************************

int zipf_sample ( double a, int *seed )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_SAMPLE samples the Zipf PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Luc Devroye,
//    Non-Uniform Random Variate Generation,
//    Springer Verlag, 1986, pages 550-551.
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Input/output, int *SEED, a seed for the random number generator.
//
//    Output, int ZIPF_SAMPLE, a sample of the PDF.
//
{
  double b;
  double t;
  double u;
  double v;
  double w;
  int x;

  b = pow ( 2.0, ( a - 1.0 ) );

  for ( ; ; )
  {
    u = d_uniform_01 ( seed );
    v = d_uniform_01 ( seed );
    w = ( int ) ( 1.0 / pow ( u, 1.0 / ( a - 1.0 ) ) );
  
    t = pow ( ( w + 1.0 ) / w, a - 1.0 );

    if ( v * w * ( t - 1.0 ) * b <= t * ( b - 1.0 ) )
    {
      break;
    }
 
  }

  x = ( int ) w;

  return x;
}
//*******************************************************************************

double zipf_variance ( double a )

//*******************************************************************************
//
//  Purpose:
//
//    ZIPF_VARIANCE returns the variance of the Zipf PDF.
//
//  Modified:
//
//    14 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double A, the parameter of the PDF.
//    1.0 < A.
//
//    Output, double ZIPF_VARIANCE, the variance of the PDF.
//    The variance is only defined for 3 < A.
//
{
  double mean;
  double variance;

  if ( a <= 3.0 )
  {
    cout << " \n";
    cout << "ZIPF_VARIANCE - Fatal error!\n";
    cout << "  No variance defined for A <= 3.0.\n";
    exit ( 1 );
  }

  mean = zipf_mean ( a );

  variance = zeta ( a - 2.0 ) / zeta ( a ) - mean * mean;

  return variance;
}
