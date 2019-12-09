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

#define WITHOUT_NUMPY

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>

#include "src/VecBinder.h"
#include "src/HyperbolicEmbedding.h"
#include "src/RSGD.h"
#include "src/MainProcessor.h"
#include "src/Hierarchy.h"
//#include "matplotlibcpp.h"
#include "argh.h"
#include "src/Args.h"
#include "src/Similarity.h"

//namespace plt = matplotlibcpp;



int main(int argc, char* argv[]) {

    argh::parser cmdl(argv);

    if (cmdl["help"]) {
        std::cout << "HierarX: Learn hyperbolic embeddings based on multi-source similarities." << std::endl
        << std::endl
        << "required parameters (type):" << std::endl
        << "\t-nvoc (integer): number of word in hyperbolic vocabulary (taking nvoc most frequent word in -input for similarity files)." << std::endl
        << "\t-dim (integer): dimension of hyperbolic embedding." << std::endl
        << "\t-thread (integer): number of thread concurrently working." << std::endl
        << "\t-input (string): path to source embedding (.vec (GloVe) format or specify similarity for (h,t,score))." << std::endl
        << "\t-expdir (string): experiment directory (where hyperbolic and other files might be written)" << std::endl
        << std::endl
        << "optional parameters (type, default value):" << std::endl
        << "\t-lr (float, 0.01): opt learning rate (sgd)." << std::endl
        << "\t--similarity (flag): input format is similarity couples." << std::endl
        << "\t--nesterov (flag): activate nesterov optimization. (default false)" << std::endl
        << "\t-momentum (float, 0.9): momentum (sgd)." << std::endl
        << "\t-bs (integer, 10): number of main word in batch." << std::endl
        << "\t-sampling (integer, 20): number of word to sample for each main word." << std::endl
        << "\t-posthres (float, 0.1): between 0-1, proportion of positive example to introduce in samples (based on annoy tree)." << std::endl
        << "\t--continue training (loading from expdir)." << std::endl
        << "\t-plateau (float, 0.10): percentage of niter with lr / 10." << std::endl
        << "\t-c (float, 1): celerity of the hyperbolic model (Lorentz/Poincaré, in [0, +inf]." << std::endl
        << "\t-hmode={'Poincare', 'PoincareStack', 'Lorentz'} : Manifold." << std::endl
        << "\t-movie (float 5000) : register every n epoch" << std::endl
        << "\t--symmetric : whether input similarity scores are symmetric or asymmetric." << std::endl;
        return EXIT_SUCCESS;
    }

    if (cmdl["distances"]) {

        std::string embedding;
        std::string opath;
        if (!(cmdl("embedding") >> embedding) or !(cmdl("opath") >> opath)) {
            std::cout << "Please specify hierarx embedding." << std::endl;
            return EXIT_FAILURE;
        }
        int thread;
        cmdl("thread", 1) >> thread;

        HyperbolicEmbedding pemb = HyperbolicEmbedding::load(embedding.c_str());

        std::cout << "Computing distances" << std::endl;
        Hierarchy dm = Hierarchy(&pemb);
        dm.threadedProcess(thread);
        dm.save(opath.c_str());

        return EXIT_SUCCESS;

    } else if (cmdl["convert"]) {

        if (cmdl["help"]) {
            std::cout << "HierarX converter : convert .bin output embedding to .vec (GloVe) format." << std::endl
            << "Usage: " << std::endl
            << "Required" << std::endl
            << "\t-embedding (path) : path to embedding to convert" << std::endl
            << "\t-opath (path) : output path" << std::endl
            << std::endl
            << "Optional" << std::endl
            << "\t--poincare (flag) : if -embedding is a Lorentz embedding, convert it to a Poincaré embedding." << std::endl
            << "\t--lorentz (flag) : if -embedding is a Poincaré embedding, convert it to a Lorentz embedding." << std::endl;
        }

        std::string embedding;
        std::string opath;
        if (!(cmdl("embedding") >> embedding) or !(cmdl("opath") >> opath)) {
            std::cout << "Please specify hierarx embedding (-embedding=) and output path (-opath=)." << std::endl;
            return EXIT_FAILURE;
        }

        HyperbolicEmbedding pemb = HyperbolicEmbedding::load(embedding.c_str());

        if (cmdl["poincare"]) {
            pemb.toPoincare();
        }
        if (cmdl["lorentz"]) {
            pemb.toLorentz();
        }


        pemb.save(opath.c_str(), HyperbolicEmbedding::FILE_FORMAT::vec);

    } else {

        Args* args = new Args(cmdl);

        std::vector<HyperbolicVector*> buffers = std::vector<HyperbolicVector*>();
        if (args->hmode == "Poincare") {
            for (int i = 0; i <= args->nthread; i++)
                buffers.push_back(new PoincareVector(args->pmf, args->dim));
        } else if (args->hmode == "PoincareStack") {
            for (int i = 0; i < args->nthread; i++)
                buffers.push_back(new PoincareStack(args->dim));
        } else if (args->hmode == "Lorentz") {
            for (int i = 0; i <= args->nthread; i++)
                buffers.push_back(new LorentzVector(args->celerity, args->dim));
        } else {
            throw "Unimplemented hyperbolic space: " + args->hmode;
            return EXIT_FAILURE;
        }

        VecBinder* ftb = NULL;
        Similarity* sims = NULL;
        std::vector<std::string> vocab;

        if (!args->similarity) {
            ftb = new VecBinder(args);
            vocab = ftb->getVocab();
        } else {
            sims = new Similarity(args);
            vocab = sims->getVocab();
        }

        args->nvoc = vocab.size();
        HyperbolicEmbedding pembref;
        std::ifstream file(args->expdir + "/embedding.bin");
        if (args->continue__ && file.good()) {
            std::cout << "Pursuing experiment stored in " << args->expdir << std::endl;
            std::cout << "Cancelling burn-in phase" << std::endl;
            args->plateau = 0;
            pembref = HyperbolicEmbedding::load((args->expdir + "/embedding.bin").c_str());
        } else {
            if (args->continue__) {
                std::cout << "Unable to load embedding" << std::endl;
            }

            pembref = HyperbolicEmbedding(args, vocab, true);
        }

        HyperbolicEmbedding momref;
        std::ifstream momfile(args->expdir + "/momentum.bin");
        if (cmdl["continue"] && file.good()) {
            momref = HyperbolicEmbedding::load((args->expdir + "/momentum.bin").c_str());
        } else {
            if (cmdl["continue"]) {
                std::cout << "Unable to load momentum" << std::endl;
            }
            momref = HyperbolicEmbedding(args, vocab, true);
        }

        HyperbolicEmbedding *pemb = &pembref;
        HyperbolicEmbedding *momentum = &momref;
        RSGD *optim = new RSGD(args, pemb->vectors, momentum->vectors, &buffers);

        MainProcessor mp = MainProcessor(optim, pemb, momentum, ftb, sims, args);
        mp.threadedTrain(args);

        return EXIT_SUCCESS;
    }
}