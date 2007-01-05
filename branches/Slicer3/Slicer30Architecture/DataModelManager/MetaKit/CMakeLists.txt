PROJECT(MetaKit)

SUBDIRS( src include )

INCLUDE_DIRECTORIES( 
  ${MetaKit_SOURCE_DIR}/src 
  ${MetaKit_SOURCE_DIR}/include 
  )

INSTALL_FILES(/include/metakit "(\\.h)$")
INSTALL_FILES(/include/metakit "(\\.inl)$")

