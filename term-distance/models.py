import os
from urllib.request import urlretrieve

from gensim.downloader import BASE_DIR, _progress
from gensim.models import KeyedVectors

_lang2model = {
    'en': ('word2vec-google-news-300', 'https://github.com/RaRe-Technologies/gensim-data/releases/download/word2vec-google-news-300/word2vec-google-news-300.gz'),
    'de': ('word2vec-german', 'https://cloud.devmount.de/d2bc5672c523b086/german.model')
}

def _ensure_download(name, url) -> str:
    directory = os.path.join(BASE_DIR, name)
    os.makedirs(directory, exist_ok=True)
    path = os.path.join(directory, f'{name}.{url.split(".")[-1]}')
    if not os.path.exists(path):
        print(f'Downloading {name} from {url}')
        download = path + '.download'
        urlretrieve(url, download, reporthook=_progress)
        os.rename(download, path)
    return path

def get_model() -> KeyedVectors:
    language = os.getenv('TEM_LANG', 'en')
    if language not in _lang2model:
        raise LookupError(f'fatal: model for language "{language}" not defined.')
    path = _ensure_download(*_lang2model[language])
    return KeyedVectors.load_word2vec_format(path, binary=True)

if __name__ == '__main__':
    for model_data in _lang2model.values():
        _ensure_download(*model_data)
