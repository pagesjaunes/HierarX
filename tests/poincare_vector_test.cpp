/*
 * HierarX is a software aiming to build hyperbolic word representations.
 * Copyright (C) 2019 Solocal SA and CNRS.
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

#include <cstdlib>
#include "gtest/gtest.h"
#include "../src/PoincareVector.h"

#define DIM 10
#define SEED 42
#define N_TEST 1000
#define EQ_THRES 1e-8
#define CELERITY 1.0

PoincareVector::PoincareManifold pmf(CELERITY, DIM);
PoincareVector pzeros = PoincareVector(&pmf, DIM, (double) 0);
PoincareVector pones = PoincareVector(&pmf, DIM, 1);
PoincareVector tmp = PoincareVector(&pmf, DIM, 1);
PoincareVector tmp2 = PoincareVector(&pmf, DIM, 1);

PoincareVector generateRandom() {
    std::vector<double>* values = new std::vector<double>(DIM, 0);
    for(int i = 0; i < DIM; i++) values->at(i) = 0.35 * ((double) std::rand()) / RAND_MAX;
    PoincareVector prand = PoincareVector(&pmf, DIM, values);
    prand.project();
    return prand;
}

PoincareVector generateRandomInfinity() {
    std::vector<double>* values = new std::vector<double>(DIM, 0);
    for(int i = 0; i < DIM; i++) values->at(i) = 0.35 * ((double) std::rand()) / RAND_MAX;
    double norm = EuclideanGeometry::norm(values);
    for(int i = 0; i < DIM; i++) values->at(i) /= norm;
    PoincareVector prand = PoincareVector(&pmf, DIM, values);
    return prand;
}

TEST(addition, Properties) {

    std::srand(SEED);
    pones.project();

    // x + 0 = 0 + x = x
    pmf.mobiusAdd(pones.coordinates, pzeros.coordinates, tmp.coordinates);
    ASSERT_EQ(pones.dist(tmp), 0);
    pmf.mobiusAdd(pzeros.coordinates, pones.coordinates, tmp.coordinates);
    ASSERT_EQ(pones.dist(tmp), 0);
    for (int i = 0; i < N_TEST; i++) {
        PoincareVector prand = generateRandom();
        pmf.mobiusAdd(pzeros.coordinates, prand.coordinates, tmp.coordinates);
        ASSERT_LT(prand.dist(tmp), EQ_THRES);
        pmf.mobiusAdd(prand.coordinates, pzeros.coordinates, tmp.coordinates);
        ASSERT_LT(prand.dist(tmp), EQ_THRES);
    }

    // -x + x = x + -x = 0
    pmf.mobiusAdd(pones.coordinates, (-pones).coordinates, tmp.coordinates);
    ASSERT_EQ(pzeros.dist(tmp), 0);
    pmf.mobiusAdd((-pones).coordinates, pones.coordinates, tmp.coordinates);
    ASSERT_EQ(pzeros.dist(tmp), 0);
    for (int i = 0; i < N_TEST; i++) {
        PoincareVector prand = generateRandom();
        pmf.mobiusAdd((-pones).coordinates, pones.coordinates, tmp.coordinates);
        ASSERT_LT(pzeros.dist(tmp), EQ_THRES);
        pmf.mobiusAdd(pones.coordinates, (-pones).coordinates, tmp.coordinates);
        ASSERT_LT(pzeros.dist(tmp), EQ_THRES);
    }

    // left cancellation
    for (int i = 0; i < N_TEST; i++) {
        PoincareVector px = generateRandom();
        PoincareVector py = generateRandom();
        pmf.mobiusAdd(px.coordinates, py.coordinates, tmp.coordinates);
        pmf.mobiusAdd((-px).coordinates, tmp.coordinates, tmp.coordinates);
        ASSERT_LT(py.dist(tmp), EQ_THRES);
    }

    // 1 is infinity
    for (int i = 0; i < N_TEST; i++) {
        PoincareVector px = generateRandomInfinity();
        PoincareVector py = generateRandom();
        pmf.mobiusAdd(px.coordinates, py.coordinates, tmp.coordinates);
        ASSERT_LT(px.dist(tmp), EQ_THRES);
    }

}

TEST(multiplication, Properties) {

    // associativity
    std::srand(SEED);

    for (int i = 0; i < N_TEST; i++) {
        double r = 2 * ((double) rand()) / RAND_MAX;
        double s = 2 * ((double) rand()) / RAND_MAX;
        PoincareVector px = generateRandom();

        // (r * s) * px
        pmf.mobiusDot(r * s, px.coordinates, tmp.coordinates);

        // r * (s * px)
        pmf.mobiusDot(s, px.coordinates, tmp2.coordinates);
        pmf.mobiusDot(r, tmp2.coordinates, tmp2.coordinates);

        // (r * s) * px = r * (s * px)
        ASSERT_LT(tmp.dist(tmp2), EQ_THRES);
    }

    // n addition
    for (int i = 0; i < N_TEST; i++) {
        double n = 10;
        PoincareVector px = generateRandom();
        PoincareVector py = PoincareVector(&pmf, DIM, new std::vector<double>(DIM, 0));
        py.project();

        pmf.mobiusAdd(py.coordinates, px.coordinates, tmp.coordinates);
        for (int j = 1; j < n; j++) pmf.mobiusAdd(px.coordinates, tmp.coordinates, tmp.coordinates);

        pmf.mobiusDot(n, px.coordinates, tmp2.coordinates);

        ASSERT_LT(tmp2.dist(tmp), 1e-2);
    }

    // distributivity
    for (int i = 0; i < N_TEST; i++) {
        double r = 2 * ((double) rand()) / RAND_MAX;
        double s = 2 * ((double) rand()) / RAND_MAX;
        PoincareVector px = generateRandom();

        // (r * px) + (s * px)
        pmf.mobiusDot(r, px.coordinates, tmp.coordinates);
        pmf.mobiusDot(s, px.coordinates, tmp2.coordinates);
        pmf.mobiusAdd(tmp.coordinates, tmp2.coordinates, tmp.coordinates);

        // (r + s) * px
        pmf.mobiusDot(r + s, px.coordinates, tmp2.coordinates);
        ASSERT_LT(tmp2.dist(tmp), EQ_THRES);
    }

    // scaling property
    for (int i = 0; i < N_TEST; i++) {
        double r = 2 * ((double) rand()) / RAND_MAX;
        PoincareVector px = generateRandom();

        //
        pmf.mobiusDot(r, px.coordinates, tmp.coordinates);
        ASSERT_LT(pmf.dist((px / px.eucnorm()).coordinates, (tmp / tmp.eucnorm()).coordinates), EQ_THRES);
    }

    // convergence
    for (int i = 0; i < N_TEST; i++) {
        double r = 0.5;
        PoincareVector px = generateRandom();
        pmf.mobiusDot(r, px.coordinates, tmp.coordinates);
        for (int j = 1; j < 100; j++) {
            pmf.mobiusDot(r, tmp.coordinates, tmp.coordinates);
            px.project();
        }
        ASSERT_LT(tmp.eucnorm(), 1e-4);
    }

}
