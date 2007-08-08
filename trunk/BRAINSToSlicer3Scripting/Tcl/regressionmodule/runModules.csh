#!/bin/csh

foreach t (module/*.tcl)
  echo "source module/util/DateStamp.tcl" > tmp.tcl
  echo "source module/util/ReportTestStatus.tcl" >> tmp.tcl
  echo "source module/util/ModuleUtils.tcl" >> tmp.tcl
  echo 'set dateString [DateStamp]' >> tmp.tcl
  echo 'set pathToRegressionDir ../regressiontest' >> tmp.tcl
  echo "source $t" >> tmp.tcl
  set r  = ${t:t}
  echo ${r:r} ' $pathToRegressionDir $dateString' >> tmp.tcl
  echo "b2 exit" >> tmp.tcl
  echo "STARTING ${r:r}" > test/${r:r}.out
  $RESEARCHHOME/$ARCH/$ABI/bin/brains2 -c tmp.tcl >>& test/${r:r}.out
  echo "END ${r:r}" >> test/${r:r}.out
end

exit
