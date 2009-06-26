/******************************************************************************
 * FileName      : InterfaceData.h
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA
 * Aim           : Formanaging interfaces
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#ifndef INTERFACE_DATA_BASE__H
#define INTERFACE_DATA_BASE__H

#include <vector>
#include "Common.h"
//#include "InterfaceBase.h"

#define DATA_TYPE_NAME_SIZE 12

/// For interface dataes
class InterfaceDataBase {
private:
  /// Data type name
  struct DataTypeInfo{
    int DataSize;
    char DataName[DATA_TYPE_NAME_SIZE];
  };

  /// Data type name
  std::vector<DataTypeInfo> DataInfo;

public:

  /// A constructor
  InterfaceDataBase(){
    this->Init();
  }

  /// A destructor
  ~InterfaceDataBase(){
    DataInfo.clear();
  }

  /// Initialize
  void Init(){
    DataInfo.clear();
  }

  /// Set data name
  /// \param name Data name
  /// \strSize name size
  void SetDataInfo(char name[], int strSize, int dataSize ){
    DataTypeInfo temp;
    memset(&temp, 0, sizeof(temp) );

    if(strSize <=DATA_TYPE_NAME_SIZE && strSize>0){
      memcpy( &temp.DataName, name, strSize );
      temp.DataSize = dataSize;

      if(GetDataType( name ) == -1)
        DataInfo.push_back(temp);
    }

  }

  /// Get Data type
  /// \param name Data name
  /// \return Data type
  int GetDataType(char name[]){
    std::vector<DataTypeInfo>::iterator itr = DataInfo.begin();
    int size = (int)DataInfo.size();
    for(int i=0; i<size; i++, itr++){
      if( strcmp( itr->DataName, name)==0)
        return i;
    }
    return -1;
  }

  /// Get data name
  /// \param datatype Data type
  /// \return Data name pointer (const)
  const char* GetDataName(int datatype){
    std::vector<DataTypeInfo>::iterator itr = DataInfo.begin();
    int size = (int)DataInfo.size();

    if( datatype < size )
      return (itr+datatype)->DataName;
    else
      return NULL;
  }

  int GetDataTypeNum(){
    return (int)DataInfo.size();
  }

  int GetDataSize(char name[]){
    std::vector<DataTypeInfo>::iterator itr = DataInfo.begin();
    return itr[ GetDataType(name) ].DataSize;
  }

};


#endif // INTERFACE_DATA_BASE__H


