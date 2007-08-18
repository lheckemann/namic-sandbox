#!/bin/csh

#foreach t (module/*.tcl)
foreach ts (`cat runModules.list`)
  set t = "module/${ts}.tcl"
  set r = ${t:t}
  set n = ${r:r}
#  echo "Testing ${t}"
  echo "source module/util/DateStamp.tcl" > drive.${n}.tcl
  echo "source module/util/ReportTestStatus.tcl" >> drive.${n}.tcl
  echo "source module/util/ModuleUtils.tcl" >> drive.${n}.tcl
  echo 'set dateString [DateStamp]' >> drive.${n}.tcl
  echo 'set pathToRegressionDir ../../../regressiontest' >> drive.${n}.tcl
  echo "source $t" >> drive.${n}.tcl
  echo ${n} ' $pathToRegressionDir $dateString' >> drive.${n}.tcl
  echo "b2_exit" >> drive.${n}.tcl
#  echo "STARTING ${n}" > test/${n}.out
#  $RESEARCHHOME/$ARCH/$ABI/bin/brains2 -c ${r}.tcl >>& test/${r:r}.out
  echo $RESEARCHHOME/$ARCH/$ABI/bin/brains2 -c drive.${n}.tcl \|\& tee drive.${n}.${ARCH}.${ABI}.1.out\; tail -1 drive.${n}.${ARCH}.${ABI}.1.out \| grep Completed \>\> ${ARCH}/${ABI}/regr.out
#  echo cat \`ls -1 ${ARCH}/${ABI}/${n}_\*_\* \| sort \| tail -1\` \>\> ${ARCH}/${ABI}/regr.out
#  echo "END ${r:r}" >> test/${n}.out
end

exit
