#pragma once

#include "vector.hpp"
#include <cmath>

// Function to compute variance of a distribution of values
double variance(std::vector<double> V, double V_av){
    std::vector<double> V_squared(V.size());
    for(int i=0; i<V.size(); i++){
        V_squared.at(i) = pow(V.at(i), 2);
    }
    double variance = vector_sum(V_squared)/((double)V_squared.size()) - pow(V_av, 2);

    return variance;
}