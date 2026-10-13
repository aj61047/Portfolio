#pragma once

#include <vector>

// Operator overload to add two vectors together in the normal way
std::vector<double> operator+(std::vector<double> v1, std::vector<double> v2){
    int n=v1.size();
    std::vector<double> result(n);
    for(int i=0; i<n; i++){
        result.at(i) = v1.at(i) + v2.at(i);
    }
    return result;
}

// Operator overload to multiply a vector by a scalar
std::vector<double> operator*(std::vector<double> v, double s){
    int n=v.size();
    std::vector<double> result(n);
    for(int i=0; i<n; i++){
        result.at(i) = v.at(i)*s;
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