
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

import subprocess
import numpy as np
from pyhierarx.cy import PyHyperbolicEmbedding, PyPoincareManifold

class HyperbolicVec:

    @staticmethod
    def load(
        path
    ):
        format = path.split('.')[-1]
        if format == 'bin':
            return HyperbolicVec.__binloading(path)
        elif format == 'vec':
            print('Only Poincare is supported for vec format')
            return HyperbolicVec.__vecloading(path)
        else:
            raise ValueError('Invalid file format')

    @staticmethod
    def __binloading(
            f
    ):
        hemb = PyHyperbolicEmbedding(f)
        vocsize, dim = hemb.vocsize(), hemb.dimension()

        cel = None
        vocabulary = [hemb.wordAt(i) for i in range(vocsize)]
        return  HyperbolicVec(cel, vocabulary, hemb)

    @staticmethod
    def __vecloading(
            f
    ):
        with open(f, 'r') as stream:
            w, d, _, c, _ = stream.readline().split()
            w, d= int(w), int(d)

            manifold = PyPoincareManifold(c, d)
            vocabulary = []
            data = np.zeros((w, d))
            for i, l in enumerate(stream.readlines()):
                vectors = l.replace('\n', '').split()
                vocabulary.append(vectors[0])
                for j, x in enumerate(vectors[1:]):
                    data[i, j] = float(x)

            return HyperbolicVec(manifold, vocabulary, data)

    def __init__(
        self,
        manifold,
        vocabulary,
        data
    ):
        self.word2idx = {w : i for i, w in enumerate(vocabulary)}
        self.vocabulary = vocabulary
        self.manifold = manifold
        self.data = data

    def is_bin_mode(self):
        return self.manifold is None

    @property
    def vocsize(self):
        return len(self.vocabulary)

    @property
    def dim(self):
        if self.is_bin_mode():
            return self.data.dimension()
        else:
            return self.data.shape[1]

    def project(self, w):
        if isinstance(w, list):
            return self.data[[self.word2idx[x] for x in w]]
        return self.data[self.word2idx[w]]

    def distance(self, w1, w2):
        if not self.is_bin_mode():
            e1, e2 = self.project(w1), self.project(w2)
            return self.manifold.distance(list(e1), list(e2))
        else:
            return self.data.distance(self.word2idx[w1], self.word2idx[w2])

    def cosine(self, w1, w2):
        e1, e2 = self.project(w1), self.project(w2)
        return np.dot(e1, e2) / np.linalg.norm(e1) / np.linalg.norm(e2)

    def is_a(self, w1, w2, alpha=1000):
        e1, e2 = self.project(w1), self.project(w2)
        return -(1 + alpha * (np.linalg.norm(e2) - np.linalg.norm(e1))) * self.distance(w1, w2)

    def keys(self):
        return self.word2idx.keys()

    def nearest(self, w, topk=10, include_distances=True):
        candidates = []
        for x in self.keys():
            candidates.append((x, self.distance(w, x)))

        top_candidates = sorted(candidates, key=lambda x: x[1])[:topk]
        if include_distances:
            return top_candidates
        else:
            return list(map(lambda x: x[0], top_candidates))
