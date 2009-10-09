mkdir /home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4

/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/bin/DeformableRegistration01 \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/42_Masked.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/42_Masked_Deformed.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/42_Mask.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_Deformed_BSplineTo_42.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_Deformed_BSplineTo_42_DifferenceAfter.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_Deformed_BSplineTo_42_DifferenceBefore.mhd \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/42_Deformed_BSplineTo_42_DeformationField.mhd \
 1 1 \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/finalAffineTransform_42_Deformed_42.txt \
/home/ibanez/bin/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Release/Testing/Temporary/$4/finalBSplineTransform_42_Deformed_42.txt \
$1  $2  1.0  $3

/home/ibanez/src/NAMIC/NAMICSandbox/SIMBIOS/KneeModeling/Registration/ImageToImage/Testing/EvaluateMetric.sh $1 $2 $3 $4


