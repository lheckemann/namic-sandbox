#!/bin/bash

if [ $# -ne 6 ] ;then
   echo "USAGE: $0 <Path to brains2> <LD_LIBRARY_PATH> <Modules Path> <Output Path> <regression_test_path> <Module Name>"
   echo "Given $# Args: $0 $1 $2 $3 $4 $5 $6 $7 $8"
   exit -20
fi

export execbin=$1
export regressiontestpathbase=$5

export modpath=$3
export currmod=$6
export currtest=${modpath}/module/${currmod}.tcl
export testpath=${modpath}/module

export outpath=$4
mkdir -p ${outpath}
export outputlog=${outpath}/${currmod}.log

export currwrapper=${outpath}/${currmod}_wrapper.b2
#  No longer needed moved to ModuleUtils.tcl source ${modpath}/module/util/ReportTestStatus.tcl
#run the script
cat > ${currwrapper} << EOF
source ${modpath}/module/util/DateStamp.tcl
source ${modpath}/module/util/ModuleUtils.tcl
source ${modpath}/../../Common/BRAINSWrappers.tcl
global OUTPUT_DIR
set OUTPUT_DIR ${outpath}
set DATA_PATH ${regressiontestpathbase}
global MODULE_SUCCESS
global MODULE_FAILURE
set dateString [DateStamp]
source ${currtest}
set moduleStatus \$MODULE_FAILURE
set tclStatus [ catch { set moduleStatus [ ${currmod} \$DATA_PATH \$dateString ] } message ]
if { \$tclStatus == 0  &&  \$moduleStatus == 0} {
   puts stderr "Successful execution"
   set status 0
} else {
   puts stderr \$message
   if { \$tclStatus != 0 } { 
      puts stderr "tclStatus (process interrruption) == \$tclStatus" 
   }
   if { \$moduleStatus != \$MODULE_SUCCESS } {
      puts stderr "moduleStatus (test violation) == \$moduleStatus" 
   }
   set status 1
}
b2_exit \$status 0
EOF
# exit staus of success is currently set to 1
#should be b2_exit, but thie status is not working properly
