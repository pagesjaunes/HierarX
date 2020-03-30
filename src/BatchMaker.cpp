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


#include "BatchMaker.h"

#define NEIGHBORHOOD 10
#define MIN_SIMILARITY -10
#define MAX_EXP 16

BatchMaker::BatchMaker(
        HyperbolicEmbedding* pemb,
        VecBinder* ftb,
        int nloop,
        const Args* args
        ) {

    this->bs = args->bs;
    this->ratioNeighbors = args->posthres;
    this->maxRatioValue = args->maxposthres;
    this->totaliter = nloop * args->sampling;
    this->weighted = args->weighted;
    this->progress = 0;
    this->alpha = args->alpha;
    assert(this->maxRatioValue >= this->ratioNeighbors);

    this->pemb = pemb;
    this->ftb = ftb;
    this->sims = NULL;


    this->batch = new Batch();
    this->batch->targetIndex = -1;
    this->batch->targetVector = new std::vector<double>(this->ftb->getDimension(), 0);
    this->batch->contextIndexes = new std::vector<int>(this->bs, -1);
    this->batch->contextSimilarity = new std::vector<double>(this->bs, 0);
    this->batch->grads = new std::vector<HyperbolicVector*>();
    for (int i = 0; i < this->pemb->getVocSize(); i++) {
        switch(args->format) {
            case (hierarx::HYPERBOLIC_SPACE::Poincare):
                this->batch->grads->push_back(new PoincareVector(args->pmf, this->pemb->getDimension(), (double) 0));
                this->batch->grads->at(i)->project();
                break;
            case (hierarx::HYPERBOLIC_SPACE::Poincare_Stack):
                this->batch->grads->push_back(new PoincareStack(this->pemb->getDimension(), (double) 0));
                this->batch->grads->at(i)->project();
                break;
            case (hierarx::HYPERBOLIC_SPACE::Lorentz):
                this->batch->grads->push_back(new LorentzVector(args->celerity, this->pemb->getDimension(), (double) 0));
                break;
            default:
                throw "Unimplemented hyperbolic space";

        }
    }

    this->batch->maxSimIndex = 0;
    this->batch->maxSimValue = MIN_SIMILARITY;
    this->batch->loss = 0;
    this->batch->niter = this->bs;
    this->batch->losses = new std::vector<double>(this->bs, 0);

    this->batch->chosen = new std::set<int>();
    this->batch->tmpPair = new pair<int, std::vector<double>*>();

}

BatchMaker::Batch* BatchMaker::iterNextBatch() {

    this->batch->chosen->clear();


    // sample new batch

    if (this->ftb != NULL) {

        this->ftb->getRandomVector(this->batch->tmpPair);
        this->batch->targetIndex = this->batch->tmpPair->first;
        this->batch->chosen->insert(this->batch->targetIndex);
        this->batch->targetVector = this->batch->tmpPair->second;
        this->batch->maxSimIndex = 0;
        this->batch->maxSimValue = MIN_SIMILARITY;
        this->batch->loss = 0;

        this->batch->niter = this->bs;


        for (int i = 0; i < this->batch->niter; i++) {

            // prevent from sampling twice the same token
            do {
                if (Random::Double(1.0) > this->positiveRatio()) {
                    this->ftb->getRandomVector(this->batch->tmpPair);
                } else {
                    this->ftb->getRandomCloseVector(this->batch->targetVector, NEIGHBORHOOD, this->batch->tmpPair);
                }
            } while (this->batch->chosen->count(this->batch->tmpPair->first));
            this->batch->chosen->insert(this->batch->tmpPair->first);

            // fill batch
            this->batch->contextIndexes->at(i) = this->batch->tmpPair->first;
            this->batch->contextSimilarity->at(i) = EuclideanGeometry::dot(this->batch->targetVector,
                                                                           this->batch->tmpPair->second);

            // store similarity
            if (this->batch->contextSimilarity->at(i) > this->batch->maxSimValue) {
                this->batch->maxSimIndex = i;
                this->batch->maxSimValue = this->batch->contextSimilarity->at(i);
            }

            // store loss
            this->batch->losses->at(i) = this->pemb->at(this->batch->targetIndex)->dist(
                    *this->pemb->at(this->batch->contextIndexes->at(i)));

        }
    }

    if (this->sims != NULL) {
        this->sims->getRandomIndex(this->batch->tmpPair);
        this->batch->targetIndex = this->batch->tmpPair->first;
        this->batch->chosen->insert(this->batch->targetIndex);

        this->batch->maxSimIndex = 0;
        this->batch->maxSimValue = MIN_SIMILARITY;
        this->batch->loss = 0;

        this->batch->niter = this->bs; //std::fmin(this->bs, this->sims->getSize(this->batch->targetIndex));
        for (int i = 0; i < this->batch->niter; i++) {

            // prevent from sampling twice the same token
            do {
                if (Random::Double(1.0) > this->positiveRatio()) {
                    this->sims->getRandomIndex(this->batch->tmpPair);
                    this->batch->contextSimilarity->at(i) = this->sims->getSimilarity(this->batch->targetIndex,
                                                                                      this->batch->tmpPair->first);
                } else {
                    this->batch->contextSimilarity->at(i) = this->sims->getCloseSimilarity(this->batch->targetIndex, this->batch->tmpPair);
                }
            } while (this->batch->chosen->count(this->batch->tmpPair->first));
            this->batch->chosen->insert(this->batch->tmpPair->first);

            // fill batch
            this->batch->contextIndexes->at(i) = this->batch->tmpPair->first;

            // store similarity
            if (this->batch->contextSimilarity->at(i) > this->batch->maxSimValue) {
                this->batch->maxSimIndex = i;
                this->batch->maxSimValue = this->batch->contextSimilarity->at(i);
            }

            // store loss
            this->batch->losses->at(i) = this->pemb->at(this->batch->targetIndex)->dist(
                    *this->pemb->at(this->batch->contextIndexes->at(i)));

        }
    }

    double expdenominator = 0;
    double cumulative_sum_similarity_negative = 1.0;
    for (int i = 0; i < this->batch->niter; i++){
        if (i != this->batch->maxSimIndex) {
            cumulative_sum_similarity_negative += this->batch->contextSimilarity->at(i);
            expdenominator += BatchMaker::exp(-this->batch->losses->at(i));
        }
    }

    double sum = 0;
    for (int i = 0; i < this->batch->niter; i++) {

        double direction = i == this->batch->maxSimIndex ? -1 : 1;
        double diff, factor1, factor2, subloss;

        if (i == this->batch->maxSimIndex) {

            switch(this->weighted) {
                case 0 : factor1 = 1.0; factor2 = 1.0; break;
                case 1 : factor1 = 1.0; factor2 = this->batch->maxSimValue / this->simNorm(); break;
                case 2 : factor2 = 1.0; factor1 = this->batch->maxSimValue / this->simNorm(); break;
                case 3 : factor2 = this->batch->maxSimValue / this->simNorm(); factor1 = this->batch->maxSimValue / this->simNorm(); break;
                default: throw std::invalid_argument("weighted should be equal to 1 or 2");
            }

            diff = factor1 *
                   NumericalDifferentiate::diff([&](double x) { return BatchMaker::neglogexp(x, factor2); },
                                                this->batch->losses->at(i));
            subloss =  factor1 * this->neglogexp(this->batch->losses->at(i) * factor2);

        } else {

            switch(this->weighted) {
                case 0 : factor1 = 1.0; factor2 = 1.0; break;
                case 1 : factor1 = 1.0; factor2 = cumulative_sum_similarity_negative / (this->batch->contextSimilarity->at(i) + 1e-8); break;
                case 2 : factor2 = 1.0; factor1 = cumulative_sum_similarity_negative / (this->batch->contextSimilarity->at(i) + 1e-8); break;
                case 3 : factor2 = cumulative_sum_similarity_negative / (this->batch->contextSimilarity->at(i) + 1e-8); factor1 = cumulative_sum_similarity_negative / (this->batch->contextSimilarity->at(i) + 1e-8); break;
                default: throw std::invalid_argument("weighted should be equal to 1 or 2");
            }

            diff = factor1 * NumericalDifferentiate::diff([&](double x) {return BatchMaker::logexp(x, factor2);},
                     this->batch->losses->at(i)) ;

            subloss =  factor1 * this->logexp(this->batch->losses->at(i) * factor2);

        }

        std::pair<HyperbolicVector*, HyperbolicVector*> gradslist(
                this->batch->grads->at(this->batch->targetIndex),
                this->batch->grads->at(this->batch->contextIndexes->at(i))
        );

        this->pemb->at(this->batch->targetIndex)->diffDist(
                this->pemb->at(this->batch->contextIndexes->at(i)),
                gradslist,
                this->batch->maxSimIndex == i ? diff : (diff / (this->bs - 1))
        );

        this->batch->chosen->clear();

        this->batch->loss += direction == -1 ? subloss : (subloss / (this->bs - 1));

    }

    this->progress += 1;

    return this->batch;

}

double BatchMaker::logexp(double x) {
    if (x > MAX_EXP) {
        return std::log(1 + std::exp(-MAX_EXP));
    } else if (x < -MAX_EXP) {
        return std::log(1 + std::exp(MAX_EXP));
    }
    return std::log(1 + std::exp(-x));
}

double BatchMaker::neglogexp(double x) {
    if (x > MAX_EXP) {
        return std::log(1 + std::exp(MAX_EXP));
    } else if (x < -MAX_EXP) {
        return std::log(1 + std::exp(-MAX_EXP));
    }
    return std::log(1 + std::exp(x));
}

double BatchMaker::exp(double x) {
    if (x > MAX_EXP) {
        return std::exp(-MAX_EXP);
    } else if (x < -MAX_EXP) {
        return std::exp(MAX_EXP);
    }
    return std::exp(-x);
}

BatchMaker::BatchMaker(HyperbolicEmbedding* pemb, Similarity* sims, int nloop, const Args* args) {
    this->bs = args->bs;
    this->ratioNeighbors = args->posthres;
    this->weighted = args->weighted;
    this->alpha = args->alpha;

    this->pemb = pemb;
    this->ftb = NULL;
    this->sims = sims;
    this->totaliter = nloop * args->sampling;
    this->progress = 0;
    this->maxRatioValue = args->maxposthres;

    this->batch = new Batch();
    this->batch->targetIndex = -1;
    this->batch->targetVector = new std::vector<double>();
    this->batch->contextIndexes = new std::vector<int>(this->bs, -1);
    this->batch->contextSimilarity = new std::vector<double>(this->bs, 0);
    this->batch->grads = new std::vector<HyperbolicVector*>();
    for (int i = 0; i < this->pemb->getVocSize(); i++) {
        switch(args->format) {
            case (hierarx::HYPERBOLIC_SPACE::Poincare):
                this->batch->grads->push_back(new PoincareVector(args->pmf, this->pemb->getDimension(), (double) 0));
                this->batch->grads->at(i)->project();
                break;
            case (hierarx::HYPERBOLIC_SPACE::Poincare_Stack):
                this->batch->grads->push_back(new PoincareStack(this->pemb->getDimension(), (double) 0));
                this->batch->grads->at(i)->project();
                break;
            case (hierarx::HYPERBOLIC_SPACE::Lorentz):
                this->batch->grads->push_back(new LorentzVector(args->celerity, this->pemb->getDimension(), (double) 0));
                break;
            default:
                throw "Unimplemented hyperbolic space";

        }

    }

    this->batch->maxSimIndex = 0;
    this->batch->maxSimValue = MIN_SIMILARITY;
    this->batch->loss = 0;
    this->batch->losses = new std::vector<double>(this->bs, 0);

    this->batch->chosen = new std::set<int>();
    this->batch->tmpPair = new pair<int, std::vector<double>*>();
}

double BatchMaker::positiveRatio() {
    return this->ratioNeighbors + this->progress * (this->maxRatioValue - this->ratioNeighbors) / this->totaliter;
}

double BatchMaker::logexp(double x, double w) {
    return BatchMaker::logexp(w * x);
}

double BatchMaker::neglogexp(double x, double w) {
    return BatchMaker::neglogexp(w * x);
}

double BatchMaker::simNorm() {
    if (this->sims != NULL) {
        return this->sims->getMedianScore();
    }
    return this->alpha;
}
