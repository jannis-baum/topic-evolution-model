import os
import pexpect

from nlp import get_corpus

class TEM:
    _te_exec = os.path.join(
        os.path.dirname(os.path.dirname(os.path.realpath(__file__))),
        '.build.out', 'out'
    )

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
        self.proc = pexpect.spawn(TEM._te_exec, args, echo=False)

    def get_output(self, text: str) -> str:
        corpus = get_corpus(text)
        structured_text = '\n\n'.join([
            '\n'.join([
                ' '.join(word for word in doc)
            for doc in period])
        for period in corpus])

        self.proc.sendline(structured_text + '\0')
        self.proc.expect('\0')

        out: bytes = self.proc.before # type: ignore
        return out.decode()
