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


#ifndef HIERARX_HYPERBOLICVECTOR_H
#define HIERARX_HYPERBOLICVECTOR_H

#include <vector>
#include <fstream>
#include "EuclideanGeometry.h"

class HyperbolicVector {

public:

    int dim;
    double* coordinates;

    HyperbolicVector();
    HyperbolicVector(int);
    HyperbolicVector(int, double); // Construct and set vector coordinates to double value
    HyperbolicVector(int, std::vector<double>*); // Construct and set vector coordinates to specified vector
    HyperbolicVector(int, std::vector<double>);
    HyperbolicVector(int, double*);
    HyperbolicVector(const HyperbolicVector&);
    virtual ~HyperbolicVector();

    double operator[](int i) const;
    double at(int i) const;
    int getDimension() const; // return dimension of vector
    void setTo(std::vector<double>*); // set coordinates to specified vector
    void zeros();
    double eucnorm() const;
    std::vector<double> get_coordinates() const;
    void diffDist(HyperbolicVector*, std::pair<HyperbolicVector*, HyperbolicVector*>&) const;
    void eucadd(const HyperbolicVector&, HyperbolicVector*); // euclidean addition
    void eucadd(const std::vector<double>&, HyperbolicVector*); // euclidean addition
    void eucmul(double, HyperbolicVector*); // euclidean scalar multiplication
    void write(std::ofstream&);
    std::string stringify();

    // virtual
    virtual inline std::string metaInfo() const {return "";};


    // Pure virtual
    virtual double dist(HyperbolicVector&) const = 0;
    virtual void diffDist(HyperbolicVector*, std::pair<HyperbolicVector*, HyperbolicVector*>&, double) const = 0;
    virtual void expmap(HyperbolicVector&, HyperbolicVector*) const = 0; // exponential map for gradient transport
    virtual void transport(HyperbolicVector&, HyperbolicVector&, HyperbolicVector*) = 0;
    virtual void egrad2hgrad(HyperbolicVector*) const = 0;
    virtual void project() = 0;
    virtual HyperbolicVector* copy() const = 0;
    virtual HyperbolicVector* buffer() const = 0;
    virtual void randomize() = 0;



};

#endif //HIERARX_HYPERBOLICVECTOR_H
