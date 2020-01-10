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

#ifndef HIERARX_VECBINDER_H
#define HIERARX_VECBINDER_H

#include <vector>
#include <iostream>
#include <utility>
#include <fstream>
#include <string>
#include <mutex>
#include "annoylib.h"
#include "kissrandom.h"
#include "EuclideanGeometry.h"
#include "Random.h"
#include "Args.h"


class VecBinder {



private:

    class SynchronizedVector {

    private:
        std::mutex mutex;

    public:
        std::vector<double> vec;

        SynchronizedVector();
        explicit SynchronizedVector(std::vector<double>);

        bool try_lock();
        void unlock();
        void setvec(std::vector<double>);
    };

    int voc_size;
    int dim;
    AnnoyIndex<int, double, Angular, Kiss32Random>* ai;
    std::vector<VecBinder::SynchronizedVector>* vectors;
    std::vector<std::string>* vocab;

public:

    struct LineReader {
        std::string line;
        int idx;
    };

    VecBinder(const Args*);
    void getRandomVector(std::pair<int, std::vector<double>*>*);
    void getRandomCloseVector(std::vector<double>* vector, int, std::pair<int, std::vector<double>*>*);
    int getDimension();
    std::string getWordAtIndex(int);

    std::vector<std::string> getVocab();

    static void readtill(LineReader*, char, std::string*);
    void unlock(int);
};


#endif //HIERARX_VECBINDER_H
