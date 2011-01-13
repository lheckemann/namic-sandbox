#Andriy
DATA_ROOT=~/SLC2011/SLC2011-ProstateRegistration/Results/SliceRegistration
EXE_ROOT=~/SLC2011/SLC2011-ProstateRegistration/Tools/bin

#Andras
#DATA_ROOT=c:/Users/andras/devel/SLC2011-Data
#EXE_ROOT=c:/Users/andras/devel/Slicer-3.6/Slicer3-ext/SliceToVolumeRegistration-build/lib/Slicer3/Plugins/Release 
 
VOLUME=TR03_series10-T2Ax
echo
echo Running case $VOLUME
for SLICE in 'TR03_series16-NeedleAx' 'TR03_series14-NeedleAx'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done
done

VOLUME=TR02_series10-T2Ax
echo
echo Running case $VOLUME
for SLICE in 'TR02_series20-trufiAx' 'TR02_series21-trufiAx2'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done

done

VOLUME=TR01_series10-T2BC
echo
echo Running case $VOLUME
for SLICE in 'TR01_series13-NeedleBC' 'TR01_series19-NeedleBC'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nii-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nii-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done

done

VOLUME=TP9_series3
echo
echo Running case $VOLUME
for SLICE in 'TP9_series424'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done

done

VOLUME=TP11_series3
echo
echo Running case $VOLUME
for SLICE in 'TP11_series18' 'TP11_series34'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done

done

VOLUME=TP10_series3
echo
echo Running case $VOLUME
for SLICE in 'TP10_series27' 'TP10_series17'
do
  echo $VOLUME $SLICE
  for LABEL in 'A' 'AC' 'ACS'
  do
    echo $LABEL
    cmd=${EXE_ROOT}'/FindTransformDifference --mask '${DATA_ROOT}'/'${VOLUME}'-label-TG.nrrd '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-'${LABEL}'-SliceRegistration.tfm '${DATA_ROOT}'/'${VOLUME}'.nrrd-'${SLICE}'-VolumeRegistration.tfm'
    $cmd
  done

done
