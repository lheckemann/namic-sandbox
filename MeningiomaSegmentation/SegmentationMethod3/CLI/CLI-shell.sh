#!/usr/bin/tclsh

#if {$argv == "--xml"} {

  puts "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
  puts "<executable>"
    puts "<category> Meningioma Segmentation Module</category>"
    puts "<title> Meningioma Segmentation </title>"
    puts "<description> Use global/local statistics based active contour method to segment meningioma image. </description>"
    puts "<version> 0.1</version>"
    puts "<documentation-url></documentation-url>"
    puts "<license></license>"
    puts "<contributor> Yi Gao, Shawn Lankton, Andriy Fedorov, Allen Tannenbaum </contributor>"
    puts "<acknowledgements> This work is part of the National Alliance for Medical Image Computing (NAMIC), funded by the National Institutes of Health through the NIH Roadmap for Medical Research, Grant U54 EB005149. </acknowledgements>"
    puts "<parameters>"

      puts "<label>Input/Output</label>"
      puts "<description>Input/output parameters</description>"

      puts "<image>"
        puts "<name>InputImage</name>"
        puts "<label>Input Image</label>"
        puts "<channel>input</channel>"
        puts "<index>0</index>"
        puts "<default>None</default>"
        puts "<description>The Input Image</description>"
      puts "</image>"

      puts "<image>"
        puts "<name>OutputImage</name>"
        puts "<label>Output Image</label>"
        puts "<channel>output</channel>"
        puts "<index>1</index>"
        puts "<default>None</default>"
        puts "<description>The Output Image</description>"
      puts "</image>"

      puts "<integer>"
        puts "<name>numOfIteration</name>"
        puts "<label>Number Of Iterations</label>"
        puts "<channel>input</channel>"
        puts "<index>2</index>"
        puts "<default>1000</default>"
        puts "<description>Number Of Iterations</description>"
      puts "</integer>"

      puts "<double>"
        puts "<name>curvatureWeight</name>"
        puts "<label>Smoothness</label>"
        puts "<channel>input</channel>"
        puts "<index>3</index>"
        puts "<default>0.5</default>"
      puts "<description>Smoothness factor[0-1]</description>"
      puts "</double>"

      puts "<integer>"
        puts "<name>cx</name>"
        puts "<label>cx</label>"
        puts "<channel>input</channel>"
        puts "<index>4</index>"
        puts "<default>100</default>"
        puts "<description>x-coord of a point in object</description>"
      puts "</integer>"

      puts "<integer>"
        puts "<name>cy</name>"
        puts "<label>cy</label>"
        puts "<channel>input</channel>"
        puts "<index>5</index>"
        puts "<default>100</default>"
        puts "<description>y-coord of a point in object</description>"
      puts "</integer>"

      puts "<integer>"
        puts "<name>cz</name>"
        puts "<label>cz</label>"
        puts "<channel>input</channel>"
        puts "<index>6</index>"
        puts "<default>10</default>"
        puts "<description>z-coord of a point in object</description>"
      puts "</integer>"
  
      puts "<integer>"
        puts "<name>globalSwitch</name>"
        puts "<label>Use global statistics?</label>"
        puts "<channel>input</channel>"
        puts "<index>7</index>"
        puts "<default>1</default>"
      puts "<description>1: Use global statistics, 0: Use local statistics.</description>"
      puts "</integer>"

  puts "</parameters>"
  puts "</executable>"
  exit 0
#}
foreach {a b c d e f g h} $argv {}
#exec /home/gaoyi/usr/work/NAMICSandBox/MeningiomaSegmentation/SegmentationMethod2/test/r/meningiomaSegmentor $a $b $c $d $e $f $g $h
exec /u4/home/gth818n/usr/work/NamicSandBox/MeningiomaSegmentation/SegmentationMethod3/test/r/meningiomaSegmentor $a $b $c $d $e $f $g $h