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


#ifndef HIERARX_HIERARCHY_H
#define HIERARX_HIERARCHY_H


#include <thread>
#include <algorithm>
#include "HyperbolicEmbedding.h"
#include "EuclideanGeometry.h"

class Hierarchy {

private:

    struct Node {
        int idx;
        double pdistance;
        double normdiff;
    };

    HyperbolicEmbedding* pemb;
    std::vector<Node> distances;

public:
    Hierarchy(HyperbolicEmbedding* pemb);
    void process(int, int, int*);
    void threadedProcess(int);
    void save(const char*);
    static matrix load(const char*);
    static bool cmp(const Node& a, const Node& b);

};


#endif //HIERARX_HIERARCHY_H
