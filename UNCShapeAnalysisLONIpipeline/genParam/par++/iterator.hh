

#include "sparsemat.hh"  


typedef struct {
  int
    max_active,           // max. # of active inequalities
    print_itn;                  // iter # for debug output; <0 : never
    //    reread_param;                // bool: re-read every 20 itns
  double
    delta,           // step for finite differences
    constr_tol,           // -tolerable constraint violation
    line_tol,           // tolerance in line search
    ineq_low,           // c > -ineq_tol is ok ...
    ineq_init,           // initial value for ineq_high
    ineq_final,            // gradually reduce ineq_high to this
    ineq_slack,                 // but not too tightly (slack > 1)
    newton_tol,           // max badness accepted by Newton step
    rho_init, c0rho, c1rho, c2rho, rho_limit,    // for adapting rho in line_search
    step_small, step_large;             // factors adjust step in line_search
  
} Parameter; 


class NewtonIterator {
  Parameter  &par;
  double        ineq_high;       // ... 2 values for hysteresis
  double  min_ineq;           // value of worst inactive inequality
             // dangerous: file scope variable!

  double  *x, *x_try, *newton_dir, *dx, *proj_dx, *lambda;
  double  rho, alpha_step, *gCG, *hCG;
  double  *constr_ineq, *grad, *gradY, *aTgrad;
  int    n_active, *active;
  double  *c_hat, *c_hat_l, *c_hat_r;
  int    count, last_complete;
  const Net  &net;
  sparseMatrix  jacobi_aT, jacobi_aTa;
  char    *activity;
  
  int   line_search(double &step, double &c_sqr_sum);
  int   step_and_check(double step, double *c_hat_try, double &badness);
  double aug_lagrangian(double step, int &activate,
      double &badness, double &c_sqr_sum, double *c_hat_try);
  int    check_constraints(int n_equal, int n_inequal,
         double *inequal, double* c_hat_old, double &badness);
  void   activate_initial(const int n_equal, const int n_inequal, const double *inequal);
  void   calc_gradient();
  void   jacobian(const sparseMatrix&);
  void   estimate_gradient();
  void   estimate_jacobian();
  int   activate(int act, char*);      // returns 0 if no_activation, else 1
  int   inactivate(int);        // returns 0 if delayed, else 1
  
public:
  NewtonIterator(const Net& netref, Parameter& par);
  NewtonIterator(const Net& netref, Parameter& par, double *initPar);
  ~NewtonIterator();
  double iterate();
  double goal_func();
  void   get_solution(double*);
  void   set_parameters(Parameter *para);        // call once before making iterators
};


const int no_activation = -1;
const double quite_bad  = 1000;

