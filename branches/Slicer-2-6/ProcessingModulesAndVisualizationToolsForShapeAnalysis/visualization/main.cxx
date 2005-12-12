#pragma warning(disable:4786)
#pragma warning(disable:4503)
#pragma warning(disable:4244)

#include "gui.h"

int main(int argc, char**argv)
{
  gui m_gui ;
  
  m_gui.make_window () ;
  m_gui.show ( argc, argv ) ;
  
  return Fl::run () ;
}

