
from pyhierarx.cy import PyExperiment


class HierarXTrainer:

    def __init__(
        self,
        nvoc = None,
        dim = None,
        nthread = None,
        expdir = None,
        input_ = None,
        lr = 1e-2,
        momentum = 0.9,
        bs = 10,
        sampling = 20,
        posthres = 0.10,
        maxposthres = None,
        plateau = 0.10,
        celerity = 1.0,
        checkpoint = 10000,
        kneighbors = 100,
        ntrees = 20,
        rebuild = 10000,
        alpha = 0.5,
        hmode = "Poincare",
        niter = 100000,
        similarity = False,
        symmetric = False,
        continue__ = False,
        nesterov = True,
        movie = False,
        lorentzian = False,
        declr = False,
        minlr = 0,
        weighted = 0):

        assert(nvoc is not None)
        self.nvoc = nvoc

        assert(dim is not None)
        self.dim = dim

        assert(nthread is not None)
        self.nthread = nthread

        assert(expdir is not None)
        self.expdir = expdir

        assert(input_ is not None)
        self.input = input_

        self.lr = lr
        self.momentum = momentum
        self.bs = bs
        self.sampling = sampling
        self.posthres = posthres
        self.maxposthres = posthres if maxposthres is None else maxposthres
        self.plateau = plateau
        self.celerity = celerity
        self.checkpoint = checkpoint
        self.kneighbors = kneighbors
        self.ntrees = ntrees
        self.rebuild = rebuild
        self.alpha = alpha
        self.hmode = hmode
        self.niter = niter
        self.similarity = similarity
        self.symmetric = symmetric
        self.continue__ = continue__
        self.nesterov = nesterov
        self.movie = movie
        self.lorentzian = lorentzian
        self.weighted = weighted
        self.declr = declr
        self.minlr = minlr

        self.pyexp = PyExperiment(self)
        self.pyexp.init_experiment()

    def train(self, niter=None, posthres=None, lr=None):

        if niter is not None:
            self.pyexp.set_run_params(niter=niter, posthres=posthres, lr=lr)
        return self.pyexp.train()

    def get_embedding(self):
        return self.pyexp.get_embedding()