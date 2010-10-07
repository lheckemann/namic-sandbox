#ifndef VTK_READER_H_
#define VTK_READER_H_

#include <string>
#include <vector>
#include "../ukf/linalg.h"

// Class that allows to read a .vtk file.
class VtkReader {
 public:
  VtkReader() { }
  virtual ~VtkReader() { }

  bool Read(const std::string& file_name);
  void Print() const;

  const std::vector<vec_t>& points() const {
    return _points;
  }
  const std::vector<std::vector<unsigned int> >& lines() const {
    return _lines;
  }
  const std::vector<std::vector<double> >& states() const {
    return _states;
  }

 protected:
  std::vector<vec_t> _points;
  std::vector<std::vector<unsigned int> > _lines;
  std::vector<std::vector<double> > _states;
};

#endif  // VTK_READER_H_
