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


#ifndef HIERARX_HYPERBOLICEMBEDDING_H
#define HIERARX_HYPERBOLICEMBEDDING_H

#include <vector>
#include <fstream>
#include "Args.h"
#include "HyperbolicVector.h"
#include "PoincareVector.h"
#include "PoincareStack.h"
#include "LorentzVector.h"
#include "VecBinder.h"
#include "Utils.h"


class HyperbolicEmbedding {

private:
    int dim;
    int format;
    std::vector<std::string>* vocab;

public:

    enum FILE_FORMAT {
        vec,
        bin
    };

    std::vector<HyperbolicVector*>* vectors;

    HyperbolicEmbedding();
    HyperbolicEmbedding(const Args*, std::vector<std::string>, bool);
    ~HyperbolicEmbedding();

    int getDimension() const;
    int getVocSize() const;
    void toPoincare();
    void toLorentz();

    HyperbolicVector& operator[](int i) const;
    HyperbolicVector* at(int i) const;
    std::string wordAt(int i) const;
    void save(const char*);
    void save(const char*, FILE_FORMAT);

    static HyperbolicEmbedding load(const char*);

};

namespace hyp {
    const std::vector<std::string> HYPERBOLIC_SPACE_NAME({"Poincare", "PoincareStack", "Lorentz"});
}

#endif //HIERARX_HYPERBOLICEMBEDDING_H
