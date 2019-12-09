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


#include "PoincareStack.h"

PoincareStack::PoincareStack() : HyperbolicVector() {
    this->hvectors = NULL;
    this->spaces = -1;
}

PoincareStack::PoincareStack(int spaces) : HyperbolicVector (PoincareStack::SUBSPACE_DIM::COMMON * spaces), spaces(spaces) {
    this->hvectors = new std::vector<PoincareVector*>(spaces);
    this->buffers = new std::vector<std::pair<HyperbolicVector*, HyperbolicVector*>>(spaces);
    this->initVectors();
}

PoincareStack::PoincareStack(int spaces, double constant_value) :
    HyperbolicVector(PoincareStack::SUBSPACE_DIM::COMMON * spaces, constant_value),
    spaces(spaces) {
    this->hvectors = new std::vector<PoincareVector*>(spaces);
    this->buffers = new std::vector<std::pair<HyperbolicVector*, HyperbolicVector*>>(spaces);
    this->initVectors();
}

PoincareStack::PoincareStack(const PoincareStack& hvec) : HyperbolicVector(PoincareStack::SUBSPACE_DIM::COMMON * hvec.spaces) {

    this->spaces = hvec.spaces;
    this->hvectors = new std::vector<PoincareVector*>(spaces);
    this->buffers = new std::vector<std::pair<HyperbolicVector*, HyperbolicVector*>>(spaces);
    this->initVectors();

    for (int i = 0; i < this->spaces; i++) {
        for (int j = 0; j < this->hvectors->at(i)->dim; j++) {
            this->hvectors->at(i)->coordinates[j] = hvec.hvectors->at(i)->at(j);
        }
    }
}


PoincareStack::PoincareStack(int dim, int subspaceDim, std::vector<double>& coords) :
    HyperbolicVector(subspaceDim * dim, &coords),
    spaces(dim) {

    this->hvectors = new std::vector<PoincareVector*>(spaces);
    this->buffers = new std::vector<std::pair<HyperbolicVector*, HyperbolicVector*>>(spaces);
    this->initVectors(subspaceDim);

}

void PoincareStack::initVectors() {
    this->initVectors(PoincareStack::SUBSPACE_DIM::COMMON);
}

void PoincareStack::initVectors(int subspaceDim) {
    PoincareVector::PoincareManifold* mf = new PoincareVector::PoincareManifold(1.0, subspaceDim);
    for (int i = 0; i < this->spaces; i++) {
        this->hvectors->at(i) = new PoincareVector(mf, subspaceDim, this->coordinates + subspaceDim * i);
        this->buffers->at(i) = std::pair<HyperbolicVector *, HyperbolicVector *>(
                this->hvectors->at(i)->buffer(),
                this->hvectors->at(i)->buffer()
                );
    }
}


void PoincareStack::spaceAdd(HyperbolicVector* hvec, int spaceIdx) {

    assert(spaceIdx < this->spaces);
    this->hvectors->at(spaceIdx)->eucadd(*hvec, this->hvectors->at(spaceIdx));

}

void PoincareStack::randomize() {
    for (int i = 0; i < this->spaces; i++) this->hvectors->at(i)->randomize();
}

double PoincareStack::dist(HyperbolicVector& pstack) const {
    PoincareStack& stackvec = dynamic_cast<PoincareStack&>(pstack);
    assert(this->spaces == stackvec.spaces);

    double distance = 0;
    for (int i = 0; i < this->spaces; i++)
        distance += std::pow(this->hvectors->at(i)->dist(*(stackvec.hvectors->at(i))), 2.0);
    return std::sqrt(distance);
}

void PoincareStack::diffDist(HyperbolicVector* pstack, std::pair<HyperbolicVector*, HyperbolicVector*>& gradslist, double coeff) const {

    PoincareStack& stackvec = dynamic_cast<PoincareStack&>(*pstack);
    PoincareStack& gradsfirst = dynamic_cast<PoincareStack&>(*(gradslist.first));
    PoincareStack& gradssecond = dynamic_cast<PoincareStack&>(*(gradslist.second));
    assert(this->spaces == stackvec.spaces);
    assert(stackvec.spaces == gradsfirst.spaces);
    assert(gradsfirst.spaces == gradssecond.spaces);

    double globalCoeff = coeff / (2 * this->dist(*pstack));
    double spacedist;
    for (int i = 0; i < this->spaces; i++) {
        spacedist = this->hvectors->at(i)->dist(*(stackvec.hvectors->at(i)));

        this->hvectors->at(i)->diffDist(stackvec.hvectors->at(i), this->buffers->at(i), globalCoeff * 2 * spacedist);

        gradsfirst.spaceAdd(this->buffers->at(i).first, i);
        gradssecond.spaceAdd(this->buffers->at(i).second, i);

        this->buffers->at(i).first->zeros();
        this->buffers->at(i).second->zeros();

    }

}

void PoincareStack::expmap(HyperbolicVector& hvec, HyperbolicVector* buffer) const {
    PoincareStack& stackvec = dynamic_cast<PoincareStack&>(hvec);
    PoincareStack& stackbuffer = dynamic_cast<PoincareStack&>(*buffer);
    for (int i = 0; i < this->spaces; i++)
        this->hvectors->at(i)->expmap(*(stackvec.hvectors->at(i)), stackbuffer.hvectors->at(i));
}

void PoincareStack::transport(HyperbolicVector& grad, HyperbolicVector& hvec, HyperbolicVector* buffer) {
    PoincareStack& stackgrad = dynamic_cast<PoincareStack&>(grad);
    PoincareStack& stackvec = dynamic_cast<PoincareStack&>(hvec);
    PoincareStack& stackbuffer = dynamic_cast<PoincareStack&>(*buffer);
    for (int i = 0; i < this->spaces; i++)
        this->hvectors->at(i)->transport(*(stackgrad.hvectors->at(i)), *(stackvec.hvectors->at(i)), stackbuffer.hvectors->at(i));
}

void PoincareStack::egrad2hgrad(HyperbolicVector* hvec) const {
    PoincareStack& stackvec = dynamic_cast<PoincareStack&>(*hvec);
    for (int i = 0; i < this->spaces; i++)
        this->hvectors->at(i)->egrad2hgrad(stackvec.hvectors->at(i));
}

void PoincareStack::project() {
    for (int i = 0; i < this->spaces; i++) this->hvectors->at(i)->project();
}

HyperbolicVector* PoincareStack::copy() const {
    return new PoincareStack(*this);
}

HyperbolicVector* PoincareStack::buffer() const {
    return new PoincareStack(this->spaces, (double) 0);
}

std::string PoincareStack::metaInfo() const {
    return " x " + std::to_string(this->hvectors->at(0)->getDimension());
}


PoincareStack::~PoincareStack() {

    delete this->hvectors;
    delete this->buffers;

}
