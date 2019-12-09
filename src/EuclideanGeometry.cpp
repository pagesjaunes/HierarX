/*
 * HierarX is a software aiming to build hyperbolic word representations.
 * Copyright (C) 2019 Solocal-SA and CNRS.
 *
 * DO NOT REMOVE THIS HEADER EVEN AFTER MODIFYING HIERARX SOURCES.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * HierarX was developped by: François Torregrossa, Guillaume Gravier, Vincent Claveau, Nihel Kooli.
 * Contact: ftorregrossa@solocal.com, francois.torregrossa@irisa.fr
 */

#include "EuclideanGeometry.h"

double EuclideanGeometry::normsquare(std::vector<double>* vec) {
    return dot(vec, vec);
}

double EuclideanGeometry::normsquare(const double* vec, int dim) {
    return dot(vec, vec, dim);
}


double EuclideanGeometry::norm(const double* vec, int dim) {
    return std::sqrt(normsquare(vec, dim));
}



double EuclideanGeometry::norm(std::vector<double>* vec) {
    return std::sqrt(normsquare(vec));
}


double EuclideanGeometry::dot(const double* vecA, const double* vecB, int dim) {
    double dotValue = 0;
    for(int i = 0; i < dim; i++) dotValue += *(vecA + i) * *(vecB + i);
    return dotValue;
}


double EuclideanGeometry::dot(std::vector<double>* vecA, std::vector<double>* vecB) {
    assert(vecA->size() == vecB->size());
    double dotValue = 0;
    for(int i = 0; i < vecA->size(); i++) dotValue += vecA->at(i) * vecB->at(i);
    return dotValue;
}

std::vector<double>* EuclideanGeometry::add(std::vector<double>* vecA, std::vector<double>* vecB) {
    assert(vecA->size() == vecB->size());
    std::vector<double>* addVector = new std::vector<double>();
    for(int i = 0; i < vecA->size(); i++) addVector->push_back(vecA->at(i) + vecB->at(i));
    return addVector;
}

std::vector<double> EuclideanGeometry::elementWiseProduct(std::vector<double>* vecA, std::vector<double>* vecB) {
    assert(vecA->size() == vecB->size());
    std::vector<double> elVector = std::vector<double>(vecA->size());
    for(int i = 0; i < vecA->size(); i++) elVector[i] = vecA->at(i) * vecB->at(i);
    return elVector;
}

void EuclideanGeometry::scalMult(double lambda, std::vector<double>* vec) {
    for (int i = 0; i < vec->size(); i++) vec->at(i) *= lambda;
}

matrix EuclideanGeometry::tensMult(std::vector<double>* vec, std::vector<matrix>* tensor) {
    assert(vec->size() == tensor->size());
    matrix res = matrix(
            tensor->at(0).size(),
            std::vector<double>(tensor->at(0)[0].size(), 0)
            );
    for (int i = 0; i < tensor->at(0).size(); i++) {
        for (int j = 0; j < tensor->at(0)[0].size(); j++) {
            for (int k = 0; k < vec->size(); k++) {
                res[i][j] += vec->at(k) * tensor->at(k)[i][j];
            }
        }
    }
    return res;
}

double EuclideanGeometry::sum(const std::vector<double>* vec) {
    double sUm = 0;
    for (int i = 0; i < vec->size() ; i++) sUm += vec->at(i);
    return sUm;
}

double EuclideanGeometry::sumWithFunction(const ::std::vector<double>* vec, double (*func)(double)) {
    double sUm = 0;
    for (int i = 0; i < vec->size(); i++) sUm += func(vec->at(i));
    return sUm;
}

double EuclideanGeometry::sumElementWiseFunction(const std::vector<double>* vecA, const std::vector<double>* vecB,
                                                 double (*func)(double, double)) {
    assert(vecA->size() == vecB->size());
    double sUm = 0;
    for (int i = 0; i < vecA->size(); i++) sUm += func(vecA->at(i), vecB->at(i));
    return sUm;
}

int EuclideanGeometry::sum(const std::vector<int>* vec) {
    int count = 0;
    for (int i = 0; i < vec->size(); i++ ) count += vec->at(i);
    return count;
}

double EuclideanGeometry::dot(const double* x, const double* y, int dim, std::function<double (double)> func) {
    double dotValue = 0;
    for(int i = 0; i < dim; i++) {
        dotValue += func(x[i] * y[i]);
    }
    return dotValue;
}

std::vector<double> operator*(double lambda, std::vector<double> vec) {
    std::vector<double> res = std::vector<double>(vec);
    for (int i = 0; i < vec.size(); i++) res[i] *= lambda;
    return res;
}

matrix operator*(std::vector<double> vec, std::vector<matrix> tensor) {
    assert(vec.size() == tensor.size());
    matrix res = matrix(
            tensor[0].size(),
            std::vector<double>(tensor[0][0].size(), 0)
            );
    for (int i = 0; i < tensor[0].size(); i++) {
        for (int j = 0; j < tensor[0][0].size(); j++) {
            for (int k = 0; k < vec.size(); k++) {
                res[i][j] += vec[k] * tensor[k][i][j];
            }
        }
    }
    return res;
}
