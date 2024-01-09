from flask import Flask, request
import gensim.downloader as api

app = Flask(__name__)
wv = api.load('word2vec-google-news-300')

@app.route('/similarity')
def hello():
    args = request.args
    if 'a' not in args or 'b' not in args:
        return 'Words a or b missing in arguments'
    return str(wv.similarity(args['a'], args['b']))
