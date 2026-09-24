#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include "matrix.hpp"
#include "output.hpp"
#include <cmath>

// Function to compute sum of all elements in a matrix
double sum(Matrix A, int rows, int cols){
    double result=0;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result = result + A.at(i,j);
        }
    }
    return result;
}

// Function to compute sum of all elements in a vector
double vector_sum(std::vector<double> vector){
    double result = 0;
    for(int i=0; i<vector.size(); i++){
        result = result + vector.at(i);
    }
    return result;
}

// Function to compute variance of a distribution of values
double variance(std::vector<double> V, double V_av){
    std::vector<double> V_squared(V.size());
    for(int i=0; i<V.size(); i++){
        V_squared.at(i) = pow(V.at(i), 2);
    }
    double variance = vector_sum(V_squared)/((double)V_squared.size()) - pow(V_av, 2);

    return variance;
}

// sigma_left = spin on the left of the spin to be flipped
// sigma_right = spin on the right of the spin to be flipped
// sigma_final = the value of the spin to be flipped, after the flip occurs
double delta_Energy(int sigma_final, int sigma_left, int sigma_right, int sigma_above, int sigma_below, double mu, double J,\
         double B){
    double delta_E = -2*sigma_final*(J*(sigma_left+sigma_right+sigma_above+sigma_below)+mu*B);
    return delta_E;
}

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

// Function to output a visual representation of the lattice to the terminal, where spin up is represented by O, 
// and spin down is represented by -
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



int main(){
    double mu = 1;
    double J = 1;
    double B = 0;

    double k = 1;
    std::vector<double> T_vec = {0.1, 0.3, 0.5, 0.7, 1, 1.3, 1.5, 1.7, 2, 2.3, 2.5, 2.7, 3, 3.3, 3.5, 3.7, 4, 4.3, 4.5, 4.7};

    int L = 16;     // Number of spins in each row/column of the square lattice
    int Ns = L*L; // Total number of spins in the lattice

    int burn_in = 100;
    int n_sweeps = 1000+burn_in;

    // Set up random number generator
    std::random_device rd;

    Matrix lattice(L, L);
    lattice = initialise_lattice(lattice, L);   // Only initialise the lattice for the first (lowest) temperature
                                // For higher temps, we take the initial lattice to be the final lattice from the previous temp

    std::vector<double> E_av(T_vec.size());
    std::vector<double> E_error(T_vec.size());
    std::vector<double> M_av(T_vec.size());
    std::vector<double> M_error(T_vec.size());
    std::vector<double> M_theory(T_vec.size());
    std::vector<double> C(T_vec.size());    // Specific heat per spin
    std::vector<double> X(T_vec.size());    // Magnetic susceptibility per spin

    // Output initial state of lattice to the terminal
    output_lattice(lattice, L);

    // Loop over temperatures
    for(int l=0; l<T_vec.size(); l++){
        double T = T_vec.at(l);

        // Calculate the initial energy and magnetisation
        double E = -mu*B*sum(lattice, L, L);
        // Add the interaction term(s)
        for(int i=0; i<L; i++){
            for(int j=0; j<L; j++){
                E = E - J*lattice.at(i, j)*(lattice.at(i, (j+1)%L) + lattice.at((i+1)%L, j));
            }
        }

        //  Calculate initial magnetisation
        double M = sum(lattice, L, L);

        std::vector<double> E_vec(n_sweeps+1);     // These vectors will be the normalised E and M
        std::vector<double> M_vec(n_sweeps+1);
        std::vector<double> sweep_number(n_sweeps+1);

        E_vec.at(0) = E/(double)Ns;     // Divided by Ns to normalise
        M_vec.at(0) = M/(double)Ns;

        for(int j=0; j<n_sweeps; j++){
            for(int i=0; i<Ns; i++){
                // Set up random number generators
                int seed = rd();
                std::mt19937 mt_generator(seed);
                std::uniform_real_distribution<double> uniform_rn1(0, 1);
                std::uniform_real_distribution<double> uniform_rn2(0, L-1);
                std::uniform_real_distribution<double> uniform_rn3(0, L-1);

                // Choose a spin to flip
                int flip_x = uniform_rn2(mt_generator);
                int flip_y = uniform_rn3(mt_generator);
            
                // Flip the spin, and calculate the change in energy
                lattice.at(flip_x, flip_y)=-lattice.at(flip_x, flip_y);
                double delta_E = delta_Energy(lattice.at(flip_x, flip_y), lattice.at((flip_x-1+L)%L, flip_y), \
                    lattice.at((flip_x+1)%L, flip_y), lattice.at(flip_x, (flip_y+1)%L), lattice.at(flip_x, (flip_y-1+L)%L),\
                    mu, J, B);

                // Either accept or reject the proposed flip
                if(delta_E > 0){
                    // generate uniform random number 0<=r<1
                    double r = uniform_rn1(mt_generator);

                    double a = exp(-delta_E/(k*T));
                    if(r > a){
                        // Return the flipped spin to its original state (ie, unflip it)
                        lattice.at(flip_x, flip_y)=-lattice.at(flip_x, flip_y);
                    }
                    else{
                        // Update the energy and magnetisation
                        E = E + delta_E;
                        double delta_M = delta_Magnetisation(lattice.at(flip_x, flip_y));
                        M = M + delta_M;
                    }
                }
                else{
                    // Update the energy and magnetisation
                    E = E + delta_E;
                    double delta_M = delta_Magnetisation(lattice.at(flip_x, flip_y));
                    M = M + delta_M;
                }
            }

            E_vec.at(j+1) = E/(double)Ns;
            M_vec.at(j+1) = std::abs(M)/(double)Ns;

            sweep_number.at(j+1) = j+1;
        }

        // Burn-in
        E_vec.erase(E_vec.begin(), E_vec.begin()+burn_in);
        M_vec.erase(M_vec.begin(), M_vec.begin()+burn_in);


        // Calculate averages

        // Average energy
        E_av.at(l) = vector_sum(E_vec)/((double)E_vec.size());

        // Energy error
        double E_variance = variance(E_vec, E_av.at(l));
        E_error.at(l) = sqrt(E_variance/((double)Ns));

        // Average magnetisation
        M_av.at(l) = vector_sum(M_vec)/((double)M_vec.size());

        // Magnetisation error
        double M_variance = variance(M_vec, M_av.at(l));
        M_error.at(l) = sqrt(M_variance/((double)Ns));

        // Theoretical/analytic magnetisation
        if(k*T/J < 2/log(1+sqrt(2))){
            M_theory.at(l) = pow(1-pow(sinh(2*J/(k*T)), -4), 1/8);
        }
        else{
            M_theory.at(l) = 0;
        }

        // Specific heat per spin
        C.at(l) = E_variance*Ns/(k*pow(T, 2));

        // Magnetic susceptibility per spin
        X.at(l) = M_variance*Ns/(k*T);

        
        // Output final state of lattice to the terminal
        output_lattice(lattice, L);

    }

    std::vector<std::vector<double>> output_vectors = {T_vec, E_av, E_error, M_av, M_error, M_theory, C, X};
    output(output_vectors, T_vec.size(), 8, "Ising_2D_averages.txt");

}
