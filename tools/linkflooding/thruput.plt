set terminal png size 1200, 800
set output "throughput.png"
set title "Throughput and Good put"
set xlabel "Node number"
set ylabel "Throughput and Good put"
plot "thruput.data" using 1:2 with linespoints title "Throughput", "thruput.data" using 1:3 with linespoint title "Goodput" lw 2

set terminal png size 1200, 800
set output "lambda.png"
set title "Lambda (Packets/second)"
set xlabel "Node number"
set ylabel "Lambda (Packets/second)"
plot "lambda.data" using 1:2 with linespoints title "Lambda" lw 2
