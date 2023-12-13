import os
import subprocess
import yaml

from model import TopicEvolution
from nlp import get_corpus

_te_exec = os.path.join(
    os.path.dirname(os.path.dirname(os.path.realpath(__file__))),
    '.build.out', 'out'
)

def get_te(
    text: str,
    c: float = 0.5,
    alpha: float = 0,
    beta: float = -1,
    gamma: float = 0,
    delta: float = 1,
    theta: int = 2,
    merge_threshold: float = 100,
    evolution_threshold: float = 100,
) -> TopicEvolution:
    corpus = get_corpus(text)
    structured_text = '\n\n'.join([
        '\n'.join([
            ' '.join(word for word in doc)
        for doc in period])
    for period in corpus])

    p = subprocess.Popen([
            _te_exec,
            '--c', str(c),
            '--alpha', str(alpha),
            '--beta', str(beta),
            '--gamma', str(gamma),
            '--delta', str(delta),
            '--theta', str(theta),
            '--merge_threshold', str(merge_threshold),
            '--evolution_threshold',str(evolution_threshold) 
        ], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    out = p.communicate(input=structured_text.encode())[0].decode()

    return TopicEvolution(yaml.safe_load(out))
