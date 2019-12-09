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


#include "Hierarchy.h"
#define NEIGHBOR 100
#define MIN_RADIUS 0.05

Hierarchy::Hierarchy(HyperbolicEmbedding *pemb) {

    this->pemb = pemb;
    this->distances = std::vector<Node>(pemb->getVocSize());

}

void Hierarchy::process(int start, int end, int* count) {
    for (int i = start; i < std::fmin(end, this->distances.size()); i++) {
        std::vector<double> ivec = pemb->at(i)->get_coordinates();
        for (int j = 0; j < this->distances.size(); j++) {
            if (j != i) {
                std::vector<double> jvec = pemb->at(j)->get_coordinates();
                this->distances[j].idx = j;
                this->distances[j].pdistance = pemb->at(i)->dist(*(pemb->vectors->at(j)));
                this->distances[j].normdiff = EuclideanGeometry::norm(&ivec) - EuclideanGeometry::norm(&jvec);
            }

        }

        std::nth_element(
                this->distances.begin(),
                this->distances.begin() + NEIGHBOR,
                this->distances.end(),
                &Hierarchy::cmp
                );
        std::cout << "\n" << this->pemb->wordAt(i) << " - " << this->pemb->wordAt(this->distances[0].idx) << "\n";

        *(count) += 1;
    }
}

void Hierarchy::threadedProcess(int nthreads) {

    std::vector<std::thread> threads;
    std::vector<int>* counts = new std::vector<int>(nthreads);
    int totalThread =  this->distances.size() / nthreads;

    std::cout << "Start threading" << std::endl;
    for (int i = 0; i < nthreads; i++) {
        std::cout << totalThread * i << " -> " << totalThread * (i + 1) << std::endl;
        threads.push_back(
                std::thread([=]() { this->process(totalThread * i, totalThread * (i + 1), &(counts->at(i))); }));
    }

    int totalCount = EuclideanGeometry::sum(counts);
    do {

        std::cout << "Global: " << totalCount;
        for (int i = 0; i < nthreads; i++) {
            std::cout << " -- Thread-" << i + 1 << ": " << counts->at(i);
        }
        std::cout << "\r";
        usleep(500);
        totalCount = EuclideanGeometry::sum(counts);

    } while (totalCount < this->distances.size());

    for (int i = 0; i < nthreads; i++) threads[i].join();
    std::cout << std::endl;


}

void Hierarchy::save(const char* filename) {

    std::ofstream ofs;
    ofs.open(filename, std::ios::out);

    int vocsize = this->distances.size();
    ofs.write((char*) &(vocsize), sizeof(int));
    for (int i = 0; i < vocsize; i++) {
        for (int j = i + 1; j < vocsize; j++) {
            ofs.write((char*) &(this->distances[i]), sizeof(double));
        }
    }
    ofs.flush();
    ofs.close();

}

matrix Hierarchy::load(const char* filename) {

    std::ifstream ifs;
    ifs.open(filename, std::ios::in);

    int vocsize;
    ifs.read((char*) &vocsize, sizeof(int));

    std::cout << "filename: " << filename << std::endl
              << "voc size: " << vocsize << std::endl;

    matrix distances = matrix();

    for (int i = 0; i < vocsize; i++) {

        // extract coords
        std::vector<double> coords = std::vector<double>(vocsize);
        for (int j = i + 1; j < vocsize; j++) ifs.read((char*) &coords[j], sizeof(double));

        distances.push_back(coords);

    }

    ifs.close();

    assert(distances.size() == vocsize);

    return distances;
}

bool Hierarchy::cmp(const Hierarchy::Node &a, const Hierarchy::Node &b) {
    return a.pdistance < b.pdistance;
}

