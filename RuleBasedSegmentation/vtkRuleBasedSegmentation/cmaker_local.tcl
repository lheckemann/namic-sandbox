# Initialize values
puts "Hello"
set SLICER_MODULE_ARG "-DVTKITK_SOURCE_DIR:PATH=$SLICER_HOME/Modules/vtkITK"
lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_DIR:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)"

if {[file exists $SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/libvtkITK.so] == 1} {
    lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/libvtkITK.so"
    lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/libvtkITKTCL.so" 
} else {
    if {$tcl_platform(os) == "Darwin"} {
        lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/vtkITK.dylib"
        lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/vtkITKTCL.dylib"  
    } else {
        lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/debug/vtkITK.lib"
        lappend SLICER_MODULE_ARG "-DVTKITK_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/debug/vtkITKTCL.lib" 
        puts "$SLICER_HOME/Modules/vtkITK/builds/$env(BUILD)/bin/debug/vtkITKTCL.lib"
    }
}

