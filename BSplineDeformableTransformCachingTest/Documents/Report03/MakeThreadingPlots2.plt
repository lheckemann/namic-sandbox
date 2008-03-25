  plot "AllThreadingResults2.txt" using 9:13 with linespoints lw 3 title 'Sparse, Threaded'
  set xlabel "Number of Grid Nodes"
  show xlabel
  set ylabel "Execution Time (sec)"
  show ylabel
  set title "Execution Time vs. BSpline Grid Nodes"
#  pause -1;
  set out 'ThreadingTiming2.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14
  replot
  set size 1,1