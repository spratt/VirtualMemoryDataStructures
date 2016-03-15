set ylabel "Time (ns)"
set boxwidth 0.9
set term aqua size 500,300
set size 1,1
set nokey
set style fill solid
set datafile separator ","
plot "output.csv" every::1::7 with boxes
