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


#include "NumericalDifferentiate.h"

double NumericalDifferentiate::diff(std::function<double (double)> func, double x0) {
    double step = adaptativeStep(x0);
    return (func(x0 + step) - func(x0 - step)) / (2 * step);
}

double NumericalDifferentiate::diff(double (*func)(double), double x0) {
    double step = adaptativeStep(x0);
    return (func(x0 + step) - func(x0 - step)) / (2 * step);
}


std::vector<double> NumericalDifferentiate::diff(
        double (*func)(std::vector<double>*),
        std::vector<double>* vec
        ) {
    std::vector<double>* upper = new std::vector<double>(vec->size());
    std::vector<double>* downer = new std::vector<double>(vec->size());
    std::vector<double> grad = std::vector<double>(vec->size());
    for (int i = 0; i < vec->size(); i++) {
        double x0 = vec->at(i);
        double step = adaptativeStep(x0);
        upper->at(i) = x0 + step;
        downer->at(i) = x0 - step;
        grad[i] = (func(upper) - func(downer)) / (2 * step);
        upper->at(i) = x0;
        downer->at(i) = x0;
    }

    delete upper;
    delete downer;
    return grad;
}

std::vector<matrix> NumericalDifferentiate::diff(
        std::function<void (matrix*, double*)> func,
        matrix* mat,
        int indim
        ) {

    std::vector<double> upperEval(indim);
    std::vector<double> downerEval(indim);
    matrix* upper = new matrix(mat->size(), std::vector<double>(mat->at(0).size()));
    matrix* downer = new matrix(mat->size(), std::vector<double>(mat->at(0).size()));
    for (int i = 0; i < mat->size(); i++) {
        for (int j = 0; j < mat->at(0).size(); j++) {
            upper->at(i)[j] = mat->at(i)[j];
            downer->at(i)[j] = mat->at(i)[j];
        }
    }
    std::vector<matrix> grad = std::vector<matrix>(
            indim,
            matrix(
                    mat->size(),
                    std::vector<double> (mat->at(0).size(), 0)
                    )
            );

    for (int i = 0; i < mat->size(); i++) {
        for (int j = 0; j < mat->at(i).size(); j++) {
            double x0 = mat->at(i)[j];
            double step = adaptativeStep(x0);
            upper->at(i)[j] = x0 + step;
            downer->at(i)[j] = x0 - step;
            func(upper, &upperEval[0]);
            func(downer, &downerEval[0]);
            for (int k = 0; k < upperEval.size(); k++) {
                grad[k][i][j] = (upperEval[k] - downerEval[k]) / (2 * step);
            }
            upper->at(i)[j] = x0;
            downer->at(i)[j] = x0;
        }
    }

    delete upper;
    delete downer;


    return grad;
}

double NumericalDifferentiate::adaptativeStep(double x0) {
    return x0 == 0 ? constants::sqreps : constants::sqreps * x0;
}
