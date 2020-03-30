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


#ifndef HIERARX_POINCAREVECTOR_H
#define HIERARX_POINCAREVECTOR_H

#define MIN_VALUE 1e-6
#define EPSILON 1e-2
#define MIN_INIT_POINCARE -0.01
#define MAX_INIT_POINCARE 0.01

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <utility>
#include <functional>
#include <cstdlib>
#include "EuclideanGeometry.h"
#include "NumericalDifferentiate.h"
#include "HyperbolicVector.h"
#include "Random.h"



class PoincareVector : public HyperbolicVector {

public:

    class PoincareManifold {

    private:
        double celerity;
        double sqrt_celerity;
        int dim;
        double MIN_NORM = MIN_VALUE;
        bool lorentzian_distance;

    public:

        PoincareManifold();
        PoincareManifold(double, int, bool);

        void mobiusAdd(const double*, const double*, double*) const;
        void mobiusDot(const double, const double*, double*) const;
        void diffHelp(matrix*, double*);
        double lambda_x(const double*) const;
        double dist(const double*, const double*);

        double getCelerity();
        double getSqrtCelerity();
        int getDim();
        bool isLorentzian();


    };

    PoincareManifold* manifold;

    PoincareVector();
    PoincareVector(PoincareManifold*, int);
    PoincareVector(PoincareManifold*, int, double); // Construct and set vector coordinates to double value
    PoincareVector(PoincareManifold*, int, std::vector<double>*); // Construct and set vector coordinates to specified vector
    PoincareVector(PoincareManifold*, int, std::vector<double>);
    PoincareVector(PoincareManifold*, int, double*);
    PoincareVector(const HyperbolicVector&);

    PoincareVector operator-() const; // set coordinates to their opposite
    PoincareVector operator/(double) const;
    bool operator==(const PoincareVector&) const;

    std::string metaInfo() const override;

    double dist(HyperbolicVector&) const override ; // return distance from this vector to argument vector
    void diffDist(HyperbolicVector*, std::pair<HyperbolicVector*, HyperbolicVector*>&, double) const override ;
    void expmap(HyperbolicVector&, HyperbolicVector*) const override ; // exponential map for gradient transport
    void transport(HyperbolicVector&, HyperbolicVector&, HyperbolicVector*) override ;
    void egrad2hgrad(HyperbolicVector*) const override; // clip gradient to poincare manifold
    void project() override; // insure that vector is in manifold
    HyperbolicVector* copy() const override;
    HyperbolicVector* buffer() const override;
    void randomize() override;

    double lambda_x() const;
    void project(double);
    double eucnormsquare() const;

    const double MIN_NORM = MIN_VALUE;
    const double STABILITY = EPSILON;

};

PoincareVector operator*(double, const PoincareVector&);
void gyration(PoincareVector*, PoincareVector*, PoincareVector*, HyperbolicVector*);
void parallelTransport(PoincareVector*, PoincareVector*, PoincareVector*, HyperbolicVector*);

#endif //HIERARX_POINCAREVECTOR_H
