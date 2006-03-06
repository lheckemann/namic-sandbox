#include "mrmlObject.h"

int testInstance(int , char *[])
{
  // Declare a pointer
  mrml::Object::Pointer o;
  // code should seg fault:
  //o->Print();

  mrml::Object::Pointer oo = mrml::Object::New();
  oo->Print();

  return 0;
}
