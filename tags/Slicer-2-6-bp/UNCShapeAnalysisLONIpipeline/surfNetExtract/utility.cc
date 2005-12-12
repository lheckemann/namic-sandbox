#include <string.h>
#include "utility.hh"  
#include "util.h"  

extern char* progname;

void test_cout()
{
  static int counter = 0;

  cout << "cout " << counter++ <<" ok.\n";
  cout.flush();
}

void write_YSMP(const char* name, INTEGER n_row, INTEGER n_col,
    INTEGER* ia, INTEGER* ja, double* a, int append)
{
  double *rowpic = new double[n_col];      // debug: print matrix
  if (append)  
    cout << "\nAppendTo[" << name << ", fromC@{\n";
  else
    cout << "\n"           << name <<" = fromC[{\n";
  for (int row = 0; row < n_row; row++) {
    if (row) cout <<"},\n";
    for (int     col = 0      ; col < n_col    ; col++) rowpic[col  ] = 0.0;
    for (INTEGER j   = ia[row]; j   < ia[row+1]; j++  ) rowpic[ja[j]] = a[j];
    cout << "{" << rowpic[0];
    for (col = 1; col < n_col; col++) {
      if (!(col%8)) cout << "\n";
      cout << ", " << rowpic[col];
    }
  }
  cout << "}}];\n\n";
  delete rowpic;
  cout.flush();
}

void write_vector(const char* name, INTEGER n, double* vector, int append)
{              // debug : print vector
  if (append) 
    cout << "\nAppendTo[" << name << ",  fromC@{" << vector[0];
  else
    cout << "\n"          << name << " = fromC[{" << vector[0];
  for (int i = 1; i < n; i++)
    cout << (i%6!=4 ? ", " : ",\n") << vector[i];
  cout << "}];\n";
  cout.flush();
}

extern "C" int finite(double);
void check_finite(const double *v)      // check first element
{
  assertStr(finite(v[0]), "finite numbers expected");
}

double read_named(istream &ist, const char* name)
{
  const int maxlen = 256;        // maximum length of name
  char name_buffer[maxlen], equal;
  eatwhite(ist);
  ist.width(maxlen);
  ist >> name_buffer;
  assertStr(ist.good(), form("reading name for '%s' at char %d", name, ist.tellg()));
  assertStr(!strcmp(name, name_buffer),
   form("found '%s' instead of '%s' at char %d", name_buffer, name, ist.tellg()));
  ist >> equal;
  assertStr(ist.good(), form("reading '=' for '%s' at char %d", name, ist.tellg()));
  assertStr(equal == '=',
   form("found '%c' at char %d; ('=' expected)", equal, ist.tellg()));
  double value;
  ist >> value;
  assertStr(ist.good(), form("reading value for '%s' at char %d", name, ist.tellg()));
  return value;
}

