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


#ifndef HIERARX_BATCHMAKER_H
#define HIERARX_BATCHMAKER_H

#include <vector>
#include <utility>
#include <cmath>
#include <set>
#include "Utils.h"
#include "VecBinder.h"
#include "PoincareVector.h"
#include "PoincareStack.h"
#include "HyperbolicEmbedding.h"
#include "EuclideanGeometry.h"
#include "Similarity.h"

class BatchMaker {

public:
    struct Batch {
        int targetIndex;
        std::vector<double>* targetVector;
        std::vector<int>* contextIndexes;
        std::vector<double>* contextSimilarity;
        std::vector<HyperbolicVector*>* grads;
        std::pair<int, std::vector<double>*>* tmpPair;
        int maxSimIndex;
        int niter;
        double maxSimValue;
        double loss;
        std::vector<double>* losses;
        std::set<int>* chosen;
    };


private:
    int bs;
    long progress;
    long totaliter;


    double ratioNeighbors;
    bool increasingRatio;
    int weighted;
    double maxRatioValue;
    double alpha;
    VecBinder* ftb;
    Similarity* sims;
    HyperbolicEmbedding* pemb;

public:

    Batch* batch;

    BatchMaker(HyperbolicEmbedding*, VecBinder*, int, const Args*);
    BatchMaker(HyperbolicEmbedding*, Similarity*, int, const Args*);

    Batch* iterNextBatch();
    void initGradients();
    double positiveRatio();
    double simNorm();

    static double logexp(double x);
    static double neglogexp(double x);
    static double logexp(double x, double w);
    static double neglogexp(double x, double w);
    static double exp(double x);

};


#endif //HIERARX_BATCHMAKER_H
