
ADD_LIBRARY(ITKStatisticsNetlib 
  # from SLATEC/FNLIB
  d1mach.c
  d9lgmc.c
  d9gmit.c
  d9lgic.c
  d9lgit.c
  dbetai.c
  dcsevl.c
  dgami.c
  dgamit.c
  dgamlm.c
  dgamma.c
  dgamr.c
  dlbeta.c
  dlgams.c
  dlngam.c
  dlnrel.c
  initds.c
  xermsg.c
  d_int.c
  d_mod.c  
 )

IF(ITK_USE_SYSTEM_VXL)
  TARGET_LINK_LIBRARIES( ITKStatisticsNetlib netlib )
ELSE(ITK_USE_SYSTEM_VXL)
  TARGET_LINK_LIBRARIES( ITKStatisticsNetlib itkv3p_netlib )
ENDIF(ITK_USE_SYSTEM_VXL)


