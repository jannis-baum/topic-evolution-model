from flask import Flask, request
import gensim.downloader as api
import numpy as np

from models import lang2model

app = Flask(__name__)
wv = api.load(lang2model['en'])

def meaningfulness(word: str):
    return max(0, -np.exp(-1.07 * np.linalg.norm(wv[word]) + 0.91) + 1)

def cos_sim_01(word1: str, word2: str):
    return (wv.similarity(word1, word2) + 1) * 0.5

def distance(word1: str, word2: str):
    return abs(1 - np.sqrt(
        cos_sim_01(word1, word2) *
        np.sqrt(meaningfulness(word1) * meaningfulness(word2))
    ))

@app.route('/similarity', methods=['POST'])
def hello():
    body = request.get_data().decode()
    words = body.split('\n')
    n = len(words)
    # for unknown distances we assume 1
    sims = [['1'] * n for _ in range(n)]
    for i in range(n):
        # identity with distance 0
        sims[i][i] = '0'
        for j in range(i + 1, n):
            try:
                sim = str(distance(words[i], words[j]))
                sims[i][j] = sim
                sims[j][i] = sim
            except:
                continue
    return '\n'.join((' '.join(row) for row in sims))
