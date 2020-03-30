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


#include "RSGD.h"
#include <iostream>

RSGD::RSGD(
        const Args* args,
        std::vector<HyperbolicVector*>* parameters,
        std::vector<HyperbolicVector*>* momentumGradients,
        std::vector<HyperbolicVector*>* buffers,
        std::vector<std::mutex>* lockers
    ) {

    assert(parameters->size() == momentumGradients->size());
    this->lr = 1e-1 * args->lr;
    this->momentum = args->momentum;
    this->nesterov = args->nesterov;
    this->parameters = parameters;
    this->momentumGradients = momentumGradients;
    for (int i = 0; i < momentumGradients->size(); i++) this->momentumGradients->at(i)->zeros();
    this->countStep = 0;
    this->plateau = (int) (args->plateau * args->niter);
    this->_wantedlr = args->lr;
    this->buffers = buffers;
    this->counts = std::vector<int>(buffers->size(), 0);
    this->declr = args->declr;
    this->offset = (int) (args->plateau * ((double) args->niter));
    this->total = args->niter - offset;
    this->minlr = args->minlr;

    this->lockers = lockers;
};

void RSGD::updateLR() {
    if (this->countStep > this->plateau && this->lr < this->_wantedlr) {
        this->lr = this->_wantedlr;
    }
}

void RSGD::step(std::vector<int>* indexes, std::vector<HyperbolicVector*>* gradients, int idx) {

    this->updateLR();

    this->countStep += 1;
    this->counts[idx] += 1;

    for (std::vector<int>::iterator it = indexes->begin(); it != indexes->end(); it++) {

        this->lockers->at(*it).lock();
        this->parameters->at(*it)->egrad2hgrad(gradients->at(*it));

        gradients->at(*it)->project();
        this->momentumGradients->at(*it)->eucmul(this->momentum, this->momentumGradients->at(*it));
        this->momentumGradients->at(*it)->eucadd(*(gradients->at(*it)), this->momentumGradients->at(*it));

        if (this->nesterov) {
            this->momentumGradients->at(*it)->eucmul(this->momentum, this->momentumGradients->at(*it));
            gradients->at(*it)->eucadd(*(this->momentumGradients->at(*it)), gradients->at(*it));
        } else {
            this->momentumGradients->at(*it)->eucmul(1.0, gradients->at(*it));
        }

        gradients->at(*it)->project();
        gradients->at(*it)->eucmul(-this->getLr(), gradients->at(*it));

        this->buffers->at(idx)->eucadd(*(this->parameters->at(*it)), this->buffers->at(idx));
        this->parameters->at(*it)->expmap(*(gradients->at(*it)), this->parameters->at(*it));

        this->parameters->at(*it)->project();
        this->buffers->at(idx)->transport(*(gradients->at(*it)), *(this->parameters->at(*it)),
                                          this->momentumGradients->at(*it));

        this->parameters->at(*it)->project();
        this->momentumGradients->at(*it)->project();
        gradients->at(*it)->zeros();

        this->buffers->at(idx)->zeros();
        this->lockers->at(*it).unlock();

    }
};

void RSGD::step(std::set<int>* indexes, std::vector<HyperbolicVector*>* gradients, int idx) {


    this->updateLR();

    this->countStep += 1;
    this->counts[idx] += 1;

    for (std::set<int>::iterator it = indexes->begin(); it != indexes->end(); it++) {

        this->lockers->at(*it).lock();
        this->parameters->at(*it)->egrad2hgrad(gradients->at(*it));
        gradients->at(*it)->project();
        this->momentumGradients->at(*it)->eucmul(this->momentum, this->momentumGradients->at(*it));
        this->momentumGradients->at(*it)->eucadd(*(gradients->at(*it)),  this->momentumGradients->at(*it));

        if (this->nesterov) {
            this->momentumGradients->at(*it)->eucmul(this->momentum, this->momentumGradients->at(*it));
            gradients->at(*it)->eucadd(*(this->momentumGradients->at(*it)), gradients->at(*it));
        } else {
            this->momentumGradients->at(*it)->eucmul(1.0, gradients->at(*it));
        }

        gradients->at(*it)->eucmul(-this->getLr(), gradients->at(*it));
        gradients->at(*it)->project();

        this->buffers->at(idx)->eucadd(*(this->parameters->at(*it)), this->buffers->at(idx));
        this->parameters->at(*it)->expmap(*(gradients->at(*it)), this->parameters->at(*it));
        this->buffers->at(idx)->transport(*(gradients->at(*it)), *(this->parameters->at(*it)), this->momentumGradients->at(*it));

        this->parameters->at(*it)->project();
        this->momentumGradients->at(*it)->project();
        gradients->at(*it)->zeros();

        this->buffers->at(idx)->zeros();
        this->lockers->at(*it).unlock();

    }

}

double RSGD::getLr() {
    if (this->declr && this->lr == this->_wantedlr) {
        return std::fmax(this->lr * (1 - ((double) this->sumCounts()) / ((double) this->total)), this->minlr);
    } else {
        return this->lr;
    }
}

int RSGD::sumCounts() {
    int val = 0;
    for (int i = 0; i < this->counts.size(); i++) {
        val += this->counts[i];
    }
    return val - offset;
};