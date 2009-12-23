(* ::Package:: *)

(*Build all the matrices*)


(*As defined in equation B .4 in Trisan-Vega*)
F[lmax_]:= Module[{j,nJ,ar},
	nJ = getJ[lmax,lmax];
	ar = Array[0&,{nJ}];
	For[j=1,j<=nJ,j++,Module[{l},
		l = getLM[j][[1]];
		ar[[j]] = 2 * Pi * P2[l,0,0]; 
		];		
	];
	Return[DiagonalMatrix[ar]];
];


(*As defined in equation B .4 in Trisan-Vega*)
L[lmax_]:= Module[{j,nJ,ar},
	nJ = getJ[lmax,lmax];
	ar = Array[0&,{nJ}];
	For[j=1,j<=nJ,j++,Module[{l},
		l = getLM[j][[1]];
		ar[[j]] = -l*(l+1); 
		];		
	];
	Return[DiagonalMatrix[ar]];
];


B[grads_,lmax_]:= compRshMatrix[grads,lmax];


compOdf[lmax_,grads_,lambda_,sig_]:=Module[{Fm,Bm,Lm,mat,ret,nFact},
	Bm = B[grads,lmax];
	Fm = F[lmax];
	Lm = L[lmax];
	mat = Fm . PseudoInverse[ (Transpose[Bm] . Bm + lambda * Lm.Lm) , Tolerance->Automatic] . Transpose[Bm];
	ret = mat.sig;
	nFact = 1 / (2 * Sqrt[Pi] * ret[[1]]);
	ret = ret * nFact;
	Return[ret];
];


compOpdf[lmax_,grads_,lambda_,sig_]:=Module[{Fm,Bm,Lm,Em,Dm,mat1,mat2,ret,nFact},
	Bm = B[grads,lmax];
	Fm = F[lmax];
	Lm = L[lmax];
	Em = sig;
	Dm = Log[Em];

	mat1 = Fm . PseudoInverse[ (Transpose[Bm] . Bm + lambda * Lm.Lm) , Tolerance->Automatic];
	mat2 = 2 * Transpose[Bm] . (Dm * (3 + 2 Dm) * Em) + Lm . Transpose[Bm] . Em;
	ret = mat1.mat2;
	nFact = 1 / (2 * Sqrt[Pi] * ret[[1]]);
	ret = ret * nFact;
	Return[ret];
];


compRsh[lmax_,grads_,lambda_,sig_]:=Module[{Bm,Lm,mat,ret,nFact},
	Bm = B[grads,lmax];
	Lm = L[lmax];
	mat = PseudoInverse[ (Transpose[Bm] . Bm + lambda * Lm.Lm) , Tolerance->Automatic] . Transpose[Bm];
	ret = mat.sig;
	Return[ret];
];


(*BELTOW IS STUFF FOR FODS*)


compSymRsh[sig_,grads_,maxL_] :=

Module[ {nGrads,j,A,mat,FullA,l},
	(* Compute the matrix to take RSH 2 DG.
		Dg = A R; *)
	
	nGrads = Length[grads];
	(*Compute only m = 0 matrix*)
	A = ConstantArray[0,{nGrads,maxL / 2}];
	FullA = compRshMatrix[grads,l];
	
	For[l=0,l<=maxL,l=l+2,
		j = getJ[l,0];
		A[[All,l/2+1]];
	];
	mat = PseudoInverse[ (Transpose[A] . A )] . Transpose[A];

	Return[ mat.sig ];
]


compR[fir_,grads_,lm_]:=Module[{firRsh,rd,l,m,val},
(*TODO THIS SHOULD BE BETTER*)
	firRsh = compRsh[lm,grads,0,fir];
	rd = ConstantArray[0,Length[firRsh]];
	For[l=0,l<=lm,l=l+2,
		Print[{l,firRsh[[getJ[l,0]]]//CForm,Sqrt[(2 l+1) / (4 Pi)]//N//CForm}];
		val = firRsh[[getJ[l,0]]] / Sqrt[(2 l+1) / (4 Pi)];
		For[m=-l,m<=l,m++, rd[[ getJ[l,m] ]] = val ];
	];
	Return[DiagonalMatrix[rd]];
];


compFod[sig_,grads_,fir_,lm_]:=
	Module[{sigRsh,R},
		sigRsh = compRsh[lm,grads,0,sig];
		R = compR[fir,grads,lm];
		Return[sigRsh / Diagonal[R]] ;
	];


compConstraintMatrix[P_,rsh_,thr_] := Module[{L,i,n,sig,thrValue},
	sig = P . rsh;
	n = Length[sig];
	L = Array[0&,{0,0}];
	thrValue=thr*Max[sig];
	For[i=1,i<=n,i++,
		If[sig[[i]] <= thrValue,
			If[ValueQ[L],L=Append[L,P[[i,All]]],L = {P[[i,All]]}];
		];
	];
	Return[L];
]


(*Constrain the FOD to be positive*)
compFodCsd[sig_,grads_,fir_,lm_,lam_,thr_,constGrads_]:=
	Module[{R,P,A,fod,cont,c,nBad},
		R = compR[fir,grads,lm];
		P = compRshMatrix[constGrads,lm];
		A = compRshMatrix[grads,lm].R;

		(*Start the minimization Process*)
		cont = True;
		c = 0;
		nBad = 0;
		fod = PadRight[compFod[sig,grads,fir,4],(lm^2 + 3 lm + 2) / 2];
		While[cont,Module[ {Lm,Mm,Minv,b},
			Lm = lam * compConstraintMatrix[P,fod,thr];
			If[Dimensions[Lm][[1]] == nBad, Break[];cont=False, nBad=Dimensions[Lm][[1]]];
			Mm = Join[A,Lm];
			b = Join[sig,ConstantArray[0,nBad]];
			fod = PseudoInverse[Mm,Tolerance->Automatic] . b;
			c++;
			Print[{ nBad,c,Min[fod],Max[fod]}];
		];];
		Return[fod];
];
