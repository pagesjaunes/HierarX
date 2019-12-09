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
#include "../src/EuclideanGeometry.h"
#include "../src/NumericalDifferentiate.h"

#define DIFF_THRES 1e-8
#define VDIM 10
#define N_TEST 1000
#define SEED 42

double xsquare(double x) {
    return x * x;
}

double polynom(double x) {
    return 3 * x * x * x - 8 * x * x + x + 4;
}

TEST(diffTest, Scalar) {

    double RANGE[] = {-8, -4, -2, -1, 0, 1, 2, 4, 8};

    for (double x : RANGE) {
        ASSERT_LT(abs(NumericalDifferentiate::diff(&xsquare, x) - 2 * x), DIFF_THRES);
        ASSERT_LT(abs(NumericalDifferentiate::diff(&std::exp, x) - std::exp(x)), DIFF_THRES * 100);
        ASSERT_LT(abs(NumericalDifferentiate::diff(&polynom, x) - (9 * x * x - 16 * x + 1)), DIFF_THRES);
        ASSERT_LT(abs(NumericalDifferentiate::diff(&std::log, 9 + x) - 1 / (9 + x)), DIFF_THRES);
        ASSERT_LT(abs(NumericalDifferentiate::diff(&std::sin, x) - std::cos(x)), DIFF_THRES);
        ASSERT_LT(abs(NumericalDifferentiate::diff(&std::cos, x) + std::sin(x)), DIFF_THRES);
    }

}

std::vector<double> generateVectorRandom() {
    std::vector<double> values = std::vector<double>(VDIM, 0);
    for(int i = 0; i < VDIM; i++) values[i] = 5 * ((double) std::rand()) / RAND_MAX;
    return values;
}

TEST(diffTest, LinearForm) {

    std::srand(SEED);

    for (int i = 0; i < N_TEST; i++) {
        std::vector<double> x0 = generateVectorRandom();
        std::vector<double> diff = NumericalDifferentiate::diff(&EuclideanGeometry::normsquare, &x0);
        for (int j = 0; j < x0.size(); j++) {
            ASSERT_LT(abs(diff[j] - 2 * x0[j]), DIFF_THRES * 1e5);
        }
    }

}

void elmult(matrix* mat, double* tmp) {
    std::vector<double> a = std::vector<double>(EuclideanGeometry::elementWiseProduct(&mat->at(0), &mat->at(1)));
    for (int i = 0; i < a.size(); i++)
        tmp[i] = a[i];
}

TEST(diffTest, Tensor) {

    std::srand(SEED);

    for (int i = 0; i < N_TEST; i++) {

        std::vector<double> x0 = generateVectorRandom();
        std::vector<double> y0 = generateVectorRandom();
        matrix* mat0 = new matrix();
        mat0->push_back(x0);
        mat0->push_back(y0);

        std::vector<matrix> diff = NumericalDifferentiate::diff(&elmult, mat0, VDIM);

        ASSERT_EQ(diff.size(), VDIM);
        ASSERT_EQ(diff.at(0).size(), 2);
        ASSERT_EQ(diff.at(0)[0].size(), VDIM);
        for (int j = 0; j < VDIM; j++) {
            double test = diff.at(j)[0][j];
            double oth = y0[j];

            ASSERT_LT(abs(diff.at(j)[0][j] - y0[j]), DIFF_THRES * 1e2);
            ASSERT_LT(abs(diff.at(j)[1][j] - x0[j]), DIFF_THRES * 1e2);
            for (int k = 0; k < VDIM; k++) {
                if (k != j) {
                    ASSERT_LT(abs(diff.at(k)[0][j]), DIFF_THRES * 1e2);
                    ASSERT_LT(abs(diff.at(k)[0][j]), DIFF_THRES * 1e2);
                }
            }
        }
    }

}