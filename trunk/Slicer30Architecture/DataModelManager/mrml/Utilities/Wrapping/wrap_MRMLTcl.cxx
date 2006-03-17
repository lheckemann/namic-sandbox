#define MRML_WRAP_PACKAGE "MRMLTcl"
#define MRML_TCL_WRAP
#ifdef CABLE_CONFIGURATION
namespace _cable_
{
  const char* const package = MRML_WRAP_PACKAGE;
  const char* const groups[] =
  {
    "mrmlObject",
  };
}
#endif
