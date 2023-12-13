from multiprocessing import Pool
import os

import pexpect

_te_exec = os.path.join(
    os.path.dirname(os.path.dirname(os.path.realpath(__file__))),
    '.build.out', 'out'
)

def _init_worker(getter, args: list[str]):
    # we assign the process to the _get_output function which is local to each
    # process
    getter.proc = pexpect.spawn(_te_exec, args, echo=False)

def _get_output(corpus: str) -> str:
    _get_output.proc.sendline(corpus + '\0')
    _get_output.proc.expect('\0')

    out: bytes = _get_output.proc.before # type: ignore
    return out.decode()

class TEM:
    def __init__(
        self,
        metrics: bool = False,
        c: float = 0.5,
        alpha: float = 0,
        beta: float = -1,
        gamma: float = 0,
        delta: float = 1,
        theta: int = 2,
        merge_threshold: float = 100,
        evolution_threshold: float = 100,
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
