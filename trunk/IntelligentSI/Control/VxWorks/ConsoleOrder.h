/***********************************************************
 * FileName      : Console.h
 * Created       : 2009/01/
 * LastModified  :
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board console
  ***********************************************************/
#ifndef CONSOLE_ORDER__H
#define CONSOLE_ORDER__H

#include "Common.h"

#include <iostream>
#include <string>
#include <stdio.h>


/// Key board console
template <typename T>
class ConsoleOrder{
private:
  /// Order string
  std::string OrderStr;

  /// Help message string
  std::string HelpStr;

  /// Event
  T E;

public:
  /// A constructor
  ConsoleOrder(){

  }

  /// A destructor
  ~ConsoleOrder(){}

  /// Set order
  /// \param e Event
  /// \param od Order
  /// \param osize Order string size
  /// \param h Help message
  void SetOrder( T e, char od[], int osize, char h[]){
    E = e;
    OrderStr = od;
    HelpStr += " * ";
    HelpStr += od;
    HelpStr.append( "          ", 11 - osize );
    HelpStr += "- ";
    HelpStr += h;
    HelpStr += "\n";
  }

  /// Get order string
  /// \return Order
  const std::string Order(){
    return OrderStr;
  }

  /// Get help message string
  /// \return Help message
  const std::string Help(){
    return HelpStr;
  }

  /// Get Event
  /// \return Event
  const T Event(){
    return E;
  }

};

#endif // CONSOLE_ORDER__H

