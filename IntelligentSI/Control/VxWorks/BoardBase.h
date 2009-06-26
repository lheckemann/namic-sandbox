/******************************************************************************
 * FileName      : BoardBase.h
 * Created       : 2007/08/23
 * LastModified  : 2007/10/
 * Author        : Hiroaki KOZUKA
 * Aim           : driver class for a motor control
 *****************************************************************************/

#ifndef BOARD_BASE__H
#define BOARD_BASE__H

#include <iostream>
#include "Common.h"
#include "occo.h"

/// Board driver base class
class BoardBase : public occo{
public:
  /// A constructor
  BoardBase();

  /// A destructor
  virtual ~BoardBase();

  /// Board type
  enum BoardType{
    TYPE_CNT = 0,  ///< Counter board
    TYPE_I,        ///< I board
    TYPE_O,        ///< O board
    TYPE_IO,       ///< IO board
    TYPE_DA,       ///< DA board
    TYPE_AD,       ///< AD board
    BOARD_TYPE_NUM ///< Number of board type
  };

  /// Write data to board
  /// \param unitNum Board unit number
  virtual void Write(int unitNum);

  /// Write data to board
  /// \param unitNum Board unit number
  /// \param type BoardType
  virtual void Write(int unitNum,  BoardType type);

  /// Read data from board
  /// \param unitNum Board unit number
  virtual void Read(int unitNum);

  /// Read data from board
  /// \param unitNum Board unit number
  /// \param type BoardType
  virtual void Read(int unitNum, BoardType type);

};

#endif //BOARD_BASE__H


