#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        PACS.tcl
# PROCEDURES:  
#   PACSInit
#   PACSBuildGUI
#   PACSEnter
#   PACSExit
#   PACSUpdateGUI
#   PACSCount
#   PACSShowFile
#   PACSBindingCallback
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module is an example for developers.  It shows how to add a module 
#  to the Slicer.  To find it when you run the Slicer, click on More->PACS.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
# 
#  int PACS(count) counts the button presses for the demo 
#  list PACS(eventManager)  list of event bindings used by this module
#  widget PACS(textBox)  the text box widget
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC PACSInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSInit {} {
    global PACS Module Volume Model

    set m PACS
    
    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module allows access to PACS."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Vidina, Monagas Machin, CTM, vidina@ctm.ulpgc.es"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories), use lindex to grab the text string 0=Core, 1=Beta, 2=Experimental, 3=Example
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) [lindex $Module(categories) 3]

    # Define Tabs
    #------------------------------------
 
    set Module($m,row1List) "Help Pacs Storage Query Retrieve"
    set Module($m,row1Name) "{Help} {Pacs} {Storage} {Query} {Retrieve}"
    set Module($m,row1,tab) Pacs



    # Define Procedures
    #------------------------------------
  
    set Module($m,procGUI) PACSBuildGUI
    set Module($m,procEnter) PACSEnter
    set Module($m,procExit) PACSExit

    # Define Dependencies
    #------------------------------------
 
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.33 $} {$Date: 2004/03/15 20:49:38 $}]

    # Initialize module-level variables
    #------------------------------------
 
    set PACS(count) 0
    set PACS(Volume1) $Volume(idNone)
    set PACS(Model1)  $Model(idNone)
    set PACS(FileName)  ""

    set PACS(Hostname) "bmw.teleco.ulpgc.es"
    set PACS(Port) "1234"
    set PACS(AE) "dicom"
    set PACS(dcm_image) ""
    set PACS(AEDestination) "Client2"
    set PACS(PortDestination) "5678"
   
    set PACS(List) "Patient Study Series Image"
    set PACS(server) [lindex $PACS(List) 0]
    set PACS(key_query) "Patient"
    set PACS(Selection) 0

    # Event bindings! (see PACSEnter, PACSExit, tcl-shared/Events.tcl)
    set PACS(eventManager)  { \
        {all <Shift-1> {PACSBindingCallback Shift-1 %W %X %Y %x %y %t}} \
        {all <Shift-2> {PACSBindingCallback Shift-2 %W %X %Y %x %y %t}} \
        {all <Shift-3> {PACSBindingCallback Shift-3 %W %X %Y %x %y %t}} }
    
}

#-------------------------------------------------------------------------------
# .PROC PACSBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc PACSBuildGUI {} {

    global Gui PACS Module Volume Model
      
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    set help "
    The PACS module shows how to access to PACS from the Slicer. The source code is in slicer/program/tcl-modules/PACS.tcl.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Pacs:</B> This tab allows users access to Picture Archiving and Communication systems, through the Verification Service Class. It defines a service which verifies application level communication between peer DICOM AEs. This verification is accomplished on an established Association using the C-ECHO DIMSE-C service.
        <BR>A DICOM AE, supporting the Verification SOP Class SCU role, requests verification of communication to a remote DICOM AE. This request is performed using the C-ECHO request primitive. The remote DICOM AE, supporting the Verification SOP Class SCP role, issues an C-ECHO response primitive. Upon receipt of the C-ECHO confirmation, the SCU determines that verification is complete.
        <LI><B>Storage:</B> The Storage Service Class defines an application-level class-of-service which facilitates the simple transfer of images. It allows one DICOM AE to send images to another.
        <LI><B>Query:</B>The Find application implements an SCU for the Query Service Class and the Basic Worklist Management Service Class. findscu only supports query functionality using the C-FIND message. It sends query keys to an SCP and waits responses. The application can be used to test SCPs of the Query/Retrieve and Basic Worklist Management Service Classes.
        <LI><B>Retrieve:</B>The Move application implements both an SCU for the Query Retrieve Service Class and an SCP for the Storage Service Class. Move Service supports retrieve functionality using the C-MOVE message. It sends query keys to an SCP and awaits responses. It will accept associations for the purpose of receiving images sent as a result of the C-MOVE request. The application can be used to test SCPs of the Query/Retrieve Service Class. The movescu application can initiate the transfer of images to a third party or can retrieve images to itself. Note that the use of the term MOVE is a misnomer. The C-MOVE operation actually performs an image copy (no images will be deleted from the SCP).
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags PACS $help
    MainHelpBuildGUI PACS
    
    #-------------------------------------------
    # Pacs frame
    #-------------------------------------------
    set fPacs $Module(PACS,fPacs)
    set f $fPacs
    
    foreach frame "Top Bottom1" {
    frame $f.f$frame -bg $Gui(activeWorkspace) 
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Pacs->Top frame
    #-------------------------------------------
    set f $fPacs.fTop
    #AE, host, port
    eval {label $f.l1 -text "Host Name:" } $Gui(WLA)
    eval {entry $f.e1 -textvariable PACS(Hostname)} $Gui(WEA)
    grid $f.l1 $f.e1 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l2 -text "Port:" } $Gui(WLA)
    eval {entry $f.e2 -textvariable PACS(Port)} $Gui(WEA)
    grid $f.l2 $f.e2 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l3 -text "Application Entity (AE):" } $Gui(WLA)
    eval {entry $f.e3 -textvariable PACS(AE)} $Gui(WEA)
    grid $f.l3 $f.e3 -padx $Gui(pad) -pady $Gui(pad) -sticky e
   
    #----------------------------------------------------------------
    # Pacs->Bottom frame. Send verification of connectivity to PACS
    #----------------------------------------------------------------
    set f $fPacs.fBottom1
    DevAddButton $f.bConnect "Connect PACS..." "ConnectPACS"
    TooltipAdd $f.bConnect "Press this button to verify DICOM connectivity with the server (C-ECHO)."
    pack $f.bConnect -pady 20


    #-------------------------------------------
    # Storage frame
    #-------------------------------------------
    set fStorage $Module(PACS,fStorage)
    set f $fStorage
    
    foreach frame "Top Middle Bottom" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    set f $fStorage.fTop

    #AE, host, port
    eval {label $f.l1 -text "Host Name:" } $Gui(WLA)
    eval {entry $f.e1 -textvariable PACS(Hostname)} $Gui(WEA)
    grid $f.l1 $f.e1 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l2 -text "Port:" } $Gui(WLA)
    eval {entry $f.e2 -textvariable PACS(Port)} $Gui(WEA)
    grid $f.l2 $f.e2 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l3 -text "Application Entity (AE):" } $Gui(WLA)
    eval {entry $f.e3 -textvariable PACS(AE)} $Gui(WEA)
    grid $f.l3 $f.e3 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    set f $fStorage.fMiddle
    DevAddFileBrowse $f PACS dcm_image "DICOM File:" "" "" "c:\database" "Open" "Browse for DICOM File" 
   
    #-------------------------------------------
    # Pacs->Bottom frame
    #-------------------------------------------
    set f $fStorage.fBottom
    DevAddButton $f.bSend "Send DICOM File" "SendDICOMFile"
    TooltipAdd $f.bSend "Press this button to send DICOM file to the server (C-STORE)."
    pack $f.bSend -pady 20



    #-------------------------------------------
    # Query frame
    #-------------------------------------------
   
    set fQuery $Module(PACS,fQuery)
    set f $fQuery

    frame $f.fInicio -bg $Gui(activeWorkspace)
    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 500
    pack $f.fInicio $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    
    #-------------------------------------------
    # Query->Inicio frame
    #-------------------------------------------
    set f $fQuery.fInicio

    #AE, host, port
    eval {label $f.l1 -text "Host Name:" } $Gui(WLA)
    eval {entry $f.e1 -textvariable PACS(Hostname)} $Gui(WEA)
    grid $f.l1 $f.e1 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l2 -text "Port:" } $Gui(WLA)
    eval {entry $f.e2 -textvariable PACS(Port)} $Gui(WEA)
    grid $f.l2 $f.e2 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l3 -text "Application Entity (AE):" } $Gui(WLA)
    eval {entry $f.e3 -textvariable PACS(AE)} $Gui(WEA)
    grid $f.l3 $f.e3 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    #-------------------------------------------
    # Query->Bot frame
    #-------------------------------------------

    set f $fQuery.fBot

    foreach s $PACS(List) {
        frame $f.f$s -bg $Gui(activeWorkspace)
        place $f.f$s -in $f -relheight 1.0 -relwidth 1.0
        set PACS(f$s) $f.f$s
    }
    raise $PACS(f[lindex $PACS(List) 0])


    #-------------------------------------------
    # Query->Top frame
    #-------------------------------------------
    set f $fQuery.fTop

    frame $f.fActive -bg $Gui(backdrop)
    pack $f.fActive -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Query->Top->Active frame
    #-------------------------------------------
    set f $fQuery.fTop.fActive

    eval {label $f.lActive -text "Query Level: "} $Gui(BLA)
    eval {menubutton $f.mbActive \
        -text [lindex $PACS(List) 0] \
        -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left
    set PACS(mbActive) $f.mbActive

    #---------------------------------------------
    # Form the Active menu 
    #---------------------------------------------
    set m $PACS(mbActive).m
    foreach s $PACS(List) {
        $m add command -label $s \
            -command "PACSSetActive $s"

    }

    #-------------------------------------------
    # Query->Bot->Patient frame
    #-------------------------------------------
    set f $fQuery.fBot.fPatient

    frame $f.fData   -bg $Gui(activeWorkspace)
    frame $f.fSendQ -bg $Gui(activeWorkspace)
    pack $f.fData $f.fSendQ -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Query->Bot->Patient->Data frame
    #-------------------------------------------
    set f $fQuery.fBot.fPatient.fData
    
    set PACS(P_Name)    0
    set PACS(P_ID)      0
    set PACS(P_BData)   0
    set PACS(P_BTime)   0
    set PACS(P_Sex)     0
    set PACS(P_RelatedStu) 0
    set PACS(P_RelatedSer) 0
    set PACS(P_RelatedIns) 0

    set PACS(Pname)  ""
    set PACS(Pid)    ""
    set PACS(PBData) ""
    set PACS(PBTime) ""
    set PACS(PSex)   ""
    set PACS(PRelatedStu) ""
    set PACS(PRelatedSer) ""
    set PACS(PRelatedIns) ""

    eval {checkbutton $f.pname   -text "Name"          -variable PACS(P_Name)  -onvalue 1 -offvalue 0}  $Gui(WRA)
    eval {entry $f.pnamebox -textvariable PACS(Pname)} $Gui(WEA) 
    eval {checkbutton $f.pid     -text "Patient ID"            -variable PACS(P_ID)    -onvalue 1 -offvalue 0}  $Gui(WRA)
    eval {entry $f.pidbox -textvariable PACS(Pid)} $Gui(WEA) 
    eval {checkbutton $f.pbirth  -text "Birth Date"  -variable PACS(P_BData) -onvalue 1 -offvalue 0}  $Gui(WRA)
    eval {entry $f.pbirthbox -textvariable PACS(PBData)} $Gui(WEA) 
    eval {checkbutton $f.ptime   -text "Birth Time"  -variable PACS(P_BTime) -onvalue 1 -offvalue 0}  $Gui(WRA)  
    eval {entry $f.ptimebox -textvariable PACS(PBTime)} $Gui(WEA) 
    eval {checkbutton $f.psex    -text "Sex"         -variable PACS(P_Sex)   -onvalue 1 -offvalue 0}  $Gui(WRA)  
    eval {entry $f.psexbox -textvariable PACS(PSex)} $Gui(WEA)  
    eval {checkbutton $f.prelatedstu    -text "Nº of Related Studies"   -variable PACS(P_RelatedStu) -onvalue 1 -offvalue 0}  $Gui(WRA)  
    eval {entry $f.prelatedstubox -textvariable PACS(PRelatedStu)} $Gui(WEA)  
    eval {checkbutton $f.prelatedser    -text "Nº of Related Series"   -variable PACS(P_RelatedSer) -onvalue 1 -offvalue 0}  $Gui(WRA)  
    eval {entry $f.prelatedserbox -textvariable PACS(PRelatedSer)} $Gui(WEA)  
    eval {checkbutton $f.prelatedins    -text "Nº of Related Instances"   -variable PACS(P_RelatedIns) -onvalue 1 -offvalue 0}  $Gui(WRA)  
    eval {entry $f.prelatedinsbox -textvariable PACS(PRelatedIns)} $Gui(WEA)  


    grid $f.pname           -column 0 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.pid             -column 0 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.pbirth          -column 0 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.ptime           -column 0 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.psex            -column 0 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.prelatedstu     -column 0 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.prelatedser     -column 0 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.prelatedins     -column 0 -row 7 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"

    grid $f.pnamebox           -column 1 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.pidbox             -column 1 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.pbirthbox          -column 1 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.ptimebox           -column 1 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.psexbox            -column 1 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.prelatedstubox     -column 1 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.prelatedserbox     -column 1 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.prelatedinsbox     -column 1 -row 7 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"

    #-------------------------------------------
    # Query->Bot->Patient->SendQ frame
    #-------------------------------------------
    set f $fQuery.fBot.fPatient.fSendQ
    DevAddButton $f.bQuery "Query..." "FindDICOMFile"
    TooltipAdd $f.bQuery "Press this button to query Patient DICOM information to the server (C-FIND)."
    pack $f.bQuery -pady 20


    #-------------------------------------------
    # Query->Bot->Study frame
    #-------------------------------------------
    set f $fQuery.fBot.fStudy

    frame $f.fData   -bg $Gui(activeWorkspace)
    frame $f.fSendQ  -bg $Gui(activeWorkspace)
    pack  $f.fData $f.fSendQ -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Query->Bot->Study->Data frame
    #-------------------------------------------
    set f $fQuery.fBot.fStudy.fData

    set PACS(S_Data)        0
    set PACS(S_Time)        0
    set PACS(S_Accession)   0
    set PACS(S_Physician)   0
    set PACS(S_Description) 0
    set PACS(S_Age)         0
    set PACS(S_Size)        0
    set PACS(S_Weight)      0
    set PACS(S_UID)         0
    set PACS(S_ID)          0
    set PACS(S_RelatedSer)  0
    set PACS(S_RelatedIns)  0

    set PACS(SData)        ""
    set PACS(STime)        ""
    set PACS(SAccession)   ""
    set PACS(SPhysician)   ""
    set PACS(SDescription) ""
    set PACS(SAge)         ""
    set PACS(SSize)        ""
    set PACS(SWeight)      ""
    set PACS(SUID)         ""
    set PACS(SID)          ""
    set PACS(SRelatedSer)  ""
    set PACS(SRelatedIns)  ""

    
    eval {checkbutton $f.sdata   -text "Study Date"   -variable PACS(S_Data)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.sdatabox -textvariable PACS(SData)} $Gui(WEA) 
    eval {checkbutton $f.stime     -text "Study Time"   -variable PACS(S_Time)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.stimebox -textvariable PACS(STime)} $Gui(WEA) 
    eval {checkbutton $f.saccession  -text "Accession Num."  -variable PACS(S_Accession) -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.saccessionbox -textvariable PACS(SAccession)} $Gui(WEA) 
    eval {checkbutton $f.sphysician   -text "Physician Name" -variable PACS(S_Physician) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sphysicianbox -textvariable PACS(SPhysician)} $Gui(WEA) 
    eval {checkbutton $f.sdescription    -text "Study Description" -variable PACS(S_Description) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sdescriptionbox -textvariable PACS(SDescription)} $Gui(WEA)  
    eval {checkbutton $f.sage    -text "Patient's Age" -variable PACS(S_Age) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sagebox -textvariable PACS(SAge)} $Gui(WEA)  
    eval {checkbutton $f.ssize    -text "Patient's Size" -variable PACS(S_Size) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.ssizebox -textvariable PACS(SSize)} $Gui(WEA)  
    eval {checkbutton $f.sweight    -text "Patient's Weight" -variable PACS(S_Weight) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sweightbox -textvariable PACS(SWeight)} $Gui(WEA)  
    eval {checkbutton $f.suid    -text "Study Instance UID" -variable PACS(S_UID) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.suidbox -textvariable PACS(SUID)} $Gui(WEA)    
    eval {checkbutton $f.sid    -text "Study ID" -variable PACS(S_ID) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sidbox -textvariable PACS(SID)} $Gui(WEA) 
    eval {checkbutton $f.srelatedser    -text "Nº of Related Series" -variable PACS(S_RelatedSer) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.srelatedserbox -textvariable PACS(SRelatedSer)} $Gui(WEA) 
    eval {checkbutton $f.srelatedins    -text "Nº of Related Instances" -variable PACS(S_RelatedIns) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.srelatedinsbox -textvariable PACS(SRelatedIns)} $Gui(WEA) 

    grid $f.sdata        -column 0 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.stime        -column 0 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.saccession   -column 0 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sphysician   -column 0 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sdescription -column 0 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sage         -column 0 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.ssize        -column 0 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sweight      -column 0 -row 7 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.suid         -column 0 -row 8 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sid          -column 0 -row 9 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.srelatedser  -column 0 -row 10 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.srelatedins  -column 0 -row 11 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"


    grid $f.sdatabox         -column 1 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.stimebox         -column 1 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.saccessionbox    -column 1 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sphysicianbox    -column 1 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sdescriptionbox  -column 1 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sagebox          -column 1 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.ssizebox         -column 1 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sweightbox       -column 1 -row 7 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.suidbox          -column 1 -row 8 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sidbox           -column 1 -row 9 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.srelatedserbox   -column 1 -row 10 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.srelatedinsbox   -column 1 -row 11 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"

    #-------------------------------------------
    # Query->Bot->Study->SendQ frame
    #-------------------------------------------
    set f $fQuery.fBot.fStudy.fSendQ
    DevAddButton $f.bQuery "Query..." "FindDICOMFile"
    TooltipAdd $f.bQuery "Press this button to query Study DICOM information to the server (C-FIND)."
    pack $f.bQuery -pady 20


    #-------------------------------------------
    # Query->Bot->Series frame
    #-------------------------------------------
    set f $fQuery.fBot.fSeries

    frame $f.fData     -bg $Gui(activeWorkspace)
    frame $f.fSendQ    -bg $Gui(activeWorkspace) 
    pack  $f.fData $f.fSendQ -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Query->Bot->Series->Data frame
    #-------------------------------------------
    set f $fQuery.fBot.fSeries.fData

    set PACS(Se_Data)        0
    set PACS(Se_Time)        0
    set PACS(Se_Modality)    0
    set PACS(Se_Body)        0
    set PACS(Se_UID)         0
    set PACS(Se_Number)      0
    set PACS(Se_RelatedIns)  0

    set PACS(SeData)        ""
    set PACS(SeTime)        ""
    set PACS(SeModality)    ""
    set PACS(SeBody)        ""
    set PACS(SeUID)         ""
    set PACS(SeNumber)      ""
    set PACS(SeRelatedIns)  ""
    
    eval {checkbutton $f.sedata   -text "Series Date"   -variable PACS(Se_Data)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.sedatabox -textvariable PACS(SeData)} $Gui(WEA) 
    eval {checkbutton $f.setime     -text "Series Time"   -variable PACS(Se_Time)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.setimebox -textvariable PACS(SeTime)} $Gui(WEA) 
    eval {checkbutton $f.semodality  -text "Modality"  -variable PACS(Se_Modality) -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.semodalitybox -textvariable PACS(SeModality)} $Gui(WEA) 
    eval {checkbutton $f.sebody   -text "Body Part Examined" -variable PACS(Se_Body) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.sebodybox -textvariable PACS(SeBody)} $Gui(WEA) 
    eval {checkbutton $f.seuid    -text "Series Instance UID" -variable PACS(Se_UID) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.seuidbox -textvariable PACS(SeUID)} $Gui(WEA)    
    eval {checkbutton $f.senumber    -text "Series Number" -variable PACS(Se_Number) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.senumberbox -textvariable PACS(SeNumber)} $Gui(WEA) 
    eval {checkbutton $f.serelatedins    -text "Nº of Related Instances" -variable PACS(Se_RelatedIns) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.serelatedinsbox -textvariable PACS(SeRelatedIns)} $Gui(WEA) 

    grid $f.sedata        -column 0 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.setime        -column 0 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.semodality    -column 0 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.sebody        -column 0 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.seuid         -column 0 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.senumber      -column 0 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.serelatedins  -column 0 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"


    grid $f.sedatabox         -column 1 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.setimebox         -column 1 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.semodalitybox     -column 1 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.sebodybox         -column 1 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.seuidbox          -column 1 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.senumberbox       -column 1 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.serelatedinsbox   -column 1 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"


    #-------------------------------------------
    # Query->Bot->Series->SendQ frame
    #-------------------------------------------
   
    set f $fQuery.fBot.fSeries.fSendQ
    DevAddButton $f.bQuery "Query..." "FindDICOMFile"
    TooltipAdd $f.bQuery "Press this button to query Series DICOM information to the server (C-FIND)."
    pack $f.bQuery -pady 20

    #-------------------------------------------
    # Query->Bot->Image frame
    #-------------------------------------------
    set f $fQuery.fBot.fImage

    frame $f.fData     -bg $Gui(activeWorkspace)
    frame $f.fSendQ    -bg $Gui(activeWorkspace) 
    pack  $f.fData $f.fSendQ -side top -fill x -pady $Gui(pad)

    
    #-------------------------------------------
    # Query->Bot->Image frame
    #-------------------------------------------
    set f $fQuery.fBot.fImage.fData

    set PACS(I_CUID)         0
    set PACS(I_IUID)         0
    set PACS(I_Instance)     0
    set PACS(I_Overlay)      0
    set PACS(I_Curve)        0
    set PACS(I_LUT)          0
    set PACS(I_Samples)      0


    set PACS(ICUID)          ""
    set PACS(IIUID)          ""
    set PACS(IInstance)      ""
    set PACS(IOverlay)       ""
    set PACS(ICurve)         ""
    set PACS(ILUT)           ""
    set PACS(ISamples)       ""

   
    eval {checkbutton $f.icuid   -text "SOP Class UID"   -variable PACS(I_CUID)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.icuidbox -textvariable PACS(ICUID)} $Gui(WEA) 
    eval {checkbutton $f.iiuid     -text "SOP Instance UID"   -variable PACS(I_IUID)  -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.iiuidbox -textvariable PACS(IIUID)} $Gui(WEA) 
    eval {checkbutton $f.iinstance  -text "Instance Number"  -variable PACS(I_Instance) -onvalue 1 -offvalue 0}  $Gui(WRA)
        eval {entry $f.iinstancebox -textvariable PACS(IInstance)} $Gui(WEA) 
    eval {checkbutton $f.ioverlay   -text "Overlay Number" -variable PACS(I_Overlay) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.ioverlaybox -textvariable PACS(IOverlay)} $Gui(WEA) 
    eval {checkbutton $f.icurve    -text "Curve Number" -variable PACS(I_Curve) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.icurvebox -textvariable PACS(ICurve)} $Gui(WEA)    
    eval {checkbutton $f.ilut    -text "LUT Number" -variable PACS(I_LUT) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.ilutbox -textvariable PACS(ILUT)} $Gui(WEA) 
    eval {checkbutton $f.isamples    -text "Samples per pixel" -variable PACS(I_Samples) -onvalue 1 -offvalue 0}  $Gui(WRA)  
        eval {entry $f.isamplesbox -textvariable PACS(ISamples)} $Gui(WEA) 
    


    grid $f.icuid        -column 0 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.iiuid        -column 0 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.iinstance    -column 0 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.ioverlay     -column 0 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.icurve       -column 0 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.ilut         -column 0 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.isamples     -column 0 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"


    grid $f.icuidbox         -column 1 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.iiuidbox         -column 1 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.iinstancebox     -column 1 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.ioverlaybox      -column 1 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.icurvebox        -column 1 -row 4 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.ilutbox          -column 1 -row 5 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.isamplesbox      -column 1 -row 6 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"


    #-------------------------------------------
    # Query->Bot->Image->SendQ frame
    #-------------------------------------------
   
    set f $fQuery.fBot.fImage.fSendQ
    DevAddButton $f.bQuery "Query..." "FindDICOMFile"
    TooltipAdd $f.bQuery "Press this button to query Image DICOM information to the server (C-FIND)."
    pack $f.bQuery -pady 20




    #-------------------------------------------
    # Retrieve frame
    #-------------------------------------------

    set fRetrieve $Module(PACS,fRetrieve)
    set f $fRetrieve
    
    frame $f.fInicio -bg $Gui(activeWorkspace)
    frame $f.fTop -bg $Gui(backdrop) 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fInicio $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad)  -fill x


    #-------------------------------------------
    # Retrieve->Top frame
    #-------------------------------------------
    set f $fRetrieve.fInicio
    #AE, host, port
    eval {label $f.l1 -text "Host Name:" } $Gui(WLA)
    eval {entry $f.e1 -textvariable PACS(Hostname)} $Gui(WEA)
    grid $f.l1 $f.e1 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l2 -text "Port:" } $Gui(WLA)
    eval {entry $f.e2 -textvariable PACS(Port)} $Gui(WEA)
    grid $f.l2 $f.e2 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l3 -text "Application Entity (AE):" } $Gui(WLA)
    eval {entry $f.e3 -textvariable PACS(AE)} $Gui(WEA)
    grid $f.l3 $f.e3 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l4 -text "AE Destination:" } $Gui(WLA)
    eval {entry $f.e4 -textvariable PACS(AEDestination)} $Gui(WEA)
    grid $f.l4 $f.e4 -padx $Gui(pad) -pady $Gui(pad) -sticky e

    eval {label $f.l5 -text "Port Destination:" } $Gui(WLA)
    eval {entry $f.e5 -textvariable PACS(PortDestination)} $Gui(WEA)
    grid $f.l5 $f.e5 -padx $Gui(pad) -pady $Gui(pad) -sticky e



    #-------------------------------------------
    # Retrieve->Bot frame
    #-------------------------------------------
    set f $fRetrieve.fBot
    frame $f.fData     -bg $Gui(activeWorkspace)
    frame $f.fSendQ    -bg $Gui(activeWorkspace) 
    pack  $f.fData $f.fSendQ -side top  -pady $Gui(pad)


    #-------------------------------------------
    # Retrieve->Bot->Data frame
    #-------------------------------------------
    set f $fRetrieve.fBot.fData
    

    set PACS(Pide)    ""
    set PACS(SUIDE)   ""
    set PACS(SeUIDE)  ""
    set PACS(IIUIDE)  ""
    
    set PACS(Selection) 0

    eval {radiobutton $f.pid   -text "Patient ID"    -variable PACS(Selection)  -value 1 }  $Gui(WRA)
    eval {entry $f.pidbox -textvariable PACS(Pide)} $Gui(WEA) 

    eval {radiobutton $f.suid    -text "Study Instance UID" -variable PACS(Selection) -value 2 }  $Gui(WRA)  
    eval {entry $f.suidbox -textvariable PACS(SUIDE)} $Gui(WEA)    

    eval {radiobutton $f.seuid    -text "Series Instance UID" -variable PACS(Selection) -value 3}  $Gui(WRA)  
    eval {entry $f.seuidbox -textvariable PACS(SeUIDE)} $Gui(WEA)    

    eval {radiobutton $f.iiuid     -text "SOP Instance UID"   -variable PACS(Selection)  -value 4}  $Gui(WRA)
    eval {entry $f.iiuidbox -textvariable PACS(IIUIDE)} $Gui(WEA) 

    grid $f.pid     -column 0 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.suid    -column 0 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.seuid   -column 0 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
    grid $f.iiuid   -column 0 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "e"
 
    grid $f.pidbox    -column 1 -row 0 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.suidbox   -column 1 -row 1 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.seuidbox  -column 1 -row 2 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"
    grid $f.iiuidbox  -column 1 -row 3 -padx $Gui(pad) -pady $Gui(pad) -sticky "snew"


    #-------------------------------------------
    # Retrieve->Bot->Image->SendQ frame
    #-------------------------------------------
   
    set f $fRetrieve.fBot.fSendQ
    DevAddButton $f.bRetrieve "Retrieve..." "RetrieveDICOMFile"
    TooltipAdd $f.bRetrieve "Press this button to retrieve DICOM image to the server (C-FIND)."
    pack $f.bRetrieve -pady 20

}

#-------------------------------------------------------------------------------
# .PROC PACSSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSSetActive {s} {
    global PACS

    set PACS(server) $s
    $PACS(mbActive) config -text $s
    set PACS(key_query) $s
    raise $PACS(f$s)
}


#-------------------------------------------------------------------------------
# .PROC SendDICOMFile
#  To send DICOM File to PACS
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SendDICOMFile {} {
    global Gui PACS Module Volume Model
 
    #As fisrt step, send a verification SCU to PACS Server
    set host $PACS(Hostname)
    set port $PACS(Port)
    set aet  $PACS(AE)
    set dicom_image $PACS(dcm_image)

    if {$host=="" || $port=="" || $aet=="" || $dicom_image==""} {
        #Display Usage
        set msg "

ECHO SCU: DICOM verification C-ECHO SCU
usage: echoscu options peer port

- parameters:
  HOST NAME: hostname of DICOM peer
  PORT: tcp/ip port number of peer
  DICOM IMAGE: DICOM file to be transmitted

- network options:
  APPLICATION ENTITY (AE): set calling AE title (default: PacsOne)"

        tk_messageBox -message $msg
        } else {

    #Call vtk to give input parameter, hostname, port number and application entity
    vtkPacs SendDICOMImage
    SendDICOMImage SetConnection_Parameters $host $port $aet
    SendDICOMImage SetDICOM_File $dicom_image
    set result [SendDICOMImage StoreSCU]
    SendDICOMImage Delete

    if {$result == 0} {
        tk_messageBox -message "C-STORE command Successful of DICOM file $dicom_image"
        } else {
        #check for error
        tk_messageBox -icon error -message "C-STORE command failed from DICOM file $dicom_image"
        }
    }


}

#-------------------------------------------------------------------------------
# .PROC FindDICOMFile
#  To Query DICOM information to PACS
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FindDICOMFile {} {
    global Gui PACS Module Volume Model

    set host $PACS(Hostname)
    set port $PACS(Port)
    set aet  $PACS(AE)
    

    if {$host=="" || $port=="" || $aet==""} {
        #Display Usage
        set msg "

ECHO SCU: DICOM verification C-ECHO SCU
usage: echoscu options peer port

- parameters:
  HOST NAME: hostname of DICOM peer
  PORT: tcp/ip port number of peer
  DICOM IMAGE: DICOM file to be transmitted

- network options:
  APPLICATION ENTITY (AE): set calling AE title (default: PacsOne)"

        tk_messageBox -message $msg
        return
        } 

   
    #Convert to Upper Letter
        switch -exact -- $PACS(key_query) {
            Patient { set PACS(key_query) "PATIENT" }
            Study   { set PACS(key_query) "STUDY"   }
            Series  { set PACS(key_query) "SERIES"  }
            Image   { set PACS(key_query) "IMAGE"   }
        }

    #If query level is PATIENT and there is not selection error shown
        if {$PACS(key_query)=="PATIENT"} {
            if { $PACS(P_Name) == 0 && $PACS(P_ID) == 0 && $PACS(P_BData)==0 && $PACS(P_BTime)==0 && $PACS(P_Sex)==0 && $PACS(P_RelatedStu)==0 && $PACS(P_RelatedSer)==0 && $PACS(P_RelatedIns)==0} {
                tk_messageBox -icon error -message "You must select any attribute for PATIENT level" -title "Error"
                return 
                }
        }

        if {$PACS(key_query)=="STUDY"} {
            if {$PACS(S_Data)==0 && $PACS(S_Time)==0 && $PACS(S_Accession)==0 && $PACS(S_Physician)==0 && $PACS(S_Description)==0 && $PACS(S_Age)==0 && $PACS(S_Size)==0 && $PACS(S_Weight)==0 && $PACS(S_Weight)==0 && $PACS(S_UID)==0 && $PACS(S_ID)==0 && $PACS(S_RelatedSer)==0 && $PACS(S_RelatedIns)==0} {
                tk_messageBox -icon error -message "You must select any attribute for STUDY level" -title "Error"
                return 
                }
        }

        if {$PACS(key_query)=="SERIES"} {
            if {$PACS(Se_Data)==0 && $PACS(Se_Time)==0 && $PACS(Se_Modality)==0 && $PACS(Se_Body)==0 && $PACS(Se_UID)==0 && $PACS(Se_Number)==0 && $PACS(Se_RelatedIns)==0} {
                tk_messageBox -icon error -message "You must select any attribute for SERIES level" -title "Error"
                return 
                }
        }

        if {$PACS(key_query)=="IMAGE"} {
            if {$PACS(I_CUID)==0 && $PACS(I_IUID)==0 && $PACS(I_Instance)==0 && $PACS(I_Overlay)==0 && $PACS(I_Curve)==0 && $PACS(I_LUT)==0 && $PACS(I_Samples)==0} {
                tk_messageBox -icon error -message "You must select any attribute for IMAGE level" -title "Error"
                return 
                }
        }


        #Antes de pasarle los comandos al findscu lo que hago será:
        #1. Guardar en un archivo los query keys, como el siguiente ejemplo:
        #       query patient name and IDs
        #       (0008,0052) CS [PATIENT]  #QueryRetrievelevel
        #       (0010,0010) PN []         #Patient Name
        #       (0010,0020) LO []         #Patient ID
        #
        #Si existe error al crearlo que de mensaje si no, que continue con el find SCU

        #Abrir archivo para escritura: 
        #open <fichero> <modo> w: escribir solo.lo borra si existe o w+: escribir y leer.lo borra si existe
        #Escribe cadena puts [<-nonewline>] <ficheroID> <cadena>

        #Crear archivo para escritura. Si existe lo borra
        set filename [open ./Modules/vtkPacs/Tests/dumpfile_temp.txt w]

        #Si el archivo no se creó que de error al usuario
        if { ![file exists ./Modules/vtkPacs/Tests/dumpfile_temp.txt] } {
            tk_messageBox -icon error -message "ASCII Dumpfile not created" -title "Error"
            return  
        }
        #Escribir en archivos datos
        #Query retrieve l,vel
        puts $filename "(0008,0052) CS \[$PACS(key_query)\]"



        #----------------------------------------------------------------
        #-------THIS KEYS ARE FOR PATIENT ROOT INFORMATION MODEL---------
        #----------------------------------------------------------------

        if {$PACS(key_query)=="PATIENT"} {
            if {$PACS(P_Name) == 1} {
                puts $filename "(0010,0010) PN \[$PACS(Pname)\]"
            }
            if {$PACS(P_ID) == 1} {
                puts $filename "(0010,0020) LO \[$PACS(Pid)\]"
            }
            if {$PACS(P_BData) == 1} {
                puts $filename "(0010,0030) DA \[$PACS(PBData)\]"
            }
            if {$PACS(P_BTime) == 1} {
                puts $filename "(0010,0032) TM \[$PACS(PBTime)\]"
            }
            if {$PACS(P_Sex) == 1} {
                puts $filename "(0010,0040) CS \[$PACS(PSex)\]"
            }
            if {$PACS(P_RelatedStu) == 1} {
                puts $filename "(0020,1200) CS \[$PACS(PRelatedStu)\]"
            }
            if {$PACS(P_RelatedSer) == 1} {
                puts $filename "(0020,1202) CS \[$PACS(PRelatedSer)\]"
            }
            if {$PACS(P_RelatedIns) == 1} {
                puts $filename "(0020,1204) CS \[$PACS(PRelatedIns)\]"
            }

        }

        if {$PACS(key_query)=="STUDY"} {
            if {$PACS(S_Data) == 1} {
                puts $filename "(0008,0020) DA \[$PACS(SData)\]"
            }
            if {$PACS(S_Time) == 1} {
                puts $filename "(0008,0030) TM \[$PACS(STime)\]"
            }
            if {$PACS(S_Accession) == 1} {
                puts $filename "(0008,0050) SH \[$PACS(SAccession)\]"
            }
            if {$PACS(S_Physician) == 1} {
                puts $filename "(0008,0090) PN \[$PACS(SPhysician)\]"
            }
            if {$PACS(S_Description) == 1} {
                puts $filename "(0008,1030) LO \[$PACS(SDescription)\]"
            }
            if {$PACS(S_Age) == 1} {
                puts $filename "(0010,1010) AS \[$PACS(SAge)\]"
            }
            if {$PACS(S_Size) == 1} {
                puts $filename "(0010,1020) DS \[$PACS(SSize)\]"
            }
            if {$PACS(S_Weight) == 1} {
                puts $filename "(0010,1030) DS \[$PACS(SWeight)\]"
            }
            if {$PACS(S_UID) == 1} {
                puts $filename "(0020,000D) UI \[$PACS(SUID)\]"
            }
            if {$PACS(S_ID) == 1} {
                puts $filename "(0020,0010) SH \[$PACS(SID)\]"
            }
        }

        if {$PACS(key_query)=="SERIES"} {
            if {$PACS(Se_Data) == 1} {
                puts $filename "(0008,0021) DA \[$PACS(SeData)\]"
            }
            if {$PACS(Se_Time) == 1} {
                puts $filename "(0008,0031) TM \[$PACS(SeTime)\]"
            }
            if {$PACS(Se_Modality) == 1} {
                puts $filename "(0008,0060) CS \[$PACS(SeModality)\]"
            }
            if {$PACS(Se_Body) == 1} {
                puts $filename "(0018,0015) CS \[$PACS(SeBody)\]"
            }
            if {$PACS(Se_UID) == 1} {
                puts $filename "(0020,000E) UI \[$PACS(SeUID)\]"
            }
            if {$PACS(Se_Number) == 1} {
                puts $filename "(0020,0011) IS \[$PACS(SeNumber)\]"
            }
        }

        if {$PACS(key_query)=="IMAGE"} {
            if {$PACS(I_CUID) == 1} {
                 puts $filename "(0008,0016) UI \[$PACS(ICUID)\]"
            }
            if {$PACS(I_IUID) == 1} {
                 puts $filename "(0008,0018) UI \[$PACS(IIUID)\]"
            }
            if {$PACS(I_Instance) == 1} {
                 puts $filename "(0020,0013) IS \[$PACS(IInstance)\]"
            }
            if {$PACS(I_Overlay) == 1} {
                 puts $filename "(0020,0022) IS \[$PACS(IOverlay)\]"
            }
            if {$PACS(I_Curve) == 1} {
                 puts $filename "(0020,0024) IS \[$PACS(ICurve)\]"
            }
            if {$PACS(I_LUT) == 1} {
                 puts $filename "(0020,0026) IS \[$PACS(ILUT)\]"
            }
            if {$PACS(I_Samples) == 1} {
                 puts $filename "(0028,0002) US \[$PACS(ISamples)\]"
            }
        }
        #Fin Escritura
        close $filename

        # Ejecutar dump2dcm file in order to generate ASCII dumpfile to dicom file
        if {[catch "exec $::PACKAGE_DIR_VTKPacs/../../../cxx/dump2dcm $::PACKAGE_DIR_VTKPacs/../../../Tests/dumpfile_temp.txt $::PACKAGE_DIR_VTKPacs/../../../Tests/dumpfile.dcm"]} {
            tk_messageBox -icon error -message "Dump2dcm: Error converting ASCII dumpfile to a dicom file " -title "Error"
            return 
        }


        #vtk connection
        vtkPacs FindDICOMData
        FindDICOMData SetConnection_Parameters $host $port $aet
        FindDICOMData Set_FindKeys $PACS(key_query) "./Modules/vtkPacs/Tests/dumpfile.dcm"
        set result [FindDICOMData FindSCU]

        if {$result == 0} {
            tk_messageBox -message "C-FIND command Successful" -title "C-FIND"
            #Modificado
            set sal [open salida.txt r]
            while {[gets $sal line] >= 0} {
                puts "$line"
            }
            close $sal

            #hasta aqui
            } else {
            #check for error
            tk_messageBox -icon error -message "C-FIND command failed" -title "Error"
        }

        FindDICOMData Delete
}



#-------------------------------------------------------------------------------
# .PROC RetrieveDICOMFile
#  To Retrieve DICOM image to PACS Server
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RetrieveDICOMFile {} {
    global Gui PACS Module Volume Model 

    set host  $PACS(Hostname)
    set port  $PACS(Port)
    set aet   $PACS(AE)
    set aem   $PACS(AEDestination)
    set portD $PACS(PortDestination)
    

    if {$host=="" || $port=="" || $aet=="" || $aem=="" || $portD==""} {
        #Display Usage
        set msg "

ECHO SCU: DICOM verification C-ECHO SCU
usage: echoscu options peer port

- parameters:
  HOST NAME: hostname of DICOM peer
  PORT: tcp/ip port number of peer
  PORT DESTINATION: tcp/ip port number of peer destination
  
- network options:
  APPLICATION ENTITY (AE): set calling AE title (default: PacsOne)
  APPLICATION ENTITY DESTINATION: set move destination AE title (default: Client)"

        tk_messageBox -message $msg
        return
        } 
        
    #If no selection of Unique ID error shown

        if {$PACS(Selection) == 0} {
            tk_messageBox -icon error -message "You must select any attribute" -title "Error"
            return 
            }


        #Depends on attribute key is the query level
        switch -exact -- $PACS(Selection) {
            1  { set PACS(key_query) "PATIENT" }
            2  { set PACS(key_query) "STUDY"   }
            3  { set PACS(key_query) "SERIES"  }
            4  { set PACS(key_query) "IMAGE"   }
        }

        #Antes de pasarle los comandos al findscu lo que hago será:
        #1. Guardar en un archivo los query keys, como el siguiente ejemplo:
        #       query patient name and IDs
        #       (0008,0052) CS [PATIENT] #QueryRetrievelevel
        #       (0010,0010) PN []        #Patient Name
        #       (0010,0020) LO []        #Patient ID
        #
        #Si existe error al crearlo que de mensaje si no, que continue con el find SCU

        #Abrir archivo para escritura: 
        #open <fichero> <modo> w: escribir solo.lo borra si existe o w+: escribir y leer.lo borra si existe
        #Escribe cadena puts [<-nonewline>] <ficheroID> <cadena>

        #Crear archivo para escritura. Si existe lo borra
        set filename [open ./Modules/vtkPacs/Tests/dumpfile_temp.txt w]

        #Si el archivo no se creó que de error al usuario
        if { ![file exists ./Modules/vtkPacs/Tests/dumpfile_temp.txt] } {
            tk_messageBox -icon error -message "ASCII Dumpfile not created" -title "Error"
            return  
        }
        #Escribir en archivos datos
        #Query retrieve l,vel
        
        puts $filename "(0008,0052) CS \[$PACS(key_query)\]"



        #----------------------------------------------------------------
        #-------THIS KEYS ARE FOR PATIENT ROOT INFORMATION MODEL---------
        #----------------------------------------------------------------

        if {$PACS(key_query)=="PATIENT"} {
            puts $filename "(0010,0020) LO \[$PACS(Pide)\]"
        }

        if {$PACS(key_query)=="STUDY"} {
            puts $filename "(0020,000D) UI \[$PACS(SUIDE)\]"
        }

        if {$PACS(key_query)=="SERIES"} {
            puts $filename "(0020,000E) UI \[$PACS(SeUIDE)\]"
        }

        if {$PACS(key_query)=="IMAGE"} {
            puts $filename "(0008,0018) UI \[$PACS(IIUIDE)\]"
        }
        #Fin Escritura
        close $filename

        # Ejecutar dump2dcm file in order to generate ASCII dumpfile to dicom file
        if {[catch "exec ./Modules/vtkPacs/cxx/dump2dcm ./Modules/vtkPacs/Tests/dumpfile_temp.txt ./Modules/vtkPacs/Tests/dumpfile.dcm"]} {
            tk_messageBox -icon error -message "Dump2dcm: Error converting ASCII dumpfile to a dicom file " -title "Error"
            return
        }
        
        #vtk connection
        vtkPacs RetrieveDICOMData
        RetrieveDICOMData SetConnection_ParametersRetrieve $host $port $aet $aem $portD
        RetrieveDICOMData Set_FindKeys $PACS(key_query) "./Modules/vtkPacs/Tests/dumpfile.dcm"
        set result [RetrieveDICOMData RetrieveSCU]
        if {$result == 0} {
            set msg "Retrieve Operation was Successfully done.

Do you want to load DICOM Series?"

            set resp [tk_messageBox -type okcancel -message $msg -title "MOVE SCU"]
            if {$resp == "cancel"} {
                RetrieveDICOMData Delete
                return
            }
            
            Tab [lindex $Module(idList) 1] 

            } else {
            #check for error
            tk_messageBox -icon error -message "C-MOVE command failed" -title "Error"
        }

        RetrieveDICOMData Delete
}





#-------------------------------------------------------------------------------
# .PROC ConnectPACS
# PACS Connection Verification
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ConnectPACS {} {
    global Gui PACS Module Volume Model 

        #As fisrt step, send a verification SCU to PACS Server
        set host $PACS(Hostname)
        set port $PACS(Port)
        set aet  $PACS(AE)


        if {$host=="" || $port=="" || $aet==""} {
            #Display Usage
            set msg "

ECHO SCU: DICOM verification C-ECHO SCU
usage: echoscu options peer port

- parameters:
  HOST NAME: hostname of DICOM peer
  PORT: tcp/ip port number of peer

- network options:
  APPLICATION ENTITY (AE): set calling AE title (default: PacsOne)"

            tk_messageBox -message $msg

            } else {
            #exec c:/WINDOWS/system32/cmd.exe
            #Call vtk to give input parameter, hostname, port number and application entity
            vtkPacs ConnectPacs
            ConnectPacs SetConnection_Parameters $host $port $aet
            set result [ConnectPacs Verification_SOP]
            ConnectPacs Delete
            
            if {$result == 0} {
                tk_messageBox -message "C-ECHO command verified Successfully from AE $aet"
            } else {
            #check for error
                tk_messageBox -icon error -message "C-ECHO command verified Unsuccessfully from AE $aet"
            }
        }
}




#-------------------------------------------------------------------------------
# .PROC PACSEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSEnter {} {
    global PACS
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
   # pushEventManager $PACS(eventManager)

    # clear the text box and put instructions there
    #$PACS(textBox) delete 1.0 end
    #$PACS(textBox) insert end "Shift-Click anywhere!\n"

}

#-------------------------------------------------------------------------------
# .PROC PACSExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}


#-------------------------------------------------------------------------------
# .PROC PACSUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
# (Note: to do this, this proc must be this module's procMRML.  Right now,
# these buttons are being updated automatically since they have been added
# to lists updated in VolumesUpdateMRML and ModelsUpdateMRML.  So this procedure
# is not currently used.)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSUpdateGUI {} {
    global PACS Volume
    
    DevUpdateNodeSelectButton Volume PACS Volume1 Volume1 DevSelectNode
    DevUpdateNodeSelectButton Model  PACS Model1  Model1  DevSelectNode
}



#-------------------------------------------------------------------------------
# .PROC PACSShowFile
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc PACSShowFile {} {
    global PACS
    
    $PACS(lfile) config -text "You entered: $PACS(FileName)"
}


#-------------------------------------------------------------------------------
# .PROC PACSBindingCallback
# Demo of callback routine for bindings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PACSBindingCallback { event W X Y x y t } {
    global PACS

    set insertText "$event at: $X $Y\n"
    
    switch $event {
    "Shift-2" {
        set insertText "Don't poke the Slicer!\n"
    }
    "Shift-3" {
        set insertText "Ouch!\n"
    }

    }
    $PACS(textBox) insert end $insertText

}
