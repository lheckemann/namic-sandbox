/***************************************************************************
 * FileName      : DABoard.h
 * Created       : 2008/12/35
 * LastModified  :
 * Author        : Hiroaki Kozuka
 * Aim           : DA Driver for a Robot Control simulator
 ***************************************************************************/

#ifndef DABoard__H
#define DABoard__H

#include <iostream>

#include "Common.h"
#include "BoardBase.h"

class BoardBaseSim;

/// D/A board driver
class DABoard : public BoardBase{
private:
  /// Number of board ch
  static const int CH_NUM = 8;

  /// Number of board
  static const int board_num = 1;

  /// Voltage data
  double Volt[board_num][CH_NUM];

  ///
  BoardBaseSim* BBSim;

public:
  /// A constrcutor
  DABoard(BoardBaseSim* ptr);

  /// A destrcutor
  ~DABoard();

  /// Set voltage
  /// \param unitNum Board unit number
  /// \param ch Board ch
  /// \param data Voltage
  void Set(int unitNum, int ch, double data);

  /// Write voltage to board
  /// \param unitNum Board unit number
  /// \param type Board type
  void Write(int unitNum, BoardType type);

  /// Get number of board ch
  /// \return Number of board ch
  int CHNum();

  /// Get number of board unit
  /// \return Number of board unit
  int BoardNum();

  /// Get voltage data pointer
  /// \param unitNum Board unit number
  /// \param ch Board ch
  /// \return Data pointer
  double* GetDataPtr(int unitNum, int ch);

};


#endif // DABoard__H

