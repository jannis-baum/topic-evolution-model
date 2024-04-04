import os

import gensim.downloader as api

lang2model = {
    'en': 'word2vec-google-news-300'
}

def get_model():
    language = os.getenv('TEM_LANG', 'en')
    if language not in lang2model:
        raise LookupError(f'fatal: model for language "{language}" not defined.')
    return api.load(lang2model[language])

if __name__ == '__main__':
    for language, model in lang2model.items():
        print(f'Downloading "{model}" for language "{language}"')
        api.load(model, return_path=True)
