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

#ifndef HIERARX_SIMILARITY_H
#define HIERARX_SIMILARITY_H

#include "VecBinder.h"
#include <map>
#include <utility>
#include "Args.h"
#define RESAMPLE_SIZE 1000

class Similarity {

private:

    struct LineFormat{
        std::string h;
        std::string t;
        double score;
        LineFormat(std::string h, std::string t, double score){
            this->h = h;
            this->t = t;
            this->score = score;
        }
    };
    std::map<std::string, std::vector<LineFormat>*>* hmap;
    std::map<std::pair<int, int>, double>* htmap;
    std::vector<std::pair<std::string, int>>* keys;
    std::map<std::string, int>* w2k;
    std::map<int, std::set<std::string>>* key2dist;
    std::map<int, std::vector<int>>* resamplers;
    bool symmetric;
    double medianScore;


public:

    Similarity();
    explicit Similarity(Args*);
    int getVocSize();
    std::vector<std::string> getVocab();
    void getRandomIndex(std::pair<int, std::vector<double>*>*);
    double getCloseSimilarity(int, std::pair<int, std::vector<double>*>*);
    double getSimilarity(int, int);
    int getSize(int);
    double getMedianScore();

    static bool compare(const std::pair<std::string, int>&, const std::pair<std::string, int>&);


};


#endif //HIERARX_SIMILARITY_H
