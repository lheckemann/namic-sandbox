static const char SccsId[] = "@(#)NewtonIterator.cc  3.29 01/28/98";
#include "utility.hh"
#include <math.h>
#include "iterator.hh"
#include "vector3.hh"

#include "startup.h"

void NewtonIterator::set_parameters(Parameter *par_in) {
    
  par.max_active   = par_in->max_active;
  par.print_itn    = par_in->print_itn;
  //par.reread_param = par_in->reread_param;
  par.delta        = par_in->delta;
  par.constr_tol   = par_in->constr_tol;
  par.line_tol     = par_in->line_tol;
  par.ineq_low     = par_in->ineq_low;
  par.ineq_init    = par_in->ineq_init;
  par.ineq_final   = par_in->ineq_final;
  par.ineq_slack   = par_in->ineq_slack;
  par.newton_tol   = par_in->newton_tol;
  par.rho_init     = par_in->rho_init;
  par.c0rho        = par_in->c0rho;
  par.c1rho        = par_in->c1rho;
  par.c2rho        = par_in->c2rho;
  par.rho_limit    = par_in->rho_limit;
  par.step_small   = par_in->step_small;
  par.step_large   = par_in->step_large;
}

char form[1000];

int NewtonIterator::check_constraints(int n_equal, int n_inequal, /*int *active,*/
        double *inequal,
        /* double* c_hat,*/
        double* c_hat_try, double &badness)
{
  int   act_i = 0;
  min_ineq =  1;        // init with max any ineq could reach
  int    min_pos  = -1;          // where the minimum was found

  for (int i = 0; i < n_inequal; i++)
    if (active[act_i] == i ) c_hat_try[n_equal + act_i++] = inequal[i];
    else if (inequal[i] < min_ineq) {
      min_ineq = inequal[i];
      min_pos  = i;
    }
  if (min_ineq < -ineq_high) return min_pos;
  //assert(active[act_i] == -1, "there must be a sentinel.");
  //assert(act_i == n_active, "all actives must be visited.");
  
  badness = 0;
  int worst = -1;
  for (act_i = n_equal + n_active; act_i-- > 0;) { // also over positive inequalities(?)
    register double increase = sqr(c_hat_try[act_i]) - sqr(c_hat[act_i]);
    if (increase > badness) {
      badness = increase;
      worst = act_i;
    }
  }
  //if (badness > 1) 
  //  cout << " worst: " << worst << "(" << badness << ") ";
  return no_activation;
}



void NewtonIterator::activate_initial(const int n_equal, const int n_inequal,const double *inequal)
{
  cout << "initially active: {";
  n_active = 0;
  for (int i = 0; i < n_inequal; i++)
    if (inequal[i] < -ineq_high) {
      cout << (n_active>0 ? ", ":"") << i;
      active[n_active] = i;
      c_hat[n_equal + n_active++] = inequal[i];
      activity[i] = 6; }
    else
      activity[i] = 0;
  active[-1] = active[n_active] = no_activation;  //mark start and end with a sentinel
  cout << "}.\n";
}



void constraints(const Net&net, const double *x, double *equal, double *inequal)
{
  const double desired_area = 4 * M_PI / net.nface;
  for (int face = 0; face < net.nface; face++)
    // determine inequal through side effect
    equal[face] = spher_area4(x, net.face + 4*face, inequal + 4*face) - desired_area;
}



double one_inequality(const Net &net, const double *x, int which)
{
  double sines[4];
  (void) spher_area4(x, net.face + which/4*4, sines);
  return sines[which%4];
}

NewtonIterator::NewtonIterator(const Net &netR, Parameter &par_in):
  net(netR), par(par_in),
  jacobi_aT (netR.nface-1 + par.max_active, 3*netR.nvert,
       12*(netR.nface-1) + 9*par.max_active),
  jacobi_aTa(netR.nface-1 + par.max_active, netR.nface-1 + par.max_active,
       18*(netR.nvert-4) + 29*par.max_active)
  // < worst case, but > expected maximum
  {
  
  count    = 0;
  last_complete = -1;          // any value < count
  rho    = par.rho_init;
  alpha_step  = 1.0;


  x    = new double[3*net.nvert];
  x_try    = new double[3*net.nvert];
  newton_dir  = new double[3*net.nvert];
  grad    = new double[3*net.nvert];
  gradY    = new double[3*net.nvert];
  gCG    = new double[3*net.nvert];
  hCG    = new double[3*net.nvert];
  constr_ineq  = new double[4*net.nface];    // values of the inequality constraints

  const int max_eq_act  = net.nface-1 + par.max_active;

  lambda  = new double[max_eq_act];    // Lagrange multipliers
  proj_dx  = new double[max_eq_act];
  aTgrad  = new double[max_eq_act];
  c_hat          = new double[max_eq_act];    // values of eql. & active constraints
  c_hat_l       = new double[max_eq_act];
  c_hat_r  = new double[max_eq_act];
  active  = new int   [max_eq_act+2]+1;    // first and last for sentinels
  activity  = new char  [4*net.nface];    // activity levels of inequalities

  start_values(net, x);
  copy_vector(gCG  , x, 3*net.nvert);
  copy_vector(x_try, x, 3*net.nvert);
  for (int i = 0; i < 3*net.nvert; hCG[i++] = 0.0);
  dx = hCG;            // dx is set to newton_dir or hCG

  constraints(net, x, c_hat, constr_ineq);
  ineq_high = par.ineq_init;
  activate_initial(net.nface-1, 4*net.nface, constr_ineq);

  cout << "fromC[expr_] := expr /. (mant_Real)*e + (expon_Integer) :> mant*10^expon;\n";
  cout.flush();
} /* NewtonIterator (constructor) */


NewtonIterator::NewtonIterator(const Net &netR, Parameter &par_in, double *initPar):
  net(netR), par(par_in),
  jacobi_aT (netR.nface-1 + par.max_active, 3*netR.nvert,
       12*(netR.nface-1) + 9*par.max_active),
  jacobi_aTa(netR.nface-1 + par.max_active, netR.nface-1 + par.max_active,
       18*(netR.nvert-4) + 29*par.max_active)
  // < worst case, but > expected maximum
  {
  
  count    = 0;
  last_complete = -1;          // any value < count
  rho    = par.rho_init;
  alpha_step  = 1.0;


  x    = new double[3*net.nvert];
  x_try    = new double[3*net.nvert];
  newton_dir  = new double[3*net.nvert];
  grad    = new double[3*net.nvert];
  gradY    = new double[3*net.nvert];
  gCG    = new double[3*net.nvert];
  hCG    = new double[3*net.nvert];
  constr_ineq  = new double[4*net.nface];    // values of the inequality constraints

  const int max_eq_act  = net.nface-1 + par.max_active;

  lambda  = new double[max_eq_act];    // Lagrange multipliers
  proj_dx  = new double[max_eq_act];
  aTgrad  = new double[max_eq_act];
  c_hat          = new double[max_eq_act];    // values of eql. & active constraints
  c_hat_l       = new double[max_eq_act];
  c_hat_r  = new double[max_eq_act];
  active  = new int   [max_eq_act+2]+1;    // first and last for sentinels
  activity  = new char  [4*net.nface];    // activity levels of inequalities

  copy_vector(x    , initPar, 3*net.nvert);
  copy_vector(gCG  , x,       3*net.nvert);
  copy_vector(x_try, x,       3*net.nvert);
  for (int i = 0; i < 3*net.nvert; hCG[i++] = 0.0);
  dx = hCG;            // dx is set to newton_dir or hCG
  
  constraints(net, x, c_hat, constr_ineq);
  ineq_high = par.ineq_init;
  activate_initial(net.nface-1, 4*net.nface, constr_ineq);

  cout << "fromC[expr_] := expr /. (mant_Real)*e + (expon_Integer) :> mant*10^expon;\n";
  cout.flush();
} /* NewtonIterator (constructor) */


NewtonIterator::~NewtonIterator()
{
  delete [] (double *)x           ;
  delete [] (double *)x_try      ;
  delete [] (double *)dx   ;
  delete [] (double *)lambda     ;
  delete [] (double *)proj_dx    ;
  delete [] (double *)newton_dir ;
  delete [] (double *)grad       ;
  delete [] (double *)gradY      ;
  delete [] (double *)gCG        ;
  delete [] (double *)hCG        ;
  delete [] (double *)constr_ineq;
  delete [] (double *)aTgrad     ;
  delete [] (double *)c_hat      ;
  delete [] (double *)c_hat_l    ;
  delete [] (double *)c_hat_r    ;
  delete [] (double *)active     ;
  delete [] (double *)activity   ;
}



double NewtonIterator::iterate()
{
  sprintf(form,"%3d ", count);
  cout << form;      // print iteration number
  int struct_change = last_complete != count++;

  //------------------------------------------------ Newton: satisfy constraints -------
  if (struct_change) {
    jacobi_aT.from_net(net, n_active, active);
    jacobi_aT.invTables();        // needed by jacobian and set_aTa
  }
  jacobian(jacobi_aT);
  jacobi_aTa.set_aTa(jacobi_aT);
  jacobi_aTa.solve(struct_change, c_hat, proj_dx);
  jacobi_aT .multT(proj_dx, newton_dir);
  if (count-1 == par.print_itn) {            // debug
    cout << "jacobian " <<  par.print_itn << endl; // debug
    jacobi_aT.print("aT", 0);
    estimate_jacobian();
    jacobi_aTa.print("aTa", 0);
    write_vector("cHat", net.nface-1+n_active, c_hat,0);
    write_vector("newtonDir", 3*net.nvert, newton_dir,0);
    write_vector("x", 3*net.nvert, x, 0);
  }
    
  int act, act_keep = no_activation;
  double badness;

  dx = newton_dir;
  double newtonStep;
  for (newtonStep = -1;
       (act = step_and_check(newtonStep, c_hat_l, badness)) != no_activation
       || badness > par.newton_tol;
       newtonStep /= 2)
    act_keep = act;
  sprintf(form," %6.0e %6d", newtonStep, act_keep);
  cout << form; // debug
  copy_vector(x, x_try, 3*net.nvert);      // accept new x
  //// write_vector("x", 3*net.nvert, x, 0);    // debug
  swap(c_hat, c_hat_l);

  //------------------------------------------------ Lagrange multipliers --------------
  calc_gradient();
  //// write_vector("gradient", 3*net.nvert, grad, 0); // debug
  //// estimate_gradient();        // debug
  jacobian(jacobi_aT);          // re-calculate after Newton-step
  //// jacobi_aT.print("aT1", 0);      // debug
  jacobi_aT .mult(grad, aTgrad);
  jacobi_aTa.set_aTa(jacobi_aT);      // re-calculate as well
  jacobi_aTa.solve(0, aTgrad, lambda);
  //// write_vector("lambda", jacobi_aTa.n_col, lambda, 0); // debug
  int inactivated = 0;
  int i;
  for (i = 0; i < n_active; i++)
    if (lambda[net.nface-1 + i] < 0 && c_hat[net.nface-1 + i] > -par.ineq_low) {
      inactivated += inactivate(i);
    }
  if (inactivated > 0) {
    activate(act_keep, "in_out");      // activate Newton's stopper anyway
    if (act_keep != no_activation)
      cout << "\n";
    return quite_bad;
  }

  //------------------------------------------------ Conjugate gradients ---------------
  jacobi_aT .multT(lambda, gradY);
  double numerator = 0.0, denominator = 0.0, norm2gradZ = 0.0;
  for (i = 0; i < 3*net.nvert; i++) {
    double gradZ = gradY[i] - grad[i];      // only one component is kept at a time
    numerator   += (gradZ-gCG[i])*gradZ;
    denominator += sqr(gCG[i]);
    norm2gradZ  += sqr(gradZ);
    gCG[i] = gradZ;
  }
  //// write_vector("gCG", 3*net.nvert, gCG, 0);  // debug
  double gamma = numerator / denominator;
  for (i = 0; i < 3*net.nvert; i++)
    hCG[i] = gCG[i] + gamma * hCG[i];
  //// write_vector("hCG", 3*net.nvert, hCG, 0);  // debug
  
  //------------------------------------------------ Linear minimization ---------------
  double c_sqr_sum;
  dx = hCG;
  //// estimate_jacobian();
  sprintf(form," %6.3f %8.1e", gamma, sqrt(norm2gradZ));
  cout << form;

  int activated = line_search(alpha_step, c_sqr_sum);
  activated += activate(act_keep, "newton");
  if ( activated == 0)
    last_complete = count;

  minimize(ineq_high, -min_ineq*par.ineq_slack);
  maximize(ineq_high, par.ineq_final);
  
  double cost = sqrt(c_sqr_sum) + norm2gradZ;
  sprintf(form," %10.3e %10.3e %10.3e\n", sqrt(c_sqr_sum), cost, ineq_high);
  cout << form;
  return cost;
}




void NewtonIterator::get_solution(double *copy)
{
  copy_vector(copy, x, 3*net.nvert);
}


double NewtonIterator::goal_func()
{
  double goal    = 0.0;

  for (int v = 0; v < net.nvert; v++) {
    Vertex *vertex = net.vert + v;
    for (int nb = 0; nb < vertex->count; nb += 2)
  goal += 1.0 - dotproduct3(x_try+3*v,  x_try+3*vertex->neighb[nb]);
  }
  return goal/2.0;
}



void NewtonIterator::calc_gradient()
{
  for (int v = 0; v < net.nvert; v++)   {
    double nbsum[3];
    nbsum[0] = nbsum[1] = nbsum[2] = 0.0;
    Vertex *vertex = net.vert + v;
    for (int nb = 0; nb < vertex->count; nb += 2) {
      double *neighbor = x+3*vertex->neighb[nb];
      nbsum[0]+= neighbor[0];
      nbsum[1]+= neighbor[1];
      nbsum[2]+= neighbor[2];
    }
    double prod = dotproduct3(x+3*v, nbsum);
    grad[3*v  ] = prod*x[3*v  ] - nbsum[0];
    grad[3*v+1] = prod*x[3*v+1] - nbsum[1];
    grad[3*v+2] = prod*x[3*v+2] - nbsum[2];
  }
}



void NewtonIterator::jacobian(const sparseMatrix &A)    // by finite differences
{
  const double desired_area = 4 * M_PI / net.nface;
  for (int col = 0; col < A.n_col; col++)  { // assume x == x_try
    x_try[col] = x[col] + par.delta;      // go a finite step
    int col_0 = 3*(col/3);        // column rounded down: x-component
    normalize(1, 3, x_try + col_0);
    int j_stop;
    for (j_stop = A.iaT[col+1]; A.rowT[j_stop-1] >= net.nface-1; j_stop--);
    int j_ineq = j_stop;
    double sines[4];
    for (int j = A.iaT[col]; j < j_stop; j++){
      double area_c = spher_area4(x_try, net.face + 4*A.rowT[j], sines) - desired_area;
      A.a[A.jaT[j]] = (area_c - c_hat[A.rowT[j]]) / par.delta;
      int c_nr;
      while (j_ineq < A.iaT[col+1]
       && (c_nr = active[A.rowT[j_ineq]-(net.nface-1)])/4 == A.rowT[j]) {
  A.a[A.jaT[j_ineq]] = (sines[c_nr%4] - c_hat[A.rowT[j_ineq]])/par.delta;
  j_ineq ++;
      }
    }
    if (j_ineq < A.iaT[col+1]) {      // unconstrained face: row = nface-1
      (void) spher_area4(x_try, net.face + 4*(net.nface-1), sines);
      while (j_ineq < A.iaT[col+1]) {
  int c_nr = active[A.rowT[j_ineq]-(net.nface-1)];
  A.a[A.jaT[j_ineq]] = (sines[c_nr%4] - c_hat[A.rowT[j_ineq]])/par.delta;
  j_ineq ++;
      }
    }
    x_try[col_0  ] = x[col_0  ];      // back up the finite step
    x_try[col_0+1] = x[col_0+1];
    x_try[col_0+2] = x[col_0+2];
  }
}



int NewtonIterator::activate(int act, char* info)
{
  if (act == no_activation) return 0;
  //assert(act >= 0 && act < 4*net.nvert, "activate: act out of range");
  cout <<"\n]]] " << info << " : constraint " << act << " -> level "
       << activity[act]+1 << "                            ";
  if (++activity[act] != 3) return 0;
  activity[act] = 5;
  int i;
  for (i = n_active; active[i-1] > act; i--) {
    active[            i] = active[            i-1];
    c_hat [net.nface-1+i] = c_hat [net.nface-1+i-1];
  }
  c_hat [net.nface-1+i] = one_inequality(net, x_try, act);
  active[i] = act;
  active[++n_active] = -1;        // sentinel for check_constraints
  cout << ">>> " << info << " activates constraint " << act
       << " (pos " << i << "); now active: " << n_active
       << "\n                                                                   ";
  //assert(active[i-1] < act, "only an inactive constraint may be activated");
  //assert(c_hat[net.nface-1+i] >= -ineq_high,
  //   "the activated constraint must not be violated");
  //assert(n_active < par.max_active, "too many constraints are active at the same time");
  return 1;
}



int NewtonIterator::inactivate(int pos)
{
  //assert(pos >= 0 && pos < n_active, "inactivate: pos out of range");
  sprintf(form,"%6d", active[pos]);
  cout <<"\n[[[ constraint " << form<< " -> level "
       << activity[active[pos]]-1 << ";          ";
  if (--activity[active[pos]] != 3) return 0;
  activity[active[pos]] = 2;
  n_active--;
  cout << "<<< inactivate constraint " << active[pos] << " at postition " << pos
       << "; now active: " << n_active << "\n";
  for (int i = pos; i < n_active; i++) {
    active[            i] = active[            i+1];
    c_hat [net.nface-1+i] = c_hat [net.nface-1+i+1];
  }
  active[n_active] = -1;        // sentinel for check_constraints
  return 1;
}



int NewtonIterator::line_search(double &fullStep, double &c_sqr_sum)
{
  double m = 0.0;
  double f_m, f_l, f_r, c2s_l, c2s_r,   bad_m, bad_l, bad_r;
  int   viol_l, viol_r, act_l, act_m, act_r;
  int   act_keep_l = no_activation, act_keep_r = no_activation;

  f_m = aug_lagrangian(m, act_m, bad_m, c_sqr_sum, c_hat);
  //assert (act_m == no_activation, "line_search: activation at m = 0");
  //assert (bad_m == 0, "line_search: bad_m should be 0.");

  for (double stepSize = par.step_large; stepSize > par.line_tol; stepSize /= 2.0) {
    double step = fullStep*stepSize;
    f_l = aug_lagrangian(m-step, act_l, bad_l, c2s_l, c_hat_l);
    f_r = aug_lagrangian(m+step, act_r, bad_r, c2s_r, c_hat_r);
    viol_l = act_l != no_activation || bad_l > par.constr_tol;
    viol_r = act_r != no_activation || bad_r > par.constr_tol;
    if      (!viol_l && f_l < f_m && (viol_r || f_l < f_r)) {
      f_m = f_l;  bad_m = bad_l;  m -= step; act_keep_r = no_activation;}
    else if (!viol_r && f_r < f_m && (viol_l || f_r < f_l)) {
      f_m = f_r;  bad_m = bad_r;  m += step; act_keep_l = no_activation;}
    else {
      act_keep_r = act_r; act_keep_l = act_l;
    }
  }

              // not the most efficient solution:
  double dum_bad;
  f_m = aug_lagrangian(m, act_m, dum_bad, c_sqr_sum, c_hat); // update also x_try
  //assert (act_m == no_activation, "line_search: no activation must occur at the end");
  //assert (dum_bad == 0, "line_search: dummy badness should be 0.");

  copy_vector(x, x_try, 3*net.nvert);      // accept new x
  if (m != 0.0)
    rho = rho * (par.constr_tol*par.c1rho/(par.constr_tol*par.c0rho - bad_m)
     + par.c2rho) + par.rho_limit;

  sprintf(form," %6.0e %10.6f %8.1e %8.1e", bad_m, f_m, rho,  m);
  cout << form;

  if (activate(act_keep_l, "left") | activate(act_keep_r, "right")) return 1;
              //else adjust stepsize
  fullStep *= par.step_small;
  maximize(fullStep, fabs(m));

  return 0;
}




int NewtonIterator::step_and_check(double step, double *c_hat_try, double &badness)
{
  spher_step(step, x, dx, net.nvert, x_try);
  constraints(net, x_try, c_hat_try, constr_ineq);
  return check_constraints(net.nface-1, 4*net.nface, /*active,*/  constr_ineq,
         /*c_hat,*/ c_hat_try, badness);
}




double NewtonIterator::aug_lagrangian(double step, int &activate, double &badness,
        double &c_sqr_sum, double *c_hat_try)
{
  activate = step_and_check(step, c_hat_try, badness);
  if (activate != no_activation) return 0;      // new active inequalities
  
  double lagr = goal_func();
  c_sqr_sum = 0;
  for (int i = 0; i < net.nface-1 + n_active; i++) {
    lagr -= lambda[i]*c_hat_try[i];
    if (i < net.nface-1 || c_hat_try[i] < 0)
      c_sqr_sum += sqr(c_hat_try[i]);
  }
  return lagr + rho * c_sqr_sum;
}



void NewtonIterator::estimate_jacobian()
{
  const double stepSize = 0.001;
  int i;
  double *old_dx = dx, dum_bad, *a_row = new double [net.nface-1 + n_active];
  dx = new double [3*net.nvert];
  cout << "a = Table[,{" << 3*net.nvert << "}];\n";
  copy_vector(x_try, x, 3*net.nvert);
  normalize(net.nvert, 3, x_try);      // almost no effect; for exact equality
  for (i = 0; i < 3*net.nvert; dx[i++] = 0.0);
  for (    i = 0; i < 3*net.nvert; i++) {
    dx[i] = 1.0;
    int act = step_and_check(stepSize, c_hat_l, dum_bad);
    if (act == no_activation) {
      for (int j = 0; j < net.nface-1 + n_active; j++)
  a_row[j] = (c_hat_l[j] - c_hat[j]) / stepSize;
      sprintf(form,"a[[%d]]", i+1);
      write_vector(form, net.nface-1 + n_active, a_row, 0);
      }
    else
      cout << "a[[" << i+1 << "]] = activate["<<act<<"];\n";
    dx[i] = 0.0;
  }
  copy_vector(x_try, x, 3*net.nvert);      // return to initial position
  delete dx;
  delete a_row;
  dx = old_dx;
}



void NewtonIterator::estimate_gradient()
{
  const double stepSize = 0.00001;
  double *old_dx = dx, *gradEstim = new double [3*net.nvert];
  dx = new double [3*net.nvert];
  int i;
  for (i = 0; i < 3*net.nvert; dx[i++] = 0.0);
  spher_step(0, x, dx, net.nvert, x_try);
  double goal = goal_func();
  for (    i = 0; i < 3*net.nvert; i++) {
    dx[i] = 1.0;
    spher_step(stepSize, x, dx, net.nvert, x_try);
    dx[i] = 0.0;
    gradEstim[i] = (goal_func() - goal) / stepSize;
  }
  copy_vector(x_try, x, 3*net.nvert);      // return to initial position
  write_vector("gradEstim", 3*net.nvert, gradEstim, 0);
  delete dx;
  dx = old_dx;
}
