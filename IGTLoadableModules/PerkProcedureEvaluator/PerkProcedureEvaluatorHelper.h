#ifndef PERKPROCEDUREEVALUATORHELPER_H
#define PERKPROCEDUREEVALUATORHELPER_H


#include <iomanip>
#include <sstream>


// Useful functions.


// Converters.


std::string DoubleToStr( double d, int digits = 2 );

void StringToBool( std::string str, bool& var );
bool StringToBool( std::string str );
void StringToInt( std::string str, int& var );
void StringToInt( std::string str, unsigned int& var );
int StringToInt( std::string str );
void StringToDouble( std::string str, double& var );
double StringToDouble( std::string str );
void StringToDoubleVector( std::string str, double* var, int n );


#endif
