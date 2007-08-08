# \auth        or    Hans J. Johnson"
# \date     $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests common components of loading any image file format, i.e., if it
#           is a valid image, then these items must be testable:
# \fn        proc CoreImageTest {}
# \params  They should be self expanatory
# \result    1 in case testing is complete or 0 in case of a fatal error
proc interactiveModes {pathToRegressionDir dateString} {
    global B2_BATCH_MODE
## Test precondition requirements

########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "interactiveModes"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2 mode commands"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#        close $LogFile
#        set LogFile stderr

        if {$B2_BATCH_MODE != 0} {
            return $MODULE_FAILURE
        }
        set SubTestDes "Test for brains2 startup."
        set SubTestQuestion "Did brains2 start correctly?"
        set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
        puts "-----\n\n\n\n\n\n $response\n--------------------------------------\n"
        ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

################################################ GE-4X Data ###############################################################
# Load a GE-4x Image
#cd $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002
        set ge4x [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.040]
        if {$ge4x == -1} {
            puts stderr "ERROR: Failed to load ge4x image"
        } else {
            puts stderr "SUCCESS: Loaded ge4x image"
        }

    set TestRoiID [b2 load roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi]
        if {$TestRoiID == -1} {
            puts stderr "ERROR: Failed to load roi"
        } else {
            puts stderr "SUCCESS: Loaded roi"
        }

########################################
########################################
# Run Tests
########################################
########################################
##########################  Test Modes #########################################################
    #NOTE ALL MODES EXCEPT RESAMPLE ARE NOW TESTED IMPLICITLY BELOW
    #set testModes "resample talairach-parameters cerebellum-parameters hippocampus-parameters landmark roi"
    set testModes "resample"
    set numModes [llength $testModes ]

        for {set currMode 0} { $currMode < $numModes } {incr currMode} {
            set SubTestDes "b2 mode [ lindex $testModes $currMode ] "
                set SubTestQuestion "Are you in [ lindex $testModes $currMode ] mode?"
                set commandresponse [ eval $SubTestDes ]
                set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
                ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName "ENTER $SubTestDes"

            set SubTestDes "b2 mode [ lindex $testModes $currMode ] "
                set SubTestQuestion "Please test [ lindex $testModes $currMode ] mode.  Does it seem to work?"
#                set commandresponse [ eval $SubTestDes ]
                set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
                ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName "PERFORM $SubTestDes"
        }

##########################  Test active #########################################################
      set SubTestDes "b2 active landmark Cerebrum-Acquisition"
      set SubTestQuestion "Are you in landmark Cerebrum-Acquisition mode?"
      b2 mode landmark
      set newland1 [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active landmark Cerebrum-Acquisition"
      set SubTestQuestion "Please test landmark Cerebrum-Acquisition mode.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "b2 active landmark Surface-Brainstem"
      set SubTestQuestion "Are you in landmark Surface-Brainstem mode?"
      b2 mode landmark
      set newland2 [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active landmark Surface-Brainstem"
      set SubTestQuestion "Please test landmark Surface-Brainstem mode.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "b2 mode roi cor"
      set SubTestQuestion "Is the coronal window in Active ROI mode?"
      set response [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "b2 active roi cran plane= cor"
      set SubTestQuestion "Is the coronal window in Active ROI mode with the ROI set to cran?"
      set newcranroi [ eval $SubTestDes ]
      ReportTestStatus $LogFile  [ expr  {$newcranroi != -1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active roi cran plane= cor"
      set SubTestQuestion "Please test roi cran plane= cor mode by creating an ROI.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test editing of ROI"
      set SubTestQuestion "Please edit the ROI you just created.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test editing of ROI -delete"
      set SubTestQuestion "Please make an roi, and then delete it.  Does this work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test editing of ROI -Keep 4"
      set SubTestQuestion "Please make 5 rois, and then keep 4.  Does this work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test editing of ROI -Keep 1"
      set SubTestQuestion "Please make 4 rois, and then keep 1.  Does this work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test editing of ROI -edit"
      set SubTestQuestion "Please edit the roi.  Does this work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test selecting of ROI"
      set SubTestQuestion "Please select an the roi.  Does this work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test selecting of ROI - change slice"
      set SubTestQuestion "Please make a new roi. Then select it. The goto another slice. Then back to original slice. Is the ROI selected? (It should not be!)."
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 0 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test Change slice of ROI while tracing"
      set SubTestQuestion "Please start, but do not finish, an roi.  Then change slices.  Does the Pop window change so that you can return to the edit slice?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "Test Change slice of ROI while editing"
      set SubTestQuestion "Please select an roi to edit, but do not finish.  Then change slices.  Does the Pop window change so that you can return to the edit slice?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "b2 active talairach-parameters -1"
      set SubTestQuestion "Are you in active talairach-parameters mode?"
      b2 mode talairach-parameters
      set newtalpar [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active talairach-parameters -1"
      set SubTestQuestion "Please test talairach-parameters mode.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "b2 active cerebellum-parameters -1"
      set SubTestQuestion "Are you in active cerebellum-parameters mode?"
      b2 mode cerebellum-parameters
      set newcerpar [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active cerebellum-parameters -1"
      set SubTestQuestion "Please test cerebellum-parameters mode.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#
      set SubTestDes "b2 active hippocampus-parameters -1"
      set SubTestQuestion "Are you in active hippocampus-parameters mode?"
      b2 mode hippocampus-parameters
      set newhippar [ eval $SubTestDes ]
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes

      set SubTestDes "Test b2 active hippocampus-parameters -1"
      set SubTestQuestion "Please test hippocampus-parameters mode.  Does it seem to work?"
      set response [tk_dialog .interactiveQuestions "$SubTestDes" "$SubTestQuestion" warning 1 "Yes" "No"]
      ReportTestStatus $LogFile  [ expr  {$response != 1 }   ] $ModuleName $SubTestDes
#

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
# Clean up from the GE-4x Scan
        ReportTestStatus $LogFile  [ expr { [ b2 destroy every transform ] != -1 } ] $ModuleName "Destroying every transform"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $ge4x ] != -1 } ] $ModuleName "Destroying image $ge4x"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $newtalpar ] != -1 } ] $ModuleName "Destroying talairach-parameters $newtalpar"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $newcerpar ] != -1 } ] $ModuleName "Destroying talairach-parameters $newcerpar"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $newhippar ] != -1 } ] $ModuleName "Destroying talairach-parameters $newhippar"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $newcranroi ] != -1 } ] $ModuleName "Destroying roi $newcranroi"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $newland1 ] != -1 } ] $ModuleName "Destroying landmark $newland1"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $newland2 ] != -1 } ] $ModuleName "Destroying landmark $newland2"

    return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

