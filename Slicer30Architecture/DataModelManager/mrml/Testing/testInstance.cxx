#include "mrmlObject.h"

int testInstance(int , char *[])
{
  // Declare a pointer
  mrml::Object::Pointer *o;
  o->Print(std::cout);

  return 0;
}
