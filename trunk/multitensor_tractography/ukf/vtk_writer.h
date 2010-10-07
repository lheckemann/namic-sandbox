#ifndef VTK_WRITER_H_
#define VTK_WRITER_H_

#include <string>
#include <vector>
#include "tractography.h"


class SignalData;

// Class that allows to write a bunch of fibers to a .vtk file
class VtkWriter {
 public:
  VtkWriter(const SignalData *signal_data)
      : _signal_data(signal_data), _transform_position(true) { }
  virtual ~VtkWriter() { }

  // Writes the fibers to the VTK file called file_name.
  bool Write(const std::string& file_name,
             const std::vector<Tractography::Fiber>& fibers);

  void set_transform_position(bool transform_position) {
    _transform_position = transform_position;
  }

 protected:
  const SignalData *_signal_data;

  bool _transform_position;
};

#endif  // VTK_WRITER_H_
