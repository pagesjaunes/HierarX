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

#include "gtest/gtest.h"
#include "../src/MainProcessor.h"
#include "../src/RSGD.h"
#include <cstring>
#include <cstdlib>

#define NVOC 5000
/*
const char* EMBEDDINGS = std::getenv("EMBEDDINGS");

TEST(processTest, Trivial) {

    char* local_path = static_cast<char *>(malloc(sizeof(char) * (std::strlen(EMBEDDINGS) + 10)));

    std::strcpy(local_path, EMBEDDINGS);
    std::strcat(local_path, "/wikien20.bin\0");

    VecBinder* ftb = new VecBinder(NVOC, local_path, "./tree.bin", true);
    HyperbolicEmbedding* pemb = new HyperbolicEmbedding(10, ftb->getVocab());
    std::vector<PoincareVector> momentum = std::vector<PoincareVector>();
    for (int i = 0; i < pemb->vectors->size(); i++) {
        PoincareVector pvec = PoincareVector(10, (double) 0);
        pvec.project();
        momentum.push_back(pvec);
    }
    RSGD<PoincareVector>* optim = new RSGD<PoincareVector>(0.01, 0.9, false, pemb->vectors, &momentum);

    MainProcessor mp = MainProcessor(optim, pemb, ftb, 5, 20, 0.20, "./");

    mp.threadedTrain(1000, 0);

}*/