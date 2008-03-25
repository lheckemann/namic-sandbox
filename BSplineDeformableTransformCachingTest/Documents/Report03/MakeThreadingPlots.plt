  plot "AllThreadingResults.txt" using 9:13 with linespoints lw 3 title 'Sparse, Threaded', "AllDefaultResults.txt" using 9:13 with linespoints lw 3  title 'Default'
  set xlabel "Number of Grid Nodes"
  show xlabel
  set ylabel "Execution Time (sec)"
  show ylabel
  set title "Execution Time vs. BSpline Grid Nodes"
#  pause -1;
  set out 'ThreadingTiming.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14
  replot
  set size 1,1