
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

cdef extern from "Args.cpp":
    pass

cdef extern from "Args.h" nogil:
    cdef cppclass Args:
        pass


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

cdef extern from "HyperbolicEmbedding.cpp":
    pass

cdef extern from "HyperbolicEmbedding.h" nogil:

    cdef cppclass FILE_FORMAT:
        pass

    cdef FILE_FORMAT fformat_vec "HyperbolicEmbedding::FILE_FORMAT::vec";
    cdef FILE_FORMAT fformat_bin "HyperbolicEmbedding::FILE_FORMAT::bin";

    cdef cppclass HyperbolicEmbedding:

        @staticmethod
        HyperbolicEmbedding load(const char*);

        void save(const char*, FILE_FORMAT);
        HyperbolicVector* at(int) const;
        string wordAt(int) const;
        int getDimension() const;
        int getVocSize() const;