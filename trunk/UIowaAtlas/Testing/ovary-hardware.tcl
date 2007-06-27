# 
# Sample rendering of 3D microscopy datasets
# with interaction
#
# C. Lisle
# SGI
#
# This script reads a stack of raw binary files that is organized as 
# multiple slices of raw microscopy data, instantiates a volume 
# class and allows the user to render the volume and move around it.
# 

package require vtk
package require vtkinteraction


# globals 

set scaleFactor 4


# Create the standard renderer, render window
# and interactor
vtkRenderer ren1
vtkRenderWindow renWin
    renWin AddRenderer ren1
    renWin StereoCapableWindowOn
    renWin SetStereoTypeToCrystalEyes
vtkRenderWindowInteractor iren
    iren SetRenderWindow renWin


# find out the size of the image files
vtkTIFFReader sizetest
  sizetest SetFilePattern "./ovary-volume/ovary_cell_death.00.tif"
  sizetest SetFileDimensionality 2
   #sizetest SetDataScalarTypeToUnsignedChar
  sizetest Update
  

#puts "dataset dimensions: [[sizetest GetOutput] GetBounds]"  

# *** TODO: modify the extents below to read from the bounds of the first
# file so that we only have to know how many slices to do. 

# Create the reader for the data
vtkTIFFReader reader
    reader SetFilePattern "./ovary-volume/ovary_cell_death.%02d.tif"
    reader SetFileDimensionality 2
    reader SetDataScalarTypeToUnsignedChar
    reader SetDataExtent 0 799 0 499 0 79
    reader Update

puts "header size:"
puts [reader GetHeaderSize]

puts "dataset dimensions: [[reader GetOutput] GetBounds]" 

#create bounding box
vtkOutlineFilter outline
  outline SetInput [reader GetOutput]
vtkPolyDataMapper outmap
  outmap SetInput [outline GetOutput]
vtkActor outact
  outact SetMapper outmap
  outact SetScale 1 1 $scaleFactor
  
#create the contours
vtkContourFilter cont
    cont SetInput [ reader GetOutput]
    #cont Update

vtkPolyDataMapper contmap
    contmap SetInput [ cont GetOutput]
vtkActor contact
    contact SetMapper contmap
    
# scale the intensity of the volume voxels up

vtkImageMathematics imath
  imath SetInput1 [reader GetOutput]
  imath SetOperationToMultiplyByK
  imath SetConstantK 1
 


# Create transfer mapping scalar value to opacity
vtkPiecewiseFunction opacityTransferFunction
    opacityTransferFunction AddPoint   0   0.00 
    opacityTransferFunction AddPoint  60   0.000
    opacityTransferFunction AddPoint  100   0.01
    opacityTransferFunction AddPoint  120   0.04
    opacityTransferFunction AddPoint  255  0.2

# Create transfer mapping scalar value to color
vtkColorTransferFunction colorTransferFunction
    colorTransferFunction AddRGBPoint      0.0 0.0 0.0 0.0
    colorTransferFunction AddRGBPoint     60.0 1.0 0.0 0.0
    colorTransferFunction AddRGBPoint    70.0 0.8 0.4 0.0
    colorTransferFunction AddRGBPoint    140.0 0.8 0.4 0.0
    colorTransferFunction AddRGBPoint    200 1 1 1
    colorTransferFunction AddRGBPoint    255.0 1.0 1.0 1.0

# The property describes how the data will look
vtkVolumeProperty volumeProperty
    volumeProperty SetColor colorTransferFunction
    volumeProperty SetScalarOpacity opacityTransferFunction
    volumeProperty ShadeOn
    volumeProperty SetInterpolationTypeToLinear

# uncomment either of the sets of lines below, depending on whether 3D acceleration via
# texture mapping is available for this version of VTK and graphics hardware

# uncomment to use GPU acceleration 
vtkVolumeTextureMapper3D volumeMapper
    volumeMapper SetInput [reader GetOutput]
    
# uncomment to use a software rendering algorithm
#vtkVolumeRayCastCompositeFunction  compositeFunction
#vtkFixedPointVolumeRayCastMapper volumeMapper
    #volumeMapper SetVolumeRayCastFunction compositeFunction

    #volumeMapper SetInput [reader GetOutput]
    volumeMapper SetInput [reader GetOutput]
    

# The volume holds the mapper and the property and
# can be used to position/orient the volume
vtkVolume volume
    volume SetMapper volumeMapper
    volume SetProperty volumeProperty
    volume SetScale 1 1 $scaleFactor

ren1 AddVolume volume
ren1 AddActor outact
renWin SetSize 600 600
renWin SetPosition 100 100
renWin Render



proc TkCheckAbort {} {
  set foo [renWin GetEventPending]
  if {$foo != 0} {renWin SetAbortRender 1}
}
renWin AddObserver AbortCheckEvent {TkCheckAbort}

iren AddObserver UserEvent {wm deiconify .vtkInteract}
iren Initialize

wm withdraw .



