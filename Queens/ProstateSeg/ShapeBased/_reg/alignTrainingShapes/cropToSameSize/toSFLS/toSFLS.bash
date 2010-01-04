#!/bin/bash

exeFile=./r/toSFLS

for j in ../*.nrrd; do
    outputName=`echo ${j} | sed s/.nrrd/-SFLS.nrrd/`
    ${exeFile} $j ${outputName}
    mv ${outputName} ./
done
