############################################################
#
#  Slicer Python module to run "SynchroGrab - 3D Ultrasound"
#
#  Author: Jan Gumprecht
#  Date:   Dec 2008
#
#  Location: 
#       SLICER_INSTALL_DIR/Slicer3-build/lib/Slicer/Plugins/
#  
############################################################

XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category></category>
  <title>SynchroGrab</title>
  <description>
    Run SynchgroGrab from within Slicer
  </description>
  <version>1.0</version>
  <documentation-url></documentation-url>
  <license></license>
  <contributor>Jan Gumprecht</contributor>

  <parameters>
    <!-- Select SynchroGrab Executable -->
    <label>Executable Selection</label>
    <description>Select executable of SynchroGrab to run</description>
    <file>
      <name>synchroGrab</name>
      <longflag>synchroGrab</longflag>
      <description>SynchroGrab executable</description>
      <label>SynchroGrab</label>
      <default>/home/ultrasound/workspace/SynchroGrabJGumprecht/Build/bin/SynchroGrab</default>
    </file>
  </parameters>

  <parameters>
    <label>General Parameters</label>
    <description>Parameters for SynchroGrab</description>

    <!-- Select Calibrations File -->
    <file>
      <name>calibrationFile</name>
      <longflag>calibrationFile</longflag>
      <description>Calibration File needed to start Synchrograb</description>
      <label>Calibration File</label>
      <default>/home/ultrasound/workspace/SynchroGrabJGumprecht/Build/bin/CalibrationFile.txt</default>
    </file>

    <!-- Enable volume reconstruction -->
    <boolean>
     <name>reconstructVolume</name>
     <longflag>reconstructVolume</longflag>
     <label>Volume Reconstruction</label>
     <description>Do Volume Reconstruction?</description>
     <default>true</default>
    </boolean>

    <!-- OpenIGTLink server -->
    <string>
      <name>oigtlServer</name>
      <longflag>oigtlServer</longflag>
      <description>IP-Address of OpenIGTLink Server</description>
      <label>OpenIGTLink Server IP-Address</label>
      <default>127.0.0.1</default>
    </string>

    <!-- OpenIGTLink port of server -->
    <string>
      <name>oigtlPort</name>
      <longflag>oigtlPort</longflag>
      <description>OpenIGTLink port of OpenIGTLink Server</description>
      <label>OpenIGTLink Port of Server</label>
      <default>18944</default>
    </string>

    <!-- Number of Frames -->
    <string>
      <name>nbFrames</name>
      <longflag>nbFrames</longflag>
      <description>Number of frames to grab</description>
      <label>Number of frames to grab</label>
      <default>50</default>
    </string>

  </parameters> 

  <parameters> 
    <label>Video Device Parameters</label>
    <description>Parameters for the Video Device</description>

    <!-- Framerate -->
    <string>
      <name>fps</name>
      <longflag>fps</longflag>
      <description>Frames per second</description>
      <label>Frame rate of the video device</label>
      <default>30</default>
    </string>

    <!-- Video Source -->
    <string>
      <name>videoSource</name>
      <longflag>videoSource</longflag>
      <description>Video device for frame grabbing</description>
      <label>Video Device</label>
      <default>/dev/video0</default>
    </string>

    <!-- Video Channel -->
    <string>
      <name>videoChannel</name>
      <longflag>videoChannel</longflag>
      <description>Video channel of frame grabber card</description>
      <label>Video Channel</label>
      <default>3</default>
    </string>

    <!-- Video Mode -->
    <string-enumeration>
      <name>videoMode</name>
      <longflag>videoMode</longflag>
      <description>Select Videomode for video device</description>
      <label>Video mode</label>
      <default>NTSC</default>
      <element>NTSC</element>  
      <element>PAL</element>
    </string-enumeration>

  </parameters> 

  <parameters> 
    <label>Ultrasound Parameters</label>
    <description>Parameters of the Ultrasound Device</description>

    <!-- Ultrasound scan depth -->
    <string>
      <name>scanDepth</name>
      <longflag>scanDepth</longflag>
      <description>Scan depth of ultrasound device</description>
      <label>Scan depth [mm]</label>
      <default>70</default>
    </string>

  </parameters> 

</executable>
"""


def Execute(synchroGrab="",\
            calibrationFile="",\
            reconstructVolume=True,\
            oigtlServer="127.0.0.1",\
            oigtlPort="18944",\
            videoSource="/dev/video0",\
            videoChannel="3",\
            videoMode="NTSC",\
            nbFrames="50",\
            fps="30",\
            scanDepth="70"):

    commandline = "xterm -e "

    #Generate commandline to execute
    commandline = commandline\
        + synchroGrab \
        + " -c "   + calibrationFile\
        + " -os "  + oigtlServer\
        + " -op "  + oigtlPort\
        + " -vs "  + videoSource\
        + " -vsc " + videoChannel\
        + " -vm "  + videoMode\
        + " -n "   + nbFrames\
        + " -fps " + fps\
        + " -sd "  + scanDepth
    
    #Enable volume reconstruction if selected
    if reconstructVolume==True:
        commandline = commandline + " --reconstruct-volume"      
    
    #Start Synchrograb         
    import os
    print os.system(commandline)

    return


