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


#ifndef HIERARX_USERINTERFACE_H
#define HIERARX_USERINTERFACE_H

#include <cassert>
#include <iostream>
#include <vector>
#include <unistd.h>
#include "VecBinder.h"
#include "HyperbolicEmbedding.h"

class UserInterface {

private:
    int cret;
    int countsave;
    bool ps;
    int checkpoint;
    bool movie;

public:
    std::vector<int>* counts;
    std::vector<int>* runcounts;
    std::vector<double>* losses;
    std::vector<double>* posthres;
    HyperbolicEmbedding* pemb;
    HyperbolicEmbedding* momentum;

    UserInterface(const Args*, HyperbolicEmbedding*, HyperbolicEmbedding*);

    void addCount(int, int);
    void setLoss(double, int);
    void addLoss(double, int);
    void setPosthres(double, int);
    void resetLoss();
    void save(std::string directory);

    int display(std::string directory);

    int sumCounts();
    double meanLoss();
    double meanPosthres();

    static int sum(std::vector<int>*);
    static double sum(std::vector<double>*);


};


#endif //HIERARX_USERINTERFACE_H
