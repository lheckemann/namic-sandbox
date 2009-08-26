
#SRCS= $(wildcard *.tex) b2helpcmds.tex
SRCS= $(wildcard *.tex)

#FIGS= $(wildcard figs/*) $(wildcard CoordinateSystems/*)
FIGS= 

#PRESENTSRCS = ./presentation/present.tex 

#MAIN=MutualRegistrationSoftware
MAIN=BRAINSFit
BIBS= $(wildcard *.bib)

all: $(MAIN).dvi

#VPATH = ./presentation
#present: present.ps

B2_EXEC=$(RESEARCHHOME)/../$(ARCH)/$(ABI)/src/bin/brains2
LD_LIB=$(RESEARCHHOME)/../$(ARCH)/$(ABI)/src/lib:$(RESEARCHHOME)/../$(ARCH)/$(ABI)/src/lib/InsightToolkit

$(MAIN).dvi: $(SRCS) $(FIGS)  $(MAIN).lvl3  $(BIBS)
	latex $(MAIN)
	latex $(MAIN)
	makeindex $(MAIN)
	bibtex $(MAIN)
	latex $(MAIN)
	latex $(MAIN)
	dvips -R0 -t letter $(MAIN).dvi -f > $(MAIN).ps

$(MAIN).lvl1: $(SRCS)
	latex $(MAIN)
	touch $(MAIN).lvl1

$(MAIN).lvl2: $(MAIN).lvl1
	latex $(MAIN)
	touch $(MAIN).lvl2

$(MAIN).lvl3: $(MAIN).lvl2
	latex $(MAIN)
	touch $(MAIN).lvl3

final: $(SRCS) $(FIGS) $(MAIN).dvi
	latex $(MAIN)

$(MAIN).ps: $(MAIN).dvi
	dvips -R0 -f -t letter $(MAIN).dvi  > $@

$(MAIN)600dpi.ps: $(MAIN).dvi
	dvips -R0 -mode ljfzzz -D 600 -Z $(MAIN).dvi -o $(MAIN)600dpi.ps

$(MAIN)1200dpi.ps: $(MAIN).dvi
	dvips -R0 -mode ljfzzz -D 1200 -Z $(MAIN).dvi -o $(MAIN)1200dpi.ps

$(MAIN).pdf: $(MAIN).dvi
	dvips -R0 -Ppdf -G0 $(MAIN).dvi -o $(MAIN)pdf.ps
	ps2pdf $(MAIN)pdf.ps 
	mv $(MAIN)pdf.pdf $(MAIN).pdf

everything: $(SRCS) $(FIGS) $(MAIN).pdf $(MAIN)1200dpi.ps $(MAIN).ps $(MAIN).dvi
	rm -f *.aux *.dvi *.log *.lof
	latex $(MAIN)
	bibtex $(MAIN)
	latex $(MAIN)
	latex $(MAIN)
	dvips -R0 -f -t letter $(MAIN).dvi  > $@
	dvips -R0 -mode ljfzzz -D 1200 -Z $(MAIN).dvi -o $(MAIN)1200dpi.ps
	dvips -R0 -Ppdf -G0 $(MAIN).dvi -o $(MAIN)pdf.ps
	ps2pdf $(MAIN)pdf.ps 
	echo DONE

html: $(MAIN).pdf $(MAIN).ps
	rm -rf $(MAIN)
	latex2html -split 1 -no_math -white -html_version 4.0,math,unicode -noexternal_images $(MAIN).tex
#	latex2html            -no_math -white -html_version 4.0,math,unicode -noexternal_images $(MAIN).tex


clean:
	rm -f *.aux *.dvi *.log *.loa *.lof *.lot *.lvl
clobber:
	rm -f *.aux *.bbl *.blg *.dvi *.idx *.ilg *.ind *.lof *.log *.lot *.lvl* *.ps *.toc
