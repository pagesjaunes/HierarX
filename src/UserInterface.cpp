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



#include "UserInterface.h"

#define MIN_CHECKPOINT 1000


UserInterface::UserInterface(const Args* args, HyperbolicEmbedding* pemb, HyperbolicEmbedding* momentum) {

    int n = args->nthread == 0 ? 1 : args->nthread;
    this->counts = new std::vector<int>(n, 0);
    this->runcounts = new std::vector<int>(n, 0);
    this->losses = new std::vector<double>(n, 0);
    this->posthres = new std::vector<double>(n, 0);
    this->cret = 0;
    this->pemb = pemb;
    this->momentum = momentum;
    this->ps = args->posthres != args->maxposthres;
    this->countsave = 0;
    this->checkpoint = std::fmax(args->checkpoint, MIN_CHECKPOINT);
    if (args->checkpoint < MIN_CHECKPOINT) {
        std::cout << std::endl << "WARN:: Minimum value for checkpoint is " << MIN_CHECKPOINT << std::endl;
    }

    this->movie = args->movie;

    std::cout.precision(7);
    //plt::figure();

}

int UserInterface::sumCounts() {
    return sum(this->counts);
}

double UserInterface::meanLoss() {
    return sum(this->losses) / this->losses->size();
}

int UserInterface::sum(std::vector<int>* vec) {
    int res = 0;
    for (int i = 0; i < vec->size(); i++) res += vec->at(i);
    return res;
}

double UserInterface::sum(std::vector<double>* vec) {
    double res = 0;
    for (int i = 0; i < vec->size(); i++) res += vec->at(i);
    return res;
}

void UserInterface::addCount(int value, int idx) {
    assert(idx < this->losses->size());
    this->counts->at(idx) += value;
}

void UserInterface::setLoss(double value, int idx) {
    assert(idx < this->losses->size());
    this->losses->at(idx) = value;
}

int UserInterface::display(std::string directory) {

    int totalCount = this->sumCounts();
    double loss = this->meanLoss();
    double meanps = this->meanPosthres();

    if (this->ps) {
        std::cout << "n: " << std::to_string(totalCount) << " -- loss: " << std::to_string(loss) << " -- avg posthres: " << std::to_string(meanps) << "\r";
    } else {
        std::cout << "n: " << totalCount << " -- loss: " << loss << "\r";
    }

    if (totalCount / this->checkpoint != this->cret) {
        this->cret = totalCount / this->checkpoint;
        std::cout << "\n";
        //this->drawFig((directory + "/figure").c_str());
        this->save(directory);
        this->resetLoss();
    }

    usleep(500);

    return totalCount;
}

/*void UserInterface::drawFig(const char* filename) {

    std::vector<double> X = std::vector<double>(this->pemb->getVocSize(), 0);
    std::vector<double> Y = std::vector<double>(this->pemb->getVocSize(), 0);

    for (int i = 0; i < this->pemb->getVocSize(); i++) {
        X[i] = pemb->vectors->at(i)[0];
        Y[i] = pemb->vectors->at(i)[1];
    }

    plt::figure_size(800, 400);
    plt::plot(X, Y, "r+");
    plt::save(filename);
    plt::clf();
    plt::close();

}*/

void UserInterface::addLoss(double loss, int idx) {
    this->losses->at(idx) = (this->runcounts->at(idx) * this->losses->at(idx) + loss) / (this->runcounts->at(idx) + 1);
    this->runcounts->at(idx) += 1;
}

void UserInterface::resetLoss() {
    for (int i = 0; i < this->runcounts->size(); i++) {
        this->runcounts->at(i) = 0;
        this->losses->at(i) = 0;
    }

}

void UserInterface::save(std::string directory) {
    this->pemb->save((directory + "/embedding" + (this->movie ? std::to_string(this->countsave) : "")).c_str());
    this->momentum->save((directory + "/momentum").c_str());
    this->countsave += 1;
}

void UserInterface::setPosthres(double val, int idx) {

    this->posthres->at(idx) = val;

}

double UserInterface::meanPosthres() {
    return sum(this->posthres) / this->posthres->size();
}

