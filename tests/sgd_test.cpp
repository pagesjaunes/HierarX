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

#include <cmath>
#include <vector>
#include "gtest/gtest.h"
//#include "../src/Args.h"
#include "../src/RSGD.h"
#include "../src/NumericalDifferentiate.h"
#include "../src/EuclideanGeometry.h"
#include "../src/DoubleVector.h"


#define EQ_THRES 1e-3
#define HDIM 4
#define SEED 42


double xsq(double x) {
    return x * x - 9 * x;
}

double norm_minus(std::vector<double>* x) {
    std::vector<double>* y = new std::vector<double>(*x);
    y->at(0) -= 1;
    y->at(1) -= 6;
    return EuclideanGeometry::norm(y);
}

TEST(stepTest, ScalarVanilla) {
    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = false;

    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, 50));
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, 40));
    std::vector<HyperbolicVector*>* buffer = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, (double) 0));
    RSGD* optim = new RSGD(args, params, moments, buffer);
    std::vector<HyperbolicVector*>* grads = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, (double) 0));
    for (int i = 0; i < 1000; i++) {
        grads->at(0)->coordinates[0] = grads->at(0)->coordinates[0] + NumericalDifferentiate::diff(&xsq, params->at(0)->coordinates[0]);
        optim->step(new std::vector<int>(1, 0), grads, 0);
    }
    ASSERT_LT(abs(params->at(0)->coordinates[0] - 4.5), EQ_THRES);
}

TEST(stepTest, ScalarNesterov) {
    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = true;

    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, 50));
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, 40));
    std::vector<HyperbolicVector*>* buffer = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, (double) 0));
    RSGD* optim = new RSGD(args, params, moments, buffer);
    std::vector<HyperbolicVector*>* grads = new std::vector<HyperbolicVector*>(1, new DoubleVector(1, (double) 0));
    for (int i = 0; i < 1000; i++) {
        grads->at(0)->coordinates[0] = grads->at(0)->coordinates[0] + NumericalDifferentiate::diff(&xsq, params->at(0)->coordinates[0]);
        optim->step(new std::vector<int>(1, 0), grads, 0);
    }
    ASSERT_LT(abs(params->at(0)->coordinates[0] - 4.5), EQ_THRES);
}

TEST(stepTest, VectorVanilla) {
    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = false;


    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, 50));
    std::vector<HyperbolicVector*>*  moments = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, 50));
    std::vector<HyperbolicVector*>*  buffer = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, (double) 0));
    RSGD* optim = new RSGD(args, params, moments, buffer);
    std::vector<HyperbolicVector*>* grads = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, (double) 0));
    std::vector<int> indexes = std::vector<int>(1, 0);
    std::vector<double> tmp = std::vector<double>(2);
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 2; j++) tmp[j] = params->at(0)->coordinates[j];
        std::vector<double> diff = NumericalDifferentiate::diff(&norm_minus, &tmp);
        for (int j = 0; j < 2; j++) grads->at(0)->coordinates[j] = diff[j];
        optim->step(&indexes, grads, 0);
    }

    ASSERT_LT(abs(params->at(0)->coordinates[0] - 1), EQ_THRES);
    ASSERT_LT(abs(params->at(0)->coordinates[1] - 6), EQ_THRES);
}

TEST(stepTest, VectorNesterov) {
    Args* args = new Args();
    args->lr = 0.1;
    args->momentum = 0.9;
    args->nesterov = true;


    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, 50));
    std::vector<HyperbolicVector*>*  moments = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, 50));
    std::vector<HyperbolicVector*>*  buffer = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, (double) 0));
    RSGD* optim = new RSGD(args, params, moments, buffer);
    std::vector<HyperbolicVector*>* grads = new std::vector<HyperbolicVector*>(1, new DoubleVector(2, (double) 0));
    std::vector<int> indexes = std::vector<int>(1, 0);
    std::vector<double> tmp = std::vector<double>(2);
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 2; j++) tmp[j] = params->at(0)->coordinates[j];
        std::vector<double> diff = NumericalDifferentiate::diff(&norm_minus, &tmp);
        for (int j = 0; j < 2; j++) grads->at(0)->coordinates[j] = diff[j];
        optim->step(&indexes, grads, 0);
    }

    ASSERT_LT(abs(params->at(0)->coordinates[0] - 1), 10 * EQ_THRES);
    ASSERT_LT(abs(params->at(0)->coordinates[1] - 6), 10 * EQ_THRES);
}


PoincareVector::PoincareManifold sgdpmf(1.0, HDIM);
PoincareVector generateRandomPoincare() {
    std::vector<double>* values = new std::vector<double>(HDIM, 0);
    for(int i = 0; i < HDIM; i++) values->at(i) = 0.05 * ((double) std::rand()) / RAND_MAX;
    PoincareVector prand = PoincareVector(&sgdpmf, HDIM, values);
    prand.project();
    return prand;
}

TEST(stepTest, HyperbolicVanilla) {

    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = false;


    std::srand(SEED);

    // init vector
    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>();
    std::vector<double>* init = new std::vector<double>(HDIM, 0.3);
    PoincareVector pvector = PoincareVector(&sgdpmf, HDIM, init);
    pvector.project();
    params->push_back(&pvector);


    // target
    PoincareVector target = generateRandomPoincare();

    // init optim
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    std::vector<HyperbolicVector*>* buffers = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    RSGD optim = RSGD(args, params, moments, buffers);
    std::pair<HyperbolicVector*, HyperbolicVector*> grads = std::pair<HyperbolicVector*, HyperbolicVector*>(
            new PoincareVector(&sgdpmf, HDIM, (double) 0),
            new PoincareVector(&sgdpmf, HDIM, (double) 0)
    );
    std::vector<int> indexes = std::vector<int>(1, 0);

    // loop
    std::vector<HyperbolicVector*>* gradarray = new std::vector<HyperbolicVector*>(1, grads.first);
    for (int i = 0; i < 10000; i++) {
        params->at(0)->diffDist(&target, grads, 1.0);
        optim.step(&indexes, gradarray, 0);
    }

    ASSERT_LT(params->at(0)->dist(target), EQ_THRES);

}

TEST(stepTest, HyperbolicVanillaNoMomentum) {

    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0;
    args->nesterov = false;


    std::srand(SEED);

    // init vector
    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>();
    std::vector<double>* init = new std::vector<double>(HDIM, 0.3);
    PoincareVector pvector = PoincareVector(&sgdpmf, HDIM, init);
    pvector.project();
    params->push_back(&pvector);


    // target
    PoincareVector target = generateRandomPoincare();

    // init optim
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    std::vector<HyperbolicVector*>* buffers = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    RSGD optim = RSGD(args, params, moments, buffers);
    std::pair<HyperbolicVector*, HyperbolicVector*> grads = std::pair<HyperbolicVector*, HyperbolicVector*>(
            new PoincareVector(&sgdpmf, HDIM, (double) 0),
            new PoincareVector(&sgdpmf, HDIM, (double) 0)
    );
    std::vector<int> indexes = std::vector<int>(1, 0);

    // loop
    std::vector<HyperbolicVector*>* gradarray = new std::vector<HyperbolicVector*>(1, grads.first);
    for (int i = 0; i < 10000; i++) {
        params->at(0)->diffDist(&target, grads, 1.0);
        optim.step(&indexes, gradarray, 0);
    }

    ASSERT_LT(params->at(0)->dist(target), EQ_THRES);
}


TEST(stepTest, HyperbolicNesterov) {


    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = true;


    std::srand(SEED);

    // init vector
    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>();
    std::vector<double>* init = new std::vector<double>(HDIM, 0.3);
    PoincareVector pvector = PoincareVector(&sgdpmf, HDIM, init);
    pvector.project();
    params->push_back(&pvector);


    // target
    PoincareVector target = generateRandomPoincare();

    // init optim
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    std::vector<HyperbolicVector*>* buffers = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    RSGD optim = RSGD(args, params, moments, buffers);
    std::pair<HyperbolicVector*, HyperbolicVector*> grads = std::pair<HyperbolicVector*, HyperbolicVector*>(
            new PoincareVector(&sgdpmf, HDIM, (double) 0),
            new PoincareVector(&sgdpmf, HDIM, (double) 0)
    );
    std::vector<int> indexes = std::vector<int>(1, 0);

    // loop
    std::vector<HyperbolicVector*>* gradarray = new std::vector<HyperbolicVector*>(1, grads.first);
    for (int i = 0; i < 1000; i++) {
        params->at(0)->diffDist(&target, grads, 1.0);
        optim.step(&indexes, gradarray, 0);
    }

    ASSERT_LT(params->at(0)->dist(target), EQ_THRES);


}

TEST(stepTest, HyperbolicVanillaAscent) {
    Args* args = new Args();
    args->lr = 0.01;
    args->momentum = 0.9;
    args->nesterov = false;


    std::srand(SEED);

    // init vector
    std::vector<HyperbolicVector*>* params = new std::vector<HyperbolicVector*>();
    std::vector<double>* init = new std::vector<double>(HDIM, 0.3);
    PoincareVector pvector = PoincareVector(&sgdpmf, HDIM, init);
    pvector.project();
    params->push_back(&pvector);


    // target
    PoincareVector target = generateRandomPoincare();

    // init optim
    std::vector<HyperbolicVector*>* moments = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    std::vector<HyperbolicVector*>* buffers = new std::vector<HyperbolicVector*>(1, new PoincareVector(&sgdpmf, HDIM, (double) 0));
    RSGD optim = RSGD(args, params, moments, buffers);
    std::pair<HyperbolicVector*, HyperbolicVector*> grads = std::pair<HyperbolicVector*, HyperbolicVector*>(
            new PoincareVector(&sgdpmf, HDIM, (double) 0),
            new PoincareVector(&sgdpmf, HDIM, (double) 0)
    );
    std::vector<int> indexes = std::vector<int>(1, 0);

    // loop
    std::vector<HyperbolicVector*>* gradarray = new std::vector<HyperbolicVector*>(1, grads.first);
    for (int i = 0; i < 1000; i++) {
        params->at(0)->diffDist(&target, grads, 1.0);
        for (int j = 0; j < HDIM; j++) grads.first->coordinates[j] = -grads.first->coordinates[j];
        optim.step(&indexes, gradarray, 0);
    }

    ASSERT_GT(params->at(0)->dist(target), 1);
}