from multiprocessing import Pool
import os

import numpy as np
import numpy.typing as npt
import pexpect

_te_exec = os.path.join(
    os.path.dirname(os.path.dirname(os.path.realpath(__file__))),
    'build', 'topic_evolution_model'
)

def _init_worker(getter, args: list[str]):
    # we assign the process to the _get_output function which is local to each
    # process
    getter.proc = pexpect.spawn(_te_exec, args, echo=False, env=os.environ)

def _get_output(corpus: str) -> str:
    _get_output.proc.sendline(corpus + '\0')
    _get_output.proc.expect('\0')

    out: bytes = _get_output.proc.before # type: ignore
    return out.decode()

class TEM:
    @classmethod
    def from_param_list(cls, params: npt.NDArray[np.float64], metrics: bool = False):
        return cls(metrics=metrics,
            c=params[0],
            alpha=params[1], beta=params[2], gamma=params[3], delta=params[4],
            theta=params[5],
            merge_threshold=params[6], evolution_threshold=params[7]
        )

    def __init__(
        self,
        metrics: bool = False,
        c: float = 0.5,
        alpha: float = 0,
        beta: float = -1,
        gamma: float = 0,
        delta: float = 1,
        merge_threshold: float = 100,
        evolution_threshold: float = 100,
        theta: float = 2,
    ):
        args = [
            '--c', str(c),
            '--alpha', str(alpha),
            '--beta', str(beta),
            '--gamma', str(gamma),
            '--delta', str(delta),
            '--theta', str(theta),
            '--merge_threshold', str(merge_threshold),
            '--evolution_threshold', str(evolution_threshold),
            '--keep_alive'
        ]
        if metrics: args.append('--metrics')
        self.pool = Pool(initializer=_init_worker, initargs=(_get_output, args))

    # get model outputs for structured corpora
    # create corpora with nlp.get_structured_corpus
    def get_outputs(self, corpora: list[str]) -> list[str]:
        return self.pool.map(_get_output, corpora)

    def get_metrics(self, corpora: list[str]) -> npt.NDArray[np.float64]:
        return np.array([eval(metrics) for metrics in self.get_outputs(corpora)])
