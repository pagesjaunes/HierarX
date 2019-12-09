
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

from itertools import chain
import plotly.offline as pyo
import plotly.graph_objs as go
import numpy as np
import math
import pandas as pd
import os

from pyhierarx.load import HyperbolicVec

class Vizualiser:

    SYMBOLS = ['circle', 'triangle-up', 'square', 'star', 'cross', 'diamond', 'pentagon', 'bowtie', 'triangle-down', 'x', 'hexagon']

    def __init__(
        self,
        path,
        animation = False,
        interactive=False,
        radius = None,
        entities = None,
        edges = None,
        marker_size = 10,
        auto_open = True
    ):
        self.path = path
        self.emb = HyperbolicVec.load(path) if not animation else HyperbolicVec.load(os.path.join(path, 'embedding0.bin'))
        self.__interactive = interactive
        self.radius = radius
        self.marker_size = marker_size
        self.auto_open = auto_open

        if entities is not None:
            self.entities = pd.read_csv(entities) if isinstance(entities, str) else entities
            assert(self.entities.shape[0] == self.emb.vocsize)
            for _, w in self.entities.iterrows():
                assert(w['entity'] in self.emb.keys())
            self.types = sorted(list(set(self.entities['type'])))
            self.maxspec = self.entities.max()['relative-specificity']
            self.colorscale = ['rgb(%d,%d,%d)' % tuple([255 - 255 / self.maxspec * i for _ in range(3)]) for i in range(int(self.maxspec) + 1)]
            self.entity2desc = {
                x['entity'] : (x['type'],x['relative-specificity']) for _, x in self.entities.iterrows()
            }
        else:
            self.entities = None

        if edges is not None:
            self.edges = pd.read_csv(edges, header=None) if isinstance(edges, str) else edges
            for w in set(chain(self.edges.iloc[:, 0], self.edges.iloc[:,1])):
                assert(w in self.emb.keys())
        else:
            self.edges = None

    def __circle(
        self,
        r = None
    ):
        if r is None:
            if self.radius is None:
                r = max(map(lambda v : np.linalg.norm(v), (self.emb.project(w) for w in self.emb.keys()))) + 0.05
            else:
                r = self.radius


        return go.Scatter(
            x = r * np.cos(np.linspace(0, 2 * math.pi, 1000)),
            y = r * np.sin(np.linspace(0, 2 * math.pi, 1000)),
            showlegend=False,
            line=dict(color='black')
        )

    def __colorbar_legend(
        self
    ):
        if self.entities is not None:
            return [
                go.Scatter(
                    x = [None],
                    y= [None],
                    showlegend=False,
                    mode='markers',
                    marker=dict(
                        colorbar=dict(
                            title='Relative specificity',
                            ticktext=[str(i) for i in range(len(self.colorscale))],
                            tickvals=[i + 0.5 for i in range(len(self.colorscale))],
                            len=0.5,
                            y = 0.3,
                        ),
                        cmin=0,
                        cmax=self.maxspec + 1,
                        colorscale=sum([[[i / (self.maxspec + 1), x], [(i + 1) / (self.maxspec + 1), x]] for i, x in enumerate(self.colorscale)], [])
                    )
                )
            ]
        else:
            return []

    def __marks_legend(
        self
    ):
        if self.entities is not None:
            return [
                go.Scatter(
                    x = [None],
                    y = [None],
                    mode = 'markers',
                    name = t,
                    showlegend = True,
                    marker = dict(
                        size=self.marker_size,
                        symbol=Vizualiser.SYMBOLS[self.types.index(t) % len(Vizualiser.SYMBOLS)],
                        color='black'
                    ),
                ) for t in self.types
            ]
        else:
            return []

    def __marks(
        self,
        text
    ):
        if self.entities is not None:
            return dict(
                color=[self.colorscale[int(self.entity2desc[v][1])] for v in text],
                symbol=[Vizualiser.SYMBOLS[self.types.index(self.entity2desc[v][0]) % len(Vizualiser.SYMBOLS)] for v in text],
                size=self.marker_size,
                line = dict(width=1, color='black')
            )
        else:
            return dict(size=self.marker_size, color='blue')

    def __edges(
        self,
        emb
    ):
        if self.edges is not None:
            X, Y, Z = [], [], []
            for _, edge in self.edges.iterrows():
                e1, e2 = emb.project(edge[0]), emb.project(edge[1])
                X.append([e1[0], e2[0], None])
                Y.append([e1[1], e2[1], None])
                if emb.dim == 3:
                    Z.append([e1[2], e2[2], None])

            if len(Z) == 0:
                return go.Scatter(x = sum(X, []), y=sum(Y, []), name='edges', line=dict(color="rgba(0,0,0,0.5)", width=1))
            else:
                return go.Scatter(x = sum(X, []), y=sum(Y, []), z = sum(Z, []), name='edges', line=dict(color="rgba(0,0,0,0.5)", width=1))
        else:
            return go.Scatter(x=None, y=None, showlegend=False)

    @staticmethod
    def __layout():
        return go.Layout(
            plot_bgcolor='white',
            xaxis=dict(
                showticklabels=False
            ),
            yaxis=dict(
                showticklabels=False,
                scaleanchor="x",
                scaleratio=1
            ),
            width=850,
            height=700
        )

    def gobjs(
        self,
        emb,
        init = True,
        r = None
    ):
        text = list(emb.keys())
        dim = emb.dim
        assert(1 < dim <= 3)

        X, Y, Z = map(lambda i : [emb.project(w)[i] for w in text] if i < dim else None, [0, 1, 2])


        if Z is None:
            gscatt = go.Scatter(x = X, y = Y, text=text, mode='markers', marker=self.__marks(text), showlegend=False)
        else:
            gscatt = go.Scatter3d(x = X, y = Y, z = Z, text=text, mode='markers', marker=self.__marks(text), showlegend=False)

        if init:
            return [self.__edges(emb), gscatt] + [ self.__circle(r = r)] + self.__marks_legend() + self.__colorbar_legend()
        else:
            return [self.__edges(emb), gscatt]

    def show(
        self,
        path = None
    ):

        fig = go.Figure(data=self.gobjs(self.emb), layout = Vizualiser.__layout())

        if self.__interactive or path is None:
            if path is not None and path.split('.')[-1] == 'html':
                pyo.plot(fig, filename=path, auto_open=True)
            else:
                pyo.plot(fig, filename='temp-plot.html' if path is None else '%s.html' % path, auto_open=True)
        else:
            fig.write_image(path)


    def animate(
        self,
        nframes,
        step = 1,
        path = None
    ):

        frames = []
        for i in range(1, nframes, step):
            i_emb = HyperbolicVec.load(os.path.join(self.path, "embedding%d.bin" % i))
            frames.append(
                go.Frame(data=self.gobjs(i_emb, init=False))
            )
        r = max(map(lambda v : np.linalg.norm(v), (i_emb.project(w) for w in i_emb.keys()))) + 0.05

        fig = go.Figure(
            data = self.gobjs(self.emb, r=r),
            frames = frames,
            layout = Vizualiser.__layout()
        )
        fig.update_layout(
            updatemenus=[
                dict(
                    type="buttons",
                    buttons=[
                        dict(
                            label="Play",
                            method="animate",
                            args=[
                                None,
                                {
                                    "frame": {
                                        "duration": 500,
                                        "redraw": False
                                    },
                                    "fromcurrent": True,
                                    "transition": {
                                        "duration": 500,
                                        "easing": "quadratic-in-out"
                                    }
                                }
                            ]
                        )
                    ],
                    showactive=False
                )
            ]
        )

        if self.__interactive or path is None:
            if path is not None and path.split('.')[-1] == 'html':
                pyo.plot(fig, filename=path, auto_open=True)
            else:
                pyo.plot(fig, filename='temp-plot.html' if path is None else '%s.html' % path, auto_open=True)
        else:
            raise ValueError('Animated plot are not serializable.')
            #fig.write_image(path)