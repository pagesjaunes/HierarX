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
#include "../src/BatchMaker.h"
#include <cstring>
#include <cstdlib>

/*
const char* EMBEDDINGS = std::getenv("EMBEDDINGS");

TEST(iterBatchTest, Trivial) {

    char* local_path = static_cast<char *>(malloc(sizeof(char) * (std::strlen(EMBEDDINGS) + 1)));
    std::strcpy(local_path, EMBEDDINGS);
    std::strcat(local_path, "/wikien20.bin");
    VecBinder* ftb = new VecBinder(100, local_path, "./tree.bin", true);

    HyperbolicEmbedding* pemb = new HyperbolicEmbedding(10, 100);

    BatchMaker bm = BatchMaker(20, 0.2, pemb, ftb);

    for (int i = 0; i < 1000; i++) {
        BatchMaker::Batch* b = bm.iterNextBatch(true);
        ASSERT_GT(b->loss, 0.01);
    }

}

TEST(initGradientTest, Trivial) {
    char* local_path = static_cast<char *>(malloc(sizeof(char) * (std::strlen(EMBEDDINGS) + 1)));
    std::strcpy(local_path, EMBEDDINGS);
    std::strcat(local_path, "/wikien20.bin");
    VecBinder* ftb = new VecBinder(100, local_path, "./tree.bin", true);

    HyperbolicEmbedding* pemb = new HyperbolicEmbedding(10, 100);

    BatchMaker bm = BatchMaker(20, 0.2, pemb, ftb);
    for (int i = 0; i < 1000; i++) {
        BatchMaker::Batch* b = bm.iterNextBatch(true);
        double totalGrad = 0;
        for (int j = 0; j < 100; j++) {
            totalGrad += EuclideanGeometry::normsquare(b->grads->at(j).get_coordinates());
        }
        ASSERT_GT(totalGrad, 1e-6);
        bm.initGradients();
        totalGrad = 0;
        for (int j = 0; j < 100; j++) {
            totalGrad += EuclideanGeometry::normsquare(b->grads->at(j).get_coordinates());
        }
        ASSERT_LT(totalGrad, 1e-6);
    }
}
*/