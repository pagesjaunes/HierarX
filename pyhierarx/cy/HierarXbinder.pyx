
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
from cy.HierarXbinder cimport Args, Similarity, VecBinder, MainProcessor, RSGD, PoincareVector, HyperbolicEmbedding, HyperbolicVector, fformat_vec

import numpy as np
import os
import sys


cdef class PyHyperbolicEmbedding:

    cdef:
        HyperbolicEmbedding hemb;

    def __cinit__(
        self,
        path
    ):
        if isinstance(path, str):
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

    cdef void set_hemb(
        self,
        HyperbolicEmbedding* h):

        self.hemb = h[0]

    def save_vec(
        self,
        path
    ):
        self.hemb.save(bytes(path, encoding='utf-8'), fformat_vec)

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


cdef class PyExperiment:

    cdef:
        Args* __cargs;
        HyperbolicEmbedding* pemb;
        HyperbolicEmbedding* buffers;
        HyperbolicEmbedding* momentum;
        Similarity* sims;
        VecBinder* vb;
        vector[string] vocab;
        MainProcessor mp;

    def __cinit__(
        self,
        args,
        record_args = True,
        verbose = 1):


        self.__cargs = new Args()

        self.__cargs.nvoc = args.nvoc
        self.__cargs.dim = args.dim
        self.__cargs.nthread = args.nthread
        self.__cargs.expdir = bytes(args.expdir, encoding='utf-8')
        self.__cargs.input = bytes(args.input, encoding='utf-8')


        self.__cargs.niter = args.niter
        self.__cargs.bs = args.bs
        self.__cargs.sampling = args.sampling
        self.__cargs.checkpoint = args.checkpoint
        self.__cargs.kneighbors = args.kneighbors
        self.__cargs.ntrees = args.ntrees
        self.__cargs.rebuild = args.rebuild
        self.__cargs.weighted = args.weighted

        self.__cargs.lr = args.lr
        self.__cargs.momentum = args.momentum
        self.__cargs.plateau = args.plateau
        self.__cargs.posthres = args.posthres
        self.__cargs.celerity = args.celerity
        self.__cargs.minlr = args.minlr
        self.__cargs.maxposthres = args.maxposthres
        self.__cargs.alpha = args.alpha
        self.__cargs.similarity = args.similarity
        self.__cargs.nesterov = args.nesterov
        self.__cargs.symmetric = args.symmetric
        self.__cargs.continue__ = args.continue__
        self.__cargs.declr = args.declr
        self.__cargs.movie = args.movie
        self.__cargs.lorentzian = args.lorentzian
        self.__cargs.hmode = bytes(args.hmode, encoding='utf-8')

        self.__cargs.pmf = new PoincareVector.PoincareManifold(self.__cargs.celerity, self.__cargs.dim, self.__cargs.lorentzian)

        if record_args:
            self.__cargs.record(bytes(os.path.join(args.expdir, "args.csv"), encoding='utf-8'))

        self.__cargs.setformat()

        if verbose >= 1:
            self.__cargs.printOut()

    def set_run_params(
        self,
        niter = None,
        posthres = None,
        lr = None
    ):
        if niter is not None:
            self.__cargs.niter = niter

        if posthres is not None:
            self.__cargs.posthres = posthres

        if lr is not None:
            self.__cargs.lr = lr

    def init_experiment(
        self,
        init_similarity = True,
        init_vectors = True):

        self.__init_experiment(init_similarity, init_vectors)


    cpdef void __init_experiment(
        self,
        bool init_similarity,
        bool init_vectors):

        if init_similarity:
            self.sims = new Similarity(self.__cargs)

        self.vocab = self.sims.getVocab()

        if init_vectors:
            self.pemb = new HyperbolicEmbedding(self.__cargs, self.vocab, True)
            self.momentum = new HyperbolicEmbedding(self.__cargs, self.vocab, True)


    cpdef double train(
        self):

        self.mp = MainProcessor(self.pemb, self.momentum, self.vb, self.sims, self.__cargs)
        return self.mp.threadedTrain(self.__cargs)


    def get_embedding(
        self):
        pyhemb = PyHyperbolicEmbedding(None)
        pyhemb.set_hemb(self.pemb)
        return pyhemb


