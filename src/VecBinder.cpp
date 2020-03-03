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


#include "VecBinder.h"

#define NTREES 20

VecBinder::VecBinder(const Args* args) {

    this->voc_size = args->nvoc;
    std::cout << "Loading embedding at location: " << args->input << "\n";

    std::ifstream ifs;
    std::string line;
    LineReader lr;
    ifs.open(args->input, std::ios::in);
    std::getline(ifs, lr.line);
    lr.idx = 0;

    std::string* data = new std::string();

    VecBinder::readtill(&lr, ' ', data);
    int maxvoc = std::stoi(*data);

    VecBinder::readtill(&lr, ' ', data);
    this->dim = std::stoi(*data);

    this->kneighbors = args->kneighbors;


    this->ai = new AnnoyIndex<int, double, Angular, Kiss32Random>(this->getDimension());

    this->vectors = new matrix();
    this->vocab = new std::vector<std::string>();

    int i = 0;
    while (std::getline(ifs, lr.line) && i < voc_size) {

        lr.idx = 0;
        VecBinder::readtill(&lr, ' ', data);
        this->vectors->push_back(std::vector<double>(this->getDimension()));
        this->vocab->push_back(*data);
        for (int j = 0; j < this->getDimension(); j++) {
            VecBinder::readtill(&lr, ' ', data);
            this->vectors->at(i)[j] = std::stof(*data);
        }
        i += 1;

        std::cout << i << " / " << voc_size << "\r";
    }
    std::cout << std::endl;

    if (voc_size > this->vectors->size()) {
        std::cout << "Less words than expected (expected " << voc_size << ", current: " << this->vectors->size() << ")" << std::endl;
        voc_size = this->vectors->size();
    }

    std::cout << std::endl;

    if (i < voc_size) {
        std::cout << "Not enough word in vocabulary (doing with " << i << " words)." << std::endl;
    }

    std::cout << "Reading finished -- dim: " << this->getDimension() << " voc: " << this->vocab->size() << std::endl;

    std::cout << "Building new annoy index\n";
    this->ntrees = args->ntrees;
    for (int i = 0; i < voc_size; i++) {
        this->ai->add_item(i, &this->vectors->at(i)[0]);
    }
    this->ai->build(this->ntrees);

    std::cout << "Done\n";

    free(data);

}

void VecBinder::getRandomVector(std::pair<int, std::vector<double>*>* sample) {

    sample->first = Random::NaturalInteger(this->voc_size);
    sample->second = &this->vectors->at(sample->first);

}

void VecBinder::getRandomCloseVector(std::vector<double>* vector, int neighborhood, std::pair<int, std::vector<double>*>* sample) {

    std::vector<int>* indexes = new std::vector<int>();
    std::vector<double>* distances = new std::vector<double>();
    this->ai->get_nns_by_vector(&vector->at(0), neighborhood, this->kneighbors, indexes, distances);

    sample->first = indexes->at(Random::NaturalInteger(indexes->size()));
    sample->second = &this->vectors->at(sample->first);

    delete indexes;
    delete distances;

}

int VecBinder::getDimension() {
    return this->dim;
}

std::string VecBinder::getWordAtIndex(int idx) {
    return this->vocab->at(idx);
}

std::vector<std::string> VecBinder::getVocab() {
    return *(this->vocab);
}

void VecBinder::readtill(LineReader* input, char delimiter, std::string* buffer) {

    buffer->clear();
    while (input->idx < input->line.size() && input->line[input->idx] != delimiter) {
        buffer->push_back(input->line[input->idx]);
        input->idx++;
    }
    input->idx++;

};


