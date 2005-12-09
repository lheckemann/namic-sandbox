static const char SccsId_utility[] = "@(#)utility.hh  3.10 07/24/94";

#include <iostream>
#include <fstream>

using namespace std;

void assert2(int, char*);
// inline void assert(int, char*) {}; // avoid all the checking
void test_cout();
void write_YSMP(const char* name, int n_row, int n_col,
    int* ia, int* ja, double* a, int append_flag);
void write_vector(const char* name, int n, double* vector, int append_flag);
void check_finite(const double *);      // check the first element
double read_named(istream&, const char* name);


// progress reporting
typedef void (*report_fncT) (char *       , const int        );
report_fncT  set_report_func(report_fncT new_func            );
void         report_progress(char *comment, const int percent); // current report_fnc


// Collection of inline functions.  Some may need overloading to accomodate
// other data types, like int.

inline void swap(double *& a, double *& b) {double *help = a; a = b; b = help;};

inline void maximize(double &old_max, const double value)
{
  if (value > old_max) old_max = value;
}

inline void minimize(double &old_min, const double value)
{
  if (value < old_min) old_min = value;
}
