#ifndef __FAST_SWEEPING_H
#define __FAST_SWEEPING_H

double get_matrix2(const double *matrix2, const int *sizes, const int row, const int col);
void set_matrix2(double *matrix2, const int *sizes, const int row, const int col, const double val);
double get_matrix3(const double *matrix3, const int *sizes, const int row, const int col, const int slice);
void set_matrix3(double *matrix3, const int *sizes, const int row, const int col, const int slice, const double val);
double get_matrix4(const double *matrix4, const int *sizes, const int row, const int col, const int slice, const int component);
void set_matrix4(double *matrix4, const int *sizes, const int row, const int col, const int slice, const int component, const double val);
double abs_double( double x );
int check_usage(int nrhs,int nlhs);
int usage(const char *msg);

#endif /* __FAST_SWEEPING_H */
