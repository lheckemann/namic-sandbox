#!/usr/bin/tclsh

# This example MATLAB module shows how to connect Slicer3 to MATLAB
# scripts using Slicer3's command-line interface. Two lines below must
# be modified (search for 'EDIT:'). This script currently works only
# in Linux.
#
# Contributed by John Melonakos, jmelonak@ece.gatech.edu, and Steve Pieper (2008).
#

if {$argv == "--xml"} {

  puts "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
  puts "<executable>"
    puts "<category> MATLAB Modules</category>"
    puts "<title> Geodesic Tractography Segmentation Module </title>"
    puts "<description>This module performs Geodesic Tractography Segmentation (GTS).  GTS is a two-part algorithm.  In the first part, an optimal \"anchor tract\" is found that connects two regions-of-interest, as defined by the ROI labelmap and the seed and target point values.  In the second part, the \"anchor tract\" is used to initialize a region-growing volumetric segmentation to capture the full fiber bundle.  This module works best for tubular fiber bundles and is not well suited for planar sheet fiber structures.</description>"
    puts "<version> 0.1</version>"
    puts "<documentation-url></documentation-url>"
    puts "<license></license>"
    puts "<contributor> John Melonakos </contributor>"
    puts "<acknowledgements> This work is part of the National Alliance for Medical Image Computing (NAMIC), funded by the National Institutes of Health through the NIH Roadmap for Medical Research, Grant U54 EB005149. </acknowledgements>"
   puts "<parameters>"
      puts "<label>Input/Output</label>"
      puts "<description>Input/output parameters</description>"
      puts "<image type=\"diffusion-weighted\">"
        puts "<name>InputVolume0</name>"
        puts "<label>DWI Data</label>"
        puts "<channel>input</channel>"
        puts "<index>0</index>"
        puts "<default>None</default>"
        puts "<description>DWI Data</description>"
      puts "</image>"
      puts "<image type=\"tensor\">"
        puts "<name>InputVolume1</name>"
        puts "<label>Tensor Data</label>"
        puts "<channel>input</channel>"
        puts "<index>1</index>"
        puts "<default>None</default>"
        puts "<description>Tensor Data</description>"
      puts "</image>"
      puts "<image>"
        puts "<name>InputVolume2</name>"
        puts "<label>White Matter Mask</label>"
        puts "<channel>input</channel>"
        puts "<index>2</index>"
        puts "<default>None</default>"
        puts "<description>White Matter Mask</description>"
      puts "</image>"
      puts "<image>"
        puts "<name>InputVolume3</name>"
        puts "<label>ROI</label>"
        puts "<channel>input</channel>"
        puts "<index>3</index>"
        puts "<default>None</default>"
        puts "<description>ROI</description>"
      puts "</image>"
      puts "<integer>"
        puts "<name>param0</name>"
        puts "<label>Seed Region Label</label>"
        puts "<channel>input</channel>"
        puts "<index>4</index>"
        puts "<default>2026</default>"
        puts "<description>Seed Region Label</description>"
      puts "</integer>"
      puts "<integer>"
        puts "<name>param1</name>"
        puts "<label>Target Region Label</label>"
        puts "<channel>input</channel>"
        puts "<index>5</index>"
        puts "<default>54</default>"
        puts "<description>Target Region Label</description>"
      puts "</integer>"
      puts "<geometry type=\"model\" fileExtensions=\".vtk\">"
        puts "<name>OutputVolume0</name>"
        puts "<label>Anchor Tract</label>"
        puts "<channel>output</channel>"
        puts "<index>6</index>"
        puts "<default>None</default>"
        puts "<description>Anchor Tract</description>"
      puts "</geometry>"
      puts "<image>"
        puts "<name>OutputVolume1</name>"
        puts "<label>Fiber Bundle</label>"
        puts "<channel>output</channel>"
        puts "<index>7</index>"
        puts "<default>None</default>"
        puts "<description>Fiber Bundle</description>"
      puts "</image>"
  puts "</parameters>"
  puts "</executable>"
  exit 0
}
foreach {a b c d e f g h} $argv {}

# EDIT: MAKE SURE TO EDIT THIS PATH TO POINT TO THE MATLAB EXECUTABLE, where 'matlab' is located!
set fp [open "|/usr/local/bin/matlab -nodesktop -nosplash" "w"]
# EDIT: MAKE SURE TO EDIT THIS PATH TO POINT TO THE MODULE's MATLAB DIRECTORY, where 'MATLABSlicerExampleModule.m' is located!
puts $fp "addpath('./MATLAB/');"

puts $fp "GeodesicTractographySegmentation('$a','$b','$c','$d','$e','$f','$g','$h');"
close $fp

# Other options that may work:
# exec /bin/sh /usr/local/bin/matlab -nodesktop -nosplash -r \"addpath('./MATLAB/')\;MATLABSlicerModule('$a','$b','$c','$d')\;\"
# exec matlab -nodesktop -nosplash -r fprintf\('%s,%s,%s,%s\n',$a,$b,$c,$d\)\;exit\;
