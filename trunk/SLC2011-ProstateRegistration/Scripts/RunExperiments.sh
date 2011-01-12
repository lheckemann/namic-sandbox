DATA_ROOT=~/SLC2011/SLC2011-ProstateRegistration/TestData
EXE_ROOT=~/Slicer/Slicer-3-6/Slicer3-build/lib/Slicer3/Plugins

VOLUME=TR03_series10-T2Ax.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TR03_series16-NeedleAx' 'TR03_series14-NeedleAx'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done


VOLUME=TR02_series10-T2Ax.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TR02_series20-trufiAx' 'TR02_series21-trufiAx2'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done

VOLUME=TR01_series10-T2BC.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TR01_series13-NeedleBC' 'TR01_series19_NeedleBC'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done

VOLUME=TP9_series3.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TP9_series424'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done 
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done

VOLUME=TP11_series3.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TP11_series18' 'TP11_series34'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done 
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done

VOLUME=TP10_series3.nrrd
echo
echo Running case $VOLUME
for SLICE in 'TP10_series27' 'TP10_series17'
do
  for LABEL in 'A' 'AC' 'ACS'
  do
    cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --volumeImageMask '${DATA_ROOT}'/'${SLICE}'-label-'${LABEL}'.nrrd --transform '${VOLUME}'-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm'
    $cmd
  done
  cmd=${EXE_ROOT}'/SliceRegistration --sliceImage '${DATA_ROOT}'/'${SLICE}'.nrrd --volumeImage '${DATA_ROOT}'/'${VOLUME}' --transform '${VOLUME}'-'${SLICE}'-VolumeRegistration.tfm'
  $cmd
done

