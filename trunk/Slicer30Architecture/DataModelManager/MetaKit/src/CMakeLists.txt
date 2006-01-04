
SET( MetaKit_Sources 
  column.cpp
  custom.cpp
  derived.cpp
  field.cpp
  fileio.cpp
  format.cpp
  handler.cpp
  persist.cpp
  remap.cpp
  std.cpp
  store.cpp
  string.cpp
  table.cpp
  univ.cpp
  view.cpp
  viewx.cpp
)

ADD_LIBRARY(MetaKit ${MetaKit_Sources})

INSTALL_TARGETS(/lib/ metakit )
INSTALL_FILES(/include/metakit "(\\.h)$")

SET(MetaKit_LIBRARY metakit)

