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


#ifndef HIERARX_MAINPROCESSOR_H
#define HIERARX_MAINPROCESSOR_H

#include <vector>
#include <set>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Args.h"
#include "RSGD.h"
#include "VecBinder.h"
#include "HyperbolicEmbedding.h"
#include "BatchMaker.h"
#include "UserInterface.h"
#include "Similarity.h"

#define EARLY_STOP_THRESHOLD 0.75

class MainProcessor {

public:

    class EarlyStopper{
    private:
        std::vector<double> current;
        std::vector<double> container;
        double* minimum;
        int kcurrent;
        int kcontainer;
        int size;

    public:

        EarlyStopper(int);
        void add_loss(double);
        bool continue_training();
    };

    MainProcessor();
    MainProcessor(HyperbolicEmbedding*, HyperbolicEmbedding*, VecBinder*, Similarity*, const Args*);
    void process(BatchMaker, RSGD*, int, int, bool);
    void initProcess(int, int, bool, const Args*);

    double threadedTrain(const Args*);

private:
    int sampling_size;
    int batch_size;
    double positive_sampling;
    VecBinder* ftb;
    Similarity* sims;
    HyperbolicEmbedding* pemb;
    HyperbolicEmbedding* momentum;
    UserInterface* ui;
    std::string directory;
    int format;
    std::vector<std::mutex>* lockers;
    bool early_stop;
    bool purse_training;
    EarlyStopper* earlystopper;



};


#endif //HIERARX_MAINPROCESSOR_H
