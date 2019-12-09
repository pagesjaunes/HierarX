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
#include "../src/VecBinder.h"
#include <cstdlib>
#include <cstring>
#include <memory>
/*
const char* EMBEDDINGS = std::getenv("EMBEDDINGS");


TEST(fasttextLoading, Construct) {

    char* local_path = (char*) malloc(sizeof(char) * (std::strlen(EMBEDDINGS)+1));
    std::strcpy(local_path, EMBEDDINGS);
    std::strcat(local_path, "/wikien20.bin");
    VecBinder ftb(100, local_path, "./tree.bin", true);

}

TEST(fasttextLoading, NoConstruct) {

    char* local_path = (char*) malloc(sizeof(char) * (std::strlen(EMBEDDINGS)+1));
    std::strcpy(local_path, EMBEDDINGS);
    std::strcat(local_path, "/wikien20.bin");
    VecBinder ftb(100, local_path, "./tree.bin", false);

}

*/
