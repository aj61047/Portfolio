# Part 2.1

set terminal png

set output "Part 2_1 Energy.png"
set xlabel "T"
set ylabel "Energy"
unset key
plot "Ising_2D_averages.txt" using 1:2:3 with errorbars

set output "Part 2_1 Magnetisation.png"
set xlabel "T"
set ylabel "Magnetisation"
set key
plot "Ising_2D_averages.txt" using 1:4:5 with errorbars title "Computational",\
     "Ising_2D_averages.txt" using 1:6 title "Analytic"

set output "Part 2_1 Specific_Heat.png"
set xlabel "T"
set ylabel "Specific Heat"
unset key
plot "Ising_2D_averages.txt" using 1:7

set output "Part 2_1 Magnetic_Susceptibility.png"
set xlabel "T"
set ylabel "Magnetic Susceptibility"
unset key
plot "Ising_2D_averages.txt" using 1:8


# Part 2.2

set output "Part 2_2 Specific Heat.png"
set xlabel "T"
set ylabel "Specific Heat"
set key
plot "a2_part2_2_specific_heat.txt" using 1:2 title "L=8", \
     "a2_part2_2_specific_heat.txt" using 1:3 title "L=16", \
     "a2_part2_2_specific_heat.txt" using 1:4 title "L=32", \
     "a2_part2_2_specific_heat.txt" using 1:5 title "L=64", \

set output "Part 2_2 Magnetic Susceptibility.png"
set xlabel "T"
set ylabel "Magnetic Susceptibility"
set key
plot "a2_part2_2_susceptibility.txt" using 1:2 title "L=8", \
     "a2_part2_2_susceptibility.txt" using 1:3 title "L=16", \
     "a2_part2_2_susceptibility.txt" using 1:4 title "L=32", \
     "a2_part2_2_susceptibility.txt" using 1:5 title "L=64", \


set output "Part 2_2 Specific Heat Rescaled.png"
set xlabel "L^(1/nu)t"
set ylabel "Rescaled Specific Heat"
set key
set xrange [-50:50]
plot "a2_part2_2_specific_heat_rescaled.txt" using 1:2 title "L=8", \
     "a2_part2_2_specific_heat_rescaled.txt" using 1:3 title "L=16", \
     "a2_part2_2_specific_heat_rescaled.txt" using 1:4 title "L=32", \
     "a2_part2_2_specific_heat_rescaled.txt" using 1:5 title "L=64", \

set output "Part 2_2 Magnetic Susceptibility Rescaled.png"
set xlabel "L^(1/nu)t"
set ylabel "Rescaled Magnetic Susceptibility"
set key
set xrange [-50:50]
plot "a2_part2_2_susceptibility_rescaled.txt" using 1:2 title "L=8", \
     "a2_part2_2_susceptibility_rescaled.txt" using 1:3 title "L=16", \
     "a2_part2_2_susceptibility_rescaled.txt" using 1:4 title "L=32", \
     "a2_part2_2_susceptibility_rescaled.txt" using 1:5 title "L=64", \

set output "Part 2_2 Critical Magnetisation.png"
set xlabel "log|T - T_c|"
set ylabel "log(M)"
set autoscale xy
unset key
plot "a2_part2_2_Q3.txt" using 1:3

set output "Part 2_2 Critical Susceptibility.png"
set xlabel "log|T - T_c|"
set ylabel "log(X)"
unset key
plot "a2_part2_2_Q3.txt" using 1:2



# Part 2.3

set output "Part 2_3 Parallelised_Energy_SweepNumber.png"
set xlabel "Sweep Number"
set ylabel "Energy"
unset key
#set xrange [-1:1]
#set yrange [-1:1]
#set autoscale xy
plot "Ising_2D_parallelised.txt" using 1:2

set output "Part 2_3 Parallelised_ThreadNumber_ElapsedTime.png"
set xlabel "Number of Threads"
set ylabel "Elapsed Time (s)"
unset key
set xrange [0.9:8.1]
#set yrange [-1:1]
#set autoscale xy
plot "Parallelised_ThreadNumber_ElapsedTime.txt" using 1:2

