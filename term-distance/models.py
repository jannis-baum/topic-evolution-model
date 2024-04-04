import gensim.downloader as api

lang2model = {
    'en': 'word2vec-google-news-300'
}

if __name__ == '__main__':
    for language, model in lang2model.items():
        print(f'Downloading "{model}" for language "{language}"')
        api.load(model, return_path=True)
