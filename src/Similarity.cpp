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


#include "Similarity.h"

Similarity::Similarity(Args* args) {

    hmap = new std::map<std::string, std::vector<LineFormat>*>();
    htmap = new std::map<std::pair<int, int>, double>();
    w2k = new std::map<std::string, int>();
    key2dist = new std::map<int, std::set<std::string>>();
    resamplers = new std::map<int, std::vector<int>>();
    this->symmetric = args->symmetric;

    std::ifstream ifs;
    std::string line;
    VecBinder::LineReader lr;
    std::string* data = new std::string();
    ifs.open(args->input, std::ios::in);

    // Count frequency
    int nline = 0;
    std::cout << "Reading file" << std::endl;
    std::cout << "Count items and frequencies" << std::endl;
    std::map<std::string, int> freqs = std::map<std::string, int>();
    while (std::getline(ifs, lr.line)) {
        lr.idx = 0;

        VecBinder::readtill(&lr, ',', data);
        std::string h = *data;
        VecBinder::readtill(&lr, ',', data);
        std::string t = *data;

        if (!freqs.count(h)) {
            freqs.insert({h, 0});
        }
        if (!freqs.count(t)) {
            freqs.insert({t, 0});
        }

        freqs.at(t) += 1;
        freqs.at(h) += 1;
        std::cout << nline++ << "\r";

    }
    std::cout << std::endl;

    // Keeping nvoc most frequent.
    std::cout << "Keeping " << std::fmin(args->nvoc, freqs.size()) << " most frequent" << std::endl;
    keys = new std::vector<std::pair<std::string, int>> (freqs.size());
    int i = 0;
    for (std::map<std::string, int>::iterator it = freqs.begin(); it != freqs.end(); ++it) {
        keys->at(i) = std::pair<std::string, int>(it->first, it->second);
        i++;
    }
    std::partial_sort(keys->begin(), keys->begin() + std::fmin(args->nvoc, keys->size()), keys->end(), Similarity::compare);
    keys->resize(std::fmin(args->nvoc, keys->size()));
    i = 0;
    for (std::vector<std::pair<std::string, int>>::iterator it = keys->begin(); it != keys->end(); it++) {

        hmap->insert({it->first, new std::vector<LineFormat>()});
        w2k->insert({it->first, i});
        i++;
        std::cout << i << " / " << keys->size() << "\r";
    }
    std::cout << std::endl;

    // Add only couple containing most frequent entities
    ifs.close();
    ifs.open(args->input, std::ios::in);
    i = 0;
    std::cout << "Preparing similarity pairs" << std::endl;
    std::vector<double> scores(nline);
    while (std::getline(ifs, lr.line)) {
        lr.idx = 0;

        VecBinder::readtill(&lr, ',', data);
        std::string h = *data;
        VecBinder::readtill(&lr, ',', data);
        std::string t = *data;
        VecBinder::readtill(&lr, ',', data);
        double score = std::stof(*data);
        scores[i] = score;

        if (hmap->count(h) && hmap->count(t)) {

            hmap->at(h)->push_back(LineFormat(h, t, score));
            hmap->at(t)->push_back(LineFormat(t, h, score));
            htmap->insert({{w2k->at(h), w2k->at(t)}, score});

            if (!key2dist->count(w2k->at(h))) {
                key2dist->insert({w2k->at(h), std::set<std::string>()});
            }
            if (!key2dist->at(w2k->at(h)).count(t)) {
                key2dist->at(w2k->at(h)).insert(t);
            }

            if (symmetric) {
                if (!key2dist->count(w2k->at(t))) {
                    key2dist->insert({w2k->at(t), std::set<std::string>()});
                }
                if (!key2dist->at(w2k->at(t)).count(h)) {
                    key2dist->at(w2k->at(t)).insert(h);
                }
            }

        }

        std::cout << i++ << " / " << nline <<  "\r";

    }

    std::cout << "Find median score... ";
    std::nth_element(scores.begin(), scores.begin() + scores.size() / 2, scores.end());
    this->medianScore = scores[scores.size()/2];
    std::cout << "value: " << this->medianScore << std::endl;

    std::cout << "Create Positive sampling tables" << std::endl;
    for (int i = 0; i < keys->size(); i++) {
        resamplers->insert({w2k->at(keys->at(i).first), std::vector<int>()});
        std::set<std::string> neighbors = key2dist->at(w2k->at(keys->at(i).first));
        double normsim = 0;
        for (std::set<std::string>::const_iterator it = neighbors.begin(); it != neighbors.end(); it++) {
            normsim += this->getSimilarity(w2k->at(keys->at(i).first), w2k->at(*it));
        }
        for (std::set<std::string>::const_iterator it = neighbors.begin(); it != neighbors.end(); it++) {
            int maxiter = std::ceil(RESAMPLE_SIZE * this->getSimilarity(w2k->at(keys->at(i).first), w2k->at(*it)) / normsim);
            for (int z = 0; z < maxiter; z++) {
                resamplers->at(w2k->at(keys->at(i).first)).push_back(w2k->at(*it));
            }
        }
        std::cout << (i + 1) << " / " << keys->size() <<  "\r";

    }


    std::cout << std::endl;
}

int Similarity::getVocSize() {
    return keys->size();
}

std::vector<std::string> Similarity::getVocab() {
    std::vector<std::string> _keys = std::vector<std::string>(keys->size());
    for (int i = 0; i < keys->size(); i++) {
        _keys[i] = keys->at(i).first;
    }
    return _keys;
}

void Similarity::getRandomIndex(std::pair<int, std::vector<double> *>* pair) {
    pair->first = Random::NaturalInteger(getVocSize());
}

double Similarity::getCloseSimilarity(int idxkey, std::pair<int, std::vector<double> *>* pair) {

    int ansidx = Random::NaturalInteger(resamplers->at(idxkey).size());
    pair->first = resamplers->at(idxkey)[ansidx];
    return this->getSimilarity(idxkey, pair->first);

/*    std::vector<LineFormat>* vlf = hmap->at(keys->at(idxkey).first);

    if (vlf->size() > 0) {
        int range = Random::NaturalInteger(vlf->size());
        pair->first = w2k->at(vlf->at(range).t);
        return vlf->at(range).score;
    } else {
        getRandomIndex(pair);
        return 0;
    }*/
}

int Similarity::getSize(int idxkey) {
    return hmap->at(keys->at(idxkey).first)->size();
}

bool Similarity::compare(const std::pair<std::string, int>& e1, const std::pair<std::string, int>& e2) {
    return e1.second > e2.second;
}

double Similarity::getSimilarity(int h, int t) {

    if (htmap->count(std::make_pair(h, t))) {

        return htmap->at(std::make_pair(h, t));
    } else if (htmap->count(std::make_pair(t, h)) && this->symmetric) {
        return htmap->at(std::make_pair(t, h));
    } else {
        return 0;
    }
}

double Similarity::getMedianScore() {
    return medianScore;
}





