#pragma once

#include <iostream>
#include "vector.hpp"
#include "matrix.hpp"
#include <cmath>

// Includes functions required to simulate the 2D Ising model, including functions to calculate the change in energy and magnetisation 
// of the system when a spin is flipped, initialise the lattice, and output a visual representation of the lattice to the terminal


// Function to calculate the change in energy of the system when a spin is flipped
// sigma_left = spin on the left of the spin to be flipped
// sigma_right = spin on the right of the spin to be flipped
// sigma_final = the value of the spin to be flipped, after the flip occurs
double delta_Energy(int sigma_final, int sigma_left, int sigma_right, int sigma_above, int sigma_below, double mu, double J,\
         double B){
    double delta_E = -2*sigma_final*(J*(sigma_left+sigma_right+sigma_above+sigma_below)+mu*B);
    return delta_E;
}

// Function to calculate the change in magnetisation of the system when a spin is flipped
// sigma_final = the value of the spin to be flipped, after the flip occurs
double delta_Magnetisation(int sigma_final){
    double delta_M = 2*sigma_final;
    return delta_M;
}

// Function to initialise the lattice such that all spins are up
Matrix initialise_lattice(Matrix lattice, int L){
    for(int j=0; j<L; j++){
        for(int i=0; i<L; i++){
            lattice.at(j, i) = 1;
        }
    }
    return lattice;
}

// Function to output a visual representation of the lattice to the terminal, where spin up is represented by O, and spin down is represented by -
void output_lattice(Matrix lattice, int L){
    for(int j=0; j<L; j++){
        for(int i=0; i<L; i++){
            if(lattice.at(j, i)==-1){
                std::cout << "-" << " ";
            }
            else if(lattice.at(j,i)==1){
                std::cout << "O" << " ";
            }
            else{
                std::cout << std::endl << "Error, element " << i << " of lattice is of not -1 or 1.";
                break;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}