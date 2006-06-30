#ifndef __FAST_SWEEPING_H
#define __FAST_SWEEPING_H

void fast_sweeping(const double *fs, const int *fs_dims,        /* fs */
                   const double *mask, const int *mask_dims,    /* mask */
                   const double *grads, const int *grads_dims,  /* grads */
                   double *tstar, double *lstar, double *dstar, /* outputs */
                   const double *seed,                          /* seed */
                   double *eps);                                /* convergence parameter */
double abs_double(double x);

#endif /* __FAST_SWEEPING_H */






/*
int check_usage(int nrhs, int nlhs);
int usage(const char *msg);
double get_fs(const double *fs, const int *fs_dims, 
              const int row, const int col, const int slice, 
              const int dir);
double get_tstar(double *tstar, const int *tstar_dims, 
                 const int row, const int col, const int slice);
void set_tstar(double *tstar, const int *tstar_dims, 
               const int row, const int col, const int slice, 
               const double val);
double get_lstar(double *tstar, const int *tstar_dims, 
                 const int row, const int col, const int slice);
void set_lstar(double *lstar, const int *lstar_dims, 
               const int row, const int col, const int slice, 
               const double val);
double get_dstar(double *dstar, const int *dstar_dims, 
                 const int row, const int col, const int slice, 
                 const int component);
void set_dstar(double *dstar, const int *dstar_dims, 
               const int row, const int col, const int slice, 
               const int component, const double val);
double get_grads(const double *grads, const int *grads_dims, 
                 const int row, const int col);
*/
