#!/bin/bash

# Compile and run each of the C++ programs for the Ising model simulations
g++ -std=c++11 -Wall -Wextra -Wpedantic single_lattice_sim.cpp -o single_lattice_sim -O3
./single_lattice_sim

g++ -std=c++11 -Wall -Wextra -Wpedantic four_lattice_sim.cpp -o four_lattice_sim -O3
./four_lattice_sim

g++ -std=c++11 -Wall -Wextra -Wpedantic critical_region_sim.cpp -o critical_region_sim -O3
./critical_region_sim

# Clean up the compiled executables after running the simulations
rm -f single_lattice_sim
rm -f four_lattice_sim
rm -f critical_region_sim