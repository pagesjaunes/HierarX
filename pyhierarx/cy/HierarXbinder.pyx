
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

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool
from cy.HierarXbinder cimport PoincareVector, HyperbolicEmbedding, HyperbolicVector

import numpy as np

cdef class PyHyperbolicEmbedding:

    cdef:
        HyperbolicEmbedding hemb;

    def __cinit__(
        self,
        path
    ):
        self.hemb = HyperbolicEmbedding.load(bytes(path, encoding='utf-8'))

    @staticmethod
    def load(path):
        return PyHyperbolicEmbedding(path)

    def dimension(self):
        return self.hemb.getDimension()

    def vocsize(self):
        return self.hemb.getVocSize()

    def __getitem__(
        self,
        idx
    ):
        if type(idx) == int:
            return self.at(idx)
        else:
            data = np.zeros((len(idx), self.dimension()))
            for i, el in enumerate(idx):
                data[i, :] = self.at(el)
            return data

    cpdef vector[double] at(
        self,
        int i
    ):
        return self.hemb.at(i).get_coordinates()

    def wordAt(
        self,
        i,
        encoding='utf-8'
    ):
        return self.cwordAt(i).decode(encoding=encoding)

    cpdef string cwordAt(
        self,
        int i
    ):
        return self.hemb.wordAt(i)

    cpdef double distance(
        self,
        int i,
        int j
    ):
        return self.hemb.at(i).dist(self.hemb.at(j)[0])

cdef class PyPoincareManifold:

    cdef:
        PoincareVector.PoincareManifold pmf;

    def __cinit__(
        self,
        double c,
        int dim,
        bool lorentzian
    ):
        self.pmf = PoincareVector.PoincareManifold(c, dim, lorentzian)

    cpdef double distance(
        self,
        vector[double] e1,
        vector[double] e2
    ):
        assert(e1.size() == e2.size())
        assert(e1.size() == self.pmf.getDim())
        return self.pmf.dist(&(e1.at(0)), &(e2.at(0)))