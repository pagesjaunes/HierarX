
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

import fire
import os
import pandas as pd

from pyhierarx.viz import Vizualiser
from nltk.corpus import wordnet as wn
from tqdm import tqdm

def add_to(hierarchie, entity_name):
    hierarchie.add(entity_name)
    for hypo in wn.synset(entity_name).closure(lambda s: s.hyponyms()):
        hierarchie.add(hypo.name())

def get_type(w, types):
    if wn.synset(w) in types:
        return types.index(wn.synset(w))
    for t in wn.synset(w).closure(lambda x: x.hypernyms()):
        if t in types:
            return types.index(t)
    return len(types)

class Runner:

    @staticmethod
    def show(
        path,
        outfile,
        animation = False,
        interactive=True,
        radius = None,
        entities = None,
        edges = None,
        marker_size = 10,
        auto_open = True,
        step = 1,
        nframes = None
    ):
        v = Vizualiser(
            path,
            animation = animation,
            interactive = interactive,
            radius = radius,
            entities = entities,
            edges = edges,
            marker_size = marker_size,
            auto_open = auto_open
        )

        if animation :
            if nframes is None:
                raise ValueError('Please specify the number of frames')
            v.animate(nframes, step=step, path=outfile)
        else:
            v.show(path=outfile)

    @staticmethod
    def wordnet_exp(
        entity_name,
        outdir,
        similarity = 'lch'
    ):
        funcsims = {
            'lch': lambda x, y: x.lch_similarity(y),
            'path': lambda x, y: x.path_similarity(y),
            'wup': lambda x, y: x.wup_similarity(y)
        }

        print('Find entities being hyponyms of %s' % entity_name)
        entities = set()
        add_to(entities, entity_name)

        print('Compute similarities between entities')
        with open(os.path.join(outdir, '%s-similarities.hierarx' % entity_name), 'w') as ofstream:
            __entities = list(entities)
            for i, x in tqdm(enumerate(__entities), total=len(__entities), unit='entity'):
                for j in range(i + 1, len(__entities)):
                    ofstream.write('%s,%s,%.6f\n' % (x, __entities[j], funcsims[similarity](wn.synset(x), wn.synset(__entities[j]))))

        print('Store edges between entities')
        edges = list(
            sum(
                map(lambda sn: sum([[wn.synset(sn), v] for v in wn.synset(sn).hypernyms() if v.name() in __entities], []),
                    __entities),
                []
            )
        )
        edges = list(zip(edges[::2], edges[1::2]))
        pd.DataFrame({
            'x': list(map(lambda x : x[0].name(), edges)),
            'y': list(map(lambda x : x[1].name(), edges))
        }).to_csv(os.path.join(outdir, '%s-edges.csv' % entity_name), header = None, index=None)

        print('Store families and specificity')
        types = list(wn.synset(entity_name).hyponyms()) + [wn.synset(entity_name)]
        y = [types[get_type(v, wn.synset(entity_name).hyponyms())].name() for v in __entities]
        mindepth = int(sum(map(len, wn.synset(entity_name).hypernym_paths())) / len( wn.synset(entity_name).hypernym_paths()))
        spec = [int(sum(len(hyp_path) for hyp_path in wn.synset(v).hypernym_paths()) / len(wn.synset(v).hypernym_paths())) - mindepth for v in __entities]

        pd.DataFrame({
            "entity": __entities,
            "type": y,
            "relative-specificity": spec
        }).to_csv(os.path.join(outdir, '%s-entities.csv' % entity_name), index=None)

if __name__ == '__main__':

    fire.Fire(Runner)