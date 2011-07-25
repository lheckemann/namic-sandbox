#ifndef PERKPROCEDUREEVALUATORHELPER_H
#define PERKPROCEDUREEVALUATORHELPER_H


#include <iomanip>
#include <sstream>


// Useful functions.


// Converters.


std::string
DoubleToStr( double d, int digits = 2 )
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision( digits ) << d;
  return ss.str();
}


void
StringToBool( std::string str, bool& var )
{
  std::stringstream ss( str );
  ss >> var;
}

bool
StringToBool( std::string str )
{
  bool var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToInt( std::string str, int& var )
{
  std::stringstream ss( str );
  ss >> var;
}

void
StringToInt( std::string str, unsigned int& var )
{
  std::stringstream ss( str );
  ss >> var;
}

int
StringToInt( std::string str )
{
  int var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToDouble( std::string str, double& var )
{
  std::stringstream ss( str );
  ss >> var;
}

double
StringToDouble( std::string str )
{
  double var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToDoubleVector( std::string str, double* var, int n )
{
  std::stringstream ss( str );
  for ( int i = 0; i < n; ++ i )
    ss >> var[ i ];
}


#endif
