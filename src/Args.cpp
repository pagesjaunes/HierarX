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


#include "Args.h"

Args::Args(argh::parser& cmdl) {

    // set required parameters
    if (
            !(cmdl("nvoc") >> nvoc) or
            !(cmdl("dim") >> dim) or
            !(cmdl("thread") >> nthread) or
            !(cmdl("expdir") >> expdir) or
            !(cmdl("input") >> input)
            ) {
        std::cout << nvoc;
        throw "Required paramaters are unset";
    }

    std::cout << "Training parameters: \n"
              << "\tVocabulary size (-nvoc): " << nvoc << std::endl
              << "\tHyperbolic dimension (-dim): " << dim << std::endl
              << "\tNumber of thread (-thread): " << nthread << std::endl
              << "\tExperiment directory (-expdir): " << expdir << std::endl
              << "\tSimilarity source (-input): " << input << std::endl;


    // set optional parameters -- sampling and bs are reversed
    cmdl("lr", 1e-2) >> lr;
    cmdl("momentum", 0.9) >> momentum;
    cmdl("bs", 10) >> bs;
    cmdl("sampling", 20) >> sampling;
    cmdl("posthres", 0.10) >> posthres;
    cmdl("plateau", 0.10) >> plateau;
    cmdl("hmode", "Poincare") >> hmode;
    cmdl("niter", 100000) >> niter;
    cmdl("c", 1.0) >> celerity;
    cmdl("minlr", 0) >> minlr;
    cmdl("maxposthres", posthres) >> maxposthres;
    cmdl("checkpoint", 10000) >> checkpoint;
    cmdl("kneighbors", 100) >> kneighbors;
    cmdl("ntrees", 20) >> ntrees;
    cmdl("rebuild", 10000) >> rebuild;

    similarity = cmdl["similarity"];
    continue__ = cmdl["continue"];
    symmetric = cmdl["symmetric"];
    nesterov = cmdl["nesterov"];
    declr = cmdl["declr"];
    movie = cmdl["movie"];
    lorentzian = cmdl["lorentzian"];
    pmf = new PoincareVector::PoincareManifold(celerity, dim, lorentzian);

    std::cout
            << "\tlearning rate (-lr): " << lr << std::endl
            << "\tmomentum (-momentum): " << momentum << std::endl
            << "\tsimilarity (--similarity): " << (similarity ? "ON" : "OFF") << std::endl
            << "\tnesterov (--nesterov): " << (nesterov ? "ON" : "OFF") << std::endl
            << "\tcontinue (--continue): " << (continue__ ? "ON" : "OFF") << std::endl
            << "\tsymmetric (--symmetric): " << (symmetric ? "ON" : "OFF") << std::endl
            << "\tbatch size (-bs): " << bs << std::endl
            << "\tsampling in batches (-sampling): " << sampling << std::endl
            << "\tpositive sampling ratio (-posthres): " << posthres << std::endl
            << "\tmaximum positive sampling (continuously increasing, -maxposthres): " << maxposthres << std::endl
            << "\tcontinue training (--continue): " << (cmdl["continue"] ? "ON" : "OFF") << std::endl
            << "\tburn-in phase (-plateau): " << plateau << std::endl
            << "\thyperbolic space (-hmode={'Poincare', 'PoincareStack', 'Lorentz'}): " << hmode << std::endl
            << "\tnumber of iteration (-niter): " << niter << std::endl
            << "\tcelerity (hyperbolic curvature, -c): " << celerity << std::endl
            << "\tdecreasing learning rate (--declr): " << (declr ? "ON" : "OFF") << std::endl
            << "\tminimum learning rate (-minlr): " << minlr << std::endl
            << "\tcheckpoint (-checkpoint): " << checkpoint << std::endl
            << "\tmovie (--movie): " << (movie ? "ON" : "OFF") << std::endl
            << "\tlorentzian distance (only for Poincaré embeddings, celerity = beta, --lorentzian): " << (lorentzian ? "ON" : "OFF") << std::endl
            << "\tnumber of approximate neighbors (-kneighbors): " << kneighbors << std::endl
            << "\tnumber of trees for approximate neighbor search (see annoy doc, -ntrees): " << ntrees << std::endl
            << "\trebuild annoy index every n iteration (only when input is .vec, -rebuild): " << rebuild << std::endl
            << std::endl;

    if (similarity) rebuild = niter;

    if (hmode == "Poincare") {
        format = hierarx::HYPERBOLIC_SPACE::Poincare;
    } else if (hmode == "PoincareStack") {
        format = hierarx::HYPERBOLIC_SPACE::Poincare_Stack;
    } else if (hmode == "Lorentz") {
        format = hierarx::HYPERBOLIC_SPACE::Lorentz;
    } else {
        throw "Unimplemented hyperbolic space: " + hmode;
    }

}

Args::Args() {

    nvoc = 0;
    dim = 0;
    nthread = 0;
    expdir = "";
    input = "";

    lr = 1e-2;
    momentum = 0.9;
    bs = 10;
    sampling = 20;
    posthres = 0.10;
    plateau = 0.10;
    celerity = 1.0;
    checkpoint = 10000;
    kneighbors = 100;
    ntrees = 20;
    rebuild = 10000;

    hmode = "Poicnare";
    niter = 100000;
    similarity = false;
    symmetric = false;
    continue__ = false;
    nesterov = true;
    movie =false;
    lorentzian=false;
    format = hierarx::HYPERBOLIC_SPACE::Poincare;

    pmf = new PoincareVector::PoincareManifold(celerity, dim, lorentzian);

}
