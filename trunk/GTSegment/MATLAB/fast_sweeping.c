/* 
FAST SWEEPING ALGORITHM
on unix:
   >> mex -setup
   >> clear mex
   >> mex -g -o fast_sweeping fast_sweeping.c -lm
on windows:
   >> mex -setup
   >> clear mex 
   >> mex fast_sweeping.c
*/
/*[T] = fast_sweeping( T, localCosts, mask, grads, processVars.sweepingConvergenceParam );*/

#include <mex.h>
#include <math.h>
#include "fast_sweeping.h"

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
  /* Setup outputs */
  double *T;

  /* Grab inputs */
  double *localCosts = mxGetPr( prhs[1] );
  double *mask = mxGetPr( prhs[2] );
  double *grads = mxGetPr( prhs[3] );
  const double *convergenceParam = mxGetPr( prhs[4] );

  /* Declare working variables */
  int wY, wX, wZ, y, x, z;                    /* indexing variables for the loops */
  int dir, dir_sign;                          /* indexing variables for the directional component */
  double lcVal;                               /* the local cost value at the neighbors for a (p,d) */
  double x_comp, y_comp, z_comp;              /* gradient direction components */
  double T_y, T_x, T_z, TVal, T_vect[7];      /* T variables */
  int mask_vect[6];                           /* mask vector for the neighbors */
  double bc_temp;                             /* temp variable for the boundary condition computation */
  double abs_x_comp, abs_y_comp, abs_z_comp;  /* weights */
  double delta_iteration = mxGetInf();        /* convergence counter */
  int iteration = 0;                          /* iteration counter */

  /* Size variables */
  const int *TSizes = mxGetDimensions( prhs[0] );
  const int *localCostsSizes = mxGetDimensions( prhs[1] );
  const int *maskSizes = mxGetDimensions( prhs[2] );
  const int *gradsSizes = mxGetDimensions( prhs[3] );

  /* Initialize outputs */
  plhs[0] = mxDuplicateArray( prhs[0] );
  T = mxGetPr( plhs[0] );

  printf("Starting the Fast Sweeping MEX code...\n");

  /* Check command arguments */
  if (check_usage(nrhs,nlhs) == 0)
    return;

  /* Perform the fast sweeping loops */
  printf( " Convergence Parameter = %d\n", (int)convergenceParam[0] );
  printf("  For Iteration 0, Total Change in Arrival Times = %f\n", delta_iteration);
  while( delta_iteration > convergenceParam[0] )
    {
    iteration++;
    delta_iteration = 0;
    /* these 6 for loops iterate through the 8 sweeping directions */
    for( wZ = -1; wZ <= 1; wZ += 2 )
      {
      for( wX = -1; wX <= 1; wX += 2 )
        {
        for( wY = -1; wY <= 1; wY += 2 )
          {
          for( z = (wZ < 0 ? TSizes[2]-2 : 1); (wZ < 0 ? z >= 1 : z <= TSizes[2]-2); z += wZ )
            {
            for( x = (wX < 0 ? TSizes[1]-2 : 1); (wX < 0 ? x >= 1 : x <= TSizes[1]-2); x += wX )
              {
              for( y = (wY < 0 ? TSizes[0]-2 : 1); (wY < 0 ? y >= 1 : y <= TSizes[0]-2); y += wY )
                {
                if( (int)get_matrix3(mask,maskSizes,y,x,z) == 1 )
                  {
                  /* get all voxels in the neighborhood */                                
                  T_vect[0] = get_matrix3(T,TSizes,y-1,x,z);
                  T_vect[1] = get_matrix3(T,TSizes,y+1,x,z);
                  T_vect[2] = get_matrix3(T,TSizes,y,x-1,z);
                  T_vect[3] = get_matrix3(T,TSizes,y,x+1,z);
                  T_vect[4] = get_matrix3(T,TSizes,y,x,z-1);
                  T_vect[5] = get_matrix3(T,TSizes,y,x,z+1);
                  T_vect[6] = get_matrix3(T,TSizes,y,x,z);

                  mask_vect[0] = (int)get_matrix3(mask,maskSizes,y-1,x,z);
                  mask_vect[1] = (int)get_matrix3(mask,maskSizes,y+1,x,z);
                  mask_vect[2] = (int)get_matrix3(mask,maskSizes,y,x-1,z);
                  mask_vect[3] = (int)get_matrix3(mask,maskSizes,y,x+1,z);
                  mask_vect[4] = (int)get_matrix3(mask,maskSizes,y,x,z-1);
                  mask_vect[5] = (int)get_matrix3(mask,maskSizes,y,x,z+1);

                  /* Interpolate missing neighbors arrival times and lengths */
                  if( mask_vect[0] == 0 )
                      {
                          /* Check to make sure the point is not surrounded by masked out region */
                          if( mask_vect[1] == 0 )
                              {
                                  continue;
                              }
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[1];
                          if( bc_temp < T_vect[1] )
                              {
                                  bc_temp = T_vect[1];
                              }
                          if( bc_temp < T_vect[0] )
                              {
                                  T_vect[0] = bc_temp;
                                  set_matrix3(T,TSizes,y-1,x,z,T_vect[0]);
                              }
                      }
                  if( mask_vect[1] == 0 )
                      {
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[0];
                          if( bc_temp < T_vect[0] )
                              {
                                  bc_temp = T_vect[0];
                              }
                          if( bc_temp < T_vect[1] )
                              {
                                  T_vect[1] = bc_temp;
                                  set_matrix3(T,TSizes,y+1,x,z,T_vect[1]);
                              }
                      }
                  if( mask_vect[2] == 0 )
                      {
                          /* Check to make sure the point is not surrounded by masked out region */
                          if( mask_vect[3] == 0 )
                              {
                                  continue;
                              }
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[3];
                          if( bc_temp < T_vect[3] )
                              {
                                  bc_temp = T_vect[3];
                              }
                          if( bc_temp < T_vect[2] )
                              {
                                  T_vect[2] = bc_temp;
                                  set_matrix3(T,TSizes,y,x-1,z,T_vect[2]);
                              }
                      }
                  if( mask_vect[3] == 0 )
                      {
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[2];
                          if( bc_temp < T_vect[2] )
                              {
                                  bc_temp = T_vect[2];
                              }
                          if( bc_temp < T_vect[3] )
                              {
                                  T_vect[3] = bc_temp;
                                  set_matrix3(T,TSizes,y,x+1,z,T_vect[3]);
                              }
                      }
                  if( mask_vect[4] == 0 )
                      {
                          /* Check to make sure the point is not surrounded by masked out region */
                          if( mask_vect[5] == 0 )
                              {
                                  continue;
                              }
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[5];
                          if( bc_temp < T_vect[5] )
                              {
                                  bc_temp = T_vect[5];
                              }
                          if( bc_temp < T_vect[4] )
                              {
                                  T_vect[4] = bc_temp;
                                  set_matrix3(T,TSizes,y,x,z-1,T_vect[4]);
                              }
                      }
                  if( mask_vect[5] == 0 )
                      {
                          /* Compute the boundary condition */
                          bc_temp = 2*T_vect[6]-T_vect[4];
                          if( bc_temp < T_vect[4] )
                              {
                                  bc_temp = T_vect[4];
                              }
                          if( bc_temp < T_vect[5] )
                              {
                                  T_vect[5] = bc_temp;
                                  set_matrix3(T,TSizes,y,x,z+1,T_vect[5]);
                              }
                      }

                  /* iterate through all positive directions */
                  for( dir_sign = -1; dir_sign <= 1; dir_sign += 2 )
                    {
                    for( dir = 0; dir < localCostsSizes[3]; dir += 1 )
                      {
                      /* get the local cost and direction for this position and direction */
                      lcVal = get_matrix4(localCosts,localCostsSizes,y,x,z,dir);
                      y_comp = ((double)dir_sign)*get_matrix2(grads,gradsSizes,dir,0);
                      x_comp = ((double)dir_sign)*get_matrix2(grads,gradsSizes,dir,1);
                      z_comp = ((double)dir_sign)*get_matrix2(grads,gradsSizes,dir,2);

                      /* choose the neighbors to use */
                      if ( y_comp > 0 )
                        {
                        T_y = T_vect[1];
                        }
                      else
                        {
                        T_y = T_vect[0];
                        }
                      if ( x_comp > 0 )
                        {
                        T_x = T_vect[3];
                        }
                      else
                        {
                        T_x = T_vect[2];
                        }
                      if ( z_comp > 0 )
                        {
                        T_z = T_vect[5];
                        }
                      else
                        {
                        T_z = T_vect[4];
                        }

                      /* take absolute value of directional components*/
                      abs_y_comp = abs_double(y_comp);
                      abs_x_comp = abs_double(x_comp);
                      abs_z_comp = abs_double(z_comp);

                      /* compute the new arrival time */
                      TVal = (T_y * abs_y_comp + T_x * abs_x_comp
                             + T_z * abs_z_comp + lcVal)
                        / (abs_y_comp + abs_x_comp + abs_z_comp);

                        /* update if new arrival time is lower than the old time */
                      if ( TVal < get_matrix3(T,TSizes,y,x,z) )
                        {
                        if( TVal < 0 )
                          printf("ERROR: Negative arrival times were computed!\n");

                        /* write results to the data structures */
                        set_matrix3(T,TSizes,y,x,z,TVal);

                        /* update the convergence parameter */
                        if( (!mxIsInf(T_vect[6])) &&
                            (!mxIsInf(delta_iteration)) )
                          {
                          delta_iteration = delta_iteration + T_vect[6]-TVal;
                          }
                        else
                          {
                          delta_iteration = mxGetInf();
                          }
                        } /* End if minimum found */
                      } /* End dir for loop */
                    } /* End dir_sign for loop */
                  } /* End if in foreground */
                } /* End y */
              } /* End x */
            } /* End z */
          } /* End wY */
        } /* End wX */
      } /* End wZ */
    printf("  For Iteration %d, Total Change in Arrival Times = %f\n", iteration, delta_iteration);
    fflush(stdout);
    } /* End while loop */
  printf("Finished Fast Sweeping successfully!\n");
} /* End fast_sweeping function */

/* get and set methods for 2D matrices */
double get_matrix2(const double *matrix2, const int *sizes, const int row, const int col)
{
    return matrix2[sizes[0] * col /* now in correct column */
                   + row];        /* now in correct row */
}
void set_matrix2(double *matrix2, const int *sizes, const int row, const int col, const double val)
{
    matrix2[sizes[0] * col /* now in correct column */
            + row]         /* now in correct row */
        = val;
}

/* get and set methods for 3D matrices */
double get_matrix3(const double *matrix3, const int *sizes, const int row, const int col, const int slice)
{
    return matrix3[sizes[0] * sizes[1] * slice /* now in correct slice */
                   + sizes[0] * col            /* now in correct column */
                   + row];                     /* now in correct row */
}
void set_matrix3(double *matrix3, const int *sizes, const int row, const int col, const int slice, const double val)
{
    matrix3[sizes[0] * sizes[1] * slice /* now in correct slice */
            + sizes[0] * col            /* now in correct column */
            + row]                      /* now in correct row */
        = val;
}

/* get and set methods for 4D matrices */
double get_matrix4(const double *matrix4, const int *sizes, const int row, const int col, const int slice, const int component)
{
    return matrix4[sizes[0] * sizes[1] * sizes[2] * component /* now in correct component */
                   + sizes[0] * sizes[1] * slice              /* now in correct slice */
                   + sizes[0] * col                           /* now in correct column */
                   + row];                                    /* now in correct row */
}
void set_matrix4(double *matrix4, const int *sizes, const int row, const int col, const int slice, const int component, const double val)
{
    matrix4[sizes[0] * sizes[1] * sizes[2] * component /* now in correct component */
            + sizes[0] * sizes[1] * slice              /* now in correct slice */
            + sizes[0] * col                           /* now in correct column */
            + row]                                     /* now in correct row */
        = val;
}

/* absolute value for doubles */
double abs_double( double x )
{
    return (x < 0 ? ((double)-1 * x ) : x);
}

/* check usage method */ 
int check_usage(int nrhs,int nlhs)
{
    /* number of parameters */
    if (nrhs != 5)
        return usage("wrong number of input parameters");
    if (nlhs != 1)
        return usage("wrong number of output parameters");
    return 1; /* passed */
}

int usage(const char *msg)
{
    printf("usage: [T] = fast_sweeping( T, localCosts, mask, grads, processVars.sweepingConvergenceParam );"
           "  %s\n", msg);
    return 0;
}
