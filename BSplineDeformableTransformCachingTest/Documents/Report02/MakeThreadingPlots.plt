
  plot "AllThreadingResults.txt" using 5:9 with linespoints lw 3 title 'Threading', "AllDefaultResults.txt" using 5:9 with linespoints lw 3 title 'NoThreading'
  set xlabel "Number of Samples"
  show xlabel
  set ylabel "Execution Time (secs)"
  show ylabel
  set title "Execution Time vs. Samples Size With and Without Threading" 
#  pause -1;
  set out 'ThreadingTiming.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14 
  replot
  set size 1,1

