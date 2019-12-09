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

#ifndef HIERARX_RSGD_H
#define HIERARX_RSGD_H

#include <set>
#include <vector>
#include "EuclideanGeometry.h"
#include "HyperbolicVector.h"
#include "Args.h"

class RSGD {

private:
    double lr;
    double _wantedlr;
    long countStep;
    long plateau;
    double momentum;
    double minlr;
    bool nesterov;
    bool declr;
    int total;
    int offset;
    std::vector<HyperbolicVector*>* parameters;
    std::vector<HyperbolicVector*>* momentumGradients;
    std::vector<HyperbolicVector*>* buffers;
    std::vector<int> counts;

public:

    RSGD(
            const Args*,
            std::vector<HyperbolicVector*>*,
            std::vector<HyperbolicVector*>*,
            std::vector<HyperbolicVector*>*
            );

    void updateLR();
    void step(std::vector<int>*, std::vector<HyperbolicVector*>*, int);
    void step(std::set<int>*, std::vector<HyperbolicVector*>*, int);
    double getLr();
    int sumCounts();
};

#endif //HIERARX_RSGD_H
