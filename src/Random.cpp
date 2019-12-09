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


#include "Random.h"

int Random::NaturalInteger(int max) {

    double idx = std::floor(Double(max));
    if (idx >= max) {
        return Random::NaturalInteger(max);
    } else {
        return idx;
    }
}

double Random::Double(double max) {
    return Double(0, max);
}

double Random::Double(double min, double max) {
    assert(min < max);
    return min + ((double) std::rand()) / RAND_MAX * (max - min);
}


std::vector<double> Random::DoubleVector(double min, double max, int dim) {
    assert(min < max);
    assert(dim > 0);
    std::vector<double> rsample = std::vector<double>(dim);
    for (int i = 0; i < dim; i++) rsample[i] = Double(min, max);
    return rsample;
}

