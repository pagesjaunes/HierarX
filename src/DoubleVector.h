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


#ifndef HIERARX_DOUBLEVECTOR_H
#define HIERARX_DOUBLEVECTOR_H

#include "HyperbolicVector.h"
#include "Random.h"

#define MIN_INIT -0.001
#define MAX_INIT 0.001

class DoubleVector : public HyperbolicVector {

public:
    using HyperbolicVector::HyperbolicVector;

    double dist(HyperbolicVector&) const override ; // return distance from this vector to argument vector
    void diffDist(HyperbolicVector*, std::pair<HyperbolicVector*, HyperbolicVector*>&, double) const override ;
    void expmap(HyperbolicVector&, HyperbolicVector*) const override ; // exponential map for gradient transport
    void transport(HyperbolicVector&, HyperbolicVector&, HyperbolicVector*) override ;
    void egrad2hgrad(HyperbolicVector*) const override; // clip gradient to poincare manifold
    void project() override; // insure that vector is in manifold
    HyperbolicVector* copy() const override;
    HyperbolicVector* buffer() const override;
    void randomize() override;


};


#endif //HIERARX_DOUBLEVECTOR_H
