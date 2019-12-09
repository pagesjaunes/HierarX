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


#ifndef HIERARX_EUCLIDEANGEOMETRY_H
#define HIERARX_EUCLIDEANGEOMETRY_H

#include <vector>
#include <cmath>
#include <cassert>
#include <functional>

typedef std::vector<std::vector<double>> matrix;

class EuclideanGeometry {

public:
    static inline double square(double x) {return x * x;};
    static inline double product(double x, double y) {return x * y;};

    static double norm(std::vector<double>*); // Euclidean Norm
    static double norm(const double*, int); // Euclidean Norm
    static double normsquare(std::vector<double>*);
    static double normsquare(const double*, int);
    static double dot(std::vector<double>*, std::vector<double>*); // Euclidean dot product
    static double dot(const double*, const double*, int);
    static double dot(const double*, const double*, int, std::function<double (double)>);
    static std::vector<double>* add(std::vector<double>*, std::vector<double>*);
    static std::vector<double> elementWiseProduct(std::vector<double>*, std::vector<double>*);
    static void scalMult(double, std::vector<double>*);
    static matrix tensMult(std::vector<double>*, std::vector<matrix>*);

    static double sum(const std::vector<double>*);
    static int sum(const std::vector<int>*);
    static double sumWithFunction(const std::vector<double>*, double (*)(double));
    static double sumElementWiseFunction(const std::vector<double>*, const std::vector<double>*, double (*) (double, double));
};

std::vector<double> operator*(double, std::vector<double>);
matrix operator*(std::vector<double>, std::vector<matrix>);

#endif //HIERARX_EUCLIDEANGEOMETRY_H
