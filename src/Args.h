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


#ifndef HIERARX_ARGS_H
#define HIERARX_ARGS_H

#include <iostream>
#include "argh.h"
#include "Utils.h"
#include "PoincareVector.h"

class Args {

public:

    // required
    int nvoc;
    int dim;
    int nthread;
    std::string expdir;
    std::string input;

    // optional
    int niter;
    int bs;
    int sampling;
    int format;
    int checkpoint;
    int kneighbors;
    int ntrees;
    int rebuild;
    int weighted;
    PoincareVector::PoincareManifold* pmf;
    double lr;
    double momentum;
    double plateau;
    double posthres;
    double celerity;
    double minlr;
    double maxposthres;
    double alpha;
    bool similarity;
    bool nesterov;
    bool symmetric;
    bool continue__;
    bool declr;
    bool movie;
    bool lorentzian;
    std::string hmode;

    Args();
    Args(argh::parser&);

    void record(const char* path);
    void printOut();
    void setformat();

};


#endif //HIERARX_ARGS_H
