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


#include "HyperbolicVector.h"
#include <iostream>

HyperbolicVector::HyperbolicVector() {

    this->dim = -1;
    this->coordinates = NULL;

}


HyperbolicVector::HyperbolicVector(int dimension) {
    this->dim = dimension;
    this->coordinates = new double[dimension];
}


HyperbolicVector::HyperbolicVector(int dimension, double constant_value) {
    this->dim = dimension;
    this->coordinates = new double[dimension];
    for (int i = 0; i < this->dim; i++) *(this->coordinates + i) = constant_value;
}


HyperbolicVector::HyperbolicVector(int dimension, std::vector<double>* values) {
    assert(dimension == values->size());
    this->dim = dimension;
    this->coordinates = new double[dimension];
    this->setTo(values);
}

HyperbolicVector::HyperbolicVector(int dimension, std::vector<double> values) {
    assert(dimension == values.size());
    this->dim = dimension;
    this->coordinates = new double[dimension];
    this->setTo(&values);
}


HyperbolicVector::HyperbolicVector(int dimension, double* values) {

    this->dim = dimension;
    this->coordinates = values;

}


HyperbolicVector::HyperbolicVector(const HyperbolicVector& pvec) {

    this->dim = pvec.dim;
    this->coordinates = new double[this->dim];
    for (int i = 0; i < this->dim; i++) *(this->coordinates + i) = *(pvec.coordinates + i);

}


double HyperbolicVector::operator[](int i) const {
    return *(this->coordinates + i);
}

double HyperbolicVector::at(int i) const {
    return *(this->coordinates + i);
}

void HyperbolicVector::setTo(std::vector<double>* values) {
    assert(this->dim == values->size());
    for (int i = 0; i < this->dim; i++) *(this->coordinates + i) = values->at(i);
}

int HyperbolicVector::getDimension() const{
    return this->dim;
}

void HyperbolicVector::zeros() {
    for (int i = 0; i < this->dim; i++) *(this->coordinates + i) = 0;
    this->project();
}

double HyperbolicVector::eucnorm() const {
    return EuclideanGeometry::norm(this->coordinates, this->dim);
}

std::vector<double> HyperbolicVector::get_coordinates() const {
    std::vector<double> coords(this->dim);
    for (int i = 0; i < this->dim; i++) coords[i] = this->at(i);
    return coords;
}

void HyperbolicVector::diffDist(HyperbolicVector* pvector, std::pair<HyperbolicVector*, HyperbolicVector*>& gradslist) const {
    return this->diffDist(pvector, gradslist, 1.0);
}

void HyperbolicVector::eucadd(const HyperbolicVector& pvector, HyperbolicVector* buffer) {
    assert(this->dim == pvector.dim && this->dim == buffer->dim);
    for(int i = 0; i < this->dim; i++) buffer->coordinates[i] = this->at(i) + pvector.at(i);
}

void HyperbolicVector::eucadd(const std::vector<double>& vec, HyperbolicVector* buffer) {
    assert(this->dim == vec.size() && this->dim == buffer->dim);
    for(int i = 0; i < this->dim; i++) buffer->coordinates[i] = this->at(i) + vec.at(i);
}

void HyperbolicVector::eucmul(double el, HyperbolicVector* buffer) {
    assert(this->dim == buffer->dim);
    for(int i = 0; i < this->dim; i++) buffer->coordinates[i] = this->at(i) * el;
}

void HyperbolicVector::write(std::ofstream& ofs) {

    ofs.write((char*) this->coordinates, sizeof(double) * this->dim);

}

std::string HyperbolicVector::stringify() {

    std::ostringstream stream;
    stream.precision(DOUBLE_SIZE);
    for (int j = 0; j < this->dim; j++) {
        stream << std::fixed << this->at(j)  << " ";
    }
    return stream.str();
}

HyperbolicVector::~HyperbolicVector() {

    delete[] this->coordinates;

}

