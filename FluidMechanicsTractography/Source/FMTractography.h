#ifndef FMTRACTOGRAPHY_H
#define FMTRACTOGRAPHY_H

//INCLUDE C++ HEADER FILES
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <utility>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <cmath>
#include <complex>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <map>
#include <exception>

//#include "nr3.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDiffusionTensor3D.h"
#include "itkmatrix.h"
#include "itksymmetricsecondranktensor.h"

#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkLine.h"

using namespace std;

namespace hageman{

#define DSL_IMGDATATYPE_FLAG "--idtype"
#define DSL_EIGENVOL_FLAG "--ieigenvolfname"
#define DSL_MASKVOL_FLAG "-imaskvolfname"
#define DSL_MINFATHRESHOLD_FLAG "--VISCOSITYCONSTANT"
#define DSL_MINTRACTLENGTH_FLAG "--MINTRACTLENGTH"
#define DSL_MAXTRACTLENGTH_FLAG "--MAXTRACTLENGTH"
#define DSL_SEEDMASKVOL_FLAG "--seedroi"
#define DSL_STOPMASKVOL_FLAG "--stoproi"
#define DSL_STEPSIZE_FLAG "-STEPSIZE"
#define DSL_MAXTURNANGLE_FLAG "--MAXTURNANGLE"
#define DSL_TOTALTRYS_FLAG "--totaltrys"
#define DSL_MAXTRACTNUMBER_FLAG "-maxtractnumber"
#define DSL_OFPREFIX_FLAG "--ofprefix"
#define DSL_WRITEEXCLUDED_FLAG "-writeexcluded"
#define DSL_OFPATH_FLAG "--ofpath"





//TYPEDEF FOR FUNDAMENTAL DATATYPES
typedef unsigned char UINT8;
typedef signed char SINT8;
typedef unsigned short UINT16;
typedef signed short SINT16;
typedef unsigned int UINT32;
typedef signed int SINT32;

//COMPILER INDEPENDENT DATATYPE SIZE
//sizeof returns the size of its operand with respect to the size of type char (in bytes; sizeof(char) = 1)
#define UINT8_SIZE sizeof(unsigned char)
#define SINT8_SIZE sizeof(signed char)
#define UINT16_SIZE sizeof(unsigned short)
#define SINT16_SIZE sizeof(signed short)
#define UINT32_SIZE sizeof(unsigned int)
#define SINT32_SIZE sizeof(signed int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
//these datatype sizes are default for the compiler and included here in case of unforeseen compatibility problems - dont use unless you have to
#define CHAR_SIZE sizeof(char)
#define INT16_SIZE sizeof(short)
#define INT32_SIZE sizeof(int)

//DEBUG MACROS
//preprocessor macros used to validate program steps
#define VERBOSE 0 //set to true if you want the program steps to be written out to stdout
#define VALIDATE 0  //set to true if you want internal program checks to be written to stdout
#define VALIDATECOORDS 0 //set to true if you want to check values at specific array or 2D or 3D coordinates in a volume (VALR,VALC,VALZ)
#define VALR 59
#define VALC 66
#define VALZ 19

//OPTIMIZATION MACROS
#define ERRORCHECK 0  //set to true if you wish to include error-checking steps (use this instead of the enumerated type below if it is in a performance critical area (b/c code never gets
//compiled) or if 
#define TIMECHECK 0  //set to true if you wish to include program benchmarks

//COMPILER/PLATFORM SPECIFIC MACROS
//use the following macros for code that is dependent on the type of compiler or platform the program is running; USE ONLY IN PERFORMANCE CRITICAL AREAS; it is safer to use the functions
//below inside if/else statements
#define WIN32PLATFORM _WIN32
#define MACPLATFORM __MACH__
#define MSVSCOMPILER _MSC_VER
#define GNUCOMPILER __GNUC__

#ifndef _WIN32  //_WIN32 is defined on Win32 platforms
#define _CVTBUFSIZE 1000  //sets _CVTBUFSIZE to 1000 on non-Win32 platforms to avoid undefined errors on those platforms
#endif

//SPECIFIC PLATFORM/ENVIRONMENT COMPILATIONS MACROS
#define PIPELINE 1  //set to true if the program is to be compiled for the LONI pipeline

//MATHEMATICAL VARIABLE DEFINITIONS
#define PI 3.15149f

//namespace hageman{

enum ImageFileFormat{DICOM, ANALYZE, NIFTI, RAWTEXT, MINC, NRRD};
enum ImageType{VOLUME, SLICE};

#define IMGFMT_DEFAULT hageman::ANALYZE

//MACHINE-SPECIFIC ENUMERATED TYPES
enum OSType{WIN,OSX,SOLARISX,UNKNOWNOS}; //an enumerated type for the operating system in which the program is being run
enum CompilerType{MSVS,GNU,UNKNOWNCOMPILER};  //an enumerated type describing the type of compiler being used

//MEMORY-HANDLING/ERROR-HANDLING ENUMERATED TYPES
enum AllocateMem{NOALLOCATE,STDALLOCATE};  //an enumerated type that specifies whether memory should be allocated to variables
enum ErrorCheck{NOCHECK,STDCHECK};  //an enumerated type that specifies whether error checking should be performed; use this enumerated type instead of the above macro if it isn't in a 
//performance critical area

//DATA ENUMERATED TYPES
enum DataType{TEXTTYPE, CHARTYPE, UINT8TYPE, UINT16TYPE, UINT32TYPE, SINT8TYPE, SINT16TYPE, SINT32TYPE, FLOATTYPE, DOUBLETYPE, RGBTYPE, UNKNOWNTYPE, EMPTYTYPE, UCHARTYPE, EIGENTYPE}; 

//SYSTEM FUNCTIONS
void continueOrExit();  //utility function used to ask the user whether they wish to continue to program or exit(1)
OSType getOSType();  //returns the type of OS as an OSType
CompilerType getCompilerType();  //return the type of compiler as a CompilerType

template<class T>
inline void allocateMem(T*& dataarray, const int ARRAYSIZE, AllocateMem memflag)  
//utility function that allocates memory to the pointer dataarray
{
 if(memflag == STDALLOCATE)
 {
  dataarray = new T[ARRAYSIZE];
 }
#if ERRORCHECK
 else
 {
  std::cout<<"WARNING in void allocateMem(T*& dataarray, const int ARRAYSIZE, AllocateMem memflag)"<<std::endl;
  std::cout<<"No valid form of memory allocation"<<std::endl;
 }
#endif

}

//DATATYPE FUNCTIONS
DataType convertStringToDataType(const std::string &input);  //converts a specific string representation of a datatype to a enum DataType 
const int getDataTypeByteSize(const DataType &dt);  //returns the size of the datatype that corresponds to dt in bytes
std::string convertDataTypeToString(const DataType &dt);  //converts an enum DataType to a specific string representation of a datatype

template<class T>
inline std::string getObjectType(const T &obj)
//returns a string representation of the object type as returned by the typeid function
{
 std::string tname(typeid(obj).name());  //uses the typeid function to get the object type at runtime
 return (tname);
}

template<class T>
DataType getDataType(const T &obj)
//returns the datatype of T as an enum DataType
{
 //std::string typeobj(getObjectType(obj));
 
 //this is a hack to get around some problem that GCC has with default constructors that are prefaced by signed/unsigned
 unsigned char *ucharobj = new unsigned char();
 signed char *scharobj = new signed char();
 unsigned short *ushortobj = new unsigned short();
 signed short *sshortobj = new signed short();
 unsigned int *uintobj = new unsigned int();
 signed int *sintobj = new signed int();
 
 //std::string uint8type(getObjectType<unsigned char>(*ucharobj));
 //std::string sint8type(getObjectType<signed char>(*scharobj));
 //std::string uint16type(getObjectType<unsigned short>(*ushortobj));
 //std::string sint16type(getObjectType<signed short>(*sshortobj));
 //std::string uint32type(getObjectType<unsigned int>(*uintobj));
 //std::string sint32type(getObjectType<signed int>(*sintobj));
 //std::string floattype(getObjectType<float>(float()));
 //std::string doubletype(getObjectType<double>(double()));
 
 //std::string chartype(getObjectType<char>(char()));  //used if the default char type is needed
 
 

 if(typeid(obj) == typeid(char()))
  return CHARTYPE;
 if(typeid(obj) == typeid(*ucharobj))
  return UINT8TYPE;
 if(typeid(obj) == typeid(*ushortobj))
  return UINT16TYPE;
 if(typeid(obj) == typeid(*uintobj))
  return UINT32TYPE;
 if(typeid(obj) == typeid(*scharobj))
  return SINT8TYPE;
 if(typeid(obj) == typeid(*sshortobj))
  return SINT16TYPE;
 if(typeid(obj) == typeid(*sintobj))
  return SINT32TYPE;
 if(typeid(obj) == typeid(float()))
  return FLOATTYPE;
 if(typeid(obj) == typeid(double()))
  return DOUBLETYPE;

 //delete the above dynamic data
 delete ucharobj;
 ucharobj = NULL;
 delete scharobj;
 scharobj = NULL;
 delete ushortobj;
 ushortobj = NULL;
 delete sshortobj;
 sshortobj = NULL;
 delete uintobj;
 uintobj = NULL;
 delete sintobj;
 sintobj = NULL;

 //if(typeobj == chartype)
 // return CHARTYPE;
 //if(typeobj == uint8type)
 // return UINT8TYPE;
 //if(typeobj == uint16type)
 // return UINT16TYPE;
 //if(typeobj == uint32type)
 // return UINT32TYPE;
 //if(typeobj == sint8type)
 // return SINT8TYPE;
 //if(typeobj == sint16type)
 // return SINT16TYPE;
 //if(typeobj == sint32type)
 // return SINT32TYPE;
 //if(typeobj == floattype)
 // return FLOATTYPE;
 //if(typeobj == doubletype)
 // return DOUBLETYPE;



 return UNKNOWNTYPE;  //returns an enumerated type to specify that the datatype is unknown if it doesnt find a match

}


//CHECK FUNCTIONS
template<class S,class T>
bool areMapKeysEqual(const std::pair<S,T> &elem1, const std::pair<S,T> &elem2)
//checks whether two map elements (as std::pair) have equal keys
{

#if VALIDATE
 std::cout<<"{From function bool areMapKeysEqual(const std::pair<S,T> &elem1, const std::pair<S,T> &elem2)"<<std::endl;
 std::cout<<"Element 1 Key: "<<elem1.first<<std::endl;
 std::cout<<"Element 2 Key: "<<elem2.first<<std::endl;
#endif

 return (elem1.first == elem2.first);

}

//STRING/CHARACTER PROCESSING FUNCTIONS
void stripQuotes(std::string &input);  //function which strips off the quotation marks on the ends of arguments on the command line
void addTerminatingBackslashToDirectoryString(std::string &input); //function which adds a terminating backslash to anything it considers a directory
void removeTerminatingBackslashFromDirectoryString(std::string &input); //function which removes a terminating backslash from anything it considers a directory
std::string removedPrecedingDotFromExtension(const std::string &ext);  //removes the . from a three letter file extension
bool doesStringBeginWith(const std::string &teststr, char test);  //returns true if the string begins with test
std::string replaceFileExtension(std::string input, std::string extension);  //replaces the three letter file extension that follows the terminal . with extension.
void addATrailingZeroIfNeeded(std::string &data); //adds a trailing zero to a number string that ends with a . 
void removeTrailingWhitespaces(std::string &input);  //removes the trailing whitespaces from the string input
void removePrecedingWhitespaces(std::string &input);  //removes the preceding whitespaces from the string input
void removeWhitespaces(std::string &input);  //removes all the whitespaces from the string input
void addPrecedingDot(std::string &input); //adds a preceding dot to input; if there is already a dot, the input is left unchanged
std::string getFileNameMinusPath(const std::string &input);  //stripes off the path and returns just the filename.  The input is not changed.
std::string getFileNameMinusExtension(const std::string &input);  //stripes off the extension and returns just the filename.  The input is not changed.
std::string addSpaceBetweenBeforeCapitals(const std::string &input);  //adds a space before any captial letter except the beginning of the string
int getNumberOfDataInLine(std::string cline, std::string delim = ",");  //gets the number of data elements in a line separated by a deliminiter character
void removePrecedingNonDigitChars(std::string &input);  //removes all the non-number characters from input



//PARSING/CONVERSION FUNCTIONS 
template<class T>
void convertVectorOfStringsToArray(const std::vector<std::string> &data, T*& dataarray, hageman::AllocateMem memflag = hageman::NOALLOCATE)
//this function converts a vector of string elements to an array of corresponding elements of type T.  if AllocateMem = NOALLOCATE, then dataarray is assumed to have enough space already
//allocated to store the converted vector of strings.  This general form of the conversion function assumes that there is a static_cast conversion between std::string and T.
{
 //checks whether memory needs to be allocated to dataarray
 if(memflag != NOALLOCATE)
 {
  const int ARRAYSIZE = data.size();
  hageman::allocateMem(dataarray,ARRAYSIZE,memflag);
 }

 //converts each string and copies it into dataarray 
 for(int idx = 0; idx < data.size(); idx++)
 {
  dataarray[idx] = static_cast<T>(data.at(idx));
 }
}

template<>
void convertVectorOfStringsToArray<double>(const std::vector<std::string> &data, double*& dataarray, hageman::AllocateMem memflag);  //this instantiation of the above general function
//this function converts a vector of string elements to an array of corresponding elements of type T.  if AllocateMem = NOALLOCATE, then dataarray is assumed to have enough space already
//allocated to store the converted vector of strings.
//uses atof to perform the conversion from string to double



template<class T>
bool convertToBoolean(const T &input) 
//this parent template function is here in case i can make a generalized boolean conversion function at some point
{

#if ERRORCHECK
 std::cout<<"ERROR in bool convertToBoolean(const T &input)"<<std::endl; 
 std::cout<<"Haven't implemented a general Boolean conversion yet.  Exiting..."<<std::endl;
 exit(1);
#endif

 return false;

}

template<>
bool convertToBoolean<std::string>(const std::string &input);
//converts the string input to a boolean value: values strings must be either "true" or "false" to be converted


template<class T>
void byteswap(T *bytearray, const int BUFFERSIZE) 
//performs a byteswap on the array
{
 //temp vector to store reversed data
 std::vector<T> bytevec;

 for(int idx = 0; idx < BUFFERSIZE; idx++)  //copies the array into a vector
  bytevec.push_back(bytearray[idx]);
 
 std::reverse(bytevec.begin(), bytevec.end());  //uses the reverse algorithm to reverse the values in place

 for(int idx = 0; idx < BUFFERSIZE; idx++)  //copies the reversed data back into the array
  bytearray[idx] = bytevec.at(idx);
 
}


template<class T>
void parseLineOfNumbers(T*& results, const int NUMSIZE, std::string textline, std::string delim, AllocateMem memflag) 
//a templated function for parsing a string of text as a series of NUMSIZE numbers of datatype T, each separated by delim
{

 //checks whether memory needs to be allocated to results
 if(memflag != NOALLOCATE)
 {
  hageman::allocateMem(results,NUMSIZE,memflag);
 }
 
 //removes whitespaces for before or after the string
 textline = hageman::removePrecedingWhitespaces(textline);
 textline = hageman::removeTrailingWhitespaces(textline);
 
 std::string numstr;
 int idx = 0;

 while((textline.find(delim) != std::string::npos) && (idx < NUMSIZE))  //searching for delimiter
 {
  numstr = textline.substr(0,textline.find(delim));  
  results[idx] = static_cast<T>(numstr);  //this only works if T supports a cast from std::string.  Need a special version to deal with 
  //built-in datatypes (i.e. double, int)
  textline.erase(0, textline.find(delim)+1);  //erases everything up to the current first delimiter
  textline = hageman::removePrecedingWhitespaces(textline); //removes any additional whitespaces that may be between elements
  numstr.clear();
  idx++;
   
 }

 //if there is one remaining item (no delim is present), then need to convert the one remaining element
 if(textline.find(delim) == std::string::npos)
 {
  results[idx] = static_cast<T>(textline);  //uses atof to convert numstr to a float/doubletextline
  idx++;
 }

//checks whether the number of converted substrings is equal to the specified arraysize NUMSIZE
#if ERRORCHECK
 if(idx != NUMSIZE)
 {
   std::cout<<"ERROR in void parseLineOfNumbers(std::string textline, T*& results, int NUMSIZE, std::string delim = " ", AllocateMem memflag = NOALLOCATE)"<<std::endl;
   std::cout<<"Number of elements (idx) not equal to specified array size.  Exiting..."<<std::endl;
   exit(1);
 }
#endif
 
}

template<>
void parseLineOfNumbers<double>(double*& results, const int NUMSIZE, std::string textline, std::string delim, AllocateMem memflag);  
//a special form of the templated function for parsing a string of text as a series of NUMSIZE numbers of datatype double, each separated by delim.  This is needed to invoke the correct 
//cast operation from string to double using atof()


template<class T>
void parseLineOfNumbers(T results[], const int NUMSIZE, std::string textline, std::string delim)
//a templated function for parsing a string of text as a series of NUMSIZE numbers of datatype T, each separated by delim
{
 T *ptr = results;
 hageman::parseLineOfNumbers(ptr,NUMSIZE,textline,delim,hageman::NOALLOCATE);


}

template<class T>
void parseLineOfNumbers(std::string textline, std::vector<T> &results, std::string delim = ",") 
//a templated function for parsing a string of text as a series of NUMSIZE numbers of datatype T, each separated by delim
{
 if(!textline.empty())
 {
  std::string numstr;
 
  while(textline.find(delim) != std::string::npos)
  {
   numstr = textline.substr(0,textline.find(delim));
   results.push_back(static_cast<T>(numstr));  //this only works if T supports a cast from std::string.  Need a special version to deal with 
   //built-in datatypes (i.e. double, int)
   textline.erase(0, textline.find(delim)+1);
   textline = hageman::removePrecedingWhitespaces(textline); //removes any additional whitespaces that may be between elements
   numstr.clear();
   
  }
 }
  
}

template<>
void parseLineOfNumbers<double>(std::string textline, std::vector<double> &results, std::string delim); 


template<class T>
void convertDTIGradient3VecTo6Vec(const T *threevec, T *sixvec)
//converts the three coord representation of a gradient direction in DTI (x,y,z) to a six coord representation [xx yy zz xy xz yz]
{
 sixvec[0] = threevec[0]*threevec[0];
 sixvec[1] = threevec[1]*threevec[1];
 sixvec[2] = threevec[2]*threevec[2];
 sixvec[3] = threevec[0]*threevec[1];
 sixvec[4] = threevec[0]*threevec[2];
 sixvec[5] = threevec[1]*threevec[2];


}

//how about a 6vec->3vec conversion?


//GET FUNCTIONS
std::string getFileExtensionWithoutDot(const std::string &fname);  //returns the three letter filename extension without a period


template<class S, class T>
std::vector<T> getMultipleParametersFromMultimap(S key, const std::multimap<S,T> &data)
//a templated function for returning all instances of a key in a STL multimap.  May be expensive because this function must create a temporary object to avoid
//altering the original data
{
 //THIS WOULD BE BETTER IF I COULD KNOW THAT THE SORTING OF MULTIMAPS PUT ALL EQUAL KEYS IN A SEQUENCE, BUT I CAN'T BE SURE OF THAT (NEED TO 
 //INVESTIGATE THIS) -> ANSWER: REWORK THIS FUNCTION TO USE EQUAL_RANGE

#if ERRORCHECK
 //first tries to find the matching key.  If no key is found, then there is no need to invoke to proceeding rather expensive statements, so the program exits 
 typename std::multimap<S, T>::const_iterator pos = data.find(key);
 if(pos == data.end())
 {
  std::cout<<"ERROR in std::vector<T> getMultipleParametersFromMultimap(S key, const std::multimap<S,T> &data)"<<std::endl;
  std::cout<<"Flag does not exist.  Exiting."<<std::endl;
  exit(1);
 }
#endif

 typename std::pair<typename std::multimap<S,T>::const_iterator, typename std::multimap<S,T>::const_iterator> rangeiter; //pair which will hold results of equal_range
 typename std::multimap<S, T>::const_iterator pos1;  //iteratates over the multimap to copy data
 rangeiter = data.equal_range(key);  //member function of multimap that return the lower and upper bound of elements that are equal to key
 std::vector<T> values;  //the resulting values
 for(pos1 = rangeiter.first; pos1 != rangeiter.second; pos1++)  //assigns the values that span the results of equal_range to a string vector 
 {
  values.push_back((*pos1).second);  //holds the found values from equal_range
 }
 
 
 return values;
}



//PRINT FUNCTIONS
template<class T>
void printOutSTLSequenceContainer(const T &container)
//a templated function that can print out the elements of any STL sequence container on the command line.  It prints it out one element per line from the
//beginning of the container to the end
{
 
 typename T::const_iterator pos;
 
 for(pos = container.begin(); pos != container.end(); pos++)
 {
  std::cout<<*pos<<std::endl;
 }

}

template<class T>
void printOutSTLMap(const T &container)
//a templated function that can print out the key/value pairs for any map or multimap.  It prints out one pair per line from beginning of the map to the end
{
 typename T::const_iterator pos;

 for(pos = container.begin(); pos != container.end(); pos++)
 {
  std::cout<<(pos)->first<<": "<<(pos)->second<<std::endl;
 }

}


template<class T>
std::ostream& operator<<(std::ostream &output, const std::pair<const T*,const int> &data)
//overloaded ostream operator that outputs an array of objects T with size of int (one per line).  const T* is the array and const int is the size of const T*
{

 for(int idx = 0; idx < data.second; idx++)  //data.second holds the size of the array
 {
  output << (data.first)[idx] << "\n";  //outputs the array values, one per line
 }

 return output;

}

//MATH FUNCTIONS

template<class T>
inline T matchSign(const T &var, const T &ref) 
//returns var with the sign that matches ref
{
 return (ref >= T() ? (var >= 0 ? var : -var) : (var >= 0 ? -var : var));
}  
 
template<class T>
inline T getMax(const T &a, const T &b)
//returns the maximum of (a,b)
{
 return (b > a ? (b) : (a));
}

template<class T>
inline T getMin(const T &a, const T &b)
//returns the maximum of (a,b)
{
 return (b < a ? (b) : (a));
}

template<class T>
T computePythagoreanTheorem(const T &a, const T &b)
//computes the vector (a^2 + b^2)^(.5)
{

 double absa, absb;
 double tmp;
 T result = T();

 absa = fabsf(a);
 absb = fabsf(b);

 if(absa > absb)
 {
  tmp = static_cast<double>(absb)/static_cast<double>(absa);
  result = static_cast<T>(absa*sqrtf(1.0f + (tmp*tmp)));
 }
 else
 {
  if(absb != T())
  {
   tmp = static_cast<double>(absa)/static_cast<double>(absb);
   result = static_cast<T>(absb*sqrtf(1.0f + (tmp*tmp)));
  }
 }

 return result;
  
} 

template<class T, const int ARRAYSIZE>
std::pair<T,int> getMaxOfArray(const T *d1)
//return the max element of an array and its array index
{
 T maxelem = d1[0];
 int maxidx = 0;

 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  if(d1[idx] > maxelem)
  {
   maxelem = d1[idx];
   maxidx = idx;
  }
 }

 std::pair<T,int> result(maxelem,maxidx);
 return result;

}

template<class T, const int ARRAYSIZE>
std::pair<T,int> getMinOfArray(const T *d1)
//return the min element of an array and its array index
{
 T minelem = d1[0];
 int minidx = 0;

 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  if(d1[idx] < minelem)
  {
   minelem = d1[idx];
   minidx = idx;
  }
 }

 std::pair<T,int> result(minelem,minidx);
 return result;
 
}


template<class T, const int ARRAYSIZE>
T generalizedDotProduct(const T *d1, const T *d2)
//this function is a generalized dot product function that takes two arrays and their common SIZE.  It is a function to be called by those classes that can not or do not want to implement 
//my full Volume hierarchy
{
 T result = T();
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  result += d1[idx]*d2[idx];
 }

 return result;

}

template<class T, const int ARRAYSIZE>
T generalizedAbsDotProduct(const T *d1, const T *d2)
//this function return the absolute value of a generalized dot product function that takes two arrays and their common SIZE.  It is a function to be called by those classes that can not or do not want to implement 
//my full Volume hierarchy
{
 return (static_cast<T>(fabsf(hageman::generalizedDotProduct<T,ARRAYSIZE>(d1,d2))));
 
}

template<class T, const int ARRAYSIZE>
double takeL2NormOfArray(const T *data)
//takes the L2 norm of the array data of size ARRAYSIZE.  It is a function to be called by those classes that can not or do not want to implement my full Volume hierarchy
{
 double norm = 0.0;

 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  norm += static_cast<double>(data[idx]) * static_cast<double>(data[idx]);
 }

 norm = sqrtf(norm);
 return norm;


}

template<class T, const int ARRAYSIZE>
void normalizeArray(T *data)
//normalizes the array data of size ARRAYSIZE using the L2 norm.  The current data is altered.  It is a function to be called by those classes that can not or do not want to implement my 
//full Volume hierarchy
{
 double norm = takeL2NormOfArray<double,ARRAYSIZE>(data);
 double coord;

 //checks to make sure that norm of vector is not zero before attempting to divide.  Not under control of ERRORCHECK b/c zero vector is a valid vector
 if(norm == 0.0)
 {
#if ERRORCHECK
  std::cout<<"WARNING in void normalizeArray(T *data)"<<std::endl;  
  std::cout<<"Cannot normalize this vector; Norm is zero.  Vector is unchanged"<<std::endl;
#endif
 }
 else
 {
  //divides each of the array elements by the norm
  for(int idx = 0; idx < ARRAYSIZE; idx++)
  {
   coord = static_cast<double>(data[idx]);
   coord /= norm;
   data[idx] = static_cast<T>(coord);
  }
 }

 
}

template<class T, int ARRAYSIZE>
void initNormedAverageArray(T*& result, const T *vec1, const T *vec2, hageman::AllocateMem memcheck = NOALLOCATE)
//computes the element-wise average of the vec1 and vec2
{

 //determines whether memory needs to be allocated
 if(memcheck != NOALLOCATE)
 {
  hageman::allocateMem(result,ARRAYSIZE,memcheck);
 }

 //computes the simple average of each array element
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  result[idx] = 0.5*(vec1[idx] + vec2[idx]);
 }
 
 //normalizes the result
 hageman::normalizeArray<T,ARRAYSIZE>(result);
 

}


template<class T, int ARRAYSIZE>
void initNormedAverageArray(T result[], const T *vec1, const T *vec2)
//computes the element-wise average of the vec1 and vec2
{

 //computes the simple average of each array element
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  result[idx] = 0.5*(vec1[idx] + vec2[idx]);
 }
 
 //normalizes the result
 hageman::normalizeArray<T,ARRAYSIZE>(result);
 

}


template<class T, const int ARRAYSIZE>
void getPerpendicularArray(T*& result, const T *paradigm, hageman::AllocateMem memcheck = NOALLOCATE)
//computes the vector perpendicular to paradigm and stores it in result.  Both result and paradigm must be equal to ARRAYSIZE.  It is a function to be called by those classes that can not or do not want to implement my 
//full Volume hierarchy
{

 //determines whether memory needs to be allocated
 if(memcheck != NOALLOCATE)
 {
  hageman::allocateMem(result,ARRAYSIZE,memcheck);
 }

 //checks to make sure that norm of vector is not zero before attempting to divide.  Not under control of ERRORCHECK b/c zero vector is a valid vector
 if(takeL2NormOfArray<T,ARRAYSIZE>(paradigm) == 0.0)
 {
#if ERRORCHECK
  std::cout<<"WARNING: void getPerpendicularArray(T *result, const T *paradigm)"<<std::endl;
  std::cout<<"Cannot take the perpendicular direction of a zero vector.  Returning a zero vector."<<std::endl;
#endif
  //generates a zero vector
  for(int idx = 0; idx < ARRAYSIZE; idx++)
   result[idx] = T();
 }
 else
 {
  //creates a normalized copy of the current object to generate the perpendicular vector from 
  T *temp = new T[ARRAYSIZE];
  
  for(int idx = 0; idx < ARRAYSIZE; idx++)
   temp[idx] = paradigm[idx];

  normalizeArray<T,ARRAYSIZE>(temp);

    
  //initializing the vector with 1's guarantees that it will not return the zero vector solution
  for(int idx = 0; idx < ARRAYSIZE; idx++)
   result[idx] = static_cast<T>(1.0);

  //returns the array index of vec of any nonzero element to prevent a divide by zero error when solving for that element in pvec
  int index = 0;
  while(temp[index] == T())
   ++index;

  double denom = static_cast<double>(temp[index]);
  
  T num = T();
  for(int idx = 0; idx < ARRAYSIZE; idx++)
  {
    if(idx == index)
    continue;
   num += temp[idx]*result[idx]; 
  }

  double element = -(static_cast<double>(num)/static_cast<double>(denom));
  result[index] = static_cast<T>(element);

  delete[] temp;
 }
 
}

template<class T>
void calculateCrossProductForArray3(T*& result, const T *vec1, const T *vec2)
//solves for the cross product between the two 3 element arrays, vec1 and vec2.  It is a function to be called by those classes that can not or do not want to implement my 
//full Volume hierarchy
{
 //intializes result as a zero vector
 for(int idx = 0; idx < 3; idx++)
  result[idx] = T();

 T x = (vec1[1])*(vec2[2]);
 x -= (vec1[2])*(vec2[1]);

 T y = (vec1[2])*(vec2[0]);
 y -= (vec1[0])*(vec2[2]);

 T z = (vec1[0])*(vec2[1]);
 z -= (vec1[1])*(vec2[0]);

 result[0] = x;
 result[1] = y;
 result[2] = z;

}



template<class T = std::string>
class FExtensionV1N1{

public:
 //CONSTRUCTORS
 FExtensionV1N1(); //empty constructors
 FExtensionV1N1(const T &extv); //sets the ext to extv and adds a preceding period
 FExtensionV1N1(const FExtensionV1N1<T> &copy); //copy constructor

 //GET FUNCTIONS
 T getExt() const {return ext;}  //returns the ext with the preceding dot (default)
 T getExtWithoutPrecedingDot() const; //returns the ext without the default preceding dot 

 //SET FUNCTIONS
 void setExt(const T &extv);  //sets the ext to extv and adds a preceding period

 //CHECK FUNCTIONS
 bool isEmpty() const {return ((std::string(ext)).empty());}

 //OVERLOADED OPERATORS
 void operator=(const T &input) {this->setExt(input);}


protected:
 T ext;



};

template<class T>
FExtensionV1N1<T>::FExtensionV1N1()
//empty constructor
{

}

template<class T>
FExtensionV1N1<T>::FExtensionV1N1(const T &extv)
//sets the ext to extv and adds a preceding period
:ext(extv)
{
 hageman::addPrecedingDot(this->ext);  //adds a preceding period to the input string - independent of the OS
 
}

template<class T>
FExtensionV1N1<T>::FExtensionV1N1(const FExtensionV1N1<T> &copy)
//copy constructor
{
 ext = copy.getExt();

}

//GET FUNCTIONS
template<class T>
T FExtensionV1N1<T>::getExtWithoutPrecedingDot() const
//returns the ext without the default preceding dot 
{
 return (hageman::removedPrecedingDotFromExtension(this->ext));
}

//SET FUNCTIONS
template<class T>
void FExtensionV1N1<T>::setExt(const T &extv)
//sets the ext to extv and adds a preceding period
{

 this->ext = extv;
 hageman::removePrecedingWhitespaces(ext);
 hageman::removeTrailingWhitespaces(ext);
 hageman::addPrecedingDot(this->ext);  //adds a preceding period to the input string - independent of the OS

 
}


//OVERLOADED OPERATORS
template<class T>
std::ostream& operator<<(std::ostream &os, const FExtensionV1N1<T> &output)
{
 os << output.getExt();
 return os;

}

template<class T = std::string>
class FNameV1N1{

public:
 //CONSTRUCTORS
 FNameV1N1(); //empty default constructor
 FNameV1N1(const T &fnamev); //sets the fname to fnamev
 FNameV1N1(const FNameV1N1<T> &copy); //copy constructor

 //GET FUNCTIONS
 T getFName() const {return fname;}  //gets the fname
 T& getNonConstFName() {return fname;}

 //SET FUNCTIONS
 void setFName(const T &fnamev) {fname = fnamev;}  //set the fname to fnamev

 //CHECK FUNCTIONS
 bool isEmpty() const {return ((std::string(fname)).empty());}

 //OVERLOADED OPERATORS
 void operator=(const T &input) {fname = input;}

protected:
 T fname;


};

template<class T>
FNameV1N1<T>::FNameV1N1()
//empty default constructor
{

}

template<class T>
FNameV1N1<T>::FNameV1N1(const T &fnamev)
//sets the fname to fnamev
:fname(fnamev)
{
 
}

template<class T>
FNameV1N1<T>::FNameV1N1(const FNameV1N1<T> &copy)
//copy constructor
{
 fname = copy.getFName();

}

template<class T>
std::ostream& operator<<(std::ostream &os, const FNameV1N1<T> &output)
{
 os << output.getFName();
 return os;

}

template<class T = std::string>
class FPathV1N1{

public:
 //CONSTRUCTORS
 FPathV1N1(); //empty constructors
 FPathV1N1(const T &pathv); //sets the path to pathv and adds a terminating backslash
 FPathV1N1(const FPathV1N1 &copy); //copy constructor

 //GET FUNCTIONS
 T getPath() const {return path;}  //returns the path with the terminating backslash (default)
 T getPathWithoutTerminatingBackslash() const {return (this->removeTerminatingBackslash());}  //returns the path w/o the default terminating backslash

 //SET FUNCTIONS
 void setPath(const T &pathv);  //sets the path to pathv and adds a terminating backslash
 
 //CHECK FUNCTIONS
 bool isEmpty() const {return ((std::string(path)).empty());}

 //OVERLOADED OPERATORS
 void operator=(const T &input) {this->setPath(input);}

 
protected:
 T path;



};

//CONSTRUCTORS

template<class T>
FPathV1N1<T>::FPathV1N1()
//empty default constructor
{

}

template<class T>
FPathV1N1<T>::FPathV1N1(const T &pathv)
:path(pathv)
//sets the path to pathv and adds a terminating backslash
{
 hageman::addTerminatingBackslashToDirectoryString(this->path);  //adds a terminating backslash depending on the OS
 
}
 
template<class T>
FPathV1N1<T>::FPathV1N1(const FPathV1N1 &copy) 
//copy constructor
{
 path = copy.getPath();

}

template<class T>
void FPathV1N1<T>::setPath(const T &pathv)
//sets the path to pathv and adds a terminating backslash
{
 this->path = pathv;
 hageman::addTerminatingBackslashToDirectoryString(this->path);  //adds a terminating backslash depending on the OS
 
 
}


//OVERLOADED OPERATORS
template<class T>
std::ostream& operator<<(std::ostream &os, const FPathV1N1<T> &output)
{
 os << output.getPath();
 return os;

}


template<class T = std::string>
class FileNameV1N1{

public:
 //CONSTRUCTORS
 FileNameV1N1(); //empty default constructor
 FileNameV1N1(const T &input);  //constructs the object, parsing an arbitrary input (this base class constructor is empty)
 FileNameV1N1(const FileNameV1N1<T> *ptr);  //copy constructor
 FileNameV1N1(const FileNameV1N1<T> &copy);  //copy constructor
 virtual FileNameV1N1<T>* clone() const {return (new FileNameV1N1<T>(*this));}  //virtual copy constructor; reimplemented in derived classes using covariant return types
 
 //GET FUNCTIONS
 virtual T getOSCompatibleFileName() const;  //returns a filename that is compatible with the specific OS.  This is a virtual function meant to be polymorhpically called by the 
 //derived classes, but it will return all the three concatenated components of the filename
 T getPathPlusFileNamePlusExtension() const;  //returns the full filename
 T getPathPlusFileName() const; //returns just the path + filename 
 T getFileNamePlusExtension() const;  //returns just the filename + extension. 
 T getPath() const {return (path.getPath());}  //stripes off the filename and returns just the path.
 T getFileName() const {return (filename.getFName());}  //returns just the filename
 T getExtension() const {return (extension.getExt());}  //returns just the extension
 FPathV1N1<>* getPathPtr() {return (&path);}
 
 //SET FUNCTIONS
 void setPath(const T &ptemp) {(this->path).setPath(ptemp);}
 void setPath(const FPathV1N1<T> *pathptr) {(this->path) = (*pathptr);}
 void setExtension(const T& etemp) {(this->extension).setExt(etemp);}
 void stripPath() {(this->path).setPath("");}
 void addToFName(const T &addstr);  //adds an addendum onto the filename string

 //CHECK FUNCTIONS
 bool hasPath() const; //checks whether this object has a non-empty path
 bool hasFileName() const; //checks whether this object has a non-empty filename
 bool hasExtension() const; //checks whether this object has a non-empty extension

 //PARSING FUNCTIONS
 virtual void parseInput(const T &input);  //parses an arbitrary fname input
 virtual void parsePath(const T &input); //parsing input as a path
 virtual void parseFileName(const T &input); //parsing input as a filename
 virtual void parseExtension(const T &input); //parsing input as a extension

 //PRINT FUNCTIONS
 void printCL() const;  //prints the 3 components on the CL

 //OVERLOADED OPERATORS
 void operator=(const FileNameV1N1<T> &input); //overloaded assignment operator that copies the components uses its overloaded assignment operator 

protected:
 FPathV1N1<T> path; 
 FNameV1N1<T> filename;
 FExtensionV1N1<T> extension;
 

};

//CONSTRUCTOR

template<class T>
FileNameV1N1<T>::FileNameV1N1()
//empty default constructor
{
#if VALIDATE
 std::cout<<"This is a FileName constructor/initalizer"<<std::endl;
#endif

}

template<class T>
FileNameV1N1<T>::FileNameV1N1(const T &input)  
//constructs the object, parsing an arbitrary input (this base class constructor is empty)
{

#if VALIDATE
 std::cout<<"This is a FileName constructor/initalizer"<<std::endl;
#endif


}

 
template<class T>
FileNameV1N1<T>::FileNameV1N1(const FileNameV1N1<T> *ptr)
//copy constructor
{
 (*this) = (*ptr);  //uses the overloaded assignment operator to copy the objects

#if VALIDATE
 std::cout<<"This is a FileName constructor/initalizer"<<std::endl;
#endif


}
 
template<class T>
FileNameV1N1<T>::FileNameV1N1(const FileNameV1N1<T> &copy)
//copy constructor
{
 (*this) = copy;  //uses the overloaded assignment operator to copy the objects

#if VALIDATE
 std::cout<<"This is a FileName constructor/initalizer"<<std::endl;
#endif


}


//GET FUNCTIONS
template<class T>
T FileNameV1N1<T>::getPathPlusFileNamePlusExtension() const
//returns the full filename
{
 std::string result;

 if(this->hasPath())
  result = std::string(path.getPath());
 if(this->hasFileName())
  result += std::string(filename.getFName());
 if(this->hasExtension())
  result += std::string(extension.getExt());

 return (static_cast<T>(result));



}

template<class T>
T FileNameV1N1<T>::getOSCompatibleFileName() const
//returns a filename that is compatible with the specific OS.  This is a virtual function meant to be polymorhpically called by the derived classes, but it will return all the 
//three concatenated components of the filename
{

 return (this->getPathPlusFileNamePlusExtension());

}
 

template<class T>
T FileNameV1N1<T>::getFileNamePlusExtension() const
//returns just the filename + extension. 
{
 std::string result;
 
 if(this->hasFileName())
  result = std::string(filename.getFName());
 if(this->hasExtension())
  result += std::string(extension.getExt());

 return (static_cast<T>(result));


}


template<class T>
T FileNameV1N1<T>::getPathPlusFileName() const
//returns just the path + filename 
{
 std::string result;

 if(this->hasPath())
  result = std::string(path.getPathWithTerminatingBackslash());
 if(this->hasFileName())
  result += std::string(filename.getFName());


 return (static_cast<T>(result));

}

//SET FUNCTIONS
template<class T>
void FileNameV1N1<T>::addToFName(const T &addstr)
//adds an addendum onto the filename string
{

 ((this->filename).getNonConstFName()) += addstr;
}



//CHECK FUNCTIONS
template<class T>
bool FileNameV1N1<T>::hasPath() const
{
 if(!(path.isEmpty()))
  return true;
 
 return false;
  
}

template<class T>
bool FileNameV1N1<T>::hasFileName() const
{
 if(!(filename.isEmpty()))
  return true;
 
 return false;
 
}

template<class T>
bool FileNameV1N1<T>::hasExtension() const
{
 if(!(extension.isEmpty()))
  return true;
 
 return false;
  
}



//PARSING FUNCTIONS

template<class T>
void FileNameV1N1<T>::parseInput(const T &input)  
//parses an arbitrary fname input
{

#if ERRORCHECK
 std::cout<<"ERROR in void FileNameV1N1<T>::parseInput(const T &input)"<<std::endl;
 std::cout<<"This is the base class function and is not meant to be called."<<std::endl;
 exit(1);
#endif

 
}



template<class T>
void FileNameV1N1<T>::parsePath(const T &input)
//parsing input as a path
{
#if ERRORCHECK
 std::cout<<"ERROR in void FileNameV1N1<T>::parsePath(const T &input)"<<std::endl;
 std::cout<<"This is the base class function and is not meant to be called."<<std::endl;
 exit(1);
#endif

}

template<class T>
void FileNameV1N1<T>::parseFileName(const T &input) 
//parsing input as a filename
{

#if ERRORCHECK
 std::cout<<"ERROR in void FileNameV1N1<T>::parseFileName(const T &input)"<<std::endl;
 std::cout<<"This is the base class function and is not meant to be called."<<std::endl;
 exit(1);
#endif
 
}

template<class T>
void FileNameV1N1<T>::parseExtension(const T &input) 
//parsing input as a extension
{

 int startidx = (std::string(input)).size();  //init to end of input in case a period is not found; that way no assignment to extension is made
 if((std::string(input)).rfind(".") != std::string::npos)
 {
  startidx = (std::string(input)).rfind(".");
 }

 std::string exttemp(std::string(input).substr(startidx));

#if ERRORCHECK
 if(exttemp.empty())
 {
  std::cout<<"WARNING in void FileNameV1N1<T>::parseExtension(const T &input)"<<std::endl;
  std::cout<<"There is no extension to parse."<<std::endl;
 }
#endif

 (this->extension).setExt(exttemp);

 
}

//PRINT FUNCTIONS
template<class T>
void FileNameV1N1<T>::printCL() const
//prints the 3 components on the CL
{

 std::cout<<"Path:"<<std::endl;
 std::cout<<this->getPath()<<std::endl;
 std::cout<<"Filename:"<<std::endl;
 std::cout<<this->getFileName()<<std::endl;
 std::cout<<"Extension:"<<std::endl;
 std::cout<<this->getExtension()<<std::endl;



}

//OVERLOADED OPERATORS
template<class T>
void FileNameV1N1<T>::operator=(const FileNameV1N1<T> &input)
//overloaded assignment operator that copies the components uses its overloaded assignment operator 
{

 //uses the overloaded assignment operators for each of the 3 component objects
 this->path = input.getPath();
 this->filename = input.getFileName();
 this->extension = input.getExtension();

#if VALIDATE
 std::cout<<"This is a FileName constructor/initalizer"<<std::endl;
#endif


}

template<class T>
std::ostream& operator<<(std::ostream &os, FileNameV1N1<T> &output)
{
 os << output.getPathPlusFileNamePlusExtension() << "\n";

 return os;

}


template<class T = std::string>
class WinFileNameV1N1 : public FileNameV1N1<T>{

public:
 WinFileNameV1N1(); //empty default constructor
 WinFileNameV1N1(const T &input);  //constructs the object, parsing an arbitrary input
 WinFileNameV1N1(const WinFileNameV1N1<T> *ptr);  //copy constructor
 WinFileNameV1N1(const WinFileNameV1N1<T> &copy);  //copy constructor

 virtual WinFileNameV1N1<T>* clone() const {return (new WinFileNameV1N1<T>(*this));}

 //GET FUNCTIONS
 virtual T getOSCompatibleFileName() const;  //returns the filename in a format that can be read by the Windows OS
 
 //PARSING FUNCTIONS
 virtual void parseInput(const T &input);  //parses an arbitrary fname input
 virtual void parsePath(const T &input); //parsing input as a path
 virtual void parseFileName(const T &input); //parsing input as a filename
 virtual void parseExtension(const T &input); //parsing input as a extension


protected:


};

template<class T>
WinFileNameV1N1<T>::WinFileNameV1N1()
//empty default constructor
:FileNameV1N1<T>()
{

#if VALIDATE
 std::cout<<"This is a WinFileName constructor/initalizer"<<std::endl;
#endif


}

template<class T>
WinFileNameV1N1<T>::WinFileNameV1N1(const T &input)
//constructs the object, parsing an arbitrary input
:FileNameV1N1<T>(input)
{

#if VALIDATE
 std::cout<<"From function WinFileNameV1N1<T>::WinFileNameV1N1(const T &input)"<<std::endl;
 std::cout<<"input: "<<input<<std::endl;
#endif

 this->parseInput(input);

 
#if VALIDATE
 std::cout<<"This is a WinFileName constructor/initalizer"<<std::endl;
#endif


}


template<class T>
WinFileNameV1N1<T>::WinFileNameV1N1(const WinFileNameV1N1<T> *ptr)
//constructs the object, parsing an arbitrary input
:FileNameV1N1<T>(ptr)
{

 
#if VALIDATE
 std::cout<<"This is a WinFileName constructor/initalizer"<<std::endl;
#endif



}

 
template<class T>
WinFileNameV1N1<T>::WinFileNameV1N1(const WinFileNameV1N1<T> &copy)
//copy constructor
:FileNameV1N1<T>(copy)
{

 
#if VALIDATE
 std::cout<<"This is a WinFileName constructor/initalizer"<<std::endl;
#endif

 
}



//GET FUNCTIONS
template<class T>
T WinFileNameV1N1<T>::getOSCompatibleFileName() const 
//returns the filename in a format that can be read by the Windows OS
{
#if ERRORCHECK
 //makes sure that the filename has at least a filename to return.  Filenames are assumed to be able to have neither path nor extension
 if(!(this->hasFileName()))
 {
  std::cout<<"ERROR in T WinFileNameV1N1<T>::getOSCompatibleFileName()"<<std::endl;
  std::cout<<"FileName = "<<this->getPathPlusFileNamePlusExtension()<<" is not in a format that can be read by the Win OS"<<std::endl;
  exit(1);
 }
#endif
   
 return (this->getPathPlusFileNamePlusExtension());


}

//PARSING FUNCTIONS
template<class T>
void WinFileNameV1N1<T>::parseInput(const T &input)  
//parses an arbitrary fname input
{
 this->parsePath(input);
 this->parseFileName(input);
 this->parseExtension(input);

}


template<class T>
void WinFileNameV1N1<T>::parsePath(const T &input)
//parsing input as a path
{

 int stopidx = 0;  //init to the end of the string in case a . is not found.
 if((std::string(input)).rfind("\\") != std::string::npos)
 {
  stopidx = (std::string(input)).rfind("\\");
 }

 std::string pathtemp(std::string(input).substr(0, stopidx));

#if ERRORCHECK
 if(pathtemp.empty())
 {
  std::cout<<"WARNING in void WinFileNameV1N1<T>::parsePath(const T &input)"<<std::endl;
  std::cout<<"There is no path to parse."<<std::endl;
 }
#endif

 (this->path).setPath(pathtemp);


 
}

template<class T>
void WinFileNameV1N1<T>::parseFileName(const T &input) 
//parses out the filename from the input
{

 int startidx = 0;  //init to 0 in case a backslash is not found
 if((std::string(input)).rfind("\\") != std::string::npos)
 {
  startidx = (std::string(input)).rfind("\\");
  startidx++;
 }

 int stopidx = (std::string(input)).size();  //init to the end of the string in case a . is not found.
 if((std::string(input)).rfind(".") != std::string::npos)
 {
  stopidx = (std::string(input)).rfind(".");
 }

 std::string fnametemp(std::string(input).substr(startidx, stopidx-startidx));

 (this->filename).setFName(fnametemp);


}

template<class T>
void WinFileNameV1N1<T>::parseExtension(const T &input) 
//parsing input as a extension
{
 FileNameV1N1<T>::parseExtension(input);

}

template<class T = std::string>
class MacFileNameV1N1 : public FileNameV1N1<T>{

public:
 MacFileNameV1N1(); //empty constructors
 MacFileNameV1N1(const T &input);  //constructs the object, parsing an arbitrary input
 MacFileNameV1N1(const MacFileNameV1N1<T> *ptr);  //copy constructor
 MacFileNameV1N1(const MacFileNameV1N1<T> &copy);  //copy constructor

 virtual MacFileNameV1N1<T>* clone() const {return (new MacFileNameV1N1<T>(*this));}

 //GET FUNCTIONS
 virtual T getOSCompatibleFileName() const;  //returns the filename in a format that can be read by the Windows OS
 //virtual T getOSCompatiblePath() const {return (this->getPath());}
 //virtual T getOSCompatibleExtension() const {return (this->getExtension());}


 //PARSING FUNCTIONS
 virtual void parseInput(const T &input);  //parses an arbitrary fname input
 
 //virtual void parseArbitraryInput(const T &input);  //parses an arbitrary fname input
 //virtual void parseFullInput(const T &input);  //parsing input into its three components: path, filename, and extension.
 //virtual void parsePathFilename(const T &input);  //parses input into path and filename
 //virtual void parseFilenameExtension(const T &input);  //parses input into filename and extension
 //void parsePathExtension(const T &input);  //parses input into path and extension TO BE IMPLEMENTED
 virtual void parsePath(const T &input); //parsing input as a path
 virtual void parseFileName(const T &input); //parsing input as a filename
 virtual void parseExtension(const T &input); //parsing input as a extension


protected:


};

template<class T>
MacFileNameV1N1<T>::MacFileNameV1N1()
//empty default constructor
:FileNameV1N1<T>()
{

}

template<class T>
MacFileNameV1N1<T>::MacFileNameV1N1(const T &input)
//constructs the object, parsing an arbitrary input
:FileNameV1N1<T>(input)
{

#if VALIDATE
 std::cout<<"From function MacFileNameV1N1<T>::MacFileNameV1N1(const T &input)"<<std::endl;
 std::cout<<"input: "<<input<<std::endl;
#endif

 this->parseInput(input);

}


template<class T>
MacFileNameV1N1<T>::MacFileNameV1N1(const MacFileNameV1N1<T> *ptr)
//constructs the object, parsing an arbitrary input
:FileNameV1N1<T>(ptr)
{

}

template<class T>
MacFileNameV1N1<T>::MacFileNameV1N1(const MacFileNameV1N1<T> &copy)
//copy constructor
:FileNameV1N1<T>(copy)
{
 
}



//GET FUNCTIONS
template<class T>
T MacFileNameV1N1<T>::getOSCompatibleFileName() const 
//returns the filename in a format that can be read by the Macdows OS
{
#if ERRORCHECK
 //makes sure that the filename has at least a filename to return.  Filenames are assumed to be able to have neither path nor extension
 if(!(this->hasFileName()))
 {
  std::cout<<"ERROR in T MacFileNameV1N1<T>::getOSCompatibleFileName()"<<std::endl;
  std::cout<<"FileName = "<<this->getPathPlusFileNamePlusExtension()<<" is not in a format that can be read by the Mac OS"<<std::endl;
  exit(1);
 }
#endif
   
 return (this->getPathPlusFileNamePlusExtension());


}

//PARSING FUNCTIONS
template<class T>
void MacFileNameV1N1<T>::parseInput(const T &input)  
//parses an arbitrary fname input
{

 this->parsePath(input);
 this->parseFileName(input);
 this->parseExtension(input);

}


template<class T>
void MacFileNameV1N1<T>::parsePath(const T &input)
//parsing input as a path
{
 int stopidx = 0;  //init to the end of the string in case a . is not found.
 if((std::string(input)).rfind("/") != std::string::npos)
 {
  stopidx = (std::string(input)).rfind("/");
 }

 std::string pathtemp(std::string(input).substr(0, stopidx));

#if ERRORCHECK
 if(pathtemp.empty())
 {
  std::cout<<"WARNING in void MacFileNameV1N1<T>::parsePath(const T &input)"<<std::endl;
  std::cout<<"There is no path to parse."<<std::endl;
 }
#endif

 (this->path).setPath(pathtemp);
}

template<class T>
void MacFileNameV1N1<T>::parseFileName(const T &input) 
//parsing input as a filename
{

 int startidx = 0;  //init to 0 in case a backslash is not found
 if((std::string(input)).rfind("/") != std::string::npos)
 {
  startidx = (std::string(input)).rfind("/");
  startidx++;
 }

 int stopidx = (std::string(input)).size();  //init to the end of the string in case a . is not found.
 if((std::string(input)).rfind(".") != std::string::npos)
 {
  stopidx = (std::string(input)).rfind(".");
 }

 std::string fnametemp(std::string(input).substr(startidx, stopidx-startidx));

 (this->filename).setFName(fnametemp);
 

}

template<class T>
void MacFileNameV1N1<T>::parseExtension(const T &input) 
//parsing input as a extension
{
 FileNameV1N1<T>::parseExtension(input);
}

template<class T>
std::string getCurrentDirectory()
//gets the current directory by writing the output of chdir (Win) or pwd (Mac) to a txt file and then reads it.  this may not work on the Pipeline system b/c of the problem with writing
//to cache
{
 FileNameV1N1<> *fptr = NULL;
 std::string path;
 char readbuffer[_CVTBUFSIZE];
 int result;

 //this uses the time class to create a unique filename based on the system time.  This prevents any possible multiple read/write errors 
 char timebuffer[_CVTBUFSIZE];
 time_t rawtime;
 struct tm* timeinfo;
 time(&rawtime);
 timeinfo = localtime(&rawtime);
 strftime(timebuffer,_CVTBUFSIZE,"%M%S",timeinfo);
 std::string ofname("currentdir_");
 ofname += std::string(timebuffer);
 ofname += ".txt";
  
 if(hageman::getOSType() == hageman::WIN)
 {
  std::string cmd("chdir > ");  //chdir displays the current path w/o terminating backslash
  cmd += ofname;

#if VALIDATE
  std::cout<<"From function std::string getCurrentDirectory()"<<std::endl;
  std::cout<<"cmd: "<<cmd<<std::endl;
#endif

  result = system(cmd.c_str());
  fptr = new WinFileNameV1N1<>(ofname);
  std::ifstream readfilestream(((fptr->getOSCompatibleFileName()).c_str()), std::ios::in);
    
  while((path.empty()) && (!(readfilestream.eof())))  //keeps reading until the EOF or a valid line has been reached
  {
   readfilestream.getline(readbuffer, _CVTBUFSIZE);
   path = std::string(readbuffer);
  }
 
  readfilestream.close();
  
  hageman::removeTrailingWhitespaces(path);
  hageman::removePrecedingWhitespaces(path);
  hageman::addTerminatingBackslashToDirectoryString(path);

#if VALIDATE
  std::cout<<"From function std::string getCurrentDirectory()"<<std::endl;
  std::cout<<"path: "<<path<<std::endl;
#endif


  delete fptr;
  
  //deletes the txt file that stores the current dir (THIS DOESNT WORK FOR SOME REASON B/C THE FILE GETS OPENED AND THE PROGRAM PAUSES)
  //cmd = "erase ";
  //cmd += ofname;
  //result = system(ofname.c_str());
 }
 else if(hageman::getOSType() == hageman::OSX)
 {
  std::string cmd("pwd > ./");
  cmd += ofname;
  system(cmd.c_str());
  fptr = new MacFileNameV1N1<>(ofname);
 
  std::ifstream readfilestream(((fptr->getOSCompatibleFileName()).c_str()), std::ios::in);
    
  while((path.empty()) && (!(readfilestream.eof())))  //keeps reading until the EOF or a valid line has been reached
  {
   readfilestream.getline(readbuffer, _CVTBUFSIZE);
   path = std::string(readbuffer);
  }
 
  readfilestream.close();
  hageman::removeTrailingWhitespaces(path);
  hageman::removePrecedingWhitespaces(path);
  hageman::addTerminatingBackslashToDirectoryString(path);
  
  delete fptr;
  
  //deletes the txt file that stores the current dir
  //cmd = "rm ./";
  //cmd += ofname;
  //system(cmd.c_str());

 }

#if VALIDATE
  std::cout<<"From function std::string getCurrentDirectory()"<<std::endl;
  std::cout<<"Current Directory: "<<path<<std::endl;
#endif


 return path;

 
}

//sets the current directory to a global variable
#if PIPELINE  //the pipeline doesn't support writing and then reading a temp file to the current directory.  If the hageman::getCurrentDirectory<std::string>() function is called, then
//pipeline module will stall indefinitely
 const std::string currentdirectory;  //empty directory
#else
 const std::string currentdirectory = hageman::getCurrentDirectory<std::string>();
#endif

template<class T>
FileNameV1N1<T>* constructEmptyFileNameObject(T dumval)
{

 FileNameV1N1<T>* fnameptr = NULL;

 if(hageman::getOSType() == hageman::WIN)
 {
  #if VALIDATE
   std::cout<<"From function FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)"<<std::endl;
   std::cout<<"This is a Windows operating system"<<std::endl; 
  #endif

  fnameptr = new WinFileNameV1N1<T>();
 }
 else if(hageman::getOSType() == hageman::OSX)
 {
  #if VALIDATE
   std::cout<<"From function FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)"<<std::endl;
   std::cout<<"This is a OSX operating system"<<std::endl; 
  #endif

  fnameptr = new MacFileNameV1N1<T>();
 }

 return fnameptr;



}


template<class T>
FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)
//constructs a FileNameV1N1 object from a string and returns a pointer to it.  This function WILL allocate dynamic memory to the FileNameV1N1 ptr; remember to delete this pointer after
//the function exits
{

 #if VALIDATE
  std::cout<<"From function FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)"<<std::endl;
  std::cout<<"Filename Str: "<<fnamestr<<std::endl;
 #endif

 FileNameV1N1<T>* fnameptr = NULL;

 if(hageman::getOSType() == hageman::WIN)
 {
  #if VALIDATE
   std::cout<<"From function FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)"<<std::endl;
   std::cout<<"This is a Windows operating system"<<std::endl; 
  #endif

  fnameptr = new WinFileNameV1N1<T>(fnamestr);
 }
 else if(hageman::getOSType() == hageman::OSX)
 {
  #if VALIDATE
   std::cout<<"From function FileNameV1N1<T>* constructFileNameObjFromString(const T &fnamestr)"<<std::endl;
   std::cout<<"This is a OSX operating system"<<std::endl; 
  #endif

  fnameptr = new MacFileNameV1N1<T>(fnamestr);
 }

 return fnameptr;

}


template<class T>
void changeMissingPathToCurrentDirectory(FileNameV1N1<T> *fptr, std::string pathoverride = "")
//looks to see if the path for a FileNameV1N1 object is empty.  if so, then set the path to the current value of currentdirectory.  Provides an option to override the currentdirectory with
//a different path
{
 if(!fptr->hasPath())
 {
  if(pathoverride.empty())  //if pathoverride is empty, then set to currentdirectory
  {
   fptr->setPath(hageman::currentdirectory);
  }
  else
  {
   fptr->setPath(pathoverride);
  }
 }

#if VALIDATE
 std::cout<<"From function void changeMissingPathToCurrentDirectory(FileNameV1N1<T> *fptr, std::string pathoverride = "")"<<std::endl;
 std::cout<<"New fptr: "<<fptr->getOSCompatibleFileName()<<std::endl;
#endif

  
}


template<class T>
void convertStringVectorToFileNamePtrList(const std::vector<T> &strlist, std::vector<FileNameV1N1<T>* > &fptrlist, std::string addpath = "")
//convert a vector of strings to a list of FileNameV1N1 ptrs
{
 OSType ostype = getOSType();

 FileNameV1N1<T> *fptr;

 for(int idx = 0; idx < strlist.size(); idx++)
 {
  if(ostype == hageman::WIN)
  {
   fptr = new WinFileNameV1N1<>(strlist.at(idx));
   if(addpath.empty())
    hageman::changeMissingPathToCurrentDirectory(fptr);  //makes sure that any empty paths are filled in with the current directory
   else
    hageman::changeMissingPathToCurrentDirectory(fptr,addpath);

   fptrlist.push_back(fptr);
  }
  else if(ostype == hageman::OSX)
  {
   fptr = new MacFileNameV1N1<>(strlist.at(idx));
   if(addpath.empty())
    hageman::changeMissingPathToCurrentDirectory(fptr);  //makes sure that any empty paths are filled in with the current directory
   else
    hageman::changeMissingPathToCurrentDirectory(fptr,addpath);

   fptrlist.push_back(fptr);
  }
 }

 

}


template<class T>
inline T SQR(const T a) {return a*a;}

template<class T>
inline const T &MAX(const T &a, const T &b)
        {return b > a ? (b) : (a);}

inline float MAX(const double &a, const float &b)
        {return b > a ? (b) : float(a);}

inline float MAX(const float &a, const double &b)
        {return b > a ? float(b) : (a);}

template<class T>
inline const T &MIN(const T &a, const T &b)
        {return b < a ? (b) : (a);}

inline float MIN(const double &a, const float &b)
        {return b < a ? (b) : float(a);}

inline float MIN(const float &a, const double &b)
        {return b < a ? float(b) : (a);}

template<class T>
inline T SIGN(const T &a, const T &b)
 {return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const float &a, const double &b)
 {return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const double &a, const float &b)
 {return (float)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}

template<class T>
inline void SWAP(T &a, T &b)
 {T dum=a; a=b; b=dum;}

// exception handling

#ifndef _USENRERRORCLASS_
#define throw(message) \
{printf("ERROR: %s\n     in file %s at line %d\n", message,__FILE__,__LINE__); throw(1);}
#else
struct NRerror {
 char *message;
 char *file;
 int line;
 NRerror(char *m, char *f, int l) : message(m), file(f), line(l) {}
};
#define throw(message) throw(NRerror(message,__FILE__,__LINE__));
void NRcatch(NRerror err) {
 printf("ERROR: %s\n     in file %s at line %d\n",
  err.message, err.file, err.line);
 exit(1);
}
#endif

// usage example:
//
// try {
//  somebadroutine();
// }
// catch(NRerror s) {NRcatch(s);}
//
// (You can of course substitute any other catch body for NRcatch(s).)


// Vector and Matrix Classes

#ifdef _USESTDVECTOR_
#define NRvector vector
#else

template <class T>
class NRvector {
private:
 int nn; // size of array. upper index is nn-1
 T *v;
public:
 NRvector();
 explicit NRvector(int n);  // Zero-based array
 NRvector(int n, const T &a); //initialize to constant value
 NRvector(int n, const T *a); // Initialize to array
 NRvector(const NRvector &rhs); // Copy constructor
 NRvector & operator=(const NRvector &rhs); //assignment
 typedef T value_type; // make T available externally
 inline T & operator[](const int i); //i'th element
 inline const T & operator[](const int i) const;
 inline int size() const;
 void resize(int newn); // resize (contents not preserved)
 void assign(int newn, const T &a); // resize and assign a constant value
 ~NRvector();
};

// NRvector definitions

template <class T>
NRvector<T>::NRvector() : nn(0), v(NULL) {}

template <class T>
NRvector<T>::NRvector(int n) : nn(n), v(n>0 ? new T[n] : NULL) {}

template <class T>
NRvector<T>::NRvector(int n, const T& a) : nn(n), v(n>0 ? new T[n] : NULL)
{
 for(int i=0; i<n; i++) v[i] = a;
}

template <class T>
NRvector<T>::NRvector(int n, const T *a) : nn(n), v(n>0 ? new T[n] : NULL)
{
 for(int i=0; i<n; i++) v[i] = *a++;
}

template <class T>
NRvector<T>::NRvector(const NRvector<T> &rhs) : nn(rhs.nn), v(nn>0 ? new T[nn] : NULL)
{
 for(int i=0; i<nn; i++) v[i] = rhs[i];
}

template <class T>
NRvector<T> & NRvector<T>::operator=(const NRvector<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//  if vector and rhs were different sizes, vector
//  has been resized to match the size of rhs
{
 if (this != &rhs)
 {
  if (nn != rhs.nn) {
   if (v != NULL) delete [] (v);
   nn=rhs.nn;
   v= nn>0 ? new T[nn] : NULL;
  }
  for (int i=0; i<nn; i++)
   v[i]=rhs[i];
 }
 return *this;
}

template <class T>
inline T & NRvector<T>::operator[](const int i) //subscripting
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
 throw("NRvector subscript out of bounds");
}
#endif
 return v[i];
}

template <class T>
inline const T & NRvector<T>::operator[](const int i) const //subscripting
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
 throw("NRvector subscript out of bounds");
}
#endif
 return v[i];
}

template <class T>
inline int NRvector<T>::size() const
{
 return nn;
}

template <class T>
void NRvector<T>::resize(int newn)
{
 if (newn != nn) {
  if (v != NULL) delete[] (v);
  nn = newn;
  v = nn > 0 ? new T[nn] : NULL;
 }
}

template <class T>
void NRvector<T>::assign(int newn, const T& a)
{
 if (newn != nn) {
  if (v != NULL) delete[] (v);
  nn = newn;
  v = nn > 0 ? new T[nn] : NULL;
 }
 for (int i=0;i<nn;i++) v[i] = a;
}

template <class T>
NRvector<T>::~NRvector()
{
 if (v != NULL) delete[] (v);
}

// end of NRvector definitions

#endif //ifdef _USESTDVECTOR_

template <class T>
class NRmatrix {
private:
 int nn;
 int mm;
 T **v;
public:
 NRmatrix();
 NRmatrix(int n, int m);   // Zero-based array
 NRmatrix(int n, int m, const T &a); //Initialize to constant
 NRmatrix(int n, int m, const T *a); // Initialize to array
 NRmatrix(const NRmatrix &rhs);  // Copy constructor
 NRmatrix & operator=(const NRmatrix &rhs); //assignment
 typedef T value_type; // make T available externally
 inline T* operator[](const int i); //subscripting: pointer to row i
 inline const T* operator[](const int i) const;
 inline int nrows() const;
 inline int ncols() const;
 void resize(int newn, int newm); // resize (contents not preserved)
 void assign(int newn, int newm, const T &a); // resize and assign a constant value
 ~NRmatrix();
};

template <class T>
NRmatrix<T>::NRmatrix() : nn(0), mm(0), v(NULL) {}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m) : nn(n), mm(m), v(n>0 ? new T*[n] : NULL)
{
 int i,nel=m*n;
 if (v) v[0] = nel>0 ? new T[nel] : NULL;
 for (i=1;i<n;i++) v[i] = v[i-1] + m;
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T &a) : nn(n), mm(m), v(n>0 ? new T*[n] : NULL)
{
 int i,j,nel=m*n;
 if (v) v[0] = nel>0 ? new T[nel] : NULL;
 for (i=1; i< n; i++) v[i] = v[i-1] + m;
 for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = a;
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T *a) : nn(n), mm(m), v(n>0 ? new T*[n] : NULL)
{
 int i,j,nel=m*n;
 if (v) v[0] = nel>0 ? new T[nel] : NULL;
 for (i=1; i< n; i++) v[i] = v[i-1] + m;
 for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = *a++;
}

template <class T>
NRmatrix<T>::NRmatrix(const NRmatrix &rhs) : nn(rhs.nn), mm(rhs.mm), v(nn>0 ? new T*[nn] : NULL)
{
 int i,j,nel=mm*nn;
 if (v) v[0] = nel>0 ? new T[nel] : NULL;
 for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
 for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
}

template <class T>
NRmatrix<T> & NRmatrix<T>::operator=(const NRmatrix<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//  if matrix and rhs were different sizes, matrix
//  has been resized to match the size of rhs
{
 if (this != &rhs) {
  int i,j,nel;
  if (nn != rhs.nn || mm != rhs.mm) {
   if (v != NULL) {
    delete[] (v[0]);
    delete[] (v);
   }
   nn=rhs.nn;
   mm=rhs.mm;
   v = nn>0 ? new T*[nn] : NULL;
   nel = mm*nn;
   if (v) v[0] = nel>0 ? new T[nel] : NULL;
   for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
  }
  for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
 }
 return *this;
}

template <class T>
inline T* NRmatrix<T>::operator[](const int i) //subscripting: pointer to row i
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
 throw("NRmatrix subscript out of bounds");
}
#endif
 return v[i];
}

template <class T>
inline const T* NRmatrix<T>::operator[](const int i) const
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
 throw("NRmatrix subscript out of bounds");
}
#endif
 return v[i];
}

template <class T>
inline int NRmatrix<T>::nrows() const
{
 return nn;
}

template <class T>
inline int NRmatrix<T>::ncols() const
{
 return mm;
}

template <class T>
void NRmatrix<T>::resize(int newn, int newm)
{
 int i,nel;
 if (newn != nn || newm != mm) {
  if (v != NULL) {
   delete[] (v[0]);
   delete[] (v);
  }
  nn = newn;
  mm = newm;
  v = nn>0 ? new T*[nn] : NULL;
  nel = mm*nn;
  if (v) v[0] = nel>0 ? new T[nel] : NULL;
  for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
 }
}

template <class T>
void NRmatrix<T>::assign(int newn, int newm, const T& a)
{
 int i,j,nel;
 if (newn != nn || newm != mm) {
  if (v != NULL) {
   delete[] (v[0]);
   delete[] (v);
  }
  nn = newn;
  mm = newm;
  v = nn>0 ? new T*[nn] : NULL;
  nel = mm*nn;
  if (v) v[0] = nel>0 ? new T[nel] : NULL;
  for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
 }
 for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = a;
}

template <class T>
NRmatrix<T>::~NRmatrix()
{
 if (v != NULL) {
  delete[] (v[0]);
  delete[] (v);
 }
}

template <class T>
class NRMat3d {
private:
 int nn;
 int mm;
 int kk;
 T ***v;
public:
 NRMat3d();
 NRMat3d(int n, int m, int k);
 inline T** operator[](const int i); //subscripting: pointer to row i
 inline const T* const * operator[](const int i) const;
 inline int dim1() const;
 inline int dim2() const;
 inline int dim3() const;
 ~NRMat3d();
};

template <class T>
NRMat3d<T>::NRMat3d(): nn(0), mm(0), kk(0), v(NULL) {}

template <class T>
NRMat3d<T>::NRMat3d(int n, int m, int k) : nn(n), mm(m), kk(k), v(new T**[n])
{
 int i,j;
 v[0] = new T*[n*m];
 v[0][0] = new T[n*m*k];
 for(j=1; j<m; j++) v[0][j] = v[0][j-1] + k;
 for(i=1; i<n; i++) {
  v[i] = v[i-1] + m;
  v[i][0] = v[i-1][0] + m*k;
  for(j=1; j<m; j++) v[i][j] = v[i][j-1] + k;
 }
}

template <class T>
inline T** NRMat3d<T>::operator[](const int i) //subscripting: pointer to row i
{
 return v[i];
}

template <class T>
inline const T* const * NRMat3d<T>::operator[](const int i) const
{
 return v[i];
}

template <class T>
inline int NRMat3d<T>::dim1() const
{
 return nn;
}

template <class T>
inline int NRMat3d<T>::dim2() const
{
 return mm;
}

template <class T>
inline int NRMat3d<T>::dim3() const
{
 return kk;
}

template <class T>
NRMat3d<T>::~NRMat3d()
{
 if (v != NULL) {
  delete[] (v[0][0]);
  delete[] (v[0]);
  delete[] (v);
 }
}

// basic type names (redefine if your bit lengths don't match)

typedef int Int; // 32 bit integer
typedef unsigned int Uint;

#ifdef _MSC_VER
typedef __int64 Llong; // 64 bit integer
typedef unsigned __int64 Ullong;
#else
typedef long long int Llong; // 64 bit integer
typedef unsigned long long int Ullong;
#endif

typedef char Char; // 8 bit integer
typedef unsigned char Uchar;

typedef double Doub; // default floating type
typedef long double Ldoub;

typedef complex<double> Complex; // default complex type

typedef bool Bool;

// NaN: uncomment one of the following 3 methods of defining a global NaN
// you can test by verifying that (NaN != NaN) is true

static const Doub NaN = numeric_limits<Doub>::quiet_NaN();

//Uint proto_nan[2]={0xffffffff, 0x7fffffff};
//double NaN = *( double* )proto_nan;

//Doub NaN = sqrt(-1.);

// vector types

typedef const NRvector<Int> VecInt_I;
typedef NRvector<Int> VecInt, VecInt_O, VecInt_IO;

typedef const NRvector<Uint> VecUint_I;
typedef NRvector<Uint> VecUint, VecUint_O, VecUint_IO;

typedef const NRvector<Llong> VecLlong_I;
typedef NRvector<Llong> VecLlong, VecLlong_O, VecLlong_IO;

typedef const NRvector<Ullong> VecUllong_I;
typedef NRvector<Ullong> VecUllong, VecUllong_O, VecUllong_IO;

typedef const NRvector<Char> VecChar_I;
typedef NRvector<Char> VecChar, VecChar_O, VecChar_IO;

typedef const NRvector<Char*> VecCharp_I;
typedef NRvector<Char*> VecCharp, VecCharp_O, VecCharp_IO;

typedef const NRvector<Uchar> VecUchar_I;
typedef NRvector<Uchar> VecUchar, VecUchar_O, VecUchar_IO;

typedef const NRvector<Doub> VecDoub_I;
typedef NRvector<Doub> VecDoub, VecDoub_O, VecDoub_IO;

typedef const NRvector<Doub*> VecDoubp_I;
typedef NRvector<Doub*> VecDoubp, VecDoubp_O, VecDoubp_IO;

typedef const NRvector<Complex> VecComplex_I;
typedef NRvector<Complex> VecComplex, VecComplex_O, VecComplex_IO;

typedef const NRvector<Bool> VecBool_I;
typedef NRvector<Bool> VecBool, VecBool_O, VecBool_IO;

// matrix types

typedef const NRmatrix<Int> MatInt_I;
typedef NRmatrix<Int> MatInt, MatInt_O, MatInt_IO;

typedef const NRmatrix<Uint> MatUint_I;
typedef NRmatrix<Uint> MatUint, MatUint_O, MatUint_IO;

typedef const NRmatrix<Llong> MatLlong_I;
typedef NRmatrix<Llong> MatLlong, MatLlong_O, MatLlong_IO;

typedef const NRmatrix<Ullong> MatUllong_I;
typedef NRmatrix<Ullong> MatUllong, MatUllong_O, MatUllong_IO;

typedef const NRmatrix<Char> MatChar_I;
typedef NRmatrix<Char> MatChar, MatChar_O, MatChar_IO;

typedef const NRmatrix<Uchar> MatUchar_I;
typedef NRmatrix<Uchar> MatUchar, MatUchar_O, MatUchar_IO;

typedef const NRmatrix<Doub> MatDoub_I;
typedef NRmatrix<Doub> MatDoub, MatDoub_O, MatDoub_IO;

typedef const NRmatrix<Bool> MatBool_I;
typedef NRmatrix<Bool> MatBool, MatBool_O, MatBool_IO;

// 3D matrix types

typedef const NRMat3d<Doub> Mat3DDoub_I;
typedef NRMat3d<Doub> Mat3DDoub, Mat3DDoub_O, Mat3DDoub_IO;

// Floating Point Exceptions for Microsoft compilers

#ifdef _TURNONFPES_
#ifdef _MSC_VER
struct turn_on_floating_exceptions {
 turn_on_floating_exceptions() {
  int cw = _controlfp( 0, 0 );
  cw &=~(EM_INVALID | EM_OVERFLOW | EM_ZERODIVIDE );
  _controlfp( cw, MCW_EM );
 }
};
turn_on_floating_exceptions yes_turn_on_floating_exceptions;
#endif /* _MSC_VER */
#endif /* _TURNONFPES */

// Define 32 bit signed and unsigned integers.
// Change these definitions, if necessary, to match a particular platform
#if defined(_WIN16) || defined(__MSDOS__) || defined(_MSDOS) 
   // 16 bit systems use long int for 32 bit integer
   typedef long int           int32;   // 32 bit signed integer
   typedef unsigned long int  uint32;  // 32 bit unsigned integer
#else
   // Most other systems use int for 32 bit integer
   typedef int                int32;   // 32 bit signed integer
   typedef unsigned int       uint32;  // 32 bit unsigned integer
#endif

// Define 64 bit signed and unsigned integers, if possible
#if (defined(__WINDOWS__) || defined(_WIN32)) && (defined(_MSC_VER) || defined(__INTEL_COMPILER))
   // Microsoft and other compilers under Windows use __int64
   typedef __int64            int64;   // 64 bit signed integer
   typedef unsigned __int64   uint64;  // 64 bit unsigned integer
   #define INT64_DEFINED               // Remember that int64 is defined
#elif defined(__unix__) && (defined(_M_IX86) || defined(_M_X64))
   // Gnu and other compilers under Linux etc. use long long
   typedef long long          int64;   // 64 bit signed integer
   typedef unsigned long long uint64;  // 64 bit unsigned integer
   #define INT64_DEFINED               // Remember that int64 is defined
#else
   // 64 bit integers not defined
   // You may include definitions for other platforms here
#endif


/***********************************************************************
System-specific user interface functions
***********************************************************************/

void EndOfProgram(void);               // System-specific exit code (userintf.cpp)

void FatalError(char * ErrorText);     // System-specific error reporting (userintf.cpp)


/***********************************************************************
Define random number generator classes
***********************************************************************/

class CRandomMersenne {                // Encapsulate random number generator
#if 0
   // Define constants for type MT11213A:
#define MERS_N   351
#define MERS_M   175
#define MERS_R   19
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   17
#define MERS_A   0xE4BD75F5
#define MERS_B   0x655E5280
#define MERS_C   0xFFD58000
#else    
   // or constants for type MT19937:
#define MERS_N   624
#define MERS_M   397
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000
#endif
public:
   CRandomMersenne(uint32 seed) {      // Constructor
      RandomInit(seed); LastInterval = 0;}
   void RandomInit(uint32 seed);       // Re-seed
   void RandomInitByArray(uint32 seeds[], int length); // Seed by more than 32 bits
   int IRandom (int min, int max);     // Output random integer
   int IRandomX(int min, int max);     // Output random integer, exact
   double Random();                    // Output random float
   uint32 BRandom();                   // Output random bits
private:
   void Init0(uint32 seed);            // Basic initialization procedure
   uint32 mt[MERS_N];                  // State vector
   int mti;                            // Index into mt
   uint32 LastInterval;                // Last interval length for IRandomX
   uint32 RLimit;                      // Rejection limit used by IRandomX
   enum TArch {LITTLE_ENDIAN1, BIG_ENDIAN1, NONIEEE}; // Definition of architecture
   TArch Architecture;                 // Conversion to float depends on architecture
};    


class CRandomMother {             // Encapsulate random number generator
public:
   void RandomInit(uint32 seed);       // Initialization
   int IRandom(int min, int max);      // Get integer random number in desired interval
   double Random();                    // Get floating point random number
   uint32 BRandom();                   // Output random bits
   CRandomMother(uint32 seed) {   // Constructor
      RandomInit(seed);}
protected:
   uint32 x[5];                        // History buffer
};

#ifdef R_BUILD
   #include "stocR.h"           // Include this when building R-language interface
#endif


/***********************************************************************
 Choose which uniform random number generator to base these classes on
***********************************************************************/

// STOC_BASE defines which base class to use for the non-uniform
// random number generator classes StochasticLib1, 2, and 3.

#ifndef STOC_BASE
   #ifdef R_BUILD
      // Inherit from StocRBase when building for R-language interface
      #define STOC_BASE StocRBase
   #else
      #define STOC_BASE CRandomMersenne
      // Or choose any other random number generator base class:
      //#define STOC_BASE CRandomMersenneA
      //#define STOC_BASE CRandomMother
   #endif
#endif

/***********************************************************************
         Other simple functions
***********************************************************************/

double LnFac(int32 n);                 // log factorial (stoc1.cpp)
double LnFacr(double x);               // log factorial of non-integer (wnchyppr.cpp)
double FallingFactorial(double a, double b); // Falling factorial (wnchyppr.cpp)
double Erf (double x);                 // error function (wnchyppr.cpp)
int32 FloorLog2(float x);              // floor(log2(x)) for x > 0 (wnchyppr.cpp)
int NumSD (double accuracy);           // used internally for determining summation interval


/***********************************************************************
         Constants and tables
***********************************************************************/

// Maximum number of colors in the multivariate distributions
#ifndef MAXCOLORS
   #define MAXCOLORS 32                // You may change this value
#endif

// constant for LnFac function:
static const int FAK_LEN = 1024;       // length of factorial table

// The following tables are tables of residues of a certain expansion
// of the error function. These tables are used in the Laplace method
// for calculating Wallenius' noncentral hypergeometric distribution.
// There are ERFRES_N tables covering desired precisions from
// 2^(-ERFRES_B) to 2^(-ERFRES_E). Only the table that matches the
// desired precision is used. The tables are defined in erfres.h which
// is included in wnchyppr.cpp.

// constants for ErfRes tables:
static const int ERFRES_B = 16;        // begin: -log2 of lowest precision
static const int ERFRES_E = 40;        // end:   -log2 of highest precision
static const int ERFRES_S =  2;        // step size from begin to end
static const int ERFRES_N = (ERFRES_E-ERFRES_B)/ERFRES_S+1; // number of tables
static const int ERFRES_L = 48;        // length of each table

// tables of error function residues:
extern "C" double ErfRes [ERFRES_N][ERFRES_L];

// number of std. deviations to include in integral to obtain desired precision:
extern "C" double NumSDev[ERFRES_N];


/***********************************************************************
         Class StochasticLib1
***********************************************************************/

class StochasticLib1 : public STOC_BASE {
   // This class encapsulates the random variate generating functions.
   // May be derived from any of the random number generators.
public:
   StochasticLib1 (int seed);          // constructor
   int Bernoulli(double p);            // bernoulli distribution
   double Normal(double m, double s);  // normal distribution
   int32 Poisson (double L);           // poisson distribution
   int32 Binomial (int32 n, double p); // binomial distribution
   int32 Hypergeometric (int32 n, int32 m, int32 N); // hypergeometric distribution
   void Multinomial (int32 * destination, double * source, int32 n, int colors); // multinomial distribution
   void Multinomial (int32 * destination, int32 * source, int32 n, int colors);  // multinomial distribution
   void MultiHypergeometric (int32 * destination, int32 * source, int32 n, int colors); // multivariate hypergeometric distribution
   void Shuffle(int * list, int min, int n); // shuffle integers

   // functions used internally
protected:
   static double fc_lnpk(int32 k, int32 N_Mn, int32 M, int32 n); // used by Hypergeometric

   // subfunctions for each approximation method
   int32 PoissonInver(double L);                       // poisson by inversion
   int32 PoissonRatioUniforms(double L);               // poisson by ratio of uniforms
   int32 PoissonLow(double L);                         // poisson for extremely low L
   int32 BinomialInver (int32 n, double p);            // binomial by inversion
   int32 BinomialRatioOfUniforms (int32 n, double p);  // binomial by ratio of uniforms
   int32 HypInversionMod (int32 n, int32 M, int32 N);  // hypergeometric by inversion searching from mode
   int32 HypRatioOfUnifoms (int32 n, int32 M, int32 N);// hypergeometric by ratio of uniforms method

   // Variables specific to each distribution:
   // Variables used by Normal distribution
   double normal_x2;  int normal_x2_valid;

   // Variables used by Hypergeometric distribution
   int32  hyp_n_last, hyp_m_last, hyp_N_last;    // Last values of parameters
   int32  hyp_mode, hyp_mp;                      // Mode, mode+1
   int32  hyp_bound;                             // Safety upper bound
   double hyp_a;                                 // hat center
   double hyp_h;                                 // hat width
   double hyp_fm;                                // Value at mode

   // Variables used by Poisson distribution
   double pois_L_last;                           // previous value of L
   double pois_f0;                               // value at x=0 or at mode
   double pois_a;                                // hat center
   double pois_h;                                // hat width
   double pois_g;                                // ln(L)
   int32  pois_bound;                            // upper bound

   // Variables used by Binomial distribution
   int32 bino_n_last;                            // last n
   double bino_p_last;                           // last p
   int32 bino_mode;                              // mode
   int32 bino_bound;                             // upper bound
   double bino_a;                                // hat center
   double bino_h;                                // hat width
   double bino_g;                                // value at mode
   double bino_r1;                               // p/(1-p) or ln(p/(1-p))
};


/***********************************************************************
Class StochasticLib2
***********************************************************************/

class StochasticLib2 : public StochasticLib1 {
   // derived class, redefining some functions
public:
   int32 Poisson (double L);                           // poisson distribution
   int32 Binomial (int32 n, double p);                 // binomial distribution
   int32 Hypergeometric (int32 n, int32 M, int32 N);   // hypergeometric distribution
   StochasticLib2(int seed):StochasticLib1(seed){};    // constructor  

   // subfunctions for each approximation method:
protected:
   int32 PoissonModeSearch(double L);                  // poisson by search from mode
   int32 PoissonPatchwork(double L);                   // poisson by patchwork rejection
   static double PoissonF(int32 k, double l_nu, double c_pm); // used by PoissonPatchwork
   int32 BinomialModeSearch(int32 n, double p);        // binomial by search from mode
   int32 BinomialPatchwork(int32 n, double p);         // binomial by patchwork rejection
   double BinomialF(int32 k, int32 n, double l_pq, double c_pm); // used by BinomialPatchwork
   int32 HypPatchwork (int32 n, int32 M, int32 N);     // hypergeometric by patchwork rejection

   // Variables used by Binomial distribution
   int32  Bino_k1, Bino_k2, Bino_k4, Bino_k5;
   double Bino_dl, Bino_dr, Bino_r1, Bino_r2, Bino_r4, Bino_r5, 
      Bino_ll, Bino_lr, Bino_l_pq, Bino_c_pm,
      Bino_f1, Bino_f2, Bino_f4, Bino_f5, 
      Bino_p1, Bino_p2, Bino_p3, Bino_p4, Bino_p5, Bino_p6;

   // Variables used by Poisson distribution
   int32  Pois_k1, Pois_k2, Pois_k4, Pois_k5;
   double Pois_dl, Pois_dr, Pois_r1, Pois_r2, Pois_r4, Pois_r5, 
      Pois_ll, Pois_lr, Pois_l_my, Pois_c_pm,
      Pois_f1, Pois_f2, Pois_f4, Pois_f5, 
      Pois_p1, Pois_p2, Pois_p3, Pois_p4, Pois_p5, Pois_p6;

   // Variables used by Hypergeometric distribution
   int32  Hyp_L, Hyp_k1, Hyp_k2, Hyp_k4, Hyp_k5;
   double Hyp_dl, Hyp_dr, 
      Hyp_r1, Hyp_r2, Hyp_r4, Hyp_r5, 
      Hyp_ll, Hyp_lr, Hyp_c_pm, 
      Hyp_f1, Hyp_f2, Hyp_f4, Hyp_f5, 
      Hyp_p1, Hyp_p2, Hyp_p3, Hyp_p4, Hyp_p5, Hyp_p6;
};


/***********************************************************************
Class StochasticLib3
***********************************************************************/

class StochasticLib3 : public StochasticLib1 {
   // This class can be derived from either StochasticLib1 or StochasticLib2.
   // Adds more probability distributions
public:
   StochasticLib3(int seed); // constructor
   void SetAccuracy(double accur);  // define accuracy of calculations
   int32 WalleniusNCHyp (int32 n, int32 m, int32 N, double odds); // Wallenius noncentral hypergeometric distribution
   int32 FishersNCHyp (int32 n, int32 m, int32 N, double odds); // Fisher's noncentral hypergeometric distribution
   void MultiWalleniusNCHyp (int32 * destination, int32 * source, double * weights, int32 n, int colors); // multivariate Wallenius noncentral hypergeometric distribution
   void MultiComplWalleniusNCHyp (int32 * destination, int32 * source, double * weights, int32 n, int colors); // multivariate complementary Wallenius noncentral hypergeometric distribution
   void MultiFishersNCHyp (int32 * destination, int32 * source, double * weights, int32 n, int colors); // multivariate Fisher's noncentral hypergeometric distribution
   // subfunctions for each approximation method
protected:
   int32 WalleniusNCHypUrn (int32 n, int32 m, int32 N, double odds); // WalleniusNCHyp by urn model
   int32 WalleniusNCHypInversion (int32 n, int32 m, int32 N, double odds); // WalleniusNCHyp by inversion method
   int32 WalleniusNCHypTable (int32 n, int32 m, int32 N, double odds); // WalleniusNCHyp by table method
   int32 WalleniusNCHypRatioOfUnifoms (int32 n, int32 m, int32 N, double odds); // WalleniusNCHyp by ratio-of-uniforms
   int32 FishersNCHypInversion (int32 n, int32 m, int32 N, double odds); // FishersNCHyp by inversion
   int32 FishersNCHypRatioOfUnifoms (int32 n, int32 m, int32 N, double odds); // FishersNCHyp by ratio-of-uniforms

   // variables
   double accuracy;                         // desired accuracy of calculations

   // Variables for Fisher
   int32 fnc_n_last, fnc_m_last, fnc_N_last;// last values of parameters
   int32 fnc_bound;                         // upper bound
   double fnc_o_last;
   double fnc_f0, fnc_scale;
   double fnc_a;                            // hat center
   double fnc_h;                            // hat width
   double fnc_lfm;                          // ln(f(mode))
   double fnc_logb;                         // ln(odds)

   // variables for Wallenius
   int32 wnc_n_last, wnc_m_last, wnc_N_last;// previous parameters
   double wnc_o_last;
   int32 wnc_bound1, wnc_bound2;            // lower and upper bound
   int32 wnc_mode;                          // mode
   double wnc_a;                            // hat center
   double wnc_h;                            // hat width
   double wnc_k;                            // probability value at mode
   int UseChopDown;                         // use chop down inversion instead
   #define WALL_TABLELENGTH  512            // max length of table
   double wall_ytable[WALL_TABLELENGTH];    // table of probability values
   int32 wall_tablen;                       // length of table
   int32 wall_x1;                           // lower x limit for table
};


/***********************************************************************
Class CWalleniusNCHypergeometric
***********************************************************************/

class CWalleniusNCHypergeometric {
   // This class contains methods for calculating the univariate
   // Wallenius' noncentral hypergeometric probability function
public:
   CWalleniusNCHypergeometric(int32 n, int32 m, int32 N, double odds, double accuracy=1.E-8); // constructor
   void SetParameters(int32 n, int32 m, int32 N, double odds); // change parameters
   double probability(int32 x);                 // calculate probability function
   int32 MakeTable(double * table, int32 MaxLength, int32 * xfirst, int32 * xlast, double cutoff = 0.); // make table of probabilities
   double mean(void);                           // approximate mean
   double variance(void);                       // approximate variance (poor approximation)
   int32 mode(void);                              // calculate mode
   double moments(double * mean, double * var); // calculate exact mean and variance
   int BernouilliH(int32 x, double h, double rh, StochasticLib1 *sto); // used by rejection method

   // implementations of different calculation methods
protected:
   double recursive(void);             // recursive calculation
   double binoexpand(void);            // binomial expansion of integrand
   double laplace(void);               // Laplace's method with narrow integration interval
   double integrate(void);             // numerical integration

   // other subfunctions
   double lnbico(void);                // natural log of binomial coefficients
   void findpars(void);                // calculate r, w, E
   double integrate_step(double a, double b); // used by integrate()
   double search_inflect(double t_from, double t_to); // used by integrate()

   // parameters
   double omega;                       // Odds
   int32 n, m, N, x;                   // Parameters
   int32 xmin, xmax;                   // Minimum and maximum x
   double accuracy;                    // Desired precision
   // parameters used by lnbico
   int32 xLastBico;
   double bico, mFac, xFac;
   // parameters generated by findpars and used by probability, laplace, integrate:
   double r, rd, w, wr, E, phi2d;
   int32 xLastFindpars;
};


/***********************************************************************
Class CMultiWalleniusNCHypergeometric
***********************************************************************/

class CMultiWalleniusNCHypergeometric {
   // This class encapsulates the different methods for calculating the
   // multivariate Wallenius noncentral hypergeometric probability function
public:
   CMultiWalleniusNCHypergeometric(int32 n, int32 * m, double * odds, int colors, double accuracy=1.E-8); // constructor
   void SetParameters(int32 n, int32 * m, double * odds, int colors); // change parameters
   double probability(int32 * x);      // calculate probability function
   void mean(double * mu);             // calculate approximate mean

      // implementations of different calculation methods
protected:
   double binoexpand(void);            // binomial expansion of integrand
   double laplace(void);               // Laplace's method with narrow integration interval
   double integrate(void);             // numerical integration

   // other subfunctions
   double lnbico(void);                // natural log of binomial coefficients
   void findpars(void);                // calculate r, w, E
   double integrate_step(double a, double b); // used by integrate()
   double search_inflect(double t_from, double t_to); // used by integrate()

   // parameters
   double * omega;
   double accuracy;
   int32 n, N;
   int32 * m, * x;
   int colors;
   int Dummy_align;
   // parameters generated by findpars and used by probability, laplace, integrate:
   double r, rd, w, wr, E, phi2d;
   // generated by lnbico
   double bico;
};


/***********************************************************************
Class CMultiWalleniusNCHypergeometricMoments
***********************************************************************/

class CMultiWalleniusNCHypergeometricMoments: public CMultiWalleniusNCHypergeometric {
   // This class calculates the exact mean and variance of the multivariate
   // Wallenius noncentral hypergeometric distribution by calculating all the 
   // possible x-combinations with probability < accuracy
public:
   CMultiWalleniusNCHypergeometricMoments(int32 n, int32 * m, double * odds, int colors, double accuracy=1.E-8) 
      : CMultiWalleniusNCHypergeometric(n, m, odds, colors, accuracy) {};
   double moments(double * mean, double * stddev, int32 * combinations = 0);

protected:
   // functions used internally
   double loop(int32 n, int c);        // recursive loops
   // data
   int32 xi[MAXCOLORS];                // x vector to calculate probability of
   int32 xm[MAXCOLORS];                // rounded approximate mean of x[i]
   int32 remaining[MAXCOLORS];         // number of balls of color > c in urn
   double sx[MAXCOLORS];               // sum of x*f(x)
   double sxx[MAXCOLORS];              // sum of x^2*f(x)
   int32 sn;                           // number of combinations
};


/***********************************************************************
Class CFishersNCHypergeometric
***********************************************************************/

class CFishersNCHypergeometric {
   // This class contains methods for calculating the univariate Fisher's
   // noncentral hypergeometric probability function
public:
   CFishersNCHypergeometric(int32 n, int32 m, int32 N, double odds, double accuracy = 1E-8); // constructor
   double probability(int32 x);                   // calculate probability function
   double probabilityRatio(int32 x, int32 x0);    // calculate probability f(x)/f(x0)
   double MakeTable(double * table, int32 MaxLength, int32 * xfirst, int32 * xlast, double cutoff = 0.); // make table of probabilities
   double mean(void);                             // calculate approximate mean
   double variance(void);                         // approximate variance
   int32 mode(void);                              // calculate mode (exact)
   double moments(double * mean, double * var);   // calculate exact mean and variance

protected:
   double lng(int32 x);                           // natural log of proportional function

   // parameters
   double odds;                        // odds ratio
   double logodds;                     // ln odds ratio
   double accuracy;                    // accuracy
   int32 n, m, N;                      // Parameters
   int32 xmin, xmax;                   // minimum and maximum of x

   // parameters used by subfunctions
   int32 xLast;
   double mFac, xFac;                  // log factorials
   double scale;                       // scale to apply to lng function
   double rsum;                        // reciprocal sum of proportional function
   int ParametersChanged;
};


/***********************************************************************
Class CMultiFishersNCHypergeometric
***********************************************************************/

class CMultiFishersNCHypergeometric {
   // This class contains functions for calculating the multivariate
   // Fisher's noncentral hypergeometric probability function and its mean and 
   // variance. Warning: the time consumption for first call to 
   // probability or moments is proportional to the total number of
   // possible x combinations, which may be extreme!
public:
   CMultiFishersNCHypergeometric(int32 n, int32 * m, double * odds, int colors, double accuracy = 1E-9); // constructor
   double probability(int32 * x);      // calculate probability function
   void mean(double * mu);             // calculate approximate mean
   void variance(double * var);        // calculate approximate variance
   double moments(double * mean, double * stddev, int32 * combinations = 0); // calculate exact mean and variance

protected:
   double lng(int32 * x);              // natural log of proportional function
   void SumOfAll(void);                // calculates sum of proportional function for all x combinations
   double loop(int32 n, int c);        // recursive loops used by SumOfAll
   int32 n, N;                         // copy of parameters
   int32 * m;
   double * odds;
   int colors;
   double logodds[MAXCOLORS];          // log odds
   double mFac;                        // sum of log m[i]!
   double scale;                       // scale to apply to lng function
   double rsum;                        // reciprocal sum of proportional function
   double accuracy;                    // accuracy of calculation

   // data used by used by SumOfAll
   int32 xi[MAXCOLORS];                // x vector to calculate probability of
   int32 xm[MAXCOLORS];                // rounded approximate mean of x[i]
   int32 remaining[MAXCOLORS];         // number of balls of color > c in urn
   double sx[MAXCOLORS];               // sum of x*f(x) or mean
   double sxx[MAXCOLORS];              // sum of x^2*f(x) or variance
   int32 sn;                           // number of possible combinations of x
};


struct Bessjy {
 static const Doub xj00,xj10,xj01,xj11,twoopi,pio4;
 static const Doub j0r[7],j0s[7],j0pn[5],j0pd[5],j0qn[5],j0qd[5];
 static const Doub j1r[7],j1s[7],j1pn[5],j1pd[5],j1qn[5],j1qd[5];
 static const Doub y0r[9],y0s[9],y0pn[5],y0pd[5],y0qn[5],y0qd[5];
 static const Doub y1r[8],y1s[8],y1pn[5],y1pd[5],y1qn[5],y1qd[5];
 Doub nump,denp,numq,denq,y,z,ax,xx;

 Doub j0(const Doub x) {
  if ((ax=abs(x)) < 8.0) {
   rat(x,j0r,j0s,6);
   return nump*(y-xj00)*(y-xj10)/denp;
  } else {
   asp(j0pn,j0pd,j0qn,j0qd,1.);
   return sqrt(twoopi/ax)*(cos(xx)*nump/denp-z*sin(xx)*numq/denq);
  }
 }

 Doub j1(const Doub x) {
  if ((ax=abs(x)) < 8.0) {
   rat(x,j1r,j1s,6);
   return x*nump*(y-xj01)*(y-xj11)/denp;
  } else {
   asp(j1pn,j1pd,j1qn,j1qd,3.);
   Doub ans=sqrt(twoopi/ax)*(cos(xx)*nump/denp-z*sin(xx)*numq/denq);
   return x > 0.0 ? ans : -ans;
  }
 }
 
 Doub y0(const Doub x) {
  if (x < 8.0) {
   Doub j0x = j0(x);
   rat(x,y0r,y0s,8);
   return nump/denp+twoopi*j0x*log(x);
  } else {
   ax=x;
   asp(y0pn,y0pd,y0qn,y0qd,1.);
   return sqrt(twoopi/x)*(sin(xx)*nump/denp+z*cos(xx)*numq/denq);
  }
 }

 Doub y1(const Doub x) {
  if (x < 8.0) {
   Doub j1x = j1(x);
   rat(x,y1r,y1s,7);
   return x*nump/denp+twoopi*(j1x*log(x)-1.0/x);
  } else {
   ax=x;
   asp(y1pn,y1pd,y1qn,y1qd,3.);
   return sqrt(twoopi/x)*(sin(xx)*nump/denp+z*cos(xx)*numq/denq);
  }
 }

 Doub jn(const Int n, const Doub x);

 Doub yn(const Int n, const Doub x);

 void rat(const Doub x, const Doub *r, const Doub *s, const Int n) {
  y = x*x;
  z=64.0-y;
  nump=r[n];
  denp=s[n];
  for (Int i=n-1;i>=0;i--) {
   nump=nump*z+r[i];
   denp=denp*y+s[i];
  }
 }

 void asp(const Doub *pn, const Doub *pd, const Doub *qn, const Doub *qd,
  const Doub fac) {
  z=8.0/ax;
  y=z*z;
  xx=ax-fac*pio4;
  nump=pn[4];
  denp=pd[4];
  numq=qn[4];
  denq=qd[4];
  for (Int i=3;i>=0;i--) {
   nump=nump*y+pn[i];
   denp=denp*y+pd[i];
   numq=numq*y+qn[i];
   denq=denq*y+qd[i];
  } 
 }
};
Doub Bessjy::jn(const Int n, const Doub x)
{
 const Doub ACC=160.0;
 const Int IEXP=numeric_limits<Doub>::max_exponent/2;
 Bool jsum;
 Int j,k,m;
 Doub ax,bj,bjm,bjp,dum,sum,tox,ans;
 if (n==0) return j0(x);
 if (n==1) return j1(x);
 ax=abs(x);
 if (ax*ax <= 8.0*numeric_limits<Doub>::min()) return 0.0;
 else if (ax > Doub(n)) {
  tox=2.0/ax;
  bjm=j0(ax);
  bj=j1(ax);
  for (j=1;j<n;j++) {
   bjp=j*tox*bj-bjm;
   bjm=bj;
   bj=bjp;
  }
  ans=bj;
 } else {
  tox=2.0/ax;
  m=2*((n+Int(sqrt(ACC*n)))/2);
  jsum=false;
  bjp=ans=sum=0.0;
  bj=1.0;
  for (j=m;j>0;j--) {
   bjm=j*tox*bj-bjp;
   bjp=bj;
   bj=bjm;
   dum=frexp(bj,&k);
   if (k > IEXP) {
    bj=ldexp(bj,-IEXP);
    bjp=ldexp(bjp,-IEXP);
    ans=ldexp(ans,-IEXP);
    sum=ldexp(sum,-IEXP);
   }
   if (jsum) sum += bj;
   jsum=!jsum;
   if (j == n) ans=bjp;
  }
  sum=2.0*sum-bj;
  ans /= sum;
 }
 return x < 0.0 && (n & 1) ? -ans : ans;
}
Doub Bessjy::yn(const Int n, const Doub x)
{
 Int j;
 Doub by,bym,byp,tox;
 if (n==0) return y0(x);
 if (n==1) return y1(x);
 tox=2.0/x;
 by=y1(x);
 bym=y0(x);
 for (j=1;j<n;j++) {
  byp=j*tox*by-bym;
  bym=by;
  by=byp;
 }
 return by;
}
struct Bessik {
 static const Doub i0p[14],i0q[5],i0pp[5],i0qq[6];
 static const Doub i1p[14],i1q[5],i1pp[5],i1qq[6];
 static const Doub k0pi[5],k0qi[3],k0p[5],k0q[3],k0pp[8],k0qq[8];
 static const Doub k1pi[5],k1qi[3],k1p[5],k1q[3],k1pp[8],k1qq[8];
 Doub y,z,ax,term;

 Doub i0(const Doub x) {
  if ((ax=abs(x)) < 15.0) {
   y = x*x;
   return poly(i0p,13,y)/poly(i0q,4,225.-y);
  } else {
   z=1.0-15.0/ax;
   return exp(ax)*poly(i0pp,4,z)/(poly(i0qq,5,z)*sqrt(ax));
  }
 }

 Doub i1(const Doub x) {
  if ((ax=abs(x)) < 15.0) {
   y=x*x;
   return x*poly(i1p,13,y)/poly(i1q,4,225.-y);
  } else {
   z=1.0-15.0/ax;
   Doub ans=exp(ax)*poly(i1pp,4,z)/(poly(i1qq,5,z)*sqrt(ax));
   return x > 0.0 ? ans : -ans;
  }
 }

 Doub k0(const Doub x) {
  if (x <= 1.0) {
   z=x*x;
   term = poly(k0pi,4,z)*log(x)/poly(k0qi,2,1.-z);
   return poly(k0p,4,z)/poly(k0q,2,1.-z)-term;
  } else {
   z=1.0/x;
   return exp(-x)*poly(k0pp,7,z)/(poly(k0qq,7,z)*sqrt(x));
  }
 }

 Doub k1(const Doub x) {
  if (x <= 1.0) {
   z=x*x;
   term = poly(k1pi,4,z)*log(x)/poly(k1qi,2,1.-z);
   return x*(poly(k1p,4,z)/poly(k1q,2,1.-z)+term)+1./x;
  } else {
   z=1.0/x;
   return exp(-x)*poly(k1pp,7,z)/(poly(k1qq,7,z)*sqrt(x));
  }
 }

 Doub in(const Int n, const Doub x);

 Doub kn(const Int n, const Doub x);

 inline Doub poly(const Doub *cof, const Int n, const Doub x) {
  Doub ans = cof[n];
  for (Int i=n-1;i>=0;i--) ans = ans*x+cof[i];
  return ans;
 }
};
Doub Bessik::kn(const Int n, const Doub x)
{
 Int j;
 Doub bk,bkm,bkp,tox;
 if (n==0) return k0(x);
 if (n==1) return k1(x);
 tox=2.0/x;
 bkm=k0(x);
 bk=k1(x);
 for (j=1;j<n;j++) {
  bkp=bkm+j*tox*bk;
  bkm=bk;
  bk=bkp;
 }
 return bk;
}
Doub Bessik::in(const Int n, const Doub x)
{
 const Doub ACC=200.0;
 const Int IEXP=numeric_limits<Doub>::max_exponent/2;
 Int j,k;
 Doub bi,bim,bip,dum,tox,ans;
 if (n==0) return i0(x);
 if (n==1) return i1(x);
 if (x*x <= 8.0*numeric_limits<Doub>::min()) return 0.0;
 else {
  tox=2.0/abs(x);
  bip=ans=0.0;
  bi=1.0;
  for (j=2*(n+Int(sqrt(ACC*n)));j>0;j--) {
   bim=bip+j*tox*bi;
   bip=bi;
   bi=bim;
   dum=frexp(bi,&k);
   if (k > IEXP) {
    ans=ldexp(ans,-IEXP);
    bi=ldexp(bi,-IEXP);
    bip=ldexp(bip,-IEXP);
   }
   if (j == n) ans=bip;
  }
  ans *= i0(x)/bi;
  return x < 0.0 && (n & 1) ? -ans : ans;
 }
}
const Doub Bessjy::xj00=5.783185962946785;
const Doub Bessjy::xj10=3.047126234366209e1;
const Doub Bessjy::xj01=1.468197064212389e1;
const Doub Bessjy::xj11=4.921845632169460e1;
const Doub Bessjy::twoopi=0.6366197723675813;
const Doub Bessjy::pio4=0.7853981633974483;
const Doub Bessjy::j0r[]={1.682397144220462e-4,2.058861258868952e-5,
 5.288947320067750e-7,5.557173907680151e-9,2.865540042042604e-11,
 7.398972674152181e-14,7.925088479679688e-17};
const Doub Bessjy::j0s[]={1.0,1.019685405805929e-2,5.130296867064666e-5,
 1.659702063950243e-7,3.728997574317067e-10,
 5.709292619977798e-13,4.932979170744996e-16};
const Doub Bessjy::j0pn[]={9.999999999999999e-1,1.039698629715637,
 2.576910172633398e-1,1.504152485749669e-2,1.052598413585270e-4};
const Doub Bessjy::j0pd[]={1.0,1.040797262528109,2.588070904043728e-1,
 1.529954477721284e-2,1.168931211650012e-4};
const Doub Bessjy::j0qn[]={-1.562499999999992e-2,-1.920039317065641e-2,
 -5.827951791963418e-3,-4.372674978482726e-4,-3.895839560412374e-6};
const Doub Bessjy::j0qd[]={1.0,1.237980436358390,3.838793938147116e-1,
 3.100323481550864e-2,4.165515825072393e-4};
const Doub Bessjy::j1r[]={7.309637831891357e-5,3.551248884746503e-6,
 5.820673901730427e-8,4.500650342170622e-10,1.831596352149641e-12,
 3.891583573305035e-15,3.524978592527982e-18};
const Doub Bessjy::j1s[]={1.0,9.398354768446072e-3,4.328946737100230e-5,
 1.271526296341915e-7,2.566305357932989e-10,
 3.477378203574266e-13,2.593535427519985e-16};
const Doub Bessjy::j1pn[]={1.0,1.014039111045313,2.426762348629863e-1,
 1.350308200342000e-2,9.516522033988099e-5};
const Doub Bessjy::j1pd[]={1.0,1.012208056357845,2.408580305488938e-1,
 1.309511056184273e-2,7.746422941504713e-5};
const Doub Bessjy::j1qn[]={4.687499999999991e-2,5.652407388406023e-2,
 1.676531273460512e-2,1.231216817715814e-3,1.178364381441801e-5};
const Doub Bessjy::j1qd[]={1.0,1.210119370463693,3.626494789275638e-1,
 2.761695824829316e-2,3.240517192670181e-4};
const Doub Bessjy::y0r[]={-7.653778457189104e-3,-5.854760129990403e-2,
 3.720671300654721e-4,3.313722284628089e-5,4.247761237036536e-8,
 -4.134562661019613e-9,-3.382190331837473e-11,
 -1.017764126587862e-13,-1.107646382675456e-16};
const Doub Bessjy::y0s[]={1.0,1.125494540257841e-2,6.427210537081400e-5,
 2.462520624294959e-7,7.029372432344291e-10,1.560784108184928e-12,
 2.702374957564761e-15,3.468496737915257e-18,2.716600180811817e-21};
const Doub Bessjy::y0pn[]={9.999999999999999e-1,1.039698629715637,
 2.576910172633398e-1,1.504152485749669e-2,1.052598413585270e-4};
const Doub Bessjy::y0pd[]={1.0,1.040797262528109,2.588070904043728e-1,
 1.529954477721284e-2,1.168931211650012e-4};
const Doub Bessjy::y0qn[]={-1.562499999999992e-2,-1.920039317065641e-2,
 -5.827951791963418e-3,-4.372674978482726e-4,-3.895839560412374e-6};
const Doub Bessjy::y0qd[]={1.0,1.237980436358390,3.838793938147116e-1,
 3.100323481550864e-2,4.165515825072393e-4};
const Doub Bessjy::y1r[]={-1.041835425863234e-1,-1.135093963908952e-5,
 2.212118520638132e-4,1.270981874287763e-6,
 -3.982892100836748e-8,-4.820712110115943e-10,
 -1.929392690596969e-12,-2.725259514545605e-15};
const Doub Bessjy::y1s[]={1.0,1.186694184425838e-2,7.121205411175519e-5,
 2.847142454085055e-7,8.364240962784899e-10,1.858128283833724e-12,
 3.018846060781846e-15,3.015798735815980e-18};
const Doub Bessjy::y1pn[]={1.0,1.014039111045313,2.426762348629863e-1,
 1.350308200342000e-2,9.516522033988099e-5};
const Doub Bessjy::y1pd[]={1.0,1.012208056357845,2.408580305488938e-1,
 1.309511056184273e-2,7.746422941504713e-5};
const Doub Bessjy::y1qn[]={4.687499999999991e-2,5.652407388406023e-2,
 1.676531273460512e-2,1.231216817715814e-3,1.178364381441801e-5};
const Doub Bessjy::y1qd[]={1.0,1.210119370463693,3.626494789275638e-1,
 2.761695824829316e-2,3.240517192670181e-4};
const Doub Bessik::i0p[]={9.999999999999997e-1,2.466405579426905e-1,
 1.478980363444585e-2,3.826993559940360e-4,5.395676869878828e-6,
 4.700912200921704e-8,2.733894920915608e-10,1.115830108455192e-12,
 3.301093025084127e-15,7.209167098020555e-18,1.166898488777214e-20,
 1.378948246502109e-23,1.124884061857506e-26,5.498556929587117e-30};
const Doub Bessik::i0q[]={4.463598170691436e-1,1.702205745042606e-3,
 2.792125684538934e-6,2.369902034785866e-9,8.965900179621208e-13};
const Doub Bessik::i0pp[]={1.192273748120670e-1,1.947452015979746e-1,
 7.629241821600588e-2,8.474903580801549e-3,2.023821945835647e-4};
const Doub Bessik::i0qq[]={2.962898424533095e-1,4.866115913196384e-1,
 1.938352806477617e-1,2.261671093400046e-2,6.450448095075585e-4,
 1.529835782400450e-6};
const Doub Bessik::i1p[]={5.000000000000000e-1,6.090824836578078e-2,
 2.407288574545340e-3,4.622311145544158e-5,5.161743818147913e-7,
 3.712362374847555e-9,1.833983433811517e-11,6.493125133990706e-14,
 1.693074927497696e-16,3.299609473102338e-19,4.813071975603122e-22,
 5.164275442089090e-25,3.846870021788629e-28,1.712948291408736e-31};
const Doub Bessik::i1q[]={4.665973211630446e-1,1.677754477613006e-3,
 2.583049634689725e-6,2.045930934253556e-9,7.166133240195285e-13};
const Doub Bessik::i1pp[]={1.286515211317124e-1,1.930915272916783e-1,
 6.965689298161343e-2,7.345978783504595e-3,1.963602129240502e-4};
const Doub Bessik::i1qq[]={3.309385098860755e-1,4.878218424097628e-1,
 1.663088501568696e-1,1.473541892809522e-2,1.964131438571051e-4,
 -1.034524660214173e-6};
const Doub Bessik::k0pi[]={1.0,2.346487949187396e-1,1.187082088663404e-2,
 2.150707366040937e-4,1.425433617130587e-6};
const Doub Bessik::k0qi[]={9.847324170755358e-1,1.518396076767770e-2,
 8.362215678646257e-5};
const Doub Bessik::k0p[]={1.159315156584126e-1,2.770731240515333e-1,
 2.066458134619875e-2,4.574734709978264e-4,3.454715527986737e-6};
const Doub Bessik::k0q[]={9.836249671709183e-1,1.627693622304549e-2,
 9.809660603621949e-5};
const Doub Bessik::k0pp[]={1.253314137315499,1.475731032429900e1,
 6.123767403223466e1,1.121012633939949e2,9.285288485892228e1,
 3.198289277679660e1,3.595376024148513,6.160228690102976e-2};
const Doub Bessik::k0qq[]={1.0,1.189963006673403e1,5.027773590829784e1,
 9.496513373427093e1,8.318077493230258e1,3.181399777449301e1,
 4.443672926432041,1.408295601966600e-1};
const Doub Bessik::k1pi[]={0.5,5.598072040178741e-2,1.818666382168295e-3,
 2.397509908859959e-5,1.239567816344855e-7};
const Doub Bessik::k1qi[]={9.870202601341150e-1,1.292092053534579e-2,
 5.881933053917096e-5};
const Doub Bessik::k1p[]={-3.079657578292062e-1,-8.109417631822442e-2,
 -3.477550948593604e-3,-5.385594871975406e-5,-3.110372465429008e-7};
const Doub Bessik::k1q[]={9.861813171751389e-1,1.375094061153160e-2,
 6.774221332947002e-5};
const Doub Bessik::k1pp[]={1.253314137315502,1.457171340220454e1,
 6.063161173098803e1,1.147386690867892e2,1.040442011439181e2,
 4.356596656837691e1,7.265230396353690,3.144418558991021e-1};
const Doub Bessik::k1qq[]={1.0,1.125154514806458e1,4.427488496597630e1,
 7.616113213117645e1,5.863377227890893e1,1.850303673841586e1,
 1.857244676566022,2.538540887654872e-2};


struct Base_interp
{
 Int n, mm, jsav, cor, dj;
 const Doub *xx, *yy;
 Base_interp(VecDoub_I &x, const Doub *y, Int m)
  : n(x.size()), mm(m), jsav(0), cor(0), xx(&x[0]), yy(y) {
  dj = MIN(1,(int)pow((Doub)n,0.25));
 }

 Doub interp(Doub x) {
  Int jlo = cor ? hunt(x) : locate(x);
  return rawinterp(jlo,x);
 }

 Int locate(const Doub x);
 Int hunt(const Doub x);
 
 Doub virtual rawinterp(Int jlo, Doub x) = 0;

};
Int Base_interp::locate(const Doub x)
{
 Int ju,jm,jl;
 if (n < 2 || mm < 2 || mm > n) throw("locate size error");
 Bool ascnd=(xx[n-1] >= xx[0]);
 jl=0;
 ju=n-1;
 while (ju-jl > 1) {
  jm = (ju+jl) >> 1;
  if (x >= xx[jm] == ascnd)
   jl=jm;
  else
   ju=jm;
 }
 cor = abs(jl-jsav) > dj ? 0 : 1;
 jsav = jl;
 return MAX(0,MIN(n-mm,jl-((mm-2)>>1)));
}
Int Base_interp::hunt(const Doub x)
{
 Int jl=jsav, jm, ju, inc=1;
 if (n < 2 || mm < 2 || mm > n) throw("hunt size error");
 Bool ascnd=(xx[n-1] >= xx[0]);
 if (jl < 0 || jl > n-1) {
  jl=0;
  ju=n-1;
 } else {
  if (x >= xx[jl] == ascnd) {
   for (;;) {
    ju = jl + inc;
    if (ju >= n-1) { ju = n-1; break;}
    else if (x < xx[ju] == ascnd) break;
    else {
     jl = ju;
     inc += inc;
    }
   }
  } else {
   ju = jl;
   for (;;) {
    jl = jl - inc;
    if (jl <= 0) { jl = 0; break;}
    else if (x >= xx[jl] == ascnd) break;
    else {
     ju = jl;
     inc += inc;
    }
   }
  }
 }
 while (ju-jl > 1) {
  jm = (ju+jl) >> 1;
  if (x >= xx[jm] == ascnd)
   jl=jm;
  else
   ju=jm;
 }
 cor = abs(jl-jsav) > dj ? 0 : 1;
 jsav = jl;
 return MAX(0,MIN(n-mm,jl-((mm-2)>>1)));
}
struct Poly_interp : Base_interp
{
 Doub dy;
 Poly_interp(VecDoub_I &xv, VecDoub_I &yv, Int m)
  : Base_interp(xv,&yv[0],m), dy(0.) {}
 Doub rawinterp(Int jl, Doub x);
};

Doub Poly_interp::rawinterp(Int jl, Doub x)
{
 Int i,m,ns=0;
 Doub y,den,dif,dift,ho,hp,w;
 const Doub *xa = &xx[jl], *ya = &yy[jl];
 VecDoub c(mm),d(mm);
 dif=abs(x-xa[0]);
 for (i=0;i<mm;i++) {
  if ((dift=abs(x-xa[i])) < dif) {
   ns=i;
   dif=dift;
  }
  c[i]=ya[i];
  d[i]=ya[i];
 }
 y=ya[ns--];
 for (m=1;m<mm;m++) {
  for (i=0;i<mm-m;i++) {
   ho=xa[i]-x;
   hp=xa[i+m]-x;
   w=c[i+1]-d[i];
   if ((den=ho-hp) == 0.0) throw("Poly_interp error");
   den=w/den;
   d[i]=hp*den;
   c[i]=ho*den;
  }
  y += (dy=(2*(ns+1) < (mm-m) ? c[ns+1] : d[ns--]));
 }
 return y;
}
struct Rat_interp : Base_interp
{
 Doub dy;
 Rat_interp(VecDoub_I &xv, VecDoub_I &yv, Int m)
  : Base_interp(xv,&yv[0],m), dy(0.) {}
 Doub rawinterp(Int jl, Doub x);
};

Doub Rat_interp::rawinterp(Int jl, Doub x)
{
 const Doub TINY=1.0e-99;
 Int m,i,ns=0;
 Doub y,w,t,hh,h,dd;
 const Doub *xa = &xx[jl], *ya = &yy[jl];
 VecDoub c(mm),d(mm);
 hh=abs(x-xa[0]);
 for (i=0;i<mm;i++) {
  h=abs(x-xa[i]);
  if (h == 0.0) {
   dy=0.0;
   return ya[i];
  } else if (h < hh) {
   ns=i;
   hh=h;
  }
  c[i]=ya[i];
  d[i]=ya[i]+TINY;
 }
 y=ya[ns--];
 for (m=1;m<mm;m++) {
  for (i=0;i<mm-m;i++) {
   w=c[i+1]-d[i];
   h=xa[i+m]-x;
   t=(xa[i]-x)*d[i]/h;
   dd=t-c[i+1];
   if (dd == 0.0) throw("Error in routine ratint");
   dd=w/dd;
   d[i]=c[i+1]*dd;
   c[i]=t*dd;
  }
  y += (dy=(2*(ns+1) < (mm-m) ? c[ns+1] : d[ns--]));
 }
 return y;
}
struct Spline_interp : Base_interp
{
 VecDoub y2;
 
 Spline_interp(VecDoub_I &xv, VecDoub_I &yv, Doub yp1=1.e99, Doub ypn=1.e99)
 : Base_interp(xv,&yv[0],2), y2(xv.size())
 {sety2(&xv[0],&yv[0],yp1,ypn);}

 Spline_interp(VecDoub_I &xv, const Doub *yv, Doub yp1=1.e99, Doub ypn=1.e99)
 : Base_interp(xv,yv,2), y2(xv.size())
 {sety2(&xv[0],yv,yp1,ypn);}

 void sety2(const Doub *xv, const Doub *yv, Doub yp1, Doub ypn);
 Doub rawinterp(Int jl, Doub xv);
};
void Spline_interp::sety2(const Doub *xv, const Doub *yv, Doub yp1, Doub ypn)
{
 Int i,k;
 Doub p,qn,sig,un;
 Int n=y2.size();
 VecDoub u(n-1);
 if (yp1 > 0.99e99)
  y2[0]=u[0]=0.0;
 else {
  y2[0] = -0.5;
  u[0]=(3.0/(xv[1]-xv[0]))*((yv[1]-yv[0])/(xv[1]-xv[0])-yp1);
 }
 for (i=1;i<n-1;i++) {
  sig=(xv[i]-xv[i-1])/(xv[i+1]-xv[i-1]);
  p=sig*y2[i-1]+2.0;
  y2[i]=(sig-1.0)/p;
  u[i]=(yv[i+1]-yv[i])/(xv[i+1]-xv[i]) - (yv[i]-yv[i-1])/(xv[i]-xv[i-1]);
  u[i]=(6.0*u[i]/(xv[i+1]-xv[i-1])-sig*u[i-1])/p;
 }
 if (ypn > 0.99e99)
  qn=un=0.0;
 else {
  qn=0.5;
  un=(3.0/(xv[n-1]-xv[n-2]))*(ypn-(yv[n-1]-yv[n-2])/(xv[n-1]-xv[n-2]));
 }
 y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0);
 for (k=n-2;k>=0;k--)
  y2[k]=y2[k]*y2[k+1]+u[k];
}
Doub Spline_interp::rawinterp(Int jl, Doub x)
{
 Int klo=jl,khi=jl+1;
 Doub y,h,b,a;
 h=xx[khi]-xx[klo];
 if (h == 0.0) throw("Bad input to routine splint");
 a=(xx[khi]-x)/h;
 b=(x-xx[klo])/h;
 y=a*yy[klo]+b*yy[khi]+((a*a*a-a)*y2[klo]
  +(b*b*b-b)*y2[khi])*(h*h)/6.0;
 return y;
}
struct BaryRat_interp : Base_interp
{
 VecDoub w;
 Int d;
 BaryRat_interp(VecDoub_I &xv, VecDoub_I &yv, Int dd);
 Doub rawinterp(Int jl, Doub x);
 Doub interp(Doub x);
};

BaryRat_interp::BaryRat_interp(VecDoub_I &xv, VecDoub_I &yv, Int dd)
  : Base_interp(xv,&yv[0],xv.size()), w(n), d(dd)
{
 if (n<=d) throw("d too large for number of points in BaryRat_interp");
 for (Int k=0;k<n;k++) {
  Int imin=MAX(k-d,0);
  Int imax = k >= n-d ? n-d-1 : k;
  Doub temp = imin & 1 ? -1.0 : 1.0;
  Doub sum=0.0;
  for (Int i=imin;i<=imax;i++) {
   Int jmax=MIN(i+d,n-1);
   Doub term=1.0;
   for (Int j=i;j<=jmax;j++) {
    if (j==k) continue;
    term *= (xx[k]-xx[j]);
   }
   term=temp/term;
   temp=-temp;
   sum += term;
  }
  w[k]=sum;
 }
}
Doub BaryRat_interp::rawinterp(Int jl, Doub x)
{
 Doub num=0,den=0;
 for (Int i=0;i<n;i++) {
  Doub h=x-xx[i];
  if (h == 0.0) {
   return yy[i];
  } else {
   Doub temp=w[i]/h;
   num += temp*yy[i];
   den += temp;
  }
 }
 return num/den;
}
Doub BaryRat_interp::interp(Doub x) {
 return rawinterp(1,x);
}


struct Linear_interp : Base_interp
{
 Linear_interp(VecDoub_I &xv, VecDoub_I &yv)
  : Base_interp(xv,&yv[0],2)  {}
 Doub rawinterp(Int j, Doub x) {
  if (xx[j]==xx[j+1]) return yy[j];
  else return yy[j] + ((x-xx[j])/(xx[j+1]-xx[j]))*(yy[j+1]-yy[j]);
 }
};





struct Bilin_interp {
 Int m,n;
 const MatDoub &y;
 Linear_interp x1terp, x2terp;

 Bilin_interp(VecDoub_I &x1v, VecDoub_I &x2v, MatDoub_I &ym)
  : m(x1v.size()), n(x2v.size()), y(ym),
  x1terp(x1v,x1v), x2terp(x2v,x2v) {}

 Doub interp(Doub x1p, Doub x2p) {
  Int i,j;
  Doub yy, t, u;
  i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
  j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
  t = (x1p-x1terp.xx[i])/(x1terp.xx[i+1]-x1terp.xx[i]);
  u = (x2p-x2terp.xx[j])/(x2terp.xx[j+1]-x2terp.xx[j]);
  yy = (1.-t)*(1.-u)*y[i][j] + t*(1.-u)*y[i+1][j]
   + (1.-t)*u*y[i][j+1] + t*u*y[i+1][j+1];
  return yy;
 }
};
struct Poly2D_interp {
 Int m,n,mm,nn;
 const MatDoub &y;
 VecDoub yv;
 Poly_interp x1terp, x2terp;

 Poly2D_interp(VecDoub_I &x1v, VecDoub_I &x2v, MatDoub_I &ym,
  Int mp, Int np) : m(x1v.size()), n(x2v.size()),
  mm(mp), nn(np), y(ym), yv(m),
  x1terp(x1v,yv,mm), x2terp(x2v,x2v,nn) {}

 Doub interp(Doub x1p, Doub x2p) {
  Int i,j,k;
  i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
  j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
  for (k=i;k<i+mm;k++) {
   x2terp.yy = &y[k][0];
   yv[k] = x2terp.rawinterp(j,x2p);
  }
  return x1terp.rawinterp(i,x1p);
 }
};
struct Spline2D_interp {
 Int m,n;
 const MatDoub &y;
 const VecDoub &x1;
 VecDoub yv;
 NRvector<Spline_interp*> srp;

 Spline2D_interp(VecDoub_I &x1v, VecDoub_I &x2v, MatDoub_I &ym)
  : m(x1v.size()), n(x2v.size()), y(ym), yv(m), x1(x1v), srp(m) {
  for (Int i=0;i<m;i++) srp[i] = new Spline_interp(x2v,&y[i][0]);
 }

 ~Spline2D_interp(){
  for (Int i=0;i<m;i++) delete srp[i];
 }

 Doub interp(Doub x1p, Doub x2p) {
  for (Int i=0;i<m;i++) yv[i] = (*srp[i]).interp(x2p);
  Spline_interp scol(x1,yv);
  return scol.interp(x1p);
 }
};
void bcucof(VecDoub_I &y, VecDoub_I &y1, VecDoub_I &y2, VecDoub_I &y12,
 const Doub d1, const Doub d2, MatDoub_O &c) {
 static Int wt_d[16*16]=
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  -3, 0, 0, 3, 0, 0, 0, 0,-2, 0, 0,-1, 0, 0, 0, 0,
  2, 0, 0,-2, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0,-3, 0, 0, 3, 0, 0, 0, 0,-2, 0, 0,-1,
  0, 0, 0, 0, 2, 0, 0,-2, 0, 0, 0, 0, 1, 0, 0, 1,
  -3, 3, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-2,-1, 0, 0,
  9,-9, 9,-9, 6, 3,-3,-6, 6,-6,-3, 3, 4, 2, 1, 2,
  -6, 6,-6, 6,-4,-2, 2, 4,-3, 3, 3,-3,-2,-1,-1,-2,
  2,-2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 1, 1, 0, 0,
  -6, 6,-6, 6,-3,-3, 3, 3,-4, 4, 2,-2,-2,-2,-1,-1,
  4,-4, 4,-4, 2, 2,-2,-2, 2,-2,-2, 2, 1, 1, 1, 1};
 Int l,k,j,i;
 Doub xx,d1d2=d1*d2;
 VecDoub cl(16),x(16);
 static MatInt wt(16,16,wt_d);
 for (i=0;i<4;i++) {
  x[i]=y[i];
  x[i+4]=y1[i]*d1;
  x[i+8]=y2[i]*d2;
  x[i+12]=y12[i]*d1d2;
 }
 for (i=0;i<16;i++) {
  xx=0.0;
  for (k=0;k<16;k++) xx += wt[i][k]*x[k];
  cl[i]=xx;
 }
 l=0;
 for (i=0;i<4;i++)
  for (j=0;j<4;j++) c[i][j]=cl[l++];
}
void bcuint(VecDoub_I &y, VecDoub_I &y1, VecDoub_I &y2, VecDoub_I &y12,
 const Doub x1l, const Doub x1u, const Doub x2l, const Doub x2u,
 const Doub x1, const Doub x2, Doub &ansy, Doub &ansy1, Doub &ansy2) {
 Int i;
 Doub t,u,d1=x1u-x1l,d2=x2u-x2l;
 MatDoub c(4,4);
 bcucof(y,y1,y2,y12,d1,d2,c);
 if (x1u == x1l || x2u == x2l)
  throw("Bad input in routine bcuint");
 t=(x1-x1l)/d1;
 u=(x2-x2l)/d2;
 ansy=ansy2=ansy1=0.0;
 for (i=3;i>=0;i--) {
  ansy=t*ansy+((c[i][3]*u+c[i][2])*u+c[i][1])*u+c[i][0];
  ansy2=t*ansy2+(3.0*c[i][3]*u+2.0*c[i][2])*u+c[i][1];
  ansy1=u*ansy1+(3.0*c[3][i]*t+2.0*c[2][i])*t+c[1][i];
 }
 ansy1 /= d1;
 ansy2 /= d2;
}


void rk4(VecDoub_I &y, VecDoub_I &dydx, const Doub x, const Doub h,
 VecDoub_O &yout, void derivs(const Doub, VecDoub_I &, VecDoub_O &))
{
 Int n=y.size();
 VecDoub dym(n),dyt(n),yt(n);
 Doub hh=h*0.5;
 Doub h6=h/6.0;
 Doub xh=x+hh;
 for (Int i=0;i<n;i++) yt[i]=y[i]+hh*dydx[i];
 derivs(xh,yt,dyt);
 for (Int i=0;i<n;i++) yt[i]=y[i]+hh*dyt[i];
 derivs(xh,yt,dym);
 for (Int i=0;i<n;i++) {
  yt[i]=y[i]+h*dym[i];
  dym[i] += dyt[i];
 }
 derivs(x+h,yt,dyt);
 for (Int i=0;i<n;i++)
  yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
}


struct SVD {
 Int m,n;
 MatDoub u,v;
 VecDoub w;
 Doub eps, tsh;
 SVD(MatDoub_I &a) : m(a.nrows()), n(a.ncols()), u(a), v(n,n), w(n) {
  eps = numeric_limits<Doub>::epsilon();
  decompose();
  reorder();
  tsh = 0.5*sqrt(m+n+1.)*w[0]*eps;
 }

 void solve(VecDoub_I &b, VecDoub_O &x, Doub thresh);
 void solve(MatDoub_I &b, MatDoub_O &x, Doub thresh);

 Int rank(Doub thresh);
 Int nullity(Doub thresh);
 MatDoub range(Doub thresh);
 MatDoub nullspace(Doub thresh);

 Doub inv_condition() {
  return (w[0] <= 0. || w[n-1] <= 0.) ? 0. : w[n-1]/w[0];
 }

 void decompose();
 void reorder();
 Doub pythag(const Doub a, const Doub b);
};
void SVD::solve(VecDoub_I &b, VecDoub_O &x, Doub thresh = -1.) {
 Int i,j,jj;
 Doub s;
 if (b.size() != m || x.size() != n) throw("SVD::solve bad sizes");
 VecDoub tmp(n);
 tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
 for (j=0;j<n;j++) {
  s=0.0;
  if (w[j] > tsh) {
   for (i=0;i<m;i++) s += u[i][j]*b[i];
   s /= w[j];
  }
  tmp[j]=s;
 }
 for (j=0;j<n;j++) {
  s=0.0;
  for (jj=0;jj<n;jj++) s += v[j][jj]*tmp[jj];
  x[j]=s;
 }
}

void SVD::solve(MatDoub_I &b, MatDoub_O &x, Doub thresh = -1.)
{
 int i,j,m=b.ncols();
 if (b.nrows() != n || x.nrows() != n || b.ncols() != x.ncols())
  throw("SVD::solve bad sizes");
 VecDoub xx(n);
 for (j=0;j<m;j++) {
  for (i=0;i<n;i++) xx[i] = b[i][j];
  solve(xx,xx,thresh);
  for (i=0;i<n;i++) x[i][j] = xx[i];
 }
}
Int SVD::rank(Doub thresh = -1.) {
 Int j,nr=0;
 tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
 for (j=0;j<n;j++) if (w[j] > tsh) nr++;
 return nr;
}

Int SVD::nullity(Doub thresh = -1.) {
 Int j,nn=0;
 tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
 for (j=0;j<n;j++) if (w[j] <= tsh) nn++;
 return nn;
}

MatDoub SVD::range(Doub thresh = -1.){
 Int i,j,nr=0;
 MatDoub rnge(m,rank(thresh));
 for (j=0;j<n;j++) {
  if (w[j] > tsh) {
   for (i=0;i<m;i++) rnge[i][nr] = u[i][j];
   nr++;
  }
 }
 return rnge;
}

MatDoub SVD::nullspace(Doub thresh = -1.){
 Int j,jj,nn=0;
 MatDoub nullsp(n,nullity(thresh));
 for (j=0;j<n;j++) {
  if (w[j] <= tsh) {
   for (jj=0;jj<n;jj++) nullsp[jj][nn] = v[jj][j];
   nn++;
  }
 }
 return nullsp;
}
void SVD::decompose() {
 bool flag;
 Int i,its,j,jj,k,l,nm;
 Doub anorm,c,f,g,h,s,scale,x,y,z;
 VecDoub rv1(n);
 g = scale = anorm = 0.0;
 for (i=0;i<n;i++) {
  l=i+2;
  rv1[i]=scale*g;
  g=s=scale=0.0;
  if (i < m) {
   for (k=i;k<m;k++) scale += abs(u[k][i]);
   if (scale != 0.0) {
    for (k=i;k<m;k++) {
     u[k][i] /= scale;
     s += u[k][i]*u[k][i];
    }
    f=u[i][i];
    g = -SIGN(sqrt(s),f);
    h=f*g-s;
    u[i][i]=f-g;
    for (j=l-1;j<n;j++) {
     for (s=0.0,k=i;k<m;k++) s += u[k][i]*u[k][j];
     f=s/h;
     for (k=i;k<m;k++) u[k][j] += f*u[k][i];
    }
    for (k=i;k<m;k++) u[k][i] *= scale;
   }
  }
  w[i]=scale *g;
  g=s=scale=0.0;
  if (i+1 <= m && i+1 != n) {
   for (k=l-1;k<n;k++) scale += abs(u[i][k]);
   if (scale != 0.0) {
    for (k=l-1;k<n;k++) {
     u[i][k] /= scale;
     s += u[i][k]*u[i][k];
    }
    f=u[i][l-1];
    g = -SIGN(sqrt(s),f);
    h=f*g-s;
    u[i][l-1]=f-g;
    for (k=l-1;k<n;k++) rv1[k]=u[i][k]/h;
    for (j=l-1;j<m;j++) {
     for (s=0.0,k=l-1;k<n;k++) s += u[j][k]*u[i][k];
     for (k=l-1;k<n;k++) u[j][k] += s*rv1[k];
    }
    for (k=l-1;k<n;k++) u[i][k] *= scale;
   }
  }
  anorm=MAX(anorm,(abs(w[i])+abs(rv1[i])));
 }
 for (i=n-1;i>=0;i--) {
  if (i < n-1) {
   if (g != 0.0) {
    for (j=l;j<n;j++)
     v[j][i]=(u[i][j]/u[i][l])/g;
    for (j=l;j<n;j++) {
     for (s=0.0,k=l;k<n;k++) s += u[i][k]*v[k][j];
     for (k=l;k<n;k++) v[k][j] += s*v[k][i];
    }
   }
   for (j=l;j<n;j++) v[i][j]=v[j][i]=0.0;
  }
  v[i][i]=1.0;
  g=rv1[i];
  l=i;
 }
 for (i=MIN(m,n)-1;i>=0;i--) {
  l=i+1;
  g=w[i];
  for (j=l;j<n;j++) u[i][j]=0.0;
  if (g != 0.0) {
   g=1.0/g;
   for (j=l;j<n;j++) {
    for (s=0.0,k=l;k<m;k++) s += u[k][i]*u[k][j];
    f=(s/u[i][i])*g;
    for (k=i;k<m;k++) u[k][j] += f*u[k][i];
   }
   for (j=i;j<m;j++) u[j][i] *= g;
  } else for (j=i;j<m;j++) u[j][i]=0.0;
  ++u[i][i];
 }
 for (k=n-1;k>=0;k--) {
  for (its=0;its<30;its++) {
   flag=true;
   for (l=k;l>=0;l--) {
    nm=l-1;
    if (l == 0 || abs(rv1[l]) <= eps*anorm) {
     flag=false;
     break;
    }
    if (abs(w[nm]) <= eps*anorm) break;
   }
   if (flag) {
    c=0.0;
    s=1.0;
    for (i=l;i<k+1;i++) {
     f=s*rv1[i];
     rv1[i]=c*rv1[i];
     if (abs(f) <= eps*anorm) break;
     g=w[i];
     h=pythag(f,g);
     w[i]=h;
     h=1.0/h;
     c=g*h;
     s = -f*h;
     for (j=0;j<m;j++) {
      y=u[j][nm];
      z=u[j][i];
      u[j][nm]=y*c+z*s;
      u[j][i]=z*c-y*s;
     }
    }
   }
   z=w[k];
   if (l == k) {
    if (z < 0.0) {
     w[k] = -z;
     for (j=0;j<n;j++) v[j][k] = -v[j][k];
    }
    break;
   }
   if (its == 29) throw("no convergence in 30 svdcmp iterations");
   x=w[l];
   nm=k-1;
   y=w[nm];
   g=rv1[nm];
   h=rv1[k];
   f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
   g=pythag(f,1.0);
   f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
   c=s=1.0;
   for (j=l;j<=nm;j++) {
    i=j+1;
    g=rv1[i];
    y=w[i];
    h=s*g;
    g=c*g;
    z=pythag(f,h);
    rv1[j]=z;
    c=f/z;
    s=h/z;
    f=x*c+g*s;
    g=g*c-x*s;
    h=y*s;
    y *= c;
    for (jj=0;jj<n;jj++) {
     x=v[jj][j];
     z=v[jj][i];
     v[jj][j]=x*c+z*s;
     v[jj][i]=z*c-x*s;
    }
    z=pythag(f,h);
    w[j]=z;
    if (z) {
     z=1.0/z;
     c=f*z;
     s=h*z;
    }
    f=c*g+s*y;
    x=c*y-s*g;
    for (jj=0;jj<m;jj++) {
     y=u[jj][j];
     z=u[jj][i];
     u[jj][j]=y*c+z*s;
     u[jj][i]=z*c-y*s;
    }
   }
   rv1[l]=0.0;
   rv1[k]=f;
   w[k]=x;
  }
 }
}

void SVD::reorder() {
 Int i,j,k,s,inc=1;
 Doub sw;
 VecDoub su(m), sv(n);
 do { inc *= 3; inc++; } while (inc <= n);
 do {
  inc /= 3;
  for (i=inc;i<n;i++) {
   sw = w[i];
   for (k=0;k<m;k++) su[k] = u[k][i];
   for (k=0;k<n;k++) sv[k] = v[k][i];
   j = i;
   while (w[j-inc] < sw) {
    w[j] = w[j-inc];
    for (k=0;k<m;k++) u[k][j] = u[k][j-inc];
    for (k=0;k<n;k++) v[k][j] = v[k][j-inc];
    j -= inc;
    if (j < inc) break;
   }
   w[j] = sw;
   for (k=0;k<m;k++) u[k][j] = su[k];
   for (k=0;k<n;k++) v[k][j] = sv[k];

  }
 } while (inc > 1);
 for (k=0;k<n;k++) {
  s=0;
  for (i=0;i<m;i++) if (u[i][k] < 0.) s++;
  for (j=0;j<n;j++) if (v[j][k] < 0.) s++;
  if (s > (m+n)/2) {
   for (i=0;i<m;i++) u[i][k] = -u[i][k];
   for (j=0;j<n;j++) v[j][k] = -v[j][k];
  }
 }
}

Doub SVD::pythag(const Doub a, const Doub b) {
 Doub absa=abs(a), absb=abs(b);
 return (absa > absb ? absa*sqrt(1.0+SQR(absb/absa)) :
  (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb))));
}

 

struct Fitsvd {
 Int ndat, ma;
 Doub tol;
 VecDoub_I *x,&y,&sig;
 VecDoub (*funcs)(const Doub);
 VecDoub a;
 MatDoub covar;
 Doub chisq;

 Fitsvd(VecDoub_I &xx, VecDoub_I &yy, VecDoub_I &ssig,
 VecDoub funks(const Doub), const Doub TOL=1.e-12)
 : ndat(yy.size()), x(&xx), xmd(NULL), y(yy), sig(ssig),
 funcs(funks), tol(TOL) {}

 void fit() {
  Int i,j,k;
  Doub tmp,thresh,sum;
  if (x) ma = funcs((*x)[0]).size();
  else ma = funcsmd(row(*xmd,0)).size();
  a.resize(ma);
  covar.resize(ma,ma);
  MatDoub aa(ndat,ma);
  VecDoub b(ndat),afunc(ma);
  for (i=0;i<ndat;i++) {
   if (x) afunc=funcs((*x)[i]);
   else afunc=funcsmd(row(*xmd,i));
   tmp=1.0/sig[i];
   for (j=0;j<ma;j++) aa[i][j]=afunc[j]*tmp;
   b[i]=y[i]*tmp;
  }
  SVD svd(aa);
  thresh = (tol > 0. ? tol*svd.w[0] : -1.);
  svd.solve(b,a,thresh);
  chisq=0.0;
  for (i=0;i<ndat;i++) {
   sum=0.;
   for (j=0;j<ma;j++) sum += aa[i][j]*a[j];
   chisq += SQR(sum-b[i]);
  }
  for (i=0;i<ma;i++) {
   for (j=0;j<i+1;j++) {
    sum=0.0;
    for (k=0;k<ma;k++) if (svd.w[k] > svd.tsh)
     sum += svd.v[i][k]*svd.v[j][k]/SQR(svd.w[k]);
    covar[j][i]=covar[i][j]=sum;
   }
  }

 }

 MatDoub_I *xmd;
 VecDoub (*funcsmd)(VecDoub_I &);

 Fitsvd(MatDoub_I &xx, VecDoub_I &yy, VecDoub_I &ssig,
 VecDoub funks(VecDoub_I &), const Doub TOL=1.e-12)
 : ndat(yy.size()), x(NULL), xmd(&xx), y(yy), sig(ssig),
 funcsmd(funks), tol(TOL) {}

 VecDoub row(MatDoub_I &a, const Int i) {
  Int j,n=a.ncols();
  VecDoub ans(n);
  for (j=0;j<n;j++) ans[j] = a[i][j];
  return ans;
 }
}; 



enum QueryState{NOQUERY, QUERY};

class CLArgParserV1N3{

public:
 //CONSTRUCTORS
 CLArgParserV1N3() {}  //default constructor
 CLArgParserV1N3(int argc, char *argv[], QueryState shouldprocess = NOQUERY, std::string htxt = "");  //takes the arguments to main parses them into a multimap 
 CLArgParserV1N3(const CLArgParserV1N3 &paradigm); //copy constructor
 
 //PRINT FUNCTIONS
 void printCLArgs() const;  //prints out the elements of the multimap on the CL
 void incorrectUsage() const; //prints out the help text with the program is executed incorrectly and exits the program

 //GET FUNTIONS
 std::vector<std::string> getParameter(std::string flag) const;  //returns the value(s) assigned to flag; throws an exception if flag is not found
 void getCLFileNamePtrList(std::string flag, std::vector<FileNameV1N1<std::string> *> &ptrlist) const;  //returns a list of value(s) assigned to flag; throws exception if flag is not found
 int getNumberOfArgs() const {return args.size();}  //return the number of flag/arg pairs
 std::string getSingleParameter(std::string flag) const; //returns the value of a flag known to have a single value; if flag is not found, return empty string
 std::string getHelpText() const {return helptext;}
 const std::multimap<std::string, std::string> &getArgs() const {return args;}  //returns a const reference to the args multimap

 //CHECK FUNCTIONS
 bool doesFlagExist(std::string flag) const {return (((this->args).find(flag)) != ((this->args).end()));}  //returns whether or not the flag exists within the multimap (remember that find returns end() if nothing is found)

protected:
 std::multimap<std::string, std::string> args;  //stores the (flag,value) pairs
 std::string helptext;  //optional help text which is displayed when the -help flag is given on the CL

};


template<class T = char>  //template argument defaults to char b/c it is the most simple 1 byte datatype
class FileIOV1N1{

public:
 //CONSTRUCTORS
 FileIOV1N1(); //default (empty) constructor
 FileIOV1N1(const FileNameV1N1<> *fobj);  //constructor that assigns the filename
 ~FileIOV1N1();

 //GET FUNCTIONS
 std::string getFilename() const {return (filename->getOSCompatibleFileName());}
 
 //SET FUNCTIONS
 void setFileName(const FileNameV1N1<> *fobj);
 
 //PRINT FUNCTIONS
 void printFilenameCL() const;

protected:
 FileNameV1N1<> *filename;

};

template<class T>
FileIOV1N1<T>::FileIOV1N1()
//default (empty) constructor
{
 filename = NULL;
}

template<class T>
FileIOV1N1<T>::FileIOV1N1(const FileNameV1N1<> *fobj)  
//constructor that assigns the filename based on fobj
{
 (this->filename) = NULL;
 this->setFileName(fobj);
}

template<class T>
FileIOV1N1<T>::~FileIOV1N1()
//destructor
{
 if(filename != NULL)
 {
  delete filename;
  filename = NULL;
 }
}


template<class T>
void FileIOV1N1<T>::setFileName(const FileNameV1N1<> *fobj)
//sets the filename to the FileNameV1N1<> obj
{
 //deletes the current data if not empty
 if(filename != NULL)
 {
  delete filename;
  filename = NULL;
 }

 (this->filename) = new FileNameV1N1<>(*fobj);  //uses FileNameV1N1 overloaded assignment operator

}


//PRINT FUNCTIONS
template<class T>
void FileIOV1N1<T>::printFilenameCL() const
//print filename to the CL
{
 std::cout<<"Filename: "<<filename->getOSCompatibleFileName()<<std::endl;

}


template<class T = char>  //template argument defaults to char b/c it is the most simple 1 byte datatype
class WriteToFileV1N1 : public FileIOV1N1<T>{

public:
 //CONSTRUCTORS
 WriteToFileV1N1(); //default (empty) constructor
 WriteToFileV1N1(const FileNameV1N1<> *fptr);  //constructor that assigns the filename
 ~WriteToFileV1N1();  //destructor

 //GET FUNCTIONS
 std::ofstream& getNonConstWriteFileStream() {return writefilestream;}

 //OPEN/CLOSE FUNCTIONS
 bool openForWrite(std::ios::openmode opentype = std::ios::out);  //opens the file for writing; default openmode is std::ios::out
 void close() {writefilestream.close();}  //explicitly close the file

 //WRITE FUNCTIONS
 template<class S>
 void writeObjectToFile(S obj);  //writes object to the text file, assumes that the object has the operator << defined
 
 template<class S>  //a member template
 void writeConvertedVectorToFile(const std::vector<S> &data); //writes the entire vector to the output file, one entry per line.  Assumes that the object has been 
 //constructed successfully and attached the file stream to a valid file.  Opens the obj for write and closes it.

protected:
 std::ofstream writefilestream;
 

};

//CONSTRUCTORS

template<class T>
WriteToFileV1N1<T>::WriteToFileV1N1()
//default (empty) constructor
:FileIOV1N1<T>()
{

}

template<class T>
WriteToFileV1N1<T>::WriteToFileV1N1(const FileNameV1N1<> *fptr)
//constructor that assigns the filename 
:FileIOV1N1<T>(fptr)
{

}



//DESTRUCTORS
template<class T>
WriteToFileV1N1<T>::~WriteToFileV1N1()
{
 writefilestream.close();
}

//OPEN/CLOSE FUNCTIONS
template<class T>
bool WriteToFileV1N1<T>::openForWrite(std::ios::openmode opentype)
//function which opens a file that is meant to be read from.  Note that the file must exist for this to work. If file is not found or object construction fails, 
//then it returns false; true otherwise
{

#if VALIDATE
 std::cout<<"From function void WriteToFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
 std::cout<<"Filename: "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif

 writefilestream.open(((this->filename)->getOSCompatibleFileName()).c_str(), opentype);

 if(!(writefilestream.is_open()))
 {

#if ERRORCHECK
  std::cout<<"ERROR in void WriteToFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for writing.  Exiting..."<<std::endl;
  exit(1);
#endif

  std::cout<<"WARNING in void WriteToFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for writing."<<std::endl;
  return false;
  
 }

#if VERBOSE
 std::cout<<"File = "<<(this->filename)->getOSCompatibleFileName()<<" Opened Successfully!"<<std::endl;
#endif

 return true;


}


//WRITE FUNCTIONS

template<class T>
template<class S>
void WriteToFileV1N1<T>::writeObjectToFile(S obj)  
//writes object to the text file, assumes that the object has the operator << defined
{
 writefilestream << obj <<"\n";

#if ERRORCHECK
 if(writefilestream.bad())
 {
  std::cout<<"ERROR in void WriteToFileV1N1<T>::writeObjectToFile(S obj)  "<<std::endl;
  std::cout<<"Writing Operation has Failed.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
}
 


template<class T>
template<class S>
void WriteToFileV1N1<T>::writeConvertedVectorToFile(const std::vector<S> &data)
//writes the entire vector to the output file, one entry per line.  Assumes that the object has been constructed successfully and attached the 
//file stream to a valid file.  This general form of the function assumes that a static_cast conversion exists between
//T = std::string and S.  For special conversions, I have instantiated special template functions below
{
 std::string cline;

 for(int idx = 0; idx < data.size(); idx++)
 {
  cline = static_cast<T>(data.at(idx));  //assumes that a static_cast conversion exists between S and T
  
  if(!cline.empty())  //empty lines are skipped
  {
   this->writeStringToFile(cline);
  }

 }
  
 
}



template<class T =  char>  //template argument defaults to char b/c it is the most simple 1 byte datatype
class WriteToBinaryFileV1N1 : public FileIOV1N1<T>{

public:
 //CONSTRUCTORS
 WriteToBinaryFileV1N1(); //default (empty) constructor
 WriteToBinaryFileV1N1(const FileNameV1N1<> *fptr);  //constructor that assigns the filename
 ~WriteToBinaryFileV1N1();  //destructor

 //OPEN/CLOSE FUNCTIONS
 bool openForWrite(std::ios::openmode opentype = std::ios::out | std::ios::binary);  //opens the file for writing; default openmode is std::ios::in and std::ios::binary
 void close() {writefilestream.close();}  //explicitly close the file

 //WRITE FUNCTIONS
 void writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem = hageman::NOALLOCATE);  //writes current line to the text file

 //SEEK FUNCTIONS
 void placePutStreamAtAbsolutePosition(const int FILEPOSITION) {writefilestream.seekp(FILEPOSITION);}  //puts the write position for the stream at FILEPOSITION

 //CHECK FUNCTIONS
 bool reachedEOF() {return (writefilestream.eof());}  //checks whether the ifstream has reached the EOF
 bool isOpen() {return (writefilestream.is_open());}  //checks whether the stream is open for reading

protected:
 std::ofstream writefilestream;
 

};

//CONSTRUCTORS
template<class T>
WriteToBinaryFileV1N1<T>::WriteToBinaryFileV1N1()
//default (empty) constructor
:FileIOV1N1<T>()
{

}

template<class T>
WriteToBinaryFileV1N1<T>::WriteToBinaryFileV1N1(const FileNameV1N1<> *fptr)
//constructor that assigns the filename
:FileIOV1N1<T>(fptr)
{

}

//DESTRUCTORS
template<class T>
WriteToBinaryFileV1N1<T>::~WriteToBinaryFileV1N1()
{
 writefilestream.close();
}

//OPEN/CLOSE FUNCTIONS
template<class T>
bool WriteToBinaryFileV1N1<T>::openForWrite(std::ios::openmode opentype)
//function which opens a file that is meant to be read from.  Note that the file must exist for this to work. If file is not found or object construction fails, 
//then it returns false; true otherwise
{

#if VALIDATE
 std::cout<<"From function void WriteToBinaryFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
 std::cout<<"Filename: "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif


 writefilestream.open(((this->filename)->getOSCompatibleFileName()).c_str(), opentype);

 if(!(writefilestream.is_open()))
 {
#if ERRORCHECK
  std::cout<<"ERROR in void WriteToBinaryFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for writing.  Exiting..."<<std::endl;
  exit(1);
#endif

  std::cout<<"WARNING in void WriteToBinaryFileV1N1<T>::openForWrite(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for writing."<<std::endl;
  return false;

 }

#if VERBOSE
 std::cout<<"File = "<<(this->filename)->getOSCompatibleFileName()<<" Opened Successfully!"<<std::endl;
#endif

 return true;
   
 
}

//WRITE FUNCTIONS
template<class T>
void WriteToBinaryFileV1N1<T>::writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem)
//write the current line from the text file
{

 if(amem != hageman::NOALLOCATE)
 {
  hageman::allocateMem(buffer,WRITESIZE,amem);
 }

 writefilestream.write(buffer,WRITESIZE);

 
 if(this->reachedEOF())
 {
#if ERRORCHECK
  std::cout<<"ERROR in void WriteToBinaryFileV1N1<T>::writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem)"<<std::endl;
  std::cout<<"End of File "<<(this->filename)->getOSCompatibleFileName()<<" has been reached.  Exiting..."<<std::endl;
  exit(1);
#endif

#if VERBOSE
  std::cout<<"WARNING in void WriteToBinaryFileV1N1<T>::writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem)"<<std::endl;  
  std::cout<<"End of File = "<<(this->filename)->getOSCompatibleFileName()<<" has been reached."<<std::endl;
#endif

 }

 if(writefilestream.bad())
 {

#if ERRORCHECK
  std::cout<<"ERROR in void WriteToBinaryFileV1N1<T>::writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem)"<<std::endl;
  std::cout<<"Writing Operation has Failed for File = "<<(this->filename)->getOSCompatibleFileName()<<" Exiting..."<<std::endl;
  exit(1);
#endif
 
#if VERBOSE
  std::cout<<"WARNING in void WriteToBinaryFileV1N1<T>::writeBytesToFileStream(char *buffer, const int WRITESIZE, hageman::AllocateMem amem)"<<std::endl;  
  std::cout<<"Writing Operation has Failed for File = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif

 }

}


template<class T = std::string>  //template argument defaults to char b/c it is the most simple 1 byte datatype 
class ReadFromFileV1N1 : public FileIOV1N1<T>{

public:
 //CONSTRUCTORS
 ReadFromFileV1N1(); //default (empty) constructor
 ReadFromFileV1N1(const FileNameV1N1<> *fobj);  //constructor that assigns the filename
 ~ReadFromFileV1N1();  //destructor

 //OPEN/CLOSE FUNCTIONS
 bool openForRead(std::ios::openmode opentype = std::ios::in);  //opens the ifstream for reading; default openmode is std::ios::in
 void close() {readfilestream.close();}  //explicitly close the ifstream

 //READ FUNCTIONS
 std::string readCurrentLineFromStream();  //reads the current line from the text file.  if the stream has reached the eof, then it doesn't read anything and returns an empty string.
 std::string readProcessedCurrentLineFromStream(); //reads and processes the current line from the text file
 //std::string peekAtCurrentLineFromStream();  //peeks at the current line from the text file without reading it (TO BE IMPLEMENTED)
 void readFromFileConvertIntoFileNamePtrList(std::vector<FileNameV1N1<T>* > &fptrlist);  //reads in a txt file of filenames and converts each line into a FileNameV1N1 object ptr.
 //the text file is assumed to have one filename per line
 
 template<class S>  //a member template
 void readFromFileConvertIntoVector(std::vector<S> &cvec); //reads the entire file into a vector of type S and returns it.  Assumes that the object has been 
 //constructed successfully and attached the file stream to a valid file.  Assumes that every line of the txt file is one object

 //CHECK FUNCTIONS
 bool reachedEOF() {return (readfilestream.eof());}  //checks whether the ifstream has reached the EOF
 bool isOpen() {return (readfilestream.is_open());}  //checks whether the stream is open for reading

 //GET FUNCTIONS
 int getCurrentStreamPos() {return (static_cast<int>(readfilestream.tellg()));}
 //SEEK FUNCTIONS
 //void placeGetStreamAtAbsolutePosition(const int FILEPOSITION) {readfilestream.seekg(FILEPOSITION);}  //puts the read position for the stream at FILEPOSITION
 //void placeGetStreamAtStartOfFile() {readfilestream.seekg(std::ios::beg);}  //puts the read position for the stream at the beginning of the file

 //PROCESSING FUNCTIONS
 
protected:
 std::ifstream readfilestream;
 

};

//CONSTRUCTORS
template<class T>
ReadFromFileV1N1<T>::ReadFromFileV1N1()
//default (empty) constructor
:FileIOV1N1<T>()
{

 
}

template<class T>
ReadFromFileV1N1<T>::ReadFromFileV1N1(const FileNameV1N1<> *fobj)
//constructor that assigns the filename
:FileIOV1N1<T>(fobj)
{

}


template<class T>
ReadFromFileV1N1<T>::~ReadFromFileV1N1()
//destructor
{
 
 readfilestream.close();

}


//OPEN/CLOSE FUNCTIONS
template<class T>
bool ReadFromFileV1N1<T>::openForRead(std::ios::openmode opentype)
//function which opens a file that is meant to be read from.  Note that the file must exist for this to work. If file is not found or object construction fails, then it returns false;
//true otherwise
{
#if VALIDATE
 std::cout<<"From function void ReadFromFileV1N1<T>::openForRead(std::ios::openmode opentype)"<<std::endl;
 std::cout<<"Filename: "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif

 readfilestream.open(((this->filename)->getOSCompatibleFileName()).c_str(), opentype);

 if(!(readfilestream.is_open()))
 {

#if ERRORCHECK
  std::cout<<"ERROR in ReadFromFileV1N1<T>::openforRead()"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for reading.  Exiting..."<<std::endl;
  exit(1);
#endif

  std::cout<<"WARNING in ReadFromFileV1N1<T>::openforRead()"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for reading."<<std::endl;
  return false;
  
 }

#if VERBOSE
 std::cout<<"File = "<<(this->filename)->getOSCompatibleFileName()<<" Opened Successfully!"<<std::endl;
#endif

 return true;

}


//READ FUNCTIONS
template<class T>
std::string ReadFromFileV1N1<T>::readCurrentLineFromStream()  
//reads the current line from the text file and returns it.  if the stream has reached the eof, then it doesn't read anything and returns an empty string.
{

 std::string result;
 char readbuffer[_CVTBUFSIZE];

 if(!readfilestream.eof())  //if the stream has reached the eof, then it doesn't read anything and returns an empty string.
 {
  readfilestream.getline(readbuffer, _CVTBUFSIZE);
  result = std::string(readbuffer);

 }

#if ERRORCHECK
 if(readfilestream.bad())
 {
  std::cout<<"ERROR in ReadFromFileV1N1::readCurrentLineFromFileStream()"<<std::endl;
  std::cout<<"Reading Operation has Failed.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 return result;


}

template<class T>
std::string ReadFromFileV1N1<T>::readProcessedCurrentLineFromStream() 
//reads and processes the current line from the text file
{
 std::string result(this->readCurrentLineFromStream());
 hageman::removePrecedingWhitespaces(result);
 hageman::removeTrailingWhitespaces(result);
 
 return result;
 
}

template<class T>
void ReadFromFileV1N1<T>::readFromFileConvertIntoFileNamePtrList(std::vector<FileNameV1N1<T>* > &fptrlist)
//reads in a txt file of filenames and converts each line into a FileNameV1N1 object ptr.  the text file is assumed to have one filename per line
{
 std::vector<std::string> strlist;
 this->readFromFileConvertIntoVector<T>(strlist);  //reads in each line of the txt file and converts it to a string

 hageman::convertStringVectorToFileNamePtrList(strlist,fptrlist);


}
 


template<class T>
template<class S>
void ReadFromFileV1N1<T>::readFromFileConvertIntoVector(std::vector<S> &cvec)
//reads the entire file into a vector of type S and returns it.  This general form of the function assumes that a static_cast conversion exists between
//T=std::string and S.  For special conversions, I have instantiated special template functions below.  Assumes that every line of the txt file is one object
{
 //assumes the stream has already been opened
 std::string cline;

 while(!(this->reachedEOF()))  //reads until EOF
 {
  cline = this->readProcessedCurrentLineFromStream();
  if(!cline.empty())  //if the cline is empty, then do nothing
   cvec.push_back(static_cast<S>(cline));  //assumes a static_cast conversion exists
 }

}

template<>
template<>
void ReadFromFileV1N1<std::string>::readFromFileConvertIntoVector(std::vector<float> &cvec);
//this is a special form of the general function above for converting T=std::string into S = float using atof.  It reads the entire file into a vector 
//of type float and returns it.  

template<>
template<>
void ReadFromFileV1N1<std::string>::readFromFileConvertIntoVector(std::vector<double> &cvec);
//this is a special form of the general function above for converting T=std::string into S = double using atod.  It reads the entire file into a vector 
//of type double and returns it.  

template<>
template<>
void ReadFromFileV1N1<std::string>::readFromFileConvertIntoVector(std::vector<int> &cvec);
//this is a special form of the general function above for converting T=std::string into S = int using atoi.  It reads the entire file into a vector 
//of type int and returns it.  


template<class T = char>  //template argument defaults to char b/c it is the most simple 1 byte datatype 
class ReadFromBinaryFileV1N1 : public FileIOV1N1<T>{

public:
 //CONSTRUCTORS
 ReadFromBinaryFileV1N1(); //default (empty) constructor
 ReadFromBinaryFileV1N1(const FileNameV1N1<> *fnameobj);  //constructor that assigns the filename
 ~ReadFromBinaryFileV1N1();  //destructor

 //OPEN/CLOSE FUNCTIONS
 bool openForRead(std::ios::openmode opentype = std::ios::in | std::ios::binary);  //opens the file for reading; default is std::ios::in and std::ios::binary
 void close() {readfilestream.close();}  //explicitly close the file

 //READ FUNCTIONS
 void readFromFileStream(char *buffer, const int READSIZE, hageman::AllocateMem amem = hageman::NOALLOCATE);  //reads the READSIZE bytes from the file into buffer

 //SEEK FUNCTIONS
 void placeGetStreamAtAbsolutePosition(const int FILEPOSITION) {readfilestream.seekg(FILEPOSITION);}  //puts the read position for the stream at FILEPOSITION
 void skipBytes(const int numbytes) {readfilestream.ignore(numbytes);}  //extracts and discards a specified number of bytes (numbytes) or until the delim = EOF (default) is found,

 //CHECK FUNCTIONS
 bool reachedEOF() {return (readfilestream.eof());}  //checks whether the ifstream has reached the EOF
 bool isOpen() {return (readfilestream.is_open());}  //checks whether the stream is open for reading

protected:
 std::ifstream readfilestream;
 

};

//CONSTRUCTORS

template<class T>
ReadFromBinaryFileV1N1<T>::ReadFromBinaryFileV1N1()
//default (empty) constructor
:FileIOV1N1<T>()
{

}

template<class T>
ReadFromBinaryFileV1N1<T>::ReadFromBinaryFileV1N1(const FileNameV1N1<> *fnameobj)
//constructor that assigns the filename
:FileIOV1N1<T>(fnameobj)
{


}

template<class T>
ReadFromBinaryFileV1N1<T>::~ReadFromBinaryFileV1N1()
//destructor
{
 readfilestream.close();

}


//OPEN/CLOSE FUNCTIONS
template<class T>
bool ReadFromBinaryFileV1N1<T>::openForRead(std::ios::openmode opentype)
//function which opens a file that is meant to be read from.  Note that the file must exist for this to work. If file is not found or object construction fails, 
//then it returns false; true otherwise
{

#if VALIDATE
 std::cout<<"From function void ReadFromFileV1N1<T>::openForRead(std::ios::openmode opentype)"<<std::endl;
 std::cout<<"Filename: "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif

 readfilestream.open(((this->filename)->getOSCompatibleFileName()).c_str(), opentype);

 if(!(readfilestream.is_open()))
 {
#if ERRORCHECK
  std::cout<<"ERROR in bool ReadFromBinaryFileV1N1<T>::openForRead(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename: "<<((this->filename)->getOSCompatibleFileName())<<std::endl;
  std::cout<<"File could not be opened for reading.  Exiting..."<<std::endl;
  exit(1);
#endif

  std::cout<<"WARNING in bool ReadFromBinaryFileV1N1<T>::openForRead(std::ios::openmode opentype)"<<std::endl;
  std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
  std::cout<<"File could not be opened for reading."<<std::endl;
  return false;
 }

#if VERBOSE
 std::cout<<"Filename = "<<(this->filename)->getOSCompatibleFileName()<<" opened successfully"<<std::endl;
#endif

 return true;
   
 
}


//READ FUNCTIONS
template<class T>
void ReadFromBinaryFileV1N1<T>::readFromFileStream(char *buffer, const int READSIZE, hageman::AllocateMem amem)  
//reads the number of bytes from the binary file into buffer.
{
 if(amem != hageman::NOALLOCATE)
 {
  hageman::allocateMem(buffer,READSIZE,amem);
 }

 readfilestream.read(buffer,READSIZE);
 
 if(this->reachedEOF())
 {
#if ERRORCHECK
  std::cout<<"ERROR in ReadFromBinaryFileV1N1::readFromFileStream()"<<std::endl;
  std::cout<<"End of File "<<(this->filename)->getOSCompatibleFileName()<<" has been reached.  Exiting..."<<std::endl;
  exit(1);
#endif

#if VERBOSE
  std::cout<<"WARNING in bool ReadFromBinaryFileV1N1<T>::readFromFileStream(char *buffer, const int READSIZE, hageman::AllocateMem amem = hageman::NOALLOCATE)"<<std::endl;  
  std::cout<<"End of File = "<<(this->filename)->getOSCompatibleFileName()<<" has been reached."<<std::endl;
#endif

 }

 if(readfilestream.bad())
 {

#if ERRORCHECK
  std::cout<<"ERROR in ReadFromBinaryFileV1N1::readFromFileStream()"<<std::endl;
  std::cout<<"Reading Operation has Failed for File = "<<(this->filename)->getOSCompatibleFileName()<<" Exiting..."<<std::endl;
  exit(1);
#endif
 
#if VERBOSE
  std::cout<<"WARNING in bool ReadFromBinaryFileV1N1<T>::readFromFileStream(char *buffer, const int READSIZE, hageman::AllocateMem amem = hageman::NOALLOCATE)"<<std::endl;  
  std::cout<<"Reading Operation has Failed for File = "<<(this->filename)->getOSCompatibleFileName()<<std::endl;
#endif
 }
 
 
}


template<class T>
std::vector<std::string> setFNameStrListUsingDirectory(FileNameV1N1<T> *pathptr, std::string ext = "???")
//searches the path specified by pathptr, writes out all files that correspond to *.ext using the OS-dependent dir utility, and then creates a vector of filename strings for all the files
//found.
{
 std::vector<std::string> result;  //vector of filenames
 
 if(hageman::getOSType() == hageman::WIN)
 {
  //cmd should be "dir /B "path\*.ext" > fnamelist.txt
  std::string syscmd("dir /B \"");  //WIN dir cmd with /B to list the bare filenames
  //syscmd += pathptr->getOSCompatibleFileName();  
  syscmd += pathptr->getPath();  
  
  //syscmd += "\\*.";
  //syscmd += "*.";
  syscmd += "*";
  
  syscmd += ext;
  syscmd += "\"";
  syscmd += " > \"";
  //syscmd += pathptr->getOSCompatibleFileName();
  syscmd += pathptr->getPath();
  
  //syscmd += "\\";
  syscmd += "fnamelist.txt\"";

#if VALIDATE
  std::cout<<"From function std::vector<std::string> setFNameStrListUsingDirectory(FileNameV1N1<T> *pathptr, std::string ext = \"???\")"<<std::endl;
  std::cout<<"syscmd: "<<syscmd<<std::endl;
#endif
  system(syscmd.c_str());

  //reads in the text file with the filenames
  //FileNameV1N1<> *fptr = new WinFileNameV1N1<>(pathptr->getOSCompatibleFileName() + "\\fnamelist.txt");
  FileNameV1N1<> *fptr = new WinFileNameV1N1<>(pathptr->getPath() + "fnamelist.txt");
  
  ReadFromFileV1N1<std::string> flist(fptr);
  flist.openForRead();
  std::string currentline;
  
  WinFileNameV1N1<> ftemp;
  
  while(!flist.reachedEOF())   //reads until EOF
  {
   currentline = flist.readProcessedCurrentLineFromStream();
   if(currentline == "fnamelist.txt")  //step over the actual filename of the list itself
    continue;

#if VALIDATE
   std::cout<<"From function std::vector<std::string> setFNameStrListUsingDirectory(FileNameV1N1<T> *pathptr, std::string ext = \"???\")"<<std::endl;
   std::cout<<"Current line read: "<<currentline<<std::endl;
#endif
   if(!currentline.empty())  //skip empty lines
   {
    ftemp.parseInput(currentline);  //parses the arbitrary input
    if(!ftemp.hasPath())  //makes sure the path is identical to the search path
     ftemp.setPath(pathptr->getPath());
     //ftemp.setPath(pathptr->getOSCompatibleFileName());
    result.push_back(ftemp.getOSCompatibleFileName());
   }
    
  }
   
  delete fptr;

 }
 else if(hageman::getOSType() == hageman::OSX)
 {

  //cmd should be find path -name *.ext > path/fnamelist.txt  (changed to find from ls because the arg list too long error)
  std::string syscmd("find ");
  //syscmd += pathptr->getOSCompatibleFileName();
  syscmd += pathptr->getPath();
  
  syscmd += "*";
  syscmd += ext;
  syscmd += " > ";
  //syscmd += pathptr->getOSCompatibleFileName();
  syscmd += pathptr->getPath();
  
  syscmd += "fnamelist.txt";

#if VALIDATE
  std::cout<<"Cmd: "<<syscmd<<std::endl;
#endif

  system(syscmd.c_str());
  
  //reads in the text file with the filenames
  //FileNameV1N1<> *fptr = new MacFileNameV1N1<>(pathptr->getOSCompatibleFileName() + "/fnamelist.txt");
  FileNameV1N1<> *fptr = new MacFileNameV1N1<>(pathptr->getPath() + "fnamelist.txt");
  
  ReadFromFileV1N1<std::string> flist(fptr);
  flist.openForRead();
  std::string currentline;
  MacFileNameV1N1<> ftemp;
  
  while(!flist.reachedEOF())  //reads until EOF
  {
   currentline = flist.readProcessedCurrentLineFromStream();
   if(currentline == "fnamelist.txt")  //step over the actual filename of the list itself
    continue;

#if VALIDATE
   std::cout<<"From function void LeviathanArgParserV1N0::setSignalFNameListUsingSignalVolDirectory(std::string imglistfname)"<<std::endl;
   std::cout<<"Current line read: "<<currentline<<std::endl;
#endif

   if(!currentline.empty())  //skip empty lines
   {
    ftemp.parseInput(currentline);  //parses the arbitrary input
    if(!ftemp.hasPath())  //makes sure the path is identical to the search path
     ftemp.setPath(pathptr->getPath());
     //ftemp.setPath(pathptr->getOSCompatibleFileName());

    result.push_back(ftemp.getOSCompatibleFileName());
   }
    
  }
   
  delete fptr;


 }

#if VALIDATE
  std::cout<<"From function std::vector<std::string> setFNameListUsingDirectory(FileNameV1N1<T> *pathptr, std::string ext)"<<std::endl;
  std::cout<<"Signal Fname List: "<<std::endl;
  printOutSTLSequenceContainer<std::vector<std::string> >(result);
#endif

  

 return result;
 

}



template<class T>
class VolumeV2N4{

public:
 //CONSTRUCTORS
 VolumeV2N4();  //empty constructor
 
 //PRINT FUNCTIONS
 virtual void printCL() const = 0;

 //UTILITY FUNCTIONS
 inline int convertToIndex(const std::vector<int> &coords, const std::vector<int> &dims) const;  //for an n-dimensional volume, converts the coords vector 
 //into index coordinates, assuming standard matrix coordinates (row,column)
  inline std::vector<int> convertToCoords(int index, const std::vector<int> &dims);  //converts an index into a coordinate vector based on the dimensions,
 //assuming standard matrix coordinates (row,column)

 
protected:
 T *data;  //dynamic allocation for the volume data
 


};

template<class T>
VolumeV2N4<T>::VolumeV2N4()
//empty constructor
{

 
}

template<class T>
inline int VolumeV2N4<T>::convertToIndex(const std::vector<int> &coords, const std::vector<int> &dims) const
//for an n-dimensional volume, converts the coords vector into index coordinates 
{

 /*
 The rationale behind this algorithm is to first deal with the first two dimensions and then all the subsequent dimensions.  The index, which is the
 result starts at zero.  First, you add to index the column index times the row dimension.  Then, you add the row index.  This takes care of the
 first two dimensions.  The other successively higher dimensions are computed by adding to the index the coordinate index * the multiplicative sum 
 of the previous dimensions.  For example, if I have a 2x2x3 matrix with a coordinate vector of (1,0,1), then my index is [(2*0)+1](first two dims) + 
 (4*1)] = 5.

 NOTE:  this approach assumes that the index is obtained by counting the elements column by column, going across rows, then in successive dimensions.
 These are standard matrix coordinates (row,column)

 */
 int currentdim;
 int index = 0;

 //the next two statements take care of the first two rows.
 index = dims.at(0)*coords.at(1);  
 index += coords.at(0);

 if(dims.size() > 2)  //if the matrix is only 2D, then there is no need to evaluate further dimensions
 {
  for(int idx = 2; idx < static_cast<int>(coords.size()); idx++)  //loops over all the remaining dimensions
  {
   currentdim = 1;  //always sets currentdim back to 1 for the next iteration, which is the multiplicative sum of the previous dimensions
   for(int idx2 = idx-1; idx2 >=0; idx2--)  //calculates the multiplicative sum of the previous dimensions
    currentdim *= dims.at(idx2);

   index += coords.at(idx)*currentdim;  //multiplies cuurentdim by the coordinate index to get that dimension's contribution to the index.
  }
 }

 return index;



}

template<class T>
inline std::vector<int> VolumeV2N4<T>::convertToCoords(int index, const std::vector<int> &dims)
//for an n-dimensional volume, converts an index into a coordinate vector based on the dimensions
{
 /*
 the rationale behind this algorithm is to start with the last dimension and work back until you reach the first two (they are a special case).  At any
 dimension, the amount of elements required to change that dimension by one is equal to the multiplicative summation of the previous dimensions.  For example,
 if you have a 2x2x2 matrix, then the number of elements you need to add to increase the last dimension by one is 4 (2x2=4).  Therefore, at each
 dimension, the currentdimmag is defined as the multiplicative sum of the previous dimensions.  Then, you find out the magnitude of the coordinate in that
 dimension by finding the maximum whole number you can multiply currentdimmag while still begin less than index.  For example, in the above 2x2x2 matrix, if my index was
 7, then I would know that the last coordinate would have to be 1, since 4(currentdimmag)* 1 < index, which is the maximum whole number I can multiply
 4 by and still be less than the index.  Then, you add this number to the appropriate position in the coordinate vector, subtract the value from index, and 
 go on to the next dimension.  Finally, when you reach the last two dimensions, you solve for them both at once.  You set the currentdimmag to the column
 dimension.  Then, you multiply by the maximum whole number you can like above.  This will equal to the row index.  After you subtract, this from the index,
 you have the column index.

 NOTE:  this approach assumes that the index is obtained by counting the elements column by column, going across rows, then in successive dimensions.
 These are standard matrix coordinates (row,column)

 */

 int currentdimmag;
 int coord;
 std::vector<int> coords;

 for(int idx = dims.size()-1; idx >= 2; idx--)  //start at the last dimension and work backwards until you reach the first two dimensions (special case)
 {
  currentdimmag = 1;  //always need to reset it to 1 before the next iteration
  for(int idx2 = idx-1; idx2 >= 0; idx2--)  //calculate the multiplicative sum of the previous dimensions.
   currentdimmag *= dims.at(idx2);
  coord = 0;  
  while((coord*currentdimmag) <= index)  //find the maximum whole number you can multiply currentdimmag by and still be less than index.
   coord++;
  coord--;  //since the previous loop went one over, take away one.
  coords.insert(coords.begin(),coord);  //that whole number is equal to the index of the current dimension
  index -= coord*currentdimmag;  //make sure to subtract from index for the next iteration.
 }

 //this is the special case of finding the indices of the first two dimensions.
 currentdimmag = dims.at(0);  //here the currentdimmag is automatically set to the column dimension
 coord = 0;
 while((coord*currentdimmag) <= index)  //again, find the maximum whole number
   coord++;
 coord--;
 index -= coord*currentdimmag;
 coords.insert(coords.begin(),index); //after subtracting, you have the column index left over
 coords.insert(coords.begin()+1,coord);  //the number is the column index
 
 return coords;


}


template<class T>
class Volume2DV2N4 : public VolumeV2N4<T>{

public:
 //using statements allow overloaded non-virtual base class functions to be visible (otherwise, they are hidden)
 using VolumeV2N4<T>::convertToIndex;  //for an n-dimensional volume, converts the coords vector 
 //into index coordinates, assuming standard matrix coordinates (row,column)
  using VolumeV2N4<T>::convertToCoords;  //converts an index into a coordinate vector based on the dimensions,
 //assuming standard matrix coordinates (row,column)

 //CONSTRUCTORS
 Volume2DV2N4();  //default constructor
 Volume2DV2N4(int RDIMv, int CDIMv, T element = T());  //constructs a 2D matrix of size RDIM x CDIM and initializes with element (default = T())
 Volume2DV2N4(const Volume2DV2N4<T> &copy);  //copy constructor
 ~Volume2DV2N4(); //destructor  

 //ASSIGNMENT FUNCTIONS
 void initVolume2D(int RDIMv, int CDIMv, T element = T());  //allows you to initialize a Volume2D object that has been created by the default constructor
 void copyVolume2D(const Volume2DV2N4<T> &paradigm);  //copies paradigm into the current object
 void makeCompatible(const Volume2DV2N4<T> &paradigm, T element = T());  //reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all 
 //values to element.  If dimensions are already the same, does nothing.

 
 //GET FUNCTIONS
 const int getRDIM() const {return RDIM;}  //returns the dimensions of the matrix
 const int getCDIM() const {return CDIM;}
 const int getXDIM() const {return CDIM;}
 const int getYDIM() const {return RDIM;}
 const int getSIZE() const {return SIZE;}

 inline T& getNonConstElementReference(int i, int j);  //returns a non-const reference to an element
 inline T& getNonConstElementReference(int index);  //returns a non-const reference to an element
 const T* getConstDataReference() const {return (this->data);}  //returns a const reference to the data array
 T* getNonConstDataReference() {return (this->data);}  //returns a non-const reference to the data array
 inline const T& getElement(int i, int j) const;  //gets the const element at the position (i,j)
 const T& getElement(int index) const {return (this->data)[index];}  //gets the element at the index
 inline const T& getElementInStandardCoordSystem(int x, int y) const;  //gets the element at the position (x,y) which is the position (x=j, y=i) in the matrix coordinate system
 void getMatrixRow(T *dataarray, int ridx, hageman::AllocateMem amem = hageman::NOALLOCATE) const;  //this function reads the matrix row idx into dataarray, which is assumed to already be allocated with enough space to hold the matrix row


 //SET FUNCTIONS
 inline void setElement(int index, T element);  //sets the element at array index position "index" to element
 inline void setElement(int i, int j, T element);  //sets the element at (xidx,yidx) to element
 inline void setElementInStandardCoordSystem(int x, int y, T element); //sets the element at the position (x,y), which is (x=j,y=i) in the matrix coord system
 void setMatrixRow(const T *dataarray, int rownum); //copies dataarray into the row, rownum.  THIS FUNCTION DOES NO MEMORY ALLOCATION AND ASSUMES THE VOLUME CAN HANDLE THE ASSIGNMENT
 void setMatrixColumn(const T *dataarray, int colnum); //copies dataarray into the column, colnum.  THIS FUNCTION DOES NO MEMORY ALLOCATION AND ASSUMES THE VOLUME CAN HANDLE THE ASSIGNMENT
 void stackVolume(Volume2DV2N4<T> &result, const Volume2DV2N4<T> &tobestacked, hageman::AllocateMem amem = hageman::NOALLOCATE); //this function creates a new volume that is the current volume stacked on top of "tobestacked"

 //CHECK FUNCTIONS
 inline bool inBounds(int ridx, int cidx) const;  //checks to make sure the coords (ridx,cidx) are within the bounds of the volume
 inline bool areDimensionsEqual(const Volume2DV2N4<T> &op) const;  //check to see whether the dimensions of op are equal to the current object.
 bool isSquare() const {return (RDIM == CDIM);}
 bool isEmpty() const {return (this->getSIZE() == 0);} //checks whether the volume is empty
 
 //PRINT FUNCTIONS
 virtual void printCL() const; //prints out the matrix on the command line

 //CONVERSION FUNCTIONS
 inline int convertToIndex(int ridx, int cidx) const;  //since the data is represented by a linear array, this converts the coords vector (ridx,cidx) 
 //into index coordinates
 inline std::vector<int> convertToCoords(int index);  //converts an index into the matrix coordinate vector based on the dimensions

 //MATH FUNCTIONS
 void transpose(Volume2DV2N4<T> &result, hageman::AllocateMem amem = hageman::NOALLOCATE); //transposes the matrix and returns a Volume2DV2N4<T>
 

 //READ FUNCTIONS
 void readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, hageman::AllocateMem amem = hageman::NOALLOCATE);  //a function of for reading in a 2D volume from a txt file.  
 //This function DOES NOT create any space for vol unless allocatemem is set to true.  Otherwise, it assumes that vol contains the space 
 //necessary to read in ifname.  Make sure you initialize vol before calling this function.  I decided to do it this way to spare any further costly 
 //initialization.  Function assumes ifname has one value per line and assumes the following read order:  the reading starts with all
 //rows for the first column, then goes to the second column and reads all the rows for the second column and so on... 

 void readInFromTxtFile(const FileNameV1N1<> *ifname, int CDIMv = 0, hageman::AllocateMem amem = hageman::NOALLOCATE, std::string delim = ",");  //a function of for reading in a 2D volume from a txt file.  
 //This function DOES NOT create any space for vol unless allocatemem is set to true.  Otherwise, it assumes that vol contains the space 
 //necessary to read in ifname.  Make sure you initialize vol before calling this function.  I decided to do it this way to spare any further costly 
 //initialization.  Function assumes ifname is in matrix form with values separated by a space.  There is one row per line, starting with row 1.
 
 //OPERATOR OVERLOADS
 T& operator[](int index);  //overloaded [] operator.  Takes in an index number and returns the element.  WARNING: THIS RETURNS A NON-CONST REFERENCE TO THE ELEMENT
 void operator=(const Volume2DV2N4<T> &paradigm);  //overloaded = operator that copies paradigm into the current object
 bool operator==(const Volume2DV2N4<T> &paradigm) const;  //overloaded == operator that tests for equality between two objects of type T
 bool operator!=(const Volume2DV2N4<T> &paradigm) const;  //overloaded != operator that tests for equality between two objects of type T

 //FILESTREAMS
 ReadFromBinaryFileV1N1<T> ibfile;  //input stream for reading from a binary file
 ReadFromFileV1N1<T> ifile;  //uninitialized input stream for reading from a text file
 WriteToFileV1N1<T> ofile;  //uninitialized output stream for writing to a text file
 WriteToBinaryFileV1N1<T> obfile;  //output stream for writing to a binary file
 
protected:
 int RDIM;
 int CDIM;
 int SIZE;


private:
 //inline int convertToIndex(const std::vector<int> &coords, const std::vector<int> &dims) const {return (VolumeV2N4<T>::convertToIndex(coords,dims));}  //for an 
 //n-dimensional volume, converts the coords vector into index coordinates - a necessary reimplementation of the Volume class function
 //inline std::vector<int> convertToCoords(int index, const std::vector<int> &dims) {return (VolumeV2N4<T>::convertToCoords(index,dims));}  //converts an index into a 
 //coordinate vector based on the dimensions - - a necessary reimplementation of the Volume class function


 
};

//CONSTRUCTORS
template<class T>
Volume2DV2N4<T>::Volume2DV2N4()
//default constructor
:VolumeV2N4<T>(), RDIM(0), CDIM(0), SIZE(0)
{
 (this->data) = NULL;
}


template<class T>
Volume2DV2N4<T>::Volume2DV2N4(int RDIMv, int CDIMv, T element)
//creates a RDIMxCDIM matrix with data = element
:VolumeV2N4<T>(), RDIM(RDIMv), CDIM(CDIMv), SIZE(RDIMv*CDIMv)
{
 (this->data) = NULL;
 (this->data) = new T[SIZE];
 
 for(int idx = 0; idx < SIZE; idx++)
 {
  this->data[idx] = element;
 }
 
}



template<class T>
Volume2DV2N4<T>::Volume2DV2N4(const Volume2DV2N4<T> &copy)
//copy constructor: makes a copy of a Volume2DV2N1 object
:VolumeV2N4<T>(), RDIM(copy.getRDIM()), CDIM(copy.getCDIM()), SIZE((copy.getRDIM())*copy.getCDIM())
{
 this->data = NULL;
 this->data = new T[SIZE];
 
 //element-wise copy of the volume
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = copy.getElement(idx);

}

 
//ASSIGNMENT FUNCTIONS
template<class T>
void Volume2DV2N4<T>::initVolume2D(int RDIMv, int CDIMv, T element)  
//allows you to initialize a Volume2D object that has been created by the default constructor
{
 RDIM = RDIMv;
 CDIM = CDIMv;
 SIZE = RDIMv*CDIMv;

 //deletes any existing data
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }

 this->data = new T[SIZE];

 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = element;


}

template<class T>
void Volume2DV2N4<T>::copyVolume2D(const Volume2DV2N4<T> &paradigm)
//copies paradigm into the current object
{
 RDIM = paradigm.getRDIM();
 CDIM = paradigm.getCDIM();
 SIZE = (RDIM*CDIM);
 
 //deletes any existing data
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }

 this->data = new T[SIZE];
 
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = paradigm.getElement(idx);
}


template<class T>
void Volume2DV2N4<T>::makeCompatible(const Volume2DV2N4<T> &paradigm, T element) 
//reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all values to element
{
 if(!(this->areDimensionsEqual(paradigm)))  //if dimensions are already equal, then do nothing
 {
  this->initVolume2D(paradigm.getRDIM(), paradigm.getCDIM(), element);
 } 
}



//DESTRUCTORS
template<class T>
Volume2DV2N4<T>::~Volume2DV2N4()  
//destructor
{
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }
}

//GET FUNCTIONS

template<class T>
inline const T& Volume2DV2N4<T>::getElement(int ridx, int cidx) const  
//gets the element at the position (x,y)
{
 int index = this->convertToIndex(ridx,cidx);
 return this->data[index];


}

template<class T>
inline const T& Volume2DV2N4<T>::getElementInStandardCoordSystem(int x, int y) const  
//gets the element at the (x,y) which is the position (i=y, j=x) in this coordinate system
{
 int index = this->convertToIndex(y,x);
 return this->data[index];
}


template<class T>
inline T& Volume2DV2N4<T>::getNonConstElementReference(int i, int j)
{
 int index = this->convertToIndex(i,j);
 return this->data[index];
}


template<class T>
inline T& Volume2DV2N4<T>::getNonConstElementReference(int index)
{
#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume2DV2N4<T>::getReference(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get reference.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
    
 return this->data[index];
}



//SET FUNCTIONS
template<class T>
inline void Volume2DV2N4<T>::setElement(int index, T element)
//sets the element at array index position index to element
{

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume2DV2N4<T>::setElement(int index, T element)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot set element. Index is not valid."<<std::endl;
  exit(1);
 }
#endif
   
 this->data[index] = element;

}

//SET FUNCTIONS

template<class T>
inline void Volume2DV2N4<T>::setElement(int ridx, int cidx, T element)
//sets the element at (ridx,cidx) to element
{
#if ERRORCHECK
 if(!(inBounds(ridx,cidx)))
 {
  std::cout<<"ERROR in void Volume2DV2N4<T>::setElement(int ridx, int cidx, T element)"<<std::endl;
  std::cout<<"Position = ("<<ridx<<","<<cidx<<")"<<std::endl;
  std::cout<<"Cannot set element.  Index is not valid."<<std::endl;
  exit(1);
 }
#endif
   
 int index = this->convertToIndex(ridx,cidx);
 this->data[index] = element;
}

template<class T>
inline void Volume2DV2N4<T>::setElementInStandardCoordSystem(int x, int y, T element) 
//sets the element at position (i,j), which is (x=j,y=i) in my coord system 
{
 this->setElement(y,x,element);
}

template<class T>
void Volume2DV2N4<T>::setMatrixRow(const T *dataarray, int rownum) 
//copies dataarray into the row, rownum.  THIS FUNCTION DOES NO MEMORY ALLOCATION AND ASSUMES THE VOLUME CAN HANDLE THE ASSIGNMENT
{
 for(int cidx = 0; cidx < CDIM; cidx++)
 {
  this->setElement(rownum,cidx,dataarray[cidx]);
 }

}

template<class T>
void Volume2DV2N4<T>::setMatrixColumn(const T *dataarray, int colnum) 
//copies dataarray into the column, colnum.  THIS FUNCTION DOES NO MEMORY ALLOCATION AND ASSUMES THE VOLUME CAN HANDLE THE ASSIGNMENT
{
 for(int ridx = 0; ridx < RDIM; ridx++)
 {
  this->setElement(ridx,colnum,dataarray[ridx]);
 }

}

template<class T>
void Volume2DV2N4<T>::stackVolume(Volume2DV2N4<T> &result, const Volume2DV2N4<T> &tobestacked, AllocateMem amem) 
//this function creates a new volume that is the current volume stacked on top of "tobestacked" in the RDIM direction (the same CDIM)
{

 
#if ERRORCHECK
 if((this->getCDIM()) != (tobestacked.getCDIM()))
 {
  std::cout<<"ERROR in Volume2DV2N4<T> Volume2DV2N4<T>::stackVolume(const Volume2DV2N4 &tobestacked)"<<std::endl;
  std::cout<<"Cannot stack volumes that have different column dimensions.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 
 const int COMBORDIM = (this->getRDIM()) + (tobestacked.getRDIM());

 //allocates a volume with RDIM dimension equal to the sum of the two volumes to be stacked
 if(amem != hageman::NOALLOCATE)
 {
  result.initVolume2D(COMBORDIM,this->getCDIM());
 }

 T *currentrow = new T[this->getCDIM()];  //a temp structure to hold the current row to be copied

 //the current volume runs from (0 - (this->getRDIM)) along the RDIM direction
 for(int idx1 = 0; idx1 < this->getRDIM(); idx1++)
 {
  this->getMatrixRow(currentrow,idx1);
  result.setMatrixRow(currentrow,idx1);
 }

 //the parameter volume is stacked below and runs from (this->getRDIM) to ((this->getRDIM())+idx2)
 for(int idx2 = 0; idx2 < (tobestacked.getRDIM()); idx2++)
 {
  tobestacked.getMatrixRow(currentrow,idx2);
  result.setMatrixRow(currentrow,((this->getRDIM())+idx2));
 }

 delete[] currentrow;
 

}

template<class T>
inline void Volume2DV2N4<T>::getMatrixRow(T *dataarray, int ridx, AllocateMem amem) const
//this function reads the matrix row idx into dataarray, which is assumed to already be allocated with enough space to hold the matrix row, if amem is specified
{
 //allocates the memory, if no already allocated
 if(amem != hageman::NOALLOCATE)
 {
  hageman::allocateMem(dataarray,ridx,amem);
 }

 //copies the row into dataarray
 for(int cidx = 0; cidx < this->getCDIM(); cidx++)
 {
  dataarray[cidx] = this->getElement(ridx,cidx);
 }

}


//CHECK FUNCTIONS
template<class T>
inline bool Volume2DV2N4<T>::inBounds(int ridx, int cidx) const
//checks to make sure the coords (ridx,cidx) are within the bounds of the volume
{
 if((ridx < 0) || (ridx >= getRDIM()))
  return false;
 else if((cidx < 0) || (cidx >= getCDIM()))
  return false;
 
 return true;

}

template<class T>
inline bool Volume2DV2N4<T>::areDimensionsEqual(const Volume2DV2N4<T> &op) const
//check to see whether the dimensions of op are equal to the current object.
{
 if((RDIM == op.getRDIM()) && (CDIM == op.getCDIM()))
  return true;

 return false;

}

template<class T>
void Volume2DV2N4<T>::printCL() const 
//prints out the volume on the command line, one object per line, row by row across columns for each row
{

 for(int idx = 0; idx < SIZE; idx++)
  std::cout<<(this->data)[idx]<<std::endl;

 
}



//CONVERSION FUNCTIONS
template<class T>
inline std::vector<int> Volume2DV2N4<T>::convertToCoords(int index)
//since the data is represented by a linear array, this converts the index into a coords vector 
{

#if ERRORCHECK
 //first determines if index is in range.  If it isn't, the program throws an exception.
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in std::vector<int> Volume2DV2N4<T>::converttoCoords(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot convert to Coordinates.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 std::vector<int> dims;
 dims.push_back(RDIM);
 dims.push_back(CDIM);
 std::vector<int> coords(this->converttoCoords(index,dims));

 return coords;
 

}

template<class T>
inline int Volume2DV2N4<T>::convertToIndex(int ridx, int cidx) const
//converts a coordinate vector into an index based on the dimensions
{
#if ERRORCHECK
 //first makes sure that the coordinates are valid using the checkCoords function
 if(!(inBounds(ridx,cidx)))
 {
  std::cout<<"ERROR in int Volume2DV2N4<T>::convertToIndex(int ridx, int cidx) const"<<std::endl;
  std::cout<<"Position = ("<<ridx<<","<<cidx<<")"<<std::endl;
  std::cout<<"Cannot convert to index. Coordinates are not valid.  Exiting..."<<std::endl; 
  std::cout<<typeid(*this).name()<<std::endl;
  std::cout<<"Dimensions: "<<this->getRDIM()<<" "<<this->getCDIM()<<std::endl;
  exit(1);
 }
#endif
   
 

 std::vector<int> dims;
 dims.push_back(RDIM);
 dims.push_back(CDIM);
 
 std::vector<int> coords;
 coords.push_back(ridx);
 coords.push_back(cidx);
  
 int index = this->convertToIndex(coords,dims);
 return index;

}

//MATH FUNCTIONS

template<class T>
void Volume2DV2N4<T>::transpose(Volume2DV2N4<T> &result, AllocateMem amem)
//transposes the matrix and returns a Volume2DV2N4<T>
{
 if(amem != hageman::NOALLOCATE)
  result.initVolume2D(CDIM, RDIM);  //the transpose result


 for(int cidx = 0; cidx < CDIM; cidx++)
  for(int ridx = 0; ridx < RDIM; ridx++)
  {
   result.setElement(cidx,ridx,this->getElement(ridx,cidx));
  }

 
}
 

//READ FUNCTIONS
template<class T>
void Volume2DV2N4<T>::readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, AllocateMem amem)
//a function of for reading in a 2D volume from a txt file.  
//This function DOES NOT create any space for vol unless allocatemem is set to true.  Otherwise, it assumes that vol contains the space 
//necessary to read in ifname.  Make sure you initialize vol before calling this function.  I decided to do it this way to spare any further costly 
//initialization.  Function assumes ifname has one value per line and assumes the following read order:  the reading starts with all
//rows for the first column, then goes to the second column and reads all the rows for the second column and so on...  This is the general form of the
//function that assumes that there exists a static_cast between char* and T.  Specialized forms of this function are specified below
{
 
 if(amem != hageman::NOALLOCATE)
  this->initVolume2D(RDIMv,CDIMv);

#if ERRORCHECK
 if(SIZE != RDIMv*CDIMv)
 {
  std::cout<<"ERROR in void Volume2DV2N4<T>::readInFromTxtFile(std::string ifname, int RDIMv, int CDIMv, bool allocatemem)"<<std::endl;
  std::cout<<"SIZE = "<<this->getSIZE()<<std::endl;
  std::cout<<"Proper memory is not allocated for reading.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
#if VERBOSE
 std::cout<<"Reading in Txt File = "<<ifname->getOSCompatibleFileName()<<std::endl;
 std::cout<<"Total points to read = "<<this->SIZE<<std::endl;
#endif


 int ptsread = 0;
 //sets and initializes the read file stream
 ifile.setFilename(ifname);
 ifile.openForRead();
 
 std::string dataptstr;
 T datapt = T();

 for(int j = 0; j < CDIMv; j++)
  for(int i = 0; i < RDIMv; i++)
  {
#if VERBOSE
    if((ptsread % 10000) == 0)
     std::cout<<"Pts Read = "<<ptsread<<" of "<<SIZE<<std::endl;
#endif
    //reading in a datapt
    dataptstr = ifile.readProcessedCurrentLineFromStream();
    if(!dataptstr.empty())  //if an empty line is read in, it is ignored
    {
     datapt = static_cast<T>(dataptstr);
     this->setElement(i,j,datapt);
     ++ptsread;
    }
      
  }

 ifile.close();

#if VERBOSE
 std::cout<<ptsread<<" Points Read!"<<std::endl;
#endif

}

template<class T>
void Volume2DV2N4<T>::readInFromTxtFile(const FileNameV1N1<> *ifname, int CDIMv, AllocateMem amem, std::string delim)  
//a function of for reading in a 2D volume from a txt file.  This function DOES NOT create any space for vol.  It assumes that vol contains the space 
//necessary to read in ifname.  Make sure you initialize vol before calling this function.  I decided to do it this way to spare any further costly 
//initialization.  Function assumes ifname is in matrix form with values separated by a space.  There is one row per line, starting with row 1.  This is the 
//general form of the
//function that assumes that there exists a static_cast between char* and T.
{

 std::string currentdataline;
 int rowsread = 0;

 if(amem != hageman::NOALLOCATE)  //need to allocate memory to the current object
 {
  //WHY DO I HAVE TO CREATE A LOCAL OBJECT INSTEAD OF USING IFILE?  FOR SOME REASON, I CAN'T READ THE FILE TWICE WITH IFILE.
  hageman::ReadFromFileV1N1<std::string> tempread(ifname);
  tempread.openForRead();

  //reads to the EOF to determine the number of rows to allocate
  while(!tempread.reachedEOF())
  {
   currentdataline = tempread.readProcessedCurrentLineFromStream();
   if(!currentdataline.empty())  //If an empty line is read in, it is ignored
   {
    rowsread++;
   }

  }

  this->initVolume2D(rowsread,CDIMv);
  //ifile.placeGetStreamAtStartOfFile();  DOESNT WORK FOR SOME REASON

  rowsread = 0;  //sets rows read back to 0 for the next read round

  tempread.close();
 }

 //sets and initializes the read stream
 ifile.setFileName(ifname);
 ifile.openForRead();
 
 T *dataptarray = new T[this->getCDIM()];  //holds the current row
  
#if ERRORCHECK
 std::cout<<"Reading in Formatted Txt File = "<<ifname->getOSCompatibleFileName()<<std::endl;
 std::cout<<"Total rows to read = "<<this->getRDIM()<<std::endl;
#endif
  
 for(int i = 0; i < (this->getRDIM()); i++)
 {

#if VERBOSE
  if((rowsread % 10000) == 0)
  std::cout<<"Rows Read = "<<rowsread<<" of "<<this->getRDIM()<<std::endl;
#endif

  //reading in a line
  currentdataline = ifile.readProcessedCurrentLineFromStream();
  if(!currentdataline.empty())  //If an empty line is read in, it is ignored
  {
   parseLineOfNumbers<T>(dataptarray,CDIM,currentdataline,delim,hageman::NOALLOCATE);  //parses the currentdataline based on delim character
   this->setMatrixRow(dataptarray,i);  
   ++rowsread;
  }
      
 }

 ifile.close();

#if VERBOSE
  std::cout<<rowsread<<" Rows Read!"<<std::endl;
#endif
 delete[] dataptarray;
 
 

}

//OPERATOR OVERLOADS
template<class T>
T& Volume2DV2N4<T>::operator[](int index)
//overloaded [] operator.  Takes in an index number and returns the element
{
#if ERRORCHECK 
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"Error in T& Volume2DV2N4<T>::operator[](int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot access element.  Index is not valid."<<std::endl;
  exit(1);
 }
#endif
 
 return this->data[index];
}

template<class T>
void Volume2DV2N4<T>::operator=(const Volume2DV2N4<T> &paradigm)
//overloaded = operator that copies paradigm into the current object
{
 this->copyVolume2D(paradigm);
}

template<class T>
bool Volume2DV2N4<T>::operator==(const Volume2DV2N4<T> &paradigm) const 
//overloaded == operator that tests for equality between two objects of type T
{
 
#if ERRORCHECK
 if(!(this->areDimensionsEqual(paradigm)))
 {
  std::cout<<"ERROR in bool Volume2DV2N4<T>::operator==(const Volume2DV2N4<T> &paradigm) const"<<std::endl;
  std::cout<<"Cannot compare volumes; dimensions are not equal.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 

 for(int idx = 0; idx < SIZE; idx++)
 {
  if(!((this->getElement(idx)) == (paradigm.getElement(idx))))
   return false;
 }

 return true;

}

template<class T>
bool Volume2DV2N4<T>::operator!=(const Volume2DV2N4<T> &paradigm) const  
//overloaded != operator that tests for equality between two objects of type T
{
 return (!((*this) == paradigm));

}


template<class T>
std::ostream& operator<<(std::ostream& os, const hageman::Volume2DV2N4<T> &output)
{
 
 for(int idx = 0; idx < output.getSIZE(); idx++)
  os << output[idx] << "\n";

 return os;


}


template<class T>
class EigenSystemV1N4{

public:
 //CONSTRUCTORS
 EigenSystemV1N4();  //empty constructor
 
 //PRINT FUNCTIONS
 virtual void printCL() const = 0;


protected:

};

//CONSTRUCTORS
template<class T>
EigenSystemV1N4<T>::EigenSystemV1N4()
//empty constructor
{

}


template<class T>
class EigenSystem3DV1N4 : public EigenSystemV1N4<T>{

public:
 //CONSTRUCTORS
 EigenSystem3DV1N4();  //empty constructor
 EigenSystem3DV1N4(T *data); //initializes the eigensystem with an array[12] assumed to be in the following format: [lambda1 lambda2 lambda3 nu1x nu1y nu1z nu2x nu2y nu2z nu3x nu3y nu3z]
 EigenSystem3DV1N4(const EigenSystem3DV1N4<T> &tobecopied);  //copy constructor
 EigenSystem3DV1N4(T l1v, T l2v, T l3v, const T v1v[3], const T v2v[3], const T v3v[3]);  //constructor which initializes all data
 void initEigenSystem3D(T l1v, T l2v, T l3v, const T v1v[3], const T v2v[3], const T v3v[3]);  //initializes the current object with these parameters
 void initEigenSystem3D(T *data); //initializes the eigensystem with an array[12] assumed to be in the following format: [lambda1 lambda2 lambda3 nu1x nu1y nu1z nu2x nu2y nu2z nu3x nu3y nu3z]
 void initEigenSystem3D(const EigenSystem3DV1N4<T> &temp); //initializes the current object to be equal to temp
 void initAverageSystem3D(const EigenSystem3DV1N4<T> &elem1, const EigenSystem3DV1N4<T> &elem2); //initializes the eigensystem as the average of elem1 and elem2

 //MATH FUNCTIONS
 
 

 //DTI MATH FUNCTIONS
 T meanDiffusivity() const {return (.33*(l1+l2+l3));}
 T selfRegularTensorProduct() const;
 T selfDeviatoricTensorProduct() const;
 T regularTensorProduct(const EigenSystem3DV1N4<T> &op) const;
 T deviatoricTensorProduct(const EigenSystem3DV1N4<T> &op) const;
 T latticeIndex(const EigenSystem3DV1N4<T> &op) const;
 T fractionalAnisotropy() const;
 void correctForDTI();


 //PRINT FUNCTIONS
 virtual void printCL() const;

 //data is public to allow direct access
 T l1;
 T l2;
 T l3;
 T v1[3];
 T v2[3];
 T v3[3];

 //OVERLOADED OPERATORS
 void operator=(const EigenSystem3DV1N4<T> &tobecopied);

};

template<class T>
EigenSystem3DV1N4<T>::EigenSystem3DV1N4()
//empty default constructor
:EigenSystemV1N4<T>(), l1(T()), l2(T()), l3(T())
{
 //initializes the data and vectors to zero
 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = T();
  v2[idx] = T();
  v3[idx] = T();
 }

}

template<class T>
EigenSystem3DV1N4<T>::EigenSystem3DV1N4(T *data) 
//initializes the eigensystem with an array[12] assumed to be in the following format: [lambda1 lambda2 lambda3 nu1x nu1y nu1z nu2x nu2y nu2z nu3x nu3y nu3z]
:EigenSystemV1N4<T>(), l1(data[0]), l2(data[1]), l3(data[2])
{
 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = data[idx+3];
  v2[idx] = data[idx+6];
  v3[idx] = data[idx+9];
 }

}


template<class T>
EigenSystem3DV1N4<T>::EigenSystem3DV1N4(const EigenSystem3DV1N4<T> &tobecopied)  
//copy constructor
{

 l1 = tobecopied.l1;
 l2 = tobecopied.l2;
 l3 = tobecopied.l3;

 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = tobecopied.v1[idx];
  v2[idx] = tobecopied.v2[idx];
  v3[idx] = tobecopied.v3[idx];
 }

 
}



template<class T>
EigenSystem3DV1N4<T>::EigenSystem3DV1N4(T l1v, T l2v, T l3v, const T v1v[3], const T v2v[3], const T v3v[3])  
//constructor which initializes all data
:EigenSystemV1N4<T>(), l1(l1v), l2(l2v), l3(l3v)
{
 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = v1v[idx];
  v2[idx] = v2v[idx];
  v3[idx] = v3v[idx];
 }


}

template<class T>
void EigenSystem3DV1N4<T>::initEigenSystem3D(T l1v, T l2v, T l3v, const T v1v[3], const T v2v[3], const T v3v[3])  
//initializes the current object with these parameters
{

 l1 = l1v;
 l2 = l2v;
 l3 = l3v;
 
 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = v1v[idx];
  v2[idx] = v2v[idx];
  v3[idx] = v3v[idx];
 }

}

template<class T>
void EigenSystem3DV1N4<T>::initEigenSystem3D(T *data) 
//initializes the eigensystem with an array[12] assumed to be in the following format: [lambda1 lambda2 lambda3 nu1x nu1y nu1z nu2x nu2y nu2z nu3x nu3y nu3z]
{
 l1 = data[0];
 l2 = data[1];
 l3 = data[2];
 
 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = data[idx+3];
  v2[idx] = data[idx+6];
  v3[idx] = data[idx+9];
 }

}

template<class T>
void EigenSystem3DV1N4<T>::initEigenSystem3D(const EigenSystem3DV1N4<T> &temp) 
//initializes the current object to be equal to temp
{
 this->l1 = temp.l1;
 this->l2 = temp.l2;
 this->l3 = temp.l3;

 for(int idx = 0; idx < 3; idx++)
 {
  this->v1[idx] = temp.v1[idx];
  this->v2[idx] = temp.v2[idx];
  this->v3[idx] = temp.v3[idx];
 }

}
 

template<class T>
void EigenSystem3DV1N4<T>::initAverageSystem3D(const EigenSystem3DV1N4<T> &elem1, const EigenSystem3DV1N4<T> &elem2) 
//initializes the eigensystem as the average of elem1 and elem2
{
 //calculates the avg eigenvectors as the normed average of the eigenvectors of elem1 and elem2
 hageman::initNormedAverageArray<T,3>(this->v1,elem1.v1,elem2.v1);
 hageman::initNormedAverageArray<T,3>(this->v2,elem1.v2,elem2.v2);
 hageman::initNormedAverageArray<T,3>(this->v3,elem1.v3,elem2.v3);
 
 //calculates the avg eigenvalues as the average of the component of the elem1/elem2's eigenvalue along the object's eigenvector
 T elem1l1 = elem1.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem1.v1));
 elem1l1 += elem1.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem1.v2));
 elem1l1 += elem1.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem1.v3));

 T elem2l1 = elem2.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem2.v1));
 elem2l1 += elem2.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem2.v2));
 elem2l1 += elem2.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v1,elem2.v3));

 this->l1 = .5*(elem1l1 + elem2l1);

 T elem1l2 = elem1.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem1.v1));
 elem1l2 += elem1.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem1.v2));
 elem1l2 += elem1.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem1.v3));

 T elem2l2 = elem2.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem2.v1));
 elem2l2 += elem2.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem2.v2));
 elem2l2 += elem2.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v2,elem2.v3));

 this->l2 = .5*(elem1l2 + elem2l2);

 T elem1l3 = elem1.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem1.v1));
 elem1l3 += elem1.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem1.v2));
 elem1l3 += elem1.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem1.v3));

 T elem2l3 = elem2.l1*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem2.v1));
 elem2l3 += elem2.l2*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem2.v2));
 elem2l3 += elem2.l3*(hageman::generalizedAbsDotProduct<T,3>(this->v3,elem2.v3));

 this->l3 = .5*(elem1l3 + elem2l3);

 //need to possibly reorder the eigensystem based on the new eigenvalues
 EigenSystem3DV1N4<T> temp;

 T evals[3];
 evals[0] = this->l1;
 evals[1] = this->l2;
 evals[2] = this->l3;

 std::pair<T,int> maxevals(hageman::getMaxOfArray<T,3>(evals));
 std::pair<T,int> minevals(hageman::getMinOfArray<T,3>(evals));
 
 temp.l1 = maxevals.first;
 temp.l3 = minevals.first;
 const int EIDX = 3-maxevals.second-minevals.second;
 temp.l2 = evals[EIDX];

 std::vector<T*> evecs;
 evecs.push_back(this->v1);
 evecs.push_back(this->v2);
 evecs.push_back(this->v3);

 for(int idx = 0; idx < 3; idx++)
 {
  temp.v1[idx] = (evecs.at(maxevals.second))[idx];
  temp.v3[idx] = (evecs.at(minevals.second))[idx];
  temp.v2[idx] = (evecs.at(EIDX))[idx];
 }

#if VALIDATE
 std::cout<<"From function void EigenSystem3DV1N4<T>::initAverageSystem3D(const EigenSystem3DV1N4<T> &elem1, const EigenSystem3DV1N4<T> &elem2)"<<std::endl;
 std::cout<<"Old Esys: "<<*this<<std::endl;
 std::cout<<"Reordered Esys: "<<temp<<std::endl;
#endif

 //copy the temp esys into the current object
 this->initEigenSystem3D(temp);

}



//MATH FUNCTIONS


//DTI FUNCTIONS
template<class T>
T EigenSystem3DV1N4<T>::selfRegularTensorProduct() const
//computes the whole tensor product of a voxel (i,j,k) with itself.  The formula is (lambda1)^2 + (lambda2)^2 + (lambda3)^2 (Basser et al. 1996) 
{
 double A = powf(static_cast<double>(l1),2.0f);
 double B = powf(static_cast<double>(l2),2.0f);
 double C = powf(static_cast<double>(l3),2.0f);
 
 return (static_cast<T>(A + B + C)); 

}

template<class T>
T EigenSystem3DV1N4<T>::selfDeviatoricTensorProduct() const
//computes the deviatoric tensor product of a voxel (i,j,k) with itself.  The formula is .33*((lambda1 - lambda2)^2 + (lambda2 - lambda3)^2 +
//(lambda3 - lambda1)^2)
{

 double A = powf(static_cast<double>(l1) - static_cast<double>(l2),2.0f);
 double B = powf(static_cast<double>(l2) - static_cast<double>(l3),2.0f);
 double C = powf(static_cast<double>(l3) - static_cast<double>(l1),2.0f);
 
 return (static_cast<T>(.33f*(A + B + C))); 

}

template<class T>
T EigenSystem3DV1N4<T>::regularTensorProduct(const EigenSystem3DV1N4<T> &op) const
//computes the regular tensor product between two different tensors.  The formula is from Basser et al., 1996 and is written in 
//Leviathan(v2-2.doc) (it is long)
{
 double lambda1 = static_cast<double>(op.l1);
 double lambda1r = static_cast<double>(l1);
 double lambda2 = static_cast<double>(op.l2);
 double lambda2r = static_cast<double>(l2);
 double lambda3 = static_cast<double>(op.l3);
 double lambda3r = static_cast<double>(l3);

 double nu1[3];
 double nu1r[3];
 double nu2[3];
 double nu2r[3];
 double nu3[3];
 double nu3r[3];

 for(int idx = 0; idx < 3; idx++)
 {
  nu1[idx] = static_cast<double>(op.v1[idx]);
  nu1r[idx] = static_cast<double>(v1[idx]);
  nu2[idx] = static_cast<double>(op.v2[idx]);
  nu2r[idx] = static_cast<double>(v2[idx]);
  nu3[idx] = static_cast<double>(op.v3[idx]);
  nu3r[idx] = static_cast<double>(v3[idx]);
 }


 double R11 = (lambda1*lambda1r)*powf(generalizedDotProduct<double,3>(nu1,nu1r),2);
 double R12 = (lambda1*lambda2r)*powf(generalizedDotProduct<double,3>(nu1,nu2r),2);
 double R13 = (lambda1*lambda3r)*powf(generalizedDotProduct<double,3>(nu1,nu3r),2);
 double R21 = (lambda2*lambda1r)*powf(generalizedDotProduct<double,3>(nu2,nu1r),2);
 double R22 = (lambda2*lambda2r)*powf(generalizedDotProduct<double,3>(nu2,nu2r),2);
 double R23 = (lambda2*lambda3r)*powf(generalizedDotProduct<double,3>(nu2,nu3r),2);
 double R31 = (lambda3*lambda1r)*powf(generalizedDotProduct<double,3>(nu3,nu1r),2);
 double R32 = (lambda3*lambda2r)*powf(generalizedDotProduct<double,3>(nu3,nu2r),2);
 double R33 = (lambda3*lambda3r)*powf(generalizedDotProduct<double,3>(nu3,nu3r),2);
 
    double product = R11 + R12 + R13 + R21 + R22 + R23 + R31 + R32 + R33;

 
 return (static_cast<T>(product));

}

template<class T>
T EigenSystem3DV1N4<T>::deviatoricTensorProduct(const EigenSystem3DV1N4<T> &op) const
//computes the deviatoric tensor product between two different tensors.  The formula is (prime is the variable voxel) 
//dD':dD = D':D - 3<D'><D> from Basser et al., 1996.
{
 T A = this->regularTensorProduct(op);
 T B = static_cast<T>(3.0f*static_cast<double>(op.meanDiffusivity()));
 T C = static_cast<T>(static_cast<double>(this->meanDiffusivity()));
 
 T result = A - (B*C);

 //std::cout<<"Deviatoric Tensor Product: "<<result<<std::endl;


 return result;
}

template<class T>
T EigenSystem3DV1N4<T>::latticeIndex(const EigenSystem3DV1N4<T> &op) const
//computes the lattice index between the current object and op
{
 double A = static_cast<double>(this->selfRegularTensorProduct());
 double B = static_cast<double>(op.selfRegularTensorProduct());

 double product = 0.0f;

 if((A == 0.0f) || (B == 0.0f))
  return T();
 else
 {
  product = this->deviatoricTensorProduct(op);
  product *= powf(A,-0.5f);
  product *= powf(B,-0.5f);
 }

 return (static_cast<T>(product));

}

template<class T>
T EigenSystem3DV1N4<T>::fractionalAnisotropy() const
//computes the FA for the current eigensystem
{

 T result = T();
 T denominator = this->selfRegularTensorProduct();
 denominator = static_cast<T>(sqrtf(static_cast<double>(denominator)));
 T numerator = this->selfDeviatoricTensorProduct();
 numerator = static_cast<T>((sqrtf(3.0f/2.0f))*sqrtf(static_cast<double>(numerator)));
    
 if(denominator != T())
  result = numerator/denominator;

 if(result < T())
  result = 0;
 
 return result;


}
 

template<class T>
void EigenSystem3DV1N4<T>::correctForDTI()
//corrects any invalid or undesired data elements based on assumptions of DTI
{

 //eigenvalues in DTI cannot be negative since they represent physical values of diffusion
 if(l1 < T())
  l1 = T();
 if(l2 < T())
  l2 = T();
 if(l3 < T())
  l3 = T();


}




//PRINT FUNCTIONS
template<class T>
void EigenSystem3DV1N4<T>::printCL() const
//prints the eigensystem elements on the CL
{
 std::cout<<"Lambda 1: "<<l1<<std::endl;
 std::cout<<"Lambda 2: "<<l2<<std::endl;
 std::cout<<"Lambda 3: "<<l3<<std::endl;

 std::cout<<"Mu 1: ("<<v1[0]<<","<<v1[1]<<","<<v1[2]<<")"<<std::endl;
 std::cout<<"Mu 2: ("<<v2[0]<<","<<v2[1]<<","<<v2[2]<<")"<<std::endl;
 std::cout<<"Mu 3: ("<<v3[0]<<","<<v3[1]<<","<<v3[2]<<")"<<std::endl;
 

}


//OVERLOADED OPERATORS
template<class T>
void EigenSystem3DV1N4<T>::operator=(const EigenSystem3DV1N4<T> &tobecopied)
//overloaded assignment operator that copies the eigensystem element-wise
{
 l1 = tobecopied.l1;
 l2 = tobecopied.l2;
 l3 = tobecopied.l3;

 for(int idx = 0; idx < 3; idx++)
 {
  v1[idx] = (tobecopied.v1)[idx];
  v2[idx] = (tobecopied.v2)[idx];
  v3[idx] = (tobecopied.v3)[idx];
 }


}



template<class T>
std::ostream& operator<< (std::ostream& os, hageman::EigenSystem3DV1N4<T> output) 
//overloaded << operator that outputs the eigensystem elements on the CL
{
 os <<"Lambda 1: "<<output.l1<<std::endl;
 os <<"Lambda 2: "<<output.l2<<std::endl;
 os <<"Lambda 3: "<<output.l3<<std::endl;

 os <<"Mu 1: ("<<output.v1[0]<<","<<output.v1[1]<<","<<output.v1[2]<<")"<<std::endl;
 os <<"Mu 2: ("<<output.v2[0]<<","<<output.v2[1]<<","<<output.v2[2]<<")"<<std::endl;
 os <<"Mu 3: ("<<output.v3[0]<<","<<output.v3[1]<<","<<output.v3[2]<<")"<<std::endl;

 return os;
}



template<class T>
class Volume3DV2N4 : public VolumeV2N4<T>{
 
public:
 //using statements allow overloaded non-virtual base class functions to be visible (otherwise, they are hidden)
 using VolumeV2N4<T>::convertToIndex;  //for an n-dimensional volume, converts the coords vector 
 //into index coordinates, assuming standard matrix coordinates (row,column)
  using VolumeV2N4<T>::convertToCoords;  //converts an index into a coordinate vector based on the dimensions,
 //assuming standard matrix coordinates (row,column)
 
 //CONSTRUCTORS
 Volume3DV2N4(); //default empty constructor 
 Volume3DV2N4(int RDIMv, int CDIMv, int ZDIMv, T element = T());  //initializes array and sets the data members to element (default is T())
 Volume3DV2N4(const Volume3DV2N4<T> &paradigm);  //copy constructor
 
 //ASSIGNMENT FUNCTIONS
 void copyVolume3D(const Volume3DV2N4<T> &paradigm);  //copies paradigm into the current object
 void initVolume3D(int RDIMv, int CDIMv, int ZDIMv, T element = T());  //allows you to initialize a Volume3D object that has been created by the default constructor
 
 template<class S>
 void makeCompatible(const Volume3DV2N4<S> &paradigm);  //reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all 
 //values to element.  If dimensions are already the same, does nothing.

 //DESTRUCTORS
 ~Volume3DV2N4();  //destructor
 
 //GET FUNCTIONS
 const int getRDIM() const {return RDIM;}  //returns the dimensions
 const int getCDIM() const {return CDIM;}
 const int getZDIM() const {return ZDIM;}
 const int getSIZE() const {return SIZE;}

 inline const T& getElement(int ridx, int cidx, int zidx) const;  //gets the const element reference at the position (ridx,cidx,zidx)
 inline const T& getElement(int index) const;  //gets the const element reference at the position (index)
 inline const T& getElementInStandardCoordSystem(int x, int y, int z) const;  //gets the const element reference at the position (x,y,z) at matrix position (i=y,j=x,k=z)
 T& getNonConstElementReference(int i, int j, int k);  //gets the non-const element reference at the position (ridx,cidx,zidx)
 T& getNonConstElementReference(int index);  //gets the non-const element reference at the position (index)
 //const T& getConstElementReference(int i, int j, int k) const;
 //const T& getConstElementReference(int index) const;
 const T* getConstDataReference() const {return this->data;}  //gets a const reference to the entire data array
 T* getNonConstDataReference() {return this->data;}  //gets a non-const reference to the entire data array 
 //void getDataAsArray(T myarray[]);  //gets the vector data as an array (assumes that myarray has sufficient space)

 //SET FUNCTIONS
 inline void setElement(int index, T element);  //sets the element at array index position "index" to element
 inline void setElementInStandardCoordSystem(int x, int y, int z, T element);  //sets the element at the position (x,y,z) which is the position (i=y,j=x,k=z) in the matrix coordinate system
 inline void setElement(int ridx, int cidx, int zidx, T element);  //sets the element at (ridx,cidx,zidx) to element
 void setRDIM(int rD) {RDIM = rD;}  //sets the dimensions of the volume
 void setCDIM(int cD) {CDIM = cD;}
 void setZDIM(int zD) {ZDIM = zD;}

 //CHECK FUNCTIONS
 inline bool inBounds(int ridx, int cidx, int zidx) const;  //checks to make sure the coords (ridx,cidx,zidx) are within the bounds of the volume
 inline bool areDimensionsEqual(const Volume3DV2N4<T> &op) const;  //check to see whether the dimensions of op are equal to the current object.
 inline bool isEqual(const Volume3DV2N4<T> &compareObj) const;  //checks to see whether current object is equal to the argument
 bool isEmpty() const {return (this->getSIZE() == 0);} //checks whether the volume is empty
 
 //READ FUNCTIONS
 void readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, hageman::AllocateMem amem = hageman::NOALLOCATE); //a function of for reading in a 3D volume from a txt file.  
 //Function assumes ifname has one value per line and assumes the following read order:  the reading starts with zidx = 0, and reads all the
 //columns for the first row, then goes to the second row and reads all the column for the second row and so on...  When it is finished with all rows and
 //columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...

 //WRITE FUNCTIONS
 void writeOuttoTxtFile(const FileNameV1N1<> *ofname);  //writes out sinple unformated data to a text file
 void writeOutDetailedtoTxtFile(const FileNameV1N1<> *ofname);  //writes out data together with coordinates to a text file
 
 //PRINT FUNCTIONS
 virtual void printCL() const;  //prints all elements on the CL
 void printElementCL(int i, int j, int k) const;  //prints the elements on the CL
 void printVolInfo() const;  //prints out the dimensions on the CL
 
 //MUTATING FUNCTIONS
 void zeroPadVolume(int dx, int dy, int dz, T element = T());  //zeropads the volume by adding element to the ends of the volume.  (dx,dy,dz) are in voxel coordinates.  Moves the voxels
 //so that the current dimensions of the volume are not changed
 void zeroPadXDIM(int dx, T element = T());  //zeropads the individual volume dimensions
 void zeroPadYDIM(int dy, T element = T());
 void zeroPadZDIM(int dz, T element = T());
 void flipRDIM();  //reverses the data in the specified dimensions
 void flipCDIM();
 void flipZDIM();

 
 //UTILITY FUNCTIONS USED BY OTHER MEMBER FUNCTIONS
 inline int convertToIndex(int ridx, int cidx, int zidx) const;  //since the data is represented by a linear array, this converts the coords vector 
 //into index coordinates - automatically reverses the image coords into matrix coords and then calls the Volume class function
 inline std::vector<int> convertToCoords(int index);  //converts an index into a coordinate vector based on the dimensions - automatically 
 //reverses the image coords into matrix coords and then calls the Volume class function
 
 //OPERATOR OVERLOADS
 inline T& operator[](int index);  //overloaded [] operator.  Takes in an index number and returns the element.  WARNING: THIS RETURNS A NONCONST REFERENCE TO THE ELEMENT
 void operator=(const Volume3DV2N4<T> &paradigm);  //overloaded assignment operator
 bool operator==(const Volume3DV2N4<T> &compareObj);  //overloaded equality operators for the volume
 bool operator!=(const Volume3DV2N4<T> &compareObj);

 ReadFromBinaryFileV1N1<> *ibfile;  //uninitialized input stream for reading from a binary file
 ReadFromFileV1N1<> *ifile;  //uninitialized input stream for reading from a text file
 WriteToFileV1N1<> *ofile;  //uninitialized output stream for writing to a text file
 WriteToBinaryFileV1N1<> *obfile;  //uninitialized output stream for writing to a binary file

protected:
 int RDIM;
 int CDIM;
 int ZDIM;
 int SIZE;

 


private:
 //inline int convertToIndex(const std::vector<int> &coords, const std::vector<int> &dims) const {return (VolumeV2N4<T>::convertToIndex(coords,dims));}  //for an 
 //n-dimensional volume, converts the coords vector into index coordinates - a necessary reimplementation of the Volume class function
 //inline std::vector<int> convertToCoords(int index, const std::vector<int> &dims) {return (VolumeV2N4<T>::convertToCoords(index,dims));}  //converts an index into a 
 //coordinate vector based on the dimensions - - a necessary reimplementation of the Volume class function


};

//CONSTRUCTORS

template<class T>
Volume3DV2N4<T>::Volume3DV2N4()
//default constructor
:VolumeV2N4<T>(), RDIM(0), CDIM(0), ZDIM(0), SIZE(0)
{
 this->data = NULL;
 ibfile = NULL;
 ifile = NULL;
 ofile = NULL;
 obfile = NULL;
}

template<class T>
Volume3DV2N4<T>::Volume3DV2N4(int RDIMv, int CDIMv, int ZDIMv, T element)
//initializes array and sets the data members to element (default is T())
:VolumeV2N4<T>(), RDIM(RDIMv), CDIM(CDIMv), ZDIM(ZDIMv), SIZE(RDIMv*CDIMv*ZDIMv)
{

 ibfile = NULL;
 ifile = NULL;
 ofile = NULL;
 obfile = NULL;

 this->data = NULL;
 this->data = new T[SIZE];

 //assigns each element of data to element (assumes that T has a valid = operator)
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = element;

}

template<class T>
Volume3DV2N4<T>::Volume3DV2N4(const Volume3DV2N4<T> &paradigm)
//copy constructor
:VolumeV2N4<T>(), RDIM(paradigm.getRDIM()), CDIM(paradigm.getCDIM()), ZDIM(paradigm.getZDIM()), SIZE(paradigm.getRDIM() * paradigm.getCDIM() * paradigm.getZDIM())
{

 ibfile = NULL;
 ifile = NULL;
 ofile = NULL;
 obfile = NULL;

 this->data = NULL;
 this->data = new T[SIZE];

 //copies the volume element-wise (assumes that T has a valid = operator)
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = paradigm.getElement(idx);

}

//ASSIGNMENT FUNCTIONS

template<class T>
void Volume3DV2N4<T>::initVolume3D(int RDIMv, int CDIMv, int ZDIMv, T element)  
//allows you to initialize a Volume3D object
{

 ibfile = NULL;
 ifile = NULL;
 ofile = NULL;
 obfile = NULL;

 //assigns the dimensions
 RDIM = RDIMv;
 CDIM = CDIMv;
 ZDIM = ZDIMv;
 SIZE = RDIM*CDIM*ZDIM;

 //deletes all current data
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }

 this->data = new T[SIZE];

 //assigns each element of data to element (assumes that T has a valid = operator)
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = element;


}

template<class T>
void Volume3DV2N4<T>::copyVolume3D(const Volume3DV2N4<T> &paradigm)  
//copies paradigm into the current object
{

 ibfile = NULL;
 ifile = NULL;
 ofile = NULL;
 obfile = NULL;

 //assigns the dimensions
 RDIM = paradigm.getRDIM();
 CDIM = paradigm.getCDIM();
 ZDIM = paradigm.getZDIM();
 SIZE = RDIM*CDIM*ZDIM;
 
 //deletes all current data
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }

 this->data = new T[SIZE];

 //copies the volume element-wise (assumes that T has a valid = operator)
 for(int idx = 0; idx < SIZE; idx++)
  this->data[idx] = paradigm.getElement(idx);

}

template<class T>
template<class S>
void Volume3DV2N4<T>::makeCompatible(const Volume3DV2N4<S> &paradigm)
//reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all values to element.  If dimensions are already the same, does nothing.
{
 T element = T();
 //if(!(this->areDimensionsEqual(paradigm)))   //if dimensions are already the same, do nothing.
 //{
  this->initVolume3D(paradigm.getRDIM(), paradigm.getCDIM(), paradigm.getZDIM(), element);
 //}

}




//DESTRUCTORS
template<class T>
Volume3DV2N4<T>::~Volume3DV2N4()  
//destructor
{
 if(this->data != NULL)
 {
  delete[] this->data;
  this->data = NULL;
 }

 if(this->ibfile != NULL)
 {
  delete ibfile;
  this->ibfile = NULL;
 }

 if(this->ifile != NULL)
 {
  delete ifile;
  this->ifile = NULL;
 }

 if(this->obfile != NULL)
 {
  delete obfile;
  this->obfile = NULL;
 }

 if(this->ofile != NULL)
 {
  delete ofile;
  this->ofile = NULL;
 }



 
 
}


//CHECK FUNCTIONS
template<class T>
inline bool Volume3DV2N4<T>::inBounds(int ridx, int cidx, int zidx) const
//checks to make sure the coords (x,y,z) are within the bounds of the volume
{
 if((ridx < 0) || (ridx >= getRDIM()))
  return false;
 else if((cidx < 0) || (cidx >= getCDIM()))
  return false;
 else if((zidx < 0) || (zidx >= getZDIM()))
  return false;

 return true;

}

template<class T>
inline bool Volume3DV2N4<T>::areDimensionsEqual(const Volume3DV2N4<T> &op) const
//check to see whether the dimensions of op are equal to the current object.
{
 if((RDIM == op.getRDIM()) && (CDIM == op.getCDIM()) && (ZDIM == op.getZDIM()) && (SIZE == op.getSIZE()))
  return true;

 return false;

}

template<class T>
bool Volume3DV2N4<T>::isEqual(const Volume3DV2N4<T> &compareObj) const
{
 if(!(areDimensionsEqual(compareObj)))
  return false;

 for(int idx = 0; idx < SIZE; idx++)
 {
  if(this->data[idx] != ((*this)[idx]))
   return false;
 }

 return true;

}

//GET FUNCTIONS

template<class T>
inline const T& Volume3DV2N4<T>::getElement(int index) const 
//gets the element at the position (index)
{
#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getElement(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 
 }
#endif
 
 return this->data[index];
}

template<class T>
inline const T& Volume3DV2N4<T>::getElement(int ridx, int cidx, int zidx) const 
//gets the element at the position (ridx,cidx,zidx)
{

 int index = this->convertToIndex(ridx,cidx,zidx);

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getElement(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 
 }
#endif


 return (this->data[index]);


}

template<class T>
inline const T& Volume3DV2N4<T>::getElementInStandardCoordSystem(int x, int y, int z) const  
//gets the element at the position (x,y,z) at matrix position (i=y,j=x,k=z)
{
 int index = this->convertToIndex(y,x,z);

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getElement(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 
 }
#endif


 return this->data[index];


}

template<class T>
inline T& Volume3DV2N4<T>::getNonConstElementReference(int i, int j, int k)
{
 int index = this->convertToIndex(i,j,k);

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getElement(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 
 }
#endif


 return this->data[index];
}


template<class T>
inline T& Volume3DV2N4<T>::getNonConstElementReference(int index)
{

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getReference(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get reference.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 
 return this->data[index];
}



//SET FUNCTIONS
template<class T>
inline void Volume3DV2N4<T>::setElement(int index, T element)
//sets the element at array index position index to element
{

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::setElement(int index, T element)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot set element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 this->data[index] = element;

}

template<class T>
inline void Volume3DV2N4<T>::setElement(int ridx, int cidx, int zidx, T element)
//sets the element at (ridx,cidx,zidx) to element
{

#if ERRORCHECK
 if(!(inBounds(ridx,cidx,zidx)))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::setElement(int ridx, int cidx, int zidx, T element)"<<std::endl;
  std::cout<<"Position = ("<<ridx<<","<<cidx<<","<<zidx<<")"<<std::endl;
  std::cout<<"Cannot set element.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 int index = this->convertToIndex(ridx,cidx,zidx);

#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::getReference(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot get reference.  Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 this->data[index] = element;
 
}

template<class T>
inline void Volume3DV2N4<T>::setElementInStandardCoordSystem(int x, int y, int z, T element)  
//sets the element at the position (x,y,z) which is the position (i=y,j=x,k=z) in the matrix coordinate system
{
 this->setElement(y,x,z,element);
 
}
 


//READ FUNCTIONS

template<class T>
void Volume3DV2N4<T>::readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, hageman::AllocateMem amem)
//a function of for reading in a 3D volume from a txt file.  This function DOES NOT create any space for vol unless allocatemem is set to true.  
//Otherwise, it assumes that vol contains the space 
//necessary to read in ifname.  Make sure you initialize vol before calling this function.  I decided to do it this way to spare any further costly 
//initialization.  Function assumes ifname has one value per line and assumes the following read order:  the reading starts with zidx = 0, and reads all the
//rows for the first column, then goes to the second column and reads all the rows for the second column and so on...  When it is finished with all rows and
//columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...
{
 
 if(amem != hageman::NOALLOCATE)
 {
  this->initVolume3D(RDIMv,CDIMv,ZDIMv);
 }

#if ERRORCHECK
 if(SIZE != RDIMv*CDIMv*ZDIMv)
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::readInFromTxtFile(std::string ifname, int RDIMv, int CDIMv, int ZDIMv, bool allocatemem)"<<std::endl;
  std::cout<<"SIZE = "<<std::endl;
  std::cout<<"Proper memory is not allocated for reading.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 
#if VERBOSE
 std::cout<<"Reading in Txt File ImageVolume from file = "<<ifname->getOSCompatibleFileName()<<std::endl;
 std::cout<<"Total points to read = "<<SIZE<<std::endl;
#endif

 int ptsread = 0;

 (this->ifile) = new ReadFromFileV1N1<>(ifname);
 (this->ifile)->openForRead();

 T datapt = T();
 std::string dataptstr;
 
 for(int k = 0; k < ZDIMv; k++)
  for(int j = 0; j < CDIMv; j++)
   for(int i = 0; i < RDIMv; i++)
   {
#if VERBOSE
    if((ptsread % 10000) == 0)
     std::cout<<"Pts Read = "<<ptsread<<" of "<<SIZE<<std::endl;
#endif
    //reading in a datapt
    dataptstr = (this->ifile)->readProcessedCurrentLineFromStream();
    datapt = static_cast<T>(atof(dataptstr.c_str()));
      
    this->setElement(i,j,k,datapt);
    ++ptsread;
      
   }

 (this->ifile)->close();
 delete (this->ifile);
 (this->ifile) = NULL;

#if VERBOSE
 std::cout<<ptsread<<" Points Read!"<<std::endl;
#endif

}

//WRITE FUNCTIONS
template<class T>
void Volume3DV2N4<T>::writeOuttoTxtFile(const FileNameV1N1<> *ofname)
//a function of for writing out a 3D volume to a txt file.  Function writes out the values of the Volume3D, one per line.  Function assumes the 
//following write order:  the writing starts with zidx = 0, and writes all the
//rows for the first column, then goes to the second column and writes all the rows for the second column and so on...  When it is finished with all rows and
//columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...
{
#if VERBOSE
 std::cout<<"Writing Out Volume3D to Txt File = "<<ofname->getOSCompatibleFileName()<<std::endl;
#endif

 (this->ofile) = new WriteToFileV1N1<>(ofname);
 (this->ofile)->openForWrite();
 
 int index = 0;
 for(int k = 0; k < ZDIM; k++)
  for(int j = 0; j < CDIM; j++)
   for(int i = 0; i < RDIM; i++)
   {
    index = this->convertToIndex(i,j,k);
#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in void Volume3DV2N4<T>::writeOuttoTxtFile(const FileNameV1N1<> *ofname)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Index is not valid.  Exiting..."<<std::endl;
  exit(1);
 }    
#endif
    (*ofile) << (*this)[index] << "\n";
   }

 ofile->close();
 delete (this->ofile);
 this->ofile = NULL;


}

template<class T>
void Volume3DV2N4<T>::writeOutDetailedtoTxtFile(const FileNameV1N1<> *ofname)
//a function of for writing out a 3D volume to a txt file along with the coordinates and the index.  Function writes out the values of the Volume3D, 
//one per line.  Function assumes the following write order:  the writing starts with zidx = 0, and writes all the
//rows for the first column, then goes to the second column and writes all the rows for the second column and so on...  When it is finished with all rows and
//columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...
{

#if VERBOSE
 std::cout<<"Writing Out Detailed Volume3D to Txt File = "<<ofname->getOSCompatibleFileName()<<std::endl;
#endif

 (this->ofile) = new WriteToFileV1N1<>(ofname);
 (this->ofile)->openForWrite();
 

 int index;
 for(int k = 0; k < ZDIM; k++)
 {
  (ofile->getNonConstWriteFileStream()) << "Z = "<< k << "\n";
  for(int j = 0; j < CDIM; j++)
   for(int i = 0; i < RDIM; i++)
   {

    index = this->converttoIndex(i,j,k);
#if ERRORCHECK
    if((index < 0) || (index >= SIZE))
    {
     std::cout<<"ERROR in void Volume3DV2N4<T>::writeOuttoTxtFile(const FileNameV1N1<> *ofname)"<<std::endl;
     std::cout<<"Index = "<<index<<std::endl;
     std::cout<<"Index is not valid.  Exiting..."<<std::endl;
     exit(1);
    }    
#endif


    (ofile->getNonConstWriteFileStream()) << "Coords ("<<i<<","<<j<<","<<k<<"); Index "<<index<<": "<<(*this)[index] << "\n";
   }
 }

 ofile->close();
 delete (this->ofile);
 this->ofile = NULL;


}


//PRINT FUNCTIONS

template<class T>
void Volume3DV2N4<T>::printCL() const
//prints all the elements on the CL
{

 for(int idx = 0; idx < (this->getSIZE()); idx++)
  std::cout<<this->data[idx]<<std::endl;

}



template<class T>
void Volume3DV2N4<T>::printElementCL(int i, int j, int k) const
//prints the elements on the CL
{
 int index = this->convertToIndex(i,j,k);
 std::cout<<"Coords ("<<i<<","<<j<<","<<k<<"); Index "<<index<<": "<<this->data[index]<<std::endl;
 
}

template<class T>
void Volume3DV2N4<T>::printVolInfo() const
//prints out the dimensions on the CL
{
 std::cout<<"Dimensions: "<<this->getRDIM()<<" "<<this->getCDIM()<<" "<<this->getZDIM()<<std::endl;

}
 

//MUTATING FUNCTIONS
template<class T>
void Volume3DV2N4<T>::zeroPadVolume(int dx, int dy, int dz, T element)  
//zeropads the volume by adding element to the ends of the volume.  (dx,dy,dz) are in voxel coordinates.
{

 this->zeroPadXDIM(dx,element);
 this->zeroPadYDIM(dy,element);
 this->zeroPadZDIM(dz,element);

}

template<class T>
void Volume3DV2N4<T>::zeroPadXDIM(int dx, T element)  
//zeropads the volume by adding element to the ends of the volume.  (dx,dy,dz) are in voxel coordinates.
{

#if VERBOSE 
 std::cout<<"Moving XDIM by "<<dx<<" voxels..."<<std::endl;
#endif

 if(dx > 0)
 {
  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = (this->getRDIM()-1); i >= dx; i--)
    {
     this->setElement(i,j,k,(this->getElement(i-dx,j,k)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < dx; i++)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }
 else if(dx < 0)
 {
  dx = abs(dx);

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < (this->getRDIM())-dx; i++)
    {
     this->setElement(i,j,k,(this->getElement(i+dx,j,k)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = (this->getRDIM())-1; i >= (this->getRDIM())-1-dx; i--)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }

     
  
}


template<class T>
void Volume3DV2N4<T>::zeroPadYDIM(int dy, T element)  
//zeropads the volume by adding element to the ends of the volume.  (dx,dy,dz) are in voxel coordinates.
{
#if VERBOSE 
 std::cout<<"Moving YDIM by "<<dy<<" voxels..."<<std::endl;
#endif

 if(dy > 0)
 {
  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = (this->getCDIM()-1); j >= dy; j--)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,(this->getElement(i,j-dy,k)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < dy; j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }
 else if(dy < 0)
 {
  dy = abs(dy);

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = 0; j < (this->getCDIM())-dy; j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,(this->getElement(i,j+dy,k)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = 0; k < (this->getZDIM()); k++)
   for(int j = (this->getCDIM())-1; j >= (this->getCDIM())-1-dy; j--)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }
     
  
}

template<class T>
void Volume3DV2N4<T>::zeroPadZDIM(int dz, T element)
//zeropads the volume by adding element to the z dimension
{
#if VERBOSE 
 std::cout<<"Moving ZDIM by "<<dz<<" voxels..."<<std::endl;
#endif

 if(dz > 0)
 {
  for(int k = (this->getZDIM())-1; k >= dz; k--)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,(this->getElement(i,j,k-dz)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = 0; k < dz; k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }
 else if(dz < 0)
 {
  dz = abs(dz);

  for(int k = 0; k < (this->getZDIM())-dz; k++)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,(this->getElement(i,j,k+dz)));  //sets the elements a distance dx away to the element i-dx
    }

  for(int k = (this->getZDIM())-1; k >= (this->getZDIM())-1-dz; k--)
   for(int j = 0; j < (this->getCDIM()); j++)
    for(int i = 0; i < (this->getRDIM()); i++)
    {
     this->setElement(i,j,k,element);  //sets the elements less than dx away to element
    }

 }
     
  
}

template<class T>
void Volume3DV2N4<T>::flipCDIM()
//reverses the data in the CDIM dimension
{
#if VERBOSE 
 std::cout<<"Flipping CDIM..."<<std::endl;
#endif

 Volume3DV2N4<T> temp(*this);
 T elem;
 
 for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  for(int cidx = 0; cidx < (this->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (this->getRDIM()); ridx++)
   {
    elem = temp.getElement(ridx,(this->getCDIM())-cidx-1,zidx);
    this->setElement(ridx,cidx,zidx,elem);



   }





}

template<class T>
void Volume3DV2N4<T>::flipRDIM()
//reverses the data in the RDIM dimension
{
#if VERBOSE 
 std::cout<<"Flipping RDIM..."<<std::endl;
#endif

 Volume3DV2N4<T> temp(*this);
 T elem;
 
 for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  for(int cidx = 0; cidx < (this->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (this->getRDIM()); ridx++)
   {
    elem = temp.getElement((this->getRDIM())-ridx-1,cidx,zidx);
    this->setElement(ridx,cidx,zidx,elem);



   }





}


template<class T>
void Volume3DV2N4<T>::flipZDIM()
//reverses the data in the ZDIM dimension
{
#if VERBOSE 
 std::cout<<"Flipping ZDIM..."<<std::endl;
#endif

 Volume3DV2N4<T> temp(*this);
 T elem;
 
 for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  for(int cidx = 0; cidx < (this->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (this->getRDIM()); ridx++)
   {
    elem = temp.getElement(ridx,cidx,(this->getZDIM())-zidx-1);
    this->setElement(ridx,cidx,zidx,elem);



   }

}




//CONVERSION FUNCTIONS
template<class T>
std::vector<int> Volume3DV2N4<T>::convertToCoords(int index)
//since the data is represented by a linear array, this converts the index into a coords vector 
{
#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in std::vector<int> Volume3DV2N4<T>::convertToCoords(int index)"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot convert to coordinates.  Index is not valid."<<std::endl;
  exit(1);
 }
#endif
 
 std::vector<int> dims;
 dims.push_back(RDIM);
 dims.push_back(CDIM);
 dims.push_back(ZDIM);

 std::vector<int> coords(this->convertToCoords(index,dims));
 return coords;
 

}

template<class T>
inline int Volume3DV2N4<T>::convertToIndex(int ridx, int cidx, int zidx) const
//converts a coordinate vector into an index based on the dimensions
{

#if ERRORCHECK
 if(!(inBounds(ridx,cidx,zidx)))
 {
  std::cout<<"ERROR in int Volume3DV2N4<T>::convertToIndex(int ridx, int cidx, int zidx) const"<<std::endl;
  std::cout<<"Coords = ("<<ridx<<","<<cidx<<","<<zidx<<")"<<std::endl;
  std::cout<<"Volume3DV2N4 ERROR: Cannot convert to index. Coordinates are not valid."<<std::endl;
  exit(1);
 }
#endif
   
 std::vector<int> dims;
 dims.push_back(RDIM);
 dims.push_back(CDIM);
 dims.push_back(ZDIM);

 std::vector<int> coords;
 coords.push_back(ridx);
 coords.push_back(cidx);
 coords.push_back(zidx);
 
 int index = this->convertToIndex(coords,dims);

 return index;

}




//OPERATOR OVERLOADS
template<class T>
inline T& Volume3DV2N4<T>::operator[](int index)
//overloaded [] operator.  Takes in an index number and returns the element
{
 
#if ERRORCHECK
 if((index < 0) || (index >= SIZE))
 {
  std::cout<<"ERROR in T Volume3DV2N4<T>::operator[](int index) const"<<std::endl;
  std::cout<<"Index = "<<index<<std::endl;
  std::cout<<"Cannot return element.  Index is not valid."<<std::endl;
  exit(1);
 }
#endif
 
 return (this->data[index]);
}

template<class T>
inline void Volume3DV2N4<T>::operator=(const Volume3DV2N4<T> &paradigm)
//overloaded = operator
{
 this->copyVolume3D(paradigm);

}

template<class T>
inline bool Volume3DV2N4<T>::operator==(const Volume3DV2N4<T> &compareObj)
//overloaded == operator
{
 return (this->isEqual(compareObj));
}

template<class T>
inline bool Volume3DV2N4<T>::operator!=(const Volume3DV2N4<T> &compareObj)
//overloaded != operator
{
 return (!(this->isEqual(compareObj)));
}



//}

template<class T>
std::ostream& operator<<(std::ostream& os, const hageman::Volume3DV2N4<T> &output)
//outputs Volume3D values, one per line.  Starting with z = 0, it output all the rows for the first column, then all the rows for the second column, and 
//so on
{
 for(int idx = 0; idx < output.getSIZE(); idx++)
 {
  os << output[idx] << "\n";
 }

 
 return os;



}

enum SwapType{ROW, COLUMN};


template<class T>
class Matrix2DV2N4 : public Volume2DV2N4<T>{
 
public:
 //CONSTRUCTORS
 Matrix2DV2N4();  //default constructor
 Matrix2DV2N4(int RDIMv, int CDIMv, T element = T());  //initializes deque and sets the data members to element (default is T())
 Matrix2DV2N4(const Matrix2DV2N4<T> &copy);  //copy constructor
 Matrix2DV2N4(const Volume2DV2N4<T> &copy);
 
 //PRINT FUNCTIONS
 virtual void printCL() const; //print out on the command line in matrix format one row per line

 //GET FUNCTIONS
 
 //MATHEMATICAL OPERATIONS
 void scalarMultiply(T scalar);  //scalar multiplies the current object.  The current object is altered.
 void scalarMultiplyRow(T scalar, int ridx); //scalar multiples the current object row = ridx by T.  The current object is altered.
 inline T getMaxElement(); //returns the greatest element in the matrix
 inline T getMinElement(); //returns the least element in the matrix
 inline void scale(T range);  //scales the matrix to run between 0 and range
 Matrix2DV2N4<T> inverse2x2();  //implementation of a simple algorithm for determining the inverse of a 2x2 matrix
 Matrix2DV2N4<T> gaussJordanInverse() const;  //returns the inverse of a square matrix
 void swap(std::pair<int, SwapType>, std::pair<int, SwapType>);  //swaps a row and a column.  Each std::pair argument is a reference to the 
 //first entry in the row or column to be swapped. 
 Matrix2DV2N4<T> rotate2xNMatrix(double angle);  //function which rotates a the matrix by an arbitrary angle (assumed to be in radians).  
 //Returns the rotated matrix and DOES NOT ALTER THE ORIGINAL DATA.
 Matrix2DV2N4<T> calculatePseudoInverse();  //calculates the Moore-Penrose Pseudoinverse and returns it as a Matrix2D
 //EigenSystem3DV1N2<T> calculateJacobiEigenSystem3D();  //calculates the Jacobi Eigensystem of a 3x3 matrix
 void calculateJacobiEigenSystem3D(T* result);  //calculates the Jacobi Eigensystem of a 3x3 matrix
 void solveForGeneratingMatrix(const EigenSystem3DV1N4<T> &esys);  //creates the 3x3 matrix which would generate the EigenSystem3D esys
 void solveForSymmetricGeneratingMatrix(const EigenSystem3DV1N4<T> &esys); //creates a 3x3 symmetric matrix which would generate the EigenSystem3D
 void singularValueDecomposition(Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V);  //given an mxn matrix, this computes its SVD (A = UWV').  The current object is replaced by U on output. w is the 
 //output of singular values (does not require allocation). V is output as an nxn matrix, not its transpose.
 void SVDBackSolve(Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V, Matrix2DV2N4<T> &b, Matrix2DV2N4<T> &x);  //this function computes the solution of Ax=b by backsolving for x using the SVD matrices
 //computed singularValueDecomposition().  U is assumed to be the current object.  Memory does not need to be allocated for any object.  NOTE: this function assumes that the w's have 
 //already been zeroed.
 inline void editSingularValues(T tol); //sets the singular values of the w matrix to zero based on a threshold.  Assumes that the current object is a row vector obtained from the SVD
 //decomposition
 void changeBasis(const Matrix2DV2N4<T> &basisMatrix);  //reexpresses the current object in the new basis.  basisMatrix is a matrix whose columns are the
 //basis vectors.  The current object data is destroyed.
 
 //OVERLOADED OPERATORS
 Matrix2DV2N4<T> operator+(const Matrix2DV2N4<T> &op);  //overloaded addition operator, adds op to the current object and returns a new Matrix3D object.  The
 //current object is not altered
 void operator+=(const Matrix2DV2N4<T> &op);  //overloaded addition equality operator, adds MatrixV1N1 to the current object.  The current object is altered; 
 //nothing is returned.
 Matrix2DV2N4<T> operator-(const Matrix2DV2N4<T> &op) const;  //overloaded subtraction operator, adds op to the current object and returns a new Matrix3D object.  The
 //current object is not altered
 void operator-=(const Matrix2DV2N4<T> &op);  //overloaded subtraction equality operator, adds MatrixV1N1 to the current object.  The current object is altered; 
 //nothing is returned.
 Matrix2DV2N4<T> operator*(const Matrix2DV2N4<T> &op); //overloaded multiplication operator, multiplies op to the current object and returns a new 
 //Matrix3D object.  The current object is not altered.
 void operator*=(const Matrix2DV2N4<T> &op); //overloaded multiplication equality operator, multiplies op to the current object.  The current object 
 //is altered; nothing is returned.
 
private:
 //UTILITY FUNCTIONS
 inline void rot(Matrix2DV2N4<T> &a, const double s, const double tau, const int i, const int j, const int k, const int l);
 inline void eigsrt(T *d, Matrix2DV2N4<T> &v);
 void jacobiEigenSystem3DRoutine(T *d, Matrix2DV2N4<T> &v);  //calculates the Jacobi Eigensystem of a 3x3 matrix
 


};



//CONSTRUCTORS
template<class T>
Matrix2DV2N4<T>::Matrix2DV2N4()
//default constructor
:Volume2DV2N4<T>()
{

}

template<class T>
Matrix2DV2N4<T>::Matrix2DV2N4(int RDIMv, int CDIMv, T element)
//initializes deque and sets the data members to element (default is T())
:Volume2DV2N4<T>(RDIMv,CDIMv,element)
{

}

template<class T>
Matrix2DV2N4<T>::Matrix2DV2N4(const Matrix2DV2N4<T> &copy)
//copy constructor
:Volume2DV2N4<T>(copy)
{
 

}

template<class T>
Matrix2DV2N4<T>::Matrix2DV2N4(const Volume2DV2N4<T> &copy)
:Volume2DV2N4<T>(copy)
//copy constructor
{

}


//PRINT FUNCTIONS
template<class T>
void Matrix2DV2N4<T>::printCL() const 
//print out on the command line in matrix format one row per line and tabs between column entries
{
 for(int ridx = 0; ridx < this->getRDIM(); ridx++)
 {
  std::cout<<std::endl;
  for(int cidx = 0; cidx < this->getCDIM(); cidx++)
   std::cout<<this->getElement(ridx,cidx)<<"\t";
 }

 std::cout<<std::endl;

}



//MATHEMATICAL OPERATIONS

template<class T>
void Matrix2DV2N4<T>::scalarMultiply(T scalar)  
//scalar multiplies the current object.  The current object is altered.
{
 for(int idx = 0; idx < this->SIZE; idx++)
 {
  this->setElement(idx, scalar*((*this)[idx]));
 }
}

template<class T>
void Matrix2DV2N4<T>::scalarMultiplyRow(T scalar, int ridx) 
//scalar multiples the current object row = ridx by T.  The current object is altered.
{

 for(int cidx = 0; cidx < (this->getCDIM()); cidx++)
 {
  this->setElement(ridx, cidx, (scalar*(this->getElement(ridx,cidx))));
 }

}


template<class T>
T Matrix2DV2N4<T>::getMaxElement() 
//returns the greatest element in the matrix
{
#if ERRORCHECK
 if(this->SIZE == 0)
 {
  std::cout<<"ERROR in T Matrix2DV2N4<T>::getMaxElement()"<<std::endl; 
  std::cout<<"SIZE = "<<this->SIZE<<std::endl;
  std::cout<<"Cannot return a maximum element; Matrix is empty."<<std::endl;
  exit(1);
 }
#endif
 
 T maxelement = (*this)[0];

 for(int idx = 0; idx < (this->SIZE); idx++)
 {
  if(((*this)[idx]) > maxelement)
   maxelement = ((*this)[idx]);
 }

 return maxelement;

}

template<class T>
T Matrix2DV2N4<T>::getMinElement() 
//returns the least element in the matrix
{

#if ERRORCHECK
 if(this->SIZE == 0)
 {
  std::cout<<"ERROR in T Matrix2DV2N4<T>::getMinElement()"<<std::endl; 
  std::cout<<"SIZE = "<<this->getSIZE()<<std::endl;
  std::cout<<"Cannot return a minimum element; Matrix is empty.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
  
 T minelement = (*this)[0];

 for(int idx = 0; idx < (this->SIZE); idx++)
 {
  if(((*this)[idx]) < minelement)
   minelement = ((*this)[idx]);
 }

 return minelement;


}
 

template<class T>
inline void Matrix2DV2N4<T>::scale(T range)  
//scales the matrix to run between 0 and range
{

 T maxelement = this->getMaxElement();

#if ERRORCHECK 
 if(maxelement == T())
 {
  std::cout<<"ERROR in void Matrix2DV2N4<T>::scale(T range)"<<std::endl;
  std::cout<<"The maximum element is zero; cannot scale.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
  
 maxelement = static_cast<T>(1.0f/(static_cast<double>(maxelement)));
 this->scalarMultiply(maxelement);
 this->scalarMultiply(range);
   
}

template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::inverse2x2()
//implementation of a simple algorithm for determining the inverse of a 2x2 matrix
{

#if ERRORCHECK
 if((this->getRDIM() != 2) || (this->getCDIM() != 2))
 {
  std::cout<<"ERROR in Matrix2DV2N4<T> Matrix2DV2N4<T>::inverse2x2()"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Dimensions must be 2x2.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 
 T det = ((*this)[0]) * ((*this)[3]);
 det -= ((*this)[1]) * ((*this)[2]);

 Matrix2DV2N4<T> result(2,2);

 result.setElement(0,(*this)[3]/det);
 result.setElement(1,-(*this)[1]/det);
 result.setElement(2,-(*this)[2]/det);
 result.setElement(3,(*this)[0]/det);

 return result;

}

template<typename T>
void Matrix2DV2N4<T>::swap(std::pair<int,SwapType> rowcol1, std::pair<int, SwapType> rowcol2)
//swaps a row and a column.  Each std::pair argument is a reference to the first entry in the row or column to be swapped. 
{
 
#if ERRORCHECK
 if(this->getRDIM() != this->getCDIM())
 {
  std::cout<<"ERROR in void Matrix2DV2N4<T>::swap(std::pair<int,SwapType> rowcol1, std::pair<int, SwapType> rowcol2)"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Matrix must be square.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 int *index1 = new int[this->getRDIM()];
 int *index2 = new int[this->getCDIM()];
 
 switch(rowcol1.second)
 {
 case ROW:
  for(int i = 0; i < this->getRDIM(); i++){
   index1[i] = this->convertToIndex(i,rowcol1.first);
  }
  break;
 case COLUMN:
  for(int i = 0; i < this->getRDIM(); i++){
   index1[i] = this->convertToIndex(rowcol1.first,i);
  }
  break;
 }

 switch(rowcol2.second)
 {
 case ROW:
  for(int i = 0; i < this->getRDIM(); i++){
   index2[i] = this->convertToIndex(i,rowcol2.first); 
  }
  break;
 case COLUMN:
  for(int i = 0 ; i < this->getRDIM(); i++){
   index2[i] = this->convertToIndex(rowcol2.first,i);
  }
  break;
 }

 T temp = 0;

 for(int i = 0; i < this->getRDIM(); i++)
 {

  temp = (*this)[index1[i]];
  
  //assigns to the first argument, index1, the value of the second argument, index2.
  this->setElement(index1[i],(*this)[index2[i]]);
  
  //copies the temp value to the second argument.
  this->setElement(index2[i],temp);
 }
  
}


template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::gaussJordanInverse() const
//returns the inverse of a square matrix
{

#if ERRORCHECK
 if(this->getRDIM() != this->getCDIM())
 {
  std::cout<<"ERROR in Matrix2DV2N4<T> Matrix2DV2N4<T>::gaussJordanInverse()"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Matrix must be square"<<std::endl;
  exit(1);
 }
#endif

 Matrix2DV2N4<T> inverse(*this);
 Matrix2DV2N4<T> b(this->getRDIM(), this->getCDIM());

 int *indxc = new int[this->getRDIM()];
 int *indxr = new int[this->getRDIM()];
 std::vector<int> ipiv(this->getRDIM(), 0);

 int irow, icol;
 double big, pivinv, dum;
 for(int i = 0; i < this->getRDIM(); i++)
 {
  big = 0.0;
  for(int j =  0; j < this->getRDIM(); j++)
  {
   if(ipiv.at(j) != 1)
    for(int k = 0; k < this->getRDIM(); k++)
    {
     if(ipiv.at(k) == 0)
     {
      if(static_cast<double>(std::abs(inverse.getElement(j,k))) >= big)
      {
       big = static_cast<double>(std::abs(inverse.getElement(j,k)));
       irow = j;
       icol = k;
      }
     }
    }
  }
  ++(ipiv[icol]);
 if(irow != icol)
 {
  std::pair<int, SwapType> idx1, idx2;
  idx1.first = irow;
  idx1.second = ROW;
  idx2.first = icol;
  idx2.second = ROW;
  inverse.swap(idx1,idx2);
  b.swap(idx1,idx2);
 }
 indxr[i] = irow;
 indxc[i] = icol;

#if ERRORCHECK
 if(inverse.getElement(icol,icol) == 0)
 {
  std::cout<<"Singular Matrix.  Can't perform operation.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 pivinv = 1.0f / static_cast<double>(inverse.getElement(icol,icol));
 inverse.setElement(icol,icol,1.0);
 
 for(int l = 0; l < this->getRDIM(); l++)
 {
  inverse.setElement(icol,l,static_cast<T>((inverse.getElement(icol,l)) * pivinv));
 }

 for(int l = 0; l < this->getRDIM(); l++)
 {
  b.setElement(icol,l,static_cast<T>((b.getElement(icol,l)) * pivinv));
 }
 for(int ll = 0; ll < this->getRDIM(); ll++)
 {
  if(ll != icol)
  {
   dum = static_cast<double>(inverse.getElement(ll,icol));
   inverse.setElement(ll,icol,T());
   for(int l = 0; l < this->getRDIM(); l++)
   {
    inverse.setElement(ll,l,(inverse.getElement(ll,l) - (inverse.getElement(icol, l)*dum)));
   }
   for(int l = 0; l < this->getRDIM(); l++)
   {
    b.setElement(ll,l,(b.getElement(ll,l) - ((b.getElement(icol, l))*dum)));
   }
  }
 }
 }

 for(int l = (this->getRDIM())-1; l >=0; l--)
  if(indxr[l] != indxc[l])
  {
   std::pair<int, SwapType> idx1, idx2;
   idx1.first = indxr[l];
   idx1.second = COLUMN;
   idx2.first = indxc[l];
   idx2.second = COLUMN;
   inverse.swap(idx1,idx2);
  }


 return inverse;
 

}



template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::rotate2xNMatrix(double angle)
//function which rotates the current matrix by an arbitrary angle (assumed to be in radians).  Returns the rotated matrix and DOES NOT ALTER THE 
//ORIGINAL DATA.
{
 Matrix2DV2N4<T> rotationmatrix(2,2);
 
 rotationmatrix.setMatrixElement(0,0,static_cast<T>(cosf(angle)));
 rotationmatrix.setMatrixElement(1,1,static_cast<T>(cosf(angle)));
 rotationmatrix.setMatrixElement(0,1,static_cast<T>(-sinf(angle)));
 rotationmatrix.setMatrixElement(1,0,static_cast<T>(sinf(angle)));

 return (rotationmatrix*(*this));

}

template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::calculatePseudoInverse()  
//calculates the Moore-Penrose Pseudoinverse, B*, and returns it as a Matrix2D, B* = ((B'*B)^-1)*B'
{

 Matrix2DV2N4<T> tpose(this->transpose());

 Matrix2DV2N4<T> intermediate(tpose*(*this));
 Matrix2DV2N4<T> final(intermediate.gaussJordanInverse());
 
 return (Matrix2DV2N4<T>(final*tpose));

}

template<class T>
void Matrix2DV2N4<T>::calculateJacobiEigenSystem3D(T *result)  
//calculates the Jacobi Eigensystem of a 3x3 matrix
{
 T d[3];
 Matrix2DV2N4<T> V(3,3);
 for(int idx = 0; idx < 3; idx++)
  d[idx] = 0.0;

 this->jacobiEigenSystem3DRoutine(d,V);
 
 this->eigsrt(d,V);

 for(int idx = 0; idx < 3; idx++)
 {
  result[idx] = d[idx];
  result[idx+3] = V.getElement(idx,0);
  result[idx+6] = V.getElement(idx,1);
  result[idx+9] = V.getElement(idx,2);
 }


}

template<class T>
void Matrix2DV2N4<T>::solveForGeneratingMatrix(const EigenSystem3DV1N4<T> &esys)  
//creates the 3x3 matrix which would generate the EigenSystem3D esys, using the matrix diagonalization relation, A = PDP^{-1}
{

 this->initVolume2D(3,3);

 //construct the diagonal eigenvalue matrix
 Matrix2DV2N4<T> D(3,3,T());
 D.setElement(0,0,esys.l1);
 D.setElement(1,1,esys.l2);
 D.setElement(2,2,esys.l3);

 //construct the matrix P that has the eignvectors as the columns
 Matrix2DV2N4<T> P(3,3,T());

 P.setMatrixColumn(esys.v1,0);
 P.setMatrixColumn(esys.v2,1);
 P.setMatrixColumn(esys.v3,2);

 Matrix2DV2N4<T> Pinv(P.gaussJordanInverse());
 Matrix2DV2N4<T> PD(P*D);

 this->copyVolume2D(PD*Pinv);
 
}
 
template<class T>
void Matrix2DV2N4<T>::solveForSymmetricGeneratingMatrix(const EigenSystem3DV1N4<T> &esys)  
//creates the symmetric 3x3 matrix which would generate the EigenSystem3D esys, using the matrix diagonalization relation, A = PDP^{T}
{

 this->initVolume2D(3,3);

 //construct the diagonal eigenvalue matrix
 Matrix2DV2N4<T> D(3,3,T());
 D.setElement(0,0,esys.l1);
 D.setElement(1,1,esys.l2);
 D.setElement(2,2,esys.l3);

 //construct the matrix P that has the eignvectors as the columns
 Matrix2DV2N4<T> P(3,3,T());

 P.setMatrixColumn(esys.v1,0);
 P.setMatrixColumn(esys.v2,1);
 P.setMatrixColumn(esys.v3,2);

 Matrix2DV2N4<T> Ptrans(3,3);
 P.transpose(Ptrans);
 Matrix2DV2N4<T> PD(P*D);

 this->copyVolume2D(PD*Ptrans);
 
}

template<class T>
void Matrix2DV2N4<T>::singularValueDecomposition(Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V)  
//given an mxn matrix, this computes its SVD (A = UwV').  The current object is replaced by U on output. w is the output of singular values (does not require allocation). V is output as 
//an nxn matrix, not its transpose.
{
 w.initVolume2D(this->getRDIM(),1);
 V.initVolume2D(this->getCDIM(), this->getCDIM());
 
 bool flag;
 int i,its,j,jj,k,l,nm;
 double anorm,c,f,g,h,s,scale,x,y,z;
 
 double element;

 int m = this->getRDIM();
 int n = this->getCDIM();

 double *rv1 = new double[n];

 g = scale = anorm = 0.0f;

 
 for(i = 0; i < n; i++)
 {
  
  l = i + 2;
  rv1[i] = scale*g;
  g = s = scale = 0.0;
  if(i < m) 
  {
   for(k = i; k < m; k++)
    scale += fabsf(static_cast<double>(this->getElement(k,i)));
   if(scale != 0.0f)
   {
    for(k = i; k < m; k++)
    {
     element = static_cast<double>(this->getElement(k,i));
     element /= scale;
     this->setElement(k,i,element);
     s += static_cast<double>((this->getElement(k,i)))*static_cast<double>((this->getElement(k,i)));
    }
    f = static_cast<double>(this->getElement(i,i));
    g = -1.0f*(hageman::matchSign<double>(sqrtf(s),f));
    h = f * g - s;
    this->setElement(i,i,static_cast<T>(f - g));
      
    for(j = l - 1; j < n; j++)
    {
     for(s = 0.0,k = i; k < m; k++)
      s += static_cast<double>((this->getElement(k,i)))*static_cast<double>((this->getElement(k,j)));
     f = s / h;
     for(k = i; k < m; k++)
     {
      element = static_cast<double>(this->getElement(k,i));
      element *= f;
      element += this->getElement(k,j);
      this->setElement(k,j,element);
     }
    }

    for(k = i; k < m; k++)
     this->setElement(k,i,scale*this->getElement(k,i));
   }
    
  }

  w.setElement(i,1,(scale * g));

   
  g = s = scale = 0.0f;
  if(i + 1 <= m && i != n) 
  {
   for(k = l - 1; k < n; k ++)
    scale += fabsf(this->getElement(i,k));

   if(scale != 0.0f)
   {
    for(k = l-1; k < n; k++)
    {
     element = static_cast<double>(this->getElement(i,k));
     element /= scale;
     this->setElement(i,k,element);
     s += static_cast<double>((this->getElement(i,k)))*static_cast<double>((this->getElement(i,k)));
    }
    f = static_cast<double>(this->getElement(i,l-1));
    g = -1.0f*(hageman::matchSign<double>(sqrtf(s),f));
    h = f * g - s;
    this->setElement(i,l-1,static_cast<T>(f - g));
     
    for(k = l - 1; k < n; k++)
     rv1[k] = (static_cast<double>(this->getElement(i,k)))/h;

    for(j = l - 1; j < m; j++)
    {
     for(s = 0.0,k = l-1; k < n; k++)
      s += static_cast<double>((this->getElement(j,k)))*static_cast<double>((this->getElement(i,k)));
      
     for(k = l-1; k < n; k++)
     {
      element = static_cast<double>(this->getElement(j,k));
      element += (s*rv1[k]);
      this->setElement(j,k,element);
     }
    }
    for(k = l-1; k < n; k++)
     this->setElement(i,k,scale*this->getElement(i,k));
     
     
   }
  }

  anorm = getMax<double>(anorm, fabsf(w.getElement(i,1))+fabsf(rv1[i]));

 }

  
 for(i = n-1; i >= 0; i--)
 {
   if(i < n-1)
   {
    if(g != 0.0f)
    {
     for(j = l; j < n; j++)
     {
      element = ((this->getElement(i,j))/(this->getElement(i,l)))/g;
      V.setElement(j,i,element);
     }
     for(j = l; j < n; j++)
     {
      for(s = 0.0,k = l;k < n; k++)
       s += static_cast<double>((this->getElement(i,k)))*static_cast<double>((V.getElement(k,j)));
      
      for(k = l; k < n; k++)
      {
       element = static_cast<double>(V.getElement(k,i));
       element *= s;
       element += static_cast<double>(V.getElement(k,j));
       V.setElement(k,j,element);
      }
     }
    }

    for(j = l; j < n; j++)
    {
     V.setElement(i,j,0.0f);
     V.setElement(j,i,0.0f);
    }
   }

   V.setElement(i,i,1.0f);
   g = rv1[i];
   l = i;
  }


  for(i = (getMin<int>(m,n))-1; i >= 0; i--)
  {
   
   l = i + 1;
   g = w[i];

   for(j = l; j < n; j++)
    this->setElement(i,j,0.0f);

   if(g != 0.0f) 
   {
    g = 1.0f / g;

    for(j = l; j < n; j++)
    {
     for(s = 0.0,k = l; k < m; k++)
      s += static_cast<double>((this->getElement(k,i)))*static_cast<double>((this->getElement(k,j)));

     f = s / (static_cast<double>(this->getElement(i,i)));
     f *= g;

     for(k = i; k < m; k++)
     {
      element = static_cast<double>(this->getElement(k,i));
      element *= f;
      element += static_cast<double>(this->getElement(k,j));
      this->setElement(k,j,element);
     }
    }

    for(j = i; j < m; j++)
    {
     element = g*(static_cast<double>(this->getElement(j,i)));
     this->setElement(j,i,element);
    }

   }else for(j = i; j < m; j++) this->setElement(j,i,0.0f);

   this->setElement(i,i,(this->getElement(i,i)) + 1);
  }

  for(k=n-1; k >= 0; k--)
  {
   for(its = 0; its < 30; its++)
   {
    flag = true;
    for(l = k; l >= 0; l--)
    {
     nm = l - 1;
     if(fabsf(rv1[l]) + anorm == anorm)
     {
      flag = false;
      break;
     }
     if(fabsf(w[nm]) + anorm == anorm)
      break;
    }
    
    if(flag)
    {
     c = 0.0;
     s = 1.0;
     for(i = l - 1; i < (k+1); i++)
     {
      f = s*rv1[i];
      rv1[i] = c*rv1[i];
      if(fabsf(f) + anorm == anorm)
       break;
      g = w[i];

      h = computePythagoreanTheorem<double>(f,g);
      
      w[i] = h;
      h = 1.0f / h;
      c = g*h;
      s = -f*h;

      for(j = 0; j < m; j++)
      {
       y = this->getElement(j,nm);
       z = this->getElement(j,i);

       this->setElement(j,nm,(y*c+z*s));
       this->setElement(j,i,(z*c-y*s));
      }
     }
    }

    z = w[k];

    if(l == k) 
    {
     if(z < 0.0f) 
     {
      w[k] = -z;
      for(j = 0; j < n; j++)
       V.setElement(j,k,-1.0f*V.getElement(j,k));
     }
     break;
    }

    if(its == 29)
    {
     std::cout<<"ERROR: No convergence in 30 svdcmp iterations"<<std::endl;
     exit(1);
    }

    x = w[l];
    nm = k-1;
    y = w[nm];
    g = rv1[nm];
    h = rv1[k];
    f = ((y-z)*(y+z) + (g-h)*(g+h))/(2.0f*h*y);
    g = computePythagoreanTheorem<double>(f,1.0f);

    f = ((x-z)*(x+z)+h*((y/(f+hageman::matchSign<double>(g,f)))-h))/x;
    c = s = 1.0f;
    for(j = l; j <= nm; j++)
    {
     i = j + 1;
     g = rv1[i];
     y = w[i];
     h = s*g;
     g = c*g;
     z= computePythagoreanTheorem<double>(f,h);
     rv1[j] = z;
     c = f/z;
     s = h/z;
     f = x*c+g*s;
     g = g*c-x*s;
     h = y*s;
     y = y*c;

     for(jj = 0; jj < n; jj++)
     {
      x = V.getElement(jj,j);
      z = V.getElement(jj,i);
      V.setElement(jj,j,(x*c+z*s));
      V.setElement(jj,i,(z*c-x*s));
     }

     z = computePythagoreanTheorem<double>(f,h);
     w[j] = z;

     if(z)
     {
      z = (1.0f)/z;
      c = f*z;
      s = h*z;
     }

     f = c*g+s*y;
     x = c*y-s*g;
     for(jj = 0; jj < m; jj++)
     {
      y = this->getElement(jj,j);
      z = this->getElement(jj,i);
      this->setElement(jj,j,y*c+z*s);
      this->setElement(jj,i,z*c-y*s);
     }
    }

    rv1[l] = 0.0f;
    rv1[k] = f;
    w[k] = x;
   }
    
 
 }

 delete[] rv1;


}

template<class T>
void Matrix2DV2N4<T>::SVDBackSolve(Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V, Matrix2DV2N4<T> &b, Matrix2DV2N4<T> &x)  
//this function computes the solution of Ax=b by backsolving for x using the SVD matrices computed singularValueDecomposition(), A = UwV'.  U is assumed to be the current object.  
//Memory does not need to be allocated for any object, since all objects but x are defined.  NOTE: this function assumes that the w's have already been zeroed.
{

 //b is assumed to be a row vector
 x.initVolume2D(b.getRDIM(),1);

 int jj,j,i;
 double s;

 int m = this->getRDIM();
 int n = this->getCDIM();

 double *tmp = new double[n];

 for(j = 0; j < n; j++)
 {
  s = 0.0;
  if(w.getElement(j,1) != 0.0f)
  {
   for(i = 0; i < m; i++)
    s += (b.getElement(i,1))*(this->getElement(i,j));

   s = s/(w.getElement(j,1));
  }

  tmp[j] = s;
 }

 for(j = 0; j < n; j++)
 {
  s = 0.0;
  for(jj = 0; jj < n; jj++)
   s += tmp[jj]*(V.getElement(j,jj));

  x.setElement(j,1) = s;
 }

 delete[] tmp;

}
 
template<class T>
inline void Matrix2DV2N4<T>::editSingularValues(T tol) 
//sets the singular values of the w matrix to zero based on a threshold.  Assumes that the current object is a row vector obtained from the SVD
//decomposition
{

 
 //finds the maximum w
 T wmax = this->getMaxElement();

 T thresh = tol*wmax;

 for(int idx = 0; idx < (this->getRDIM()); idx++)
 {
  if((this->getElement(idx,1)) < thresh)
  {
   this->setElement(idx,1,0.0);
  }
 }


}
template<class T>
void Matrix2DV2N4<T>::changeBasis(const Matrix2DV2N4<T> &basisMatrix)  
//reexpresses the current object in the new basis.  basisMatrix is a matrix whose columns are the basis vectors.  The current object data is destroyed.
{
 Matrix2DV2N4<T> temp(*this);
 (*this) = (basisMatrix.gaussJordanInverse())*(temp);
 
}


//OVERLOADED OPERATORS

template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::operator+(const Matrix2DV2N4<T> &op)
//overloaded addition operator, adds op to the current object and returns a new Matrix2D object.  The current object is not altered
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in Matrix2DV2N4<T> Matrix2DV2N4<T>::operator+(const Matrix2DV2N4<T> &op) const"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot add objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 Matrix2DV2N4<T> result(*this);

 for(int idx = 0; idx < result.getSIZE(); idx++)
 {
  result.setElement(idx, result.getElement(idx) + op.getElement(idx));
 }

 return result;

}

template<class T>
void Matrix2DV2N4<T>::operator+=(const Matrix2DV2N4<T> &op)  
//overloaded addition equality operator, adds op to the current object.  The current object is altered; nothing is returned.
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in void Matrix2DV2N4<T>::operator+=(const Matrix2DV2N4<T> &op)"<<std::endl;  
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot add objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
  
 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  this->setElement(idx, (this->getElement(idx)) + op.getElement(idx));
 }

}


template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::operator-(const Matrix2DV2N4<T> &op) const
//overloaded subtraction operator, subtracts op from the current object and returns a new Matrix2D object.  The current object is not altered
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in Matrix2DV2N4<T> Matrix2DV2N4<T>::operator-(const Matrix2DV2N4<T> &op) const."<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot subtract objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 Matrix2DV2N4<T> result(*this);

 for(int idx = 0; idx < result.getSIZE(); idx++)
 {
  result.setElement(idx, result.getElement(idx) - op.getElement(idx));
 }


 return result;

}



template<class T>
void Matrix2DV2N4<T>::operator-=(const Matrix2DV2N4<T> &op)  
//overloaded subtraction equality operator, subtracts op from the current object.  The current object is altered; nothing is returned.
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in void Matrix2DV2N4<T>::operator-=(const Matrix2DV2N4<T> &op)"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot subtract objects because the dimensions do not match"<<std::endl;
  exit(1);
 }
#endif
   
 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  this->setElement(idx, (this->getElement()) - op.getElement(idx));
 }
}

template<class T>
Matrix2DV2N4<T> Matrix2DV2N4<T>::operator*(const Matrix2DV2N4<T> &op) 
//overloaded multiplication operator, multiplies op to the current object and returns a new Matrix3D object.  The current object is not altered.
{
#if ERRORCHECK
 if(this->getCDIM() != op.getRDIM())
 {
  std::cout<<"ERROR in Matrix2DV2N4<T> Matrix2DV2N4<T>::operator*(const Matrix2DV2N4<T> &op)"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot multiply objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 Matrix2DV2N4<T> result(this->getRDIM(),op.getCDIM());
 T element = T();
   
 for(int k = 0; k < result.getCDIM(); k++)
  for(int i = 0; i < result.getRDIM(); i++)
  {
   element = T();
   for(int j = 0; j < this->getCDIM(); j++)
    element += ((*this).getElement(i,j))*(op.getElement(j,k));
   
   result.setElement(i,k,element);
  }

 return result;

}

template<class T>
void Matrix2DV2N4<T>::operator*=(const Matrix2DV2N4<T> &op) 
//overloaded multiplication equality operator, multiplies op to the current object.  The current object is altered; nothing is returned.
{
 //CONSIDER REIMPLEMENTING THIS WITH OUT THE TEMPORARY OBJECT RESULT.
 Matrix2DV2N4<T> result((*this)*op);
 (*this) = result;


}

//UTILITY FUNCTIONS
template<class T>
void Matrix2DV2N4<T>::jacobiEigenSystem3DRoutine(T *d, Matrix2DV2N4<T> &v)
//this function is modified from one in Numerical Recipes in C++.  It returns the result in the following format, double* = new double [12]), that has the eigensystem in the 
//following format [lambda1 lambda2 lambda3 nu1x nu1y nu1z nu2x nu2y nu2z nu3x nu3y nu3z]
{
 
 Matrix2DV2N4<T> a(*this);
 
 const int n = (this->getRDIM());
 std::vector<double> b(n);
 std::vector<double> z(n);
 
 double tresh;
 double denom;
 double h;
 double t;
 double theta;
 double c;
 double s;
 double tau;
 double g;
 double sm;

 
 for(int cidx = 0; cidx < v.getCDIM(); cidx++)
  for(int ridx = 0; ridx < v.getRDIM(); ridx++)
  {
   if(ridx == cidx)
    v.setElement(ridx,cidx,static_cast<T>(1.0));
   else
    v.setElement(ridx,cidx,T());
  }

 for(int ip = 0; ip < n; ip++)
 {
  b.at(ip) = static_cast<double>(a.getElement(ip,ip));
  d[ip] = a.getElement(ip,ip);
  
  z.at(ip) = 0.0;
 }

 int nrot = 0;
 for(int i = 1; i <=500; i++)
 {
  sm = 0.0;
  for(int ip = 0; ip < n-1; ip++)
  {
   for(int iq = ip+1; iq<n; iq++)
    sm += fabsf((double)a.getElement(ip,iq));
  }
  if(sm == 0.0)
  {
   return;
   
  }
  if(i < 4)
  {
   denom = powf(static_cast<double>(n),2.0f);
   denom = powf(denom, -1.0f);
   tresh = 0.2f*sm*denom;
  }
  else
   tresh = 0.0;
  for(int ip = 0; ip < n-1; ip++)
  {
   for(int iq = ip+1; iq < n; iq++)
   {
    g = 100.0f*fabsf((double)a.getElement(ip,iq));
    if((i > 4) && ((fabsf(static_cast<double>(d[ip]))+g) == fabsf(static_cast<double>(d[ip]))) && ((fabsf(static_cast<double>(d[iq]))+g) == fabsf(static_cast<double>(d[iq]))))
     a.setElement(ip,iq,T());
    else if(fabsf((double)a.getElement(ip,iq)) > tresh)
    {
     h = static_cast<double>(d[iq] - d[ip]);
     if((fabsf(h)+g) == fabsf(h))
      t = (double)(a.getElement(ip,iq))*(powf(h, -1));
     else
     {
      theta = 0.5f*h*(powf((double)(a.getElement(ip,iq)), -1.0f));
      t = fabsf(theta) + sqrtf(1.0f + theta * theta);
      t = powf(t, -1);
      if(theta < 0.0)
       t = -t;
     }
     c = sqrtf(1.0f + t*t);
     c = powf(c, -1.0f);
     s = t*c;
     tau = s*(powf((1.0f +c), -1.0f));
     h = t*((double)a.getElement(ip,iq));
     z.at(ip) -= h;
     z.at(iq) += h;
     d[ip] -= static_cast<T>(h);
     d[iq] += static_cast<T>(h);
     a.setElement(ip,iq,T());

     for(int j = 0; j < ip; j++)
      rot(a,s,tau,j,ip,j,iq);
     for(int j = ip+1; j < iq; j++)
      rot(a,s,tau,ip,j,j,iq);
     for(int j = iq+1; j < n; j++)
      rot(a,s,tau,ip,j,iq,j);
     for(int j = 0; j < n; j++)
      rot(v,s,tau,j,ip,j,iq);
     ++nrot;
    }
   }
  }
  for(int ip = 0; ip < n; ip++)
  {
   b.at(ip) += z.at(ip);
   d[ip] = static_cast<T>(b.at(ip));
   z.at(ip) = 0.0;
  }
 }
 
#if ERRORCHECK
 std::cout<<"WARNING in void Matrix2DV2N4<T>::calculateJacobiEigenSystem3D(T eigvals[], Matrix2DV2N4<T> &eigvecs)."<<std::endl;
 std::cout<<"Too many iterations in Jacobi routine.  Stopping..."<<std::endl;
 std::cout<<"Returning a zero eigensystem."<<std::endl;
#endif

 srand(time(NULL));
 
 //sets the eigenvalues to zero
 d[0] = 0;
 d[1] = 0;
 d[2] = 0;

 //sets the eigenvectors to some set of random coordinates.  
 double *v1v = new double[3];
 double *v2v = new double[3];
 double *v3v = new double[3];

 //sets v1 to a random vector, covering -1 to +1
 for(int idx = 0; idx < 3; idx++)
 {
  v1v[idx] = static_cast<double>(rand() % 200) -  100.0f;
  v1v[idx] /= 100.0f;
 }

 normalizeArray<double,3>(v1v);
 
 //gets a random perpendicular vector to v1
 getPerpendicularArray<double,3>(v2v,v1v);
 normalizeArray<double,3>(v2v);
 
 
 //solves for v3v using the cross product
 calculateCrossProductForArray3<double>(v3v,v1v,v2v);
 normalizeArray<double,3>(v3v);


 for(int idx = 0; idx < 3; idx++)
 {
  v.setElement(idx,0,static_cast<T>(v1v[idx]));
  v.setElement(idx,1,static_cast<T>(v2v[idx]));
  v.setElement(idx,2,static_cast<T>(v3v[idx]));
 }



}

template<class T>
void Matrix2DV2N4<T>::rot(Matrix2DV2N4<T> &a, const double s, const double tau, const int i, const int j, const int k, const int l)
{
 double g;
 double h;
 
 g = static_cast<double>(a.getElement(i,j));
 h = static_cast<double>(a.getElement(k,l));
 double val1 = g-s*(h+g*tau);
 a.setElement(i,j,static_cast<T>(val1));
 double val2 = h+s*(g-h*tau);
 a.setElement(k,l,static_cast<T>(val2));
}

template<class T>
void Matrix2DV2N4<T>::eigsrt(T *d, Matrix2DV2N4<T> &v)
{
 double p;
 int k;
 int n = v.getRDIM();

 for(int i = 0; i < n-1; i++)
 {
  p = d[k = i];
  for(int j = i; j < n; j++)
   if(d[j] >= p)
    p=d[k=j];
   if(k != i)
   {
    d[k] = d[i];
    d[i] = p;
    for(int j = 0 ; j < n; j++)
    {
     p = v.getElement(j,i);
     v.setElement(j,i,v.getElement(j,k));
     v.setElement(j,k,p);
    }
   }
 }

 
}

template<class T>
std::ostream& operator<<(std::ostream& os, const hageman::Matrix2DV2N4<T> &output)
//overloaded ostream operator that outputs the data is matrix format on the CL
{
 
 int index;
 
 for(int ridx = 0; ridx < output.getRDIM(); ridx++)
 {
  for(int cidx = 0; cidx < output.getCDIM(); cidx++)
  {
   index = output.convertToIndex(ridx,cidx);
   os << output.getElement(index) << "  ";
  }
  os << std::endl;
  
 }

 return os;

}


template<class T>
class Matrix3DV2N4 : public Volume3DV2N4<T>{

public:
 //CONSTRUCTORS
 Matrix3DV2N4();  //default constructor
 Matrix3DV2N4(int RDIMv, int CDIMv, int ZDIMv, T element = T());  //initializes array and sets the data members to element (default is T())
 Matrix3DV2N4(const Matrix3DV2N4<T> &copy);  //copy constructor
 
 //MATHEMATICAL OPERATIONS
 void scalarMultiply(T scalar);  //scalar multiplies the current object.  The current object is altered.
 inline T getMaxElement(); //returns the greatest element in the matrix
 inline T getMinElement(); //returns the greatest element in the matrix
 void scale(T range);  //scales the matrix to run between 0 and range
 
 template<class S>  //member function template allows an arbitrary datatype for the mask
 T average(const Matrix3DV2N4<S> *mask = NULL);  //calculates the simple average of the matrix elements.  It takes an optional mask volume that defines a subregion to take an average of
 
 //OVERLOADED OPERATORS
 Matrix3DV2N4<T> operator+(const Matrix3DV2N4<T> &op) const;  //overloaded addition operator, adds op to the current object and returns a new Matrix3D object.  The
 //current object is not altered
 void operator+=(const Matrix3DV2N4<T> &op);  //overloaded addition equality operator, adds MatrixV1N1 to the current object.  The current object is altered; 
 //nothing is returned.
 Matrix3DV2N4<T> operator-(const Matrix3DV2N4<T> &op) const;  //overloaded addition operator, adds op to the current object and returns a new Matrix3D object.  The
 //current object is not altered
 void operator-=(const Matrix3DV2N4<T> &op);  //overloaded addition equality operator, adds MatrixV1N1 to the current object.  The current object is altered; 
 //nothing is returned.


protected:


};


//CONSTRUCTORS
template<class T>
Matrix3DV2N4<T>::Matrix3DV2N4()
//default constructor
:Volume3DV2N4<T>()
{

}

template<class T>
Matrix3DV2N4<T>::Matrix3DV2N4(int RDIMv, int CDIMv, int ZDIMv, T element)
//initializes array and sets the data members to element (default is T())
:Volume3DV2N4<T>(RDIMv,CDIMv,ZDIMv,element)
{

}

template<class T>
Matrix3DV2N4<T>::Matrix3DV2N4(const Matrix3DV2N4<T> &copy)
//copy constructor
:Volume3DV2N4<T>(copy)
{

}


//MATHEMATICAL OPERATIONS
template<class T>
void Matrix3DV2N4<T>::scalarMultiply(T scalar)  
//scalar multiplies the current object.  The current object is altered.
{
 for(int idx = 0; idx < (this->getSIZE()); idx++)
 {
  this->setElement(idx, scalar*((this->data)[idx]));
 }
}

template<class T>
T Matrix3DV2N4<T>::getMaxElement() 
//returns the greatest element in the matrix
{
#if ERRORCHECK
 if(this->getSIZE() == 0)
 {
  std::cout<<"ERROR in T Matrix3DV2N4<T>::getMaxElement()"<<std::endl; 
  std::cout<<"SIZE = "<<this->getSIZE()<<std::endl;
  std::cout<<"Cannot return a maximum element; Matrix is empty.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 T maxelement = (this->data)[0];

 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  if(((this->data)[idx]) > maxelement)
   maxelement = ((this->data)[idx]);
 }

 return maxelement;

}

template<class T>
T Matrix3DV2N4<T>::getMinElement() 
//returns the greatest element in the matrix
{

#if ERRORCHECK
 if(this->getSIZE() == 0)
 {
  std::cout<<"ERROR in T Matrix3DV2N4<T>::getMaxElement()"<<std::endl; 
  std::cout<<"SIZE = "<<this->getSIZE()<<std::endl;
  std::cout<<"Cannot return a maximum element; Matrix is empty.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 T minelement = (this->data)[0];

 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  if(((this->data)[idx]) < minelement)
   minelement = ((this->data)[idx]);
 }

 return minelement;

}

template<class T>
void Matrix3DV2N4<T>::scale(T range)  
//scales the matrix to run between 0 and range
{
 T maxelement = (this->getMaxElement());

#if ERRORCHECK
 if(maxelement == T())
 {
  std::cout<<"ERROR in void Matrix3DV2N4<T>::scale(T range)"<<std::endl;
  std::cout<<"The maximum element is zero.  Cannot scale.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 maxelement = static_cast<T>(1.0f/(static_cast<double>(maxelement)));
 this->scalarMultiply(maxelement);
 this->scalarMultiply(range);

 
}

template<class T>
template<class S>
T Matrix3DV2N4<T>::average(const Matrix3DV2N4<S> *mask)  
//calculates the simple average of the matrix elements.  It takes an optional mask volume that defines a subregion to take an average of.  The mask volume must be binary, with the 
//non-zero element denoting the non-masked elements and the zero element denoting the masked elements.  
{
 T result = T();

 if(mask == NULL)  //the mask is empty
 {
  for(int idx = 0; idx < this->getSIZE(); idx++)
  {
   result += this->getElement(idx);
  }

  if((this->getSIZE()) > 0)  //makes sure to avoid the divide by zero error
   result = (result / static_cast<T>(this->getSIZE()));
 }
 else  //the mask is valid
 {
  int count = 0;  //keeps track of the number of unmasked elements

  for(int idx = 0; idx < this->getSIZE(); idx++)
  {
   if(mask->getElement(idx) != S())  //non-zero mask element denotes an unmasked element
   {
    result += this->getElement(idx);
    count++;
   }
  }

  if(count > 0)  //makes sure that at least one element was counted to avoid the divide by zero error
   result = (result / static_cast<T>(count));

 }

 return result;
}
 


//OVERLOADED OPERATORS
template<class T>
Matrix3DV2N4<T> Matrix3DV2N4<T>::operator+(const Matrix3DV2N4<T> &op) const
//overloaded addition operator, adds op to the current object and returns a new Matrix3D object.  The current object is not altered
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in Matrix3DV2N4<T> Matrix3DV2N4<T>::operator+(const Matrix3DV2N4<T> &op) const"<<std::endl;
  std::cout<<"Cannot add objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
   
 Matrix3DV2N4<T> result(*this);
 for(int idx = 0; idx < result.getSIZE(); idx++)
 {
  result.setElement(idx, result[idx]+op[idx]);
 }

 return result;

}

template<class T>
void Matrix3DV2N4<T>::operator+=(const Matrix3DV2N4<T> &op)  
//overloaded addition equality operator, adds MatrixV1N1 to the current object.  The current object is altered; nothing is returned.
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in void Matrix3DV2N4<T>::operator+=(const Matrix3DV2N4<T> &op)"<<std::endl;
  std::cout<<"Cannot add objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  this->setElement(idx, ((*this)[idx]) + op[idx]);
 }

}


template<class T>
Matrix3DV2N4<T> Matrix3DV2N4<T>::operator-(const Matrix3DV2N4<T> &op) const 
//overloaded subtraction operator, subtracts op from the current object and returns a new Matrix3D object.  The current object is not altered
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in Matrix3DV2N4<T> Matrix3DV2N4<T>::operator-(const Matrix3DV2N4<T> &op) const"<<std::endl;
  std::cout<<"Cannot subtract objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 Matrix3DV2N4<T> result(*this);
 for(int idx = 0; idx < result.getSIZE(); idx++)
 {
  result.setElement(idx, result[idx]-op[idx]);
 }

 return result;

}

template<class T>
void Matrix3DV2N4<T>::operator-=(const Matrix3DV2N4<T> &op) 
//overloaded subtraction equality operator, subtracts op from the current object.  The current object is altered; nothing is returned.
{

#if ERRORCHECK
 if(!(this->areDimensionsEqual(op)))
 {
  std::cout<<"ERROR in void Matrix3DV2N4<T>::operator-=(const Matrix3DV2N4<T> &op)"<<std::endl;
  std::cout<<"Cannot subtract objects because the dimensions do not match.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  this->setElement(idx, (((this->data)[idx])-op[idx]));
 }
}



template<class T>
class VectorV2N4 : public Matrix2DV2N4<T>{

public:
 //CONSTRUCTORS
 VectorV2N4();  //empty constructor
 VectorV2N4(int DIM, T element = T()); //initializes the row vector (1 column) to have DIM elements each of which is initialized to element
 VectorV2N4(const VectorV2N4<T> &copy);  //copy constructor
 VectorV2N4(const T *dataarray, int ARRAYSIZE);  //copy constructor for just an array
 VectorV2N4(const Matrix2DV2N4<T> &copy);  //copy constructor
 
 //ASSIGNMENT FUNCTIONS
 void initVector(const T *dataarray, int ARRAYSIZE);  //initializes a vector with dataarray of size ARRAYSIZE
 void initAverageVector(const VectorV2N4<T> &vec1, const VectorV2N4<T> &vec2);  //this function initializes the vector that is the average of vec1 and vec2

 //GET FUNCTIONS
 
 
 //MATHEMATICAL OPERATIONS
 T dot(const VectorV2N4<T> &op);  //computes and returns the dot product of op and the current object.  neither object is altered.
 double L2norm() const;  //returns the L2 norm 
 void normalize();  //normalizes the vector; the current object is altered and nothing is returned
 std::pair<int,int> getTwoGreatestElementCoords();  //returns the array indices of the two greatest elements
 VectorV2N4<T> getPerpendicularDirection(); //returns a unit vector perpendicular to the current object
 double getEuclideanDistanceMetric(const VectorV2N4<T> &destination);  //returns the generalized Euclidean distance metric between destination and 
 //the current point.  NOTE THAT THE CONVENTION IS d(Destination - (*this))
 VectorV2N4<T> getMidPoint(const VectorV2N4<T> &pt);
 std::pair<T,double> polynomialInterpolation(double pt);

 //PRINT FUNCTIONS
 virtual void printCL() const;
 
 //OPERATOR OVERLOADS
 bool operator<(const VectorV2N4<T> &paradigm) const;

protected:

};

//CONSTRUCTORS
template<class T>
VectorV2N4<T>::VectorV2N4()
//empty constructor
:Matrix2DV2N4<T>()
{

}

template<class T>
VectorV2N4<T>::VectorV2N4(int DIM, T element)
//initializes the row vector to have DIM elements, each of which is initialized to element
:Matrix2DV2N4<T>(DIM,1,element)
{
 
}

template<class T>
VectorV2N4<T>::VectorV2N4(const VectorV2N4<T> &copy)
//copy constructor
:Matrix2DV2N4<T>(copy)
{


}

template<class T>
VectorV2N4<T>::VectorV2N4(const Matrix2DV2N4<T> &copy)
:Matrix2DV2N4<T>(copy)
{
}

template<class T>
VectorV2N4<T>::VectorV2N4(const T *dataarray, int ARRAYSIZE)  
//copy constructor for just an array
:Matrix2DV2N4<T>(ARRAYSIZE,1,T())
{
 for(int idx = 0; idx < ARRAYSIZE; idx++)
  (*this).data[idx] = dataarray[idx];


}


template<class T>
void VectorV2N4<T>::initVector(const T *dataarray, int ARRAYSIZE)
//initializes a vector with dataarray of size ARRAYSIZE
{
 this->initVolume2D(ARRAYSIZE,1,T());
 for(int idx = 0; idx < ARRAYSIZE; idx++)
  (*this).data[idx] = dataarray[idx];
}

template<class T>
void VectorV2N4<T>::initAverageVector(const VectorV2N4<T> &vec1, const VectorV2N4<T> &vec2)
//this function initializes the vector that is the average of vec1 and vec2
{
 const int ARRAYSIZE = vec1.getSIZE();
 this->copyVolume2D(vec1);
 
 (*this) += vec2;
 
 double normresult = .5*(this->L2norm());
 this->normalize();
 this->scalarMultiply(normresult);

}


//MATHEMATICAL OPERATIONS
template<class T>
T VectorV2N4<T>::dot(const VectorV2N4<T> &op)  
//computes and returns the dot product of op and the current object.  neither object is altered.
{
#if ERRORCHECK
 if((this->getSIZE()) != (op.getSIZE()))
 {
  std::cout<<"ERROR in T VectorV2N4<T>::dot(const VectorV2N4<T> &op)"<<std::endl;
  std::cout<<"RDIM = "<<this->getRDIM()<<" CDIM = "<<this->getCDIM()<<std::endl;
  std::cout<<"Cannot compute dot product of vectors; dimensions are not equal.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 T result = T();

 for(int idx = 0; idx < (this->getSIZE()); idx++)
 {
  result += ((this->getElement(idx))*(op.getElement(idx)));
 }


 return result;


}

template<class T>
double VectorV2N4<T>::L2norm() const 
//returns the L2 norm 
{
 double norm = 0.0;

 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  norm += ((this->getElement(idx)) * (this->getElement(idx)));
 }

 norm = sqrtf(norm);
 return norm;

}

template<class T>
void VectorV2N4<T>::normalize()  
//normalizes the vector; the current object is altered and nothing is returned
{
 double norm = this->L2norm();
 T coord;

 if(norm == 0.0)  //if norm is equal to zero, then avoids a divide by zero error
 {

#if ERRORCHECK
  std::cout<<"ERROR in void VectorV2N4<T>::normalize()"<<std::endl;
  std::cout<<"Cannot normalize this vector.  Norm is zero"<<std::endl;
  std::cout<<"Exiting..."<<std::endl;
  exit(1);
#endif
  
#if VERBOSE
  std::cout<<"WARNING in void VectorV2N4<T>::normalize()"<<std::endl;  
  std::cout<<"Cannot normalize this vector.  Norm is zero"<<std::endl;
#endif
 }
 else  //norm is not zero
 {
  for(int idx = 0; idx < this->SIZE; idx++)
  {
   coord = static_cast<double>((*this)[idx]);
   coord /= norm;
   this->setElement(idx,static_cast<T>(coord));
  }
 }

 
}

template<class T>
std::pair<int,int> VectorV2N4<T>::getTwoGreatestElementCoords()  
//returns the array indices of the two greatest elements
{
 const int ARRAYSIZE = this->getSIZE();
 T refarray[ARRAYSIZE];

 for(int idx = 0; idx < ARRAYSIZE; idx++)
  refarray[idx] = (this->data)[idx];
 
 T max1value, max2value;
 int max1idx, max2idx;

 max1value = refarray[0];
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  if(refarray[idx] >= max1value)
  {
   max1value = refarray[idx];
   max1idx = idx;
  }
 }
 refarray[max1idx] = -99;
 
 max2value = refarray[0];
 
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  if(refarray[idx] >= max2value)
  {
   max2value = refarray[idx];
   max2idx = idx;
  }
 }

 std::pair<int, int> coords;
 coords.first = max1idx;
 coords.second = max2idx;
 
 return coords;
 
}

template<class T>
VectorV2N4<T> VectorV2N4<T>::getPerpendicularDirection() 
//returns a unit vector perpendicular to the current object.  Note that there are are an infinite number of vectors perpendicular to the current object;
//this function just returns an arbitrary one.  Computes the perpendicular vector by solving for a vector whose dot product with the current object
//is zero
{
     
 if((*this).L2norm() == 0)  //tests to see whether the vector is a zero vector.
 {
#if ERRORCHECK
  std::cout<<"ERROR in VectorV2N4<T> VectorV2N4<T>::getPerpendicularDirection()"<<std::endl;
  std::cout<<"Cannot return a perpendicular direction of a zero vector.  Exiting..."<<std::endl;
  exit(1);
#endif

#if VERBOSE
  std::cout<<"WARNING: VectorV2N4<T> VectorV2N4<T>::getPerpendicularDirection(CheckStatus status)"<<std::endl;
  std::cout<<"Cannot take the perpendicular direction of a zero vector.  Returning a zero vector."<<std::endl;
#endif
  return (VectorV2N4<T>(this->getSIZE()));
 
 }

 //creates a normalized copy of the current object to generate the perpendicular vector from 
 VectorV2N4<T> vec(*this);
 vec.normalize();

 const int ARRAYSIZE = this->getSIZE();
 
 //initializing the vector with 1's guarantees that it will not return the zero vector solution
 VectorV2N4<T> pvec(ARRAYSIZE, static_cast<T>(1.0));
 
 //returns the array index of vec of any nonxero element to prevent a divide by zero error when solving for that element in pvec
 int index = 0;
 while((vec.getElement(index)) == T())
  ++index;

 double denom = static_cast<double>(vec.getElement(index));
 
 T num = T();
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  if(idx == index)
   continue;
  num += (vec.getElement(idx)*pvec.getElement(idx)); 
 }

 double element = -(static_cast<double>(num)/static_cast<double>(denom));
 pvec.setElement(index,static_cast<T>(element));

 return pvec;


}

template<class T>
double VectorV2N4<T>::getEuclideanDistanceMetric(const VectorV2N4<T> &destination)  
//returns the generalized Euclidean distance metric between destination and the current point.  NOTE THAT THE CONVENTION IS d(destination - (*this)).
//d is calculated using the standard Euclidean distance formula: d(x,y) = sqrt((x1-y2)^2 + ... + (xn-yn)^2).  T must support conversion to double.
{
 double distance = 0.0;
 double currentdiff = 0.0;

 for(int idx = 0; idx < this->SIZE; idx++)
 {
  currentdiff = static_cast<double>((destination.getElement(idx,0))) - static_cast<double>((this->getElement(idx,0)));
  distance += (currentdiff * currentdiff); 

 }

 distance = sqrtf(distance);

 return distance;

}

template<class T>
VectorV2N4<T> VectorV2N4<T>::getMidPoint(const VectorV2N4<T> &pt)
{
#if ERRORCHECK
 if(!(this->areDimensionsEqual(pt)))
 {
  std::cout<<"ERROR in VectorV2N4<T> VectorV2N4<T>::getMidPoint(const VectorV2N4<T> &pt)"<<std::endl;
  std::cout<<"Could not calculate the midpoint.  Dimensions are not equal."<<std::endl;
  exit(1);
 }
#endif

 VectorV2N4<T> diff;
 diff.makeCompatible(*this);

 diff = (*this) - pt;
 diff.scalarMultiply(0.5);

 VectorV2N4<T> result;
 result.makeCompatible(*this);
 
 result = pt + diff;

 return result;
 

}


template<class T>
std::pair<T,double> VectorV2N4<T>::polynomialInterpolation(double xpt)
{
 T result = T();
 double error = 0.0;

 int i,m,ns = 0;
 double den,dif,dift,ho,hp,w;

 int n = this->getSIZE();
 VectorV2N4<T> c(n);
 VectorV2N4<T> d(n);

 double xa[n];
 for(int idx = 0; idx < n; idx++)
  xa[idx] = static_cast<double>(idx);

 dif = fabsf(xpt - xa[0]);
 for(int i = 0; i < n; i++)
 {
  if((dift = fabsf(xpt - xa[i])) < dif) 
  {
   ns = i;
   dif = dift;
  }
  c.setElement(i,this->getElement(i));
  d.setElement(i,this->getElement(i));
 }

 result = this->getElement(ns--);
 for(int m = 1; m < n; m++)
  for(int i = 0; i < n-m; i++)
  {
   ho=xa[i] - xpt;
   hp = xa[i+m]-xpt;
   w=c.getElement(i+1) - d.getElement(i);
   
#if ERRORCHECK
   if((den=ho-hp) == 0.0f)
   {
    std::cout<<"ERROR in std::pair<T,double> VectorV2N4<T>::polynomialInterpolation(double xpt)"<<std::endl;
    std::cout<<"VectorV2N4<T>::polynomialInterpolation Error"<<std::endl;
    exit(1);
   }
#endif

   
  }
    
  return (std::make_pair<T,double>(T(), 0.0));

}

//PRINT FUNCTIONS
template<class T>
void VectorV2N4<T>::printCL() const
//implementation of pure virutal function from the base Volume class
{

 Matrix2DV2N4<T>::printCL();


}


//OPERATOR OVERLOADS
template<class T>
bool VectorV2N4<T>::operator<(const VectorV2N4<T> &paradigm) const
{
 if(this->L2norm() == paradigm.L2norm())
 {
  int idx = 0;
  while(idx < (this->getSIZE()))
  {
   if(this->getElement(idx) != paradigm.getElement(idx))
    return (this->getElement(idx) < paradigm.getElement(idx));
   idx++;

  }

 }

 return (this->L2norm() < paradigm.L2norm());

}



template<class T>
std::ostream& operator<<(std::ostream& os, const VectorV2N4<T> &output)
//overloaded ostream operator that prints out the data on the CL
{
 
 for(int idx = 0; idx < output.getSIZE(); idx++)
 {
  os << output.getElement(idx) << "  ";
 }
  
 return os;

}



template<class T>
class Vector2V2N4 : public VectorV2N4<T>{

 public:
 //CONSTRUCTORS
 Vector2V2N4();  //default constructor; does initialize the Vector2 to T()
 Vector2V2N4(T xv, T yv);  //initializes the Vector2 to (xv,yv,zv)
 Vector2V2N4(T element); //initializes the 2 values to element.  You can not include a default parameter here because of the conflict with the default constructor
 //Note the important difference between this constructor and the default constructor:
 Vector2V2N4(const Vector2V2N4<T> &copy);  //copy constructor
 Vector2V2N4(const Matrix2DV2N4<T> &copy);  //copy constructor
 Vector2V2N4(const VectorV2N4<T> &copy);  //copy constructor
 void initVector2(T element);  //allows you to initialize vectors created by the default constructor
 
 //GET FUNCTIONS
 const T& x() const {return (this->getElement(0));}  //gets the individual elements
 const T& y() const {return (this->getElement(1));}

 //MATHEMATICAL OPERATIONS
 Vector2V2N4<T> rotateVector2D(double angle);  //function which returns a vector which is the current vector, rotated by angle (must be given in radians)

 //PRINT FUNCTIONS
 virtual void printCL() const {Volume2DV2N4<T>::printCL();}

};

//CONSTRUCTORS
template<class T>
Vector2V2N4<T>::Vector2V2N4()
//default constuctor; does initialize the Vector2 to T()
:VectorV2N4<T>(2,T())
{


}

template<class T>
Vector2V2N4<T>::Vector2V2N4(T xv, T yv)
//initializes the Vector2 to (xv,yv)
:VectorV2N4<T>(2)
{
 this->data[0] = xv;
 this->data[1] = yv;
 

}

template<class T>
Vector2V2N4<T>::Vector2V2N4(T element)
//initializes the 2 values to element.  You can not include a default parameter here because of the conflict with the default constructor
:VectorV2N4<T>(2,element)
{

}

template<class T>
Vector2V2N4<T>::Vector2V2N4(const Vector2V2N4<T> &copy)
//copy constructor
:VectorV2N4<T>(copy)
{

}

template<class T>
Vector2V2N4<T>::Vector2V2N4(const Matrix2DV2N4<T> &copy)
//copy constructor
:VectorV2N4<T>(copy)
{

}

template<class T>
Vector2V2N4<T>::Vector2V2N4(const VectorV2N4<T> &copy)
:VectorV2N4<T>(copy)
{
}


template<class T>
void Vector2V2N4<T>::initVector2(T element)  
//allows you to initialize vectors created by the default constructor
{
 this->initVolume2D(2,1,element);

}


//MATHEMATICAL OPERATIONS
template<class T>
Vector2V2N4<T> Vector2V2N4<T>::rotateVector2D(double angle)  
//function which returns a vector which is the current vector, rotated by angle (must be given in radians)
{

 Matrix2DV2N4<double> rot2dmatrix(2,2);  //this is the standard 2D rotation matrix

 rot2dmatrix.setElement(0,0,cosf(angle));
 rot2dmatrix.setElement(1,1,cosf(angle));
 rot2dmatrix.setElement(0,1,-sinf(angle));
 rot2dmatrix.setElement(1,0,sinf(angle));

 return (rot2dmatrix*(*this));

}


template<class T>
class Vector3V2N4 : public VectorV2N4<T> {

public:
 //CONSTRUCTORS
 Vector3V2N4();  //default constructor; does initialize the Vector3 to T()
 Vector3V2N4(T xv, T yv, T zv);  //initializes the Vector3 to (xv,yv,zv)
 Vector3V2N4(T element); //initializes the 3 values to element.  You can not include a default parameter here because of the conflict with the default constructor
 Vector3V2N4(const T *dataarray);  //initializes the vector with dataarray
 Vector3V2N4(const Matrix2DV2N4<T> &copy);  //copy constructor
 Vector3V2N4(const VectorV2N4<T> &copy);  //copy constructor
 Vector3V2N4(const Vector3V2N4<T> &copy);  //copy constructor
 
 //ASSIGNMENT FUNCTIONS
 inline void initVector3(T element);  //allows you to initialize vectors created by the default constructor
 inline void initVector3(T xv, T yv, T zv);  //allows you to initialize vectors created by the default constructor
 void initVector3(const Vector3V2N4<T> &copy);  //initializes the vector with a copy

 //GET FUNCTION
 const T& x() const {return this->data[0];}  //gets const reference to the individual vector elements
 const T& y() const {return this->data[1];}
 const T& z() const {return this->data[2];}

 //MATHEMATICAL OPERATIONS
 inline Vector3V2N4<T> cross(const Vector3V2N4<T> &op); //returns the cross product of the current object and op
 inline Vector3V2N4<T> rotateAboutAxis(double angle, Vector3V2N4<T> axis);  //function which rotate ref around axis by angle (must be given in radians)
 
 //OVERLOADED OPERATORS
 
 //PRINT FUNCTIONS
 virtual void printCL() const {Volume2DV2N4<T>::printCL();}  //reimplementation of the virtual base class function


};

//CONSTRUCTORS
template<class T>
Vector3V2N4<T>::Vector3V2N4()
//default constuctor; does initialize the Vector3 to T()
:VectorV2N4<T>(3,T())
{


}

template<class T>
Vector3V2N4<T>::Vector3V2N4(T xv, T yv, T zv)
//initializes the Vector3 to (xv,yv,zv)
:VectorV2N4<T>(3)  //calls the Vector constructor
{
 this->data[0] = xv;
 this->data[1] = yv;
 this->data[2] = zv;

}

template<class T>
Vector3V2N4<T>::Vector3V2N4(T element)
//initializes the 3 values to element.  You can not include a default parameter here because of the conflict with the default constructor
:VectorV2N4<T>(3,element)  //calls the Vector constructor
{

}

template<class T>
Vector3V2N4<T>::Vector3V2N4(const T *dataarray)
:VectorV2N4<T>(dataarray,3)  //calls the Vector constructor
{

}

template<class T>
Vector3V2N4<T>::Vector3V2N4(const Vector3V2N4<T> &copy)
//copy constructor
:VectorV2N4<T>(copy)
{

}

template<class T>
Vector3V2N4<T>::Vector3V2N4(const Matrix2DV2N4<T> &copy)
//copy constructor
:VectorV2N4<T>(copy)
{

}

template<class T>
Vector3V2N4<T>::Vector3V2N4(const VectorV2N4<T> &copy)
//copy constructor
:VectorV2N4<T>(copy)
{
}


//ASSIGNMENT FUNCTIONS
template<class T>
void Vector3V2N4<T>::initVector3(T element)  
//allows you to initialize vectors created by the default constructor
{
 this->initVolume2D(3,1,element);

}

template<class T>
void Vector3V2N4<T>::initVector3(T xv, T yv, T zv)  
//allows you to initialize vectors created by the default constructor.  Don't need to call copyVolum2D() which reinitializes the data, since the size of the data will not change.  It is 
//better to initialize the individual data members
{
 (this->data[0]) = xv;
 (this->data[1]) = yv;
 (this->data[2]) = zv;
 
}

template<class T>
void Vector3V2N4<T>::initVector3(const Vector3V2N4<T> &copy)
//copies a vector3 into the current object.  Don't need to call copyVolum2D() which reinitializes the data, since the size of the data will not change.  It is 
//better to initialize the individual data members
{
 this->data[0] = copy.x();
 this->data[1] = copy.y();
 this->data[2] = copy.z();

}



//MATHEMATICAL FUNCTIONS
template<class T>
Vector3V2N4<T> Vector3V2N4<T>::cross(const Vector3V2N4<T> &op) 
//returns the cross product of the current object and op
{

 Vector3V2N4<T> result;

 T x = (this->y())*(op.z());
 x -= (this->z())*(op.y());

 T y = (this->z())*(op.x());
 y -= (this->x())*(op.z());

 T z = (this->x())*(op.y());
 z -= (this->y())*(op.x());

 result.setElement(0,x);
 result.setElement(1,y);
 result.setElement(2,z);

 return result;

}

template<class T>
Vector3V2N4<T> Vector3V2N4<T>::rotateAboutAxis(double angle, Vector3V2N4<T> axis)  
//function which rotate ref around axis by angle (must be given in radians)
{
 axis.normalize();
 T resultx, resulty, resultz;

 T rotxx, rotyy, rotzz, rotxy, rotyx, rotxz, rotzx, rotyz, rotzy;  //elements of the rotation matrix

 //construct the rotation matrix, given a direction, axis, the angle, and the reference vector, ref
 rotxx = (axis.x()*axis.x()) + (cosf(angle)*(1-axis.x()*axis.x()));
 rotyy = (axis.y()*axis.y()) + (cosf(angle)*(1-axis.y()*axis.y()));
 rotzz = (axis.z()*axis.z()) + (cosf(angle)*(1-axis.z()*axis.z()));
 rotxy = (axis.x()*axis.y()) + (cosf(angle)*(-axis.x()*axis.y())) + (sinf(angle)*(-axis.z()));
 rotyx = (axis.y()*axis.x()) + (cosf(angle)*(-axis.y()*axis.x())) + (sinf(angle)*(axis.z()));
 rotxz = (axis.x()*axis.z()) + (cosf(angle)*(-axis.x()*axis.z())) + (sinf(angle)*(axis.y()));
 rotzx = (axis.z()*axis.x()) + (cosf(angle)*(-axis.z()*axis.x())) + (sinf(angle)*(-axis.y()));
 rotyz = (axis.y()*axis.z()) + (cosf(angle)*(-axis.y()*axis.z())) + (sinf(angle)*(-axis.x()));
 rotzy = (axis.z()*axis.y()) + (cosf(angle)*(-axis.z()*axis.y())) + (sinf(angle)*(axis.x()));
 
 resultx = this->x()*rotxx + this->y()*rotxy + this->z()*rotxz;
 resulty = this->x()*rotyx + this->y()*rotyy + this->z()*rotyz;
 resultz = this->x()*rotzx + this->y()*rotzy + this->z()*rotzz;

 Vector3V2N4<T> result(resultx, resulty, resultz);
 

 return result;
 
 
}


template<class T>
class ImageSliceV2N4 : public Matrix2DV2N4<T>{
public:
 //CONSTRUCTORS
 ImageSliceV2N4();  //default constructor
 ImageSliceV2N4(int RDIMv, int CDIMv, T element = T(), double RLENGTHv = 1.0, double CLENGTHv = 1.0);  //initializes array and sets the data members to element 
 //(default is T()). Also sets the voxel lengths (in mm) (default 1.0mm)
 ImageSliceV2N4(const ImageSliceV2N4 &copy);  //copy constructor
 
 //PRINT FUNCTIONS
 virtual void printCL() const;  //reimplementation of the virtual base class function that prints out the image slice on the CL


protected:
 double RLENGTH;  //the voxel lengths
 double CLENGTH;


};

//CONSTRUCTORS
template<class T>
ImageSliceV2N4<T>::ImageSliceV2N4()
//default constructor
:Matrix2DV2N4<T>(), RLENGTH(0.0), CLENGTH(0.0)
{


}

template<class T>
ImageSliceV2N4<T>::ImageSliceV2N4(int RDIMv, int CDIMv, T element, double RLENGTHv, double CLENGTHv)  
//initializes array and sets the data members to element (default is T()).  Also sets the voxel lengths (in mm) (default 1.0 mm)
:Matrix2DV2N4<T>(RDIMv,CDIMv,element), RLENGTH(RLENGTHv), CLENGTH(CLENGTHv)
{
 
 
}

template<class T>
ImageSliceV2N4<T>::ImageSliceV2N4(const ImageSliceV2N4<T> &copy)
//copy constructor
:Matrix2DV2N4<T>(copy)
{

}

template<class T>
void ImageSliceV2N4<T>::printCL() const
//reimplementation of the virtual base class function that prints out the image slice on the CL
{
 Matrix2DV2N4<T>::printCL();

}


template<class T>
class NoiseV1N2{

public:
 //CONSTRUCTORS
 NoiseV1N2();  //default empty constructor


};


template<class T>
NoiseV1N2<T>::NoiseV1N2()
//default empty constructor
{


}


template<class T>
class GaussianNoiseV1N2 : public NoiseV1N2<T>{

public:
 //CONSTRUCTORS
 GaussianNoiseV1N2();  //default empty constructor
 GaussianNoiseV1N2(int seed);  //initialize the generator with seed = time(NULL)

 //GENERATORS
 T generateGaussianRandomNumber(double mean, double stddev);  //Normal distribution with mean and standard deviation stddev. This distribution simulates the sum of many random factors. 
 //Definition: f(x) = (2*pi*s2)-0.5*exp(-0.5*s-2*(x-m)2).  The generator returns a double, which is internally converted to type T.

protected:
 StochasticLib1 *stolib;  //random number generator



};


template<class T>
GaussianNoiseV1N2<T>::GaussianNoiseV1N2()
//default empty constructor
:NoiseV1N2<T>()
{

 stolib = NULL;


}

template<class T>
GaussianNoiseV1N2<T>::GaussianNoiseV1N2(int seed)
//initializes the generator with seed
:NoiseV1N2<T>()
{
 
 stolib = new StochasticLib1(seed);


}


template<class T>
T GaussianNoiseV1N2<T>::generateGaussianRandomNumber(double mean, double stddev)
//Normal distribution with mean and standard deviation stddev. This distribution simulates the sum of many random factors. Definition: f(x) = (2*pi*s2)-0.5*exp(-0.5*s-2*(x-m)2).  
//The generator returns a double, which is internally converted to type T.
{
 double result = stolib->Normal(mean,stddev);
 return (static_cast<T>(result));

}


template<class T>
class FileHeaderV1N2{

public:
 //CONSTRUCTORS
 FileHeaderV1N2();  //empty default constructor
 
 //PRINT FUNCTIONS
 virtual void printHeader() const = 0;


protected:

};

template<class T>
FileHeaderV1N2<T>::FileHeaderV1N2()
{

 
}


template<class T>
class ImageVolumeHeaderV1N2 : public FileHeaderV1N2<T>{

public:
 //CONSTRUCTORS
 ImageVolumeHeaderV1N2();  //default empty constructor
 ImageVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, DataType dtv, int bitspervoxelv, bool shouldbyteswapv, int RDIMv, int CDIMv, int ZDIMv, int offsetv = 0, double RLv = 1.0, double CLv = 1.0, double ZLv = 1.0);  //default empty constructor
 ImageVolumeHeaderV1N2(const ImageVolumeHeaderV1N2<T> &copy); //copy constructor
 ~ImageVolumeHeaderV1N2();  

 //PRINT FUNCTIONS
 virtual void printHeader() const;  //prints the header information, one value per line

 //READ FUNCTIONS
 virtual void readInHeaderFile(const FileNameV1N1<> *fnamev) = 0; //read in the header file, fnamev

 //WRITE FUNCTIONS
 virtual void writeOutHeaderFile(const FileNameV1N1<> *fnamev) const = 0;  //writes out the header file 

 //GET FUNCTIONS
 std::string getFName() const {return ((this->fname)->getOSCompatibleFileName());}
 const FileNameV1N1<>* getFNamePtr() const {return fname;}
 FileNameV1N1<>* getFNamePtrNonConstReference() {return fname;}
 
 ImageFileFormat getImageFileFormat() const {return imgformat;}
 DataType getDataType() const {return datatype;}
 int getRDIM() const {return RDIM;}  //sets the volume dimensions
 int getCDIM() const {return CDIM;}
 int getZDIM() const {return ZDIM;}
 double getRLENGTH() const {return RLENGTH;}  //sets the voxel lengths
 double getCLENGTH() const {return CLENGTH;}
 double getZLENGTH() const {return ZLENGTH;}
 int getBitsPerVoxel() const {return bitspervoxel;}
 bool getShouldByteSwap() const {return shouldbyteswap;}
 int getOffset() const {return offset;}

 //SET FUNCTIONS
 void setFName(const FileNameV1N1<> *fnamev); 
 
 void setRDIM(int RD) {RDIM = RD;}  //sets the volume dimensions
 void setCDIM(int CD) {CDIM = CD;}
 void setZDIM(int ZD) {ZDIM = ZD;}
 void setRLENGTH(double RL) {RLENGTH = RL;}  //sets the voxel lengths
 void setCLENGTH(double CL) {CLENGTH = CL;}
 void setZLENGTH(double ZL) {ZLENGTH = ZL;}
 void setBitsPerVoxel(int bpv) {bitspervoxel = bpv;}
 void setDataType(DataType dt) {datatype = dt;}
 void setByteSwap(bool bswap) {shouldbyteswap = bswap;}
 void setOffset(int ofset) {offset = ofset;}

protected:
 FileNameV1N1<> *fname;  //filename of the header file
  
 ImageFileFormat imgformat;
 int bitspervoxel;
 DataType datatype;
 int RDIM;  //the volume/slice dimensions
 int CDIM;
 int ZDIM;
 double RLENGTH;  //the voxels lengths in mm (default to 1 mm)
 double CLENGTH;
 double ZLENGTH;
 bool shouldbyteswap;  //whether or not the data is byteswapped
 int offset;  //the offset in the header file to when the data begins.  if separate header file, offset should be 0 (default = 0)

};

//CONSTRUCTORS
template<class T>
ImageVolumeHeaderV1N2<T>::ImageVolumeHeaderV1N2()
//empty default constructor
:FileHeaderV1N2<T>(), shouldbyteswap(false), RDIM(0), CDIM(0), ZDIM(0), RLENGTH(1.0), CLENGTH(1.0), ZLENGTH(1.0), bitspervoxel(0), datatype(UNKNOWNTYPE), offset(0)
{

 fname = NULL;
  
}

template<class T>
ImageVolumeHeaderV1N2<T>::ImageVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, DataType dtv, int bitspervoxelv, bool shouldbyteswapv, int RDIMv, int CDIMv, int ZDIMv, int offsetv, double RLv, double CLv, double ZLv)  
//constructor that assigns all arguments
:FileHeaderV1N2<T>(), imgformat(ifv), datatype(dtv), RDIM(RDIMv), CDIM(CDIMv), ZDIM(ZDIMv), RLENGTH(RLv), CLENGTH(CLv), ZLENGTH(ZLv), bitspervoxel(bitspervoxelv), shouldbyteswap(shouldbyteswapv), offset(offsetv)
{

 (this->fname) = NULL;
 (this->fname) = (fnamev->clone());
 
}


template<class T>
ImageVolumeHeaderV1N2<T>::ImageVolumeHeaderV1N2(const ImageVolumeHeaderV1N2<T> &copy) 
//copy constructor
:FileHeaderV1N2<T>()
{

 (this->fname) = NULL;
 fname = (copy.getFNamePtr())->clone();

 imgformat = copy.getImageFileFormat();
 datatype = copy.getDataType();

 RDIM = copy.getRDIM();  //copies the dimensions
 CDIM = copy.getCDIM();
 ZDIM = copy.getZDIM();

 RLENGTH = copy.getRLENGTH();  //copies the voxel lengths
 CLENGTH = copy.getCLENGTH();
 ZLENGTH = copy.getZLENGTH();

 this->bitspervoxel = copy.getBitsPerVoxel();
 this->shouldbyteswap = copy.getShouldByteSwap();
 this->offset = copy.getOffset();

}

template<class T>
ImageVolumeHeaderV1N2<T>::~ImageVolumeHeaderV1N2()
{
 //deletes the fname is initialized dynamically
 if((this->fname) != NULL)
 {
  delete (this->fname);
  (this->fname) = NULL;
 }
}

template<class T>
void ImageVolumeHeaderV1N2<T>::printHeader() const  
//prints the header information, one value per line
{

 std::cout<<"Filename: "<<fname->getOSCompatibleFileName()<<std::endl;
 
 std::cout<<"Image Format: ";
 
 switch(imgformat){
  case DICOM:
   std::cout<<"DICOM"<<std::endl;
   break;
  case ANALYZE:
   std::cout<<"ANALYZE"<<std::endl;
   break;
  case NIFTI:
   std::cout<<"NIFTI"<<std::endl;
   break;
  case RAWTEXT:
   std::cout<<"TEXT"<<std::endl;
   break;
  case MINC:
   std::cout<<"MINC"<<std::endl;
   break;
  case NRRD:
   std::cout<<"NRRD"<<std::endl;
   break;
  default:
   std::cout<<"Unknown Image Format"<<std::endl;
   break;
 }
 
 if(shouldbyteswap)
  std::cout<<"File is byte swapped"<<std::endl;
 else 
  std::cout<<"File is not byte swapped"<<std::endl;

 std::cout<<"Datatype: "<<hageman::convertDataTypeToString(this->datatype)<<std::endl;
 std::cout<<"Bytes Per Voxel: "<<bitspervoxel/8<<std::endl;

 std::cout<<"RDIM: "<<this->getRDIM()<<std::endl;
 std::cout<<"CDIM: "<<this->getCDIM()<<std::endl;
 std::cout<<"ZDIM: "<<this->getZDIM()<<std::endl;

 std::cout<<"RLENGTH: "<<this->getRLENGTH()<<std::endl;
 std::cout<<"CLENGTH: "<<this->getCLENGTH()<<std::endl;
 std::cout<<"ZLENGTH: "<<this->getZLENGTH()<<std::endl;

 std::cout<<"Offset: "<<this->offset<<std::endl;



}

//GET FUNCTIONS

template<class T>
void ImageVolumeHeaderV1N2<T>::setFName(const FileNameV1N1<> *fnamev) 
{
 //destroys the filename if already initialized
 if(fname != NULL)
 {
  delete (this->fname);
  (this->fname) = NULL;
 }

 fname = fnamev->clone();
 
 

}



//absolute key file positions within the header file (in bytes) [unique to Analyze header format]
#define XDIM_FILEPOS 42
#define DATATYPE_FILEPOS 70
#define XLENGTH_FILEPOS 80
#define GLMAX_FILEPOS 140
#define EXTENTS_FILEPOS 32
#define REGULAR_FILEPOS 38
#define DIMS_FILEPOS 40
#define VOXOFFSET_FILEPOS 108


template<class T>  //should be the datatype of the image volume, but this is not a requirement
class AnalyzeVolumeHeaderV1N2 : public ImageVolumeHeaderV1N2<T>{

public:
 //CONSTRUCTORS
 AnalyzeVolumeHeaderV1N2();  //default empty constructor
 AnalyzeVolumeHeaderV1N2(const FileNameV1N1<> *fnamev);  //constructs an object from a .hdr file
 AnalyzeVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, DataType dtv, int bitspervoxel, bool shouldbyteswap, int RDIMv, int CDIMv, int ZDIMv, double RLv = 1.0, double CLv = 1.0, double ZLv = 1.0);  //default empty constructor
 AnalyzeVolumeHeaderV1N2(const AnalyzeVolumeHeaderV1N2<T> &copy); //copy constructor
 
 //PRINT FUNCTIONS
 virtual void printHeader() const;  //prints the header information, one value per line
 
 //READ FUNCTIONS
 virtual void readInHeaderFile(const FileNameV1N1<> *fnamev); //read in the header file, fnamev

 //WRITE FUNCTIONS
 virtual void writeOutHeaderFile(const FileNameV1N1<> *fnamev) const;  //writes out the header file in Analyze image format

protected:
 

};

//CONSTRUCTORS
template<class T>
AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2()
//empty default constructor
:ImageVolumeHeaderV1N2<T>()
{
 this->imgformat = hageman::ANALYZE;
 this->offset = 348;


}

template<class T>
AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, DataType dtv, int bitspervoxelv, bool shouldbyteswapv,int RDIMv, int CDIMv, int ZDIMv, double RLv, double CLv, double ZLv)  
//constructor that assigns all arguments
:ImageVolumeHeaderV1N2<T>(fnamev,hageman::ANALYZE,dtv,bitspervoxelv,shouldbyteswapv,RDIMv,CDIMv,ZDIMv,348,RLv,CLv,ZLv)
{

 //reassigns the extension of fnamev (which will be .hdr) to .img
 (this->getFNamePtrNonConstReference())->setExtension(".img");
 
 
}

template<class T>
AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(const FileNameV1N1<> *fnamev)  
//constructor that assigns all arguments based on an Analyze header file
:ImageVolumeHeaderV1N2<T>()
{
 this->imgformat = hageman::ANALYZE;
 this->offset = 348;
 this->readInHeaderFile(fnamev);

}

template<class T>
AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(const AnalyzeVolumeHeaderV1N2<T> &copy) 
//copy constructor
:ImageVolumeHeaderV1N2<T>(copy)
{
 
}
 
//PRINT FUNCTIONS
template<class T>
void AnalyzeVolumeHeaderV1N2<T>::printHeader() const
//print header function
{
 ImageVolumeHeaderV1N2<T>::printHeader();
 
}

//READ FUNCTIONS
template<class T>
void AnalyzeVolumeHeaderV1N2<T>::readInHeaderFile(const FileNameV1N1<> *fnamev)  
//reads in an Analyze header file, fname.  the extension of fnamev will be 
{

#if VALIDATE
  std::cout<<"From function void AnalyzeVolumeHeaderV1N2<T>::readInHeaderFile(FileNameV1N1<> *fnamev)"<<std::endl;  
  std::cout<<"Filename: "<<fnamev->getOSCompatibleFileName()<<std::endl;
#endif

 //deletes the existing fname, if any
 if((this->fname) != NULL)
 {
  delete (this->fname);
  (this->fname) = NULL;
 }

 //assigns the fname based on fnamev
 (this->fname) = (fnamev->clone());
 //if the input filename doesn't have a .img extension, assigns .img to the extension
 if(((this->fname)->getExtension()) != ".img")
  (this->fname)->setExtension(".img");

 FileNameV1N1<> *hdrfptr = new FileNameV1N1<>(fnamev);
 //if the hdrfptr filename doesn't have a .hdr extension, assigns .hdr to the extension
 if((hdrfptr->getExtension()) != ".hdr")
  hdrfptr->setExtension(".hdr");

 ReadFromBinaryFileV1N1<> hdrifile(hdrfptr);
 hdrifile.openForRead();

 char twobytebuffer[2];
 char fourbytebuffer[4];
 int *intptr;
 signed short int *shortptr;
 float *floatptr;

 //the first 4 bytes specify the size of the ANALYZE file header (must be 348) and is used to determine if the file is byte swapped
 hdrifile.readFromFileStream(fourbytebuffer,4);
 intptr = reinterpret_cast<int*>(fourbytebuffer);  //size of the header is stored as an int (int32)
 char swapbuffer[4];
 for(int idx = 0; idx < 4; idx++)
  swapbuffer[idx] = fourbytebuffer[idx];

 byteswap<char>(swapbuffer,4);
 int *swapintptr = reinterpret_cast<int*>(swapbuffer);
 
 if(*intptr == static_cast<int>(348))
  this->shouldbyteswap = false;
 else if(*swapintptr == static_cast<int>(348))
  this->shouldbyteswap = true;
#if ERRORCHECK
 else
 {
  std::cout<<"ERROR in AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
  std::cout<<"This is not a valid ANALYZE header file!  1st 4 bytes not equal to 348.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 //the dimensions of the volume
 hdrifile.placeGetStreamAtAbsolutePosition(XDIM_FILEPOS);

 //XDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setRDIM(*shortptr);  
  
 //YDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setCDIM(*shortptr);  
 
 //ZDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setZDIM(*shortptr);
 
 //datatype
 hdrifile.placeGetStreamAtAbsolutePosition(DATATYPE_FILEPOS);

 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<short int*>(twobytebuffer);

#if VALIDATE
 std::cout<<"Anlz specific Datatype No. of Anlz hdr read from file: "<<*shortptr<<std::endl;
#endif
 
 switch(*shortptr)
 {
  case 0:
   this->datatype = UNKNOWNTYPE;
   break;
  case 1:  //Analyze format specifies 1 as a binary format
   this->datatype = UINT8TYPE;
   break;
  case 2:  //Analyze format specifies 2 as unsigned char, so i made that the default (avoid using char)
   if(hageman::getDataType(T()) == SINT8TYPE)
    this->datatype = SINT8TYPE;
   else
    this->datatype = UINT8TYPE;
   break;
  case 4:  //Analyze format specifies 4 as a signed short format, so i made that the default
   if(hageman::getDataType(T()) == UINT16TYPE)
    this->datatype = UINT16TYPE;
   else
    this->datatype = SINT16TYPE;
   break;
  case 8:
   this->datatype = SINT32TYPE;
   break;
  case 16:
   this->datatype = FLOATTYPE;
   break;
  case 64:
   this->datatype = DOUBLETYPE;
   break;
  case 128:
   this->datatype = RGBTYPE;
   break;
#if ERRORCHECK
  default:
  {
   std::cout<<"ERROR in AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
   std::cout<<"Not a valid datatype.  Exiting..."<<std::endl;
   std::cout<<"Datatype: "<<this->datatype<<std::endl;
   exit(1);
   break;
  }
#endif
 }

 

 if(hageman::getDataType(T()) != this->datatype)  //can't make this a requirement because static functions need to be called when the datatype is unknown
 {

#if VERBOSE
  std::cout<<"WARNING: Reading fname = "<<fnamev->getOSCompatibleFileName()<<std::endl;
  std::cout<<"Datatype of hdr file = "<<hageman::convertDataTypeToString(this->datatype)<<" does not match datatype specified = "<<hageman::convertDataTypeToString(hageman::getDataType(T()))<<std::endl;
  std::cout<<"AnalyzeVolumeHeaderV1N2<T>::AnalyzeVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
#endif

 }
 

 //bitspervoxel
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(twobytebuffer,2);
 shortptr = reinterpret_cast<short int*>(twobytebuffer);
 this->setBitsPerVoxel(static_cast<int>(*shortptr));

 //addendum to adjust the datatype if it is UNKNOWN; goes by the template argument and the bitspervoxel
 if(this->datatype == UNKNOWNTYPE)
 {
  if(this->bitspervoxel == 32)
  {
   if(hageman::getDataType(T()) == SINT32TYPE)
    this->datatype = SINT32TYPE;
   else
    this->datatype = UINT32TYPE;
  }
  else if(this->bitspervoxel == 16)
  {
   if(hageman::getDataType(T()) == SINT16TYPE)
    this->datatype = SINT16TYPE;
   else
    this->datatype = UINT16TYPE;
  }
  else if(this->bitspervoxel == 8)
  {
   if(hageman::getDataType(T()) == SINT8TYPE)
    this->datatype = SINT8TYPE;
   else if(hageman::getDataType(T()) == UINT8TYPE)
    this->datatype = UINT8TYPE; 
   else
    this->datatype = CHARTYPE;
  }
 }
  
 
 //hdrifile.seekg(XLENGTH_FILEPOS);
 hdrifile.placeGetStreamAtAbsolutePosition(XLENGTH_FILEPOS);


 //XLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setRLENGTH(static_cast<double>(*floatptr));  //the XLENGTH is equal to the CLENGTH in matrix coordinates
 
 //YLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setCLENGTH(static_cast<double>(*floatptr));  //the YLENGTH is equal to the RLENGTH in matrix coordinates
 
 //ZLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setZLENGTH(static_cast<double>(*floatptr));
 
 
 hdrifile.close();

 delete hdrfptr;
 hdrfptr = NULL;
 
 
}


//WRITE FUNCTIONS
template<class T>
void AnalyzeVolumeHeaderV1N2<T>::writeOutHeaderFile(const FileNameV1N1<> *fnamev) const
//writes out the header file in Analyze image format
{
#if VERBOSE
 std::cout<<"Writing Out Analyze Image Header File = "<<fnamev->getOSCompatibleFileName()<<std::endl;
#endif

 WriteToBinaryFileV1N1<> hdrofile(fnamev);
 hdrofile.openForWrite();
  
 char *writebuffer = NULL;
 int inttag = 0;
 int *intptr = NULL;
 char chartag;
 short shorttag = 0;
 short *shortptr = NULL;
 float floattag = 0.0f;
 float *floatptr = NULL;

 //the first 4 bytes specify the size of the ANALYZE file header (must be 348) and is used to determine if the file is byte swapped
 inttag = 348;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 hdrofile.placePutStreamAtAbsolutePosition(EXTENTS_FILEPOS);

 inttag = 16384;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 hdrofile.placePutStreamAtAbsolutePosition(REGULAR_FILEPOS);
 chartag = 'r';
 writebuffer = &chartag;
 hdrofile.writeBytesToFileStream(writebuffer,1);
 

 hdrofile.placePutStreamAtAbsolutePosition(DIMS_FILEPOS);
 
 //DIMS
 shorttag = 4;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);

 //XDIM
 shorttag = static_cast<short>(this->getRDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //YDIM
 shorttag = static_cast<short>(this->getCDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(ImageVolumeHeaderV1N2<T>::getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //ZDIM
 shorttag = static_cast<short>(this->getZDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //TIME
 shorttag = 1;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 hdrofile.placePutStreamAtAbsolutePosition(DATATYPE_FILEPOS);
 
 short int dtypetag = 0;
 
 switch(this->getDataType())
 {
  case UNKNOWNTYPE:
   dtypetag = 0;
   break;
  case CHARTYPE:
  case UINT8TYPE:
  case SINT8TYPE:
   dtypetag = 2;
   break;
  case SINT16TYPE:
  case UINT16TYPE:
   dtypetag = 4;
   break;
  case SINT32TYPE:
  case UINT32TYPE:
   dtypetag = 8;
   break;
  case FLOATTYPE:
   dtypetag = 16;
   break;
  case EIGENTYPE:
   dtypetag = 21;
   break;
  case DOUBLETYPE:
   dtypetag = 64;
   break;
  case RGBTYPE:
   dtypetag = 128;
   break;
#if ERRORCHECK
  default:
    std::cout<<"ERROR in void AnalyzeVolumeHeaderV1N2<T>::writeOutHeaderFile() const"<<std::endl;
    std::cout<<"Not a valid datatype.  Exiting..."<<std::endl;
    std::cout<<"Datatype: "<<this->getDataType()<<std::endl;
    exit(1);
    break;
#endif
    
  }

 
 //DATATYPE
 shortptr = &dtypetag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);


 //BITS PER VOXEL
 shorttag = static_cast<short>(ImageVolumeHeaderV1N2<T>::getBitsPerVoxel());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 hdrofile.placePutStreamAtAbsolutePosition(XLENGTH_FILEPOS);
 
 //XLENGTH
 floattag = (static_cast<float>(this->getRLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //YLENGTH
 floattag = (static_cast<float>(this->getCLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //ZLENGTH
 floattag = (static_cast<float>(this->getZLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 hdrofile.placePutStreamAtAbsolutePosition(VOXOFFSET_FILEPOS);
 
 //VOX OFFSET
 floattag = 0.0f;
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 hdrofile.placePutStreamAtAbsolutePosition(344);

 //SMIN
 inttag = 0;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 hdrofile.close();


}



template<class T>
class ImageVolumeV2N4 : public Matrix3DV2N4<T>{

public:
 using Volume3DV2N4<T>::readInFromTxtFile;

 //CONSTRUCTORS
 ImageVolumeV2N4();  //default constructor
 ImageVolumeV2N4(int RDIMv, int CDIMv, int ZDIMv, T element = T(), double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);  
 //initializes array and sets the data members to element (default is T()). Also sets the voxel lengths (in mm) (default 1.0mm)
 ImageVolumeV2N4(const ImageVolumeV2N4 &copy);  //copy constructor
 ImageVolumeV2N4(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLEGNTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);  //a function of for reading in 
 //a 3D volume from a txt file.  This function DOES create space for vol.
 //ImageVolumeV2N4(const AnalyzeVolumeHeaderV1N2<T> &hdr); //constructs an image volume by reading in the img file given by hdr
 //ImageVolumeV2N4(const Volume3DV2N4<T> &copy);
 
 template<class S>
 void makeCompatible(const ImageVolumeV2N4<S> &paradigm);  //reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all values to element.  If dimensions are already the same, does nothing.

 //ASSIGNMENT FUNCTIONS
 void initImageVolume(int RDIMv, int CDIMv, int ZDIMv, T element = T(), double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);  //initializes array and sets the data members to element (default is T()). Also sets the voxel lengths (in mm) (default 1.0mm)

 //DESTRUCTORS
 ~ImageVolumeV2N4();

 //GET FUNCTIONS
 const double getRLENGTH() const {return RLENGTH;}
 const double getCLENGTH() const {return CLENGTH;}
 const double getZLENGTH() const {return ZLENGTH;}
 inline double getSmallestVoxelLength() const;

 //SET FUNCTIONS
 void setRLENGTH(double rL) {RLENGTH = rL;}  //sets the voxel lengths of the ImageVolume
 void setCLENGTH(double cL) {CLENGTH = cL;}
 void setZLENGTH(double zL) {ZLENGTH = zL;}
 

 //READ FUNCTIONS
 void readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv, double CLENGTHv, double ZLENGTHv, AllocateMem amem = hageman::NOALLOCATE);
 void readInImageVolumeFromFile(const ImageVolumeHeaderV1N2<T> *imhdrptr);
 void readInAnalyzeVolumeFromFile(const ImageVolumeHeaderV1N2<T> *imghdrptr);
 
 //void readInAnalyzeVolumeFromFile(const FileNameV1N1<> *anlzfname);
 //void readInCombinedNiftiVolumeFromFile(const NiftiVolumeHeaderV1N1<T> &niftihdr);
 //void readInCombinedNiftiVolumeFromFile(const FileNameV1N1<> *niftifname);

 //template<class S>
 //void readInConvertedAnalyzeVolumeFromFile(FileNameV1N1<> *ofname, S scalefactor);  //creates an image voluem from an Analyze file, converting each element to the datatype T
 //S is the datatype of the voxels specified by the Analyze header; this datatype will be what is read from the file.  The scalefactor will normalize the volume to 
 //between [0,scalefactor] - set to zero if you do not wish any scaling
 
 //template<class S>
 //void readInConvertedAnalyzeVolumeFromFile(const AnalyzeVolumeHeaderV1N2<S> &hdr, S scalefactor);  //creates an image voluem from an Analyze file, converting each element to the datatype T
 //S is the datatype of the voxels specified by the Analyze header; this datatype will be what is read from the file.  An scalefactor will normalize the volume to 
 //between [0,scalefactor] - set to zero if you do not wish any scaling
  
 //WRITE FUNCTIONS
 void writeOutImageVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<T> *imghdrptr);
 void writeOutAnalyzeVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<T> *imghdrptr);

 template<class S>
 void writeOutConvertedImageVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<S> *imghdrptr);

 template<class S>
 void writeOutConvertedAnalyzeVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<S> *imghdrptr);
 
 //void writeOutAnalyzeVolumeToFile(FileNameV1N1<> *ofname, bool shouldbyteswapv);
 
 //template<class S>
 //void writeOutConvertedAnalyzeVolumeToFile(const AnalyzeVolumeHeaderV1N2<S> &hdr, S scalefactor);  //writes out the volume as an Analyze img but first converts each element to dt
 
 //template<class S>
 //void writeOutConvertedAnalyzeVolumeToFile(FileNameV1N1<> *ofname, bool shouldbyteswapv, S scalefactor);  //writes out the volume as an Analyze img but first converts each element to dt
  
 //CHECK FUNCTIONS


 //PRINT FUNCTIONS
 void printVolInfo() const;  //prints out the volume information (dimensions, lengths)
 virtual void printCL() const;  //implementation of virtual function from base Volume class

 //MATH FUNCTIONS
 void addGaussianNoise(double stddev);  //adds Gaussian noise to the image around a mean of zero and with a standard deviation of stddev.  Corrects any voxels which fall below zero by
 //setting them back to zero 
 

 //CONVERSION FUNCTIONS
 //converts the voxel coords into real world coords.  returns the center of the voxel in real world coords
 inline double convertRVoxelToRReal(int rvoxel) const;
 inline double convertCVoxelToCReal(int cvoxel) const;
 inline double convertZVoxelToZReal(int zvoxel) const;
 
 //converts real world coords anywhere within a voxel to the voxel coords
 inline int convertRRealToRVoxel(double rcoord) const;
 inline int convertCRealToCVoxel(double ccoord) const;
 inline int convertZRealToZVoxel(double zcoord) const;


 //UTILITY FUNCTIONS USED BY OTHER MEMBER FUNCTIONS


 //INTERPOLATION UTILITY DATA
 //these arrays serve as anchor points for interpolation
 double *x1a; 
 double *x2a; 
 double *x3a; 
 
protected:
 double RLENGTH;  //voxel lengths
 double CLENGTH;
 double ZLENGTH;

 

private:
 

};

//CONSTRUCTORS
template<class T>
ImageVolumeV2N4<T>::ImageVolumeV2N4()
//default constructor
:Matrix3DV2N4<T>(), RLENGTH(0.0), CLENGTH(0.0), ZLENGTH(0.0)
{
 x1a = NULL; 
 x2a = NULL; 
 x3a = NULL; 

}

template<class T>
ImageVolumeV2N4<T>::ImageVolumeV2N4(int RDIMv, int CDIMv, int ZDIMv, T element, double RLENGTHv, double CLENGTHv, double ZLENGTHv)  
//initializes array and sets the data members to element (default is T()).  Also sets the voxel lengths (in mm) (default 1.0 mm)
:Matrix3DV2N4<T>(RDIMv,CDIMv,ZDIMv,element)
{
 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

 x1a = NULL; 
 x2a = NULL; 
 x3a = NULL; 

 
}

template<class T>
ImageVolumeV2N4<T>::ImageVolumeV2N4(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv, double CLENGTHv, double ZLENGTHv)
//a function of for reading in a 3D volume from a txt file.  This function DOES create space for vol.  Function assumes ifname has one value per line and assumes the following read order:  
//the reading starts with zidx = 0, and reads all the
//columns for the first row, then goes to the second row and reads all the column for the second row and so on...  When it is finished with all rows and
//columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...
:Matrix3DV2N4<T>(RDIMv,CDIMv,ZDIMv,T())
{

 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

 x1a = NULL; 
 x2a = NULL; 
 x3a = NULL; 

 this->readInFromTxtFile(ifname, RDIMv, CDIMv, ZDIMv, RLENGTH, CLENGTHv, ZLENGTHv, hageman::NOALLOCATE);

 
}


//template<class T>
//ImageVolumeV2N4<T>::ImageVolumeV2N4(const AnalyzeVolumeHeaderV1N2<T> &hdr)
//constructs an image volume by reading in the img file given by hdr
//:Matrix3DV2N4<T>(), RLENGTH(0.0), CLENGTH(0.0), ZLENGTH(0.0)
//{
// x1a = NULL; 
// x2a = NULL; 
// x3a = NULL;

// this->readInAnalyzeVolumeFromFile(hdr);


//}
 


template<class T>
ImageVolumeV2N4<T>::ImageVolumeV2N4(const ImageVolumeV2N4<T> &copy)
//copy constructor
:Matrix3DV2N4<T>(copy)
{
 x1a = NULL; 
 x2a = NULL; 
 x3a = NULL; 

}


template<class T>
void ImageVolumeV2N4<T>::initImageVolume(int RDIMv, int CDIMv, int ZDIMv, T element, double RLENGTHv, double CLENGTHv, double ZLENGTHv)  
//initializes array and sets the data members to element (default is T()). Also sets the voxel lengths (in mm) (default 1.0mm)
{
 this->initVolume3D(RDIMv,CDIMv,ZDIMv,element);

 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

 x1a = NULL; 
 x2a = NULL; 
 x3a = NULL; 

}

template<class T>
ImageVolumeV2N4<T>::~ImageVolumeV2N4()
{

 //deletes any existing interpolation volumes
 if(x1a != NULL)
 {
  delete[] x1a;
  x1a = NULL;
 }

 if(x2a != NULL)
 {
  delete[] x2a;
  x2a = NULL;
 }

 if(x3a != NULL)
 {
  delete[] x3a;
  x3a = NULL;
 }

}

template<class T>
template<class S>
void ImageVolumeV2N4<T>::makeCompatible(const ImageVolumeV2N4<S> &paradigm)
//reinitializes and makes the dimensions compatible with paradigm without copying any values.  Sets all values to element.  If dimensions are already the same, does nothing.
{
 T element = T();
 
 this->initImageVolume(paradigm.getRDIM(), paradigm.getCDIM(), paradigm.getZDIM(), element,paradigm.getRLENGTH(), paradigm.getCLENGTH(), paradigm.getZLENGTH());


}



//GET FUNCTIONS
template<class T>
double ImageVolumeV2N4<T>::getSmallestVoxelLength() const
{
 double result = this->getRLENGTH();

 if(result > (this->getCLENGTH()))
  result = this->getCLENGTH();
 if(result > (this->getZLENGTH()))
  result = this->getZLENGTH();

 return result;



}

//READ FUNCTIONS
template<class T>
void ImageVolumeV2N4<T>::readInFromTxtFile(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv, double CLENGTHv, double ZLENGTHv, AllocateMem amem)
//a function of for reading in a 3D volume from a txt file.  Calls the same function in the parent class, Volume3D, and then further assigns the voxel lengths
{

 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

 this->readInFromTxtFile(ifname, RDIMv, CDIMv, ZDIMv, amem);  //read function from Volume3D class

}

template<class T>
void ImageVolumeV2N4<T>::readInImageVolumeFromFile(const ImageVolumeHeaderV1N2<T> *imghdrptr)
{

 this->initVolume3D(imghdrptr->getRDIM(), imghdrptr->getCDIM(), imghdrptr->getZDIM());  //initializes the object with the dimensions from the analyze header
 
 this->setRLENGTH(imghdrptr->getRLENGTH()); 
 this->setCLENGTH(imghdrptr->getCLENGTH()); 
 this->setZLENGTH(imghdrptr->getZLENGTH());
 
#if VERBOSE
 std::cout<<"Reading in image volume from file = "<<imghdrptr->getFName()<<std::endl;
#endif

 if(imghdrptr->getImageFileFormat() == hageman::ANALYZE)
  this->readInAnalyzeVolumeFromFile(imghdrptr);
 else if(imghdrptr->getImageFileFormat() == hageman::NIFTI)
  std::cout<<"Read in Nifti file"<<std::endl;

}



//template<class T>
//void ImageVolumeV2N4<T>::readInAnalyzeVolumeFromFile(const FileNameV1N1<>* anlzhdrfname)
//a function for reading in a 3D Analyze Image Volume
//{
 
//#if VALIDATE
// std::cout<<"From function void ImageVolumeV2N4<T>::readInAnalyzeVolumeFromFile(const FileNameV1N1<> *anlzfname, DataType dt)"<<std::endl;
// std::cout<<"Anlzhdrfname: "<<anlzhdrfname->getOSCompatibleFileName()<<std::endl;
//#endif

// AnalyzeVolumeHeaderV1N2<T> anlzhdr(anlzhdrfname);
// this->readInAnalyzeVolumeFromFile(anlzhdr);
//}


template<class T>
void ImageVolumeV2N4<T>::readInAnalyzeVolumeFromFile(const ImageVolumeHeaderV1N2<T> *imghdrptr)
//a function for reading in a 3D Analyze Image Volume with voxel datatypes equal to T
{

 (this->ibfile) = new ReadFromBinaryFileV1N1<>(imghdrptr->getFNamePtr());  //links the binary read stream for the current object to the analyze image file
 (this->ibfile)->openForRead();  //opens the stream for reading

 const int BYTENUM = (imghdrptr->getBitsPerVoxel())/8;  //the number of bytes to read for each voxel

 char *buffer = new char[BYTENUM];
 T *ptr;  //the current voxel value

 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < this->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Reading Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < this->getCDIM(); cidx++)
   for(int ridx = 0; ridx < this->getRDIM(); ridx++)
   {
    (this->ibfile)->readFromFileStream(buffer,BYTENUM);
    if(imghdrptr->getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);  //converts to read buffer to the voxel value
    
    this->setElement(ridx,cidx,zidx,*ptr);  
    
   }
 }


 (this->ibfile)->close();
 delete (this->ibfile);
 (this->ibfile) = NULL;
 delete[] buffer;

}

/*

template<class T>
void ImageVolumeV2N4<T>::readInCombinedNiftiVolumeFromFile(const FileNameV1N1<>* niftifname)
//a function for reading in a 3D Nifti Image Volume
{
 
#if VALIDATE
 std::cout<<"From function void ImageVolumeV2N4<T>::readInCombinedNiftiVolumeFromFile(const FileNameV1N1<> *niftifname)"<<std::endl;
 std::cout<<"Niftifname: "<<niftifname->getOSCompatibleFileName()<<std::endl;
#endif

 NiftiVolumeHeaderV1N1<T> niftihdr;
 niftihdr.readInCombinedHeaderFile(niftifname);

 this->readInCombinedNiftiVolumeFromFile(niftihdr);


}


template<class T>
void ImageVolumeV2N4<T>::readInCombinedNiftiVolumeFromFile(const NiftiVolumeHeaderV1N1<T> &niftihdr)
//a function for reading in a 3D Nifti Image Volume with voxel datatypes equal to T
{

 FileNameV1N1<> *niftiimgfname = NULL;
 niftiimgfname = (niftihdr.getFNamePtr())->clone();
 
 //tries to open an Analyze image filename with the same name but .img extension
 //if((anlzimgfname->getExtension()) != "img")
 // anlzimgfname->setExtension("img");

 (this->ibfile).setFileName(niftiimgfname);  //links the binary read stream for the current object to the analyze image file
 (this->ibfile).openForRead();  //opens the stream for reading
 this->initVolume3D(niftihdr.getRDIM(), niftihdr.getCDIM(), niftihdr.getZDIM());  //initializes the object with the dimensions from the analyze header
 this->setRLENGTH(niftihdr.getRLENGTH()); 
 this->setCLENGTH(niftihdr.getCLENGTH()); 
 this->setZLENGTH(niftihdr.getZLENGTH());
 const int BYTENUM = (niftihdr.getBitsPerVoxel())/8;  //the number of bytes to read for each voxel
 
 char *buffer = new char[BYTENUM];
 T *ptr;  //the current voxel value

#if VERBOSE
  std::cout<<"Reading in image volume from Nifti volume file = "<<niftiimgfname->getOSCompatibleFileName()<<std::endl;
#endif
 
 int voxoffset = static_cast<int>(niftihdr.getVoxoffset());
 (this->ibfile).placeGetStreamAtAbsolutePosition(voxoffset);

 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < this->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Reading Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < this->getCDIM(); cidx++)
   for(int ridx = 0; ridx < this->getRDIM(); ridx++)
   {
    (this->ibfile).readFromFileStream(buffer,BYTENUM);
    if(niftihdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);  //converts to read buffer to the voxel value
    
    this->setElement(ridx,cidx,zidx,*ptr);  
    
   }
 }


 (this->ibfile).close();
 delete niftiimgfname;
 delete[] buffer;

}


 
template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is read from the file
void ImageVolumeV2N4<T>::readInConvertedAnalyzeVolumeFromFile(const AnalyzeVolumeHeaderV1N2<S> &anlzhdr, S scalefactor)  
//creates an image volume from an Analyze file, converting each element to the datatype T  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{

 FileNameV1N1<> *anlzimgfname = NULL;
 anlzimgfname = (anlzhdr.getFNamePtr())->clone();
 
 //tries to open an Analyze image filename with the same name but .img extension
 if((anlzimgfname->getExtension()) != "img")
  anlzimgfname->setExtension("img");

 (this->ibfile).setFileName(anlzimgfname);
 (this->ibfile).openForRead();  //links the binary read stream for the current object to the analyze image file
 this->initVolume3D(anlzhdr.getRDIM(), anlzhdr.getCDIM(), anlzhdr.getZDIM());  //initializes the object with the dimensions from the analyze header
 this->setRLENGTH(anlzhdr.getRLENGTH()); 
 this->setCLENGTH(anlzhdr.getCLENGTH()); 
 this->setZLENGTH(anlzhdr.getZLENGTH());
 const int BYTENUM = (anlzhdr.getBitsPerVoxel())/8;  //the number of bytes to read for each voxel
 
 char *buffer = new char[BYTENUM];
 S *ptr;
 S oval;  //the current voxel value
 T val;  //the converted voxel value

#if VERBOSE
  std::cout<<"Reading in image volume from Analyze volume file = "<<anlzimgfname->getOSCompatibleFileName()<<std::endl;
#endif
 
 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < this->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Reading Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < this->getCDIM(); cidx++)
   for(int ridx = 0; ridx < this->getRDIM(); ridx++)
   {
    (this->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<S*>(buffer);  //converts the read buffer to the datatype S
    oval = static_cast<S>(*ptr);
    if(scalefactor != S())  //if scalefactor is not 0, then multiply the current value by the scale factor
     oval = scalefactor*oval;
    val = static_cast<T>(oval);  //converts the current voxel value to datatype T
    
    this->setElement(ridx,cidx,zidx,val);
    
   }
 }


 (this->ibfile).close();
 delete anlzimgfname;
 delete[] buffer;



}

template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is read from the file
void ImageVolumeV2N4<T>::readInConvertedAnalyzeVolumeFromFile(FileNameV1N1<> *ifptr, S scalefactor)  
//creates an image voluem from an Analyze file, converting each element to the datatype T.  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{

 
#if VALIDATE
  std::cout<<"From function void ImageVolumeV2N4<T>::readInConvertedAnalyzeVolumeFromFile(FileNameV1N1<> *ifptr, S scalefactor)"<<std::endl;
  std::cout<<"Anlzhdrfname: "<<ifptr->getOSCompatibleFileName()<<std::endl;
#endif

 DataType dtv = getDataType(scalefactor);  //gets the datatype of S
 AnalyzeVolumeHeaderV1N2<S> hdr(ifptr);  //constructs an Analyze header file of type S
 hdr.setDataType(dtv);  //explicity sets the datatype and size of S
 hdr.setBitsPerVoxel(8*hageman::getDataTypeByteSize(dtv));

 this->readInConvertedAnalyzeVolumeFromFile(hdr, scalefactor);

 
 
}
*/



//WRITE FUNCTIONS
template<class T>
void ImageVolumeV2N4<T>::writeOutImageVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<T> *imghdrptr)
{

#if VERBOSE
  std::cout<<"Writing out image volume to file = "<<hdrfname->getOSCompatibleFileName()<<std::endl;
#endif

 if(imghdrptr->getImageFileFormat() == hageman::ANALYZE)
  this->writeOutAnalyzeVolumeToFile(hdrfname,imghdrptr);
 else if(imghdrptr->getImageFileFormat() == hageman::NIFTI)
  std::cout<<"Read in Nifti file"<<std::endl;



}




template<class T>
void ImageVolumeV2N4<T>::writeOutAnalyzeVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<T> *imghdrptr)
//a function for writing out the image volume to a 3D Analyze file.  Writes the voxels out in default Analyze order, which runs across the columns first 
//by default, then the second dimension is A-P, then third dimension is I-S
{

#if VALIDATE
  std::cout<<"From function void ImageVolumeV2N4<T>::writeOutAnalyzeVolumeToFile(const FileNameV1N1<> *imgfname, const ImageVolumeHeaderV1N2<T> *imhdrptr)"<<std::endl;
  std::cout<<"Hdr: "<<std::endl;
  imghdrptr->printHeader();
#endif
 
 imghdrptr->writeOutHeaderFile(hdrfname);
 (this->obfile) = new WriteToBinaryFileV1N1<>(imghdrptr->getFNamePtr());
 (this->obfile)->openForWrite();

 const int BYTENUM = (imghdrptr->getBitsPerVoxel())/8;  //the number of bytes for the datatype specified the hdr file
 char *charptr = NULL;
 T *ptr = NULL;
 T val = T();  //the current voxel value

 for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
 {
#if VERBOSE
  std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < this->getCDIM(); cidx++)
   for(int ridx = 0; ridx < this->getRDIM(); ridx++)
   {
    val = this->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);  //converts the current voxel to a char*
    if(imghdrptr->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (this->obfile)->writeBytesToFileStream(charptr,BYTENUM);  //writes the buffer to the binary image file
        
   }
 }

 (this->obfile)->close();
 delete (this->obfile);
 (this->obfile) = NULL;
 

}

template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is written to the file
void ImageVolumeV2N4<T>::writeOutConvertedImageVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<S> *imghdrptr)  
//creates an Analyze file from the current object, converting each element to the datatype S.  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{

#if VERBOSE
  std::cout<<"Writing out image volume to file = "<<hdrfname->getOSCompatibleFileName()<<std::endl;
#endif

 if(imghdrptr->getImageFileFormat() == hageman::ANALYZE)
  this->writeOutConvertedAnalyzeVolumeToFile(hdrfname,imghdrptr);
 else if(imghdrptr->getImageFileFormat() == hageman::NIFTI)
  std::cout<<"Read in Nifti file"<<std::endl;


}
  




template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is written to the file
void ImageVolumeV2N4<T>::writeOutConvertedAnalyzeVolumeToFile(const FileNameV1N1<> *hdrfname, const ImageVolumeHeaderV1N2<S> *hdr)  
//creates an Analyze file from the current object, converting each element to the datatype S.  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{
  
 //FileNameV1N1<> *anlzimgfname = NULL;
 //anlzimgfname = (imghdrptr->getFNamePtr())->clone();

 //if((anlzimgfname->getExtension()) != ".img")
 // anlzimgfname->setExtension(".img");

#if VERBOSE
 std::cout<<"Writing Out "<<(hdr->getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif

 hdr->writeOutHeaderFile(hdrfname);  //writes out the .hdr file
 (this->obfile) = new WriteToBinaryFileV1N1<>(hdr->getFNamePtr());
 (this->obfile)->openForWrite();  //associates the binary write stream of the current object with the Analyze header filename

 const int BYTENUM = (hdr->getBitsPerVoxel())/8;  //the number of bytes representing each voxel
 char *charptr = NULL;
 S *ptr = NULL;
 S val = S();  //the current converted voxel value to be written

//#if VERBOSE
// std::cout<<"Writing Out "<<anlzimgfname->getOSCompatibleFileName()<<"..."<<std::endl;
//#endif

  for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  {
#if VERBOSE
   std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
   for(int cidx = 0; cidx < this->getCDIM(); cidx++)
    for(int ridx = 0; ridx < this->getRDIM(); ridx++)
    {
     val = static_cast<S>(this->getElement(ridx,cidx,zidx));  //scale factor is multipled into the current voxel value
     ptr = &val;  //assigns the value to a ptr
     charptr = reinterpret_cast<char *>(ptr);  //converts the value to a char*
     if(hdr->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
      byteswap<char>(charptr,BYTENUM);
     (this->obfile)->writeBytesToFileStream(charptr,BYTENUM);  //writes out the char bytes
        
    }
  }
 


 (this->obfile)->close();
 //delete anlzimgfname;
}





/*
template<class T>
void ImageVolumeV2N4<T>::writeOutAnalyzeVolumeToFile(FileNameV1N1<> *ofname, bool shouldbyteswapv)
//a function for writing out the image volume to a 3D Analyze file in the datatype T.  Writes the voxels out in default Analyze order, which runs across the columns first 
//by default, then the second dimension is A-P, then third dimension is I-S
{
 T val = T();  //the current voxel
 DataType dtv = getDataType(val);  //the datatype T
 
 AnalyzeVolumeHeaderV1N2<T> hdr(ofname,ANALYZE,VOLUME,dtv,8*(getDataTypeByteSize(dtv)),shouldbyteswapv,this->getRDIM(), this->getCDIM(), this->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH());
 this->writeOutAnalyzeVolumeToFile(hdr);
}


template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is written to the file
void ImageVolumeV2N4<T>::writeOutConvertedAnalyzeVolumeToFile(const AnalyzeVolumeHeaderV1N2<S> &hdr, S scalefactor)  
//creates an Analyze file from the current object, converting each element to the datatype S.  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{
  
 FileNameV1N1<> *anlzimgfname = NULL;
 anlzimgfname = (hdr.getFNamePtr())->clone();

 if((anlzimgfname->getExtension()) != ".img")
  anlzimgfname->setExtension(".img");

#if VERBOSE
 std::cout<<"Writing Out "<<(hdr.getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif

 hdr.writeOutHeaderFile();  //writes out the .hdr file
 (this->obfile).setFileName(anlzimgfname);
 (this->obfile).openForWrite();  //associates the binary write stream of the current object with the Analyze header filename

 const int BYTENUM = (hdr.getBitsPerVoxel())/8;  //the number of bytes representing each voxel
 char *charptr = NULL;
 S *ptr = NULL;
 S val = S();  //the current converted voxel value to be written

#if VERBOSE
 std::cout<<"Writing Out "<<anlzimgfname->getOSCompatibleFileName()<<"..."<<std::endl;
#endif

 if(scalefactor != S())  //scale factor is non-zero; will be multiplied into each voxel before it is written
 {
  for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  {
#if VERBOSE
   std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
   for(int cidx = 0; cidx < this->getCDIM(); cidx++)
    for(int ridx = 0; ridx < this->getRDIM(); ridx++)
    {
     //must cast everything as a double to avoid any possible truncation
     val = static_cast<S>(static_cast<double>(scalefactor)*(static_cast<double>(this->getElement(ridx,cidx,zidx))));  //scale factor is multipled into the current voxel value
     ptr = &val;  //assigns the value to a ptr
     charptr = reinterpret_cast<char *>(ptr);  //converts the value to a char*
     if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
      byteswap<char>(charptr,BYTENUM);
     (this->obfile).writeBytesToFileStream(charptr,BYTENUM);  //writes out the char bytes
        
    }
  }
 }
 else  //scale factor is zero
 {
  for(int zidx = 0; zidx < (this->getZDIM()); zidx++)
  {
#if VERBOSE
   std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
   for(int cidx = 0; cidx < this->getCDIM(); cidx++)
    for(int ridx = 0; ridx < this->getRDIM(); ridx++)
    {
     val = static_cast<S>(this->getElement(ridx,cidx,zidx));  //scale factor is multipled into the current voxel value
     ptr = &val;  //assigns the value to a ptr
     charptr = reinterpret_cast<char *>(ptr);  //converts the value to a char*
     if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
      byteswap<char>(charptr,BYTENUM);
     (this->obfile).writeBytesToFileStream(charptr,BYTENUM);  //writes out the char bytes
        
    }
  }
 }


 (this->obfile).close();
 delete anlzimgfname;

}



template<class T>
template<class S>  //the datatype of the voxels specified by the Analyze header; this datatype will be what is read from the file
void ImageVolumeV2N4<T>::writeOutConvertedAnalyzeVolumeToFile(FileNameV1N1<> *ofname, bool shouldbyteswapv, S scalefactor)
//creates an Analyze file from the current object, converting each element to the datatype S.  The optional scalefactor will be multiplied to each voxel value - set to zero if you do
//do not wish any scaling
{
 DataType dtv = getDataType(scalefactor);
 AnalyzeVolumeHeaderV1N2<S> hdr(ofname,ANALYZE,VOLUME,dtv,8*(getDataTypeByteSize(dtv)),shouldbyteswapv,this->getRDIM(), this->getCDIM(), this->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH());
 this->writeOutConvertedAnalyzeVolumeToFile(hdr, scalefactor);
 
}
*/

//PRINT FUNCTIONS
template<class T>
void ImageVolumeV2N4<T>::printVolInfo() const
//prints out the volume dimensions/lengths
{

 Volume3DV2N4<T>::printVolInfo();  //calls the base class function
 std::cout<<"Voxel Lengths: "<<this->getRLENGTH()<<" "<<this->getCLENGTH()<<" "<<this->getZLENGTH()<<std::endl;

}
 
template<class T>
void ImageVolumeV2N4<T>::printCL() const  
//implementation of virtual function from base Volume class
{
 Volume3DV2N4<T>::printCL();  //calls the base class function


}

//MATH FUNCTIONS
template<class T>
void ImageVolumeV2N4<T>::addGaussianNoise(double stddev)  
//adds Gaussian noise to the image around a mean of zero and with a standard deviation of stddev.  Corrects any voxels which fall below zero by setting them back to zero 
{
 GaussianNoiseV1N2<T> gnoise(time(NULL));
 T noise = T();

 for(int idx = 0; idx < this->getSIZE(); idx++)
 {
  noise = gnoise.generateGaussianRandomNumber(0,stddev);
  this->setElement(idx,(((*this)[idx]) + noise));
  if((this->getElement(idx)) < T())  //corrects any voxels that have fallen below zero.
   this->setElement(idx,T());  //sets them back to zero
 }


}

//CONVERSION FUNCTIONS
template<class T>
inline double ImageVolumeV2N4<T>::convertRVoxelToRReal(int rvoxel) const
//converts the voxel coordinates to real world coordinates in the RDIM
{
 double rconv = static_cast<double>(rvoxel)*RLENGTH;
 rconv += (0.5f * RLENGTH);
 return rconv;


}

template<class T>
inline int ImageVolumeV2N4<T>::convertRRealToRVoxel(double rcoord) const
//converts the real world coordinates to voxel coordinates in the RDIM
{
 int rconv = static_cast<int>(floor(rcoord/RLENGTH));
 return rconv;
}

template<class T>
inline double ImageVolumeV2N4<T>::convertCVoxelToCReal(int cvoxel) const
//converts the voxel coordinates to real world coordinates in the CDIM
{
 double cconv = static_cast<double>(cvoxel)*CLENGTH;
 cconv += (0.5f * CLENGTH);
 return cconv;
}

template<class T>
inline int ImageVolumeV2N4<T>::convertCRealToCVoxel(double ccoord) const
//converts the real world coordinates to voxel coordinates in the CDIM
{
 int cconv = static_cast<int>(floor(ccoord/CLENGTH));
 return cconv;
}

template<class T>
inline double ImageVolumeV2N4<T>::convertZVoxelToZReal(int zvoxel) const
//converts the voxel coordinates to real world coordinates in the ZDIM

{
 double zconv = static_cast<double>(zvoxel)*ZLENGTH;
 zconv += (0.5f * ZLENGTH);
 return zconv;
}

template<class T>
inline int ImageVolumeV2N4<T>::convertZRealToZVoxel(double zcoord) const
//converts the real world coordinates to voxel coordinates in the ZDIM
{
 int zconv = static_cast<int>(floor(zcoord/ZLENGTH));
 return zconv;
}


#define NATURALSPLINETHRESHOLD 1.0e30



template<class T = double>
class MathV1N2{

public:
 //CONSTRUCTORS
 MathV1N2(); //empty default constructor
 MathV1N2(int seedv); //used to initialize a random number generator with a specific seed
 ~MathV1N2();  //destructor

 //GET FUNCTIONS
 
 //SET FUNCTIONS

 //CALCULATION FUNCTIONS
 T calculateGaussian(T x, T a, T b, T c); //formula is f(x) = a*exp(-(x-b)^2/(2c^2))
 

 //INTERPOLATION
 static void calculate1DCubicSplineSecondDerivatives(const T *x, const T *y, const T &yp1, const T &ypn, T *y2, const int &ARRAYSIZE);  //from Numerical Recipes in C++, this 
 //is a utility function used to calculate interpolating splines
 static void calculate1DCubicSpline(const T *xa, const T *ya, const T *y2a, const T &x, T &y, const int &ARRAYSIZE); //calculates a 1-dimensional cubic spline interpolation at the 
 //point (x,y)
 static VectorV2N4<T> calculate1DCubicSplineSecondDerivatives(const VectorV2N4<T> &x, const VectorV2N4<T> &y, const T &yp1, const T &ypn);  //from Numerical Recipes in C++, this 
 //is a utility function used to calculate interpolating splines
 static T calculate1DCubicSpline(const VectorV2N4<T> &xa, const VectorV2N4<T> &ya, const VectorV2N4<T> &y2a, const T &x); //calculates a 1-dimensional cubic spline interpolation at the 
 //point (x,y)
 static void calculate2DCubicSplineSecondDerivatives(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &ya, Matrix2DV2N4<T> &y2a);  //from Numerical Recipes in C++, this 
 //is a utility function used to calculate 2D interpolating splines
 static T calculate2DCubicSpline(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &ya, const Matrix2DV2N4<T> &y2a, const T &x1, const T &x2); //calculates a 2-dimensional cubic spline interpolation at the 
 //point (x1,x2)
 static void calculate3DCubicSplineSecondDerivatives(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, Matrix3DV2N4<T> &y2a);  //from Numerical Recipes in C++, this 
 //is a utility function used to calculate 3D interpolating splines
 static T calculate3DCubicSpline(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, const Matrix3DV2N4<T> &y2a, const T &x1, const T &x2, const T &x3); //calculates a 3-dimensional 
 //cubic spline interpolation at the point (x1,x2,x3)

 
 //RANDOM NUMBERS
 int getRandomNumberMerseeneTwister(const int &RANDMAXIMUM, const int &RANDMINIMUM = 0);  //returns a random number based on the Merseene Twister generator class.  This makes use of
 //the code on Non-Uniform Random Number Generators by Agner Fog
 //T generateRiceNumber(const T mu, const T sigma);

 //LINEAR SYSTEMS
 static void leastSquaresFitUsingSVD(const T *x, const T *y, Matrix2DV2N4<T> &a, const Matrix2DV2N4<T> &A, Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V, T &chisq, const T *sig);
 //Numerical Recipes function that performs a least squares fit using SVD

protected:

 //DATA
 
 //RANDOM NUMBERS
 StochasticLib1 *stolib;
 static int seed;
 
 

private:



};

template<class T>
int MathV1N2<T>::seed = time(NULL);

//CONSTRUCTORS

template<class T>
MathV1N2<T>::MathV1N2()
//default empty constructor
{
 //initializes the stochastic library to generate random numbers
 stolib = NULL;
 stolib = new StochasticLib1(seed);
 seed++;
 
}



template<class T>
MathV1N2<T>::MathV1N2(int seedv)
//initializes stolib with a specific seed
{
 //initializes the stochastic library to generate random numbers
 stolib = NULL;
 stolib = new StochasticLib1(seedv);

}

template<class T>
MathV1N2<T>::~MathV1N2()
//destructor
{
 //deletes the stolib if it has been initialized
 if(stolib != NULL)
  delete stolib;


}

//CALCULATION FUNCTIONS
template<class T>
T MathV1N2<T>::calculateGaussian(T x, T a, T b, T c) 
//formula is f(x) = a*exp(-(x-b)^2/(2c^2))
{

 T expon = pow((x - b),2.0);
 expon /= (2*pow(c,2.0));

 T result = a*(exp(-expon));

 return result;


}


//INTERPOLATION
template<class T>
void MathV1N2<T>::calculate1DCubicSplineSecondDerivatives(const T *x, const T *y, const T &yp1, const T &ypn, T *y2, const int &ARRAYSIZE)  
//from Numerical Recipes in C++, this is a utility function used to calculate interpolating splines. Given arrays x[0...n-1] and y[0...n-1] containing
//tabulated function (y = f(x)) and given values yp1 and ypn for the first derivatives of the interpolating function at points 0 and n-1.  Routine
//returns an array y2[0...n-1] that contains the second derivatives of the interpolating function at the tabulated points.  If yp1 and or ypn is 
//equal to 1x10^30 or larger, then the routine is signaled to set the corresponding boundary condition for a natural spline (zero second derivative
//on the boundary)
{
 int i,k;
 T p,qn,sig,un;

 int n = ARRAYSIZE;
 T *u = new T[n-1];

 if(yp1 >= (NATURALSPLINETHRESHOLD))
  y2[0] = u[0] = 0;
 else
 {
  y2[0] = static_cast<T>(-0.5);
  u[0] = (static_cast<T>(3.0f)/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
 }

 for(i = 1; i<(n-1); i++)
 {
  sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
  p = sig * y2[i-1] + static_cast<T>(2.0f);
  y2[i] = (sig - static_cast<T>(1.0f)) / p;
  u[i] = (y[i+1] - y[i]) / (x[i+1] - x[i]) - (y[i] - y[i-1])/(x[i] - x[i-1]);
  u[i] = (static_cast<T>(6.0f) * u[i] / (x[i+1] - x[i-1]) - sig * u[i-1]) / p;
 }

 if(ypn >= (NATURALSPLINETHRESHOLD))
  qn = un = 0;
 else
 {
  qn = static_cast<T>(0.5f);
  un = (static_cast<T>(3.0f) / (x[n-1] - x[n-2])) * (ypn - (y[n-1] - y[n-2]) / (x[n-1] - x[n-2]));
 }
 
 y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + static_cast<T>(1.0f));
 
 for(k = n-2; k >= 0; k--)
  y2[k] = y2[k] * y2[k+1] + u[k];

 delete[] u;


}

template<class T>
void MathV1N2<T>::calculate1DCubicSpline(const T *xa, const T *ya, const T *y2a, const T &x, T &y, const int &ARRAYSIZE) 
//from Numerical Recipes in C++, calculates a 1-dimensional cubic spline interpolation at the point (x,y).  Given arrays x[0...n-1] and y[0...n-1] 
//containing tabulated function (y = f(x)) and given an array y2[0...n-1] that contains the second derivatives of the interpolating function at 
//the tabulated points (output of calculateCubicSplineSecondDerivatives() above).  Given the value x, the routine will output the cubic spline 
//interpolated value y. 
{
 int k;
 T h,b,a;

 int n = ARRAYSIZE;
 int klo = 0;
 int khi = n-1;

 while(khi-klo > 1)
 {
  k = (khi + klo) >> 1;
  if(xa[k] > x)
   khi = k;
  else
   klo = k;
 }

 h = xa[khi] - xa[klo];
 
#if ERRORCHECK
 if(h == static_cast<T>(0.0f))
 {
  std::cout<<"ERROR in static void MathV1N2<T>::calculateCubicSpline"<<std::endl;
  std::cout<<"Bad xa input to routine.  The xa's must be distinct. Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 a = (xa[khi] - x) / h;
 b = (x - xa[klo]) / h;
 y = a * ya[klo] + b * ya[khi] + ((a*a*a - a) * y2a[klo] + (b*b*b - b) * y2a[khi]) * (h*h) / static_cast<T>(6.0f); 


}


template<class T>
VectorV2N4<T> MathV1N2<T>::calculate1DCubicSplineSecondDerivatives(const VectorV2N4<T> &x, const VectorV2N4<T> &y, const T &yp1, const T &ypn)
//from Numerical Recipes in C++, this is a utility function used to calculate interpolating splines. Given Vectors x[0...n-1] and y[0...n-1] containing
//tabulated function (y = f(x)) and given values yp1 and ypn for the first derivatives of the interpolating function at points 0 and n-1.  Routine
//returns a Vector[0...n-1] that contains the second derivatives of the interpolating function at the tabulated points.  If yp1 and or ypn is 
//equal to 1x10^30 or larger, then the routine is signaled to set the corresponding boundary condition for a natural spline (zero second derivative
//on the boundary)
{

 const int ARRAYSIZE = x.getSIZE();
 VectorV2N4<T> result(ARRAYSIZE);
 calculate1DCubicSplineSecondDerivatives(x.getConstDataReference(), y.getConstDataReference(), yp1, ypn, result.getNonConstDataReference(), ARRAYSIZE);

 return (result);
 
 
}

template<class T>
T MathV1N2<T>::calculate1DCubicSpline(const VectorV2N4<T> &xa, const VectorV2N4<T> &ya, const VectorV2N4<T> &y2a, const T &x) 
//from Numerical Recipes in C++, calculates a 1-dimensional cubic spline interpolation at the point (x,y).  Given Vectors x[0...n-1] and y[0...n-1] 
//containing tabulated function (y = f(x)) and given a Vector y2[0...n-1] that contains the second derivatives of the interpolating function at 
//the tabulated points (output of calculate1DCubicSplineSecondDerivatives() above).  Given the value x, the routine will output the cubic spline 
//interpolated value y. 
{

 const int ARRAYSIZE = xa.getSIZE();
 T result = T();
 calculate1DCubicSpline(xa.getConstDataReference(), ya.getConstDataReference(), y2a.getConstDataReference(), x, result, ARRAYSIZE);

 return result;

}

template<class T>
void MathV1N2<T>::calculate2DCubicSplineSecondDerivatives(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &ya, Matrix2DV2N4<T> &y2a)  
//from Numerical Recipes in C++, this is a utility function used to calculate interpolating splines. Given a 2D matrix of values, ya, and tabulated
//independent variables x2a in the CDIM dimension, this routine constructs 1D natural (2nd deriv = 0) cubic splines of the columns of ya and
//stores the result in the Matrix2D y2a.  x1a is included for consistency with the routine below, calculate2DCubicSpline()
{
 int m,n,j,k;

 m = ya.getRDIM();
 n = ya.getCDIM();
 const int ARRAYSIZE = n;

 y2a.makeCompatible(ya); //makes sure that y2a is of the same dimenions as ya

 T *ya_t = new T[ARRAYSIZE];
 T *y2a_t = new T[ARRAYSIZE];

 for(j = 0; j < m; j++)
 {
  for(k = 0; k < n; k++)
   ya_t[k] = ya.getElement(j,k);

  MathV1N2<T>::calculate1DCubicSplineSecondDerivatives(x2a,ya_t,static_cast<double>(1+NATURALSPLINETHRESHOLD),static_cast<double>(1+NATURALSPLINETHRESHOLD),y2a_t,ARRAYSIZE);
  //1+NATURALSPLINETHRESHOLD sets the second derivative to 0 (natural spline)

  for(k = 0; k < n; k++)
   y2a.setElement(j,k,y2a_t[k]);
 }

 delete[] ya_t;
 delete[] y2a_t;


}


template<class T>
T MathV1N2<T>::calculate2DCubicSpline(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &ya, const Matrix2DV2N4<T> &y2a, const T &x1, const T &x2) 
//from Numerical Recipes in C++, calculates a 2-dimensional cubic spline interpolation at the point (x1,x2).  Given a 2D matrix of values, ya, and tabulated
//independent variables x1a and x2a in the RDIM and CDIM dimensions, respectively, this routine returns an interpolated value at (x1,x2) by bicubic spline
//interpolation
{

 T result = T();
 int j,k;

 const int m = ya.getRDIM();
 const int n = ya.getCDIM();
 
 T *ya_t = new T[n];
 T *y2a_t = new T[n];
 T *yytmp = new T[m];
 T *ytmp = new T[m];

 for(j = 0; j < m; j++)
 {
  for(k = 0; k < n; k++)
  {
   ya_t[k] = ya.getElement(j,k);
   y2a_t[k] = y2a.getElement(j,k);
   
  }
  MathV1N2<T>::calculate1DCubicSpline(x2a,ya_t,y2a_t,x2,yytmp[j],n);
 }

 MathV1N2<T>::calculate1DCubicSplineSecondDerivatives(x1a,yytmp, static_cast<double>(1+NATURALSPLINETHRESHOLD), static_cast<double>(1+NATURALSPLINETHRESHOLD), ytmp, m);
 MathV1N2<T>::calculate1DCubicSpline(x1a,yytmp,ytmp,x1,result,m);

 delete[] ya_t;
 delete[] y2a_t;
 delete[] ytmp;
 delete[] yytmp;


 return result;


}


template<class T>
void MathV1N2<T>::calculate3DCubicSplineSecondDerivatives(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, Matrix3DV2N4<T> &y2a)  
//from Numerical Recipes in C++, this is a utility function used to calculate interpolating splines. Given a 3D matrix of values, ya, and tabulated
//independent variables x2a in the CDIM dimension, this routine constructs 1D natural (2nd deriv = 0) cubic splines of the columns of ya and
//stores the result in the Matrix3D y2a.  x1a, x3a are included for consistency with the routine below, calculate3DCubicSpline()
{
 int m,n,o,j,k,l;

 m = ya.getRDIM();
 n = ya.getCDIM();
 o = ya.getZDIM();
 
 y2a.makeCompatible(ya); //makes sure that y2a is of the same dimenions as ya

 Matrix2DV2N4<T> ya_t(m,n);
 Matrix2DV2N4<T> y2a_t(m,n);

 for(l = 0; l < o; l++)
 {
  for(j = 0; j < m; j++)
   for(k = 0; k < n; k++)
    ya_t.setElement(j,k,ya.getElement(j,k,l));

  MathV1N2<T>::calculate2DCubicSplineSecondDerivatives(x1a,x2a,ya_t,y2a_t);
  
  for(j = 0; j < m; j++)
   for(k = 0; k < n; k++)
    y2a.setElement(j,k,l,y2a_t.getElement(j,k));
  
 }

}


template<class T>
T MathV1N2<T>::calculate3DCubicSpline(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, const Matrix3DV2N4<T> &y2a, const T &x1, const T &x2, const T &x3) 
//from Numerical Recipes in C++, calculates a 3-dimensional cubic spline interpolation at the point (x1,x2,x3).  Given a 3D matrix of values, ya, and tabulated
//independent variables x1a, x2a, and x3a in the RDIM, CDIM, and ZDIM dimensions, respectively, this routine returns an interpolated value at (x1,x2,x3) by bicubic spline
//interpolation
{

 T result = T();
 int j,k,l;

 const int m = ya.getRDIM();
 const int n = ya.getCDIM();
 const int o = ya.getZDIM();
  
 Matrix2DV2N4<T> ya_t(m,n);
 Matrix2DV2N4<T> y2a_t(m,n);

 T *yytmp = new T[o];
 T *ytmp = new T[o];


 for(l = 0; l < o; l++)
 {
  for(j = 0; j < m; j++)
   for(k = 0; k < n; k++)
   {
    ya_t.setElement(j,k,ya.getElement(j,k,l));
    y2a_t.setElement(j,k,y2a.getElement(j,k,l));
    
   }

  yytmp[l] = MathV1N2<T>::calculate2DCubicSpline(x1a,x2a,ya_t,y2a_t,x1,x2);
 }

 MathV1N2<T>::calculate1DCubicSplineSecondDerivatives(x3a,yytmp, static_cast<double>(1+NATURALSPLINETHRESHOLD), static_cast<double>(1+NATURALSPLINETHRESHOLD), ytmp, o);
 MathV1N2<T>::calculate1DCubicSpline(x2a,yytmp,ytmp,x3,result,o);

 delete[] ytmp;
 delete[] yytmp;


 return result;


}

//RANDOM NUMBERS

template<class T>
int MathV1N2<T>::getRandomNumberMerseeneTwister(const int &RANDMAXIMUM, const int &RANDMINIMUM)  
//returns a random number based on the Merseene Twister generator class.  This makes use of the code on Non-Uniform Random Number Generators by 
//Agner Fog
{
 int result = stolib->IRandom(RANDMINIMUM,RANDMAXIMUM);
 return result;

}

//LINEAR SYSTEMS
template<class T>
void MathV1N2<T>::leastSquaresFitUsingSVD(const T *x, const T *y, Matrix2DV2N4<T> &a, const Matrix2DV2N4<T> &A, Matrix2DV2N4<T> &w, Matrix2DV2N4<T> &V, T &chisq, const T *sig)
{
 int i,j;

 const double TOL = 1.0e-6;
 double wmax,tmp,thresh,sum;

 //A is the predefined NxM design matrix, with N datapts and M basis functions
 Matrix2DV2N4<T> U(A);
 int ndata = A.getRDIM();
 int ma = A.getCDIM();
 Matrix2DV2N4<T> b(ndata,1);
 
 for(i = 0; i < ndata; i++)
 {
  //function for evaluating basis function (funcs(x[i],afunc)
  tmp = 1.0f / sig[i];

  for(int j = 0; j < ma; j++)
   U.setElement(i,j,(static_cast<T>(tmp))*U.getElement(i,j));
  
  b[i] = static_cast<double>(y[i])*tmp;

 }
 
 U.singularValueDecomposition(w,V);
 w.editSingularValues(static_cast<T>(TOL));
 U.SVDBackSolve(w,V,b,a);

 chisq = 0.0;

 for(i = 0; i < ndata; i++)
 {
  sum = 0.0;
  for(j = 0; j < ma; j++)
   sum += a.getElement(j,1)*A.getElement(i,j);
   
  chisq += (tmp=(y[i]-sum)/sig[i],tmp*tmp);
 }
}
  
#define MY_DTYPE double


template<class T = double>
class NumericalRecipesV1N2 : public MathV1N2<T>{

public:
 //CONSTRUCTORS
  NumericalRecipesV1N2(); //empty default constructor
  ~NumericalRecipesV1N2();  //destructor

 //GET FUNCTIONS
 
 //SET FUNCTIONS


 //PRINT FUNCTIONS
    void printNRVectorCL(const NRvector<T> &vec);

 //CONVERSION FUNCTIONS
 static void convertArrayToNRVector(const T *ptr, NRvector<T> &vec, const int ARRAYSIZE);
 static NRvector<T> convertVectorToNRVector(const VectorV2N4<T> &vec);
 static void convertMatrixToNRMatrix(const Matrix2DV2N4<T> &paradigm, NRmatrix<T> &mat);

 //INTERPOLATION
 static T calculate1DLinearInterpolation(const T *xa, const T *ya, const T &x, const int ARRAYSIZE);
 static T calculate2DBiLinearInterpolation(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &y, const T &x1, const T &x2);
 static T calculate2DBiLinearInterpolation(const VectorV2N4<T> &x1vec, const VectorV2N4<T> &x2vec, const Matrix2DV2N4<T> &ya, const T &x1, const T &x2);
 static T calculate3DTriLinearInterpolation(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, const T &x1, const T &x2, const T &x3);
 static T calculate3DTriLinearInterpolation(const VectorV2N4<T> &x1vec, const VectorV2N4<T> &x2vec, const VectorV2N4<T> &x3vec, const Matrix3DV2N4<T> &ya, const T &x1, const T &x2, const T &x3);

 //RANDOM NUMBERS
 T generateRiceNumber(const T nu, const T sigma);  //this function uses the standard rejection method for generating a random deviate that follows the Rice distribution: 
 //p(y) = (x/sigma^2)*(exp[-(x^2 + nu^2)/(2sigma^2)]*Io(x*nu/sigma^2), where Io is a modified Bessel function of the first kind.  The comparison function used for this 
 //method is 10 the magnitude of the Lorentzian distribution. 

 //LINEAR SYSTEMS
 VectorV2N4<T> solveLinearSystemLeastSquaresSVD(const VectorV2N4<T> &yvec, const VectorV2N4<T> &sigvec, const Matrix2DV2N4<T> &A); 
 //solves the linear system for the user supplied design matrix.  Returns the coefficients of the fit 

 
protected:

 //DATA
 static Matrix2DV2N4<T> *designmatrix;  //matrix used for the SVD method
 
private:
  
 //UTILITY FUNCTIONS
 static NRvector<double> funks(const double x);  //utility function for the SVD method
 

 

};

template<class T>
Matrix2DV2N4<T>* NumericalRecipesV1N2<T>::designmatrix = NULL;


//CONSTRUCTORS

template<class T>
NumericalRecipesV1N2<T>::NumericalRecipesV1N2()
//default empty constructor
:MathV1N2<T>()
{
 designmatrix = NULL;

  
}


template<class T>
NumericalRecipesV1N2<T>::~NumericalRecipesV1N2()
//destructor
{
 //deletes the designmatrix, if initialized
 if(designmatrix != NULL)
 {
  delete designmatrix;
  designmatrix = NULL;
 }

}


//PRINT FUNCTIONS
template<class T>
void NumericalRecipesV1N2<T>::printNRVectorCL(const NRvector<T> &vec)
//prints out the NR vector to the CL
{

 std::cout<<"Vector:"<<std::endl;

 for(int idx = 0; idx < vec.size(); idx++)
 {
  std::cout<<"Idx ="<<idx<<" : "<<vec[idx]<<std::endl;
  

 }




}



//CONVERSION FUNCTIONS
template<class T>
void NumericalRecipesV1N2<T>::convertArrayToNRVector(const T *ptr, NRvector<T> &vec, const int ARRAYSIZE)
//element-wise copy of the array to a NR vector
{
 for(int idx = 0; idx < ARRAYSIZE; idx++)
 {
  vec[idx] = ptr[idx];
 }


}

template<class T>
void NumericalRecipesV1N2<T>::convertMatrixToNRMatrix(const Matrix2DV2N4<T> &paradigm, NRmatrix<T> &mat)
//element-wise copy of the matrix to a NR matrix
{
 for(int j = 0; j < paradigm.getCDIM(); j++)
  for(int i = 0; i < paradigm.getRDIM(); i++)
  {
   mat[i][j] = paradigm.getElement(i,j);

  }

}

template<class T>
NRvector<T> NumericalRecipesV1N2<T>::convertVectorToNRVector(const VectorV2N4<T> &vec)
//element-wise copy of a Vector to a NR vector
{

 NRvector<T> result(vec.getSIZE());

 for(int idx = 0; idx < vec.getSIZE(); idx++)
  result[idx] = vec.getElement(idx);
 
 return result;



}
 


//INTERPOLATION

template<class T>
T NumericalRecipesV1N2<T>::calculate1DLinearInterpolation(const T *xa, const T *ya, const T &x, const int ARRAYSIZE)
//1D interpolation function from NR
{
 T result = T();

 NRvector<double> xx(ARRAYSIZE);
 NRvector<double> yy(ARRAYSIZE);

 NumericalRecipesV1N2<T>::convertArrayToNRVector(xa,xx,ARRAYSIZE);
 NumericalRecipesV1N2<T>::convertArrayToNRVector(ya,yy,ARRAYSIZE);

 Linear_interp myfunc(xx,yy);

 result = static_cast<T>(myfunc.interp(static_cast<double>(x)));

 return result;

}

template<class T>
T NumericalRecipesV1N2<T>::calculate2DBiLinearInterpolation(const T *x1a, const T *x2a, const Matrix2DV2N4<T> &y, const T &x1, const T &x2)
//2D interpolation function from NR
{

 T result = T();
 
 NRvector<double> nrx1(y.getRDIM());
 NRvector<double> nrx2(y.getCDIM());

 NumericalRecipesV1N2<T>::convertArrayToNRVector(x1a,nrx1,y.getRDIM());
 NumericalRecipesV1N2<T>::convertArrayToNRVector(x2a,nrx2,y.getCDIM());

 NRmatrix<double> ymat(y.getRDIM(), y.getCDIM());
 NumericalRecipesV1N2<T>::convertMatrixToNRMatrix(y,ymat);

 Bilin_interp myfunc(nrx1,nrx2,ymat);

 result = myfunc.interp(x1,x2);

 return result;


}

template<class T>
T NumericalRecipesV1N2<T>::calculate2DBiLinearInterpolation(const VectorV2N4<T> &x1vec, const VectorV2N4<T> &x2vec, const Matrix2DV2N4<T> &ya, const T &x1, const T &x2)
//2D interpolation function from NR
{

 T result;
 result = NumericalRecipesV1N2<T>::calculate2DBiLinearInterpolation(x1vec.getConstDataReference(), x2vec.getConstDataReference(), ya, x1, x2);

 return result;

}
 
template<class T>
T NumericalRecipesV1N2<T>::calculate3DTriLinearInterpolation(const T *x1a, const T *x2a, const T *x3a, const Matrix3DV2N4<T> &ya, const T &x1, const T &x2, const T &x3)
//given vectors x1[0...M-1] and x2[0...N-1] and x2[0...P-1] and a MxNxP matrix, which define the set of anchor points y = f(x1,x2,x3), this function 
//computes a value y for the pt x that is the N-1 polynomial interpolated value.  The error dy is returned as well if desired, but only in the x1 direction
{

 T result;
 int i,j,k;
 T element;

 int m = ya.getRDIM();
 int n = ya.getCDIM();
 int p = ya.getZDIM();

 double *ymtmp = new double[p];
 Matrix2DV2N4<T> ya_t(m,n);
 
 for(k = 0; k < p; k++)
 {
  for(j = 0; j < n; j++)
   for(i = 0; i < m; i++)
   {
    element = ya.getElement(i,j,k);
    ya_t.setElement(i,j,element);

   }

   ymtmp[k] = NumericalRecipesV1N2<T>::calculate2DBiLinearInterpolation(x1a,x2a,ya_t,x1,x2);
   
 }

 //interpolates along the z dimension last
 result = NumericalRecipesV1N2<T>::calculate1DLinearInterpolation(x3a,ymtmp,x3,p);

 return result;

}

template<class T>
T NumericalRecipesV1N2<T>::calculate3DTriLinearInterpolation(const VectorV2N4<T> &x1vec, const VectorV2N4<T> &x2vec, const VectorV2N4<T> &x3vec, const Matrix3DV2N4<T> &ya, const T &x1, const T &x2, const T &x3)
{
 T result;

 result = NumericalRecipesV1N2<T>::calculate3DTriLinearInterpolation(x1vec.getConstDataReference(), x2vec.getConstDataReference(), x3vec.getConstDataReference(), ya, x1, x2, x3);
 return result;
}

 

//RANDOM NUMBERS
template<class T>
T NumericalRecipesV1N2<T>::generateRiceNumber(const T nu, const T sigma)
//this function uses the standard rejection method for generating a random deviate that follows the Rice distribution: p(y) = (x/sigma^2)*(exp[-(x^2 + nu^2)/(2sigma^2)]*Io(x*nu/sigma^2),
//where Io is a modified Bessel function of the first kind.  The comparison function used for this method is 10 the magnitude of the Lorentzian distribution.
{
 bool flag = false;

 //amplitude of comparison function is set at 10X the cdf of the approximate Lorentzian analogous to the Rice distribution function, where gamma = sigma, xo = nu
 //Lorentzian pdf: p(y) = (1/PI)*[gamma/((x-xo)^2 + gamma^2)]
 //Lorentzian cdf: F(y) = (1/PI)*arctan[x-xo/gamma] + 1/2
 //Inverse Lorentzian cdf: F-1(p) = xo + gamma * tan[PI(p-0.5)] 
 //the total area under the curve is equal to the Lorentzian cdf amplitude * 10 = 10
 T result;

 while(!flag)  //loop repeats until a value is accepted
 {
  //generate a uniform random number between (0-10)
  T uniformnum = 0;
  while((uniformnum == 0) || (uniformnum == 10))  //if number is 0 or 10, the Lorentzian cdf will give back an infinity
  {
   uniformnum = static_cast<T>(this->getRandomNumberMerseeneTwister(1000));
   uniformnum /= static_cast<T>(100.0);  //this factors in the 10X amplitude of the Lorentzian
   
  }
 
  //solve for x: uniformnum = lorentz cdf(x); analogously, x = F-1(uniformnum) [the amplitude gets moved to the inside of the tan function] 
  T tancoeff = (PI/10.0)*(uniformnum - 0.5);
  T lorentzx = nu + (sigma*(tan(tancoeff)));

  //solve for p(x): lorentzpdf = 10*p(lorentzx)
  T Ldenom = pow((lorentzx - nu),2.0) + pow(sigma,2.0);
  T lorentzpdf = (10.0/PI)*(sigma/Ldenom);

  //generate a second uniform deviate that spans [0,p(x)]
  T uniformnum2 = 0;
  while((uniformnum2 == 0) || (uniformnum2 == lorentzpdf))  //if number is 0 or lorentzpdf, the upper and lower bounds may lead to unexpected behavior
  {
   uniformnum2 = static_cast<T>(this->getRandomNumberMerseeneTwister(1000));
   uniformnum2 /= static_cast<T>(1000.0);  //this yields a uniform(0,1)
   uniformnum2 *= lorentzpdf;  //this scales between [0,lorentzpdf]
  }

  //determine value of the Rice pdf at lorentzx
  T besselcoeff = (lorentzx*nu) / (pow(sigma,2.0));
  Bessik mybesselfn;  //need a initiate a struct to call the evaluation function
  T besselnum = mybesselfn.in(0,besselcoeff);  //a call to the modified Bessel evaluation function in NR (bessel.h)
  T expcoeff = -1.0*(pow(lorentzx,2.0) + pow(nu,2.0));
  expcoeff /= 2.0*pow(sigma,2.0);
  T ricepdf = (lorentzx/pow(sigma,2.0))*expf(expcoeff)*besselnum;

  //if uniformnum2 is greater than the value of the Rice pdf, then reject lorentzx and try again
  if(uniformnum2 <= ricepdf)
  {
   flag = true;
   result = lorentzx;
  }

  
 }

 return result;

  
}



//LINEAR SYSTEMS
template<class T>
VectorV2N4<T> NumericalRecipesV1N2<T>::solveLinearSystemLeastSquaresSVD(const VectorV2N4<T> &yvec, const VectorV2N4<T> &sigvec, const Matrix2DV2N4<T> &A)  
//solves the linear system for the user supplied design matrix
{
 //must reinitialize this matrix everytime because it gets altered by the method
 if(designmatrix != NULL)
 {
  delete designmatrix;
  designmatrix = NULL;
 }
 
 designmatrix = new Matrix2DV2N4<T>(A);
 
 NRvector<double> yy(yvec.getSIZE());
 NRvector<double> xx(yvec.getSIZE());
 NRvector<double> ssig(sigvec.getSIZE());

 for(int idx = 0; idx < xx.size(); idx++)
  xx[idx] = idx;

 NumericalRecipesV1N2<T>::convertArrayToNRVector(yvec.getConstDataReference(),yy,yvec.getSIZE());
 NumericalRecipesV1N2<T>::convertArrayToNRVector(sigvec.getConstDataReference(),ssig,sigvec.getSIZE());
 
 NRvector<double> (*fptr)(const double);
 fptr = &funks;

 Fitsvd myfit(xx,yy,ssig,fptr);
 myfit.fit();

 VectorV2N4<T> coeffs((myfit.a).size());

 for(int idx = 0; idx < coeffs.getSIZE(); idx++)
 {
  coeffs.setElement(idx,(myfit.a)[idx]);
 }

 return coeffs;


 
}

//UTILITY FUNCTIONS

template<class T>
NRvector<double> NumericalRecipesV1N2<T>::funks(const double x)
//this is a utility function used for the SVD method
{
 //x is assumed to be the row index of the design matrix of the current object, which is also assumed to be defined.
 NRvector<double> result(designmatrix->getCDIM());
 double *ptr = new double[designmatrix->getCDIM()];  //the basis functions are assumed to run over the columns

 designmatrix->getMatrixRow(ptr,x);

 NumericalRecipesV1N2<double>::convertArrayToNRVector(ptr,result,designmatrix->getCDIM());

 delete[] ptr;

 return result;
}


template<class T>
class RicianNoiseV1N2 : public NoiseV1N2<T>{

public:
 //CONSTRUCTORS
 RicianNoiseV1N2();  //default empty constructor
 RicianNoiseV1N2(int seed);  //initialize the generator with seed = time(NULL)

 //GENERATORS
 T generateRicianRandomNumber(T sigamp, T stddev);  //Normal distribution with mean and standard deviation stddev. This distribution simulates the sum of many random factors. 
 //Definition: f(x) = (2*pi*s2)-0.5*exp(-0.5*s-2*(x-m)2).  The generator returns a double, which is internally converted to type T.

protected:
 StochasticLib1 *stolib;  //random number generator



};


template<class T>
RicianNoiseV1N2<T>::RicianNoiseV1N2()
//default empty constructor
:NoiseV1N2<T>()
{
 stolib = NULL;
}

template<class T>
RicianNoiseV1N2<T>::RicianNoiseV1N2(int seed)
//initialize the generator with seed = time(NULL)
:NoiseV1N2<T>()
{
 
 stolib = new StochasticLib1(seed);


}


template<class T>
T RicianNoiseV1N2<T>::generateRicianRandomNumber(T sigamp, T stddev)
//Normal distribution with mean and standard deviation stddev. This distribution simulates the sum of many random factors. Definition: f(x) = (2*pi*s2)-0.5*exp(-0.5*s-2*(x-m)2).  
//The generator returns a double, which is internally converted to type T.
{
 NumericalRecipesV1N2<> numr;
 T result = numr.generateRiceNumber(sigamp,stddev);
 return result;

}



template<class T>
class LeviathanFileHeaderV1N2 : public ImageVolumeHeaderV1N2<T>{

public:
 //CONSTRUCTORS
 LeviathanFileHeaderV1N2();  //default empty constructor
 LeviathanFileHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, DataType dtv, int bitspervoxelv, bool byteswapv, int RDIMv, int CDIMv, int ZDIMv, double bvalv, double gnumv[6], int anumv = 1, int offset = 0, double RLv = 1.0, double CLv = 1.0, double ZLv = 1.0);  //constructor which assigns values
 LeviathanFileHeaderV1N2(const FileNameV1N1<> *fnamev); //constructor which assigns the values based on a .lev header file
 LeviathanFileHeaderV1N2(const ImageVolumeHeaderV1N2<T> *hdr, int anumv, double bvalv, const double gnumv[]);  //create a leviathan file header from an image volume file header
 LeviathanFileHeaderV1N2(const LeviathanFileHeaderV1N2<T> &copy); //copy constructor
 
 //ASSIGNMENT FUNCTIONS
 void initFromImageVolumeHeader(const ImageVolumeHeaderV1N2<T> *hdr, int anumv, double bvalv, const double gnumv[]);  //create a leviathan file header from a analyze volume file header

 //PRINT FUNCTIONS
 virtual void printHeader() const;  //prints the header information, one value per line

 //GET FUNCTIONS
 const int getANum() const {return anum;}
 const double getBVal() const {return bval;}
 const double* getGNum() const {return gnum;}

 //SET FUNTIONS
 void setANum(int a) {anum = a;}
 void setBVal(double b) {bval = b;}
 void setGNum(double g[]); 

 //READ/WRITE FUNCTIONS
 virtual void readInHeaderFile(const FileNameV1N1<> *fnamev);  //reads in a .lev file
 virtual void writeOutHeaderFile(const FileNameV1N1<> *fnamev) const;  //writes out as a .lev file
 void parseLevFileHeaderLine(const std::string &flag, const std::string &value);  //a utility function used by readInLevFileHeader to convert and assign each line to
 //the correct variable

protected:
 int anum;  //the acquisition number
 double bval;  //the b value
 double gnum[6]; //the gradient coordinate vector: [gxx, gyy, gzz, gxy, gxz, gyz]
 

};

//CONSTRUCTORS

template<class T>
LeviathanFileHeaderV1N2<T>::LeviathanFileHeaderV1N2()
//empty default constructor
:ImageVolumeHeaderV1N2<T>(), anum(0), bval(0)
{
 //sets the acquisition, b-value, and gradient direction to zero
 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = 0.0;

}

template<class T>
LeviathanFileHeaderV1N2<T>::LeviathanFileHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, DataType dtv, int bitspervoxelv, bool byteswapv, int RDIMv, int CDIMv, int ZDIMv, double bvalv, double gnumv[6], int anumv, int offsetv, double RLv, double CLv, double ZLv)
//constructor which assigns values
:ImageVolumeHeaderV1N2<T>(fnamev, ifv,dtv,bitspervoxelv,byteswapv,RDIMv,CDIMv,ZDIMv,offsetv,RLv,CLv,ZLv), anum(anumv), bval(bvalv)
{
 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = gnumv[idx];

}

template<class T>
LeviathanFileHeaderV1N2<T>::LeviathanFileHeaderV1N2(const FileNameV1N1<> *fnamev)
//constructor which assigns the values based on a .lev header file
:ImageVolumeHeaderV1N2<T>()
{
 this->readInHeaderFile(fnamev);

}

template<class T>
LeviathanFileHeaderV1N2<T>::LeviathanFileHeaderV1N2(const ImageVolumeHeaderV1N2<T> *hdr, int anumv, double bvalv, const double gnumv[])
//create a leviathan file header from a analyze volume file header
:ImageVolumeHeaderV1N2<T>(hdr->getFNamePtr(), hdr->getImageFileFormat(), hdr->getDataType(), hdr->getBitsPerVoxel(), hdr->getShouldByteSwap(), hdr->getRDIM(), hdr->getCDIM(), hdr->getZDIM(), hdr->getOffset(), hdr->getRLENGTH(), hdr->getCLENGTH(), hdr->getZLENGTH()), anum(anumv), bval(bvalv)
{
 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = gnumv[idx];


}
 

template<class T>
LeviathanFileHeaderV1N2<T>::LeviathanFileHeaderV1N2(const LeviathanFileHeaderV1N2<T> &copy) 
//copy constructor
:ImageVolumeHeaderV1N2<T>(copy)
{
 anum = copy.getANum();
 bval = copy.getBVal();

 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = (copy.getGNum())[idx];


}
 

//ASSIGNMENT FUNCTIONS
template<class T>
void LeviathanFileHeaderV1N2<T>::initFromImageVolumeHeader(const ImageVolumeHeaderV1N2<T> *hdr, int anumv, double bvalv, const double gnumv[])
//create a leviathan file header from an Analyze volume file header
{
 this->setFName(hdr->getFNamePtr());

 //copies all the base class data from the Analyze header
 this->imgformat = hdr->getImageFileFormat();
 this->bitspervoxel = hdr->getBitsPerVoxel();
 this->datatype = hdr->getDataType();
 this->RDIM = hdr->getRDIM();  //the volume/slice dimensions
 this->CDIM = hdr->getCDIM();
 this->ZDIM = hdr->getZDIM();
 this->RLENGTH = hdr->getRLENGTH();  //the voxels lengths in mm (default to 1 mm)
 this->CLENGTH = hdr->getCLENGTH();
 this->ZLENGTH = hdr->getZLENGTH();
 this->shouldbyteswap = hdr->getShouldByteSwap();
 this->offset = hdr->getOffset();

 //copies the DWI data
 anum = anumv;
 bval = bvalv;
 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = gnumv[idx];


}


//PRINT FUNCTION

template<class T>
void LeviathanFileHeaderV1N2<T>::printHeader() const
//reimplementation of the base class data that prints the header information, one value per line
{
 ImageVolumeHeaderV1N2<T>::printHeader();  //calls the base class function

 std::cout<<"Acquisition Number: "<<anum<<std::endl;
 std::cout<<"B Value: "<<bval<<std::endl;
 std::cout<<"Gradient Direction: ";

 for(int idx = 0; idx < 6; idx++)
  std::cout<<gnum[idx]<<" ";

 std::cout<<std::endl;
 

}

//SET FUNCTIONS
template<class T>
void LeviathanFileHeaderV1N2<T>::setGNum(double g[])
//sets the gradient vector from the array
{
 for(int idx = 0; idx < 6; idx++)
  gnum[idx] = g[idx];

}

//READ/WRITE FUNCTIONS
template<class T>
void LeviathanFileHeaderV1N2<T>::readInHeaderFile(const FileNameV1N1<> *fnamev)
//reads in a .lev file
{

#if VERBOSE
 std::cout<<"Reading In LEV file = "<<fnamev->getOSCompatibleFileName()<<std::endl;
#endif

 ReadFromFileV1N1<> ifile(fnamev);
 ifile.openForRead();
 std::string textline(ifile.readProcessedCurrentLineFromStream());  //reads in the first line from the stream
 
 if(textline != "#!LEV")  //this is a required character string that denotes a .lev file
 {
#if ERRORCHECK
  std::cout<<"ERROR in void LeviathanFileHeaderV1N2<T>::readInLevFileHeader(std::string ifname)!"<<std::endl;
  std::cout<<"This is not a valid .lev file.  Must contain the #!LEV tag as the first line. Exiting..."<<std::endl;
  exit(1);
#endif

  std::cout<<"WARNING in void LeviathanFileHeaderV1N2<T>::readInLevFileHeader(std::string ifname)!"<<std::endl;
  std::cout<<"This is not a valid .lev file.  Must contain the #!LEV tag as the first line.  Trying to proceed..."<<std::endl;

 }

 while(!(ifile.reachedEOF()))  //tests whether the EOF has been reached.  If not, keep reading in lines
 {
  textline.clear();
  textline = ifile.readProcessedCurrentLineFromStream();
  
  if(textline.empty())  //skip any empty lines
   continue;
  else if(textline.find("-") != std::string::npos) //checks to make sure the line contains a valid flag that begins with a dash.  Remember that
  //std::string::npos is what is returned if a string does not find the searched text successfully
  {
   //parses the current line into the flag and the value
   std::string flag(textline.substr(0, (textline.find(" "))));  //gets the next flag, which is terminated by a space, which separates it from the value
   std::string value(textline.substr((textline.find(" ")) + 1));
   this->parseLevFileHeaderLine(flag,value);  //based on the flag, converts value and assigns it to the right variable
  }
  else  //skips any invalid lines
  {
#if VERBOSE
   std::cout<<"Invalid Line: "<<textline<<std::endl;
   std::cout<<"Text Line = "<<textline<<std::endl;
   std::cout<<"This line does not contain a valid flag.  Skipping..."<<std::endl;
#endif
   
  }
 }

 (this->getFNamePtrNonConstReference())->setPath(fnamev->getPath());  //the lev file FNAME field has no path associated with it, so the path is set to the path from fnamev
 
 ifile.close();


}

template<class T>
void LeviathanFileHeaderV1N2<T>::writeOutHeaderFile(const FileNameV1N1<> *fnamev) const
//writes out the header to a .lev file 
{

#if VERBOSE
 std::cout<<"Writing Out LEV file = "<<fnamev->getOSCompatibleFileName()<<std::endl;
#endif

 char buffer[_CVTBUFSIZE];

 std::ofstream ofile((fnamev->getOSCompatibleFileName()).c_str(), std::ios::out);
 
 ofile.fixed;
 ofile.showpoint;
 ofile.precision(6);
 
 std::cout.fixed;
 std::cout.showpoint;
 std::cout.precision(6);

 //.lev file tag
 std::string levtag("#!LEV");
 ofile << levtag;
 ofile << "\n";

 //Filename
 std::string fnamestr("-FNAME ");
 fnamestr += (fnamev->getFileNamePlusExtension());  //in order to make the filenames more universal, i am not going to write out the paths
 ofile << fnamestr;
 ofile << "\n";

 //ImageFileFormat
 std::string imageformatstr("-ImageFileFormat ");
 sprintf(buffer,"%d",static_cast<int>(this->imgformat));
 imageformatstr += std::string(buffer);
 ofile << imageformatstr;
 ofile << "\n";

 //DataType
 std::string datatypestr("-DataType ");
 sprintf(buffer,"%d",static_cast<int>(this->datatype));
 datatypestr += std::string(buffer);
 ofile << datatypestr;
 ofile << "\n";

 //BitsPerVoxel
 std::string bitspervoxelstr("-BitsPerVoxel ");
 sprintf(buffer,"%d",static_cast<int>(this->bitspervoxel));
 bitspervoxelstr += std::string(buffer);
 ofile << bitspervoxelstr;
 ofile << "\n";

 //Shouldbyteswap
 std::string byteswapstr("-ByteSwap ");
 sprintf(buffer,"%d",static_cast<int>(this->shouldbyteswap));
 byteswapstr += std::string(buffer);
 ofile << byteswapstr;
 ofile << "\n";

 //XDIM
 std::string xdimstr("-XDIM ");
 sprintf(buffer,"%d",static_cast<int>(this->getRDIM()));
 xdimstr += std::string(buffer);
 ofile << xdimstr;
 ofile << "\n";

 //YDIM
 std::string ydimstr("-YDIM "); 
 sprintf(buffer,"%d",static_cast<int>(this->getCDIM()));
 ydimstr += std::string(buffer);
 ofile << ydimstr;
 ofile << "\n";
 
 //ZDIM
 std::string zdimstr("-ZDIM ");
 sprintf(buffer,"%d",static_cast<int>(this->getZDIM()));
 zdimstr += std::string(buffer);
 ofile << zdimstr;
 ofile << "\n";

 //XLENGTH
 std::string xlengthstr("-XLENGTH ");
 sprintf(buffer,"%5.5f",static_cast<float>(this->getRLENGTH()));
 xlengthstr += std::string(buffer);
 addATrailingZeroIfNeeded(xlengthstr);
 ofile << xlengthstr;
 ofile << "\n";

 //YLENGTH
 std::string ylengthstr("-YLENGTH ");
 sprintf(buffer,"%5.5f",static_cast<float>(this->getCLENGTH()));
 ylengthstr += std::string(buffer);
 addATrailingZeroIfNeeded(ylengthstr);
 ofile << ylengthstr;
 ofile << "\n";

 //ZLENGTH
 std::string zlengthstr("-ZLENGTH ");
 sprintf(buffer,"%5.5f",static_cast<float>(this->getZLENGTH()));
 zlengthstr += std::string(buffer);
 addATrailingZeroIfNeeded(zlengthstr);
 ofile << zlengthstr;
 ofile << "\n";

 //ANUM
 std::string anumstr("-AcquisitionNumber ");
 sprintf(buffer,"%d",static_cast<int>(this->anum));
 anumstr += std::string(buffer);
 ofile << anumstr;
 ofile << "\n";
 
 //BVAL
 std::string bvalstr("-BValue ");
 sprintf(buffer,"%5.5f",static_cast<float>(this->bval));
 bvalstr += std::string(buffer);
 addATrailingZeroIfNeeded(bvalstr);
 ofile << bvalstr;
 ofile << "\n";

 //GNUM
 std::string gnumtag("-GradientDirection ");
 ofile << gnumtag;
 std::string gnumstr;
 const double *gptr = this->getGNum();
 for(int idx = 0; idx < 6; idx++)
 {
  sprintf(buffer,"%5.5f",static_cast<float>(gptr[idx]));
  gnumstr = std::string(buffer);
  addATrailingZeroIfNeeded(gnumstr);
  ofile << gnumstr;
  ofile << " ";
 }
 ofile << "\n";

 ofile.close();
 //delete fptr;


}

template<class T>
void LeviathanFileHeaderV1N2<T>::parseLevFileHeaderLine(const std::string &flag, const std::string &value)
//parses a value line that corresponds to a valid .lev file header
{
 //constructing an if/else statement block so that the value can be assigned to the right variable
 if(flag == "-XDIM")
  this->RDIM = atoi(value.c_str());
 else if(flag == "-YDIM")
  this->CDIM = atoi(value.c_str());
 else if(flag == "-ZDIM")
  this->ZDIM = atoi(value.c_str());
 else if(flag == "-XLENGTH")
  this->RLENGTH = atof(value.c_str());
 else if(flag == "-YLENGTH")
  this->CLENGTH = atof(value.c_str());
 else if(flag == "-ZLENGTH")
  this->ZLENGTH = atof(value.c_str());
 else if(flag == "-FNAME")
 {
  //deletes any existing fname data
  if((this->fname) != NULL)
  {
   delete (this->fname);
   (this->fname) = NULL;
  }

  (this->fname) = hageman::constructFileNameObjFromString(value);
 }
 else if(flag == "-ImageFileFormat")
  this->imgformat = static_cast<ImageFileFormat>(atoi(value.c_str()));
 //else if(flag == "-ImageType")
 // this->imgtype = static_cast<ImageType>(atoi(value.c_str()));
 else if(flag == "-DataType")
  this->datatype = static_cast<DataType>(atoi(value.c_str()));
 else if(flag == "-BitsPerVoxel")
  this->bitspervoxel = (atoi(value.c_str()));
 else if(flag == "-ByteSwap")
  this->shouldbyteswap = static_cast<bool>(atoi(value.c_str()));
 else if(flag == "-AcquisitionNumber")
  anum = atoi(value.c_str());
 else if(flag == "-BValue")
  bval = atof(value.c_str());
 else if(flag == "-GradientDirection")
 {
  std::string delim(" ");
  parseLineOfNumbers<double>(gnum,6,value,delim);  //the gradient coordinate vector: [gxx, gyy, gzz, gxy, gxz, gyz]
 }
#if VERBOSE
 else  //skips any invalid flags
  std::cout<<"This line does not contain a valid flag.  Skipping..."<<std::endl;
#endif
 
 
}




//absolute key file positions within the header file (in bytes) [unique to Analyze header format]
#define DIM0_FILEPOS 40
#define XDIM_FILEPOS 42
#define VOLDIM_FILEPOS 50
#define INTENTCODE_FILEPOS 68
#define DATATYPE_FILEPOS 70
#define XLENGTH_FILEPOS 80
#define VOXOFFSET_FILEPOS 108
#define GLMAX_FILEPOS 140
#define INTENTNAME_FILEPOS 328
#define NIFTISTR_FILEPOS 344
#define EXTFLAG_FILEPOS 348

template<class T>
class NiftiVolumeHeaderV1N2 : public ImageVolumeHeaderV1N2<T>{

public:
 //CONSTRUCTORS
 NiftiVolumeHeaderV1N2();  //default empty constructor
 NiftiVolumeHeaderV1N2(const FileNameV1N1<> *fnamev);  //constructs an object from a .hdr file
 //NiftiVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, ImageType itv, DataType dtv, int bitspervoxel, bool shouldbyteswap, int RDIMv, int CDIMv, int ZDIMv, double RLv = 1.0, double CLv = 1.0, double ZLv = 1.0);  //default empty constructor
 NiftiVolumeHeaderV1N2(const NiftiVolumeHeaderV1N2<T> &copy); //copy constructor
 
 //GET FUNCTIONS
 
 //PRINT FUNCTIONS
 virtual void printHeader() const;  //prints the header information, one value per line
 
 //READ FUNCTIONS
 virtual void readInHeaderFile(const FileNameV1N1<> *fnamev); //read in the header file, fnamev

 //WRITE FUNCTIONS
 virtual void writeOutHeaderFile(const FileNameV1N1<> *fnamev) const;  //writes out the header file 

 //void readInCombinedHeaderFile(const FileNameV1N1<> *fnamev); //read in the header file, fnamev

protected:
 double bval;  //the b value
 double gnum[3]; //the gradient coordinate vector: [gx, gy, gz]
 int numvols;  //the number of DWI data volumes
 
 //double voxoffset;  //the offset that defines the first byte of the image data in a combined Nifti file

};

//CONSTRUCTORS
template<class T>
NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2()
//empty default constructor
:ImageVolumeHeaderV1N2<T>(), bval(0), numvols(0)
{
 for(int idx = 0; idx < 3; idx++)
  gnum[idx] = 0;

}

//template<class T>
//NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, ImageType itv, DataType dtv, int bitspervoxelv, bool shouldbyteswapv,int RDIMv, int CDIMv, int ZDIMv, double RLv, double CLv, double ZLv)  
//constructor that assigns all arguments
//:ImageVolumeHeaderV1N2<T>(fnamev,ifv,itv,dtv,bitspervoxelv,shouldbyteswapv,RDIMv,CDIMv,ZDIMv,RLv,CLv,ZLv), voxoffset(0.0)
//{
 
//}

template<class T>
NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(const FileNameV1N1<> *fnamev)  
//constructor that assigns all arguments based on an Analyze header file
:ImageVolumeHeaderV1N2<T>()
{
 this->readInHeaderFile(fnamev);

}



template<class T>
NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(const NiftiVolumeHeaderV1N2<T> &copy) 
//copy constructor
:ImageVolumeHeaderV1N2<T>(copy)
{

}
 

//PRINT FUNCTIONS
template<class T>
void NiftiVolumeHeaderV1N2<T>::printHeader() const
//print header function
{
 ImageVolumeHeaderV1N2<T>::printHeader();
 std::cout<<"B Val: "<<this->bval<<std::endl;
 std::cout<<"Gradient Direction: ("<<this->gnum[0]<<","<<this->gnum[1]<<","<<this->gnum[2]<<")"<<std::endl;
 
}


//READ FUNCTIONS
template<class T>
void NiftiVolumeHeaderV1N2<T>::readInHeaderFile(const FileNameV1N1<> *fnamev)  
//reads in an Analyze header file, fname
{
 
 //deletes the existing fname, if any
 if((this->fname) != NULL)
 {
  delete (this->fname);
  (this->fname) = NULL;
 }

 (this->fname) = (fnamev->clone());
 
 //if the input filename doesn't have a .nii extension, tries to open a .nii file with the same name
 //if(((this->fname)->getExtension()) != "nii")
 //(this->fname)->setExtension("nii");
 
 #if VALIDATE
  std::cout<<"From function void NiftiVolumeHeaderV1N2<T>::readInHeaderFile(FileNameV1N1<> *fnamev, CheckStatus cstat)"<<std::endl;  
  std::cout<<"Filename: "<<(this->fname)->getOSCompatibleFileName()<<std::endl;
 #endif


 this->imgformat = NIFTI;
  
 ReadFromBinaryFileV1N1<std::string> hdrifile(this->fname);
 hdrifile.openForRead();

 char twobytebuffer[2];
 char fourbytebuffer[4];
 int *intptr;
 signed short int *shortptr;
 float *floatptr;
 char eightbytebuffer[8];
 char sixteenbytebuffer[16];

 //the first 4 bytes specify the size of the ANALYZE file header (must be 348) and is used to determine if the file is byte swapped
 hdrifile.readFromFileStream(fourbytebuffer,4);
 intptr = reinterpret_cast<int*>(fourbytebuffer);  //size of the header is stored as an int (int32)
 char swapbuffer[4];
 for(int idx = 0; idx < 4; idx++)
  swapbuffer[idx] = fourbytebuffer[idx];

 byteswap<char>(swapbuffer,4);
 int *swapintptr = reinterpret_cast<int*>(swapbuffer);
 
 if(*intptr == static_cast<int>(348))
  this->shouldbyteswap = false;
 else if(*swapintptr == static_cast<int>(348))
  this->shouldbyteswap = true;
//#if ERRORCHECK
 else
 {
  std::cout<<"ERROR in NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
  std::cout<<"This is not a valid NIFTI header file!  1st 4 bytes not equal to 348.  Exiting..."<<std::endl;
  exit(1);
 }
//#endif

 //the dimensions of the volume
 //how about dims[0] - the number of dims; check for 3

 //for MiND data, DIM0 should be equal to 5
 hdrifile.placeGetStreamAtAbsolutePosition(DIM0_FILEPOS);
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 if(*shortptr == 5)  //MiND specifies 5 dimensions
  std::cout<<"This is a MiND data file"<<std::endl;

 hdrifile.placeGetStreamAtAbsolutePosition(XDIM_FILEPOS);

 //XDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setRDIM(*shortptr);  
  
 //YDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setCDIM(*shortptr);  
 
 //ZDIM
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->setZDIM(*shortptr);
 
 //NUMVOLUMES
 hdrifile.placeGetStreamAtAbsolutePosition(VOLDIM_FILEPOS);
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<signed short int*>(twobytebuffer);
 this->numvols = *shortptr;
 std::cout<<"There are "<<this->numvols<<" volumes in this Nifti file"<<std::endl;
 
 //INTENT CODE
 hdrifile.placeGetStreamAtAbsolutePosition(INTENTCODE_FILEPOS);
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<short int*>(twobytebuffer);
 if(*shortptr == 1007)
  std::cout<<"This is the proper MiND intent code"<<std::endl;
 
 //datatype
 hdrifile.placeGetStreamAtAbsolutePosition(DATATYPE_FILEPOS);

 hdrifile.readFromFileStream(twobytebuffer,2);
 if(this->shouldbyteswap)
  byteswap<char>(twobytebuffer,2);
 shortptr = reinterpret_cast<short int*>(twobytebuffer);

#if VALIDATE
 std::cout<<"Anlz specific Datatype No. of Anlz hdr read from file: "<<*shortptr<<std::endl;
#endif
 
 switch(*shortptr)
 {
  case 0:
   this->datatype = UNKNOWNTYPE;
   break;
  case 1:  //Analyze format specifies 1 as a binary format
   this->datatype = UINT8TYPE;
   break;
  case 2:  //Analyze format specifies 2 as unsigned char, so i made that the default (avoid using char)
   if(hageman::getDataType(T()) == SINT8TYPE)
    this->datatype = SINT8TYPE;
   else
    this->datatype = UINT8TYPE;
   break;
  case 4:  //Analyze format specifies 4 as a signed short format, so i made that the default
   if(hageman::getDataType(T()) == UINT16TYPE)
    this->datatype = UINT16TYPE;
   else
    this->datatype = SINT16TYPE;
   break;
  case 8:
   this->datatype = SINT32TYPE;
   break;
  case 16:
   this->datatype = FLOATTYPE;
   break;
  case 64:
   this->datatype = DOUBLETYPE;
   break;
  case 128:
   this->datatype = RGBTYPE;
   break;
#if ERRORCHECK
  default:
  {
   std::cout<<"ERROR in NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
   std::cout<<"Not a valid datatype.  Exiting..."<<std::endl;
   std::cout<<"Datatype: "<<this->datatype<<std::endl;
   exit(1);
   break;
  }
#endif
 }

 

 if(hageman::getDataType(T()) != this->datatype)  //can't make this a requirement because static functions need to be called when the datatype is unknown
 {

#if VERBOSE
  std::cout<<"WARNING: Reading fname = "<<fnamev->getOSCompatibleFileName()<<std::endl;
  std::cout<<"Datatype of hdr file = "<<hageman::convertDataTypeToString(this->datatype)<<" does not match datatype specified = "<<hageman::convertDataTypeToString(hageman::getDataType(T()))<<std::endl;
  std::cout<<"NiftiVolumeHeaderV1N2<T>::NiftiVolumeHeaderV1N2(std::string fnamev)"<<std::endl;
#endif

 }
 

 //bitspervoxel
 hdrifile.readFromFileStream(twobytebuffer,2);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(twobytebuffer,2);
 shortptr = reinterpret_cast<short int*>(twobytebuffer);
 this->setBitsPerVoxel(static_cast<int>(*shortptr));

 //addendum to adjust the datatype if it is UNKNOWN; goes by the template argument and the bitspervoxel
 if(this->datatype == UNKNOWNTYPE)
 {
  if(this->bitspervoxel == 32)
  {
   if(hageman::getDataType(T()) == SINT32TYPE)
    this->datatype = SINT32TYPE;
   else
    this->datatype = UINT32TYPE;
  }
  else if(this->bitspervoxel == 16)
  {
   if(hageman::getDataType(T()) == SINT16TYPE)
    this->datatype = SINT16TYPE;
   else
    this->datatype = UINT16TYPE;
  }
  else if(this->bitspervoxel == 8)
  {
   if(hageman::getDataType(T()) == SINT8TYPE)
    this->datatype = SINT8TYPE;
   else if(hageman::getDataType(T()) == UINT8TYPE)
    this->datatype = UINT8TYPE; 
   else
    this->datatype = CHARTYPE;
  }
 }
  
 
 //hdrifile.seekg(XLENGTH_FILEPOS);
 hdrifile.placeGetStreamAtAbsolutePosition(XLENGTH_FILEPOS);


 //XLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setRLENGTH(static_cast<double>(*floatptr));  //the XLENGTH is equal to the CLENGTH in matrix coordinates
 
 //YLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setCLENGTH(static_cast<double>(*floatptr));  //the YLENGTH is equal to the RLENGTH in matrix coordinates
 
 //ZLENGTH
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->setZLENGTH(static_cast<double>(*floatptr));
 
 //VOXEL OFFSET
 hdrifile.placeGetStreamAtAbsolutePosition(VOXOFFSET_FILEPOS);
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 this->offset = static_cast<double>(*floatptr);

 //INTENT NAME
 hdrifile.placeGetStreamAtAbsolutePosition(INTENTNAME_FILEPOS);
 hdrifile.readFromFileStream(sixteenbytebuffer,16);
 if(this->shouldbyteswap)
  byteswap<char>(sixteenbytebuffer,16);
 //char* intentptr = reinterpret_cast<char*>(sixteenbytebuffer);
 std::cout<<"The intent name is "<<sixteenbytebuffer[0]<<sixteenbytebuffer[1]<<sixteenbytebuffer[2]<<sixteenbytebuffer[3]<<std::endl;

  
 //NIFTI STRING
 hdrifile.placeGetStreamAtAbsolutePosition(NIFTISTR_FILEPOS);
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);

//#if ERRORCHECK
// std::string magicstr(fourbytebuffer);
// if((magicstr != "n+1") && (magicstr != "ni1"))
// {
//  std::cout<<"ERROR in void NiftiVolumeHeaderV1N2<T>::readInCombinedHeaderFile(const FileNameV1N1<> *fnamev)"<<std::endl;
//  std::cout<<"This is not a valid Nifti file!  Exiting..."<<std::endl;
//  exit(1);
// }
//#endif

 //EXTENSION TAG
 hdrifile.readFromFileStream(fourbytebuffer,4);
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 if(fourbytebuffer[0] != '0')
  std::cout<<"There is extended header information"<<std::endl;

 //for raw dwi volumes

 hdrifile.placeGetStreamAtAbsolutePosition(356);
 hdrifile.readFromFileStream(fourbytebuffer,4); //for processing code
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 intptr = reinterpret_cast<int*>(fourbytebuffer);
 std::cout<<"The Processing Code is: "<<*intptr<<std::endl;


 hdrifile.placeGetStreamAtAbsolutePosition(372);
 hdrifile.readFromFileStream(fourbytebuffer,4); //for processing code
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 intptr = reinterpret_cast<int*>(fourbytebuffer);
 std::cout<<"BVal ECode: "<<*intptr<<std::endl;


 //FOR NOW, ASSUME A SINGLE NIFTI FILE
//for(int idx = 0; idx < 35; idx++)
//{

 //if(idx != 0)
 // hdrifile.skipBytes(8);
 
 //hdrifile.readFromFileStream(eightbytebuffer,8); //for b val
 //if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
 // byteswap(eightbytebuffer,8);
 //for(int bidx = 0; bidx < 4; bidx++)
 // fourbytebuffer[bidx] = eightbytebuffer[bidx];
 hdrifile.readFromFileStream(fourbytebuffer,4); //for processing code
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 (this->bval) = static_cast<double>(*floatptr);
 std::cout<<"BVal: "<<this->bval<<std::endl;

 //hdrifile.placeGetStreamAtAbsolutePosition(392);  
 hdrifile.skipBytes(12);

 hdrifile.readFromFileStream(fourbytebuffer,4); //for azimuthal angle
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 float azimuth = (*floatptr);

 hdrifile.readFromFileStream(fourbytebuffer,4);  //for polar angle
 if(ImageVolumeHeaderV1N2<T>::shouldbyteswap)
  byteswap(fourbytebuffer,4);
 floatptr = reinterpret_cast<float*>(fourbytebuffer);
 float polar = (*floatptr);

 std::cout<<"Spherical Coords are: azimuth = "<<azimuth<<"; polar = "<<polar<<std::endl;
 //convert spherical coords to cartesian coords (set r=1)
 //x = r cos(azi)sin(theta)
 //y = r sin(azi)sin(theta)
 //z = r cos(theta)

 this->gnum[0] = cos(azimuth)*sin(polar);
 this->gnum[1] = sin(azimuth)*sin(polar);
 this->gnum[2] = cos(polar);

 std::cout<<"Gnum = ("<<this->gnum[0]<<","<<this->gnum[1]<<","<<this->gnum[2]<<")"<<std::endl;

//}


 hdrifile.close();
 
 
}


//WRITE FUNCTIONS

template<class T>
void NiftiVolumeHeaderV1N2<T>::writeOutHeaderFile(const FileNameV1N1<> *fnamev) const  
//writes out the header file.  assumes that fnamev corresponds to a combined nifti file 
{

 #if VERBOSE
 std::cout<<"Writing Out Nifti Image File = "<<fnamev->getOSCompatibleFileName()<<std::endl;
#endif

 WriteToBinaryFileV1N1<> hdrofile(fnamev);
 hdrofile.openForWrite();
  
 char *writebuffer = NULL;
 int inttag = 0;
 int *intptr = NULL;
 char chartag;
 short shorttag = 0;
 short *shortptr = NULL;
 float floattag = 0.0f;
 float *floatptr = NULL;

 //the first 4 bytes specify the size of the ANALYZE file header (must be 348) and is used to determine if the file is byte swapped
 inttag = 348;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 //hdrofile.placePutStreamAtAbsolutePosition(EXTENTS_FILEPOS);  //NOT USED IN THE NIFTI HEADER

 //inttag = 16384;
 //intptr = &inttag;
 //writebuffer = reinterpret_cast<char *>(intptr);
 //if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
 // byteswap<char>(writebuffer,4);
 //hdrofile.writeBytesToFileStream(writebuffer,4);
 
 //hdrofile.placePutStreamAtAbsolutePosition(REGULAR_FILEPOS);  //NOT USED IN THE NIFTI HEADER
 //chartag = 'r';
 //writebuffer = &chartag;
 //hdrofile.writeBytesToFileStream(writebuffer,1);
 

 hdrofile.placePutStreamAtAbsolutePosition(DIM0_FILEPOS);
 
 //DIMS
 shorttag = 5;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);

 //XDIM
 shorttag = static_cast<short>(this->getRDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //YDIM
 shorttag = static_cast<short>(this->getCDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(ImageVolumeHeaderV1N2<T>::getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //ZDIM
 shorttag = static_cast<short>(this->getZDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //TIME
 shorttag = 1;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //NUM VOLUMES  (assume to be 1 for now)
 shorttag = 1;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);

 hdrofile.placePutStreamAtAbsolutePosition(INTENTCODE_FILEPOS);
 
 //INTENT CODE
 shorttag = 1007;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);

 hdrofile.placePutStreamAtAbsolutePosition(DATATYPE_FILEPOS);
 
 short int dtypetag = 0;
 
 switch(this->getDataType())
 {
  case UNKNOWNTYPE:
   dtypetag = 0;
   break;
  case CHARTYPE:
  case UINT8TYPE:
  case SINT8TYPE:
   dtypetag = 2;
   break;
  case SINT16TYPE:
  case UINT16TYPE:
   dtypetag = 4;
   break;
  case SINT32TYPE:
  case UINT32TYPE:
   dtypetag = 8;
   break;
  case FLOATTYPE:
   dtypetag = 16;
   break;
  case EIGENTYPE:
   dtypetag = 21;
   break;
  case DOUBLETYPE:
   dtypetag = 64;
   break;
  case RGBTYPE:
   dtypetag = 128;
   break;
#if ERRORCHECK
  default:
    std::cout<<"ERROR in void AnalyzeVolumeHeaderV1N2<T>::writeOutHeaderFile() const"<<std::endl;
    std::cout<<"Not a valid datatype.  Exiting..."<<std::endl;
    std::cout<<"Datatype: "<<this->getDataType()<<std::endl;
    exit(1);
    break;
#endif
    
  }

 
 //DATATYPE
 shortptr = &dtypetag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);


 //BITS PER VOXEL
 shorttag = static_cast<short>(ImageVolumeHeaderV1N2<T>::getBitsPerVoxel());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 hdrofile.placePutStreamAtAbsolutePosition(XLENGTH_FILEPOS);
 
 //XLENGTH
 floattag = (static_cast<float>(this->getRLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //YLENGTH
 floattag = (static_cast<float>(this->getCLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //ZLENGTH
 floattag = (static_cast<float>(this->getZLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 hdrofile.placePutStreamAtAbsolutePosition(VOXOFFSET_FILEPOS);
 
 //VOX OFFSET
 floattag = 400.0f;  //CORRESPONDS TO 1 VOLUME MIND DWI EXTENDED HEADER - HACK!
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 //INTENT NAME
 hdrofile.placePutStreamAtAbsolutePosition(328);
 char intentbuffer[16];
 intentbuffer[0] = 'M';
 intentbuffer[1] = 'i';
 intentbuffer[2] = 'N';
 intentbuffer[3] = 'D';
 intentbuffer[4] = '\0';
 intentbuffer[5] = '\0';
 intentbuffer[6] = '\0';
 intentbuffer[7] = '\0';
 intentbuffer[8] = '\0';
 intentbuffer[9] = '\0';
 intentbuffer[10] = '\0';
 intentbuffer[11] = '\0';
 intentbuffer[12] = '\0';
 intentbuffer[13] = '\0';
 intentbuffer[14] = '\0';
 intentbuffer[15] = '\0';
 writebuffer = reinterpret_cast<char *>(intentbuffer);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,16);
 hdrofile.writeBytesToFileStream(writebuffer,16);


 //NIFTI STRING
 hdrofile.placePutStreamAtAbsolutePosition(NIFTISTR_FILEPOS);
 char nifstrbuffer[4];
 nifstrbuffer[0] = 'n';
 nifstrbuffer[1] = '+';
 nifstrbuffer[2] = '1';
 nifstrbuffer[3] = '\0';
 writebuffer = reinterpret_cast<char *>(nifstrbuffer);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 //EXTENSION TAG
 char extbuffer[4];
 extbuffer[0] = '1';
 extbuffer[1] = '0';
 extbuffer[2] = '0';
 extbuffer[3] = '0';
 writebuffer = reinterpret_cast<char *>(extbuffer);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //for raw dwi volumes

 //hdrofile.placePutStreamAtAbsolutePosition(352);

 inttag = 16;  //esize
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //356
 inttag = 18;  //DWI processing code
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 char rawstrbuffer[8];
 rawstrbuffer[0] = 'R';
 rawstrbuffer[1] = 'A';
 rawstrbuffer[2] = 'W';
 rawstrbuffer[3] = 'D';
 rawstrbuffer[4] = 'T';
 rawstrbuffer[5] = 'I';
 rawstrbuffer[6] = '\0';
 rawstrbuffer[7] = '\0';
 writebuffer = reinterpret_cast<char *>(rawstrbuffer);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,8);
 hdrofile.writeBytesToFileStream(writebuffer,8);

 //hdrofile.placePutStreamAtAbsolutePosition(372);
 
 inttag = 16;  //esize
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 inttag = 20;  //B val ecode
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 floattag = static_cast<float>(this->bval);  //b value
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 char fillbuffer[4];
 fillbuffer[0] = '\0';
 fillbuffer[1] = '\0';
 fillbuffer[2] = '\0';
 fillbuffer[3] = '\0';
 writebuffer = reinterpret_cast<char *>(fillbuffer);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 inttag = 16;  //esize
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 inttag = 22;  //gnum ecode
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 float azimuth = atan(this->gnum[1]/this->gnum[0]);
 float polar = acos(this->gnum[2]);

 std::cout<<"Spherical Coords are: azimuth = "<<azimuth<<"; polar = "<<polar<<std::endl;
 

 floattag = azimuth;  //azimuth angle (radians)
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 floattag = polar;  //polar angle (radians)
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);



 hdrofile.close();



}


/*
template<class T>
void NiftiVolumeHeaderV1N2<T>::writeOutHeaderFile() const
//writes out the header file in Analyze image format
{
#if VERBOSE
 std::cout<<"Writing Out Analyze Image Header File = "<<(this->fname)->getOSCompatibleFileName()<<std::endl;
#endif

 const int EXTENTS_FILEPOS = 32;  //MOVE TO FILE POSITIONS
 const int REGULAR_FILEPOS = 38;
 const int DIMS_FILEPOS = 40;
 const int VOXOFFSET_FILEPOS = 108;

 //replaces the file extension with hdr to try to open header file with the same name
 if(((this->fname)->getExtension()) != ".hdr")
  (this->fname)->setExtension(".hdr");

 WriteToBinaryFileV1N1<std::string> hdrofile(this->fname);
 hdrofile.openForWrite();
  
 char *writebuffer = NULL;
 int inttag = 0;
 int *intptr = NULL;
 char chartag;
 short shorttag = 0;
 short *shortptr = NULL;
 float floattag = 0.0f;
 float *floatptr = NULL;

 //the first 4 bytes specify the size of the ANALYZE file header (must be 348) and is used to determine if the file is byte swapped
 inttag = 348;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 

 hdrofile.placePutStreamAtAbsolutePosition(EXTENTS_FILEPOS);

 inttag = 16384;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 
 hdrofile.placePutStreamAtAbsolutePosition(REGULAR_FILEPOS);
 chartag = 'r';
 writebuffer = &chartag;
 hdrofile.writeBytesToFileStream(writebuffer,1);
 

 hdrofile.placePutStreamAtAbsolutePosition(DIMS_FILEPOS);
 
 //DIMS
 shorttag = 4;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);

 //XDIM
 shorttag = static_cast<short>(this->getRDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //YDIM
 shorttag = static_cast<short>(this->getCDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(ImageVolumeHeaderV1N2<T>::getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //ZDIM
 shorttag = static_cast<short>(this->getZDIM());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 //TIME
 shorttag = 1;
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 hdrofile.placePutStreamAtAbsolutePosition(DATATYPE_FILEPOS);
 
 short int dtypetag = 0;
 
 switch(this->getDataType())
 {
  case UNKNOWNTYPE:
   dtypetag = 0;
   break;
  case CHARTYPE:
  case UINT8TYPE:
  case SINT8TYPE:
   dtypetag = 2;
   break;
  case SINT16TYPE:
  case UINT16TYPE:
   dtypetag = 4;
   break;
  case SINT32TYPE:
  case UINT32TYPE:
   dtypetag = 8;
   break;
  case FLOATTYPE:
   dtypetag = 16;
   break;
  case EIGENTYPE:
   dtypetag = 21;
   break;
  case DOUBLETYPE:
   dtypetag = 64;
   break;
  case RGBTYPE:
   dtypetag = 128;
   break;
#if ERRORCHECK
  default:
    std::cout<<"ERROR in void NiftiVolumeHeaderV1N2<T>::writeOutHeaderFile() const"<<std::endl;
    std::cout<<"Not a valid datatype.  Exiting..."<<std::endl;
    std::cout<<"Datatype: "<<this->getDataType()<<std::endl;
    exit(1);
    break;
#endif
    
  }

 
 //DATATYPE
 shortptr = &dtypetag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);


 //BITS PER VOXEL
 shorttag = static_cast<short>(ImageVolumeHeaderV1N2<T>::getBitsPerVoxel());
 shortptr = &shorttag;
 writebuffer = reinterpret_cast<char *>(shortptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,2);
 hdrofile.writeBytesToFileStream(writebuffer,2);
 
 hdrofile.placePutStreamAtAbsolutePosition(XLENGTH_FILEPOS);
 
 //XLENGTH
 floattag = (static_cast<float>(this->getRLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //YLENGTH
 floattag = (static_cast<float>(this->getCLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);

 //ZLENGTH
 floattag = (static_cast<float>(this->getZLENGTH()));
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 hdrofile.placePutStreamAtAbsolutePosition(VOXOFFSET_FILEPOS);
 
 //VOX OFFSET
 floattag = 0.0f;
 floatptr = &floattag;
 writebuffer = reinterpret_cast<char *>(floatptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);
 

 hdrofile.placePutStreamAtAbsolutePosition(344);

 //SMIN
 inttag = 0;
 intptr = &inttag;
 writebuffer = reinterpret_cast<char *>(intptr);
 if(this->getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
  byteswap<char>(writebuffer,4);
 hdrofile.writeBytesToFileStream(writebuffer,4);


 hdrofile.close();


}
*/


template<class T>
void convertThreeGradMatrixToSixGradMatrix(hageman::Matrix2DV2N4<T> &sixgrad, hageman::Matrix2DV2N4<T> &threegrad, hageman::AllocateMem amem = hageman::NOALLOCATE)
//converts a matrix of 3 coordinate gradient vectors (gx,gy,gz) to six coordinate vectors (gxx,gyy,gzz,gxy,gxz,gyz)
{

 if(amem != hageman::NOALLOCATE)
 {
  sixgrad.initVolume2D(threegrad.getRDIM(),6);
 }

 T *threearray = new T[3];  //current arrays for conversion
 T *sixarray = new T[6];

 for(int idx = 0; idx < sixgrad.getRDIM(); idx++)
 {
  threegrad.getMatrixRow(threearray,idx);
  hageman::convertDTIGradient3VecTo6Vec(threearray,sixarray);
  sixgrad.setMatrixRow(sixarray,idx);
 }

 delete[] threearray;
 delete[] sixarray;


}


template<class T>
T getMaxXCoord(const std::vector<Vector3V2N4<T> > &coords)
//return the maximum value of the x coordinate in a list of 3D coordinate values
{
 typename std::vector<Vector3V2N4<T> >::const_iterator pos;
 T maxval = 0;  //max value is guaranteed to be greater than 0

 for(pos = coords.begin(); pos != coords.end(); pos++)
 {
  if((*pos).x() > maxval)
   maxval = (*pos).x();
 }

 return maxval;


}


template<class T>
T getMaxYCoord(const std::vector<Vector3V2N4<T> > &coords)
//return the maximum value of the y coordinate in a list of 3D coordinate values
{
 typename std::vector<Vector3V2N4<T> >::const_iterator pos;
 T maxval = 0;  //max value is guaranteed to be greater than 0

 for(pos = coords.begin(); pos != coords.end(); pos++)
 {
  if((*pos).y() > maxval)
   maxval = (*pos).y();
 }

 return maxval;


}

template<class T>
T getMaxZCoord(const std::vector<Vector3V2N4<T> > &coords)
//return the maximum value of the z coordinate in a list of 3D coordinate values
{
 typename std::vector<Vector3V2N4<T> >::const_iterator pos;
 T maxval = 0;  //max value is guaranteed to be greater than 0

 for(pos = coords.begin(); pos != coords.end(); pos++)
 {
  if((*pos).z() > maxval)
   maxval = (*pos).z();
 }

 return maxval;


}


template<class T>
void addRicianNoiseToImgVol(ImageVolumeV2N4<T> &imgvol, double stddev)
//adds Rician noise to imgvol with standard deviation, stddev
{
 double voxnoise = 0;  //noise to be added to the current voxel
 RicianNoiseV1N2<double> rnoise(time(NULL));
 
 for(int idx = 0; idx < imgvol.getSIZE(); idx++)  //loops over all voxels in the imgvol
 {
  voxnoise = rnoise.generateRicianRandomNumber(static_cast<double>(imgvol.getElement(idx)), stddev*(static_cast<double>(imgvol.getElement(idx))));
  //generates a random number based on the Rician distribution
  imgvol.setElement(idx,static_cast<T>(voxnoise));  //be careful of the static cast from double to T; rounding errors may occur for some datatypes
 }

}


template<class S, class T>
void maskVolume3D(Volume3DV2N4<T> &datavol, const ImageVolumeV2N4<S> &maskvol)
//masks the 3D volume by setting all voxels in the datavol that have corresponding zero voxels in the mask volume to zero
{
#if ERRORCHECK
 if(!(datavol.areDimensionsEqual(maskvol)))
 {
  std::cout<<"ERROR in function void maskVolume3D(Volume3DV2N2<T> &datavol, const ImageVolumeV2N4<S> &maskvol)"<<std::endl;
  std::cout<<"Cannot mask volume; Dimensions of volume and mask are not equal.  Exiting..."<<std::endl;
  exit(1);
 }
#endif

 for(int idx = 0; idx < datavol.getSize(); idx++)
 {
  if(maskvol == S())  //if the mask voxel is zero, then zero out the corresponding voxel in the datavol
   datavol.setElement(idx,T());
 }


}

template<class T>
Vector3V2N4<double> getRandomPtFromImageVoxel(int i, int j, int k, const ImageVolumeV2N4<T> &imgvol)
//gets a random point in real world coordinates from the image volume
{
 MathV1N2<> mygenerator;  //create the random number generator

 //get random numbers between 0 and 1 with three sig digits
 double irand = static_cast<double>(mygenerator.getRandomNumberMerseeneTwister(1000))/1000.0;
 double jrand = static_cast<double>(mygenerator.getRandomNumberMerseeneTwister(1000))/1000.0;
 double krand = static_cast<double>(mygenerator.getRandomNumberMerseeneTwister(1000))/1000.0;

 //scale the random nums by the voxel lengths
 irand *= imgvol.getRLENGTH();
 jrand *= imgvol.getCLENGTH();
 krand *= imgvol.getZLENGTH();

 //get the start of the voxel in real world coords
 double iref = imgvol.convertRVoxelToRReal(i);
 double jref = imgvol.convertCVoxelToCReal(j);
 double kref = imgvol.convertZVoxelToZReal(k);

 //add the voxel real coords to the random number
 Vector3V2N4<double> result(iref+irand, jref+jrand, kref+krand);
 return result;

}


template<class T>
ImageSliceV2N4<T> getImageZSlice(int slicenum, const ImageVolumeV2N4<T> &imgvol)
//returns a slice zidx = slicenum in the Z dimension
{
 ImageSliceV2N4<T> result(imgvol.getRDIM(), imgvol.getCDIM());

 for(int j = 0; j < (imgvol.getCDIM()); j++)
  for(int i = 0; i < (imgvol.getRDIM()); i++)
  {
   result.setElement(i,j,imgvol.getElement(i,j,slicenum));
  }

 return result;


}

template<class T>
Vector3V2N4<int> getVoxelCoordinatesForRealWorldPt(const ImageVolumeV2N4<T> &imgvol, const Vector3V2N4<double> &realpt)
//returns the voxel coordinates for realpt in read world coordinates based on the dimensions of imgvol
{
 Vector3V2N4<int> result(imgvol.convertRRealToRVoxel(realpt.x()), imgvol.convertCRealToCVoxel(realpt.y()), imgvol.convertZRealToZVoxel(realpt.z()));
 return result;

}

template<class T>
Vector3V2N4<int> getVoxelCoordinatesForRealWorldPt(const Vector3V2N4<T> &realpt, const int RDIMv, const int CDIMv, const int ZDIMv, const double RLENGTHv, const double CLENGTHv, const double ZLENGTHv)
//returns the voxel coordinates for realpt in read world coordinates based on the dimensions of imgvol
{

 ImageVolumeV2N4<T> temp;  //creates an empty image volume
 temp.setRDIM(RDIMv);  //sets the empty image volume dimensions and lengths
 temp.setCDIM(CDIMv);
 temp.setZDIM(ZDIMv);

 temp.setRLENGTH(RLENGTHv);
 temp.setCLENGTH(CLENGTHv);
 temp.setZLENGTH(ZLENGTHv);


 return (hageman::getVoxelCoordinatesForRealWorldPt(temp,realpt));
}


template<class T>
Vector3V2N4<double> getCenterOfVoxelInRealWorldCoordinates(const ImageVolumeV2N4<T> &imgvol, const Vector3V2N4<int> &voxpt)
//returns the center of the voxel (specified by the voxel coordinates, voxpt) in real world coordinates based on the dimensions/lengths of imgvol
{
 
 double x,y,z;  //the voxel coordinates
 x = (imgvol.convertRVoxelToRReal(voxpt.x()));  //converts the voxel coordinates to real world coordinates
 y = (imgvol.convertCVoxelToCReal(voxpt.y()));
 z = (imgvol.convertZVoxelToZReal(voxpt.z()));
 
 Vector3V2N4<double> result(x,y,z);

 return result;

}

template<class T>
T getInterpolatedBSplineValue(ImageVolumeV2N4<T> &imgvol, double ir, double jr, double kr, ImageVolumeV2N4<T> &y2a)  
//gets the value at real world pt (ir,jr,kr) based on a B-spline interpolation
{
 //creates the vectors to hold the anchor points for each dimension; only need to check x1a b/c they are initialized together
 if((imgvol.x1a) == NULL)
 {
  imgvol.x1a = new double[imgvol.getRDIM()];  //initializes the interpolation arrays
  imgvol.x2a = new double[imgvol.getCDIM()];
  imgvol.x3a = new double[imgvol.getZDIM()];
     
  for(int i = 0; i < (imgvol.getRDIM()); i++)
   imgvol.x1a[i] = (static_cast<double>(i) * (imgvol.getRLENGTH())) + (0.5f*(imgvol.getRLENGTH())); //puts the anchor pt at the center of the voxel
  for(int j = 0; j < (imgvol.getCDIM()); j++)
   imgvol.x2a[j] = (static_cast<double>(j) * (imgvol.getCLENGTH())) + (0.5f*(imgvol.getCLENGTH())); //puts the anchor pt at the center of the voxel
  for(int k = 0; k < (imgvol.getZDIM()); k++)
   imgvol.x3a[k] = (static_cast<double>(k) * (imgvol.getZLENGTH())) + (0.5f*(imgvol.getZLENGTH())); //puts the anchor pt at the center of the voxel
 }

 if(y2a.isEmpty())
 {
  y2a.makeCompatible(imgvol);
  MathV1N2<>::calculate3DCubicSplineSecondDerivatives(imgvol.x1a,imgvol.x2a,imgvol.x3a,imgvol,y2a);
 }
 
 T result = (MathV1N2<>::calculate3DCubicSpline(imgvol.x1a,imgvol.x2a,imgvol.x3a,imgvol,y2a,ir,jr,kr));
 
 return result;

}

template<class T>
T getInterpolatedTriLinearValue(ImageVolumeV2N4<T> &imgvol, double ir, double jr, double kr)  
//gets the value at real world pt (ir,jr,kr) based on a polynomial interpolation
{
 //creates the vectors to hold the anchor points for each dimension; only need to check x1a b/c they are initialized together
 if((imgvol.x1a) == NULL)
 {
  imgvol.x1a = new double[imgvol.getRDIM()];  //initializes the interpolation arrays
  imgvol.x2a = new double[imgvol.getCDIM()];
  imgvol.x3a = new double[imgvol.getZDIM()];
     
  for(int i = 0; i < (imgvol.getRDIM()); i++)
   imgvol.x1a[i] = (static_cast<double>(i) * (imgvol.getRLENGTH())) + (0.5f*(imgvol.getRLENGTH())); //puts the anchor pt at the center of the voxel
  for(int j = 0; j < (imgvol.getCDIM()); j++)
   imgvol.x2a[j] = (static_cast<double>(j) * (imgvol.getCLENGTH())) + (0.5f*(imgvol.getCLENGTH())); //puts the anchor pt at the center of the voxel
  for(int k = 0; k < (imgvol.getZDIM()); k++)
   imgvol.x3a[k] = (static_cast<double>(k) * (imgvol.getZLENGTH())) + (0.5f*(imgvol.getZLENGTH())); //puts the anchor pt at the center of the voxel
 }

 T result = (NumericalRecipesV1N2<double>::calculate3DTriLinearInterpolation(imgvol.x1a,imgvol.x2a,imgvol.x3a,imgvol,ir,jr,kr));
 
 return result;

}


template<class T>
class NodeV1N1{

public:
 NodeV1N1();  //empty default constructor
 NodeV1N1(const T &obj);  //initializes with data
 NodeV1N1(const NodeV1N1<T> &copy);  //copy constructor

 T data; 
 NodeV1N1<T> *next;  //pointer to the next node in the list

protected:


};

template<class T>
NodeV1N1<T>::NodeV1N1()
//empty default constructor
{
 next = NULL;

}

template<class T>
NodeV1N1<T>::NodeV1N1(const T &obj)  
//initializes with data
:data(obj)
{
 next = NULL;
 
}

template<class T>
NodeV1N1<T>::NodeV1N1(const NodeV1N1<T> &copy)  
//copy constructor
{
 next = NULL;
 (this->data) = copy.data;  //copies the node data


}


template<class T>
class LinkedListV1N1{

public:
 //CONSTRUCTORS
 LinkedListV1N1();  //default empty constructor
 LinkedListV1N1(const LinkedListV1N1<T> &copy);  //copy constructor
 ~LinkedListV1N1();  //destructor

 //GET FUNCTIONS
 inline int getSize() const;  //returns the number of nodes in the list
 inline const T& getNodeData(int idx) const;  //returns the data of the node at list position, idx
 inline T& getNonConstNodeData(int idx);  //returns the data of the node at list position, idx
 const NodeV1N1<T>& getNode(int idx) const;  //returns a reference to the node at list position, idx


 //CHECK FUNCTIONS
 bool isEmpty() const {return head == NULL;}

 //LIST FUNCTIONS
 void addAtBack(const NodeV1N1<T> &obj);  //adds a node to the back of the list
 void addAtFront(const NodeV1N1<T> &obj);  //adds a node to the front of the list
 void insertPoint(const NodeV1N1<T> &obj, int idx);  //adds a node AFTER the node at idx
 void copyList(const LinkedListV1N1<T> &copy); //copies the list
 //void removeAtBack();


 //PRINT FUNCTIONS
 void printCL() const;  //prints the list elements on the CL in order

 //OVERLOADED OPERATORS
 void operator=(const LinkedListV1N1<T> &copy);

protected:
 NodeV1N1<T> *head;
 


};

//CONSTRUCTORS

template<class T>
LinkedListV1N1<T>::LinkedListV1N1()
//empty default constructor
{
 head = NULL;
 
 
}

template<class T>
LinkedListV1N1<T>::LinkedListV1N1(const LinkedListV1N1<T> &copy)
//copy constructor
{
 head = NULL;
 if(!copy.isEmpty())
 {
  for(int idx = 0; idx < copy.getSize(); idx++)
  {
   this->addAtBack(copy.getNode(idx));
  }
 }


}
 

template<class T>
LinkedListV1N1<T>::~LinkedListV1N1()
//destructor
{
 if(!this->isEmpty())
 {
  NodeV1N1<T> *ptr;
  
  while(head != NULL)  //individually deletes the nodes until there are none left
  {
   ptr = head;

   //sets the head to the next node in the list, if any
   head = head->next;

   delete ptr;
   ptr = NULL;
  }
 }
   
}


//GET FUNCTIONS
template<class T>
int LinkedListV1N1<T>::getSize() const
//returns the number of nodes in the list
{
 int numnodes = 0;

 if(!this->isEmpty())
 {
  NodeV1N1<T> *ptr;
  ptr = head;

  while(ptr->next != NULL) //this loop DOES NOT count the last object
  {
   ptr = ptr->next;
   numnodes++;
  }

  //even though we are at the last object, we still need to count it.
  numnodes++;
 }

 return numnodes;
 

}

template<class T>
const T& LinkedListV1N1<T>::getNodeData(int idx) const
//returns the data of the node at list position, idx
{
 NodeV1N1<T> *ptr;
 ptr = this->head;

 for(int i = 0; i < idx; i++)  //this loop traverses until ptr = element in position idx
 {
  ptr = ptr->next;
 }

 return ptr->data;
}

template<class T>
T& LinkedListV1N1<T>::getNonConstNodeData(int idx)  
//returns the non-const data of the node at list position, idx
{

 NodeV1N1<T> *ptr;
 ptr = this->head;

 for(int i = 0; i < idx; i++)  //this loop traverses until ptr = element in position idx
 {
  ptr = ptr->next;
 }

 //returns the non-const data reference
 return ptr->data;

}

template<class T>
const NodeV1N1<T>& LinkedListV1N1<T>::getNode(int idx) const 
//returns a reference to the node at list position, idx
{
 NodeV1N1<T> *ptr;
 ptr = this->head;

 for(int i = 0; i < idx; i++)  //this loop traverses until ptr = element in position idx
 {
  ptr = ptr->next;
 }

 //returns the non-const data reference
 return (*ptr);


}

 
 

//LIST FUNCTIONS
template<class T>
void LinkedListV1N1<T>::addAtBack(const NodeV1N1<T> &obj)  
//adds a node to the back of the list
{
 //this all must be done on the heap to prevent Nodes from being deleted when they go out of scope
 NodeV1N1<T> *newnode = new NodeV1N1<T>(obj);
 
 if(this->isEmpty())
 {
  head = newnode;
  head->next = NULL;
 }
 else
 {
  NodeV1N1<T> *ptr;
  ptr = this->head;

  while((ptr->next) != NULL) //traverses the list until ptr points at the last object
  {
   ptr = ptr->next;
  }
  
  ptr->next = newnode;
 }


}

template<class T>
void LinkedListV1N1<T>::addAtFront(const NodeV1N1<T> &obj)  
//adds a node to the front of the list
{
 //this all must be done on the heap to prevent Nodes from being deleted when they go out of scope
 NodeV1N1<T> *newnode = new NodeV1N1<T>(obj);
 
 if(this->isEmpty())
 {
  head = newnode;
  head->next = NULL;
 }
 else
 {
  NodeV1N1<T> *ptr;
  ptr = this->head;

  head = newnode;  //moves the head to the new node
  head->next = ptr;

 }


}
 

template<class T>
void LinkedListV1N1<T>::insertPoint(const NodeV1N1<T> &obj, int idx)  
//adds a node AFTER the node at idx
{
 //this all must be done on the heap to prevent Nodes from being deleted when they go out of scope
 NodeV1N1<T> *newnode = new NodeV1N1<T>(obj);
 
 NodeV1N1<T> *ptr;
 ptr = this->head;

 for(int i = 0; i < idx; i++)  //this loop traverses until ptr = element in position idx
 {
  ptr = ptr->next;
 }

 NodeV1N1<T> *temp;
 temp = ptr->next;  //holds the next element in the sequence
 
 ptr->next = newnode;
 newnode->next = temp;


}

template<class T>
void LinkedListV1N1<T>::copyList(const LinkedListV1N1<T> &copy) 
//copies the list
{
 head = NULL;

 for(int idx = 0; idx < copy.getSize(); idx++)
 {
  this->addAtBack(copy.getNode(idx));
 }


}



//PRINT FUNCTIONS
template<class T>
void LinkedListV1N1<T>::printCL() const
//prints the list elements on the CL in order.  Objects must have the operator << defined
{

 std::cout<<"Printing List..."<<std::endl;

 if(this->isEmpty())
  std::cout<<"List is Empty!"<<std::endl;
 else
 {
  NodeV1N1<T> *ptr;
  ptr = this->head;

  while(ptr->next != NULL)  //this loop DOES NOT print out the last object
  {
   std::cout<<ptr->data<<std::endl;
   ptr = ptr->next;
  }

  //even though we are at the last object, we still need to print it out.
  std::cout<<ptr->data<<std::endl;

 }


}

//OVERLOADED OPERATORS
template<class T>
void LinkedListV1N1<T>::operator=(const LinkedListV1N1<T> &copy)
//overloaded assignment operator
{
 (this->copyList(copy));

}


template<class T>
void initEigenSystem3D(EigenSystem3DV1N4<T> &esys, const T &l1v, const T &l2v, const T &l3v, const Vector3V2N4<T> &v1vec, const Vector3V2N4<T> &v2vec, const Vector3V2N4<T> &v3vec)
//initializes esys using 3 Vector3 structures for eigenvectors.  This function needs to be outside the EigenSystem3D class to prevent dependence on
//the Volume class hierarchy
{

 T v1v[3] = {v1vec.x(), v1vec.y(), v1vec.z()};
 T v2v[3] = {v2vec.x(), v2vec.y(), v2vec.z()};
 T v3v[3] = {v3vec.x(), v3vec.y(), v3vec.z()};

 esys.initEigenSystem3D(l1v,l2v,l3v,v1v,v2v,v3v);
 
}



template<class T>
EigenSystem3DV1N4<double> generateRandomEigenSystem3D(const Vector3V2N4<double> &evals)
//EigenSystem3DV1N2<double> generateRandomEigenSystem3D(const Vector3V2N2<double> &evals, const MathV1N0<> &randnumgen)
//generates an EigenSystem3D with evals with a random prinicple direction
{

 double dx = 0.0f;
 double dy = 0.0f; 
 double dz = 0.0f;

 MathV1N2<> randnumgen;

 //determines the principal eigenvector randomly.  vector element values run from -1 to 1 with a precision of 2
 while((dx == 0.0f) && (dy == 0.0f) && (dz == 0.0f))  //prevents a zero vector 
 {
  dx = static_cast<double>(randnumgen.getRandomNumberMerseeneTwister(100,-100));
  dy = static_cast<double>(randnumgen.getRandomNumberMerseeneTwister(100,-100));
  dz = static_cast<double>(randnumgen.getRandomNumberMerseeneTwister(100,-100));
  
 }

 Vector3V2N4<double> dirf(dx,dy,dz);
 dirf.normalize();

 //the second eigenvector is taken as the direction perpendicular to dirf
 Vector3V2N4<double> pdirf(dirf.getPerpendicularDirection());
 pdirf.normalize();

 //the third eigenvector is taken as the cross product of the two other eignevectors
 Vector3V2N4<double> pdir2f(dirf.cross(pdirf));
 pdir2f.normalize();

 EigenSystem3DV1N4<double> esys;
 hageman::initEigenSystem3D(esys,evals.x(), evals.y(), evals.z(), dirf, pdirf, pdir2f);

 return esys;

 
}

#if 0

#define MAXTAG 0x29

template<class T>
class DICOMVolumeHeaderV1N2 : public ImageVolumeHeaderV1N2<T>{

public:
 //CONSTRUCTORS
 DICOMVolumeHeaderV1N2();  //default empty constructor
 DICOMVolumeHeaderV1N2(const FileNameV1N1<> *fnamev);  //constructs an object from a .hdr file
 DICOMVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, ImageType itv, DataType dtv, int bitspervoxel, bool shouldbyteswap, int RDIMv, int CDIMv, int ZDIMv, double RLv = 1.0, double CLv = 1.0, double ZLv = 1.0);  //default empty constructor
 DICOMVolumeHeaderV1N2(const DICOMVolumeHeaderV1N2<T> &copy); //copy constructor
 
 //PRINT FUNCTIONS
 virtual void printHeader() const;  //prints the header information, one value per line
 
 //GET FUNCTIONS
 const std::multimap<std::pair<unsigned short,unsigned short>,double>& getNumMap() const {return nummap;}  //returns a const reference to the numeric map
 const std::multimap<std::pair<unsigned short,unsigned short>,std::string>& getStrMap() const {return strmap;}  //returns a const reference to the string map
 double getSingleNumericTag(std::pair<unsigned short,unsigned short> key) const;  //returns the value for a DICOM numeric tag known to have one number value associated with it
 std::string getSingleStringTag(std::pair<unsigned short,unsigned short> key) const; //returns the value for a DICOM string tag known to have one string value associated with it
 std::vector<double> getNumericTag(std::pair<unsigned short,unsigned short> key) const;  //returns an arbitrary number of numeric values associated with key; 
 //values are converted to double 
 std::vector<std::string> getStringTag(std::pair<unsigned short,unsigned short> key) const;  //returns an arbitrary number of string values associated with key; 
 //values are converted to std::string 

 //READ FUNCTIONS
 virtual void readInHeaderFile(const FileNameV1N1<> *fnamev); //read in the DICOM header file, fnamev
 bool readInDataElement(ReadFromBinaryFileV1N1<char> *hdrifile);  //reads in an arbitrary DICOM header element and puts it into the appropriate map; returns true if reached the pixel data
 void readNumElement(ReadFromBinaryFileV1N1<char> *hdrifile, std::string vr, unsigned int vl, std::pair<unsigned short,unsigned short> elementtag);  //reads a numeric element into the 
 //num map
 bool readStrElement(ReadFromBinaryFileV1N1<char> *hdrifile, std::string vr, unsigned int vl, std::pair<unsigned short,unsigned short> elementtag);  //reads a string element into the
 //string map

 //WRITE FUNCTIONS
 virtual void writeOutHeaderFile(const FileNameV1N1<> *fnamev) const;  //writes out the header file 

protected:
 std::multimap<std::pair<unsigned short,unsigned short>,double> nummap;  //DICOM header elements with numeric values
 std::multimap<std::pair<unsigned short,unsigned short>,std::string> strmap;  //DICOM header elements with numeric values

};

//CONSTRUCTORS
template<class T>
DICOMVolumeHeaderV1N2<T>::DICOMVolumeHeaderV1N2()
//empty default constructor
:ImageVolumeHeaderV1N2<T>()
{

}

template<class T>
DICOMVolumeHeaderV1N2<T>::DICOMVolumeHeaderV1N2(const FileNameV1N1<> *fnamev, ImageFileFormat ifv, ImageType itv, DataType dtv, int bitspervoxelv, bool shouldbyteswapv,int RDIMv, int CDIMv, int ZDIMv, double RLv, double CLv, double ZLv)  
//constructor that assigns all arguments
:ImageVolumeHeaderV1N2<T>(fnamev,ifv,itv,dtv,bitspervoxelv,shouldbyteswapv,RDIMv,CDIMv,ZDIMv,RLv,CLv,ZLv)
{
 
}

template<class T>
DICOMVolumeHeaderV1N2<T>::DICOMVolumeHeaderV1N2(const FileNameV1N1<> *fnamev)  
//constructor that assigns all arguments based on an Analyze header file
:ImageVolumeHeaderV1N2<T>()
{
 this->imgformat = DICOM;
 this->readInHeaderFile(fnamev);
 
}



template<class T>
DICOMVolumeHeaderV1N2<T>::DICOMVolumeHeaderV1N2(const DICOMVolumeHeaderV1N2<T> &copy) 
//copy constructor
:ImageVolumeHeaderV1N2<T>(copy)
{
 
 (this->nummap) = copy.getNumMap();  //copies the <string,number> map
 (this->strmap) = copy.getStrMap();  //copies the <string,string> map

 
}
 
//PRINT FUNCTIONS
template<class T>
void DICOMVolumeHeaderV1N2<T>::printHeader() const
//reimplementation of the virtual print header function from the base class
{
 ImageVolumeHeaderV1N2<T>::printHeader();  //calls the parent class print function
 
 std::cout<<"Printing Element Tags"<<std::endl;
 typename std::multimap<std::pair<unsigned short,unsigned short>,double>::const_iterator numpos;  //iterator for numeric map
 typename std::multimap<std::pair<unsigned short,unsigned short>,std::string>::const_iterator strpos;  //iterator for string map

 std::cout<<"Numeric Tags:"<<std::endl;  //the <string,number> map elements
 for(numpos = (this->nummap.begin()); numpos != (this->nummap.end()); numpos++)
 {
  std::cout<<std::hex<<std::showbase<<(numpos->first).first<<","<<(numpos->first).second<<": "<<std::dec<<numpos->second<<std::endl;
 }

 std::cout<<"String Tags:"<<std::endl;  //the <string,string> map elements
 for(strpos = (this->strmap.begin()); strpos != (this->strmap.end()); strpos++)
 {
  std::cout<<std::hex<<std::showbase<<(strpos->first).first<<","<<(strpos->first).second<<": "<<std::dec<<strpos->second<<std::endl;
 }



}

//GET FUNCTIONS
template<class T>
double DICOMVolumeHeaderV1N2<T>::getSingleNumericTag(std::pair<unsigned short,unsigned short> key) const
//returns the value for a DICOM numeric tag known to have one number associated with it
{

#if ERRORCHECK
 if((this->nummap.find(key)) == (nummap.end()))
 {
  std::cout<<"ERROR in double DICOMVolumeHeaderV1N2<T>::getSingleNumericTag(std::pair<short,short> key) const"<<std::endl;
  std::cout<<"Cannot find key in map"<<std::endl;
  std::cout<<std::hex<<std::showbase<<(key.first)<<","<<key.second<<std::endl;
  exit(1);
 }
#endif

 return ((this->nummap.find(key))->second);



}

template<class T>
std::string DICOMVolumeHeaderV1N2<T>::getSingleStringTag(std::pair<unsigned short,unsigned short> key) const
//returns the value for a DICOM numeric tag known to have one number associated with it
{

#if ERRORCHECK
 if((this->strmap.find(key)) == (strmap.end()))
 {
  std::cout<<"ERROR in std::string DICOMVolumeHeaderV1N2<T>::getSingleStringTag(std::pair<short,short> key) const"<<std::endl;
  std::cout<<"Cannot find key in map"<<std::endl;
  std::cout<<std::hex<<std::showbase<<(key.first)<<","<<key.second<<std::endl;
  exit(1);
 }
#endif


 return ((this->strmap.find(key))->second);

}

template<class T>
std::vector<double> DICOMVolumeHeaderV1N2<T>::getNumericTag(std::pair<unsigned short,unsigned short> key) const
//returns an arbitrary number of numeric values associated with key; values are converted to double 
{
 std::multimap<std::pair<unsigned short,unsigned short>,double>::const_iterator pos;
 std::pair<std::multimap<std::pair<unsigned short,unsigned short>,double>::const_iterator, std::multimap<std::pair<unsigned short,unsigned short>,double>::const_iterator> finditers;
 finditers = (this->nummap).equal_range(key);  //returns a pair of iterators that span the range of value that have key

 std::vector<double> result;
 
 for(pos = finditers.first; pos != finditers.second; pos++)  //puts all the values with key into a vector
 {
  result.push_back(pos->second);
 }


 return result;

}


template<class T>
std::vector<std::string> DICOMVolumeHeaderV1N2<T>::getStringTag(std::pair<unsigned short,unsigned short> key) const
//returns an arbitrary number of string values associated with key; values are converted to std::string 
{

 std::multimap<std::pair<unsigned short,unsigned short>,std::string> >::const_iterator pos;
 std::pair<std::multimap<std::pair<unsigned short,unsigned short>,std::string>::const_iterator, std::multimap<std::pair<unsigned short,unsigned short>,std::string::const_iterator> finditers;
 finditers = (this->nummap).equal_range(key);  //returns a pair of iterators that span the range of value that have key

 std::vector<std::string> result;

 for(pos = finditers.first; pos != finditers.second; pos++)  //puts all the values with key into a vector
 {
  result.push_back(pos->second);
 }


 return result;


}




//READ FUNCTIONS
template<class T>
void DICOMVolumeHeaderV1N2<T>::readInHeaderFile(const FileNameV1N1<> *fnamev)  
//reads in an DICOM header file that is part of fname
{
 //deletes any fname currently associated with the DICOM header
 if((this->fname) != NULL)
 {
  delete (this->fname);
  (this->fname) = NULL;
 }

 (this->fname) = fnamev->clone();

#if VALIDATE
  std::cout<<"From function void DICOMVolumeHeaderV1N2<T>::readInHeaderFile(FileNameV1N1<> *fnamev)"<<std::endl;  
  std::cout<<"Filename: "<<(this->fname)->getOSCompatibleFileName()<<std::endl;
#endif

 
 ReadFromBinaryFileV1N1<char> *hdrifile = new ReadFromBinaryFileV1N1<char>(this->fname);
 hdrifile->openForRead();
 hdrifile->skipBytes(128);  //the first 128 bytes of DICOM file are a preamble

 char fourbytebuffer[4];
 
 //these next four bytes will determine whether byte swapping needs to be done on this data.  They should read 'D' 'I' 'C' 'M'
 hdrifile->readFromFileStream(fourbytebuffer,4);
 if(fourbytebuffer[0] == 'D')
  this->shouldbyteswap = false;
 else if(fourbytebuffer[0] == 'M')
  this->shouldbyteswap = true;
#if ERRORCHECK
 else
 {
  std::cout<<"ERROR in void DICOMVolumeHeaderV1N2<T>::readInHeaderFile(const FileNameV1N1<> *fnamev, CheckStatus cstat)"<<std::endl;
  std::cout<<"Not a valid DICOM file.  Exiting..."<<std::endl; 
  exit(1);
 }
#endif
 
 while(!this->readInDataElement(hdrifile))  //successively reads in the DICOM data elements until an error is encountered or end of header file is reached, as determined by the bool
 //value returned by readInDataElement
 {
 }

 hdrifile->close();
 delete hdrifile;
  
}

template<class T>
bool DICOMVolumeHeaderV1N2<T>::readInDataElement(ReadFromBinaryFileV1N1<char> *hdrifile)
//reads in a DICOM header element into the map.  Returns true if the entire header was read; returns false if the read was successful but there are more header elements to process
{

 char fourbytebuffer[4];
 char twobytebuffer[2];
 char vrbuffer[2];
 char vlbuffer[2];
 char imbuffer[4];
 unsigned short int *ushortptr;
 unsigned int *uintptr;
 unsigned short int ushortval;

 bool endfile = false;
 std::pair<unsigned short, unsigned short> elementtag;  //DICOM header tags which are of the hexidecimal format (0x??,0x??).
 //in the DICOM standard, Element ID is defined in hexadecimal format.  when printing in the ostream, std::dec must be called at the end because std::hex sets the stream to 
 //hexadecimal format until it is explicitly changed.

 hdrifile->readFromFileStream(twobytebuffer,2);  //reads in the first two bytes which is the group number
 ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);  //needs to be converted to an unsigned short
 if((*ushortptr) == 0x7FE0)  //this is the group ID which designates pixel data and means that the header has finished; since all values are automatically stored in binary rep., i can do a comparison between hex and dec reps.
  return true;
 
 elementtag.first = (*ushortptr);  //2 byte unsigned short group ID

 hdrifile->readFromFileStream(twobytebuffer,2);  //reads in the two bytes which is the element number
 ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);  //needs to be converted to an unsigned short
 elementtag.second = (*ushortptr);  //2 byte unsigned short element ID

 //tries to determine the VR, which is a two byte char string that gives the datatype of the element
 hdrifile->readFromFileStream(vrbuffer,2);  //reads in the next two bytes which is equal to VR with explicit VR scheme
 std::string vr(vrbuffer);

 //erases any characters in the string larger than the two byte limit
 if(vr.size() > 2)
  vr.erase(2);

 //std::cout<<"Element Tag: "<<elementtag.first<<","<<elementtag.second<<std::endl;
 //std::cout<<"VR: "<<vr<<std::endl;

 if((vr == "OB") || (vr == "OW") || (vr == "OF") || (vr == "SQ") || (vr == "UT") || (vr == "UN"))  //if VR equals these datatypes, then skip the next 2 bytes and read in the
 //the 4 bytes which are equal to the value length (32 bit unsigned int).  these values are complex so they are skipped
 {
  hdrifile->skipBytes(2);  //skips the two reserved bytes
  hdrifile->readFromFileStream(fourbytebuffer,4);  //the value length, VL
  uintptr = reinterpret_cast<unsigned int*>(fourbytebuffer);  //VL is expressed as an unsigned int

  if(*uintptr == 0xffffffff)  //this designates an sequence item with undefined length
  {
    std::pair<unsigned short,unsigned short> termtag = std::make_pair<unsigned short, unsigned short>(0,0);  //the element tag for a single sequence item
    
    while((termtag.first != (0xfffe)) && (termtag.second != (0xe0dd)))  //not equal the sequence delimitation item tag
    {
     hdrifile->readFromFileStream(twobytebuffer,2);  //the item tag
     ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);
     termtag.first = (*ushortptr);

     hdrifile->readFromFileStream(twobytebuffer,2); //the item length
     ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);
     termtag.second = (*ushortptr);

     
    }

    hdrifile->skipBytes(4);  //skips empty four bytes at the end of a sequence
  }
  else
  {

   hdrifile->skipBytes(*uintptr);  //skips over the value to the next header tag
   std::cout<<"Skipping bytes = "<<*uintptr<<std::endl;
   //std::cout<<std::hex<<*uintptr<<std::endl;
  }
  
 }
 else  //the next two bytes are the VL in the explicit scheme (as a 2 byte unsigned short) or it is an implicit scheme
 {
  hdrifile->readFromFileStream(vlbuffer,2);  //read in the possible two byte VL
  ushortptr = reinterpret_cast<unsigned short int*>(vlbuffer);  //convert it to an unsigned short
  ushortval = *ushortptr;
  unsigned int vl = static_cast<unsigned int>(ushortval);  //VL THEN NEEDS TO BE CONVERTED TO UNSIGNED INT - ALWAYS HAS TO BE UNSIGNED INT (NOT SHORT) TO AVOID OUT OF RANGE ERRORS
  
  if((vr == "UL") || (vr == "US") || (vr == "FD") || (vr == "FL") || (vr == "SL") || (vr == "SS"))  //in this exlicit VR scheme, these are numeric tags to be read into the num map
  {

   this->readNumElement(hdrifile,vr,vl,elementtag);

  }
  else if((vr == "CS") || (vr == "SH") || (vr == "UI") || (vr == "DA") || (vr == "TM") || (vr == "LO") || (vr == "ST") || (vr == "PN") || (vr == "AS") || (vr == "DS") || (vr == "IS") || (vr == "AE"))
  //in this explicit VR, these are string tags to be read into the string map
  {

   this->readStrElement(hdrifile,vr,vl,elementtag);

  }
  else if(vr == "UN")  //VR is unknown, so skip over the bytes for these values
  {
   hdrifile->skipBytes(ushortval);
   
  }
  else  //does not correspond to any explicit VR/VL, so it is an implicit length scheme.  therefore, the VR and VL together are the 4 bytes that represent the value length as a 
  //32 bit unsigned int
  {
   imbuffer[0] = vrbuffer[0];
   imbuffer[1] = vrbuffer[1];
   imbuffer[2] = vlbuffer[0];
   imbuffer[3] = vlbuffer[1];

   uintptr = reinterpret_cast<unsigned int*>(imbuffer);
   
   if(*uintptr == 0xffffffff)  //this designates an implicit sequence item with undefined length
   {
    std::pair<unsigned short,unsigned short> termtag = std::make_pair<unsigned short, unsigned short>(0,0);  //the element tag for a single sequence item
    
    while((termtag.first != (0xfffe)) && (termtag.second != (0xe0dd)))  //not equal the sequence delimitation item tag
    {
     hdrifile->readFromFileStream(twobytebuffer,2);  //the item tag
     ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);
     termtag.first = (*ushortptr);

     hdrifile->readFromFileStream(twobytebuffer,2); //the item length
     ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);
     termtag.second = (*ushortptr);

     
    }

    hdrifile->skipBytes(4);  //skips empty four bytes at the end of a sequence
    

   }
   else  //sequence is of implicit VR with explicit length
   {

    if(elementtag.first != 0xfffe)  //denotes the item tag
    {
     vr = "CS";  //sets the VR to CS by default, designating it as a string
     vl = (*uintptr);  //gets the explicit length
     endfile = this->readStrElement(hdrifile,vr,vl,elementtag);  //reads in the string element into the string map
    }
    else  //no item, so skip these bytes
     hdrifile->skipBytes(*uintptr);
   }

       
  }
 }

 return endfile;  //returns the status of the read file (true = EOF/error)

}

template<class T>
void DICOMVolumeHeaderV1N2<T>::readNumElement(ReadFromBinaryFileV1N1<char> *hdrifile, std::string vr, unsigned int vl, std::pair<unsigned short, unsigned short> elementtag)
//reads in the numeric element into the num map
{

 char fourbytebuffer[4];
 char twobytebuffer[2];
 char eightbytebuffer[8];
 unsigned short int *ushortptr;
 float *floatptr;
 double *doubleptr;
 unsigned long int *ulongptr;
 signed long int *slongptr;
 signed short int *sshortptr;
 unsigned short int ushortval;
 float floatval;
 double doubleval;

 short int vm;  //this is the value multiplicity, obtained by dividing the length of vr by vl; DICOM supports multiplicity of values for a single element
 double elementval;

 if(vr == "UL")  //unsigned long int
  {
   vm = vl/4;  //value multiplicity in 4 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(fourbytebuffer,4);
    ulongptr = reinterpret_cast<unsigned long int*>(fourbytebuffer);
    elementval = static_cast<double>(*ulongptr);  //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
    
   }
  }
  else if(vr == "US")  //unsigned short
  {
   vm = vl/2;  //value multiplicity in 2 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(twobytebuffer,2);
    ushortptr = reinterpret_cast<unsigned short int*>(twobytebuffer);
    ushortval = *ushortptr;
    elementval = static_cast<double>(ushortval); //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
   }

  }
  else if(vr == "FD")  //double
  {
   vm = vl/8;  //value multiplicity in 8 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(eightbytebuffer,8);
    doubleptr = reinterpret_cast<double*>(eightbytebuffer);
    doubleval = *doubleptr;
    elementval = doubleval;  //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
        
   }

  }
  else if(vr == "FL")  //float
  {
   vm = vl/4; //value multiplicity in 4 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(fourbytebuffer,4);
    floatptr = reinterpret_cast<float*>(fourbytebuffer);
    floatval = *floatptr;
    elementval = static_cast<double>(floatval);  //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
    
   }

  }
  else if(vr == "SL")  //signed long int
  {
   vm = vl/4;  //value multiplicity in 4 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(fourbytebuffer,4);
    slongptr = reinterpret_cast<signed long int*>(fourbytebuffer);
    elementval = static_cast<double>(*slongptr);  //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
    
   }

  }
  else if(vr == "SS")  //signed short
  {
   vm = vl/2;  //value multiplicity in 4 bytes
   for(int idx = 0; idx < vm; idx++)
   {
    hdrifile->readFromFileStream(twobytebuffer,2);
    sshortptr = reinterpret_cast<signed short int*>(twobytebuffer);
    elementval = static_cast<double>(*sshortptr);  //converts val to a double by default
    this->nummap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,double>(elementtag,elementval));  //inserts the value into the map with the key
    
   }

  }





}

template<class T>
bool DICOMVolumeHeaderV1N2<T>::readStrElement(ReadFromBinaryFileV1N1<char> *hdrifile, std::string vr, unsigned int vl, std::pair<unsigned short, unsigned short> elementtag)
//reads in the string to the string map with the appropriate key
{

 char UNbuffer[500000];  //large buffer for holding string value
 hdrifile->readFromFileStream(UNbuffer,vl);  //reads the designated number of bytes from the stream
 
 if(!hdrifile->reachedEOF())  //EOF has not been reached
 {
  std::string strval(UNbuffer);
  if((strval.size()) > (vl))  //if the string read in has more bytes than the VL specified, then erase all the bytes after that
  {
   strval.erase(vl);
  }

  this->strmap.insert(std::make_pair<std::pair<unsigned short,unsigned short>,std::string>(elementtag,strval));  //inserts the string into the string map with approriate key
    
 }
 else
  return true;  //EOF has been reached

 return false;




}

//WRITE FUNCTIONS
template<class T>
void DICOMVolumeHeaderV1N2<T>::writeOutHeaderFile(const FileNameV1N1<> *fnamev) const 
//reads in an DICOM header file that is part of fname
{

 //CODE GOES HERE

}

#endif

template<class T>
class DTITensorVolumeV1N2{

public:
 //CONSTRUCTORS
 DTITensorVolumeV1N2();  //empty constructor
 
 
protected:

};

//CONSTRUCTORS
template<class T>
DTITensorVolumeV1N2<T>::DTITensorVolumeV1N2()
//empty constructor
{

}



template<class T>
class DTITensorVolume3DV1N2 : public DTITensorVolumeV1N2<T>{

public:
 //CONSTRUCTORS
 DTITensorVolume3DV1N2();  //empty constructor
 DTITensorVolume3DV1N2(int RDIMv, int CDIMv, int ZDIMv, T element = T(), double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLEGNTHv = 1.0); //initializes all the image volumes with T() 
 DTITensorVolume3DV1N2(const DTITensorVolume3DV1N2<T> &copy);  //copy constructor
 ~DTITensorVolume3DV1N2(); //destructor

 //ASSIGNMENT FUNCTIONS
 void initTensorVolume(int RDIMv, int CDIMv, int ZDIMv, T element = T(), double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLEGNTHv = 1.0); //initializes all the image volumes with T()

 //MATH FUNCTIONS
 void correctTensorElement(int ridx, int cidx, int zidx);  //corrects the tensor at (ridx,cidx,zidx) for negative values
 
 //GET FUNCTIONS
 int getRDIM() const {return RDIM;}  //gets the dimensions of the tensor volume
 int getCDIM() const {return CDIM;}
 int getZDIM() const {return ZDIM;}
 double getRLENGTH() const {return RLENGTH;}  //gets the voxel lengths of the tensor volume
 double getCLENGTH() const {return CLENGTH;}
 double getZLENGTH() const {return ZLENGTH;}
 Matrix2DV2N4<T> getTensorElementAsMatrix(int i,int j, int k) const;  //return the tensor element at (i,j,k) as a 3x3 Matrix2D object
 Matrix2DV2N4<T> getTensorElementAsMatrix(int idx) const; //return the tensor element at idx as a 3x3 Matrix2D object
 VectorV2N4<T> getTensorElementAsVector(int idx) const; //return the tensor element at idx as a 6x1 Vector object with the format [dxx dyy dzz dxy dxz dyz]
 const ImageVolumeV2N4<T>* getDxx() const {return (this->dxx);}  //gets the const references to individual tensor element volumes
 const ImageVolumeV2N4<T>* getDyy() const {return (this->dyy);}
 const ImageVolumeV2N4<T>* getDzz() const {return (this->dzz);}
 const ImageVolumeV2N4<T>* getDxy() const {return (this->dxy);}
 const ImageVolumeV2N4<T>* getDxz() const {return (this->dxz);}
 const ImageVolumeV2N4<T>* getDyz() const {return (this->dyz);}

 //SET FUNCTIONS
 void setTensorElement(int ridx, int cidx, int zidx, const Matrix2DV2N4<T> &tensor);  //copies the tensor in (ridx,cidx,zidx)
 void setTensorElement(int idx, const Matrix2DV2N4<T> &tensor);  //copies the tensor in (ridx,cidx,zidx)
 void setCorrectedTensorElement(int ridx, int cidx, int zidx, const Matrix2DV2N4<T> &tensor);  //corrects the tensor for any negative values and copies it into (ridx,cidx,zidx)
 void negateYDir();  //negates the elements which have a single y (dxy, dyz)

 //PRINT FUNCTIONS
 void printTensorElement(int ridx, int cidx, int zidx);  //prints the tensor element at position (ridx,cidx,zidx)

 
 //READ FUNCTIONS
 void readInTensorElementImageVolumes(const ImageVolumeHeaderV1N2<T> **hdrlist);  //reads in the individual tensor volumes in 
 //Analyze image format.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz]
 
 //void readInAsAnalyzeImage(const FileNameV1N1<> *ifimgname);  //read in the whole tensor volume from one Analyze file
 //void readInAsNRRDImage(const FileNameV1N1<> *ifimgname);  //read in the whole tensor volume from one NRRD file
 //template<class S>
 //void readInConvertedTensorElementAnalyzeImageVolumes(FileNameV1N1<> **fnamelist, S scalefactor);  //reads in the individual tensor volumes in 
 //Analyze image format with datatype S - convert to T.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz]
 //template<class S>
 //void readInConvertedTensorElementCombinedNiftiImageVolumes(FileNameV1N1<> **fnamelist, S scalefactor);

 //WRTIE FUNCTIONS
 void writeOutTensorElementImageVolumes(const FileNameV1N1<> **hdrfnamelist, const ImageVolumeHeaderV1N2<T> **hdrlist);
 
 //void writeOutElementVolumesAsImageVolumes(const FileNameV1N1<> *ofimgname, bool shouldbyteswapv);  //writes out each of the individual tensor element volumes as an Analyze img volume.
 //void writeOutAsImageVolume(const ImageVolumeHeaderV1N2<T> *hdrv); //REDO THIS FUNCTION SO IT OUTPUTS ONE BIG ANALYZE VOLUME
 
 //template<class S>
 //void writeOutConvertedElementVolumesAsAnalyzeVolumes(std::string ofprefix, bool shouldbyteswapv, S scalefactor);  //writes out each of the individual tensor element volumes as 
 //an Analyze img volume with datatype S


protected:
 ImageVolumeV2N4<T> *dxx;  //the individual tensor element volumes
 ImageVolumeV2N4<T> *dyy;
 ImageVolumeV2N4<T> *dzz;
 ImageVolumeV2N4<T> *dxy;
 ImageVolumeV2N4<T> *dxz;
 ImageVolumeV2N4<T> *dyz;

 int RDIM;  //the tensor volume dimensions
 int CDIM;
 int ZDIM;
 double RLENGTH;  //the tensor volume voxel lengths
 double CLENGTH;
 double ZLENGTH;
 
};


template<class T>
DTITensorVolume3DV1N2<T>::DTITensorVolume3DV1N2()
//empty constructor
:DTITensorVolumeV1N2<T>()
{
 dxx = NULL;
 dyy = NULL;
 dzz = NULL;
 dxy = NULL;
 dxz = NULL;
 dyz = NULL;

}

template<class T>
DTITensorVolume3DV1N2<T>::DTITensorVolume3DV1N2(int RDIMv, int CDIMv, int ZDIMv, T element, double RLENGTHv, double CLENGTHv, double ZLENGTHv) 
//initializes all the image volumes with T() 
:DTITensorVolumeV1N2<T>(), RDIM(RDIMv), CDIM(CDIMv), ZDIM(ZDIMv), RLENGTH(RLENGTHv), CLENGTH(CLENGTHv), ZLENGTH(ZLENGTHv) 
{
 dxx = NULL;
 dyy = NULL;
 dzz = NULL;
 dxy = NULL;
 dxz = NULL;
 dyz = NULL;

 dxx = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dyy = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dzz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dxy = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dxz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dyz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);


}

template<class T>
DTITensorVolume3DV1N2<T>::DTITensorVolume3DV1N2(const DTITensorVolume3DV1N2<T> &copy)
//copy constructor
:DTITensorVolumeV1N2<T>(), RDIM(copy.getRDIM()), CDIM(copy.getCDIM()), ZDIM(copy.getZDIM()), RLENGTH(copy.getRLENGTH()), CLENGTH(copy.getCLENGTH()), ZLENGTH(copy.getZLENGTH()) 
{
 dxx = NULL;
 dyy = NULL;
 dzz = NULL;
 dxy = NULL;
 dxz = NULL;
 dyz = NULL;

 dxx = new ImageVolumeV2N4<T>(*(copy.getDxx()));
 dyy = new ImageVolumeV2N4<T>(*(copy.getDyy()));
 dzz = new ImageVolumeV2N4<T>(*(copy.getDzz()));
 dxy = new ImageVolumeV2N4<T>(*(copy.getDxy()));
 dxz = new ImageVolumeV2N4<T>(*(copy.getDxz()));
 dyz = new ImageVolumeV2N4<T>(*(copy.getDyz()));
 
 
}
 

template<class T>
void DTITensorVolume3DV1N2<T>::initTensorVolume(int RDIMv, int CDIMv, int ZDIMv, T element, double RLENGTHv, double CLENGTHv, double ZLENGTHv)
//initializes all the image volumes with T() or element
{
 //deletes any existing data
 if(dxx != NULL)
 {
  delete dxx;
  dxx = NULL;
 }

 if(dyy != NULL)
 {
  delete dyy;
  dyy = NULL;
 }

 if(dzz != NULL)
 {
  delete dzz;
  dzz = NULL;
 }

 if(dxy != NULL)
 {
  delete dxy;
  dxy = NULL;
 }

 if(dxz != NULL)
 {
  delete dxz;
  dxz = NULL;
 }

 if(dyz != NULL)
 {
  delete dyz;
  dyz = NULL;
 }


 dxx = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dyy = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dzz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dxy = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dxz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);
 dyz = new ImageVolumeV2N4<T>(RDIMv,CDIMv,ZDIMv,element,RLENGTHv,CLENGTHv,ZLENGTHv);

 
 RDIM = RDIMv;
 CDIM = CDIMv;
 ZDIM = ZDIMv;
 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;


}


template<class T>
DTITensorVolume3DV1N2<T>::~DTITensorVolume3DV1N2()  
//destructor
{

 //deletes any existing element data
 if(dxx != NULL)
 {
  delete dxx;
  dxx = NULL;
 }

 if(dyy != NULL)
 {
  delete dyy;
  dyy = NULL;
 }

 if(dzz != NULL)
 {
  delete dzz;
  dzz = NULL;
 }

 if(dxy != NULL)
 {
  delete dxy;
  dxy = NULL;
 }

 if(dxz != NULL)
 {
  delete dxz;
  dxz = NULL;
 }

 if(dyz != NULL)
 {
  delete dyz;
  dyz = NULL;
 }
 
}

//MATH FUNCTIONS
template<class T>
void DTITensorVolume3DV1N2<T>::correctTensorElement(int ridx, int cidx, int zidx)  
//corrects the tensor at (ridx,cidx,zidx) for negative values
{

 //empty unused function

}

//GET FUNCTIONS
template<class T>
Matrix2DV2N4<T> DTITensorVolume3DV1N2<T>::getTensorElementAsMatrix(int i, int j, int k) const
//return the tensor element at (i,j,k) as a 3x3 Matrix2D object
{
 Matrix2DV2N4<T> result(3,3);
 result.setElement(0,0,dxx->getElement(i,j,k));
 result.setElement(1,1,dyy->getElement(i,j,k));
 result.setElement(2,2,dzz->getElement(i,j,k));
 result.setElement(1,0,dxy->getElement(i,j,k));
 result.setElement(0,1,dxy->getElement(i,j,k));
 result.setElement(2,0,dxz->getElement(i,j,k));
 result.setElement(0,2,dxz->getElement(i,j,k));
 result.setElement(2,1,dyz->getElement(i,j,k));
 result.setElement(1,2,dyz->getElement(i,j,k));

 return result;

}

template<class T>
Matrix2DV2N4<T> DTITensorVolume3DV1N2<T>::getTensorElementAsMatrix(int idx) const
//return the tensor element at (idx) as a 3x3 Matrix2D object
{
 Matrix2DV2N4<T> result(3,3);
 result.setElement(0,0,dxx->getElement(idx));
 result.setElement(1,1,dyy->getElement(idx));
 result.setElement(2,2,dzz->getElement(idx));
 result.setElement(1,0,dxy->getElement(idx));
 result.setElement(0,1,dxy->getElement(idx));
 result.setElement(2,0,dxz->getElement(idx));
 result.setElement(0,2,dxz->getElement(idx));
 result.setElement(2,1,dyz->getElement(idx));
 result.setElement(1,2,dyz->getElement(idx));

 return result;

}

template<class T>
VectorV2N4<T> DTITensorVolume3DV1N2<T>::getTensorElementAsVector(int idx) const 
//return the tensor element at idx as a 6x1 Vector object with the format [dxx dyy dzz dxy dxz dyz]
{
 VectorV2N4<T> result(6,1);
 result.setElement(0,dxx->getElement(idx));
 result.setElement(1,dyy->getElement(idx));
 result.setElement(2,dzz->getElement(idx));
 result.setElement(3,dxy->getElement(idx));
 result.setElement(4,dxz->getElement(idx));
 result.setElement(5,dyz->getElement(idx));
 return result;


}


//SET FUNCTIONS
template<class T>
void DTITensorVolume3DV1N2<T>::setTensorElement(int ridx, int cidx, int zidx, const Matrix2DV2N4<T> &tensor)  
//copies the tensor in (ridx,cidx,zidx)
{
 if(tensor.getCDIM() == 3) //sets the tensor element if tensor is a 3x3 matrix
 {
  dxx->setElement(ridx,cidx,zidx,tensor.getElement(0,0));
  dyy->setElement(ridx,cidx,zidx,tensor.getElement(1,1));
  dzz->setElement(ridx,cidx,zidx,tensor.getElement(2,2));
  dxy->setElement(ridx,cidx,zidx,tensor.getElement(0,1));
  dxz->setElement(ridx,cidx,zidx,tensor.getElement(0,2));
  dyz->setElement(ridx,cidx,zidx,tensor.getElement(2,1));
 }
 else if(tensor.getCDIM() == 1)  //sets the tensor element if tensor is a 3x3 matrix
 {
  dxx->setElement(ridx,cidx,zidx,tensor.getElement(0,0));
  dyy->setElement(ridx,cidx,zidx,tensor.getElement(1,0));
  dzz->setElement(ridx,cidx,zidx,tensor.getElement(2,0));
  dxy->setElement(ridx,cidx,zidx,tensor.getElement(3,0));
  dxz->setElement(ridx,cidx,zidx,tensor.getElement(4,0));
  dyz->setElement(ridx,cidx,zidx,tensor.getElement(5,0));
 }
#if ERRORCHECK
 else
 {
  std::cout<<"ERROR in void DTITensorVolume3DV1N2<T>::setTensorElement(int ridx, int cidx, int zidx, const Matrix2DV2N4<T> &tensor)"<<std::endl;
  std::cout<<"No Valid Way of Entering Tensor"<<std::endl;
  exit(1);
 }
#endif


}

template<class T>
void DTITensorVolume3DV1N2<T>::setTensorElement(int idx, const Matrix2DV2N4<T> &tensor)  
//copies the tensor at idx
{
 if(tensor.getCDIM() == 3) //sets the tensor element if tensor is a 3x3 matrix
 {
  dxx->setElement(idx,tensor.getElement(0,0));
  dyy->setElement(idx,tensor.getElement(1,1));
  dzz->setElement(idx,tensor.getElement(2,2));
  dxy->setElement(idx,tensor.getElement(0,1));
  dxz->setElement(idx,tensor.getElement(0,2));
  dyz->setElement(idx,tensor.getElement(2,1));
 }
 else if(tensor.getCDIM() == 1) //sets the tensor element if tensor is a 6x1 vector
 {
  dxx->setElement(idx,tensor.getElement(0,0));
  dyy->setElement(idx,tensor.getElement(1,0));
  dzz->setElement(idx,tensor.getElement(2,0));
  dxy->setElement(idx,tensor.getElement(3,0));
  dxz->setElement(idx,tensor.getElement(4,0));
  dyz->setElement(idx,tensor.getElement(5,0));
 }
#if ERRORCHECK
 else
 {
  std::cout<<"ERROR in void DTITensorVolume3DV1N2<T>::setTensorElement(int idx, const Matrix2DV2N4<T> &tensor)"<<std::endl;
  std::cout<<"No Valid Way of Entering Tensor"<<std::endl;
  exit(1);
 }
#endif


}
 

template<class T>
void DTITensorVolume3DV1N2<T>::setCorrectedTensorElement(int ridx, int cidx, int zidx, const Matrix2DV2N4<T> &tensor)  
//corrects the tensor for any negative values and copies it into (ridx,cidx,zidx)
{
 this->setTensorElement(ridx,cidx,zidx,tensor);
 this->correctTensorElement(ridx,cidx,zidx);

}

template<class T>
void DTITensorVolume3DV1N2<T>::negateYDir()  
//negates the elements which have a single y (dxy, dyz)
{
 for(int idx = 0; idx < (this->dxx)->getSIZE(); idx++)
 {
  (this->dxy)->setElement(idx,-1.0*(this->dxy)->getElement(idx));
  (this->dyz)->setElement(idx,-1.0*(this->dxy)->getElement(idx));

 }


}



//PRINT FUNCTIONS
template<class T>
void DTITensorVolume3DV1N2<T>::printTensorElement(int ridx, int cidx, int zidx)  
//prints the tensor element at position (ridx,cidx,zidx)
{
 std::cout<<"DTI Tensor Element at Position ("<<ridx<<","<<cidx<<","<<zidx<<") :"<<std::endl;
 std::cout<<"dxx = "<<dxx->getElement(ridx,cidx,zidx)<<std::endl;
 std::cout<<"dyy = "<<dyy->getElement(ridx,cidx,zidx)<<std::endl;
 std::cout<<"dzz = "<<dzz->getElement(ridx,cidx,zidx)<<std::endl;
 std::cout<<"dxy = "<<dxy->getElement(ridx,cidx,zidx)<<std::endl;
 std::cout<<"dxz = "<<dxz->getElement(ridx,cidx,zidx)<<std::endl;
 std::cout<<"dyz = "<<dyz->getElement(ridx,cidx,zidx)<<std::endl;


}

//READ FUNCTIONS

template<class T>
void DTITensorVolume3DV1N2<T>::readInTensorElementImageVolumes(const ImageVolumeHeaderV1N2<T> **hdrlist)  
//reads in the individual tensor volumes in as image volumes.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz]
{

 dxx = new ImageVolumeV2N4<T>();
 dxx->readInImageVolumeFromFile(hdrlist[0]);

 dyy = new ImageVolumeV2N4<T>();
 dyy->readInImageVolumeFromFile(hdrlist[1]);

 dzz = new ImageVolumeV2N4<T>();
 dzz->readInImageVolumeFromFile(hdrlist[2]);

 dxy = new ImageVolumeV2N4<T>();
 dxy->readInImageVolumeFromFile(hdrlist[3]);

 dxz = new ImageVolumeV2N4<T>();
 dxz->readInImageVolumeFromFile(hdrlist[4]);

 dyz = new ImageVolumeV2N4<T>();
 dyz->readInImageVolumeFromFile(hdrlist[5]);

 this->RDIM = dxx->getRDIM();
 this->CDIM = dxx->getCDIM();
 this->ZDIM = dxx->getZDIM();
 
 this->RLENGTH = dxx->getRLENGTH();
 this->CLENGTH = dxx->getCLENGTH();
 this->ZLENGTH = dxx->getZLENGTH();

}

template<class T>
void DTITensorVolume3DV1N2<T>::writeOutTensorElementImageVolumes(const FileNameV1N1<> **hdrfnamelist, const ImageVolumeHeaderV1N2<T> **hdrlist)  
//reads in the individual tensor volumes in as image volumes.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz]
{

 dxx->writeOutImageVolumeToFile(hdrfnamelist[0],hdrlist[0]);
 dyy->writeOutImageVolumeToFile(hdrfnamelist[1],hdrlist[1]);
 dzz->writeOutImageVolumeToFile(hdrfnamelist[2],hdrlist[2]);
 dxy->writeOutImageVolumeToFile(hdrfnamelist[3],hdrlist[3]);
 dxz->writeOutImageVolumeToFile(hdrfnamelist[4],hdrlist[4]);
 dyz->writeOutImageVolumeToFile(hdrfnamelist[5],hdrlist[5]);


}


/*
template<class T>
void DTITensorVolume3DV1N2<T>::readInAsAnalyzeImage(const FileNameV1N1<> *ifimgnamev)
//read in whole tensor volume from an Analyze image file
{
 FileNameV1N1<> *ifimgname = (ifimgnamev->clone());

 if(ifimgname->getExtension() != "img")
  ifimgname->setExtension("img");

 AnalyzeVolumeHeaderV1N1<T> anlzhdr(ifimgname);
 this->initTensorVolume(anlzhdr.getRDIM(), anlzhdr.getCDIM(), anlzhdr.getZDIM(), T(), anlzhdr.getRLENGTH(), anlzhdr.getCLENGTH(), anlzhdr.getZLENGTH());

 ReadFromBinaryFileV1N1<T> imgfile(anlzhdr.getFNamePtr());  //associates a binary read stream with the input filename
 imgfile.openForRead();

 const int BYTENUM = (anlzhdr.getBitsPerVoxel())/8;  //the number of bytes for each voxel to write
 char *buffer = new char[BYTENUM];
 T *ptr;  //the current voxel to read

#if VERBOSE
 std::cout<<"Reading in tensor volume from Analyze image file = "<<(anlzhdr.getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif

#if VERBOSE
 std::cout<<"Reading in dxx..."<<std::endl;
#endif

 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {

    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dxx->setElement(ridx,cidx,zidx,*ptr);
    
     
   }
 }

#if VERBOSE
 std::cout<<"Reading in dyy..."<<std::endl;
#endif
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dyy->setElement(ridx,cidx,zidx,*ptr);
    
   }
 }

#if VERBOSE
 std::cout<<"Reading in dzz..."<<std::endl;
#endif

 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dzz->setElement(ridx,cidx,zidx,*ptr);
   
     
   }
 }

#if VERBOSE
 std::cout<<"Reading in dxy..."<<std::endl;
#endif
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dxy->setElement(ridx,cidx,zidx,*ptr);
   
     
   }
 }

#if VERBOSE
 std::cout<<"Reading in dxz..."<<std::endl;
#endif
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dxz->setElement(ridx,cidx,zidx,*ptr);
   
     
   }
 }

#if VERBOSE
 std::cout<<"Reading in dyz..."<<std::endl;
#endif
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    imgfile.readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    dyz->setElement(ridx,cidx,zidx,*ptr);
   
     
   }
 }

 
 imgfile.close();
 delete ifimgname;
 delete[] buffer;


}

template<class T>
void DTITensorVolume3DV1N2<T>::readInAsNRRDImage(const FileNameV1N1<> *ifimgnamev)
//read in the whole tensor volume from one NRRD file
{

#if 0
 FileNameV1N1<> *ifimgname = (ifimgnamev->clone());

 //if no .nrrd extension, tries to open a NRRD image file with the same filename
 if(ifimgname->getExtension() != "nrrd")
  ifimgname->setExtension("nrrd");

 NRRDVolumeHeaderV1N1<T> nrrdhdr(ifimgname);
 this->initTensorVolume(nrrdhdr.getRDIM(), nrrdhdr.getCDIM(), nrrdhdr.getZDIM(), T(), nrrdhdr.getRLENGTH(), nrrdhdr.getCLENGTH(), nrrdhdr.getZLENGTH());

 //ReadFromBinaryFileV1N1<T> imgfile(nrrdhdr.getFNamePtr());  //associates a binary read stream with the input filename
 //imgfile.openForRead();

#if VERBOSE
 std::cout<<"Reading in tensor volume from NRRD image file = "<<(nrrdhdr.getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif


 const int BYTENUM = (nrrdhdr.getBitsPerVoxel())/8;  //the number of bytes for each voxel to write
 
 
 char *buffer = new char[BYTENUM];
 T *ptr;  //the current voxel to read

 izstream *zstreamptr = new izstream();
 zstreamptr->open((ifimgname->getOSCompatibleFileName()).c_str());

// void *vbuf = new void[BYTENUM];
// void *tempbuf = new void[_CVTBUFSIZE];
 void *vbuf = new char[BYTENUM];
 //void *tempbuf = new char[_CVTBUFSIZE];
 void *tempbuf = new char[400];
 //char *vcharbuf = new char[BYTENUM];
 char *vcharbuf = new char[1];
 
 //zstreamptr->read(tempbuf,391);

 int bytesread = BYTENUM;
 int totalbytesread = 0;
 float *valptr = NULL;

 //zstreamptr->myseek(392);
 std::cout<<"Starting Position: "<<zstreamptr->mytell()<<std::endl;

 while(1)
 {
  zstreamptr->read(vbuf,1);
  vcharbuf = reinterpret_cast<char*>(vbuf);
  std::cout<<*vcharbuf<<std::endl;
  //hageman::byteswap<char>(vcharbuf,BYTENUM);
  //std::cout<<*vcharbuf<<std::endl;
  
  unsigned char *ucharptr;
  ucharptr = reinterpret_cast<unsigned char*>(vbuf);
  std::cout<<*ucharptr<<std::endl;
  //hageman::byteswap<unsigned char>(vcharbuf,);
  //std::cout<<*ucharptr<<std::endl;
  
  char ans = 'y';
  std::cin>>ans;
  
 }
  

#if 0
 while(bytesread == BYTENUM)
 {
  bytesread = zstreamptr->read(vbuf,BYTENUM);
  totalbytesread += bytesread;
  char ans = 'y';
  vcharbuf = reinterpret_cast<char*>(vbuf);
  std::cout<<*vcharbuf<<std::endl;
  valptr = reinterpret_cast<float*>(vcharbuf);
  std::cout<<*valptr<<std::endl;
  valptr = reinterpret_cast<float*>(vbuf);
  std::cout<<*valptr<<std::endl;
  
  hageman::byteswap<char>(vcharbuf,BYTENUM);
  std::cout<<*vcharbuf<<std::endl;
  valptr = reinterpret_cast<float*>(vcharbuf);
  std::cout<<*valptr<<std::endl;
  
  std::cin>>ans;
  
 }
#endif

 zstreamptr->close();
 delete zstreamptr;
 delete[] tempbuf;
 delete[] vbuf;

//#if VALIDATE
 std::cout<<"Total bytes read: "<<totalbytesread<<std::endl;
//#endif


#if VERBOSE
 std::cout<<"Reading in dxx..."<<std::endl;
#endif

#endif



}
 

template<class T>
void DTITensorVolume3DV1N2<T>::readInTensorElementAnalyzeImageVolumes(const FileNameV1N1<> **fnamelist)  //reads in the individual tensor volumes in 
//Analyze image format.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz]
{
 dxx = new ImageVolumeV2N4<T>();
 dxx->readInAnalyzeVolumeFromFile(fnamelist[0]);

 dyy = new ImageVolumeV2N4<T>();
 dyy->readInAnalyzeVolumeFromFile(fnamelist[1]);

 dzz = new ImageVolumeV2N4<T>();
 dzz->readInAnalyzeVolumeFromFile(fnamelist[2]);

 dxy = new ImageVolumeV2N4<T>();
 dxy->readInAnalyzeVolumeFromFile(fnamelist[3]);

 dxz = new ImageVolumeV2N4<T>();
 dxz->readInAnalyzeVolumeFromFile(fnamelist[4]);

 dyz = new ImageVolumeV2N4<T>();
 dyz->readInAnalyzeVolumeFromFile(fnamelist[5]);

 this->RDIM = dxx->getRDIM();
 this->CDIM = dxx->getCDIM();
 this->ZDIM = dxx->getZDIM();
 
 this->RLENGTH = dxx->getRLENGTH();
 this->CLENGTH = dxx->getCLENGTH();
 this->ZLENGTH = dxx->getZLENGTH();

 
}

template<class T>
template<class S>  //the datatype to write out to the Analyze image file
void DTITensorVolume3DV1N2<T>::readInConvertedTensorElementAnalyzeImageVolumes(FileNameV1N1<> **fnamelist, S scalefactor)  
//reads in the individual tensor volumes in Analyze image format.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz].  The optional scalefactor will be multiplied 
//into the current voxel value - set to 0 if you do not wish any scaling
{
 dxx = new ImageVolumeV2N4<T>();
 dxx->readInConvertedAnalyzeVolumeFromFile(fnamelist[0],scalefactor);

 dyy = new ImageVolumeV2N4<T>();
 dyy->readInConvertedAnalyzeVolumeFromFile(fnamelist[1],scalefactor);

 dzz = new ImageVolumeV2N4<T>();
 dzz->readInConvertedAnalyzeVolumeFromFile(fnamelist[2],scalefactor);

 dxy = new ImageVolumeV2N4<T>();
 dxy->readInConvertedAnalyzeVolumeFromFile(fnamelist[3],scalefactor);

 dxz = new ImageVolumeV2N4<T>();
 dxz->readInConvertedAnalyzeVolumeFromFile(fnamelist[4],scalefactor);

 dyz = new ImageVolumeV2N4<T>();
 dyz->readInConvertedAnalyzeVolumeFromFile(fnamelist[5],scalefactor);

 this->RDIM = dxx->getRDIM();
 this->CDIM = dxx->getCDIM();
 this->ZDIM = dxx->getZDIM();
 
 this->RLENGTH = dxx->getRLENGTH();
 this->CLENGTH = dxx->getCLENGTH();
 this->ZLENGTH = dxx->getZLENGTH();
}

template<class T>
template<class S>  //the datatype to write out to the Analyze image file
void DTITensorVolume3DV1N2<T>::readInConvertedTensorElementCombinedNiftiImageVolumes(FileNameV1N1<> **fnamelist, S scalefactor)  
//reads in the individual tensor volumes in Analyze image format.  The format of fnamelist is assumed to be [dxx dyy dzz dxy dxz dyz].  The optional scalefactor will be multiplied 
//into the current voxel value - set to 0 if you do not wish any scaling
{
 dxx = new ImageVolumeV2N4<T>();
 dxx->readInCombinedNiftiVolumeFromFile(fnamelist[0]);
 //dxx->readInCombinedNiftiVolumeFromFile(fnamelist[0],scalefactor);
 
 dyy = new ImageVolumeV2N4<T>();
 dyy->readInCombinedNiftiVolumeFromFile(fnamelist[1]);
 //dyy->readInCombinedNiftiVolumeFromFile(fnamelist[1],scalefactor);
 
 dzz = new ImageVolumeV2N4<T>();
 dzz->readInCombinedNiftiVolumeFromFile(fnamelist[2]);
 //dzz->readInCombinedNiftiVolumeFromFile(fnamelist[2],scalefactor);
 
 dxy = new ImageVolumeV2N4<T>();
 dxy->readInCombinedNiftiVolumeFromFile(fnamelist[3]);
 //dxy->readInCombinedNiftiVolumeFromFile(fnamelist[3],scalefactor);
 
 dxz = new ImageVolumeV2N4<T>();
 dxz->readInCombinedNiftiVolumeFromFile(fnamelist[4]);
 //dxz->readInCombinedNiftiVolumeFromFile(fnamelist[4],scalefactor);
 
 dyz = new ImageVolumeV2N4<T>();
 dyz->readInCombinedNiftiVolumeFromFile(fnamelist[5]);
 //dyz->readInCombinedNiftiVolumeFromFile(fnamelist[5],scalefactor);
 
 this->RDIM = dxx->getRDIM();
 this->CDIM = dxx->getCDIM();
 this->ZDIM = dxx->getZDIM();
 
 this->RLENGTH = dxx->getRLENGTH();
 this->CLENGTH = dxx->getCLENGTH();
 this->ZLENGTH = dxx->getZLENGTH();
}

*/




//WRTIE FUNCTIONS


/*
template<class T>
void DTITensorVolume3DV1N2<T>::writeOutElementVolumesAsAnalyzeVolumes(const FileNameV1N1<> *ofprefix, bool shouldbyteswapv)  
//writes out each of the individual tensor element volumes as an Analyze img volume.
{
 std::string dxxfnamestr(ofprefix->getOSCompatibleFileName());
 dxxfnamestr += "_dxx.img";
 hageman::FileNameV1N1<> *dxxfname = hageman::constructFileNameObjFromString(dxxfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxxfname);
 
 std::string dyyfnamestr(ofprefix->getOSCompatibleFileName());
 dyyfnamestr += "_dyy.img";
 hageman::FileNameV1N1<> *dyyfname = hageman::constructFileNameObjFromString(dyyfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dyyfname);
 
 std::string dzzfnamestr(ofprefix->getOSCompatibleFileName());
 dzzfnamestr += "_dzz.img";
 hageman::FileNameV1N1<> *dzzfname = hageman::constructFileNameObjFromString(dzzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dzzfname);
 
 std::string dxyfnamestr(ofprefix->getOSCompatibleFileName());
 dxyfnamestr += "_dxy.img";
 hageman::FileNameV1N1<> *dxyfname = hageman::constructFileNameObjFromString(dxyfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxyfname);
 
 std::string dxzfnamestr(ofprefix->getOSCompatibleFileName());
 dxzfnamestr += "_dxz.img";
 hageman::FileNameV1N1<> *dxzfname = hageman::constructFileNameObjFromString(dxzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxzfname);
 
 std::string dyzfnamestr(ofprefix->getOSCompatibleFileName());
 dyzfnamestr += "_dyz.img";
 hageman::FileNameV1N1<> *dyzfname = hageman::constructFileNameObjFromString(dyzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dyzfname);
 

 dxx->writeOutAnalyzeVolumeToFile(dxxfname,shouldbyteswapv);
 dyy->writeOutAnalyzeVolumeToFile(dyyfname,shouldbyteswapv);
 dzz->writeOutAnalyzeVolumeToFile(dzzfname,shouldbyteswapv);
 dxy->writeOutAnalyzeVolumeToFile(dxyfname,shouldbyteswapv);
 dxz->writeOutAnalyzeVolumeToFile(dxzfname,shouldbyteswapv);
 dyz->writeOutAnalyzeVolumeToFile(dyzfname,shouldbyteswapv);

 delete dxxfname;
 delete dyyfname;
 delete dzzfname;
 delete dxyfname;
 delete dxzfname;
 delete dyzfname;
 
 
}


template<class T>
template<class S>  //the datatype to write out to the Analyze image file
void DTITensorVolume3DV1N2<T>::writeOutConvertedElementVolumesAsAnalyzeVolumes(std::string ofprefix, bool shouldbyteswapv, S scalefactor)  
//writes out each of the individual tensor element volumes as an Analyze img volume.  The optional scalefactor will be multiplied  into the current voxel value - set to 0 
//if you do not wish any scaling
{
 std::string dxxfnamestr(ofprefix);
 dxxfnamestr += "_dxx.img";
 hageman::FileNameV1N1<> *dxxfname = hageman::constructFileNameObjFromString(dxxfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxxfname);
 
 std::string dyyfnamestr(ofprefix);
 dyyfnamestr += "_dyy.img";
 hageman::FileNameV1N1<> *dyyfname = hageman::constructFileNameObjFromString(dyyfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dyyfname);
 
 std::string dzzfnamestr(ofprefix);
 dzzfnamestr += "_dzz.img";
 hageman::FileNameV1N1<> *dzzfname = hageman::constructFileNameObjFromString(dzzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dzzfname);
 
 std::string dxyfnamestr(ofprefix);
 dxyfnamestr += "_dxy.img";
 hageman::FileNameV1N1<> *dxyfname = hageman::constructFileNameObjFromString(dxyfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxyfname);
 
 std::string dxzfnamestr(ofprefix);
 dxzfnamestr += "_dxz.img";
 hageman::FileNameV1N1<> *dxzfname = hageman::constructFileNameObjFromString(dxzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dxzfname);
 
 std::string dyzfnamestr(ofprefix);
 dyzfnamestr += "_dyz.img";
 hageman::FileNameV1N1<> *dyzfname = hageman::constructFileNameObjFromString(dyzfnamestr);
 hageman::changeMissingPathToCurrentDirectory(dyzfname);

 dxx->writeOutConvertedAnalyzeVolumeToFile(dxxfname,shouldbyteswapv,scalefactor);
 dyy->writeOutConvertedAnalyzeVolumeToFile(dyyfname,shouldbyteswapv,scalefactor);
 dzz->writeOutConvertedAnalyzeVolumeToFile(dzzfname,shouldbyteswapv,scalefactor);
 dxy->writeOutConvertedAnalyzeVolumeToFile(dxyfname,shouldbyteswapv,scalefactor);
 dxz->writeOutConvertedAnalyzeVolumeToFile(dxzfname,shouldbyteswapv,scalefactor);
 dyz->writeOutConvertedAnalyzeVolumeToFile(dyzfname,shouldbyteswapv,scalefactor);

 delete dxxfname;
 delete dyyfname;
 delete dzzfname;
 delete dxyfname;
 delete dxzfname;
 delete dyzfname;
 
}


 


template<class T>
void DTITensorVolume3DV1N2<T>::writeOutAsAnalyzeImage(const AnalyzeVolumeHeaderV1N1<T> &hdr)
//writes out the whole DTI tensor volume into one Analyze file
{
 
#if VERBOSE
 std::cout<<"Writing out Analyze header file = "<<(hdr.getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif

 hdr.writeOutHeaderFile();
 
 FileNameV1N1<> *ofimgname = (hdr.getFNamePtr())->clone();
 if(ofimgname->getExtension() != "img")
  ofimgname->setExtension("img");
  
 const int BYTENUM = (hdr.getBitsPerVoxel())/8;  //the number of bytes for each voxel to write
 T val = T();
 T *ptr;  //the current voxel value to write
 char *charptr = NULL;

#if VERBOSE
 std::cout<<"Writing out Analyze image file = "<<ofimgname->getOSCompatibleFileName()<<std::endl;
#endif

#if VERBOSE
  std::cout<<"Writing Out dxx..."<<std::endl;
#endif

 (dxx->obfile).openForWrite(ofimgname);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dxx->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dxx->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dxx->obfile).close();

#if VERBOSE
  std::cout<<"Writing Out dyy..."<<std::endl;
#endif

 (dyy->obfile).openForWrite(ofimgname,std::ios::app|std::ios::binary);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dyy->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dyy->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dyy->obfile).close();

#if VERBOSE
  std::cout<<"Writing Out dzz..."<<std::endl;
#endif


 (dzz->obfile).openForWrite(ofimgname,std::ios::app|std::ios::binary);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dzz->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dzz->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dzz->obfile).close();

#if VERBOSE
  std::cout<<"Writing Out dxy..."<<std::endl;
#endif


 (dxy->obfile).openForWrite(ofimgname,std::ios::app|std::ios::binary);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dxy->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dxy->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dxy->obfile).close();

#if VERBOSE
  std::cout<<"Writing Out dxz..."<<std::endl;
#endif

 (dxz->obfile).openForWrite(ofimgname,std::ios::app|std::ios::binary);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dxz->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dxz->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dxz->obfile).close();

#if VERBOSE
  std::cout<<"Writing Out dyz..."<<std::endl;
#endif

 (dyz->obfile).openForWrite(ofimgname,std::ios::app|std::ios::binary);
   
 for(int zidx = 0; zidx < ZDIM; zidx++)
 {
  for(int cidx = 0; cidx < CDIM; cidx++)
   for(int ridx = 0; ridx < RDIM; ridx++)
   {
    val = dyz->getElement(ridx,cidx,zidx);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (dyz->obfile).writeBytesToFileStream(charptr,BYTENUM);
     
   }
 }

 (dyz->obfile).close();

 delete ofimgname;


}

*/


template<class T>
class EigenVolumeV1N4{

public:
 //CONSTRUCTORS
 EigenVolumeV1N4(); //empty constructor
 


};

//CONSTRUCTORS

template<class T>
EigenVolumeV1N4<T>::EigenVolumeV1N4()
//empty default constructor
{

}



template<class T>
class EigenVolume3DV1N4 : public EigenVolumeV1N4<T>{

public:
 //CONSTRUCTOS
 EigenVolume3DV1N4(); //empty constructor
 EigenVolume3DV1N4(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);
 EigenVolume3DV1N4(int RDIMv, int CDIMv, int ZDIMv, EigenSystem3DV1N4<T> esys = EigenSystem3DV1N4<T>(), double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);
 //constructs a eigenvolume with esys = default
 EigenVolume3DV1N4(const DTITensorVolume3DV1N2<T> &dtivol);  //constructs an eigenvolume from the DTI tensor volume
 ~EigenVolume3DV1N4();  //destructor

 //ASSIGNMENT FUNCTIONS
 void initEigenVolume3D(int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv = 1.0, double CLENGTHv = 1.0, double ZLENGTHv = 1.0);
 
 //GET FUNCTIONS
 inline const T* getV1ConstReference(int i, int j, int k);  //returns the principle eigenvector v1 at (i,j,k) (in matrix voxel coords)
 inline const T* getV1ConstReference(const Vector3V2N4<double> &realpt);  //returns the principle eigenvector v1 at (i,j,k) (in matrix voxel coords) and at realpt (in real world coords)
 Vector3V2N4<T> getV1InterpolatedBSpline(const Vector3V2N4<double> &pt);  //returns the principle eigenvector at pt 
 //that has been interpolated using B Spline interpolation.
 Vector3V2N4<T> getV1InterpolatedPolynomial(const Vector3V2N4<double> &pt);  //returns the principle eigenvector at pt 
 //that has been interpolated using a 3D polynomial interpolation.
 Vector3V2N4<T> getV1InterpolatedTriLinear(const Vector3V2N4<double> &pt);  //returns the principle eigenvector at pt 
 //that has been interpolated using a trilinear interpolation.
 const EigenSystem3DV1N4<T>& getElement(int idx) const {return (evol->getElement(idx));}
 const EigenSystem3DV1N4<T>& getElement(int ridx, int cidx, int zidx) const;
 const Volume3DV2N4<EigenSystem3DV1N4<T> >* getEVolReference() const {return (this->evol);}
 Volume3DV2N4<EigenSystem3DV1N4<T> >* getNonConstEVolReference() {return (this->evol);}


 const int getRDIM() const {return evol->getRDIM();}
 const int getCDIM() const {return evol->getCDIM();}
 const int getZDIM() const {return evol->getZDIM();}
 const int getSIZE() const {return evol->getSIZE();}
 const double getRLENGTH() const {return RLENGTH;}
 const double getCLENGTH() const {return CLENGTH;}
 const double getZLENGTH() const {return ZLENGTH;}
 double getSmallestVoxelLength() const; 

 //SET FUNCTIONS
 inline void setElement(int i, int j, int k, const EigenSystem3DV1N4<T> &element);
 inline void setElement(int idx, const EigenSystem3DV1N4<T> &element);
 void setl1(int i, int j, int k, T l1v) {(evol->getNonConstElementReference(i,j,k)).l1 = l1v;}
 void setl2(int i, int j, int k, T l2v) {(evol->getNonConstElementReference(i,j,k)).l2 = l2v;}
 void setl3(int i, int j, int k, T l3v) {(evol->getNonConstElementReference(i,j,k)).l3 = l3v;}

 
 //MATH FUNCTIONS
 
 //DTI MATH FUNCTIONS
 void constructEigenVolume3D(const DTITensorVolume3DV1N2<T> &dtivol);
 void correctElementForDTI(int i, int j, int k);
 T meanDiffusivity(int i, int j, int k) {return ((evol->getElement(i,j,k)).meanDiffusivity());}
 T selfRegularTensorProduct(int i, int j, int k) {return ((evol->getElement(i,j,k)).selfRegularTensorProduct());}
 T selfDeviatoricTensorProduct(int i, int j, int k) {return ((evol->getElement(i,j,k)).selfDeviatoricTensorProduct());}
 T regularTensorProduct(int i, int j, int k, const EigenSystem3DV1N4<T> &op) {return ((evol->getElement(i,j,k)).regularTensorProduct());}
 T deviatoricTensorProduct(int i, int j, int k, const EigenSystem3DV1N4<T> &op) {return ((evol->getElement(i,j,k)).deviatoricTensorProduct());}
 void calculateFAVolume(ImageVolumeV2N4<T> &favol);
 void calculateMDVolume(ImageVolumeV2N4<T> &mdvol);
 void calculateLIVolume(ImageVolumeV2N4<T> &livol);
 T calculateLIPointUsingNN(int i, int j, int k);


 //PRINT FUNCTIONS
 void printElementCL(int i, int j, int k) const {evol->printElementCL(i,j,k);}
 void printVolInfo() const;

 //READ FUNCTIONS
 void readInAsAnalyzeImageForBSuite(const FileNameV1N1<> *imgfname);
 
 //void readInVolumeForBSuite(const FileNameV1N1<> *imgfname, const ImageVolumeHeaderV1N2<T> *hdr);
 //void readInAsAnalyzeImage(const FileNameV1N1<> *ifname);
 //void readInClarkEigenVolume(const FileNameV1N1<> *ifname);  //this is a function to read in Kristi Clark's format of EigenVolume 
 
 //WRITE FUNCTIONS
 void writeOutAsAnalyzeImageForBSuite(const FileNameV1N1<> *imgfname);
 
 //void writeOutVolumeForBSuite(const FileNameV1N1<> *hdrfname, const FileNameV1N1<> *imgfname, const ImageVolumeHeaderV1N2<T> *hdr); 
 //void writeOutToTxtFile(const FileNameV1N1<> *ofname);
 //void writeOutAsAnalyzeImage(const AnalyzeVolumeHeaderV1N2<T> &hdr);
 //void writeOutAsAnalyzeImage(const FileNameV1N1<> *ofname, bool shouldbyteswapv);
  
 
 //OVERLOADED OPERATORS


protected:
 Volume3DV2N4<EigenSystem3DV1N4<T> > *evol;
 double RLENGTH;
 double CLENGTH;
 double ZLENGTH;

 //these are volumes used by the BSpline interpolation routine.  
 ImageVolumeV2N4<T> *v1x;
 ImageVolumeV2N4<T> *v1y;
 ImageVolumeV2N4<T> *v1z;

 ImageVolumeV2N4<T> *y2xa;
 ImageVolumeV2N4<T> *y2ya;
 ImageVolumeV2N4<T> *y2za;
 


};


//CONSTRUCTORS
template<class T>
EigenVolume3DV1N4<T>::EigenVolume3DV1N4()
//empty constructor
:EigenVolumeV1N4<T>()
{
 evol = NULL;

 v1x = NULL;
 v1y = NULL;
 v1z = NULL;

 y2xa = NULL;
 y2ya = NULL;
 y2za = NULL;


}

template<class T>
EigenVolume3DV1N4<T>::EigenVolume3DV1N4(const FileNameV1N1<> *ifname, int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv, double CLENGTHv, double ZLENGTHv)
//a function for initializing using a txt file that contains the data for this eigenvol.  Function assumes ifname is has one value per line and assumes the 
//following read order:  the reading starts with zidx = 0, and reads all the
//rows for the first column, then goes to the second  and reads all the rows for the second column and so on...  When it is finished with all rows and
//columns for zidx = 0, it repeats the same pattern for zidx = 1 and so on...
//Each eigensystem data point is assumed to be output one value per line in the following way:
//lambda1
//lambda2
//lambda3
//mu1x
//mu1y
//mu1z
//mu2x
//mu2y
//mu2z
//mu3x
//mu3y
//mu3z
:EigenVolumeV1N4<T>(), RLENGTH(RLENGTHv), CLENGTH(CLENGTHv), ZLENGTH(ZLENGTHv)
{
 evol = NULL;
 
 v1x = NULL;
 v1y = NULL;
 v1z = NULL;

 y2xa = NULL;
 y2ya = NULL;
 y2za = NULL;


 evol = new Volume3DV2N4<EigenSystem3DV1N4<T> >(RDIMv,CDIMv,ZDIMv);

 const int SIZE = RDIMv*CDIMv*ZDIMv;

 int ptsread = 0;
 std::ifstream ifile((ifname->getOSCompatibleFileName()).c_str(), std::ios::in);

#if ERRORCHECK
 if(!ifile)
 {
  std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolumeV1N4()"<<std::endl;
  std::cout<<"Could not open input txt file to construct object.  Exiting..."<<std::endl;
  exit(1);
 }
#endif
 
 char buffer[80];
 T l1v,l2v,l3v;
 T mux,muy,muz;
 Vector3V2N4<T> v1,v2,v3;

#if VERBOSE
 std::cout<<"Reading in EigenVolume3D from Txt File = "<<(ifname->getOSCompatibleFileName())<<std::endl;
 std::cout<<"Total points to read = "<<SIZE<<std::endl;
#endif


 for(int k = 0; k < ZDIMv; k++)
  for(int j = 0; j < CDIMv; j++)
   for(int i = 0; i < RDIMv; i++)
   {
#if VERBOSE
    if((ptsread % 10000) == 0)
     std::cout<<"Pts Read = "<<ptsread<<" of "<<SIZE<<std::endl;
#endif

    //reading in lambda 1
    ifile.getline(buffer,80);  
    std::string l1str(buffer);
    if(l1str.empty())
    //a special handling of the first eigenvalue.  If an empty line is read in, it is assumed that EOF has been reached and the reading is terminated
     break;
    
    l1v = static_cast<T>(atof(l1str.c_str()));

    //reading in lambda 2
    ifile.getline(buffer,80);  
    std::string l2str(buffer);

#if ERRORCHECK
    if(l2str.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif
    
    l2v = static_cast<T>(atof(l2str.c_str()));

    //reading in lambda 3
    ifile.getline(buffer,80);  
    std::string l3str(buffer);
#if ERRORCHECK
    if(l3str.empty()) 
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif
    l3v = static_cast<T>(atof(l3str.c_str()));
    //reading in mu 1
    ifile.getline(buffer,80);
    std::string v1xstr(buffer);
#if ERRORCHECK
    if(v1xstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif    
    mux = static_cast<T>(atof(v1xstr.c_str()));
     
    ifile.getline(buffer,80);
    std::string v1ystr(buffer);
#if ERRORCHECK
    if(v1ystr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif    
    muy = static_cast<T>(atof(v1ystr.c_str()));

    ifile.getline(buffer,80);
    std::string v1zstr(buffer);
#if ERRORCHECK
    if(v1zstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    muz = static_cast<T>(atof(v1zstr.c_str()));

    v1.initVector3(mux,muy,muz);

    //reading in mu 2
    ifile.getline(buffer,80);
    std::string v2xstr(buffer);
#if ERRORCHECK
    if(v2xstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    mux = static_cast<T>(atof(v2xstr.c_str()));
     
    ifile.getline(buffer,80);
    std::string v2ystr(buffer);
#if ERRORCHECK
    if(v2ystr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    muy = static_cast<T>(atof(v2ystr.c_str()));

    ifile.getline(buffer,80);
    std::string v2zstr(buffer);
#if ERRORCHECK
    if(v2zstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif    
    muz = static_cast<T>(atof(v2zstr.c_str()));

    v2.initVector3(mux,muy,muz);

    //reading in mu 3
    ifile.getline(buffer,80);
    std::string v3xstr(buffer);
#if ERRORCHECK
    if(v3xstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    mux = static_cast<T>(atof(v3xstr.c_str()));
     
    ifile.getline(buffer,80);
    std::string v3ystr(buffer);
#if ERRORCHECK
    if(v3ystr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    muy = static_cast<T>(atof(v3ystr.c_str()));

    ifile.getline(buffer,80);
    std::string v3zstr(buffer);
#if ERRORCHECK
    if(v3zstr.empty())
    {
     std::cout<<"ERROR in EigenVolume3DV1N4<T>::EigenVolume3DV1N4().  Exiting..."<<std::endl;
     std::cout<<"EigenVolume3DV1N1 ERROR: Invalid format for reading in Eigensystem3."<<std::endl;
     exit(1);
    }
#endif 
    muz = static_cast<T>(atof(v3zstr.c_str()));

    v3.initVector3(mux,muy,muz);

    v1.normalize();
    v2.normalize();
    v3.normalize();

    T mu1[3];
    T mu2[3];
    T mu3[3];

    mu1[0] = v1.x();
    mu1[1] = v1.y();
    mu1[2] = v1.z();

    mu2[0] = v2.x();
    mu2[1] = v2.y();
    mu2[2] = v2.z();

    mu3[0] = v3.x();
    mu3[1] = v3.y();
    mu3[2] = v3.z();

    evol->setElement(i,j,k,EigenSystem3DV1N4<T>(l1v,l2v,l3v,mu1,mu2,mu3));
     
    ++ptsread;
     
   }


#if VERBOSE
 std::cout<<ptsread<<" Points Read!"<<std::endl;
#endif

}


template<class T>
EigenVolume3DV1N4<T>::EigenVolume3DV1N4(int RDIMv, int CDIMv, int ZDIMv, EigenSystem3DV1N4<T> esys, double RLENGTHv, double CLENGTHv, double ZLENGTHv)
//constructs a eigenvolume with esys = default
{
 evol = NULL;

 v1x = NULL;
 v1y = NULL;
 v1z = NULL;

 y2xa = NULL;
 y2ya = NULL;
 y2za = NULL;

 evol = new Volume3DV2N4<EigenSystem3DV1N4<T> >(RDIMv,CDIMv,ZDIMv,esys);
 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

}

template<class T>
EigenVolume3DV1N4<T>::EigenVolume3DV1N4(const DTITensorVolume3DV1N2<T> &dtivol)  
//constructs an eigenvolume from the DTI tensor volume
:EigenVolumeV1N4<T>()
{
 evol = NULL;

 v1x = NULL;
 v1y = NULL;
 v1z = NULL;

 y2xa = NULL;
 y2ya = NULL;
 y2za = NULL;

 this->constructEigenVolume3D(dtivol);

}
 

template<class T>
EigenVolume3DV1N4<T>::~EigenVolume3DV1N4()  
//destructor
{
 if(evol != NULL)
 {
  delete evol;
  evol = NULL;
 }

 //deletes any interpolation volumes
 if(v1x != NULL)
 {
  delete v1x;
  v1x = NULL;
 }

 if(v1y != NULL)
 {
  delete v1y;
  v1y = NULL;
 }

 if(v1z != NULL)
 {
  delete v1z;
  v1z = NULL;
 }

 if(y2xa != NULL)
 {
  delete y2xa;
  y2xa = NULL;
 }

 if(y2ya != NULL)
 {
  delete y2ya;
  y2ya = NULL;
 }

 if(y2za != NULL)
 {
  delete y2za;
  y2za = NULL;
 }
 
}

//ASSIGNMENT FUNCTIONS
template<class T>
void EigenVolume3DV1N4<T>::initEigenVolume3D(int RDIMv, int CDIMv, int ZDIMv, double RLENGTHv, double CLENGTHv, double ZLENGTHv)
//initializes an eigenvolume with default element
{

 //deletes any existing data
 if(evol != NULL)
 {
  delete evol;
  evol = NULL;

  v1x = NULL;
  v1y = NULL;
  v1z = NULL;

  y2xa = NULL;
  y2ya = NULL;
  y2za = NULL;

 }

 evol = new Volume3DV2N4<EigenSystem3DV1N4<T> >(RDIMv,CDIMv,ZDIMv);
 RLENGTH = RLENGTHv;
 CLENGTH = CLENGTHv;
 ZLENGTH = ZLENGTHv;

}

//GET FUNCTIONS
template<class T>
inline const T* EigenVolume3DV1N4<T>::getV1ConstReference(int i, int j, int k)  
//returns the principle eigenvector v1 at matrix voxel coordinates (i,j,k)
{
 return ((evol->getElement(i,j,k)).v1);

}

template<class T>
inline const T* EigenVolume3DV1N4<T>::getV1ConstReference(const Vector3V2N4<double> &realpt)  
//returns the principle eigenvector v1 at (i,j,k) (in matrix voxel coords) and at realpt (in real world coords)
{
 Vector3V2N4<int> voxelcoords(hageman::getVoxelCoordinatesForRealWorldPt(realpt,evol->getRDIM(), evol->getCDIM(), evol->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH()));
 return (this->getV1ConstReference(voxelcoords.x(), voxelcoords.y(), voxelcoords.z()));

}
 

template<class T>
Vector3V2N4<T> EigenVolume3DV1N4<T>::getV1InterpolatedBSpline(const Vector3V2N4<double> &pt)  
//returns the principle eigenvector at pt that has been interpolated using B Spline interpolation.
{
 //initialize these v1 pointers if they are equal to NULL; only need to check v1x because they are initialized together
 if(v1x == NULL) 
 {
  v1x = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());
  v1y = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());
  v1z = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());


  for(int k = 0; k < (evol->getZDIM()); k++)
   for(int j = 0; j < (evol->getCDIM()); j++)
    for(int i = 0; i < (evol->getRDIM()); i++)
    {
     v1x->setElement(i,j,k,(evol->getElement(i,j,k)).v1[0]);
     v1y->setElement(i,j,k,(evol->getElement(i,j,k)).v1[1]);
     v1z->setElement(i,j,k,(evol->getElement(i,j,k)).v1[2]);
    }
 }

 //initialize these y2 pointers if they are equal to NULL; only need to check y2xa because they are initialized together
 if(y2xa == NULL)
 {
  y2xa = new ImageVolumeV2N4<T>();
  y2ya = new ImageVolumeV2N4<T>();
  y2za = new ImageVolumeV2N4<T>();
 }
 
 //interpolates each dimension separately
 double xval = hageman::getInterpolatedBSplineValue(*v1x,pt.x(),pt.y(),pt.z(),*y2xa);
 double yval = hageman::getInterpolatedBSplineValue(*v1y,pt.x(),pt.y(),pt.z(),*y2ya);
 double zval = hageman::getInterpolatedBSplineValue(*v1z,pt.x(),pt.y(),pt.z(),*y2za);
 
 Vector3V2N4<double> result(xval,yval,zval);

 return (result);

}

template<class T>
Vector3V2N4<T> EigenVolume3DV1N4<T>::getV1InterpolatedPolynomial(const Vector3V2N4<double> &pt)
//returns the principle eigenvector at pt that has been interpolated using a 3D polynomial interpolation.
{

 //PUT CODE HERE

 return Vector3V2N4<double>();

}

template<class T>
Vector3V2N4<T> EigenVolume3DV1N4<T>::getV1InterpolatedTriLinear(const Vector3V2N4<double> &pt)  
//returns the principle eigenvector at pt that has been interpolated using a trilinear interpolation.
{

 //initialize these v1 pointers if they are equal to NULL; only need to check v1x because they are initialized together
 if(v1x == NULL) 
 {
  v1x = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());
  v1y = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());
  v1z = new ImageVolumeV2N4<T>(evol->getRDIM(), evol->getCDIM(), evol->getZDIM());


  for(int k = 0; k < (evol->getZDIM()); k++)
   for(int j = 0; j < (evol->getCDIM()); j++)
    for(int i = 0; i < (evol->getRDIM()); i++)
    {
     v1x->setElement(i,j,k,(evol->getConstElementReference(i,j,k)).v1[0]);
     v1y->setElement(i,j,k,(evol->getConstElementReference(i,j,k)).v1[1]);
     v1z->setElement(i,j,k,(evol->getConstElementReference(i,j,k)).v1[2]);
    }
 }

 //interpolates each dimension separately
 double xval = hageman::getInterpolatedTriLinearValue(*v1x,pt.x(),pt.y(),pt.z());
 double yval = hageman::getInterpolatedTriLinearValue(*v1y,pt.x(),pt.y(),pt.z());
 double zval = hageman::getInterpolatedTriLinearValue(*v1z,pt.x(),pt.y(),pt.z());
 
 Vector3V2N4<double> result(xval,yval,zval);

 return (result);

}

template<class T>
const EigenSystem3DV1N4<T>& EigenVolume3DV1N4<T>::getElement(int ridx, int cidx, int zidx) const
{
 return ((this->evol)->getConstElementReference(ridx,cidx,zidx));
}


//SET FUNCTIONS
template<class T>
inline void EigenVolume3DV1N4<T>::setElement(int i, int j, int k, const EigenSystem3DV1N4<T> &element)
{
 evol->setElement(i,j,k,element);
}

template<class T>
inline void EigenVolume3DV1N4<T>::setElement(int idx, const EigenSystem3DV1N4<T> &element)
{
 evol->setElement(idx,element);
}

template<class T>
double EigenVolume3DV1N4<T>::getSmallestVoxelLength() const
{
 double result = this->getRLENGTH();

 if(result > (this->getCLENGTH()))
  result = this->getCLENGTH();
 if(result > (this->getZLENGTH()))
  result = this->getZLENGTH();

 return result;
}


//MATH FUNCTIONS
template<class T>
void EigenVolume3DV1N4<T>::constructEigenVolume3D(const DTITensorVolume3DV1N2<T> &dtivol)
{

#if VERBOSE
 std::cout<<"Constructing eigenvolume from tensor volume..."<<std::endl;
#endif

 this->initEigenVolume3D(dtivol.getRDIM(),dtivol.getCDIM(),dtivol.getZDIM(),dtivol.getRLENGTH(),dtivol.getCLENGTH(),dtivol.getZLENGTH());
 
 T *jresult = new T[12];

 for(int k = 0; k < dtivol.getZDIM(); k++)
 {
#if VERBOSE
  std::cout<<"Computing Z = "<<k<<std::endl;
#endif
  for(int j = 0; j < dtivol.getCDIM(); j++)
   for(int i = 0; i < dtivol.getRDIM(); i++)
   {
    (dtivol.getTensorElementAsMatrix(i,j,k)).calculateJacobiEigenSystem3D(jresult);
    this->setElement(i,j,k,EigenSystem3DV1N4<T>(jresult));

#if VALIDATECOORDS
    if((i == VALR) && (j == VALC) && (k == VALZ))
    {
     std::cout<<"Eigensystem element at ("<<i<<","<<j<<","<<k<<"):"<<std::endl;
     this->printElementCL(i,j,k);
    }
     
#endif
    //this->correctElementForDTI(i,j,k);    
   }
 }

 delete[] jresult;


}

//DTI MATH FUNCTIONS
template<class T>
void EigenVolume3DV1N4<T>::calculateFAVolume(ImageVolumeV2N4<T> &favol)
{

#if VERBOSE
 std::cout<<"Constructing FA volume..."<<std::endl;
#endif

 for(int k = 0; k < getZDIM(); k++)
 {
#if VERBOSE
  std::cout<<"Computing Z = "<<k<<std::endl;
#endif
  for(int j = 0; j < getCDIM(); j++)
   for(int i = 0; i < getRDIM(); i++)
   {
    favol.setElement(i,j,k,(evol->getElement(i,j,k)).fractionalAnisotropy());
      
   }
 }


}

template<class T>
void EigenVolume3DV1N4<T>::calculateMDVolume(ImageVolumeV2N4<T> &mdvol)
{

 T result = T();

#if VERBOSE
 std::cout<<"Constructing MD volume..."<<std::endl;
#endif


 for(int k = 0; k < getZDIM(); k++)
 {
#if VERBOSE
  std::cout<<"Computing Z = "<<k<<std::endl;
#endif
  for(int j = 0; j < getCDIM(); j++)
   for(int i = 0; i < getRDIM(); i++)
   {
    result = this->meanDiffusivity(i,j,k);
    mdvol.setElement(i,j,k,result);
    
   }
 }


}

template<class T>
void EigenVolume3DV1N4<T>::calculateLIVolume(ImageVolumeV2N4<T> &livol)
{

#if VERBOSE
 std::cout<<"Constructing LI volume..."<<std::endl;
#endif


 for(int k = 0; k < getZDIM(); k++)
 {
#if VERBOSE
  std::cout<<"Computing Z = "<<k<<std::endl;
#endif
  for(int j = 0; j < getCDIM(); j++)
   for(int i = 0; i < getRDIM(); i++)
   {
    livol.setElement(i,j,k,this->calculateLIPointUsingNN(i,j,k));
        
   }
 }


}

template<class T>
T EigenVolume3DV1N4<T>::calculateLIPointUsingNN(int ir, int jr, int kr)
//computes the LI for a particular point (ir,jr,kr).  The kernel is just the nearest neighbors (but not the corner voxels) (=6 for an interior voxel)
{
 T latticeindex = T();
 int neighbors = 0;

 for(int i = -1; i <= 1; i++)
 {
  if(i == 0)  //skip the current voxel
   continue;
  else if(!(evol->inBounds(ir+i,jr,kr)))  //the voxel is outside the boundaries of the volume
   continue;
  else{
   ++neighbors;
   latticeindex += (evol->getElement(ir,jr,kr)).latticeIndex(evol->getElement(ir+i,jr,kr));
  }
 }

 for(int j = -1; j <= 1; j++)
 {
  if(j == 0)  //skip the current voxel
   continue;
  else if(!(evol->inBounds(ir,jr+j,kr)))  //the voxel is outside the boundaries of the volume
   continue;
  else{
   ++neighbors;
   latticeindex += (evol->getElement(ir,jr,kr)).latticeIndex(evol->getElement(ir,jr+j,kr));
  }
 }

 for(int k = -1; k <= 1; k++)
 {
  if(k == 0)  //skip the current voxel
   continue;
  else if(!(evol->inBounds(ir,jr,kr+k)))  //the voxel is outside the boundaries of the volume
   continue;
  else{
   ++neighbors;
   latticeindex += (evol->getElement(ir,jr,kr)).latticeIndex(evol->getElement(ir,jr,kr+k));
  }
 }
 
 if(neighbors > 0)  //don't try to divide the result if neighbors are equal to zero
  latticeindex = latticeindex * static_cast<T>(powf((double)neighbors,-1));

 if(latticeindex < T())  //if LI is less than zero, set it back to zero
  latticeindex = T();

 return latticeindex;

}


template<class T>
void EigenVolume3DV1N4<T>::correctElementForDTI(int i, int j, int k)
//correct the eigensystem for DTI errors
{
  (evol->getNonConstElementReference(i,j,k)).correctForDTI();
}


//PRINT FUNCTIONS
template<class T>
void EigenVolume3DV1N4<T>::printVolInfo() const
//prints out the volume dimensions and lengths
{

 evol->printVolInfo();  //calls Volume3D function
 std::cout<<"Voxel Lengths: "<<this->RLENGTH<<" "<<this->CLENGTH<<" "<<this->ZLENGTH<<std::endl;

}



//READ FUNCTIONS


template<class T>
void EigenVolume3DV1N4<T>::readInAsAnalyzeImageForBSuite(const FileNameV1N1<> *ifimgnamev)
//reads in an eigenvolume from an Analyze file that is compatible for visualization with BrainSuite
{

 //BRAINSUITE REQUIREMENTS:
 //datatype must be read in as float
 //each voxel is 48 bytes (4 bytes for each of the 12 values)
 //data CANNOT be byteswapped
 //AnalyzeVolumeHeader must read UNKNOWN for the datatype

 //initializes the dimensions of the volumes based on the header
 AnalyzeVolumeHeaderV1N2<float> hdr(ifimgnamev);  //must be float
 FileNameV1N1<> *ifimgname = ifimgnamev->clone();

 hdr.printHeader();
 
 if(ifimgname->getExtension() != ".img")
  ifimgname->setExtension(".img");

 this->initEigenVolume3D(hdr.getRDIM(), hdr.getCDIM(), hdr.getZDIM(), hdr.getRLENGTH(), hdr.getCLENGTH(), hdr.getZLENGTH());

 //opens up the input binary stream for evol for reading.  Attaches a new fname to it
 ((this->evol)->ibfile) = new ReadFromBinaryFileV1N1<>(ifimgname);
 ((this->evol)->ibfile)->openForRead();
 
 const int BYTENUM = 4;  //4 byte floats required by BS
 char *buffer = new char[BYTENUM];
 float *ptr;  //the current element read in from the stream
 EigenSystem3DV1N4<T> etemp;

#if VERBOSE
 std::cout<<"Reading in 3D eigenvolume from Analyze file (BrainSuite compatible) = "<<ifimgname->getOSCompatibleFileName()<<std::endl;
#endif

 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Reading Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v1[0] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v1[1] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v1[2] = static_cast<T>(*ptr);
 
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v2[0] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v2[1] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v2[2] = static_cast<T>(*ptr);
 

    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v3[0] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v3[1] = static_cast<T>(*ptr);
 
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.v3[2] = static_cast<T>(*ptr);

    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.l1 = static_cast<T>(*ptr);
    
    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.l2 = static_cast<T>(*ptr);

    (evol->ibfile)->readFromFileStream(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    etemp.l3 = static_cast<T>(*ptr);
 
    this->setElement(ridx,cidx,zidx,etemp);
    
   }
 }


 ((this->evol)->ibfile)->close();
 delete ifimgname;
 delete[] buffer;
 delete ((this->evol)->ibfile);
 ((this->evol)->ibfile) = NULL;
}

//template<class T>
//void EigenVolume3DV1N4<T>::readInVolumeForBSuite(const FileNameV1N1<> *imgfname, const ImageVolumeHeaderV1N2<T> *hdr)
//{

//}

//template<class T>
//void EigenVolume3DV1N4<T>::readInAsAnalyzeImageForBSuite(const FileNameV1N1<> *imgfname, const ImageVolumeHeaderV1N2<T> *hdr)
//{


//}
 


/*
template<class T>
void EigenVolume3DV1N4<T>::readInAsAnalyzeImage(const FileNameV1N1<> *ifimgnamev)
{
 FileNameV1N1<> *ifimgname = (ifimgnamev)->clone();
 AnalyzeVolumeHeaderV1N1<T> anlzhdr(ifimgnamev);

 if(ifimgname->getExtension() != ".img")
  ifimgname->setExtension(".img");

 #if VALIDATE
  std::cout<<"From function void EigenVolume3DV1N4<T>::readInAsAnalyzeImage(const FileNameV1N0<> *ifimgname)"<<std::endl;
  anlzhdr.printHeader();
 #endif

 this->initEigenVolume3D(anlzhdr.getRDIM(), anlzhdr.getCDIM(), anlzhdr.getZDIM(), anlzhdr.getRLENGTH(), anlzhdr.getCLENGTH(), anlzhdr.getZLENGTH());
 (evol->ibfile).setFileName(ifimgname);
 (evol->ibfile).openForRead();

 const int BYTENUM = (anlzhdr.getBitsPerVoxel())/8;  //the number of bytes for each element of the Analyze volume
 char *buffer = new char[BYTENUM];
 T *ptr;  //the current element read in from the stream
 EigenSystem3DV1N4<T> etemp;

#if VERBOSE
 std::cout<<"Reading in 3D eigenvolume from a text file = "<<ifimgname->getOSCompatibleFileName()<<std::endl;
#endif

 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Reading Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.l1 = *ptr;
    
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.l2 = *ptr;

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.l3 = *ptr;
 

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v1[0] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v1[1] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v1[2] = *ptr;
 
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v2[0] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v2[1] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v2[2] = *ptr;
 

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v3[0] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v3[1] = *ptr;
 
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(anlzhdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<T*>(buffer);
    
    etemp.v3[2] = *ptr;
 
    this->setElement(ridx,cidx,zidx,etemp);
    
   }
 }


 (evol->ibfile).close();
 delete ifimgname;
 delete[] buffer;


}



template<class T>
void EigenVolume3DV1N4<T>::readInClarkEigenVolume(const FileNameV1N1<> *ifimgnamev)  
//this is a function to read in Kristi Clark's format of 3D eigenvolume from a Analyze file
{

 //CLARK REQUIREMENTS:
 //datatype must be read in as float
 //each voxel is 48 bytes (4 bytes for each of the 12 values)
 //data CANNOT be byteswapped
 //AnalyzeVolumeHeader must read UNKNOWN for the datatype

 AnalyzeVolumeHeaderV1N1<float> hdr(ifimgnamev); 
 FileNameV1N1<> *ifimgname = ifimgnamev->clone();

 if(ifimgname->getExtension() != ".img")
  ifimgname->setExtension(".img");

 this->initEigenVolume3D(hdr.getRDIM(), hdr.getCDIM(), hdr.getZDIM(), hdr.getRLENGTH(), hdr.getCLENGTH(), hdr.getZLENGTH());

 //opens up the input binary stream for evol for reading.  Attaches a new fname to it
 ((this->evol)->ibfile).setFileName(ifimgname);
 ((this->evol)->ibfile).openForRead();
 
 const int BYTENUM = 4;  //reading in 4 byte floats which are required
 char *buffer = new char[BYTENUM];
 float *ptr;
 EigenSystem3DV1N4<T> etemp;
 T eval;

#if VERBOSE
 std::cout<<"Reading in 3D eigenvolume from Analyze file (Kristi Clark's format) = "<<ifimgname->getOSCompatibleFileName()<<std::endl;
 std::cout<<"Reading in lambda 1 "<<std::endl;
#endif

 //the default Analyze format runs across the columns first by default, then the second dimension is A-P, then third dimension is I-S
 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
  
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
  
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).l1 = eval;
   
   }
 }

#if VERBOSE 
 std::cout<<"Reading in lambda 2"<<std::endl;
#endif
  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    //etemp.l2 = static_cast<T>(*ptr);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).l2 = eval;
   }

  }

#if VERBOSE
 std::cout<<"Reading in lambda 3"<<std::endl;
#endif  

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    
    //etemp.l3 = static_cast<T>(*ptr);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).l3 = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 1[0] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v1[0] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 1[1] "<<std::endl;
#endif  

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);    
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v1[1] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 1[2] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v1[2] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 2[0] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v2[0] = eval;

   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 2[1] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);    
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v2[1] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 2[2] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v2[2] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 3[0] "<<std::endl;
#endif

  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v3[0] = eval;

   }
  }
  
#if VERBOSE
  std::cout<<"Reading in nu 3[1] "<<std::endl;
#endif


  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  {
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {

    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v3[1] = eval;
   }
  }

#if VERBOSE
  std::cout<<"Reading in nu 3[2] "<<std::endl;
#endif
  
  for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
  { 
   for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
   
    (evol->ibfile).readFromFileStream(buffer,BYTENUM);
    if(hdr.getShouldByteSwap()) //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(buffer,BYTENUM);
    ptr = reinterpret_cast<float*>(buffer);
    eval = static_cast<T>(*ptr);
    (evol->getNonConstElementReference(ridx,cidx,zidx)).v3[2] = eval;

   }
  }    

 (evol->ibfile).close();
 delete ifimgname;
 delete[] buffer;


}
*/



//WRITE FUNCTIONS

template<class T>
void EigenVolume3DV1N4<T>::writeOutAsAnalyzeImageForBSuite(const FileNameV1N1<> *ofimgname)
{

 //BRAINSUITE REQUIREMENTS:
 //datatype must be written out as float
 //each voxel must be 48 bytes
 //data CANNOT be byteswapped
 //AnalyzeVolumeHeader must read UNKNOWN for the datatype

 FileNameV1N1<> *ofhdrname = ofimgname->clone();
 if((ofhdrname->getExtension()) != ".hdr")
  ofhdrname->setExtension(".hdr");
 

 AnalyzeVolumeHeaderV1N2<float> hdr(ofimgname,hageman::UNKNOWNTYPE,8*48,false,this->getRDIM(), this->getCDIM(), this->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH());
 hdr.writeOutHeaderFile(ofhdrname);

 
 ((this->evol)->obfile) = new WriteToBinaryFileV1N1<>(ofimgname);
 (evol->obfile)->openForWrite();
 const int BYTENUM = 4;  //BS requires 4 byte floats
 float val = float();
 float *ptr;
 char *charptr = NULL;
 
#if VERBOSE
 std::cout<<"Reading in 3D eigenvolume from Analyze file (BrainSuite compatible) = "<<ofimgname->getOSCompatibleFileName()<<std::endl;
#endif

 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    //v1x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[0]);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v1y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[1]);    
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v1z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[2]);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v2x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[0]); 
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v2y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[1]);  
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v2z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[2]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);
    
    //v3x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[0]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v3y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[1]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //v3z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[2]);    
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);

    //l1
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l1);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);
        
    //l2
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l2);  
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);
    
    //l3
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l3);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile)->writeBytesToFileStream(charptr,BYTENUM);
    

   }
 }

 (evol->obfile)->close();
 delete ofhdrname;
 delete ((this->evol)->obfile);
 (this->evol)->obfile = NULL;
}

 

/*
template<class T>
void EigenVolume3DV1N4<T>::writeOutAsAnalyzeImage(const AnalyzeVolumeHeaderV1N1<T> &hdr)
//writes out the 3D eigenvolume as an Analyze file
{
 FileNameV1N1<> *ofimgname = (hdr.getFNamePtr())->clone();

 if((ofimgname->getExtension()) != "img")
  ofimgname->setExtension("img");

#if VERBOSE
 std::cout<<"Writing out Analyze header file = "<<(hdr.getFNamePtr())->getOSCompatibleFileName()<<std::endl;
#endif

 hdr.writeOutHeaderFile();

 (evol->obfile).setFileName(ofimgname);
 (evol->obfile).openForWrite();
 
 const int BYTENUM = (hdr.getBitsPerVoxel())/8;  //the number of bytes for each element in a voxel
 T val = T();
 T *ptr;  //the current element to write out
 char *charptr = NULL;

#if VERBOSE
 std::cout<<"Writing out 3D eigenvolume to Analyze image file = "<<ofimgname->getOSCompatibleFileName()<<std::endl;
#endif

 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    val = (evol->getReference(ridx,cidx,zidx)).l1;
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
        
    val = (evol->getReference(ridx,cidx,zidx)).l2;
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    
    val = (evol->getReference(ridx,cidx,zidx)).l3;
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    val = (evol->getReference(ridx,cidx,zidx)).v1[0];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v1[1];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v1[2];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v2[0];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v2[1];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v2[2];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    
    val = (evol->getReference(ridx,cidx,zidx)).v3[0];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v3[1];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);


    val = (evol->getReference(ridx,cidx,zidx)).v3[2];
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    

   }
 }

 (evol->obfile).close();
 delete ofimgname;

}

template<class T>
void EigenVolume3DV1N4<T>::writeOutAsAnalyzeImage(const FileNameV1N1<> *ofname, bool shouldbyteswapv)
//writes out the 3D eigenvolume as an Analyze file
{
 DataType dtv = hageman::getDataType(T());
 
 AnalyzeVolumeHeaderV1N1<T> hdr(ofname,ANALYZE,VOLUME,UNKNOWNTYPE,8*(getDataTypeByteSize(dtv)),shouldbyteswapv,this->getRDIM(), this->getCDIM(), this->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH());
 this->writeOutAsAnalyzeImage(hdr);
}

template<class T>
void EigenVolume3DV1N4<T>::writeOutAsAnalyzeImageForBSuite(const FileNameV1N1<> *ofname)
{

 //BRAINSUITE REQUIREMENTS:
 //datatype must be written out as float
 //each voxel must be 48 bytes
 //data CANNOT be byteswapped
 //AnalyzeVolumeHeader must read UNKNOWN for the datatype

 AnalyzeVolumeHeaderV1N1<float> hdr(ofname,ANALYZE,VOLUME,hageman::UNKNOWNTYPE,8*48,false,this->getRDIM(), this->getCDIM(), this->getZDIM(), this->getRLENGTH(), this->getCLENGTH(), this->getZLENGTH());
 hdr.writeOutHeaderFile();

 FileNameV1N1<> *ofimgname = ofname->clone();
 if((ofimgname->getExtension()) != ".img")
  ofimgname->setExtension(".img");
 (evol->obfile).setFileName(ofimgname); 
 (evol->obfile).openForWrite();
 const int BYTENUM = 4;  //BS requires 4 byte floats
 float val = float();
 float *ptr;
 char *charptr = NULL;
 
#if VERBOSE
 std::cout<<"Reading in 3D eigenvolume from Analyze file (BrainSuite compatible) = "<<ofimgname->getOSCompatibleFileName()<<std::endl;
#endif

 for(int zidx = 0; zidx < evol->getZDIM(); zidx++)
 {
#if VERBOSE
  std::cout<<"Writing Z = "<<zidx<<std::endl;
#endif
  for(int cidx = 0; cidx < (evol->getCDIM()); cidx++)
   for(int ridx = 0; ridx < (evol->getRDIM()); ridx++)
   {
    //v1x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[0]);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v1y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[1]);    
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v1z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v1[2]);
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v2x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[0]); 
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v2y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[1]);  
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v2z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v2[2]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    
    //v3x
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[0]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v3y
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[1]);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //v3z
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).v3[2]);    
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);

    //l1
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l1);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
        
    //l2
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l2);  
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    
    //l3
    val = static_cast<float>((evol->getElement(ridx,cidx,zidx)).l3);   
    ptr = &val;
    charptr = reinterpret_cast<char *>(ptr);
    if(hdr.getShouldByteSwap())  //checks whether the analyze header indicates that the file should be byte swapped
     byteswap<char>(charptr,BYTENUM);
    (evol->obfile).writeBytesToFileStream(charptr,BYTENUM);
    

   }
 }

 (evol->obfile).close();
 delete ofimgname;
}
*/
 
 

template<class T>
ImageVolumeHeaderV1N2<T>* constructImageVolumeHeader(const FileNameV1N1<> *imgfptr, T dumval)  //imgfptr must be the image file, not the hdr file
//determines ImageVolumeHeader based on file extension
{

 ImageVolumeHeaderV1N2<T> *imghdr = NULL;

 if(imgfptr->getExtension() == std::string(".img"))
 {
  FileNameV1N1<> *hdrfptr = new FileNameV1N1<>(imgfptr);
  hdrfptr->setExtension(".hdr");
  imghdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfptr);
  delete hdrfptr;
  hdrfptr = NULL;
 }
 else if(imgfptr->getExtension() == std::string(".nii"))
 {
  imghdr = new NiftiVolumeHeaderV1N2<T>(imgfptr);

 }

 return imghdr;



}

template<class T>
hageman::DataType determineDataType(const FileNameV1N1<T> *ptr)
{

 hageman::DataType dtype = hageman::EMPTYTYPE;

 hageman::AnalyzeVolumeHeaderV1N2<T> hdr(ptr);
 dtype = hdr.getDataType();

#if VALIDATE
 std::cout<<"Datatype from Analyze Hdr File: "<<dtype<<std::endl;
#endif

 return dtype;

}

template<class T>
hageman::DataType determineDataType(const std::vector<FileNameV1N1<T>* > &ptrvec)
{

 hageman::DataType dtype = hageman::EMPTYTYPE;

 for(int idx = 0; idx < ptrvec.size(); idx++)
 {
  hageman::DataType dtypetemp = dtype;
  hageman::AnalyzeVolumeHeaderV1N2<T> hdr(ptrvec.at(idx));
  dtypetemp = hdr.getDataType();

  if((dtypetemp != dtype) && (idx != 0))
  {

#if ERRORCHECK
   std::cout<<"ERROR in hageman::DataType determineDataType(const std::vector<FileNameV1N1<T>* > &ptrvec)"<<std::endl;
   std::cout<<"Datatypes within FileName vector are not the same.  Exiting..."<<std::endl;
   exit(1);
#endif

#if VERBOSE
   std::cout<<"WARNING: Datatypes within FileName vector are not the same."<<std::endl;
#endif
  }

  dtype = dtypetemp;
 }


#if VALIDATE
 std::cout<<"Datatype from Analyze Hdr File: "<<dtype<<std::endl;
#endif

 return dtype;

}


template<class T>
hageman::DataType determineDataType(const std::vector<T> &ptrstrvec)
{

 hageman::DataType dtype = hageman::EMPTYTYPE;

 std::vector<FileNameV1N1<T> *> ptrvec;
 hageman::convertStringVectorToFileNamePtrList(ptrstrvec,ptrvec);

 dtype = hageman::determineDataType(ptrvec);

 for(int idx = 0; idx < ptrvec.size(); idx++)
 {
  delete ptrvec.at(idx);
  ptrvec.at(idx) = NULL;
 }
  

 return dtype;

}


template<class T>
class Object2DV1N1{

public:
 Object2DV1N1();  //default empty constructor



};

template<class T>
Object2DV1N1<T>::Object2DV1N1()
//default empty constructor
{


}


template<class T>
class Object3DV1N1{

public:
 Object3DV1N1();  //default empty constructor



};

template<class T>
Object3DV1N1<T>::Object3DV1N1()
//default empty constructor
{


}



template<class T>  
class ROIV1N4{

public:
 //CONSTRUCTORS
 ROIV1N4();  //empty default constructor
 
 
protected:

};

//CONSTRUCTORS
template<class T>
ROIV1N4<T>::ROIV1N4()
//empty default constructor
{
}


template<class T>
class ROI3DV1N4 : public ROIV1N4<T>{

public:
 //CONSTRUCTORS
 ROI3DV1N4();  //empty default constructor
 

 //GET FUNCTIONS
 virtual Vector3V2N4<T> getRandomPt() = 0;  //virtual function which returns a point randomly selected within the ROI
 virtual Vector3V2N4<double> getRandomRealWorldPt(double RL, double CL, double ZL) = 0;  //virtual function which returns a point randomly selected within the ROI in real world coordinates
 virtual int getSize() = 0;  //pure virtual function that computes the size of the ROI

 //SET FUNCTIONS

 //CHECK FUNCTIONS
 virtual bool isInROI(const Vector3V2N4<T> &pt) const = 0;  //pure virtual fn for determing whether a 3D pt is contained in the ROI

 //READ FUNCTIONS

 //WRITE FUNCTIONS

 //PRINT FUNCTIONS
 
 
 
protected:

};

//CONSTRUCTORS
template<class T>
ROI3DV1N4<T>::ROI3DV1N4()
//empty default constructor
:ROIV1N4<T>()
{
}



template<class T> //the datatype of the roivoxels
class VoxelROI3DV1N4 : public ROI3DV1N4<T>{

public:
 //CONSTRUCTORS
 VoxelROI3DV1N4();  //empty default constructor
 
 //INIT FUNCTIONS
 template<class S> //the datatype of the mask volume
 void initROIFromMaskVolume(const ImageVolumeHeaderV1N2<S> *hdr);  //initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)

 template<class S> //the datatype of the mask volume
 void initROIFromMaskVolume(const ImageVolumeV2N4<S> &imvol);  //initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)
 
 //template<class S> //the datatype of the mask volume
 //void initROIFromAnalyzeMaskVolume(const AnalyzeVolumeHeaderV1N2<S> &hdr);  //initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)

 //GET FUNCTIONS
 virtual Vector3V2N4<T> getRandomPt();  //virtual function which returns a point randomly selected within the ROI
 virtual Vector3V2N4<double> getRandomRealWorldPt(double RL, double CL, double ZL);  //virtual function which returns a point randomly selected within the ROI in real world coordinates
 //(RL,CL,ZL) are the voxel lengths in 3D (in mm)
 virtual int getSize() {return (roivoxels.getSize());}  //gets the size of the roi as number of voxels

 //SET FUNCTIONS

 //CHECK FUNCTIONS
 virtual bool isInROI(const Vector3V2N4<T> &pt) const;  //determines whether pt is in the voxel ROI

 //READ FUNCTIONS

 //WRITE FUNCTIONS

 //PRINT FUNCTIONS
 void printCL();  //prints the roi voxels out on the CL

protected:
 LinkedListV1N1<Vector3V2N4<T> > roivoxels;

};

//CONSTRUCTORS

template<class T>
VoxelROI3DV1N4<T>::VoxelROI3DV1N4()
//empty default constructor
:ROI3DV1N4<T>()
{


}

//INIT FUNCTIONS
template<class T>
template<class S>  //the datatype of the mask volume
void VoxelROI3DV1N4<T>::initROIFromMaskVolume(const ImageVolumeHeaderV1N2<S> *hdr)  
//initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)
{
 //GENERALIZE THIS FOR OTHER IMAGE FILE FORMATS
 ImageVolumeV2N4<S> maskvol;
 maskvol.readInImageVolumeFromFile(hdr);

 Vector3V2N4<T> pt;
 std::vector<int> coords;
 
 for(int idx = 0; idx < maskvol.getSIZE(); idx++)
 {
  if(maskvol.getElement(idx) > S())  //the mask voxel is non-zero - denotes an roi voxel
  {
   coords = maskvol.convertToCoords(idx);  //gets the coords for the current mask voxel
   pt.initVector3(coords.at(0), coords.at(1), coords.at(2));
   roivoxels.addAtBack(pt);  //adds the coordinates of the roi voxel to the list
  }
 }


}


template<class T>
template<class S>  //the datatype of the mask volume
void VoxelROI3DV1N4<T>::initROIFromMaskVolume(const ImageVolumeV2N4<S> &maskvol)  
//initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)
{
 //GENERALIZE THIS FOR OTHER IMAGE FILE FORMATS
 //ImageVolumeV2N4<S> maskvol;
 //maskvol.readInImageVolumeFromFile(hdr);

 Vector3V2N4<T> pt;
 std::vector<int> coords;
 
 std::cout<<"Initializing ROI..."<<std::endl;
 for(int idx = 0; idx < maskvol.getSIZE(); idx++)
 {
  if((idx % 10000) == 0)
   std::cout<<"idx = "<<idx<<" of "<<maskvol.getSIZE()<<std::endl;

  if(maskvol.getElement(idx) > S())  //the mask voxel is non-zero - denotes an roi voxel
  {
   coords = maskvol.convertToCoords(idx);  //gets the coords for the current mask voxel
   pt.initVector3(coords.at(0), coords.at(1), coords.at(2));
   roivoxels.addAtBack(pt);  //adds the coordinates of the roi voxel to the list
  }
 }


}


//template<class T>
//template<class S> //the datatype of the mask volume
//void VoxelROI3DV1N4<T>::initROIFromAnalyzeMaskVolume(const AnalyzeVolumeHeaderV1N2<S> &hdr)  
//initializes the roi voxels using an Analyze mask volume (0 = nonmask; nonzero = mask)
//{
// ImageVolumeV2N4<S> maskvol;
// Vector3V2N4<T> pt;
// std::vector<int> coords;
 
// for(int idx = 0; idx < maskvol.getSIZE(); idx++)
// {
//  if(maskvol.getElement(idx) > S())  //the mask voxel is non-zero - denotes an roi voxel
//  {
//   coords = maskvol.convertToCoords(idx);  //gets the coords for the current mask voxel
//   pt.initVector3(coords.at(0), coords.at(1), coords.at(2));
//   roivoxels.addAtBack(pt);  //adds the coordinates of the roi voxel to the list
//  }
// }
//}

//GET FUNCTIONS
 
template<class T>
Vector3V2N4<T> VoxelROI3DV1N4<T>::getRandomPt()  
//virtual function which returns a point randomly selected within the ROI
{
 Vector3V2N4<T> result;

 static MathV1N2<> rgenerator;  //the random number generator only needs to be initialized once.
 int idx = rgenerator.getRandomNumberMerseeneTwister((roivoxels.getSize())-1);  //returns a static index of roivoxels between [0,roivoxels.size()]

 result = roivoxels.getNodeData(idx);  //gets the coordinates for the roi voxel at idx 
 return result;

}

template<class T>
Vector3V2N4<double> VoxelROI3DV1N4<T>::getRandomRealWorldPt(double RL, double CL, double ZL)  
//virtual function which returns a point randomly selected within the ROI in real world coordinates.  (RL,CL,ZL) are the voxel lengths in 3D (in mm)
{
 //function assumes that the roivoxels are in voxel coordinates
 Vector3V2N4<double> realworldpt;
 Vector3V2N4<T> voxelpt;

 static MathV1N2<> rgenerator;  //the random number generator only needs to be initialized once.
 int idx = rgenerator.getRandomNumberMerseeneTwister((roivoxels.getSize())-1);  //returns a static index of roivoxels between [0,roivoxels.size()]
 voxelpt = roivoxels.getNodeData(idx); //gets the coordinates for the roi voxel at idx 
 
 //voxelpt only defines the voxel region.  we still need to randomly select a region within the voxel
 double dx,dy,dz;
 dx = (static_cast<double>(rgenerator.getRandomNumberMerseeneTwister(1000)))/1000.0f;
 dy = (static_cast<double>(rgenerator.getRandomNumberMerseeneTwister(1000)))/1000.0f;
 dz = (static_cast<double>(rgenerator.getRandomNumberMerseeneTwister(1000)))/1000.0f;

 //add the random numbers along each axis to get the final real world coords
 double x,y,z;
 x = (RL*voxelpt.x())+(dx*RL);
 y = (CL*voxelpt.y())+(dy*CL);
 z = (ZL*voxelpt.z())+(dz*ZL);

 realworldpt.initVector3(x,y,z);

 return realworldpt;


}



//SET FUNCTIONS

//CHECK FUNCTIONS
template<class T>
bool VoxelROI3DV1N4<T>::isInROI(const Vector3V2N4<T> &pt) const
//determines whether pt is in the voxel ROI
{

 for(int idx = 0; idx < roivoxels.getSize(); idx++)
 {
  if((roivoxels.getNodeData(idx)) == pt)
   return true;
 }

 return false;

}

//READ FUNCTIONS

//WRITE FUNCTIONS

//PRINT FUNCTIONS
template<class T>
void VoxelROI3DV1N4<T>::printCL()
//prints the roi voxels on the CL
{
 roivoxels.printCL();  //LinkedList printCL function
}



template<class T>
class CubeROIV1N4 : public ROI3DV1N4<T>{

public:
 //CONSTRUCTORS
 CubeROIV1N4();  //empty default constructor
 
 //GET FUNCTIONS
 

 //SET FUNCTIONS

 //CHECK FUNCTIONS
 virtual bool isInROI();  //determines whether pt is in the ROI

 //READ FUNCTIONS

 //WRITE FUNCTIONS

 //PRINT FUNCTIONS
 void printCL();  //prints the dimensions out on the CL

 
protected:

};

//CONSTRUCTORS
template<class T>
CubeROIV1N4<T>::CubeROIV1N4()
//empty default constructor
:ROI3DV1N4<T>()
{
}


//GET FUNCTIONS
 

//SET FUNCTIONS

//CHECK FUNCTIONS
template<class T>
bool CubeROIV1N4<T>::isInROI()
//determines whether pt is in the ROI
{

 //TO BE IMPLEMENTED

 return false;

}

//READ FUNCTIONS

//WRITE FUNCTIONS

//PRINT FUNCTIONS

template<class T>
void CubeROIV1N4<T>::printCL()
//prints the dimensions out on the CL
{
 //TO BE IMPLEMENTED
}




template<class T>
class PathV1N3{

public:
 PathV1N3();  //empty constructor
 

protected:


};

template<class T>
PathV1N3<T>::PathV1N3()
//empty constructor
{



}



template<class T>
class Path2DV1N3 : public PathV1N3<T>, public Object2DV1N1<T>{

public:
 //CONSTRUCTORS
 Path2DV1N3();  //empty constructor
 Path2DV1N3(const Path2DV1N3<T> &copy);  //copy constructor
 
 //CHECK FUNCTIONS
 bool isEmpty() const {return (pts.empty());}  //returns whether the path is empty
 

 //PRINT FUNCTIONS
 void printCL() const; //write out the points on the command line

 //WRITE OUT FUNCTIONS
 void writeOutToTxtFile(FileNameV1N1<> *fptr) const;  //writes out a tract to a txt file, 1 pt per line
 

 //READ IN FUNCTIONS
 void readInTxtFile(FileNameV1N1<> *fptr, std::string delim = " ");  //reads in a 2D tract from a txt file

 
 //SET FUNCTIONS
 void addPointAtBack(const Vector2V2N4<T> &pt) {(this->pts).push_back(pt);}  //adds a Vector2 to the end of the data deque 
 void addPointAtFront(const Vector2V2N4<T> &pt) {(this->pts).push_front(pt);}  //adds a Vector2 to the end of the data deque 
 void clearTract() {pts.clear();} //clears the deque
 
 //GET FUNCTIONS
 const std::deque<Vector2V2N4<T> >& getConstPtsReference() const {return pts;}  //returns a const reference to pts
 std::deque<Vector2V2N4<T> >& getPtsReference() {return pts;}  //gets a non-const reference to pts
 int getNumPoints() const {return static_cast<int>(pts.size());}  //return the number of points in the tract
 Vector2V2N4<T> getLeastValueCoords();  //returns the smallest values for x and y in the path
 Vector2V2N4<T> getGreatestValueCoords();  //returns the greatest values x and y pts in the path
 
 const Vector2V2N4<T>& getPoint(int idx) const {return pts.at(idx);}  //returns the Vector2 located at index idx in the deque
 Vector2V2N4<T>& getNonConstPoint(int idx) {return pts.at(idx);}  //returns the Vector2 located at index idx in the deque
 

 //INTERPOLATION FUNCTIONS
 
 //OVERLOADED OPERATORS
 

protected:
 std::deque<Vector2V2N4<T> > pts;  //the 2D anchor points for the tract

};

template<class T>
Path2DV1N3<T>::Path2DV1N3()
//empty constructor
:PathV1N3<T>(), Object2DV1N1<T>()
{



}

template<class T>
Path2DV1N3<T>::Path2DV1N3(const Path2DV1N3<T> &copy)
:PathV1N3<T>(), Object2DV1N1<T>()
{

}

//PRINT FUNCTIONS
template<class T>
void Path2DV1N3<T>::printCL() const
//write out the points on the command line
{
 printOutSTLSequenceContainer<std::deque<Vector2V2N4<T> > >(pts); 

}

//WRITE FUNCTIONS
template<class T>
void Path2DV1N3<T>::writeOutToTxtFile(FileNameV1N1<> *fptr) const  
//writes out a tract to a txt file, 1 pt per line
{
 WriteToFileV1N1<std::string> ifile(fptr);  //write stream associated with fptr
 ifile.openForWrite();

 if(!this->isEmpty())  //path is not empty
 {
  for(int idx = 0; idx < this->getNumPoints(); idx++)
  {
   ifile.writeObjectToFile<Vector2V2N4<T> >(this->getPoint(idx));
  }
 }

 ifile.close();

}


//READ IN FUNCTIONS
template<class T>
void Path2DV1N3<T>::readInTxtFile(FileNameV1N1<> *fptr, std::string delim)  
//reads in a 2D tract from a txt file; assumes reading in double
{

 ReadFromFileV1N1<std::string> rfs(fptr);
 rfs.openForRead();

 std::string cline, sstr;
 while(!rfs.reachedEOF())
 {
  cline = rfs.readProcessedCurrentLineFromStream();

  if(!cline.empty())
  {
   hageman::removePrecedingNonDigitChars(cline);  //removes all preceding non-digit chars

   double *results = new double[2];
   hageman::parseLineOfNumbers(results,2,cline,delim);  //parses the first two entries in cline as the coords
   Vector2V2N4<double> pt(results[0],results[1]);
   this->addPointAtBack(pt);

   delete[] results;
  }

 }

 rfs.close();




}


//GET FUNCTIONS
template<class T>
Vector2V2N4<T> Path2DV1N3<T>::getLeastValueCoords()  
//returns the smallest values for x and y in the path
{
 Vector2V2N4<T> results(pts.at(0));

 for(int idx = 0; idx < pts.size(); idx++)
 {
  if((pts.at(idx)).x() < results.x())
   results.setElement(0, (pts.at(idx)).x());
   
  if((pts.at(idx)).y() < results.y())
   results.setElement(1, (pts.at(idx)).y());
 }


 //std::deque<hageman::Vector2V2N4<T> >::iterator pos;

 //for(pos = pts.begin(); pos != pts.end(); pos++)
 //{
 // if((*pos).x() < results.x())
 //  results.setElement(0,(*pos).x());

 // if((*pos).y() < results.y())
 //  results.setElement(1,(*pos).y());

 //}



}
 
 


template<class T>
class Path3DV1N3 : public PathV1N3<T>, public Object3DV1N1<T>{

public:
 //CONSTRUCTORS
 Path3DV1N3();  //empty constructor
 Path3DV1N3(const Path3DV1N3<T> &copy);  //copy constructor
 
 //PRINT FUNCTIONS
 void printCL() const; //write out the points on the command line

 //WRITE FUNCTIONS
 void writeOutToTxtFile(FileNameV1N1<> *fptr) const;  //writes out a tract to a txt file, 1 pt per line
 void writeOutToTxtFileForBSuite(FileNameV1N1<> *fptr) const;  //writes out tract in a form to be read in to Brainsuite.
 void writeOutAsUCF(FileNameV1N1<> *fptr, const int RDIM, const int CDIM, const int ZDIM) const;  //writes out the 3D tract as a single UCF 

 //READ FUNCTIONS
 void readInTxtFileInBSuiteFormat(FileNameV1N1<> *fptr);  //function which reads in a txt file that was output in BrainSuite format
 void readInUCF(FileNameV1N1<> *fptr);  //reads in a 3D tract from a UCF file
 void readInTxtFile(FileNameV1N1<> *fptr);  //reads in a 3D tract from a txt file

 //SET FUNCTIONS
 void addPointAtBack(const Vector3V2N4<T> &pt) {(this->pts).addAtBack(pt);}  //adds a Vector3 to the end of the data deque 
 void addPointAtFront(const Vector3V2N4<T> &pt) {(this->pts).addAtFront(pt);}  //adds a Vector3 to the end of the data deque 
 void addPointAtBack(T ptx, T pty, T ptz) {(this->pts).addAtBack(Vector3V2N4<T>(ptx,pty,ptz));}  //adds a Vector3 to the end of the data deque 
 void insertPoint(const Vector3V2N4<T> &pt, int idx) {(this->pts).insertPoint(pt,idx);}  //inserts a Vector3 at the index position idx
 void moveTract(T dx, T dy, T dz);  //moves each anchor pt by (dx,dy,dz)
 
 //GET FUNCTIONS
 const LinkedListV1N1<Vector3V2N4<T> >& getConstPtsReference() const {return pts;}  //returns a const reference to the pts of the LinkedList
 LinkedListV1N1<Vector3V2N4<T> >& getPtsReference() {return pts;}  //gets non-const reference to the pts of the LinkedList
 int getNumPoints() const {return pts.getSize();}  //return the number of points in the tract
 const Vector3V2N4<T>& getPoint(int idx) const {return pts.getNodeData(idx);}  //returns the Vector3 located at index idx in the deque
 Vector3V2N4<T>& getNonConstPoint(int idx) const {return pts.getNonConstNodeData(idx);}  //returns the Vector3 located at index idx in the deque
 
 //CALCULATION FUNCTIONS
 Vector3V2N4<T> getDirectionOfNextPoint(int idx) const;  //gets a unit vector that points in the direction of the next anchor point
 double getTurnAngle(int idx) const;  //gets the angle between vectors (idx+1 - idx) and (idx - idx-1) in radians
 double getTurnAngleAtBack(const Vector3V2N4<T> &pt) const; //gets the angle between the terminal point (end - end-1) and (pt - end) in radians
 double getTurnAngleAtFront(const Vector3V2N4<T> &pt) const; //gets the angle between the terminal point (beg - beg+1) and (pt - beg) in radians 
 double getTurnAngle(int idx1, int idx2, int idx3) const;  //gets the angle between vectors (idx3 - idx2) and (idx2 - idx1) in radians
 double getLength() const;  //returns the actual length of the tract (in mm), assuming the tract is in real world coordinates
 double getDistanceToNextPoint(int currentidx) const; //returns the distance from the current point to the next point in the deque
 double getDistanceToLastPoint(int currentidx) const; //returns the distance from the current point to the last point in the deque
 
 template<class S> //the datatype of the imagevolume
 S calculateMetricAlongTract(const ImageVolumeV2N4<S> &scalarvol);  //computes the average of the metric along the tract as it goes through the scalar volume
 
 //CHECK FUNCTIONS
 bool isEmpty() const {return (pts.isEmpty());}  //returns whether the path is empty
 inline bool isPtInTract(const Vector3V2N4<T> &pt) const; //returns whether pt is within the tract

 template<class S>
 bool doesTractIntersectROI3D(const ROI3DV1N4<int>* const roiptr, const ImageVolumeV2N4<S> &imgvol) const;  //returns whether or not the tract intersects the ROI3D object 
 //(uses a virtual fn (isInROI()) within the ROI3D hierarchy)

 //INTERPOLATION FUNCTIONS
 
 //OVERLOADED OPERATORS
 

protected:
 LinkedListV1N1<Vector3V2N4<T> > pts;  //a linked list containing the anchor points of the tract

};

//CONSTRUCTORS

template<class T>
Path3DV1N3<T>::Path3DV1N3()
//empty constructor
:PathV1N3<T>(), Object3DV1N1<T>()
{



}

template<class T>
Path3DV1N3<T>::Path3DV1N3(const Path3DV1N3<T> &copy)
//copy constructor
:PathV1N3<T>(), Object3DV1N1<T>()
{
 pts.copyList(copy.getConstPtsReference());
}

//PRINT FUNCTIONS
template<class T>
void Path3DV1N3<T>::printCL() const
//write out the points on the command line
{
 (this->pts).printCL();

}


//WRITE FUNCTIONS
template<class T>
void Path3DV1N3<T>::writeOutToTxtFile(FileNameV1N1<> *fptr) const  
//writes out a tract to a txt file, 1 pt per line
{
 WriteToFileV1N1<std::string> ifile(fptr);  //write stream associated with fptr
 ifile.openForWrite();

 if(!this->isEmpty())  //path is not empty
 {
  for(int idx = 0; idx < this->getNumPoints(); idx++)
  {
   ifile.writeObjectToFile<Vector3V2N4<T> >(this->getPoint(idx));
  }
 }

 ifile.close();

}
 
template<class T>
void Path3DV1N3<T>::writeOutToTxtFileForBSuite(FileNameV1N1<> *fptr) const
//writes out the tract to a text file that can be read in by BrainSuite for visualization.  Takes a string as the output filename
//note: this is an empty function.  I only instantiate a function of type double because that is what Brainsuite reads in
{
#if ERRORCHECK
 std::cout<<"ERROR in void Path3DV1N3<T>::writeOuttoTxtforBSuite(std::string ofname) const"<<std::endl;
 std::cout<<"This form of the equation is never meant to be called"<<std::endl;
 exit(1);
#endif
 
}

template<>
void Path3DV1N3<double>::writeOutToTxtFileForBSuite(FileNameV1N1<> *fptr) const;
//writes out the tract to a text file that can be read in by BrainSuite for visualization.  Takes a string as the output filename


template<class T>
void Path3DV1N3<T>::writeOutAsUCF(FileNameV1N1<> *fptr, const int RDIM, const int CDIM, const int ZDIM) const
//writes out the 3D tract as a UCF.  T is required to be a fundamental datatype. ofname is the output ASCII filename.  
//(RDIM,CDIM,ZDIM) are the matrix dimensions in which the tract exists.  NOTE: RDIM AND CDIM MUST BE SWTICHED IN ORDER TO CHANGE TO REAL WORLD 
//ORDERING
{

 std::string ofname(fptr->getOSCompatibleFileName());
 std::ofstream ofile(ofname.c_str(), std::ios::out);
 

#if VERBOSE
 std::cout<<"Writing out tract to UCF file = "<<ofname<<std::endl;
#endif

 
 ofile <<"<width=>\n";
 ofile << RDIM << " " << CDIM << "\n";
 
 ofile <<"<height=>\n";
 ofile << ZDIM << "\n";
 
 ofile << "<xrange=>\n";
 ofile << "0 " << RDIM << "\n";


 ofile << "<yrange=>\n";
 ofile << "0 " << CDIM << "\n";

 ofile << "<zrange=>\n";
 ofile << "0 " << ZDIM << "\n";

 ofile << "<levels>\n";
 ofile << "1\n";

 ofile << "<level number=>\n";
 ofile << "0.0\n";

 ofile << "<point_num=>\n";
 ofile << (this->getNumPoints()) << "\n";

 ofile << "<contour_data=>\n";

 for(int idx = 0; idx < this->getNumPoints(); idx++)
 {
  ofile << (pts.getNodeData(idx)).x() << " " << (pts.getNodeData(idx)).y() <<  " " << (pts.getNodeData(idx)).z() << "\n";
 }

 ofile << "<end of level>\n";
 ofile << "<end>\n";

 ofile.close();

}


//READ FUNCTIONS
template<class T>
void Path3DV1N3<T>::readInTxtFileInBSuiteFormat(FileNameV1N1<> *fptr)
//reads in the tract from a text file that can be read in by BrainSuite for visualization.  Takes a string as the input filename
//note: this is an empty function.  I only instantiate a function of type double because that is what Brainsuite reads in
{

#if ERRORCHECK
 std::cout<<"ERROR in void Path3DV1N3<T>::readInTxtFileInBSuiteFormat(std::string ofname) const"<<std::endl;
 std::cout<<"This form of the equation is never meant to be called"<<std::endl;
 exit(1);
#endif

}

template<>
void Path3DV1N3<double>::readInTxtFileInBSuiteFormat(FileNameV1N1<> *fptr);

template<class T>
void Path3DV1N3<T>::readInUCF(FileNameV1N1<> *fptr)
//reads the 3D tract as a UCF.  T is required to be a fundamental datatype. fptr is the input filename.  
{

 hageman::ReadFromFileV1N1<std::string> ifile(fptr);  //constructs a read stream that is associated with fptr
 ifile.openForRead();

 T currentnumarray[3];
 std::string currentline(ifile.readProcessedCurrentLineFromStream());

 //skip over lines until contour data starts
 while(currentline != "<contour_data=>")  
 {
  currentline = ifile.readProcessedCurrentLineFromStream();  

  if(ifile.reachedEOF())  //if file has reached EOF, then stop reading
   break;
 }

 //read in contour data until the end of level
 while(true)  
 {
  if(ifile.reachedEOF())  //if file has reached EOF, then stop reading
   break;
  
  currentline = ifile.readProcessedCurrentLineFromStream();
 
  if(currentline == "<end of level>")  //once the end of contour data has been reached, then break out of loop
   break;
   
  hageman::parseLineOfNumbers(currentnumarray,3,currentline," ");  //parse the current line into 3 vector coords
  this->addPointAtBack(currentnumarray[0],currentnumarray[1],currentnumarray[2]);  //add the anchor pt to the tract



 }
 
#if ERRORCHECK
 if(this->isEmpty())
 {
  std::cout<<"ERROR in void Path3DV1N3<T>::readInUCF(FileNameV1N1<> *fptr)"<<std::endl;
  std::cout<<"There are no points in this file!  Exiting..."<<std::endl;
  exit(1);
 }
#endif

  
 
 ifile.close();



}

template<class T>
void Path3DV1N3<T>::readInTxtFile(FileNameV1N1<> *fptr)
//reads in a 3D tract from a txt file
{
 //TO BE IMPLEMENTED

}



//CALCULATING FUNCTIONS

template<class T>
Vector3V2N4<T> Path3DV1N3<T>::getDirectionOfNextPoint(int idx) const
//gets a unit vector that points in the direction of the next anchor point
{
 Vector3V2N4<T> dir;

 if(idx == this->getNumPoints()-1)  //this is the last anchor pt, so determine the direction from the pt behind this one
 {
  dir = (this->getPoint(idx)) - (this->getPoint(idx-1));
 }
 else  
 {
  dir = (this->getPoint(idx+1)) - (this->getPoint(idx));
 }

 dir.normalize();  //normalize to get the unit vector

 return dir;

}

template<class T>
double Path3DV1N3<T>::getTurnAngle(int idx) const  
//gets the angle between vectors (idx+1 - idx) and (idx - idx-1) in radians
{
 return 0.0;

}

template<class T>
double Path3DV1N3<T>::getTurnAngleAtBack(const Vector3V2N4<T> &pt) const 
//gets the angle between the terminal point (end - end-1) and (pt - end) in radians
{

 double angle = 0.0;
 Vector3V2N4<T> endpt(this->getPoint(this->getNumPoints()-1));

 Vector3V2N4<T> dirvec1(endpt - this->getPoint(this->getNumPoints()-2));
 dirvec1.normalize();
 
 Vector3V2N4<T> dirvec2(pt - endpt);
 dirvec2.normalize();
 
 angle = dirvec1.dot(dirvec2);
 angle = acos(angle);
 
 return angle;


}

template<class T>
double Path3DV1N3<T>::getTurnAngleAtFront(const Vector3V2N4<T> &pt) const 
//gets the angle between the first point (beg - beg+1) and (pt - beg) in radians
{
 double angle = 0.0;
 Vector3V2N4<T> begpt(this->getPoint(0));

 Vector3V2N4<T> dirvec1(begpt - this->getPoint(1));
 dirvec1.normalize();
 
 Vector3V2N4<T> dirvec2(pt - begpt);
 dirvec2.normalize();
 
 angle = dirvec1.dot(dirvec2);
 angle = acos(angle);
 
 return angle;


}



template<class T>
double Path3DV1N3<T>::getTurnAngle(int idx1, int idx2, int idx3) const 
//gets the angle between the terminal point (idx3 - idx2) and (idx2 - idx1) in radians
{
 
 return 0.0;


}

template<class T>
double Path3DV1N3<T>::getLength() const 
//returns the actual length of the tract (in mm) assuming the tract is in real world coordinates
{
 double tractlength = 0.0;

 if(!pts.isEmpty())
 {
  for(int idx = 0; idx < (this->getNumPoints())-1; idx++)
  {
   hageman::Vector3V2N4<T> diff((this->getPoint(idx+1)) - (this->getPoint(idx)));
   tractlength += diff.L2norm();
  }
 }

 return tractlength;



}

template<class T>
double Path3DV1N3<T>::getDistanceToNextPoint(int currentidx) const
//returns the distance from the current point (currentidx) to the next point in the deque
{
 double distance = 0.0;

 Vector3V2N4<T> current(this->getPoint(currentidx));
 Vector3V2N4<T> next(this->getPoint(currentidx+1));
 Vector3V2N4<T> diff(next-current);
 distance = diff.L2norm();

 return distance;

}

template<class T>
double Path3DV1N3<T>::getDistanceToLastPoint(int currentidx) const
//returns the distance from the current point, currentidx, to the last point in the deque
{
 double distance = 0.0;

 Vector3V2N4<T> current(this->getPoint(currentidx));
 Vector3V2N4<T> last(this->getPoint(currentidx-1));
 Vector3V2N4<T> diff(last-current);
 distance = diff.L2norm();

 return distance;

}

template<class T>
void Path3DV1N3<T>::moveTract(T dx, T dy, T dz)
//moves each anchor pt by (dx,dy,dz)
{
 T x,y,z;

 for(int idx = 0; idx < this->getNumPoints(); idx++)
 {
  x = (this->getPoint(idx)).x();
  y = (this->getPoint(idx)).y();
  z = (this->getPoint(idx)).z();

  x += dx;  //moves the current anchor pt by (dx,dy,dz)
  y += dy;
  z += dz;

  (this->getNonConstPoint(idx)).initVector3(x,y,z);
 }


}
 
//CHECK FUNCTIONS

template<class T>
inline bool Path3DV1N3<T>::isPtInTract(const Vector3V2N4<T> &pt) const
//returns whether or not the pt is in pts
{

 for(int idx = 0; idx < pts.getSize(); idx++)
 {
  if(pts.getNodeData(idx) == pt)
   return true;
 }

 return false;

}

template<class T>
template<class S>
bool Path3DV1N3<T>::doesTractIntersectROI3D(const ROI3DV1N4<int>* const roiptr, const ImageVolumeV2N4<S> &imgvol) const
//returns whether or not the tract intersects the ROI3D object (uses a virtual fn withing ROI3D hierarchy)
{

 Vector3V2N4<int> vcoords;  //voxel coords for the current roi pt
 
 for(int idx = 0; idx < pts.getSize(); idx++)
 {
  vcoords = hageman::getVoxelCoordinatesForRealWorldPt(imgvol,this->getPoint(idx));
  if(roiptr->isInROI(vcoords))
   return true;
 }

 return false;

}



//CALCULATING FUNCTIONS
template<class T>
template<class S> //the datatype of the imagevolume
S Path3DV1N3<T>::calculateMetricAlongTract(const ImageVolumeV2N4<S> &scalarvol)
//computes the average of the metric along the tract as it goes through the scalar volume
{
 S result = S();

 Vector3V2N4<T> currentpt;
 Vector3V2N4<int> voxpt;

 for(int idx = 0; idx < (this->getNumPoints()); idx++)
 {
  currentpt = this->getPoint(idx);
  voxpt = hageman::getVoxelCoordinatesForRealWorldPt(scalarvol,currentpt);
  result += scalarvol.getElement(voxpt.x(), voxpt.y(), voxpt.z());
 }

 if(this->getNumPoints() != 0)
  result /= static_cast<double>(this->getNumPoints());

 return result;

}

//THIS INSTANTIATED TEMPLATE FUNCTIONS ARE DEFINED HERE INSTEAD OF A .CPP FILE, B/C OF A CONFLICT WITH NUMERICAL RECIPES CODE THAT CAUSES LINKER ERRORS.  THE NR CODE NEEDS TO BE
//OVERHAULED BEFORE THIS CAN BE RESOLVED

//WRITE FUNCTIONS
template<>
void Path3DV1N3<double>::writeOutToTxtFileForBSuite(FileNameV1N1<std::string> *fptr) const
//writes out the tract to a text file that can be read in by BrainSuite for visualization. 
{
 std::string ofname(fptr->getOSCompatibleFileName());
 std::ofstream ofile(ofname.c_str(), std::ios::out);
 char buffer[_CVTBUFSIZE];

 //each line is of a form that can be read by Brainsuite
 for(int idx=0; idx < (this->getNumPoints()); idx++)
 {
  std::string data;

  double xconv = .5f*(((this->getPoint(idx)).x())-.5f);  //this conversion is unique to BS (a bug in the program)
  sprintf(buffer,"%5.5f",xconv);
  data += std::string(buffer);
  if(data.at(data.length()-1) == '.')
   data += "00";
  data += "e+000  ";

  double yconv = .5f*(((this->getPoint(idx)).y())-.5f);  //this conversion is unique to BS (a bug in the program)
  sprintf(buffer,"%5.5f",yconv); 
  data += std::string(buffer);
  if(data.at(data.length()-1) == '.')
   data += "00";
  data += "e+000  ";
  
  
  double zconv = .5f*(((this->getPoint(idx)).z())-.5f);  //this conversion is unique to BS (a bug in the program)
  sprintf(buffer,"%5.5f",zconv);
  data += std::string(buffer);
  if(data.at(data.length()-1) == '.')
   data += "00";
  data += "e+000  ";
  ofile << data << "\n";
 }

 ofile.close();

}

//READ FUNCTIONS
template<>
void Path3DV1N3<double>::readInTxtFileInBSuiteFormat(FileNameV1N1<> *fptr)
//function which reads in a txt file that was output in BrainSuite format.
{
 std::string ifname(fptr->getOSCompatibleFileName());
 std::ifstream ifile(ifname.c_str(), std::ios::in);
 
 char *str = new char[200];
 double x,y,z;
 int xpow,ypow,zpow;

 while(ifile.getline(str,200))
 {
  std::string currentline(str);
  
  std::string zpowstr(currentline.substr(currentline.rfind("e")));
  zpowstr.erase(zpowstr.find("e"),2);
  currentline.erase(currentline.rfind("e"));
  std::string zstr(currentline.substr(currentline.rfind(" ")));
  currentline.erase(currentline.rfind(" "));
 
  z = atof(zstr.c_str());
  zpow = atoi(zpowstr.c_str());
        z *= powf(10.0f,((double)zpow));

  std::string ypowstr(currentline.substr(currentline.rfind("e")));
  ypowstr.erase(ypowstr.find("e"),2);
  currentline.erase(currentline.rfind("e"));
  std::string ystr(currentline.substr(currentline.rfind(" ")));
  currentline.erase(currentline.rfind(" "));
  
  y = atof(ystr.c_str());
  ypow = atoi(ypowstr.c_str());
  y *= powf(10.0f,((double)ypow));

  std::string xpowstr(currentline.substr(currentline.rfind("e")));
  xpowstr.erase(xpowstr.find("e"),2);
  currentline.erase(currentline.rfind("e"));
  std::string xstr(currentline);
  
  x = atof(xstr.c_str());
  xpow = atoi(xpowstr.c_str());
  x *= powf(10.0f,((double)xpow));

  //this conversion is unique to BS (a bug in the program)
  double xconv = (2.0f*x) + 0.5f;
  double yconv = (2.0f*y) + 0.5f;
  double zconv = (2.0f*z) + 0.5f;

  this->addPointAtBack(xconv,yconv,zconv);
  
  currentline.clear();
  delete[] str;
  str = new char[200];
 
 }

 ifile.close();

}



template<class T>
class DTITract3DV1N3 : public Path3DV1N3<T>{

public:
 //CONSTRUCTORS
 DTITract3DV1N3();  //empty default constructor
 DTITract3DV1N3(const DTITract3DV1N3<T> &copy);  //copy constructor

};

//CONSTRUCTORS

template<class T>
DTITract3DV1N3<T>::DTITract3DV1N3()
//empty default constructor
:Path3DV1N3<T>()
{


}

template<class T>
DTITract3DV1N3<T>::DTITract3DV1N3(const DTITract3DV1N3<T> &copy)  
//copy constructor
:Path3DV1N3<T>(copy)
{


}




template<class T>
const int compileUCFs(FileNameV1N1<> *pathptr, DTITract3DV1N3<T>*& tractarray)
//reads in a collection of UCF files in a directory (pathptr) and compiles the DTITract3D's into a single array; returns the number of tracts read in
{
 std::string ext(".ucf");
 std::vector<std::string> tractfnames(hageman::setFNameStrListUsingDirectory(pathptr,ext));  //constructs all filenames the have .ucf extension
 const int TRACTSIZE = tractfnames.size();
 
 tractarray = new DTITract3DV1N3<T>[TRACTSIZE];  
 FileNameV1N1<> *fptr = NULL;

 //reads in the UCF files from list, tractfnames
 for(int idx = 0; idx < TRACTSIZE; idx++)
 {
  fptr = hageman::constructFileNameObjFromString(tractfnames.at(idx));
  hageman::changeMissingPathToCurrentDirectory(fptr);

  tractarray[idx].readInUCF(fptr);  //reads in the current UCF filename

  delete fptr;
  fptr = NULL;
 }

 return TRACTSIZE;  //returns the number of tracts read in

}


template<class T>
void writeOutCompiledUCFs(DTITract3DV1N3<T>*& tractarray, const int TRACTSIZE, int RDIM, int CDIM, int ZDIM, FileNameV1N1<> *ofptr)
//writes out an array of DTITract3D objects of size TRACTSIZE into a single UCF file that traverses a volume of (RDIM,CDIM,ZDIM) dimensions
{

 std::string ofname(ofptr->getOSCompatibleFileName());

#if VERBOSE
 std::cout<<"Writing Out UCF file = "<<ofname<<std::endl;
#endif

 std::ofstream ofile(ofname.c_str(), std::ios::out);
 
 ofile <<"<width=>\n";
 ofile << RDIM << " " << CDIM << "\n";
 
 ofile <<"<height=>\n";
 ofile << ZDIM << "\n";
 
 ofile << "<xrange=>\n";
 ofile << "0 " << RDIM << "\n";

 ofile << "<yrange=>\n";
 ofile << "0 " << CDIM << "\n";

 ofile << "<zrange=>\n";
 ofile << "0 " << ZDIM << "\n";

 ofile << "<levels>\n";
 ofile << "1\n";
 ofile << "<level number=>\n";
 ofile << "0.0\n";

 for(int tidx = 0; tidx < TRACTSIZE; tidx++)
 {

#if VERBOSE
  std::cout<<"Writing out tract number = "<<tidx<<std::endl;
#endif

  ofile << "<point_num=>\n";
  ofile << tractarray[tidx].getNumPoints() << "\n";

  ofile << "<contour_data=>\n";

  for(int idx = 0; idx < (tractarray[tidx].getNumPoints()); idx++)
  {
   ofile << ((tractarray[tidx].getConstPtsReference()).getNodeData(idx)).x() << " " << ((tractarray[tidx].getConstPtsReference()).getNodeData(idx)).y() <<  " " << ((tractarray[tidx].getConstPtsReference()).getNodeData(idx)).z() << "\n";
  }

  
 }
 ofile << "<end of level>\n";
 ofile << "<end>\n";

 ofile.close();

}



template<class T>
void printTensorElementITK(const typename itk::Image<itk::DiffusionTensor3D<T>,3>::Pointer &image, int i, int j, int k)
{

 //pixel index
 typename itk::Image<itk::DiffusionTensor3D<T>,3>::IndexType pixIndex;
 pixIndex[0] = i;
 pixIndex[1] = j;
 pixIndex[2] = k;

 //gets pixel data
 std::cout<<"Tensor Element at ("<<i<<","<<j<<","<<k<<"): "<<std::endl;
 typename itk::Image<itk::DiffusionTensor3D<T>,3>::PixelType pixval = image->GetPixel(pixIndex);
 std::cout<<"dxx: "<<pixval[0]<<std::endl;
 std::cout<<"dxy: "<<pixval[1]<<std::endl;
 std::cout<<"dxz: "<<pixval[2]<<std::endl;
 std::cout<<"dyy: "<<pixval[3]<<std::endl;
 std::cout<<"dyz: "<<pixval[4]<<std::endl;
 std::cout<<"dzz: "<<pixval[5]<<std::endl;

}


template<class T>
void printEigenSystemITK(const typename itk::Image<itk::DiffusionTensor3D<T>,3>::Pointer &image, int i, int j, int k)
{
 //pixel index
 typename itk::Image<itk::DiffusionTensor3D<T>,3>::IndexType pixIndex;
 pixIndex[0] = i;
 pixIndex[1] = j;
 pixIndex[2] = k;

 std::cout<<"Eigensystem at ("<<i<<","<<j<<","<<k<<"): "<<std::endl;

 typename itk::Image<itk::DiffusionTensor3D<T>,3>::PixelType pixval = image->GetPixel(pixIndex);
 
 //generate the eigensystem from the tensor data
 typename itk::SymmetricSecondRankTensor<T,3>::EigenValuesArrayType eigvals;
 typename itk::SymmetricSecondRankTensor<T,3>::EigenVectorsMatrixType eigvecs;
 pixval.ComputeEigenAnalysis(eigvals,eigvecs);

 std::cout<<"Eigenvalues (l0,l1,l2): "<<std::endl;
 std::cout<<eigvals[2]<<","<<eigvals[1]<<","<<eigvals[0]<<std::endl;

 T *vec = new T[3];
 std::cout<<"Eigenvectors: "<<std::endl;
 vec = eigvecs[2];
 std::cout<<"v0: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
 
 vec = eigvecs[1];
 std::cout<<"v1: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
 
 vec = eigvecs[0];
 std::cout<<"v2: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;

 delete[] vec;

}


template<class T>
void printDIMSITK(const typename itk::Image<T,3>::Pointer &image)
//prints the dimensions of the image
{
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::SizeType &size = wholeregion.GetSize();
 
 const typename itk::Image<T,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 const typename itk::Image<T,3>::SizeType &size = wholeregion.GetSize();
 std::cout<<"Size: "<<size[0]<<","<<size[1]<<","<<size[2]<<std::endl;
 
}


template<class T>
int getRDIMITK(const typename itk::Image<T,3>::Pointer &image)
//gets the RDIM of the image
{
 const typename itk::Image<T,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 const typename itk::Image<T,3>::SizeType &size = wholeregion.GetSize();
 
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::SizeType &size = wholeregion.GetSize();
 //std::cout<<"Size: "<<size[0]<<","<<size[1]<<","<<size[2]<<std::endl;

 return (static_cast<int>(size[0]));
}

template<class T>
int getCDIMITK(const typename itk::Image<T,3>::Pointer &image)
//gets the RDIM of the image
{
 const typename itk::Image<T,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 const typename itk::Image<T,3>::SizeType &size = wholeregion.GetSize();
 
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::SizeType &size = wholeregion.GetSize();
 //std::cout<<"Size: "<<size[0]<<","<<size[1]<<","<<size[2]<<std::endl;

 return (static_cast<int>(size[1]));
}

template<class T>
int getZDIMITK(const typename itk::Image<T,3>::Pointer &image)
//gets the RDIM of the image
{
 const typename itk::Image<T,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 const typename itk::Image<T,3>::SizeType &size = wholeregion.GetSize();
 

 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::RegionType &wholeregion = image->GetLargestPossibleRegion();
 //const typename itk::Image<itk::DiffusionTensor3D<T>,3>::SizeType &size = wholeregion.GetSize();
 //std::cout<<"Size: "<<size[0]<<","<<size[1]<<","<<size[2]<<std::endl;

 return (static_cast<int>(size[2]));
}

template<class T>
void printVoxelLengthsITK(const typename itk::Image<T,3>::Pointer &image)
{
 //gets the voxel lengths
 const typename itk::Image<T,3>::SpacingType &spacing = image->GetSpacing();
 std::cout<<"Spacing: "<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<std::endl;
 
}

template<class T>
double getRLENGTHITK(const typename itk::Image<T,3>::Pointer &image)
{
 //gets the voxel lengths
 const typename itk::Image<T,3>::SpacingType &spacing = image->GetSpacing();
 //std::cout<<"Spacing: "<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<std::endl;
 return (static_cast<double>(spacing[0]));
}

template<class T>
double getCLENGTHITK(const typename itk::Image<T,3>::Pointer &image)
{
 //gets the voxel lengths
 const typename itk::Image<T,3>::SpacingType &spacing = image->GetSpacing();
 //std::cout<<"Spacing: "<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<std::endl;
 return (static_cast<double>(spacing[1]));
}

template<class T>
double getZLENGTHITK(const typename itk::Image<T,3>::Pointer &image)
{
 //gets the voxel lengths
 const typename itk::Image<T,3>::SpacingType &spacing = image->GetSpacing();
 //std::cout<<"Spacing: "<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<std::endl;
 return (static_cast<double>(spacing[2]));
}


template<class T>
void constructVolumeITK(const typename itk::Image<T,3>::Pointer &image, hageman::ImageVolumeV2N4<T> &imgvol)
//function assumes that memory for imgvol has already been allocated and that it has the correct dimensions AND voxel lengths
{

 //pixel index
 typename itk::Image<T,3>::IndexType pixIndex;

 //typename itk::Image<itk::DiffusionTensor3D<T>,3>::PixelType pixval = image->GetPixel(pixIndex);
 
 typename itk::Image<T,3>::PixelType pixval;
 //typename itk::SymmetricSecondRankTensor<T,3>::EigenValuesArrayType eigvals;
 //typename itk::SymmetricSecondRankTensor<T,3>::EigenVectorsMatrixType eigvecs;
 
 for(int k = 0; k < (hageman::getZDIMITK<T>(image)); k++)
  for(int j = 0; j < (hageman::getCDIMITK<T>(image)); j++)
   for(int i = 0; i < (hageman::getRDIMITK<T>(image)); i++)
   {
    pixIndex[0] = i;
    pixIndex[1] = j;
    pixIndex[2] = k;
    pixval = image->GetPixel(pixIndex);

    imgvol.setElement(i,j,k,pixval);

    //generate the eigensystem from the tensor data
    //pixval.ComputeEigenAnalysis(eigvals,eigvecs);

    //std::cout<<"Eigenvalues (l0,l1,l2): "<<std::endl;
    //std::cout<<eigvals[2]<<","<<eigvals[1]<<","<<eigvals[0]<<std::endl;
    //double *vec = new double[3];
    //std::cout<<"Eigenvectors: "<<std::endl;
    //vec = eigvecs[2];
    //std::cout<<"v0: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
    
    //vec = eigvecs[1];
    //std::cout<<"v1: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
    
    //vec = eigvecs[0];
    //std::cout<<"v2: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;

    //EigenSystem3DV1N2<T> esys(eigvals[2],eigvals[1],eigvals[0],eigvecs[2],eigvecs[1],eigvecs[0]);
    //eigvol.setElement(i,j,k,esys);



   }


}




template<class T>
void constructEigenVolumeITK(const typename itk::Image<itk::DiffusionTensor3D<T>,3>::Pointer &image, EigenVolume3DV1N4<T> &eigvol)
//function assumes that memory for eigvol has already been allocated and that it has the correct dimensions AND voxel lengths
{

 //pixel index
 typename itk::Image<itk::DiffusionTensor3D<T>,3>::IndexType pixIndex;

 //typename itk::Image<itk::DiffusionTensor3D<T>,3>::PixelType pixval = image->GetPixel(pixIndex);
 
 typename itk::Image<itk::DiffusionTensor3D<T>,3>::PixelType pixval;
 typename itk::SymmetricSecondRankTensor<T,3>::EigenValuesArrayType eigvals;
 typename itk::SymmetricSecondRankTensor<T,3>::EigenVectorsMatrixType eigvecs;
 
 for(int k = 0; k < (hageman::getZDIMITK<itk::DiffusionTensor3D<T> >(image)); k++)
  for(int j = 0; j < (hageman::getCDIMITK<itk::DiffusionTensor3D<T> >(image)); j++)
   for(int i = 0; i < (hageman::getRDIMITK<itk::DiffusionTensor3D<T> >(image)); i++)
   {
    pixIndex[0] = i;
    pixIndex[1] = j;
    pixIndex[2] = k;
    pixval = image->GetPixel(pixIndex);

    //generate the eigensystem from the tensor data
    pixval.ComputeEigenAnalysis(eigvals,eigvecs);

    //std::cout<<"Eigenvalues (l0,l1,l2): "<<std::endl;
    //std::cout<<eigvals[2]<<","<<eigvals[1]<<","<<eigvals[0]<<std::endl;
    //double *vec = new double[3];
    //std::cout<<"Eigenvectors: "<<std::endl;
    //vec = eigvecs[2];
    //std::cout<<"v0: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
    
    //vec = eigvecs[1];
    //std::cout<<"v1: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;
    
    //vec = eigvecs[0];
    //std::cout<<"v2: "<<vec[0]<<","<<vec[1]<<","<<vec[2]<<std::endl;

    EigenSystem3DV1N4<T> esys(eigvals[2],eigvals[1],eigvals[0],eigvecs[2],eigvecs[1],eigvecs[0]);
    eigvol.setElement(i,j,k,esys);



   }

}



template<class T>
void writeTractsToVTPFile(const std::vector<DTITract3DV1N3<T> > &tracts, std::string fname, int RDIM, int CDIM, int ZDIM, double RLENGTH, double CLENGTH, double ZLENGTH)
{
 //Create 10 points.
 const int NUMTRACTS = tracts.size();
 int TOTALPTS = 0;

 std::cout<<"Creating Points and Lines..."<<std::endl;
 
 vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
 std::vector<int> tractLengths;
 
 T pt[3];
 
 //std::vector<vtkSmartPointer<vtkLine> *> larray;
 //std::vector<int> lsize;
  
 
 for(int tidx = 0; tidx < NUMTRACTS; tidx++)
 {
  
  DTITract3DV1N3<T> tract(tracts.at(tidx));
  //tract.printCL();

  const int NUMPTS = tract.getNumPoints();
  tractLengths.push_back(NUMPTS);

  for(int i = 0; i < NUMPTS; i++)
  {
   //in Slicer, there is an orientation flip wrt the AP axis.  also volume has an image origin, which we assume to be centered.  this gets fixed here b/c the BS volume is fine.
   pt[0] = (tract.getPoint(i)).x()-((double(RDIM)*RLENGTH)/2.0);
   pt[1] = ((double(CDIM)*CLENGTH)-(tract.getPoint(i)).y())-((double(CDIM)*CLENGTH)/2.0);  //needs to be flipped b/c of the orientation problem
   pt[2] = (tract.getPoint(i)).z()-((double(ZDIM)*ZLENGTH)/2.0);
   
   //pt[0] = (tract.getPoint(i)).x();
   //pt[1] = (tract.getPoint(i)).y();
   //pt[2] = (tract.getPoint(i)).z();
   
   points->InsertNextPoint(pt);
   TOTALPTS++;
  }
 }

 vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

 int cidx = 0;
 for(int tidx = 0; tidx < NUMTRACTS; tidx++)
 {
  vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
  line->GetPointIds()->SetNumberOfIds(tractLengths.at(tidx));

  for(int i = 0; i < tractLengths.at(tidx); i++)
  {
   line->GetPointIds()->SetId(i,cidx); //the second 0 is the index of the pt in the vtkPoints
   cidx++;
  }
  
  lines->InsertNextCell(line);
 }



 
 // vtkSmartPointer<vtkLine> *lsubarray = new vtkSmartPointer<vtkLine>[NUMPTS-1];
    
 // for(int i = 0; i < NUMPTS-1; i++)
 // {
 //  lsubarray[i] = vtkSmartPointer<vtkLine>::New();
 //  lsubarray[i]->GetPointIds()->SetId(0,i); //the second 0 is the index of the pt in the vtkPoints
 //  lsubarray[i]->GetPointIds()->SetId(1,i+1); //the second 0 is the index of the pt in the vtkPoints

 // }



 // larray.push_back(lsubarray);
 // lsize.push_back(NUMPTS-1);
 //}
 

 
 //for(int i = 0; i < NUMTRACTS; i++)
 // for(int j = 0; j < lsize.at(i)-1; j++)
 // {
 //  lines->InsertNextCell((larray.at(i))[j]);
 // }



  

  //std::cout<<"Creating Line 1..."<<std::endl;
  
  //vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
   
  //for(int i = 0; i < 40; i++)
  //{
  // line1->GetPointIds()->SetId(i,i); //the second 0 is the index of the pt in the vtkPoints
   
  //}

 //std::cout<<"Creating Cell Array..."<<std::endl;
  
 //create a cell array to store the lines in and add the lines to it
 //for(int i = 0; i < NUMTRACTS; i++)
 // for(int j = 0; j < ; i++)
 //{
 // lines->InsertNextCell(larray[i]);
 //}
  
 std::cout<<"Creating PolyData..."<<std::endl;
  
 //Create a polydata object and add the points to it.
 vtkSmartPointer<vtkPolyData> pdata = vtkSmartPointer<vtkPolyData>::New();
 pdata->SetPoints(points);
 pdata->SetLines(lines);
   
 std::cout<<"Writing PolyData to File..."<<std::endl;
  
 //Write the file
 vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
 writer->SetInput(pdata);
 writer->SetFileName(fname.c_str());
   
 //Optional - set the mode. The default is binary.
 //writer->SetDataModeToBinary();
 //writer->SetDataModeToAscii();
   
 writer->Write();
}
   

template<class T>
void writeTractToVTPFile(const DTITract3DV1N3<T> &tract, std::string fname)
{
 //Create 10 points.
 //const int NUMTRACTS = tracts.size();
 //int TOTALPTS = 0;

 std::cout<<"Creating Points and Lines..."<<std::endl;
 vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
 T pt[3];
 
 const int NUMPTS = tract.getNumPoints();

 for(int i = 0; i < NUMPTS; i++)
 {
   //pt[0] = (tract.getPoint(i)).x()-127.5;
   //pt[1] = (256-(tract.getPoint(i)).y())-127.5;  //needs to be flipped b/c of the orientation problem
   //pt[2] = (tract.getPoint(i)).z()-67.6;
   
   pt[0] = (tract.getPoint(i)).x();
   pt[1] = (tract.getPoint(i)).y();
   pt[2] = (tract.getPoint(i)).z();
   
   points->InsertNextPoint(pt);
   //TOTALPTS++;
 }

 
 //vtkSmartPointer<vtkLine> *lsubarry = new vtkSmartPointer<vtkLine>[NUMPTS-1];
 
 //for(int i = 0; i < NUMPTS-1; i++)
 //{
 // lsubarray[i] = vtkSmartPointer<vtkLine>::New();
 // lsubarray[i]->GetPointIds()->->SetId(0,i); //the second 0 is the index of the pt in the vtkPoints
 // lsubarray[i]->GetPointIds()->SetId(1,i+1); //the second 0 is the index of the pt in the vtkPoints
 //}

 vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
 line->GetPointIds()->SetNumberOfIds(NUMPTS);

 for(int i = 0; i < NUMPTS; i++)
 {
  line->GetPointIds()->SetId(i,i); //the second 0 is the index of the pt in the vtkPoints
 }

  



 
 vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
 
 //for(int i = 0; i < NUMPTS-1; i++)
 // lines->InsertNextCell(lsubarray[i]);

 lines->InsertNextCell(line);



  

  //std::cout<<"Creating Line 1..."<<std::endl;
  
  //vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
   
  //for(int i = 0; i < 40; i++)
  //{
  // line1->GetPointIds()->SetId(i,i); //the second 0 is the index of the pt in the vtkPoints
   
  //}

 //std::cout<<"Creating Cell Array..."<<std::endl;
  
 //create a cell array to store the lines in and add the lines to it
 //for(int i = 0; i < NUMTRACTS; i++)
 // for(int j = 0; j < ; i++)
 //{
 // lines->InsertNextCell(larray[i]);
 //}
  
 std::cout<<"Creating PolyData..."<<std::endl;
  
 //Create a polydata object and add the points to it.
 vtkSmartPointer<vtkPolyData> pdata = vtkSmartPointer<vtkPolyData>::New();
 pdata->SetPoints(points);
 pdata->SetLines(lines);
   
 std::cout<<"Writing PolyData to File..."<<std::endl;
  
 //Write the file
 vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
 writer->SetInput(pdata);
 writer->SetFileName(fname.c_str());
   
 //Optional - set the mode. The default is binary.
 //writer->SetDataModeToBinary();
 //writer->SetDataModeToAscii();
   
 writer->Write();
}


class FMArgParserV1N4 : public CLArgParserV1N3{

public:
 //CONSTRUCTORS
 FMArgParserV1N4(); //empty default constructor
 FMArgParserV1N4(int argc, char *argv[], QueryState shouldprocess = NOQUERY, std::string htxt = "");  //constructor which initializes the class using arguments from the command line

 //GET FUNCTIONS
 double getMINFATHRESHOLD() const;  //returns the minimum FA threshold as a double; if no flag, then returns 0.0
 double getMINTRACTLENGTH() const;  //returns the minimum tract length as a double; if no flag, then returns 0.0
 double getMAXTRACTLENGTH() const;  //returns the max tract length as a double; if no flag, then returns 0.0
 double getSTEPSIZE() const;  //returns the stepsize interpolating eigenvolume as a double; if no flag, then return 0.0
 double getMAXTURNANGLE() const;  //returns the max turn angle in degrees (as a double); if no flag, then return 0.0
 std::vector<std::string> getSeedMaskVolFNameVec() const;  //returns the vector of filenames of seed ROI masks; if no flag, then returns an empty vector 
 std::vector<std::string> getStopMaskVolFNameVec() const;  //returns the vector of filenames of stop ROI masks; if no flag, then returns an empty vector 
 int getTotalTrys() const;  //returns the total number of trys as an integer; if no flag, then returns the default of 1 
 int getMaxTractNumber() const;  //returns the max tract number as an integer; if no flag, then return 0
  
 //CHECK FUNCTIONS
 bool isThereMINFATHRESHOLD() const {return (this->doesFlagExist(DSL_MINFATHRESHOLD_FLAG));}
 bool isThereMINTRACTLENGTH() const {return (this->doesFlagExist(DSL_MINTRACTLENGTH_FLAG));}
 bool isThereMAXTRACTLENGTH() const {return (this->doesFlagExist(DSL_MAXTRACTLENGTH_FLAG));}
 bool isThereSTEPSIZE() const {return (this->doesFlagExist(DSL_STEPSIZE_FLAG));}
 bool isThereMAXTURNANGLE() const {return (this->doesFlagExist(DSL_MAXTURNANGLE_FLAG));}
 bool isThereMaskVolFName() const {return (this->doesFlagExist(DSL_MASKVOL_FLAG));}
 bool isThereSeedMaskVolFName() const {return (this->doesFlagExist(DSL_SEEDMASKVOL_FLAG));}
 bool isThereStopMaskVolFName() const {return (this->doesFlagExist(DSL_STOPMASKVOL_FLAG));}
 bool isThereTotalTrys() const {return (this->doesFlagExist(DSL_TOTALTRYS_FLAG));}
 bool isThereMaxTractNumber() const {return (this->doesFlagExist(DSL_MAXTRACTNUMBER_FLAG));}
 bool isThereOFPath() const {return (this->doesFlagExist(DSL_OFPATH_FLAG));}

protected:
 

};



enum SeedMethod{WHOLEVOLUME, SEEDROI, MASKED};
enum TractROIIntersect{ALLROIS, ANYONEROI, ANYTWOROIS};
enum InterpolationMethod{BSPLINE, TRILINEAR, NOINTERPOLATION};
enum IntegrationMethod{RK4, RK2, NOINTEGRATION};
enum TractWriteFormat{UCF, BSUITE, TXT};
enum TractographyMethod{NOMETHOD, TEND};


//yet to be implemented
enum SeedDensity{RANDOM, REGULARGRID, IRREGULARGRID};
enum TractTrim{TWOROITRIM, THREEROITRIM, ALLROITRIM};
//enum TractInterpolate{BSPLINE, LINEAR, POLYNOMIAL};
enum TractEnd{FRONT,BACK};

template<class T>
class FluidMechanicsTrackV1N4{

public:
 //CONSTRUCTOR
 FluidMechanicsTrackV1N4();  //default empty constructor

protected:
 


};

//CONSTRUCTOR

template<class T>
FluidMechanicsTrackV1N4<T>::FluidMechanicsTrackV1N4()
//empty default constructor
{

}


template<class T>  //T is the datatype for the seed/stop volumes
class FluidMechanicsTrack3DV1N4 : public FluidMechanicsTrackV1N4<T>{

public:
 //CONSTRUCTORS
 FluidMechanicsTrack3DV1N4();  //default empty constructor
 FluidMechanicsTrack3DV1N4(const FMArgParserV1N4 &dtiap); //initializes the program based on a DTIStreamlineArgParser object.
 FluidMechanicsTrack3DV1N4(EigenVolume3DV1N4<double> *evol, double MINFA, double MAXTL, double MINTL, double MAXTA);
 ~FluidMechanicsTrack3DV1N4();  //destructor

 //INIT FUNCTIONS
 void initEigenVol(const FMArgParserV1N4 &dtiap);  //initializes the eigenvolume from the filename provided by the user
 void initFAVol();  //initializes the fa volume from the eigenvol provided by the user
 void initTractographyConstants(const FMArgParserV1N4 &dtiap); //initializes the tractography constants
 
 //GET FUNCTIONS
 const std::vector<DTITract3DV1N3<double> >& getIncludedTracts() {return includedtracts;}

 //CHECK FUNCTIONS
 bool isBelowFAThreshold(const Vector3V2N4<double> &pt) const;  //returns whether the FA at the current pt is below MINFATHRESHOLD
 bool isGreaterThanMaxTurnAngle(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, TractEnd tend) const;  //returns whether the angle between the
 //terminal pt in tract and pt is greater than MAXTURNANGLE
 bool isLongerThanMaxTractLength(const DTITract3DV1N3<double> &tract) const; //return whether the length of the tract is longer than MAXTRACTLENGTH
 bool isLongerThanMinTractLength(const DTITract3DV1N3<double> &tract) const; //return whether the length of the tract is longer than MINTRACTLENGTH
 virtual bool shouldTerminate(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, TractEnd tend) const = 0; //determines if an end of the tract should be terminated based 
 //on the current position, pt 
 virtual bool shouldIncludeTract(const DTITract3DV1N3<double> &tract) const = 0; //determines if tract should be in includedtracts or excludedtracts
 

 //TRACT FUNCTIONS
 virtual void performStreamlineTractography(const FMArgParserV1N4 &dtiap) = 0;  //tries totaltrys times to sample a seed pt and construct a tract; writes out the tracts as they are found
 virtual void constructTract(DTITract3DV1N3<double> &result, const Vector3V2N4<double> &seedpt) = 0;  //constructs a tract starting at seedpt and evolving both ends
 virtual void constructTractEnd(DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &seedpt, TractEnd tend) = 0;  
 //constructs a tract starting at seedpt, using no interpolation
 
 //PRINT FUNCTIONS
 void printTractographyConstants();

 //READ FUNCTIONS

 //WRITE FUNCTIONS
 void writeOutIncludedTracts(std::string ofprefix) const;
 void writeOutExcludedTracts(std::string ofprefix) const;
 void writeOutIncludedTractsAsUCF(std::string ofprefix) const;
 void writeOutExcludedTractsAsUCF(std::string ofprefix) const;
 void writeOutIncludedTractsForBSuite(std::string ofprefix) const;
 void writeOutExcludedTractsForBSuite(std::string ofprefix) const;
 void writeOutMostRecentIncludedTract(std::string ofprefix) const;
 void writeOutMostRecentIncludedTractAsUCF(std::string ofprefix) const;
 void writeOutMostRecentIncludedTractAsBSUITE(std::string ofprefix) const;

 void writeOutIncludedTractsAsVTP(std::string ofprefix) const;
 

protected:
 //INPUT
 EigenVolume3DV1N4<double> *eigenvol; //the eigenvolume that will be used to generate the tracts
 ImageVolumeV2N4<double> *favol; //the favolume that will be used generate tracts 
 
 //CONSTANTS
 double MINFATHRESHOLD;  //the minimum value for the local FA below which the tract is terminated (value of 0.0f means that there is no FA threshold) (default = 0.04)
 double MAXTRACTLENGTH;  //the maximum length for the tracts (value of 0.0f means that there is no max tract length) (default = 0.0f)
 double MINTRACTLENGTH; //the minimum length for the tracts (value of 0.0f means that there is no min tract length) (default = 0,0f)
 double STEPSIZE; //the stepsize taken before the eigenvector field is sampled again (default = 10% of smallest voxel dimension)
 double MAXTURNANGLE; //the maximum angle that the tract is allowed to turn (in degrees) (if value is 0.0f, then there is no max turn angle, but abs value of direction is still taken to make sure the tract doesnt double back on itself) (default = 0.0f)

 //TRACTS
 std::vector<DTITract3DV1N3<double> > includedtracts;  
 std::vector<DTITract3DV1N3<double> > excludedtracts;


 
 
 //void writeOutIncludedTracts(std::string ofprefix, TractWriteFormat twf) const;
 //void writeOutExcludedTracts(std::string ofprefix, TractWriteFormat twf) const;

 
 //void writeOutMostRecentIncludedTractAsTXT(std::string ofprefix) const;


 
 //TDF data
 //Volume3DV2N2<std::vector<EigenSystem3DV1N2<double> > > *tdfevol;
 //Volume3DV2N2<std::vector<double> > *weights;
 //inline bool hasEnteredMaskVolume(const Vector3V2N2<double> &pt) const; //returns whether pt has entered the area defined by maskvol
 //void performStreamlineTractography(int totaltrys);  //master fn which calls the proper streamline tractography based on the default flags or based on the input data, where possible
 //void performStreamlineTractography(int totaltrys, hageman::TractographyMethod tm, hageman::SeedMethod sm, hageman::TractROIIntersect tri, hageman::InterpolationMethod interpm, hageman::IntegrationMethod intm);  
 //master fn which calls the proper streamline tractography based on the enumerated flags
 //void performSeedROIIntersectAllROITractographyUsingRK4BSpline(int totaltrys);  //this function constructs tracts by seeding the regions defined by seedrois
 //void performMaskedIntersectAllROITractographyUsingRK4BSpline(int totaltrys);  //this function constructs tracts by seeding the whole region defined by maskvol
 //void performWholeVolumeIntersectAllROITractographyUsingRK4BSpline(int totaltrys); //this function constructs tracts by seeding the 
 //entire image volume.  It will pass through each of the voxels, idx = numtrys, taking a point randomly from each voxel
 //void performWholeVolumeIntersectAllROITractographyUsingNoIntegrationNoInterpolation(int totaltrys); //this function constructs tracts by seeding the 
 //entire image volume.  It will pass through each of the voxels, idx = numtrys, taking a point randomly from each voxel
 //void performSeedROIIntersectAllROITractographyUsingNoIntegrationNoInterpolation(int totaltrys); //this function constructs tracts by seeding a specific 
 //number of seed rois.  It will pass through each of the seed voxels, idx = numtrys, taking a point randomly from each voxel
 //void performWholeVolumeIntersectAllROITENDTractographyUsingNoIntegrationNoInterpolation(int totaltrys); //this function constructs tracts by seeding the 
 //entire image volume.  It will pass through each of the voxels, idx = numtrys, taking a point randomly from each voxel
 //void performSeedROIIntersectAllROITENDTractographyUsingNoIntegrationNoInterpolation(int totaltrys); //this function constructs tracts by seeding a specific 
 //number of seed rois.  It will pass through each of the seed voxels, idx = numtrys, taking a point randomly from each voxel
 //void constructTENDTractUsingNoIntegrationNoInterpolation(DTITract3DV1N1<double> &result, const Vector3V2N2<double> &seedpt);  //constructs a tract starting at seedpt
 //void constructTENDTractEndUsingNoIntegrationNoInterpolation(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);  
 //constructs a tract starting at seedpt, using no interpolation with an initial direction of Ezero
 
 
 //DTITract3DV1N1<double> constructTractUsingRK4BSpline(const Vector3V2N2<double> &seedpt);  //constructs a tract starting at seedpt
 //void constructTractEndUsingRK4BSpline(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);  
 //constructs a tract starting at seedpt, using an RK4 approach, r(n+1) = r(n) + h(Vn+1), with an initial direction of Ezero
 
 //DTITract3DV1N1<double> constructTractUsingRK2(const Vector3V2N2<double> &seedpt);  //constructs a tract starting at seedpt
 //void constructTractEndUsingRK2(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);  
 
 //DTITract3DV1N1<double> constructTractUsingNoIntegration(const Vector3V2N2<double> &seedpt);
 //void constructTractEndUsingNoIntegration(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);

 //DTITract3DV1N1<double> constructTractUsingNRRK4(const Vector3V2N2<double> &seedpt);  //constructs a tract starting at seedpt
 //void constructTractEndUsingNRRK4(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);  
 
 //Vector3V2N2<double> getTENDVOut(Vector3V2N2<double> vin, const Vector3V2N2<int> &voxpt, const Vector3V2N2<int> &lastvoxpt);

 //TDF TRACT FUNCTIONS
 //void initTDFData(FMArgParserV1N4 *dtiargs);
 //void initTDFFAVolume();
 //EigenSystem3DV1N2<double> getEigenSystem(const Vector3V2N2<int> &voxpt);
 //Vector3V2N2<double> getV1FromEigenSystem(const Vector3V2N2<int> &voxpt);
 //void readInTDFEigenVolumeFromTxtFile(FileNameV1N0<> *fptr);  //tdf read function
 //void parseTDFTextLine(std::string cline, std::vector<Vector3V2N2<int> > &coords, std::vector<int> &num, std::vector<std::vector<double> > &tdfdata);  //parse tdf text line

 //void performTDFStreamlineTractography(int totaltrys); 
 //void performSeedROIIntersectAllROITDFTractographyUsingNoIntegrationNoInterpolation(int totaltrys); //this function constructs tracts by seeding a specific 
 //number of seed rois.  It will pass through each of the seed voxels, idx = numtrys, taking a point randomly from each voxel
 //void constructTDFTractUsingNoIntegrationNoInterpolation(DTITract3DV1N1<double> &result, const Vector3V2N2<double> &seedpt);  //constructs a tract starting at seedpt
 //void constructTDFTractEndUsingNoIntegrationNoInterpolation(DTITract3DV1N1<double> &tract, const Vector3V2N2<double> &seedpt, const TractEnd &tend);  
 //constructs a tract starting at seedpt, using no interpolation with an initial direction of Ezero

};

//CONSTRUCTORS
template<class T>
FluidMechanicsTrack3DV1N4<T>::FluidMechanicsTrack3DV1N4()
//empty default constructor
:FluidMechanicsTrackV1N4<T>()
{


}

template<class T>
FluidMechanicsTrack3DV1N4<T>::FluidMechanicsTrack3DV1N4(const FMArgParserV1N4 &dtiap)
//initializes the program based on a DTIStreamlineArgParser object.
:FluidMechanicsTrackV1N4<T>(), MINFATHRESHOLD(0.4), MINTRACTLENGTH(0.0), MAXTRACTLENGTH(0.0), MAXTURNANGLE(0.0), STEPSIZE(0.0)  //sets the default values for the data
{
 eigenvol = NULL;
 favol = NULL;
 
 //initialize the eigenvolume.  NOTE: this must be initalized before the tractography constants to assign a proper default value to STEPSIZE
#if VERBOSE
 std::cout<<"Reading In Eigenvolume..."<<std::endl;
#endif
 this->initEigenVol(dtiap);

 //initializes the favolume. It calculates it from the eigenvolume just read in
#if VERBOSE
 std::cout<<"Calculating FA Volume..."<<std::endl;
#endif
 this->initFAVol();

 //initialize the tractography constants
#if VERBOSE
 std::cout<<"Initializing Tractography Constants..."<<std::endl;
#endif
 this->initTractographyConstants(dtiap);

#if VALIDATE
 this->printTractographyConstants();
#endif

 
}

template<class T>
FluidMechanicsTrack3DV1N4<T>::FluidMechanicsTrack3DV1N4(EigenVolume3DV1N4<double> *evol, double MINFA, double MAXTL, double MINTL, double MAXTA)
:FluidMechanicsTrackV1N4<T>(), MINFATHRESHOLD(MINFA), MINTRACTLENGTH(MINTL), MAXTRACTLENGTH(MAXTL), MAXTURNANGLE(MAXTA), STEPSIZE(0.0)  //sets the default values for the data
{

 //initialize the eigenvolume.  NOTE: this must be initalized before the tractography constants to assign a proper default value to STEPSIZE
 eigenvol = evol;
 favol = NULL;

 //assigns a default value to STEPSIZE based on the voxel lengths (default = 1% of smallest voxel length)
 STEPSIZE = 0.01f*(eigenvol->getSmallestVoxelLength());
 
 //initializes the favolume. It calculates it from the eigenvolume just read in
 #if VERBOSE
  std::cout<<"Calculating FA Volume..."<<std::endl;
 #endif
 favol = new ImageVolumeV2N4<double>(eigenvol->getRDIM(), eigenvol->getCDIM(), eigenvol->getZDIM(), double(), eigenvol->getRLENGTH(), eigenvol->getCLENGTH(), eigenvol->getZLENGTH());
 eigenvol->calculateFAVolume(*favol);

 



}



template<class T>
FluidMechanicsTrack3DV1N4<T>::~FluidMechanicsTrack3DV1N4()
{

 if(eigenvol != NULL)
 {
  delete eigenvol;
  eigenvol = NULL;
 }

 if(favol != NULL)
 {
  delete favol;
  favol = NULL;
 }

 

}

//INIT FUNCTIONS
template<class T>
void FluidMechanicsTrack3DV1N4<T>::initEigenVol(const FMArgParserV1N4 &dtiap)  
//initializes the eigenvolume from the filename provided by the user, contained in the DTIStreamlineArgParser objects
{

 //assumes that dtiargparser has already been initialized
 eigenvol = new EigenVolume3DV1N4<double>();
 FileNameV1N1<> *fptr = hageman::constructFileNameObjFromString(dtiap.getSingleParameter(DSL_EIGENVOL_FLAG));
 hageman::changeMissingPathToCurrentDirectory(fptr);
 eigenvol->readInAsAnalyzeImageForBSuite(fptr);

 //assigns a default value to STEPSIZE based on the voxel lengths (default = 1% of smallest voxel length)
 STEPSIZE = 0.01f*(eigenvol->getSmallestVoxelLength());

 delete fptr;


}


template<class T>
void FluidMechanicsTrack3DV1N4<T>::initFAVol()  
//initializes the favolume from the object's eigenvolume
{
 //assumes that dtiargparser has already been initialized
 favol = new ImageVolumeV2N4<double>(eigenvol->getRDIM(), eigenvol->getCDIM(), eigenvol->getZDIM(), double(), eigenvol->getRLENGTH(), eigenvol->getCLENGTH(), eigenvol->getZLENGTH());
 eigenvol->calculateFAVolume(*favol);
 
}


template<class T>
void FluidMechanicsTrack3DV1N4<T>::initTractographyConstants(const FMArgParserV1N4 &dtiap) 
//initializes the tractography constants
{

 //the minimum value for the local FA below which the tract is terminated (value of 0.0f means that there is no FA threshold) (default = 0.04)
 if(dtiap.isThereMINFATHRESHOLD())
  MINFATHRESHOLD = dtiap.getMINFATHRESHOLD();  
 
 //the maximum length for the tracts (value of 0.0f means that there is no max tract length) (default = 0.0f)
 if(dtiap.isThereMAXTRACTLENGTH())
  MAXTRACTLENGTH = dtiap.getMAXTRACTLENGTH();

 //the minimum length for the tracts (value of 0.0f means that there is no min tract length) (default = 0.0f)
 if(dtiap.isThereMINTRACTLENGTH())
  MINTRACTLENGTH = dtiap.getMINTRACTLENGTH();

 //the stepsize taken before the eigenvector field is sampled again (default = 25% of smallest voxel dimension)
 if(dtiap.isThereSTEPSIZE())
  STEPSIZE = dtiap.getSTEPSIZE();

 //the maximum angle that the tract is allowed to turn (in degrees) (if value is 0.0f, then there is no max turn angle, but abs value of direction is still taken to make sure the tract doesnt double back on itself) (default = 0.0f)
 if(dtiap.isThereMAXTURNANGLE())
 {
  //remember that MAXTURNANGLE is assumed to be per mm
  MAXTURNANGLE = dtiap.getMAXTURNANGLE();
  MAXTURNANGLE = (MAXTURNANGLE*3.14159)/180.0;  //converts to radians (rads = angle*(PI/180))
 }
 


}

//CHECK FUNCTIONS
template<class T>
bool FluidMechanicsTrack3DV1N4<T>::isBelowFAThreshold(const Vector3V2N4<double> &pt) const  
//returns whether the FA at the current pt is below MINFATHRESHOLD
{

 Vector3V2N4<int> voxelpt(getVoxelCoordinatesForRealWorldPt(*favol,pt));  //returns the coords of the current voxel that pt is in.

 if((favol->getElement(voxelpt.x(), voxelpt.y(), voxelpt.z())) < (this->MINFATHRESHOLD))
  return true;

 return false;

}


template<class T>
bool FluidMechanicsTrack3DV1N4<T>::isGreaterThanMaxTurnAngle(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, TractEnd tend) const  
//returns whether the angle between the terminal pt in tract and pt is greater than MAXTURNANGLE
{
 
 double angle = 0.0;
 
 if(tend == hageman::BACK)  //the pt is being added to the back end of the tract
 {
  angle = fabsf(tract.getTurnAngleAtFront(pt));  //need to be the absolute value since this is a magnitude metric
  
  //remember that MAXTURNANGLE is assumed to be per mm, so we must divide the turn angle by the length
  double length = tract.getDistanceToNextPoint(tract.getNumPoints()-2);

  Vector3V2N4<double> endpt(tract.getPoint(tract.getNumPoints()-1));
  Vector3V2N4<double> diff;
  diff = (pt-endpt);
  length += diff.L2norm();
  
  angle = angle/length;

#if VALIDATE
  std::cout<<"Angle per mm: "<<angle<<std::endl;
#endif

 }
 else  //the pt is being added to the front end of the tract
 {

  angle = fabsf(tract.getTurnAngleAtBack(pt));
  
  //remember that MAXTURNANGLE is assumed to be per mm, so we must divide the turn angle by the length
  double length = tract.getDistanceToLastPoint(1);
  
  Vector3V2N4<double> begpt(tract.getPoint(0));
  Vector3V2N4<double> diff;
  diff = (pt-begpt);
  length += diff.L2norm();
  
  angle = angle/length;
#if VALIDATE  
  std::cout<<"Angle per mm: "<<angle<<std::endl;
#endif

 }

 if(angle > (this->MAXTURNANGLE))
  return true;
 
 return false;

}


template<class T>
bool FluidMechanicsTrack3DV1N4<T>::isLongerThanMaxTractLength(const DTITract3DV1N3<double> &tract) const
//return whether the length of the tract is longer than MAXTRACTLENGTH
{
 if(tract.getLength() > MAXTRACTLENGTH)
  return true;
 
 return false;
}

template<class T>
bool FluidMechanicsTrack3DV1N4<T>::isLongerThanMinTractLength(const DTITract3DV1N3<double> &tract) const
//return whether the length of the tract is longer than MINTRACTLENGTH
{
 if(tract.getLength() < MINTRACTLENGTH)
  return false;
 
 return true;
}

//TRACT FUNCTIONS





//PRINT FUNCTIONS
template<class T>
void FluidMechanicsTrack3DV1N4<T>::printTractographyConstants()
{

 std::cout<<"MINFATHRESHOLD: "<<this->MINFATHRESHOLD<<std::endl;
 std::cout<<"MINTRACTLENGTH: "<<this->MINTRACTLENGTH<<std::endl;
 std::cout<<"MAXTRACTLENGTH: "<<this->MAXTRACTLENGTH<<std::endl;
 std::cout<<"MAXTURNANGLE: "<<this->MAXTURNANGLE<<std::endl;
 std::cout<<"STEPSIZE: "<<this->STEPSIZE<<std::endl;



}

//READ FUNCTIONS



//WRITE FUNCTIONS

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutIncludedTracts(std::string ofprefix) const
//writes out the included tracts using ofprefix (assumed to contain path info, if needed)
{
#if VERBOSE
 std::cout<<"Writing Out Tracts..."<<std::endl;
#endif

 this->writeOutIncludedTractsAsUCF(ofprefix);
 
}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutExcludedTracts(std::string ofprefix) const
//writes out the excluded tracts using ofprefix (assumed to contain path info, if needed)
{
#if VERBOSE
 std::cout<<"Writing Out Excluded Tracts..."<<std::endl;
#endif

 this->writeOutExcludedTractsAsUCF(ofprefix);
}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutIncludedTractsAsUCF(std::string ofprefix) const
//writes out the included tracts as UCFs using ofprefix (assumed to contain path info, if needed)
{

 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;

 for(int idx = 0; idx < includedtracts.size(); idx++)
 {
  sprintf(buffer,"%i",idx);
  ofname = ofprefix;
  ofname += "_tract";
  ofname += std::string(buffer);
  ofname += ".ucf";

  fptr = hageman::constructFileNameObjFromString(ofname);
  hageman::changeMissingPathToCurrentDirectory(fptr);
  
  (includedtracts.at(idx)).writeOutAsUCF(fptr, favol->getRDIM(), favol->getCDIM(), favol->getZDIM());
  delete fptr;
  fptr = NULL;
 }


}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutExcludedTractsAsUCF(std::string ofprefix) const
//writes out the excluded tracts as UCFs using ofprefix (assumed to contain path info, if needed)
{

 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;
 for(int idx = 0; idx < excludedtracts.size(); idx++)
 {
  sprintf(buffer,"%i",idx);
  ofname = ofprefix;
  ofname += "_excludedtract";
  ofname += std::string(buffer);
  ofname += ".ucf";

  fptr = hageman::constructFileNameObjFromString(ofname);
  hageman::changeMissingPathToCurrentDirectory(fptr);
  
  (excludedtracts.at(idx)).writeOutAsUCF(fptr, favol->getRDIM(), favol->getCDIM(), favol->getZDIM());
  delete fptr;
  fptr = NULL;
 }

}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutIncludedTractsForBSuite(std::string ofprefix) const
{

 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;
 for(int idx = 0; idx < includedtracts.size(); idx++)
 {
  //itoa(idx,buffer,10);
  sprintf(buffer,"%i",idx);
  ofname = ofprefix;
  ofname += "_tract";
  ofname += std::string(buffer);
  ofname += ".txt";

  fptr = hageman::constructFileNameObjFromString(ofname);
  hageman::changeMissingPathToCurrentDirectory(fptr);
  
  (includedtracts.at(idx)).writeOutToTxtFileForBSuite(fptr);
  delete fptr;
  fptr = NULL;
 }




}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutExcludedTractsForBSuite(std::string ofprefix) const
{

 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;
 for(int idx = 0; idx < excludedtracts.size(); idx++)
 {
  //itoa(idx,buffer,10);
  sprintf(buffer,"%i",idx);
  ofname = ofprefix;
  ofname += "_excludedtract";
  ofname += std::string(buffer);
  ofname += ".txt";

  fptr = hageman::constructFileNameObjFromString(ofname);
  hageman::changeMissingPathToCurrentDirectory(fptr);
  
  (excludedtracts.at(idx)).writeOutToTxtFileForBSuite(fptr);
  delete fptr;
  fptr = NULL;
 }




}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutMostRecentIncludedTract(std::string ofprefix) const
{
 
 this->writeOutMostRecentIncludedTractAsUCF(ofprefix);
 
}



template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutMostRecentIncludedTractAsUCF(std::string ofprefix) const
{
 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;
 int idx = includedtracts.size()-1;

 sprintf(buffer,"%i",idx);
 ofname = ofprefix;
 ofname += "_";
 ofname += std::string(buffer);
 ofname += ".ucf";

 fptr = hageman::constructFileNameObjFromString(ofname);
 hageman::changeMissingPathToCurrentDirectory(fptr);

 (includedtracts.at(idx)).writeOutAsUCF(fptr, favol->getRDIM(), favol->getCDIM(), favol->getZDIM());
 delete fptr;
 fptr = NULL;



}

template<class T>
void FluidMechanicsTrack3DV1N4<T>::writeOutMostRecentIncludedTractAsBSUITE(std::string ofprefix) const
{

 std::string ofname;
 char buffer[_CVTBUFSIZE];

 FileNameV1N1<> *fptr = NULL;
 int idx = includedtracts.size()-1;

 //itoa(idx,buffer,10);
 sprintf(buffer,"%i",idx);
 ofname = ofprefix;
 //ofname += "_includedtract";
 ofname += "_";
 ofname += std::string(buffer);
 ofname += ".txt";

 fptr = hageman::constructFileNameObjFromString(ofname);
 hageman::changeMissingPathToCurrentDirectory(fptr);
  
 (includedtracts.at(idx)).writeOutToTxtFileForBSuite(fptr);
 delete fptr;
 fptr = NULL;

  
}



template<class T>  //T is the datatype for the mask volume
class FMMaskedTrack3DV1N4 : public FluidMechanicsTrack3DV1N4<T>{

public:
 //CONSTRUCTORS
 FMMaskedTrack3DV1N4();
 FMMaskedTrack3DV1N4(const FMArgParserV1N4 &dtiap);
 ~FMMaskedTrack3DV1N4();
 
 //INIT FUNCTIONS
 void initMaskVolume(FileNameV1N1<> *fname);  //this function initializes a seedroi from a mask volume and returns a ptr to it
 
 //CHECK FUNCTIONS
 virtual bool shouldIncludeTract(const DTITract3DV1N3<double> &tract) const; //determines if tract should be in includedtracts or excludedtracts
 virtual bool shouldTerminate(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, TractEnd tend) const; //determines if an end of the tract should be terminated based 
 //on the current position, pt 
 
 //TRACT FUNCTIONS
 virtual void performStreamlineTractography(const FMArgParserV1N4 &dtiap);  //tries totaltrys times to sample a seed pt and construct a tract; writes out the tracts as they are found
 virtual void constructTract(DTITract3DV1N3<double> &result, const Vector3V2N4<double> &seedpt);  //constructs a tract starting at seedpt and evolving both ends
 virtual void constructTractEnd(DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &seedpt, TractEnd tend);  
 //constructs a tract starting at seedpt, using no interpolation
 

protected:

 ImageVolumeV2N4<T> *maskvolume;  //an array of the stop ROIs that will be used.  If empty, then no tracts are excluded
 

};



//CONSTRUCTORS
template<class T>
FMMaskedTrack3DV1N4<T>::FMMaskedTrack3DV1N4()
//default constructor
:FluidMechanicsTrack3DV1N4<T>()
{
 maskvolume = NULL;


}

template<class T>
FMMaskedTrack3DV1N4<T>::FMMaskedTrack3DV1N4(const FMArgParserV1N4 &dtiap)
//default constructor
:FluidMechanicsTrack3DV1N4<T>(dtiap)
{

 maskvolume = NULL;

 std::string imgifname(dtiap.getSingleParameter(DSL_MASKVOL_FLAG));  //get the filename from the arg parser
 hageman::FileNameV1N1<> *imgfptr = hageman::constructFileNameObjFromString(imgifname);

 //hageman::FileNameV1N1<> *imgfptr = hdrfptr->clone(); //construct the image filename
 //hageman::FileNameV1N1<> *hdrfptr = hageman::constructFileNameObjFromString(hdrifname);
 //imgfptr->setExtension(".img");

 //GENERALIZE THIS FOR OTHER IMAGE FILE FORMATS
 //ImageVolumeHeaderV1N2<T> *hdr = NULL;
 //hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfptr);
 //maskvolume = new ImageVolumeV2N4<T>();
 //maskvolume->readInImageVolumeFromFile(imgfptr,hdr);
 
 T dumval = T();
 ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfptr,dumval);
 maskvolume = new ImageVolumeV2N4<T>();
 maskvolume->readInImageVolumeFromFile(hdr);
 
 //delete hdrfptr;
 //hdrfptr = NULL;
 delete imgfptr;
 imgfptr = NULL;
 delete hdr;
 hdr = NULL;

}


template<class T>
FMMaskedTrack3DV1N4<T>::~FMMaskedTrack3DV1N4()
//default constructor
{

 //deletes maskvolume
 if(maskvolume != NULL)
 {
  delete maskvolume;
  maskvolume = NULL;
 }
 
 

}



template<class T>
void FMMaskedTrack3DV1N4<T>::performStreamlineTractography(const FMArgParserV1N4 &dtiap)  
//tries totaltrys times to sample a seed pt and construct a tract; writes out the tracts as they are found
{

 Vector3V2N4<double> seedpt;
 int totaltrys = dtiap.getTotalTrys();  //gets the user specified totaltrys on the CL; if not present, then it defaults to 1
 int tractidx = 0;
 int foundtractidx = 0;
 
 //constructs the tract filename prefix, which includes path info
 std::string tractprefix;
 if(dtiap.isThereOFPath())
 {
  tractprefix = (dtiap.getSingleParameter(DSL_OFPATH_FLAG));
  hageman::addTerminatingBackslashToDirectoryString(tractprefix);
 }
 if(dtiap.doesFlagExist(DSL_OFPREFIX_FLAG))
  tractprefix += dtiap.getSingleParameter(DSL_OFPREFIX_FLAG);
 else
  tractprefix += "tractout";
 
 for(int idx = 0; idx < totaltrys; idx++)  //for each try, sample a point from each voxel of the masked volume
  for(int seedidx = 0; seedidx < this->favol->getSIZE(); seedidx++)  //loops over the number of seedrois
  //for(int seedidx = 0; seedidx < 10000; seedidx++)  //loops over the number of seedrois
  {

   std::vector<int> seedvoxel(this->favol->convertToCoords(seedidx));
    
#if VERBOSE
   if((seedidx % 100) == 0)
   {
    std::cout<<"Have sampled "<<seedidx<<" pts..."<<std::endl;
    std::cout<<"Currently at "<<seedvoxel.at(0)<<","<<seedvoxel.at(1)<<","<<seedvoxel.at(2)<<std::endl;
   }
#endif


   if((maskvolume->getElement(seedidx)) != 0)
   {

    //std::vector<int> seedvoxel(favol->convertToCoords(seedidx));
    seedpt = hageman::getRandomPtFromImageVoxel(seedvoxel.at(0), seedvoxel.at(1), seedvoxel.at(2), *(this->favol));  //gets a 
    //random real world seed pt from the current seed voxel
     
    DTITract3DV1N3<double> currenttract;
    this->constructTract(currenttract,seedpt);
      
    //checks to see whether to include or exclude the tract
    if(this->shouldIncludeTract(currenttract))
    {
     if(!(currenttract.isEmpty()))  //makes certain the tract is not empty
     {
 #if VERBOSE
      std::cout<<"Found a tract!"<<std::endl;
 #endif

      std::string ofname;
      char buffer[_CVTBUFSIZE];

      sprintf(buffer,"%i",foundtractidx);
      ofname = tractprefix;
      ofname += "_";
      ofname += std::string(buffer);
      ofname += ".ucf";

      hageman::FileNameV1N1<> *fptr = hageman::constructFileNameObjFromString(ofname);
      hageman::changeMissingPathToCurrentDirectory(fptr);

      currenttract.writeOutAsUCF(fptr, this->favol->getRDIM(), this->favol->getCDIM(), this->favol->getZDIM());
      delete fptr;
      fptr = NULL;
      foundtractidx++;

      //removed this section because of memory issues
      //includedtracts.push_back(currenttract);
      //this->writeOutMostRecentIncludedTract(tractprefix);  //writes out the tract just added so that the tracts are being written out as they are found
     }
      
    }
    else if(dtiap.doesFlagExist(DSL_WRITEEXCLUDED_FLAG))  //a flag for collecting the excluded tracts for future writing out
    {
     if(!(currenttract.isEmpty()))  //makes certain the tract is not empty
      this->excludedtracts.push_back(currenttract);
    }

    //sets a limit for the max number of tracts the program will find before terminating
    if(dtiap.isThereMaxTractNumber())
    {
     //if(includedtracts.size() >= dtiap.getMaxTractNumber())
     if(foundtractidx >= dtiap.getMaxTractNumber())
      return;
    }

    tractidx++;
   }
 }

  


}

 
template<class T>
void FMMaskedTrack3DV1N4<T>::constructTract(DTITract3DV1N3<double> &result, const Vector3V2N4<double> &seedpt)  
//constructs a tract starting at seedpt
{
 
 //checks to see if the seed point itself is in an area that denotes termination.  If so, don't proceed
 if(!(this->shouldTerminate(result,seedpt,hageman::BACK)))
 {
 
  Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),seedpt));

  //the general equation for steps is r(n+1) = r(n) + h(Vn+1) (Euler method)
#if VERBOSE
  std::cout<<"Constructing Tract Starting at Seed Voxel: "<<voxpt<<std::endl;
#endif

  //add the starting point to the tract
  result.addPointAtBack(seedpt);
 
  //constructs the front end
  this->constructTractEnd(result,seedpt,FRONT);
 
  //constructs the back end
  this->constructTractEnd(result,seedpt,BACK);


 }


}

template<class T>
void FMMaskedTrack3DV1N4<T>::constructTractEnd(DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &seedpt, TractEnd tend)  
//constructs a tract starting at seedpt, using no interpolation with an initial direction of Ezero
{

 Vector3V2N4<double> ezero(this->eigenvol->getV1ConstReference(seedpt));
 Vector3V2N4<double> Er(ezero);
 Vector3V2N4<double> Er1(Er);
 Vector3V2N4<double> Etemp;
 Vector3V2N4<double> rn(seedpt);
 Vector3V2N4<double> rn1(rn);

 if(tend == BACK)  //if constructing the back end of the tract, then reverse the direction of the initial eigensystem
  Er.scalarMultiply(-1.0f);

 Er.normalize();  //done for safety

 //the general equation for steps is r(n+1) = r(n) + h(Vn+1) (Euler method)
 Er.scalarMultiply(this->STEPSIZE);  //multiplies the normalized direction vector by STEPSIZE before adding it to the current direction

 //if(this->shouldTerminate(tract,rn,tend))  //THIS SHOULD NOT BE NECESSARY SINCE THE SEEDPT HAS ALREADY BEEN CHECKED
 // return;
 
 while(1)  //the while loop is continued until a break
 {

  rn1 = rn + Er;
  
  Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),rn1));

  //checks whether the new pt is within the volume before accessing volumes to check for termination criteria
  if(!(this->favol->inBounds(voxpt.x(), voxpt.y(), voxpt.z())))
  {
#if VERBOSE
   std::cout<<"Tract has reached end of volume. Terminating end..."<<std::endl;
#endif
   break;
  }

  //only calculates a new eigenvector direction if the current pt has entered a new voxel 
  if(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol), rn1) != hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol), rn))
  {
   
   //Etemp.initVector3(eigenvol->getV1ConstReference(voxpt.x(), voxpt.y(), voxpt.z()));
   //Er1.initVector3(Etemp.x(),Etemp.y(),Etemp.z());

   Er1.initVector3(this->eigenvol->getV1ConstReference(voxpt.x(), voxpt.y(), voxpt.z()));  //get the new direction
   
   Er.normalize();  //normalized the previous direction before taking the dot product
   if((Er1.dot(Er)) < 0)  //if the new direction is oriented backwards of the previous direction, then flip the eigenvector
    Er1.scalarMultiply(-1.0);
   
   Er1.normalize();  //done for safety
   Er.initVector3(Er1);  //sets the current direction as the new direction
   Er.scalarMultiply(this->STEPSIZE);  //multiplies the normalized direction vector by STEPSIZE before adding it to the current direction

  }

  rn = rn1;  //sets the new pt as the current pt

  if(this->shouldTerminate(tract,rn,tend))  //if the tract meets the termination criteria, then break out of this while loop before adding this point to the tract
   return;

  //add the current point to the tract
  if(tend == FRONT)
   tract.addPointAtBack(rn);
  else
   tract.addPointAtFront(rn);

 }
}
 


template<class T>
bool FMMaskedTrack3DV1N4<T>::shouldTerminate(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, hageman::TractEnd tend) const
//determines if an end of the tract should be terminated based on the current position, pt,
{
 
 Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),pt));
  
 //checks whether the new pt is within the volume before accessing volumes to check for termination criteria
 if(!(this->favol->inBounds(voxpt.x(), voxpt.y(), voxpt.z())))
 {
#if VERBOSE
  std::cout<<"Terminating end because tract has left volume dimensions."<<std::endl;
#endif
  return true;
 }

 //determines if the FA value at the current voxel is less than the MINFATHRESHOLD
 if(this->isBelowFAThreshold(pt))
 {
#if VERBOSE
  std::cout<<"Terminating end because FA is below threshold."<<std::endl;
  std::cout<<"FA at pt: "<<voxpt<<" = "<<this->favol->getElement(voxpt.x(), voxpt.y(), voxpt.z())<<std::endl;
#endif
  return true;
 }

 if(this->MAXTRACTLENGTH != 0.0)  //checks whether MAXTRACTLENGTH = 0 which signifies that no flag was specified by the user
 {
  if(this->isLongerThanMaxTractLength(tract))
  {
#if VERBOSE
   std::cout<<"Terminating end because tract is longer than the maximum length specified."<<std::endl;
#endif
   return true;
  }
 }

 if((tract.getNumPoints() > 2) && (this->MAXTURNANGLE != 0.0))  //checks whether MAXTURNANGLE = 0 which signifies that no flag was specified by the user
 //also the tract must be more than 2 points long to check for an angle
 {
  if(this->isGreaterThanMaxTurnAngle(tract,pt,tend))
  {
#if VERBOSE
   std::cout<<"Terminating end because tract has turned an angle that is too large."<<std::endl;
#endif
   return true;
  }
 }
 

 return false;

}


template<class T>
bool FMMaskedTrack3DV1N4<T>::shouldIncludeTract(const DTITract3DV1N3<double> &tract) const
//determines if tract should be in includedtracts or excludedtracts
{

 if(tract.isEmpty())  //if the tract is empty, then don't include it
  return false;
 
 if(this->MINTRACTLENGTH > 0)  //checks for MINTRACTLENGTH != 0, which signifies the user has specified a value on the CL
 {
  if(!(this->isLongerThanMinTractLength(tract)))  //checks whether the tract has exeeded the min tract length
   return false;
 }
 
 if(this->MAXTRACTLENGTH > 0)  //checks for MAXTRACTLENGTH != 0, which signifies the user has specified a value on the CL
 {
  //if(!this->isLongerThanMaxTractLength(tract))  //checks whether the tract has exeeded the max tract length ???
  if(this->isLongerThanMaxTractLength(tract))  //checks whether the tract has exeeded the max tract length
   return false;
 }

 
 return true;

}



template<class T>  //T is the datatype for the seed/stop ROI volumes
class FMROITrack3DV1N4 : public FluidMechanicsTrack3DV1N4<T>{

public:

 //CONSTRUCTORS
 FMROITrack3DV1N4();
 FMROITrack3DV1N4(const FMArgParserV1N4 &dtiap);
 FMROITrack3DV1N4(EigenVolume3DV1N4<double> *evol, double MINFA, double MAXTL, double MINTL, double MAXTA, const std::vector<ROI3DV1N4<int>* > &seedroisv, const std::vector<ROI3DV1N4<int>* > &stoproisv);
 
 ~FMROITrack3DV1N4();  //destructor
 
 //INIT FUNCTIONS
 ROI3DV1N4<int>* initSeedMaskROI(FileNameV1N1<> *fname);  //this function initializes a seedroi from a mask volume and returns a ptr to it
 //ROI3DV1N4<int>* initSeedMaskROI(const ImageVolumeV2N4<T> &imvol);  //this function initializes a seedroi from a mask volume and returns a ptr to it
 
 void initSeedROIs(const FMArgParserV1N4 &dtiap);  //initializes the array of seed ROIs
 //void initStopROIs(const FMArgParserV1N4 &dtiap);  //intializes the array of stop ROIs
 //ROI3DV1N4<int>* initStopMaskROI(FileNameV1N1<> *fname);  //this function initializes a stoproi from a mask volume and returns a ptr to it
 
 //CHECK FUNCTIONS
 virtual bool shouldIncludeTract(const DTITract3DV1N3<double> &tract) const; //determines if tract should be in includedtracts or excludedtracts
 virtual bool shouldTerminate(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, TractEnd tend) const; //determines if an end of the tract should be terminated based 
 //on the current position, pt 
 

 //TRACT FUNCTIONS
 virtual void performStreamlineTractography(const FMArgParserV1N4 &dtiap);  //tries totaltrys times to sample a seed pt and construct a tract; writes out the tracts as they are found
 virtual void constructTract(DTITract3DV1N3<double> &result, const Vector3V2N4<double> &seedpt);  //constructs a tract starting at seedpt and evolving both ends
 virtual void constructTractEnd(DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &seedpt, TractEnd tend);  
 //constructs a tract starting at seedpt, using no interpolation
 

protected:

 //ROIs
 std::vector<ROI3DV1N4<int>* > seedrois;  //an array of the seed rois that will limit the number of voxels used for tract seeding.
 std::vector<ROI3DV1N4<int>* > stoprois;  //an array of the stop ROIs that will be used.  If empty, then no tracts are excluded
 

};


//CONSTRUCTORS
template<class T>
FMROITrack3DV1N4<T>::FMROITrack3DV1N4()
//default constructor
:FluidMechanicsTrack3DV1N4<T>()
{


}

template<class T>
FMROITrack3DV1N4<T>::FMROITrack3DV1N4(const FMArgParserV1N4 &dtiap)
//default constructor
:FluidMechanicsTrack3DV1N4<T>(dtiap)
{
 
 //initialize the stop ROI
 //this->initStopROIs(dtiap);

 //initialize the seed ROI
 this->initSeedROIs(dtiap);

}

template<class T>
FMROITrack3DV1N4<T>::FMROITrack3DV1N4(EigenVolume3DV1N4<double> *evol, double MINFA, double MAXTL, double MINTL, double MAXTA, const std::vector<ROI3DV1N4<int>* > &seedroisv, const std::vector<ROI3DV1N4<int>* > &stoproisv)
:FluidMechanicsTrack3DV1N4<T>(evol,MINFA,MAXTL,MINTL,MAXTA)
{
 seedrois = seedroisv;
 stoprois = stoproisv;




}


template<class T>
FMROITrack3DV1N4<T>::~FMROITrack3DV1N4()
//default constructor
//:~FluidMechanicsTrack3DV1N4<T>()
{

 //deletes seedroi and stoproi vectors
 for(int idx = 0; idx < seedrois.size(); idx++)
 {
  if(seedrois.at(idx) != NULL)
  {
   delete seedrois.at(idx);
   seedrois.at(idx) = NULL;
  }
 }

 //for(int idx = 0; idx < stoprois.size(); idx++)
 //{
 // if(stoprois.at(idx) != NULL)
 // {
 //  delete stoprois.at(idx);
 //  stoprois.at(idx) = NULL;
 // }
 //}

}



template<class T>
void FMROITrack3DV1N4<T>::initSeedROIs(const FMArgParserV1N4 &dtiap)
//intializes the array of seed ROIs
{
 //if there is a mask volume list, then all the seed rois are of type VoxelROI3DV1N1
 if(dtiap.doesFlagExist(DSL_SEEDMASKVOL_FLAG))
 {
#if VERBOSE
  std::cout<<"Initializing Seed ROIs..."<<std::endl;
#endif
  
  ROI3DV1N4<int> *roiptr = NULL;

  //gets the fnames of the seed roi mask volumes
  std::vector<std::string> seedfnames(dtiap.getSeedMaskVolFNameVec());
  FileNameV1N1<> *fptr = NULL;

  //reads all the seed rois into the array using the individual mask vols
  for(int idx = 0; idx < seedfnames.size(); idx++)
  {
#if VERBOSE
   std::cout<<"Reading in "<<seedfnames.at(idx)<<std::endl;
#endif
  
   fptr = hageman::constructFileNameObjFromString(seedfnames.at(idx));
   hageman::changeMissingPathToCurrentDirectory(fptr);
   roiptr = this->initSeedMaskROI(fptr);
   seedrois.push_back(roiptr);
   delete fptr;
   fptr = NULL;
  }
 }
 else
 {
#if VERBOSE
  std::cout<<"There are no seed rois defined"<<std::endl;
#endif
 }
}


template<class T>
ROI3DV1N4<int>* FMROITrack3DV1N4<T>::initSeedMaskROI(FileNameV1N1<> *imgfname)  
//this function initializes a seedroi from a mask volume and returns a ptr to it
{

 VoxelROI3DV1N4<int> *roiptr = new VoxelROI3DV1N4<int>();

 //GENERALIZE THIS FOR OTHER FILE FORMATS
 //FileNameV1N1<> *imgfname = hdrfname->clone();
 //imgfname->setExtension(".img");

 //ImageVolumeHeaderV1N2<T> *hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfname);
 T dumval = T();
 ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfname,dumval);
 roiptr->initROIFromMaskVolume(hdr);

 delete hdr;
 hdr = NULL;

 return roiptr;

}





//template<class T>
//ROI3DV1N4<int>* FluidMechanicsTrack3DV1N4<T>::initStopMaskROI(FileNameV1N1<> *fname)  
//this function initializes a stoproi from a mask volume and returns a ptr to it
//{
// VoxelROI3DV1N4<int> *roiptr = new VoxelROI3DV1N4<int>();
// roiptr->initROIFromAnalyzeMaskVolume<T>(fname);
// return roiptr;
//}
 

template<class T>
void FMROITrack3DV1N4<T>::performStreamlineTractography(const FMArgParserV1N4 &dtiap)  
//tries totaltrys times to sample a seed pt and construct a tract; writes out the tracts as they are found
{

 Vector3V2N4<double> seedpt;
 int totaltrys = dtiap.getTotalTrys();  //gets the user specified totaltrys on the CL; if not present, then it defaults to 1
 int tractidx = 0;
 int foundtractidx = 0;
 
 //constructs the tract filename prefix, which includes path info
 std::string tractprefix;
 if(dtiap.isThereOFPath())
 {
  tractprefix = (dtiap.getSingleParameter(DSL_OFPATH_FLAG));
  hageman::addTerminatingBackslashToDirectoryString(tractprefix);
 }
 if(dtiap.doesFlagExist(DSL_OFPREFIX_FLAG))
  tractprefix += dtiap.getSingleParameter(DSL_OFPREFIX_FLAG);
 else
  tractprefix += "tractout";
 

 for(int idx = 0; idx < totaltrys; idx++)  //for each try, sample a point from each of the seedrois
  for(int seedidx = 0; seedidx < seedrois.size(); seedidx++)  //loops over the number of seedrois
  { 
#if VERBOSE
    if((tractidx % 10) == 0)
      std::cout<<"Have sampled "<<tractidx<<" pts..."<<std::endl;
#endif

    seedpt = (seedrois.at(seedidx))->getRandomRealWorldPt(this->favol->getRLENGTH(), this->favol->getCLENGTH(), this->favol->getZLENGTH());  //gets a 
    //random real world seed pt from the current seed voxel

    DTITract3DV1N3<double> currenttract;
    this->constructTract(currenttract,seedpt);
     
    //checks to see whether to include or exclude the tract
    if(this->shouldIncludeTract(currenttract))
    {
     if(!(currenttract.isEmpty()))  //makes certain the tract is not empty
     {
#if VERBOSE
      std::cout<<"Found a tract!"<<std::endl;
#endif

      std::string ofname;
      char buffer[_CVTBUFSIZE];

      sprintf(buffer,"%i",foundtractidx);
      ofname = tractprefix;
      ofname += "_";
      ofname += std::string(buffer);
      ofname += ".ucf";

      hageman::FileNameV1N1<> *fptr = hageman::constructFileNameObjFromString(ofname);
      hageman::changeMissingPathToCurrentDirectory(fptr);

      currenttract.writeOutAsUCF(fptr, this->favol->getRDIM(), this->favol->getCDIM(), this->favol->getZDIM());
      delete fptr;
      fptr = NULL;
      foundtractidx++;

      //removed this section because of memory issues
      (this->includedtracts).push_back(currenttract);
      //this->writeOutMostRecentIncludedTract(tractprefix);  //writes out the tract just added so that the tracts are being written out as they are found
     }
     
    }
    else if(dtiap.doesFlagExist(DSL_WRITEEXCLUDED_FLAG))  //a flag for collecting the excluded tracts for future writing out
    {
     if(!(currenttract.isEmpty()))  //makes certain the tract is not empty
      this->excludedtracts.push_back(currenttract);
    }

    //sets a limit for the max number of tracts the program will find before terminating
    if(dtiap.isThereMaxTractNumber())
    {
     //if(includedtracts.size() >= dtiap.getMaxTractNumber())
     if(foundtractidx >= dtiap.getMaxTractNumber())
      return;
    }

    tractidx++;
  }

 

}

 
template<class T>
void FMROITrack3DV1N4<T>::constructTract(DTITract3DV1N3<double> &result, const Vector3V2N4<double> &seedpt)  
//constructs a tract starting at seedpt
{
 
 //checks to see if the seed point itself is in an area that denotes termination.  If so, don't proceed
 if(!(this->shouldTerminate(result,seedpt,hageman::BACK)))
 {
 
  Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),seedpt));

  //the general equation for steps is r(n+1) = r(n) + h(Vn+1) (Euler method)
#if VERBOSE
  std::cout<<"Constructing Tract Starting at Seed Voxel: "<<voxpt<<std::endl;
#endif

  //add the starting point to the tract
  result.addPointAtBack(seedpt);
 
  //constructs the front end
  this->constructTractEnd(result,seedpt,FRONT);
 
  //constructs the back end
  this->constructTractEnd(result,seedpt,BACK);


 }


}

template<class T>
void FMROITrack3DV1N4<T>::constructTractEnd(DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &seedpt, TractEnd tend)  
//constructs a tract starting at seedpt, using no interpolation with an initial direction of Ezero
{

 Vector3V2N4<double> ezero(this->eigenvol->getV1ConstReference(seedpt));
 Vector3V2N4<double> Er(ezero);
 Vector3V2N4<double> Er1(Er);
 Vector3V2N4<double> Etemp;
 Vector3V2N4<double> rn(seedpt);
 Vector3V2N4<double> rn1(rn);

 if(tend == BACK)  //if constructing the back end of the tract, then reverse the direction of the initial eigensystem
  Er.scalarMultiply(-1.0f);

 Er.normalize();  //done for safety

 //the general equation for steps is r(n+1) = r(n) + h(Vn+1) (Euler method)
 Er.scalarMultiply(this->STEPSIZE);  //multiplies the normalized direction vector by STEPSIZE before adding it to the current direction

 //if(this->shouldTerminate(tract,rn,tend))  //THIS SHOULD NOT BE NECESSARY SINCE THE SEEDPT HAS ALREADY BEEN CHECKED
 // return;
 
 while(1)  //the while loop is continued until a break
 {

  rn1 = rn + Er;
  
  Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),rn1));

  //checks whether the new pt is within the volume before accessing volumes to check for termination criteria
  if(!(this->favol->inBounds(voxpt.x(), voxpt.y(), voxpt.z())))
  {
#if VERBOSE
   std::cout<<"Tract has reached end of volume. Terminating end..."<<std::endl;
#endif
   break;
  }

  //only calculates a new eigenvector direction if the current pt has entered a new voxel 
  if(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol), rn1) != hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol), rn))
  {
   
   //Etemp.initVector3(eigenvol->getV1ConstReference(voxpt.x(), voxpt.y(), voxpt.z()));
   //Er1.initVector3(Etemp.x(),Etemp.y(),Etemp.z());

   Er1.initVector3(this->eigenvol->getV1ConstReference(voxpt.x(), voxpt.y(), voxpt.z()));  //get the new direction
   
   Er.normalize();  //normalized the previous direction before taking the dot product
   if((Er1.dot(Er)) < 0)  //if the new direction is oriented backwards of the previous direction, then flip the eigenvector
    Er1.scalarMultiply(-1.0);
   
   Er1.normalize();  //done for safety
   Er.initVector3(Er1);  //sets the current direction as the new direction
   Er.scalarMultiply(this->STEPSIZE);  //multiplies the normalized direction vector by STEPSIZE before adding it to the current direction

  }

  rn = rn1;  //sets the new pt as the current pt

  if(this->shouldTerminate(tract,rn,tend))  //if the tract meets the termination criteria, then break out of this while loop before adding this point to the tract
   return;

  //add the current point to the tract
  if(tend == FRONT)
   tract.addPointAtBack(rn);
  else
   tract.addPointAtFront(rn);

 }
}
 


template<class T>
bool FMROITrack3DV1N4<T>::shouldTerminate(const DTITract3DV1N3<double> &tract, const Vector3V2N4<double> &pt, hageman::TractEnd tend) const
//determines if an end of the tract should be terminated based on the current position, pt,
{
 
 Vector3V2N4<int> voxpt(hageman::getVoxelCoordinatesForRealWorldPt(*(this->favol),pt));
  
 //checks whether the new pt is within the volume before accessing volumes to check for termination criteria
 if(!(this->favol->inBounds(voxpt.x(), voxpt.y(), voxpt.z())))
 {
#if VERBOSE
  std::cout<<"Terminating end because tract has left volume dimensions."<<std::endl;
#endif
  return true;
 }

 //determines if the FA value at the current voxel is less than the MINFATHRESHOLD
 if(this->isBelowFAThreshold(pt))
 {
#if VERBOSE
  std::cout<<"Terminating end because FA is below threshold."<<std::endl;
  std::cout<<"FA at pt: "<<voxpt<<" = "<<this->favol->getElement(voxpt.x(), voxpt.y(), voxpt.z())<<std::endl;
#endif
  return true;
 }

 if(this->MAXTRACTLENGTH != 0.0)  //checks whether MAXTRACTLENGTH = 0 which signifies that no flag was specified by the user
 {
  if(this->isLongerThanMaxTractLength(tract))
  {
#if VERBOSE
   std::cout<<"Terminating end because tract is longer than the maximum length specified."<<std::endl;
#endif
   return true;
  }
 }

 if((tract.getNumPoints() > 2) && (this->MAXTURNANGLE != 0.0))  //checks whether MAXTURNANGLE = 0 which signifies that no flag was specified by the user
 //also the tract must be more than 2 points long to check for an angle
 {
  if(this->isGreaterThanMaxTurnAngle(tract,pt,tend))
  {
#if VERBOSE
   std::cout<<"Terminating end because tract has turned an angle that is too large."<<std::endl;
#endif
   return true;
  }
 }
 

 return false;

}


template<class T>
bool FMROITrack3DV1N4<T>::shouldIncludeTract(const DTITract3DV1N3<double> &tract) const
//determines if tract should be in includedtracts or excludedtracts
{

 if(tract.isEmpty())  //if the tract is empty, then don't include it
  return false;
 
 if(this->MINTRACTLENGTH > 0)  //checks for MINTRACTLENGTH != 0, which signifies the user has specified a value on the CL
 {
  if(!(this->isLongerThanMinTractLength(tract)))  //checks whether the tract has exeeded the min tract length
   return false;
 }
 
 if(this->MAXTRACTLENGTH > 0)  //checks for MAXTRACTLENGTH != 0, which signifies the user has specified a value on the CL
 {
  //if(!this->isLongerThanMaxTractLength(tract))  //checks whether the tract has exeeded the max tract length ???
  if(this->isLongerThanMaxTractLength(tract))  //checks whether the tract has exeeded the max tract length
   return false;
 }


 for(int idx = 0; idx < stoprois.size(); idx++)  //loops through the stop ROIs, checking whether the current pt is contained in them
 {
  std::cout<<"Yes there are stop rois"<<std::endl;
  const ROI3DV1N4<int>* const ptr = reinterpret_cast<const ROI3DV1N4<int>* const>(stoprois.at(idx));  //gets the current stop ROI as a base class ptr
  if(!(tract.doesTractIntersectROI3D(ptr,*(this->favol))))  //if the tract doesn't intersect even one of the stop ROIs, then reject the tract
   return false;
 }
 
 return true;

}




template<class T>
ROI3DV1N4<int>* initSeedMaskROI(const ImageVolumeV2N4<T> &imvol)  
//this function initializes a seedroi from a mask volume and returns a ptr to it
{

 VoxelROI3DV1N4<int> *roiptr = new VoxelROI3DV1N4<int>();

 //GENERALIZE THIS FOR OTHER FILE FORMATS
 //FileNameV1N1<> *imgfname = hdrfname->clone();
 //imgfname->setExtension(".img");

 //ImageVolumeHeaderV1N2<T> *hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfname);
 //T dumval = T();
 //ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfname,dumval);
 roiptr->initROIFromMaskVolume(imvol);

 //delete hdr;
 //hdr = NULL;

 return roiptr;

}


template<class T>
ROI3DV1N4<int>* initStopMaskROI(const ImageVolumeV2N4<T> &imvol)  
//this function initializes a stoproi from a mask volume and returns a ptr to it
{

 VoxelROI3DV1N4<int> *roiptr = new VoxelROI3DV1N4<int>();

 //GENERALIZE THIS FOR OTHER FILE FORMATS
 //FileNameV1N1<> *imgfname = hdrfname->clone();
 //imgfname->setExtension(".img");

 //ImageVolumeHeaderV1N2<T> *hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfname);
 //T dumval = T();
 //ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfname,dumval);
 roiptr->initROIFromMaskVolume(imvol);

 //delete hdr;
 //hdr = NULL;

 return roiptr;

}



template<class T>
void runITK(const FMArgParserV1N4 &dtiargs, T dumval)
{

 std::cout<<"Running ITK version..."<<std::endl;
 //initialize the eigenvolume
 std::cout<<"Initializing EigenVolume..."<<std::endl;
 std::string difffname(dtiargs.getSingleParameter(DSL_EIGENVOL_FLAG));
 typedef itk::Image<itk::DiffusionTensor3D<double>,3> DiffImageType;
 typedef itk::Image<T,3> MaskImageType;
 typedef itk::ImageFileReader<MaskImageType> ReaderType;

 itk::ImageFileReader<DiffImageType>::Pointer diffreader = itk::ImageFileReader<DiffImageType>::New();
 //std::string difffname("DiffusionEditor_1._TensorNode.nrrd");
 diffreader->SetFileName(difffname.c_str());
 diffreader->Update();
 typename DiffImageType::Pointer diffimage = diffreader->GetOutput();
  
 hageman::EigenVolume3DV1N4<double> *evol = new hageman::EigenVolume3DV1N4<double>(hageman::getRDIMITK<itk::DiffusionTensor3D<double> >(diffimage), hageman::getCDIMITK<itk::DiffusionTensor3D<double> >(diffimage), hageman::getZDIMITK<itk::DiffusionTensor3D<double> >(diffimage), hageman::EigenSystem3DV1N4<double>(), hageman::getRLENGTHITK<itk::DiffusionTensor3D<double> >(diffimage), hageman::getCLENGTHITK<itk::DiffusionTensor3D<double> >(diffimage), hageman::getZLENGTHITK<itk::DiffusionTensor3D<double> >(diffimage));
 hageman::constructEigenVolumeITK<double>(diffimage,*evol);

 //hageman::FileNameV1N1<> *evolfptr = hageman::constructFileNameObjFromString(difffname);
 //evolfptr->setExtension(".img");
 //evol->writeOutAsAnalyzeImageForBSuite(evolfptr);
 //delete evolfptr;
 //evolfptr = NULL;
 
 
 std::cout<<"Initializing Fluid Tractography Constants..."<<std::endl;
 
 double MINFATHRESHOLD = 0.0;  //the minimum value for the local FA below which the tract is terminated (value of 0.0f means that there is no FA threshold) (default = 0.04)
 double MAXTRACTLENGTH = 0.0;  //the maximum length for the tracts (value of 0.0f means that there is no max tract length) (default = 0.0f)
 double MINTRACTLENGTH = 0.0; //the minimum length for the tracts (value of 0.0f means that there is no min tract length) (default = 0,0f)
 double STEPSIZE = 0.0; //the stepsize taken before the eigenvector field is sampled again (default = 10% of smallest voxel dimension)
 double MAXTURNANGLE = 0.0; //the maximum angle that the tract is allowed to turn (in degrees)

 //the minimum value for the local FA below which the tract is terminated (value of 0.0f means that there is no FA threshold) (default = 0.04)
 if(dtiargs.isThereMINFATHRESHOLD())
  MINFATHRESHOLD = dtiargs.getMINFATHRESHOLD();  
 
 //the maximum length for the tracts (value of 0.0f means that there is no max tract length) (default = 0.0f)
 if(dtiargs.isThereMAXTRACTLENGTH())
  MAXTRACTLENGTH = dtiargs.getMAXTRACTLENGTH();

 //the minimum length for the tracts (value of 0.0f means that there is no min tract length) (default = 0.0f)
 if(dtiargs.isThereMINTRACTLENGTH())
  MINTRACTLENGTH = dtiargs.getMINTRACTLENGTH();

 //the stepsize taken before the eigenvector field is sampled again (default = 25% of smallest voxel dimension)
 if(dtiargs.isThereSTEPSIZE())
  STEPSIZE = dtiargs.getSTEPSIZE();

 //the maximum angle that the tract is allowed to turn (in degrees) (if value is 0.0f, then there is no max turn angle, but abs value of direction is still taken to make sure the tract doesnt double back on itself) (default = 0.0f)
 if(dtiargs.isThereMAXTURNANGLE())
 {
  //remember that MAXTURNANGLE is assumed to be per mm
  MAXTURNANGLE = dtiargs.getMAXTURNANGLE();
  MAXTURNANGLE = (MAXTURNANGLE*3.14159)/180.0;  //converts to radians (rads = angle*(PI/180))
 }


 //initialize the seedROI
 std::vector<ROI3DV1N4<int>* > seedroisv;

 std::vector<ROI3DV1N4<int>* > stoproisv;
 
 //if there is a mask volume list, then all the seed rois are of type VoxelROI3DV1N1
 if(dtiargs.doesFlagExist(DSL_SEEDMASKVOL_FLAG))
 {
#if VERBOSE
  std::cout<<"Initializing Fluid Source ROIs..."<<std::endl;
#endif
  
  ROI3DV1N4<int> *roiptr = NULL;

  //gets the fnames of the seed roi mask volumes
  std::vector<std::string> seedfnames(dtiargs.getSeedMaskVolFNameVec());
  FileNameV1N1<> *fptr = NULL;

  //reads all the seed rois into the array using the individual mask vols
  for(int idx = 0; idx < seedfnames.size(); idx++)
  {
#if VERBOSE
   std::cout<<"Reading in "<<seedfnames.at(idx)<<std::endl;
#endif
  
   fptr = hageman::constructFileNameObjFromString(seedfnames.at(idx));
   hageman::changeMissingPathToCurrentDirectory(fptr);
   
   typename ReaderType::Pointer maskreader = ReaderType::New();
   std::string maskfname(fptr->getOSCompatibleFileName());
   maskreader->SetFileName(maskfname.c_str());
   maskreader->Update();
   typename MaskImageType::Pointer maskimage = maskreader->GetOutput();

   hageman::ImageVolumeV2N4<T> *imvol = new hageman::ImageVolumeV2N4<T>(hageman::getRDIMITK<T>(maskimage), hageman::getCDIMITK<T>(maskimage), hageman::getZDIMITK<T>(maskimage), T(), hageman::getRLENGTHITK<T>(maskimage), hageman::getCLENGTHITK<T>(maskimage), hageman::getZLENGTHITK<T>(maskimage));
   hageman::constructVolumeITK<T>(maskimage,*imvol);
   
   hageman::printDIMSITK<T>(maskimage);

   //hageman::FileNameV1N1<> *svolfptr = hageman::constructFileNameObjFromString(maskfname);
   //svolfptr->setExtension(".hdr");
   //hageman::DataType dtv = hageman::UINT16TYPE;
   //hageman::ImageVolumeHeaderV1N2<T> *shdr = new hageman::AnalyzeVolumeHeaderV1N2<T>(svolfptr,dtv,16,false,imvol->getRDIM(),imvol->getCDIM(),imvol->getZDIM(),imvol->getRLENGTH(),imvol->getCLENGTH(),imvol->getZLENGTH());
   
   //imvol->writeOutAnalyzeVolumeToFile(svolfptr,shdr);
   //delete svolfptr;
   //svolfptr = NULL;
   //delete shdr;
   //shdr = NULL;

   //roiptr = new VoxelROI3DV1N4<int>();
   roiptr = hageman::initSeedMaskROI(*imvol);
   
   //GENERALIZE THIS FOR OTHER FILE FORMATS
   //FileNameV1N1<> *imgfname = hdrfname->clone();
   //imgfname->setExtension(".img");

   //ImageVolumeHeaderV1N2<T> *hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfname);
   //T dumval = T();
   //ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfname,dumval);
   //roiptr->initROIFromMaskVolume(*imvol);

   //delete imvol;
   //imvol = NULL;
     
   seedroisv.push_back(roiptr);
   delete fptr;
   fptr = NULL;
  }
 }
 else
 {
#if VERBOSE
  std::cout<<"There are no fluid source rois defined"<<std::endl;
#endif
 }
 
 
 //if there is a mask volume list, then all the seed rois are of type VoxelROI3DV1N1
 if(dtiargs.doesFlagExist(DSL_STOPMASKVOL_FLAG))
 {
#if VERBOSE
  std::cout<<"Initializing Tract Selection ROIs..."<<std::endl;
#endif
  
  ROI3DV1N4<int> *roiptr = NULL;

  //gets the fnames of the seed roi mask volumes
  std::vector<std::string> stopfnames(dtiargs.getStopMaskVolFNameVec());
  FileNameV1N1<> *fptr = NULL;

  //reads all the seed rois into the array using the individual mask vols
  for(int idx = 0; idx < stopfnames.size(); idx++)
  {
#if VERBOSE
   std::cout<<"Reading in "<<stopfnames.at(idx)<<std::endl;
#endif
  
   fptr = hageman::constructFileNameObjFromString(stopfnames.at(idx));
   hageman::changeMissingPathToCurrentDirectory(fptr);
   
   typename ReaderType::Pointer maskreader = ReaderType::New();
   std::string maskfname(fptr->getOSCompatibleFileName());
   maskreader->SetFileName(maskfname.c_str());
   maskreader->Update();
   typename MaskImageType::Pointer maskimage = maskreader->GetOutput();

   hageman::ImageVolumeV2N4<T> *imvol = new hageman::ImageVolumeV2N4<T>(hageman::getRDIMITK<T>(maskimage), hageman::getCDIMITK<T>(maskimage), hageman::getZDIMITK<T>(maskimage), T(), hageman::getRLENGTHITK<T>(maskimage), hageman::getCLENGTHITK<T>(maskimage), hageman::getZLENGTHITK<T>(maskimage));
   hageman::constructVolumeITK<T>(maskimage,*imvol);
   
   hageman::printDIMSITK<T>(maskimage);

   //hageman::FileNameV1N1<> *svolfptr = hageman::constructFileNameObjFromString(maskfname);
   //svolfptr->setExtension(".hdr");
   //hageman::DataType dtv = hageman::UINT16TYPE;
   //hageman::ImageVolumeHeaderV1N2<T> *shdr = new hageman::AnalyzeVolumeHeaderV1N2<T>(svolfptr,dtv,16,false,imvol->getRDIM(),imvol->getCDIM(),imvol->getZDIM(),imvol->getRLENGTH(),imvol->getCLENGTH(),imvol->getZLENGTH());
   
   //imvol->writeOutAnalyzeVolumeToFile(svolfptr,shdr);
   //delete svolfptr;
   //svolfptr = NULL;
   //delete shdr;
   //shdr = NULL;

   //roiptr = new VoxelROI3DV1N4<int>();
   roiptr = hageman::initStopMaskROI(*imvol);
   
   //GENERALIZE THIS FOR OTHER FILE FORMATS
   //FileNameV1N1<> *imgfname = hdrfname->clone();
   //imgfname->setExtension(".img");

   //ImageVolumeHeaderV1N2<T> *hdr = new AnalyzeVolumeHeaderV1N2<T>(hdrfname);
   //T dumval = T();
   //ImageVolumeHeaderV1N2<T> *hdr = hageman::constructImageVolumeHeader(imgfname,dumval);
   //roiptr->initROIFromMaskVolume(*imvol);

   //delete imvol;
   //imvol = NULL;
     
   stoproisv.push_back(roiptr);
   delete fptr;
   fptr = NULL;
  }
 }
 else
 {
#if VERBOSE
  std::cout<<"There are no tract selection rois defined"<<std::endl;
#endif
 }
 
 
 
 std::cout<<"Running Fluid Tractography Simulation..."<<std::endl;
 FluidMechanicsTrack3DV1N4<T> *dslptr = NULL;

 //if(dtiargs.isThereSeedMaskVolFName())
 //{
 dslptr = new FMROITrack3DV1N4<T>(evol,MINFATHRESHOLD,MAXTRACTLENGTH,MINTRACTLENGTH,MAXTURNANGLE,seedroisv,stoproisv);
 
 //}
 //else if(dtiargs.isThereMaskVolFName())
 //{
 // dslptr = new FMMaskedTrack3DV1N4<T>(dtiargs);
// }

 dslptr->performStreamlineTractography(dtiargs);

 std::cout<<"Performing Tractography..."<<std::endl;
 
 std::cout<<"Writing out tracts in VTK format..."<<std::endl;
 //constructs the tract filename prefix, which includes path info
 std::string tractprefix;
 if(dtiargs.isThereOFPath())
 {
  tractprefix = (dtiargs.getSingleParameter(DSL_OFPATH_FLAG));
  hageman::addTerminatingBackslashToDirectoryString(tractprefix);
 }

 //construct tract prefix for excluded tracts
 if(dtiargs.doesFlagExist(DSL_OFPREFIX_FLAG))
  tractprefix += (dtiargs.getSingleParameter(DSL_OFPREFIX_FLAG));
 else
  tractprefix += "output";

 tractprefix += "_VTK.vtp";
 std::cout<<tractprefix<<std::endl;
 hageman::writeTractsToVTPFile(dslptr->getIncludedTracts(),tractprefix, evol->getRDIM(), evol->getCDIM(), evol->getZDIM(), evol->getRLENGTH(),evol->getCLENGTH(),evol->getZLENGTH());

 //writes out the excluded tracts if the flag is specified
 //if(dtiargs.doesFlagExist(DSL_WRITEEXCLUDED_FLAG))
 // dslptr->writeOutExcludedTracts(tractprefix);

 delete dslptr;
 dslptr = NULL;
 

}



template<class T>
void run(const FMArgParserV1N4 &dtiargs, T dumval)
{

 FluidMechanicsTrack3DV1N4<T> *dslptr = NULL;

 if(dtiargs.isThereSeedMaskVolFName())
 {
  dslptr = new FMROITrack3DV1N4<T>(dtiargs);
 }
 //else if(dtiargs.isThereMaskVolFName())
 //{
 // dslptr = new FMMaskedTrack3DV1N4<T>(dtiargs);
// }

 dslptr->performStreamlineTractography(dtiargs);

 //constructs the tract filename prefix, which includes path info
 std::string tractprefix;
 if(dtiargs.isThereOFPath())
 {
  tractprefix = (dtiargs.getSingleParameter(DSL_OFPATH_FLAG));
  hageman::addTerminatingBackslashToDirectoryString(tractprefix);
 }

 //construct tract prefix for excluded tracts
 if(dtiargs.doesFlagExist(DSL_OFPREFIX_FLAG))
  tractprefix += (dtiargs.getSingleParameter(DSL_OFPREFIX_FLAG));
 else
  tractprefix += "tractout_excluded";

 //writes out the excluded tracts if the flag is specified
 if(dtiargs.doesFlagExist(DSL_WRITEEXCLUDED_FLAG))
  dslptr->writeOutExcludedTracts(tractprefix);

 delete dslptr;
 dslptr = NULL;


}



}



#endif
