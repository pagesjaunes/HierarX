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


#ifndef HIERARX_NUMERICALDIFFERENTIATE_H
#define HIERARX_NUMERICALDIFFERENTIATE_H

#include <vector>
#include <limits>
#include <functional>
#include <cmath>
#include "PoincareVector.h"
#include "EuclideanGeometry.h"

namespace constants {
    const double sqreps = std::sqrt(std::numeric_limits<double>::epsilon());
}

class NumericalDifferentiate {

public:
    static double adaptativeStep(double);
    static double diff(std::function<double (double)>, double);
    static double diff(double (*)(double), double);
    static std::vector<double> diff(double (*)(std::vector<double>*), std::vector<double>*);
    static std::vector<matrix> diff(std::function<void (matrix*, double*)>, matrix*, int);//std::vector<double> (*)(matrix*), matrix*);

};


#endif //HIERARX_NUMERICALDIFFERENTIATE_H
