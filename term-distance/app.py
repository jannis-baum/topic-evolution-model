from flask import Flask, request

from model import DistanceModel

app = Flask(__name__)
model = DistanceModel()

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
                sim = str(model.distance(words[i], words[j]))
                sims[i][j] = sim
                sims[j][i] = sim
            except:
                continue
    return '\n'.join((' '.join(row) for row in sims))
