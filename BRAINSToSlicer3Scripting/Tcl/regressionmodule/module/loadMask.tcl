# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various mask file formats
# \fn        proc loadMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a Run Length Endoded Mask
# Load a BRAINS2 mask (Octree)
#
# To Do
#------------------------------------------------------------------------
# Add BRAINS bit packed masks? I don't think that there are any of these
# on our system
#


# proc CoreMaskTest has been moved to ModuleUtils.tcl.

proc loadMask {pathToRegressionDir dateString} {

    set ModuleName "loadMask"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_load_mask command and loading various Mask file formats"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_load_mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_mask /invalid_directory_name/cran_mask.dummy]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    ############################### BRAINS2 Octree Masks ###########################################
    ## Set known information about mask
    set MaskTypeName "BRAINS2-OCTREE-MASK"
    set NumDims 3
    set XDims 256
    set YDims 256
    set ZDims 192
    set NumRes 3
    set XRes 1.015625
    set YRes 1.015625
    set ZRes 1.015625
    set SubTestDes "load $MaskTypeName test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_brain_trim.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ]} {
        CoreMaskTest $MaskTypeName $TestMaskID $NumDims $XDims $YDims $ZDims $NumRes $XRes $YRes $ZRes $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"
    }

    ############################### BRAINS Bit Packed Mask ################################################
    # Do we have any of these on the system
    #######################################################################################################
    return [ StopModule  $LogFile $ModuleName ]
}

