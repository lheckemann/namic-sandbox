#ifndef VTK_WRITER_H_
#define VTK_WRITER_H_

#include <map>
#include <string>
#include <vector>
#include "linalg.h"


// Class that allows to write a bunch of fibers to a .vtk file
class VtkWriter {
 public:
  VtkWriter() { }
  virtual ~VtkWriter() { }

  // Writes the fibers to the VTK file called file_name.
  bool Write(const std::string& file_name,
             std::vector<vec_t> points,
             std::vector<std::vector<unsigned int> > lines,
             std::map<std::string, std::vector<std::vector<double> > > fields);

 protected:
  // Writes a point to the output stream (it first transforms it if
  // necessary). Also this function makes sure that the output VTK file is well
  // formatted.
  void WritePoint(const vec_t& point, std::ofstream& output, int& counter);
};

#endif  // VTK_WRITER_H_
