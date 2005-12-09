static const char SccsId[] = "@(#)utility.cc  3.8 19 Jul 1994";
#include <string.h>
#include "utility.hh"       
#include <math.h>
#ifdef sparc
#include <ieeefp.h>
#endif



extern char* progname;

void assert2(int condition, char* comment)
{
  if (!condition) {
    cerr << progname << ": An assertion is violated:\n\""
         << comment
         << "\"\nStop, terminate or quit this program now !\n"
         << "Or hit 'c' <Return> to continue and see what happens :-)\n";
  char dummy;
  cin >> dummy;
  }
}

void test_cout()
{
  static int counter = 0;

  cout << "cout " << counter++ <<" ok.\n";
  cout.flush();
}



//----------------- for installing a new_handler -----------------------

static void out_of_store()
{
  assert2(0, "operator new failed: out of store.\n");
}

typedef void (*PF)();          // pointer to function type

// extern PF set_new_handler(PF);

// class installer {
//  public:
//   installer(int)
//     {
//       set_new_handler(out_of_store);
//     }
// };

// static installer dummy1(5007);



void write_YSMP(const char* name, int n_row, int n_col,
    int* ia, int* ja, double* a, int append)
{
  double *rowpic = new double[n_col];      // debug: print matrix
  if (append)  
    cout << "\nAppendTo[" << name << ", fromC@{\n";
  else
    cout << "\n"           << name <<" = fromC[{\n";
  for (int row = 0; row < n_row; row++) {
    if (row) cout <<"},\n";
    int col;    
    for (col = 0      ; col < n_col    ; col++) rowpic[col  ] = 0.0;
    for (int j   = ia[row]; j   < ia[row+1]; j++  ) rowpic[ja[j]] = a[j];
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



void write_vector(const char* name, int n, double* vector, int append)
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


void check_finite(const double *v)      // check first element
{
  assert2(finite(v[0]), "finite numbers expected");
}



double read_named(istream &ist, const char* name)
{
  const int maxlen = 256;        // maximum length of name
  char name_buffer[maxlen], equal;
  //eatwhite(ist);
  ist.width(maxlen);
  ist >> name_buffer;

  //assert2(ist.good(), form("reading name for '%s' at char %d", name, ist.tellg()));
  // assert2(!strcmp(name, name_buffer),
  //   form("found '%s' instead of '%s' at char %d", name_buffer, name, ist.tellg()));
  ist >> equal;
  // assert2(ist.good(), form("reading '=' for '%s' at char %d", name, ist.tellg()));
  // assert2(equal == '=',
  //   form("found '%c' at char %d; ('=' expected)", equal, ist.tellg()));
  double value;
  ist >> value;
  // assert2(ist.good(), form("reading value for '%s' at char %d", name, ist.tellg()));
  return value;
}




//------------------------- progress reporting ----------------------------

static void report_func_default(char * msg, const int percent)
{
  cerr << "--st-- " << msg << " : " << percent << "%\n";
}



static report_fncT report_func = report_func_default;



report_fncT set_report_func(report_fncT new_func)
{
  report_fncT old_func = report_func;
  report_func = new_func;
  return old_func;
}



void report_progress(char *message, const int percent)
{
  (*report_func)(message, percent);
}
