
"""
HierarX is a software aiming to build hyperbolic word representations.
Copyright (C) 2019 Solocal-SA and CNRS.

DO NOT REMOVE THIS HEADER EVEN AFTER MODIFYING HIERARX SOURCES.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

HierarX was developped by: François Torregrossa, Guillaume Gravier, Vincent Claveau, Nihel Kooli.
Contact: ftorregrossa@solocal.com, francois.torregrossa@irisa.fr
"""

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "UserInterface.cpp":
    pass

cdef extern from "UserInterface.h" nogil:
    cdef cppclass UserInterface:
        pass

cdef extern from "BatchMaker.cpp":
    pass

cdef extern from "BatchMaker.h" nogil:
    cdef cppclass BatchMaker:
        pass

cdef extern from "VecBinder.cpp":
    pass

cdef extern from "VecBinder.h" nogil:
    cdef cppclass VecBinder:
        VecBinder();

cdef extern from "Similarity.cpp":
    pass

cdef extern from "Similarity.h" nogil:
    cdef cppclass Similarity:

        Similarity();
        Similarity(Args*);

        vector[string] getVocab();

cdef extern from "Random.cpp":
    pass

cdef extern from "Random.h" nogil:
    cdef cppclass Random:
        pass

cdef extern from "NumericalDifferentiate.cpp":
    pass

cdef extern from "NumericalDifferentiate.h" nogil:
    cdef cppclass NumericalDifferentiate:
        pass

cdef extern from "EuclideanGeometry.cpp":
    pass

cdef extern from "EuclideanGeometry.h" nogil:
    cdef cppclass EuclideanGeometry:
        pass

cdef extern from "HyperbolicVector.cpp":
    pass

cdef extern from "HyperbolicVector.h" nogil:
    cdef cppclass HyperbolicVector:

        HyperbolicVector();
        vector[double] get_coordinates() const;
        double dist(HyperbolicVector&) const

cdef extern from "PoincareStack.cpp":
    pass

cdef extern from "PoincareStack.h" nogil:
    cdef cppclass PoincareStack:
        pass

cdef extern from "LorentzVector.cpp":
    pass

cdef extern from "LorentzVector.h" nogil:
    cdef cppclass LorentzVector:
        pass

cdef extern from "PoincareVector.cpp":
    pass

cdef extern from "PoincareVector.h" nogil:

    cdef cppclass PoincareVector:

        cppclass PoincareManifold:

            PoincareManifold();
            PoincareManifold(double, int, bool);

            double dist(const double*, const double*);
            int getDim();



cdef extern from "Args.cpp":
    pass

cdef extern from "Args.h" nogil:
    cdef cppclass Args:
        # required
        int nvoc;
        int dim;
        int nthread;
        string expdir;
        string input;

        # optional
        int niter;
        int bs;
        int sampling;
        int format;
        int checkpoint;
        int kneighbors;
        int ntrees;
        int rebuild;
        int weighted;
        PoincareVector.PoincareManifold* pmf;
        double lr;
        double momentum;
        double plateau;
        double posthres;
        double celerity;
        double minlr;
        double maxposthres;
        double alpha;
        bool similarity;
        bool nesterov;
        bool symmetric;
        bool continue__;
        bool declr;
        bool movie;
        bool lorentzian;
        string hmode;

        void record(const char*);
        void printOut();
        void setformat();

cdef extern from "HyperbolicEmbedding.cpp":
    pass

cdef extern from "HyperbolicEmbedding.h" nogil:

    cdef cppclass FILE_FORMAT:
        pass

    cdef FILE_FORMAT fformat_vec "HyperbolicEmbedding::FILE_FORMAT::vec";
    cdef FILE_FORMAT fformat_bin "HyperbolicEmbedding::FILE_FORMAT::bin";

    cdef cppclass HyperbolicEmbedding:

        vector[HyperbolicVector*]* vectors;

        HyperbolicEmbedding();
        HyperbolicEmbedding(const Args*, vector[string], bool);

        @staticmethod
        HyperbolicEmbedding load(const char*);

        void save(const char*, FILE_FORMAT);
        HyperbolicVector* at(int) const;
        string wordAt(int) const;
        int getDimension() const;
        int getVocSize() const;



cdef extern from "RSGD.cpp":
    pass

cdef extern from "RSGD.h" nogil:
    cdef cppclass RSGD:

        RSGD();
        RSGD(Args*, vector[HyperbolicVector*]*, vector[HyperbolicVector*]*, vector[HyperbolicVector*]*);


cdef extern from "MainProcessor.cpp":
   pass

cdef extern from "MainProcessor.h" nogil:
   cdef cppclass MainProcessor:

       MainProcessor();
       MainProcessor(HyperbolicEmbedding*, HyperbolicEmbedding*, VecBinder*, Similarity*, const Args*);

       double threadedTrain(const Args*);