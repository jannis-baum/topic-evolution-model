from flask import Flask, request
import gensim.downloader as api

app = Flask(__name__)
wv = api.load('word2vec-google-news-300')

@app.route('/similarity', methods=['POST'])
def hello():
    body = request.get_data().decode()
    words = body.split('\n')
    n = len(words)
    sims = [['0'] * n for _ in range(n)]
    for i in range(n):
        for j in range(i + 1, n):
            dist = str(wv.similarity(words[i], words[j]))
            sims[i][j] = dist
            sims[j][i] = dist
    return '\n'.join((' '.join(row) for row in sims))
