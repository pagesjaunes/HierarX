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


#include "HyperbolicEmbedding.h"

HyperbolicEmbedding::HyperbolicEmbedding(const Args* args, std::vector<std::string> vocab, bool random) {
    this->dim = args->dim;
    this->vocab = new std::vector<std::string>(vocab);
    this->vectors = new std::vector<HyperbolicVector*>();

    for (int i = 0; i < this->vocab->size(); i++) {

        HyperbolicVector* hvec;

        switch (args->format) {
            case hierarx::HYPERBOLIC_SPACE::Poincare:
                hvec = new PoincareVector(args->pmf, this->dim);
                break;
            case hierarx::HYPERBOLIC_SPACE::Poincare_Stack:
                hvec = new PoincareStack(this->dim);
                break;
            case hierarx::HYPERBOLIC_SPACE::Lorentz:
                hvec = new LorentzVector(args->celerity, this->dim);
                break;
            default:
                throw "Unimplemented hyperbolic space";
        }

        if (random) {
            hvec->randomize();
        }
        this->vectors->push_back(hvec);
        this->vectors->at(i)->project();

    }

    this->format = args->format;

}

HyperbolicEmbedding::HyperbolicEmbedding() {
    this->dim = -1;
    this->vocab = new std::vector<std::string>();
    this->vectors = new std::vector<HyperbolicVector*>();
}

HyperbolicEmbedding::~HyperbolicEmbedding() {
}


int HyperbolicEmbedding::getVocSize() const {
    return this->vocab->size();
}

int HyperbolicEmbedding::getDimension() const {
    return this->dim;
}

HyperbolicVector& HyperbolicEmbedding::operator[](int i) const {
    assert(i < this->getVocSize());
    return *(this->vectors->at(i));
}

HyperbolicVector* HyperbolicEmbedding::at(int i) const {
    return this->vectors->at(i);
}

void HyperbolicEmbedding::save(const char* filename) {

    std::ofstream ofs;
    std::string __filename(filename);

    ofs.open((__filename + ".bin").c_str(), std::ios::out);
    ofs.write((char*) &this->dim, sizeof(int));
    int vocsize = this->getVocSize();
    ofs.write((char*) &(vocsize), sizeof(int));
    ofs.write((char*) &this->format, sizeof(int));

    int subspaceDim;
    double celerity;
    switch (this->format) {
        case hierarx::HYPERBOLIC_SPACE::Poincare:
            celerity = dynamic_cast<PoincareVector*>(this->vectors->at(0))->manifold->getCelerity();
            ofs.write((char*) &celerity, sizeof(double));
            break;
        case hierarx::HYPERBOLIC_SPACE::Poincare_Stack:
            subspaceDim = PoincareStack::SUBSPACE_DIM::COMMON;
            ofs.write((char*) &subspaceDim, sizeof(int));
            break;
        case hierarx::HYPERBOLIC_SPACE::Lorentz:
            celerity = dynamic_cast<LorentzVector*>(this->vectors->at(0))->getCelerity();
            ofs.write((char*) &celerity, sizeof(double));
            break;
        default:
            throw "Unimplemented hyperbolic space";
    }

    for (int i = 0; i < this->getVocSize(); i++) {
        std::string word = this->vocab->at(i);
        ofs.write(word.data(), word.size() * sizeof(char));
        ofs.put(0);
        this->vectors->at(i)->write(ofs);
    }

    ofs.flush();
    ofs.close();


}

HyperbolicEmbedding HyperbolicEmbedding::load(const char* filename) {

    std::ifstream ifs;
    Args args = Args();

    ifs.open(filename, std::ios::in);
    if (!ifs.good()) {
        throw "file is not good";
    }

    int coordsDim, metaarg;
    ifs.read((char*) &args.dim, sizeof(int));
    ifs.read((char*) &args.nvoc, sizeof(int));
    ifs.read((char*) &args.format, sizeof(int));

    switch (args.format) {
        case hierarx::HYPERBOLIC_SPACE::Poincare:
            ifs.read((char*) &args.celerity, sizeof(double));
            coordsDim = args.dim;
            args.pmf = new PoincareVector::PoincareManifold(args.celerity, args.dim);
            std::cout << "Poincare space (celerity: " << args.celerity << ")";
            break;
        case hierarx::HYPERBOLIC_SPACE::Poincare_Stack:
            ifs.read((char*) &metaarg, sizeof(int));
            coordsDim = args.dim * metaarg;
            std::cout << "Poincare Stack (subspace dim: " << metaarg << ")";
            break;
        case hierarx::HYPERBOLIC_SPACE::Lorentz:
            ifs.read((char*) &args.celerity, sizeof(double));
            metaarg = 0;
            coordsDim = args.dim + 1;
            std::cout << "Lorentz space";
            break;
        default:
            throw "Unimplemented hyperbolic space";
    }
    std::cout << std::endl;
    std::cout << "filename: " << filename << std::endl
    << "voc size: " << args.nvoc << std::endl
    << "dim size: " << args.dim << std::endl
    << "Using " << hyp::HYPERBOLIC_SPACE_NAME[args.format] << " Hyperbolic space." << std::endl;

    std::vector<std::string> vocab = std::vector<std::string>(args.nvoc);
    std::vector<HyperbolicVector*> vectors = std::vector<HyperbolicVector*>();

    for (int i = 0; i < args.nvoc; i++) {

        // extract word
        std::string word = std::string();
        char buff;
        do {
            ifs.read(&buff, sizeof(char));
            if (buff != 0) word.push_back(buff);
        } while(buff != 0);
        vocab[i] = std::string(word);

        // extract coords
        std::vector<double> coords = std::vector<double>(coordsDim);
        for (int j = 0; j < coordsDim; j++) ifs.read((char*) &coords.at(j), sizeof(double));

        HyperbolicVector* hvec;
        switch(args.format) {
            case hierarx::HYPERBOLIC_SPACE::Poincare:
                hvec = new PoincareVector(args.pmf, args.dim, coords);
                break;
            case hierarx::HYPERBOLIC_SPACE::Poincare_Stack:
                hvec = new PoincareStack(args.dim, metaarg, coords);
                break;
            case hierarx::HYPERBOLIC_SPACE::Lorentz:
                hvec = new LorentzVector(args.celerity, args.dim, coords);
                break;
            default:
                throw "Unimplemented hyperbolic space";
        }

        vectors.push_back(hvec);
        std::cout << "words: " << (i + 1) << " / " << args.nvoc << "\r";

    }
    std::cout << std::endl;

    ifs.close();

    HyperbolicEmbedding loaded = HyperbolicEmbedding(&args, vocab, false);
    loaded.vectors = new std::vector<HyperbolicVector*>(vectors);

    return loaded;

}

std::string HyperbolicEmbedding::wordAt(int i) const {
    return this->vocab->at(i);
}

void HyperbolicEmbedding::save(const char* filename, HyperbolicEmbedding::FILE_FORMAT format) {

    std::string fn(filename);
    std::string __filename(filename);

    if(fn.find_last_of(".") < fn.size()) {
        __filename = fn.substr(0, fn.find_last_of('.'));
    }

    std::cout << __filename << std::endl;

    if (format == HyperbolicEmbedding::FILE_FORMAT::bin) {
        this->save(__filename.c_str());
    } else {

        std::ofstream ofs;
        int vocsize = this->getVocSize();
        int dimension = this->dim;
        if (this->format == hierarx::HYPERBOLIC_SPACE::Lorentz) {
            dimension = this->dim + 1;
        }

        ofs.open((__filename + ".vec").c_str(), std::ios::out);
        ofs << std::to_string(vocsize) << " " << std::to_string(dimension) << this->at(0)->metaInfo() << " " << hyp::HYPERBOLIC_SPACE_NAME[this->format] << std::endl;
        for (int i = 0; i < vocsize; i++) {
            ofs << this->vocab->at(i);
            ofs << " " << this->vectors->at(i)->stringify();
            ofs << std::endl;
        }
        ofs.flush();
        ofs.close();


    }

}

void HyperbolicEmbedding::toPoincare() {

    if (this->format == hierarx::HYPERBOLIC_SPACE::Poincare) {
        std::cout << "Embeddinng is already Poincare, no conversion required." << std::endl;
    } else if (this->format == hierarx::HYPERBOLIC_SPACE::Lorentz) {
        this->format = hierarx::HYPERBOLIC_SPACE::Poincare;
        for (int i = 0; i < this->getVocSize(); i++) {
            double normalize = this->vectors->at(i)->coordinates[0] + 1;
            for (int j = 0; j < this->dim; j++) {
                this->vectors->at(i)->coordinates[j] = this->vectors->at(i)->coordinates[j + 1] / normalize;
            }
            this->vectors->at(i)->dim -= 1;
        }
    } else {
        throw "Conversion unavailable for this embedding mode.";
    }

}

void HyperbolicEmbedding::toLorentz() {
    if (this->format == hierarx::HYPERBOLIC_SPACE::Lorentz) {
        std::cout << "Embeddinng is already Poincare, no conversion required." << std::endl;
    } else if (this->format == hierarx::HYPERBOLIC_SPACE::Poincare) {
        this->format = hierarx::HYPERBOLIC_SPACE::Lorentz;
        for (int i = 0; i < this->getVocSize(); i++) {
            double nsv = EuclideanGeometry::normsquare(this->vectors->at(i)->coordinates, this->dim);
            double normalize = 1 - nsv;
            double* newvec = new double[this->dim + 1];
            *newvec = 1 + nsv / normalize;
            for (int j = 1; j < this->dim + 1; j++) {
                *(newvec + i) = 2 * this->vectors->at(i)->coordinates[j-1] / normalize;
            }
            //free(this->vectors->at(i)->coordinates);
            this->vectors->at(i)->coordinates = newvec;
        }
    } else {
        throw "Conversion unavailable for this embedding mode.";
    }
}




