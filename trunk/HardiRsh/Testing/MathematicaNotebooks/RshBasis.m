(* ::Package:: *)

P2[l_, m_, x_] := Module[{}, 
   If[m < 0, Return[(((l + m)!/(l - m)!)*LegendreP[l, -m, x])/(-1)^m], 
     Return[LegendreP[l, m, x]]]; ]


Y[l_, m_, th_, ph_] := Sqrt[((2*l + 1)/(4*Pi))*((l - m)!/(l + m)!)]*
    P2[l, m, Cos[th]]*E^(I*m*ph);


getLM[j_] := Module[{l, m}, l = Floor[(1 + Sqrt[8*j - 7])/2, 2]; 
        m = j - 1 - (l^2 + l)/2; Return[{l, m}]; ];


getJ[l_, m_] := 1 + (l^2 + l)/2 + m; 


RealSPH[j_, t_, p_] := Module[{l, m, tmp, val}, tmp = getLM[j]; l = tmp[[1]]; 
        m = tmp[[2]]; If[m == 0, val = SphericalHarmonicY[l, m, t, p]]; 
        If[m < 0, val = Sqrt[2]*Re[SphericalHarmonicY[l, m, t, p]]; ]; 
        If[m > 0, val = Sqrt[2]*Im[SphericalHarmonicY[l, m, t, p]]; ];
	 Return[Re[val]];
 ]; 



rshFunc[coeffs_,grad_]:=
Module[{t,p},
	If [ grad[[3]] == 1,
		t=0;p=0;,
		(*ELSE*)
		t=ArcCos[grad[[3]]];(* in [0-Pi] *)
		p=ArcTan[grad[[1]],grad[[2]]];
	];
	Return[rshFunc[coeffs,t,p]];
];


rshFunc[coeff_,t_,p_]:=
	Module[{i,val},
		val = 0;
		For[i=1,i<= Length[coeff],i++,
			val = val + coeff[[i]] * RealSPH[i,t,p];
		];
		Return[val];
	];


plotRSH[coeff_]:=	
	SphericalPlot3D[rshFunc[coeff,t,p],
		{t,0,Pi},{p,0,2*Pi},PlotPoints->10,MaxRecursion->0,AxesLabel->{"x","y","z"},
		ColorFunction->Function[{x,y,z,u,v,r},ColorData["TemperatureMap"][rshFunc[coeff,u,v]/1.0]],
		ColorFunctionScaling->False];


plotRSHPara[coeff_]:=
	ParametricPlot3D[ rshFunc[coeff,t,p]*{Sin[t]Cos[p],Sin[t]Sin[p],Cos[t]},	
		{t,0,Pi},{p,0,2*Pi},PlotPoints->10,MaxRecursion->0,AxesLabel->{"x","y","z"},
		ColorFunction->Function[{x,y,z,u,v},ColorData["TemperatureMap"][rshFunc[coeff,u,v]]],
		ColorFunctionScaling->True,PlotRange->{ {-1,1},{-1,1},{-1,1}}];


compRshMatrix[grads_,l_]:=
Module[ {nGrads,t,p,maxJ,i,j,A},
	(* Compute the matrix to take RSH 2 DG.
		Dg = A R; *)
	maxJ    = (l^2 + 3 l + 2) / 2 ;
	nGrads  = Length[grads];
	A = ConstantArray[0,{nGrads,maxJ}];

	For[i=1,i<=nGrads,i++,
		If [ grads[[i]][[3]] == 1,
			t=0;p=0;,
			(*ELSE*)
			If [ grads[[i]][[3]] == -1,
				t=Pi;p=0;,
				(*ELSE*)
				t=ArcCos[grads[[i]][[3]]];(* in [0-Pi] *)
				p=ArcTan[grads[[i]][[1]],grads[[i]][[2]]];
			];
		];
		For[j=1,j<=maxJ,j++,A[[i,j]] = RealSPH[j,t,p]];
	];
	Return[A];
]


compSigFromRsh[rsh_,grads_] :=
Module[ {nGrads,sig,i},
	nGrads = Length[grads];
	sig = ConstantArray[0,nGrads];
	For[i=1,i<=nGrads,i++,sig[[i]] = rshFunc[rsh,grads[[i]] ] ];	
	Return[sig];
];


findMin[rsh_]:=
Module[ {mt,mp,mv,t,p,v,nt,np},
(*{t,0,Pi},{p,0,2*Pi}*)
nt = 45;
np = 90;
mv = 9999999;
For[t=0,t<=Pi,t=t+Pi/nt,
	For[p=0,p<= 2 Pi,p=p+(2 Pi/np),
		v = rshFunc[rsh,t,p];
		If[v < mv,
			mv = v;
			mt = t;
			mp = p;
		];
	];
  ];
If[mv != 9999999, Return[{mv,mt,mp}],Return[{0,0,0}]]; 
];


compMinGradDir[rsh_,grads_]:=
Module[ {sig,mGrad,i,mVal},
	sig = compSigFromRsh[rsh ,grads];
	i = Position[ sig, Min[sig]][[1,1]];
	mGrad = grads[[ i ]];
	mVal  = sig[[ i ]];
	Return[{mVal,mGrad}];
];


compMaxGradDir[rsh_,grads_]:=
Module[ {sig,mGrad,i,mVal},
	sig = compSigFromRsh[rsh ,grads];
	i = Position[ sig, Max[sig]][[1,1]];
	mGrad = grads[[ i ]];
	mVal  = sig[[ i ]];
	Return[{mVal,mGrad}];
];


rshVec[grad_,maxJ_]:=Module[ {ret,t,p,j},
	If [ grad[[3]] == 1,
		t=0;p=0;,
		(*ELSE*)
		t=ArcCos[grad[[3]]];(* in [0-Pi] *)
		p=ArcTan[grad[[1]],grad[[2]]];
	];

	ret = Array[0&,maxJ];
	For[j=1,j<=maxJ,j++,ret[[j]] = RealSPH[j,t,p]];

	Return[ret];
];


compPosRsh[rsh_,maxSteps_,grads_]:=
Module[ {s, u, i, ret, outRsh,mJ,lVal,B},
	s = 1.0; (*Initial time step*)
	outRsh = rsh;
	i = 0;
	mJ = Length[rsh];
	
	lVal = -999999999;
	While[i<=maxSteps,
		ret = compMinGradDir[outRsh,grads];
		If[ret[[1]] > 0,Print["Done"];Break[]; ]; (*all done*)
		If[s < 0.001,Print["Done"];Break[]; ]; (*all done*)
		
		If[ ret[[1]] < lVal , s = s/2., outRsh = outRsh + s * rshVec[ret[[2]],mJ];lVal = ret[[1]];];
		
		i++;
		Print[{i,s,lVal}]
	];
	Return[outRsh];
 ];

