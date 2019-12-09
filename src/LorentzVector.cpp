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


#include "LorentzVector.h"
#include <iostream>


LorentzVector::LorentzVector() : HyperbolicVector() {
}

LorentzVector::LorentzVector(double c, int dim, double val) : HyperbolicVector(dim + 1 , val){
    this->celerity = c;
}

LorentzVector::LorentzVector(double c, int dim) : HyperbolicVector(dim + 1) {
    this->celerity = c;
}

LorentzVector::LorentzVector(const LorentzVector& lvec) : HyperbolicVector(lvec.dim) {
    this->celerity = lvec.celerity;
    double c = this->celerity;
}

LorentzVector::LorentzVector(double c, int dim, std::vector<double> coords) : HyperbolicVector(dim + 1, coords){
    this->celerity = c;
}

LorentzVector::~LorentzVector() {

}

double LorentzVector::dist(HyperbolicVector& lvec) const {

    return std::acosh(-this->innerProduct(lvec));
}

void LorentzVector::diffDist(HyperbolicVector* hvector, std::pair<HyperbolicVector*, HyperbolicVector*>& gradslist, double coeff) const {

    assert(this->dim == gradslist.first->dim);
    assert(this->dim == gradslist.second->dim);
    assert(hvector->dim == this->dim);

    double inprod = this->innerProduct(*hvector);
    double lambda = - coeff / std::sqrt(inprod * inprod - 1);

    gradslist.first->coordinates[0] += -lambda * hvector->coordinates[0];
    gradslist.second->coordinates[0] += -lambda * this->coordinates[0];

    for (int i = 1; i < this->dim; i++) {
        gradslist.first->coordinates[i] += lambda * this->celerity * hvector->coordinates[i];
        gradslist.second->coordinates[i] += lambda * this->celerity * this->coordinates[i];
    }

}

void LorentzVector::expmap(HyperbolicVector& lvector, HyperbolicVector* buffer) const {

    assert(this->dim == lvector.dim);
    assert(buffer->dim == lvector.dim);

    double lvectorNorm = this->celerity * EuclideanGeometry::dot(lvector.coordinates + 1, lvector.coordinates + 1, lvector.dim - 1);
    lvectorNorm += - lvector.coordinates[0] * lvector.coordinates[0];
    lvectorNorm = std::sqrt(std::fmax(1e-16, lvectorNorm));


    for (int i = 1; i < this->dim; i++){
        buffer->coordinates[i] = std::cosh(lvectorNorm) * this->coordinates[i] + std::sinh(lvectorNorm) * lvector.coordinates[i] / lvectorNorm;

    }
    buffer->coordinates[0] = std::sqrt(1 + this->celerity * EuclideanGeometry::dot(buffer->coordinates + 1, buffer->coordinates + 1, buffer->dim - 1));
}

void LorentzVector::transport(HyperbolicVector &, HyperbolicVector &, HyperbolicVector *) {

}

void LorentzVector::egrad2hgrad(HyperbolicVector* lvector) const {

    assert(this->dim == lvector->dim);
    lvector->coordinates[0] = -lvector->coordinates[0];
    for (int i = 1; i < this->dim; i++) lvector->coordinates[i] /= this->celerity;
    double inprod = this->innerProduct(*lvector);
    for (int i = 0; i < this->dim; i++) {
        lvector->coordinates[i] += inprod * this->coordinates[i];
    }

}

void LorentzVector::project() {
    //this->coordinates[0] = std::sqrt(1 + EuclideanGeometry::norm(this->coordinates + 1, this->dim - 1));
}

HyperbolicVector* LorentzVector::copy() const {
    return nullptr;
}

HyperbolicVector* LorentzVector::buffer() const {
    return nullptr;
}

void LorentzVector::randomize() {
    std::vector<double> rsample = Random::DoubleVector(MIN_INIT, MAX_INIT, this->dim - 1);
    for (int i = 1; i < this->dim; i++) {
        this->coordinates[i] = rsample.at(i - 1);
    }
    this->clip();
}

double LorentzVector::innerProduct(HyperbolicVector& lvec) const {
    assert(lvec.dim == this->dim);
    double prod = EuclideanGeometry::dot(this->coordinates + 1, lvec.coordinates + 1, this->dim - 1);
    return this->celerity * prod - this->coordinates[0] * lvec.coordinates[0];
}

void LorentzVector::clip() {
    this->coordinates[0] = std::sqrt(1 + this->celerity * EuclideanGeometry::dot(this->coordinates + 1, this->coordinates + 1, this->dim - 1));
}

double LorentzVector::getCelerity() const {
    return this->celerity;
}

std::string LorentzVector::metaInfo() const {
    return " c " + std::to_string(this->celerity);
}