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

#include <cstdlib>
#include "gtest/gtest.h"
#include "../src/HyperbolicEmbedding.h"
#include "../src/Random.h"

/*
TEST(saveLoadTest, IO) {

    std::vector<std::string> vocab = std::vector<std::string>();

    for (int i = 0; i < 10; i++) {
        if (i %2 ==  0) {
            vocab.push_back("bonjour");
        } else {
            vocab.push_back("bye");
        }
    }

    HyperbolicEmbedding pemb = HyperbolicEmbedding(4, vocab);
    for (int i = 0; i < 10; i++) {
        std::vector<double> test = Random::DoubleVector(-1, 1, 4);
        pemb.vectors->at(i) = PoincareVector(4, &test);
    }
    pemb.save("testsave.hbin");

    HyperbolicEmbedding pembloaded = HyperbolicEmbedding::load("testsave.hbin");

    ASSERT_EQ(pembloaded.getVocSize(), pemb.getVocSize());
    ASSERT_EQ(pembloaded.getDimension(), pemb.getDimension());
    for (int i = 0; i < pemb.getVocSize(); i++) {
        std::string wordLoaded = pembloaded.wordAt(i);
        std::string wordOrigin = pemb.wordAt(i);

        ASSERT_EQ(wordLoaded.size(), wordOrigin.size());
        for (int j = 0; j < wordOrigin.size(); j++) {
            ASSERT_EQ(wordLoaded.at(j), wordOrigin.at(j));
        }
        for (int j = 0; j < pemb.getDimension(); j++){
            ASSERT_EQ(pembloaded.at(i)->get_coordinates().at(j), pemb.at(i)->get_coordinates().at(j));
        }
    }

}*/