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


#include "DoubleVector.h"

double DoubleVector::dist(HyperbolicVector& hvec) const {
    assert(hvec.dim == this->dim);
    double diff = 0;
    for (int i = 0; i < this->dim; i++) diff += std::pow(this->coordinates[i] - hvec.coordinates[i], 2);
    return std::sqrt(diff);
}

void DoubleVector::diffDist(HyperbolicVector* hvec, std::pair<HyperbolicVector*, HyperbolicVector*> & grads, double coeff) const {

    assert(this->dim == hvec->dim);

    double distance = this->dist(*hvec);
    double lambda = coeff / (2 * distance);

    for (int i = 0; i < this->dim;i++) {
        grads.first->coordinates[i] = 2 * lambda * this->coordinates[i] * (this->coordinates[i] - hvec->coordinates[i]);
        grads.second->coordinates[i] = -2 * lambda * hvec->coordinates[i] * (this->coordinates[i] - hvec->coordinates[i]);
    }

}

void DoubleVector::expmap(HyperbolicVector & dvec, HyperbolicVector * buffer) const {
    assert(dvec.dim == buffer->dim);
    assert(this->dim == buffer->dim);

    for (int i= 0; i < this->dim; i++)
        buffer->coordinates[i] = this->coordinates[i] + dvec.coordinates[i];
}

void DoubleVector::transport(HyperbolicVector& dvec, HyperbolicVector& vec, HyperbolicVector* buffer) {

}

void DoubleVector::egrad2hgrad(HyperbolicVector *) const {

}

void DoubleVector::project() {

}

HyperbolicVector *DoubleVector::copy() const {
    return nullptr;
}

HyperbolicVector *DoubleVector::buffer() const {
    return nullptr;
}

void DoubleVector::randomize() {

    std::vector<double> rsample = Random::DoubleVector(MIN_INIT, MAX_INIT, this->dim);
    for (int i = 0; i < this->dim; i++) {
        this->coordinates[i] = rsample.at(i);
    }

}
