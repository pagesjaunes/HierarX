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


#include "MainProcessor.h"

MainProcessor::MainProcessor(
        HyperbolicEmbedding* pemb,
        HyperbolicEmbedding* momentum,
        VecBinder* ftb,
        Similarity* sims,
        const Args* args
        ) {

    this->pemb = pemb;
    this->momentum = momentum;
    this->ftb = ftb;
    this->sims = sims;
    this->sampling_size = args->sampling;
    this->batch_size = args->bs;
    this->positive_sampling = args->posthres;
    this->ui = NULL;
    this->directory = args->expdir;
    this->format = args->format;
    this->early_stop = args->early_stop;
    this->earlystopper = this->early_stop ? new EarlyStopper(args->checkpoint) : NULL;
    this->purse_training = true;

    this->lockers = new std::vector<std::mutex>(this->pemb->getVocSize());


}

void MainProcessor::initProcess(int nloop, int idx, bool indisplay, const Args* args) {

    if (this->ftb != NULL) {
        HyperbolicEmbedding buffers = HyperbolicEmbedding(args, this->ftb->getVocab(), false);
        HyperbolicEmbedding momloc = HyperbolicEmbedding(args, this->ftb->getVocab(), false);
        BatchMaker bm = BatchMaker(this->pemb, this->ftb, nloop, args);
        RSGD *optim = new RSGD(args, pemb->vectors, momloc.vectors, buffers.vectors, this->lockers);
        this->process(bm, optim, nloop, idx, indisplay);
    } else if (this->sims != NULL) {
        HyperbolicEmbedding buffers = HyperbolicEmbedding(args, this->sims->getVocab(), false);
        HyperbolicEmbedding momloc = HyperbolicEmbedding(args, this->sims->getVocab(), false);
        BatchMaker bm = BatchMaker(this->pemb, this->sims, nloop, args);
        RSGD *optim = new RSGD(args, pemb->vectors, momloc.vectors, buffers.vectors, this->lockers);
        this->process(bm, optim, nloop, idx, indisplay);
    } else {
        throw "Input not understood";
    }

}

void MainProcessor::process(BatchMaker bm, RSGD* optim, int nloop, int idx, bool indisplay) {

    std::set<int>* iset = new std::set<int>();
    double lossBuffer;


    for (int i = 0; i < nloop; i++) {

        if (this->purse_training) {
            lossBuffer = 0;

            for (int j = 0; j < this->sampling_size; j++) {

                bm.iterNextBatch();
                iset->insert(bm.batch->targetIndex);
                for (int k = 0; k < bm.batch->niter; k++) {
                    iset->insert(bm.batch->contextIndexes->at(k));
                }
                lossBuffer += bm.batch->loss / this->sampling_size;

            }


            if (lossBuffer == lossBuffer) {
                optim->step(iset, bm.batch->grads, idx);
                iset->clear();
                this->ui->addCount(1, idx);
                this->ui->addLoss(lossBuffer, idx);
                this->ui->setPosthres(bm.positiveRatio(), idx);
            } else {
                i--;
                iset->clear();
            }

            if (indisplay) {
                // only if mono threaded
                this->ui->display(this->directory);
            }
        } else {
            break;
        }
    }

}

double MainProcessor::threadedTrain(const Args* args) {

    this->ui = new UserInterface(args, this->pemb, this->momentum);
    double loss = 0;

    for (int j = 0; j < (args->niter / args->rebuild); j++) {
        if (this->purse_training) {
            if (args->nthread == 0) {

                this->initProcess(args->rebuild, 0, true, args);

            } else {

                std::vector<std::thread> threads;

                for (int i = 0; i < args->nthread; i++)
                    threads.push_back(
                            std::thread([=]() {
                                this->initProcess(std::ceil((double) args->rebuild / (double) args->nthread), i, false,
                                                  args);
                            }));

                int totalCount = 0;
                while (totalCount < args->rebuild && purse_training) {
                    bool nullLoss = this->ui->nonNullLoss();
                    std::pair<int, double> total_and_loss = this->ui->display(this->directory);
                    totalCount = total_and_loss.first - j * args->rebuild;
                    loss = total_and_loss.second;
                    if (this->earlystopper != NULL && !nullLoss) {
                        this->earlystopper->add_loss(loss);
                        this->purse_training = this->earlystopper->continue_training();
                    }
                }

                for (int i = 0; i < args->nthread; i++) threads[i].join();

            }
            if (this->ftb != NULL) {
                std::cout << std::endl << "Rebuilding annoy index... ";
                this->ftb->rebuild();
                std::cout << "Done." << std::endl;
            }
        }
    }

    if (!this->purse_training) {
        std::cout << "Early stop" << std::endl;
    }

    this->pemb->save((this->directory + "/embedding").c_str());
    std::cout << "\n";

    free(this->ui);

    return loss;


}

MainProcessor::MainProcessor() {

}

void MainProcessor::EarlyStopper::add_loss(double loss) {
    if (this->kcurrent < this->size) {
        this->current[this->kcurrent] = loss;
        this->kcurrent += 1;
    } else {
        double avg = EuclideanGeometry::sum(&this->current) / this->size;
        if (this->minimum == NULL) {
            this->minimum = new double[1];
            this->minimum[0] = avg;
        } else {
            if (this->minimum[0] > avg) {
                this->minimum[0] = avg;
            }
        }

        if (this->kcontainer >= this->container.size()) {
            this->kcontainer = 0;
        }
        this->container[this->kcontainer] = avg;
        this->kcontainer += 1;

        this->kcurrent = 0;
        this->add_loss(loss);
    }
}

MainProcessor::EarlyStopper::EarlyStopper(int size) {
    this->current = std::vector<double>(size, 0);
    this->container = std::vector<double>(5, 0);
    this->minimum = NULL;
    this->kcurrent = 0;
    this->kcontainer = 0;
    this->size = size;
}

bool MainProcessor::EarlyStopper::continue_training() {
    if (this->minimum == NULL) return true;
    double ratio = 0;
    int pivot = this->kcontainer == 0 ? this->container.size() - 1 : this->kcontainer - 1;
    for (int i = pivot; i > 0; i--) {
        if (this->container[i] >= this->minimum[0]) {
            ratio += this->container[i - 1] - this->container[i];
        }
    }

    if (pivot < this->container.size() - 1 && this->container[this->container.size()] >= this->minimum[0]) {
        ratio += this->container[this->container.size() - 1] - this->container[0];
    }

    for (int i = this->container.size() - 1; i > pivot + 1; i--) {
        if (this->container[i] >= this->minimum[0]) {
            ratio += this->container[i - 1] - this->container[i];
        }
    }

    return ratio >= 0;
}
