mkdir /home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4

/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/bin/DeformableRegistration01 \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/64_Masked.mhd  \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/42_Masked.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/64_Mask.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_BSplineTo_64.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_BSplineTo_64_DifferenceAfter.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_BSplineTo_64_DifferenceBefore.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_BSplineTo_64_DeformationField.mhd \
 1 1 \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/finalAffineTransform_64_42.txt \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/finalBSplineTransform_64_42.txt \
$1  $2  1.0  $3

