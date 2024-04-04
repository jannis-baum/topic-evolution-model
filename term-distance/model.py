import os
from urllib.request import urlretrieve

from gensim.downloader import BASE_DIR, _progress
from gensim.models import KeyedVectors
import numpy as np

# language -> data: (name, url)
#             params: (meaningfulness-scalar, meaningfulness-translator)
_lang2model = {
    'en': {
        'data': ('word2vec-google-news-300', 'https://github.com/RaRe-Technologies/gensim-data/releases/download/word2vec-google-news-300/word2vec-google-news-300.gz'),
        'params': (-1.07, 0.91)
    },
    'de': {
        'data': ('word2vec-german', 'https://dl.fbaipublicfiles.com/fasttext/vectors-wiki/wiki.de.vec'),
        'params': (-0.73, 1.48)
    }
}

# gets *_lang2model[data] and downloads/loads the model
def _load_model(name: str, url: str, download_only = False):
    directory = os.path.join(BASE_DIR, name)
    os.makedirs(directory, exist_ok=True)

    path = os.path.join(directory, f'{name}.{url.split(".")[-1]}')
    if not os.path.exists(path):
        print(f'Downloading {name} from {url}')
        download = path + '.download'
        urlretrieve(url, download, reporthook=_progress)
        os.rename(download, path)
        print()

    if download_only: return
    try:
        return KeyedVectors.load_word2vec_format(path, binary=True)
    except:
        return KeyedVectors.load_word2vec_format(path, binary=False)

class DistanceModel:
    _wv: KeyedVectors
    _a: float
    _b: float

    def __init__(self):
        language = os.getenv('TEM_LANG', 'en')
        if language not in _lang2model:
            raise LookupError(f'fatal: model for language "{language}" not defined.')

        model = _lang2model[language]

        (self._a, self._b) = model['params']
        self._wv = _load_model(*model['data'])

    def _meaningfulness(self, word: str):
        return max(0, -np.exp(self._a * np.linalg.norm(self._wv[word]) + self._b) + 1)

    def _cos_sim_01(self, word1: str, word2: str):
        return (self._wv.similarity(word1, word2) + 1) * 0.5

    def distance(self, word1: str, word2: str):
        return abs(1 - np.sqrt(
            self._cos_sim_01(word1, word2) *
            np.sqrt(self._meaningfulness(word1) * self._meaningfulness(word2))
        ))

if __name__ == '__main__':
    for model in _lang2model.values():
        _load_model(*model['data'], download_only=True)
