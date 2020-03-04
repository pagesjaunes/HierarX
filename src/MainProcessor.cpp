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
        RSGD* optim,
        HyperbolicEmbedding* pemb,
        HyperbolicEmbedding* momentum,
        VecBinder* ftb,
        Similarity* sims,
        const Args* args
        ) {
    this->optim = optim;
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
}

void MainProcessor::initProcess(int nloop, int idx, bool indisplay, const Args* args) {
    if (this->ftb != NULL) {
        BatchMaker bm = BatchMaker(this->pemb, this->ftb, nloop, args);
        this->process(bm, nloop, idx, indisplay);
    } else if (this->sims != NULL) {
        BatchMaker bm = BatchMaker(this->pemb, this->sims, nloop, args);
        this->process(bm, nloop, idx, indisplay);
    } else {
        throw "Input not understood";
    }

}

void MainProcessor::process(BatchMaker bm, int nloop, int idx, bool indisplay) {

    std::set<int>* iset = new std::set<int>();
    double lossBuffer;

    for (int i = 0; i < nloop; i++) {
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
            this->optim->step(iset, bm.batch->grads, idx);
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
    }

}

void MainProcessor::threadedTrain(const Args* args) {

    for (int j = 0; j < (args->niter / args->rebuild); j++) {
        if (args->nthread == 0) {

            this->ui = new UserInterface(args, this->pemb, this->momentum);
            this->initProcess(args->rebuild, 0, true, args);

        } else {

            this->ui = new UserInterface(args, this->pemb, this->momentum);
            std::vector<std::thread> threads;

            for (int i = 0; i < args->nthread; i++)
                threads.push_back(
                        std::thread([=]() { this->initProcess(std::ceil((double) args->rebuild / (double) args->nthread), i, false, args); }));

            int totalCount = 0;
            while (totalCount < args->rebuild) totalCount = this->ui->display(this->directory);


            for (int i = 0; i < args->nthread; i++) threads[i].join();

        }
        if (this->ftb != NULL) {
            std::cout << std::endl << "Rebuilding annoy index" << std::endl;
            this->ftb->rebuild();
        }
    }

    this->pemb->save((this->directory + "/embedding").c_str());
    std::cout << "\n";
    free(this->ui);



}
