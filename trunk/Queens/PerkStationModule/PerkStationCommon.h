
#ifndef PERKSTATIONCOMMON_H
#define PERKSTATIONCOMMON_H


  // Definition of plan calibration columns.

enum
  {
  CALIBRATION_COL_NAME = 0,
  CALIBRATION_COL_TX,
  CALIBRATION_COL_TY,
  CALIBRATION_COL_RO,
  CALIBRATION_COL_FV,
  CALIBRATION_COL_FH,
  CALIBRATION_COL_COUNT
  };
static const char* CALIBRATION_COL_LABELS[ CALIBRATION_COL_COUNT ] = { "Name", "TX", "TY", "RO", "FV", "FH" };
static const int CALIBRATION_COL_WIDTHS[ CALIBRATION_COL_COUNT ] = { 10, 5, 5, 5, 5, 5 };



  // Definition of plan list columns.

enum
  {
  PLAN_COL_NAME = 0,
  PLAN_COL_ER,
  PLAN_COL_EA,
  PLAN_COL_ES,
  PLAN_COL_TR,
  PLAN_COL_TA,
  PLAN_COL_TS,
  PLAN_COL_COUNT
  };
static const char* PLAN_COL_LABELS[ PLAN_COL_COUNT ] = { "Name", "ER", "EA", "ES", "TR", "TA", "TS" };
static const int PLAN_COL_WIDTHS[ PLAN_COL_COUNT ] = { 10, 5, 5, 5, 5, 5, 5 };


// --------------------------------------------------------------


static std::string BoolToString( bool d )
{
  std::stringstream ss;
  ss << d;
  return ss.str();
}


/**
 * @param cstr C string to be converted.
 * @returns String content converted to bool.
 */
static bool CharToBool( char* cstr )
{
  std::stringstream ss;
  ss << cstr;
  bool ret;
  ss >> ret;
  return ret;
}


/**
 * @param cstr C string to be converted.
 * @returns String content converted to double.
 */
static double CharToDouble( char* cstr )
{
  std::stringstream ss;
  ss << cstr;
  double ret;
  ss >> ret;
  return ret;
}


/**
 * @param cstr C string containing double numbers.
 * @returns Vector of doubles converted from argument.
 */
static std::vector< double > CharToDoubleVector( char* cstr )
{
  std::stringstream ss( cstr );
  double d;
  std::vector< double > ret;
  while ( ss >> d )
    {
    ret.push_back( d );
    }
  return ret;
}


static std::string DoubleToString( double d )
{
  std::stringstream ss;
  ss << d;
  return ss.str();
}


#endif
