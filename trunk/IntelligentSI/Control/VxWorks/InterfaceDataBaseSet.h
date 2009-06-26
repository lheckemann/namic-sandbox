/******************************************************************************
 * FileName      : InterfaceData.h
 * Created       : 2009/04/16
 * LastModified  : 2009/
 * Author        : Hiroaki KOZUKA
 * Aim           : Formanaging interfaces
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/

#ifndef INTERFACE_DATA_BASE_SET__H
#define INTERFACE_DATA_BASE_SET__H

#include "Common.h"
#include "InterfaceDataBase.h"

#define DATA_TYPE_NAME_SIZE 12

/// For interface dataes
class InterfaceDataBaseSet {
private:
  InterfaceDataBase S_IFDB;
  InterfaceDataBase R_IFDB;

  void SetS_Data(){
    S_IFDB.SetDataInfo("POSITION", sizeof("POSITION"), sizeof(Coord_6DoF_N) );
    S_IFDB.SetDataInfo("C_POSITION", sizeof("C_POSITION"), sizeof(Coord_6DoF_N) );
    S_IFDB.SetDataInfo("TRANSFORM", sizeof("TRANSFORM"), sizeof(TF_Matrix_N) );
    S_IFDB.SetDataInfo("FORCE", sizeof("FORCE"), sizeof(Coord_6DoF_N) );
    S_IFDB.SetDataInfo("EVENT", sizeof("EVENT"), sizeof(EVENT) );
    S_IFDB.SetDataInfo("CUR_ANGLES", sizeof("CUR_ANGLES"), 4*3 );
    S_IFDB.SetDataInfo("DEST_ANGLES", sizeof("DEST_ANGLES"), 4*3 );
    S_IFDB.SetDataInfo("TIMESTAMP", sizeof("TIMESTAMP"), sizeof(unsigned long long)*2 );
    S_IFDB.SetDataInfo("TIME_TRNSCT", sizeof("TIME_TRNSCT"), sizeof(float) );
  }

  void SetR_Data(){
    R_IFDB.SetDataInfo("POSITION", sizeof("POSITION"), sizeof(Coord_6DoF_N) );
    R_IFDB.SetDataInfo("TRANSFORM", sizeof("TRANSFORM"), sizeof(TF_Matrix_N) );
    R_IFDB.SetDataInfo("FORCE", sizeof("FORCE"), sizeof(Coord_6DoF_N) );
    R_IFDB.SetDataInfo("EVENT", sizeof("EVENT"), sizeof(EVENT) );
    R_IFDB.SetDataInfo("TIMESTAMP", sizeof("TIMESTAMP"), sizeof(unsigned long long)*2 );

  }

public:
  InterfaceDataBaseSet(){
    SetS_Data();
    SetR_Data();
  }

  ~InterfaceDataBaseSet(){
  }

  const char* GetSDName(int datatype){
    return S_IFDB.GetDataName(datatype);
  }

  const char* GetRDName(int datatype){
    return R_IFDB.GetDataName(datatype);
  }

  int GetSDType(char name[]){
    return S_IFDB.GetDataType(name);
  }

  int GetRDType(char name[]){
    return R_IFDB.GetDataType(name);
  }

  int GetSDTypeNum(){
    return (int)S_IFDB.GetDataTypeNum();
  }

  int GetRDTypeNum(){
    return (int)R_IFDB.GetDataTypeNum();
  }

  int GetSDSize(char name[]){
    return (int)S_IFDB.GetDataSize(name);
  }

  int GetRDSize(char name[]){
    return (int)R_IFDB.GetDataSize(name);
  }

};

#endif // INTERFACE_DATA_BASE_SET__H
