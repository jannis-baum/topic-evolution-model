import numpy as np
import pandas as pd

def import_data(filter=None, path='logs/tem_metrics_0.5_1.0_0.25_0.75_1.25_0.3_0.3_0.95'):
    X = np.array(pd.read_pickle(f'{path}/features.pickle'))
    y = np.array([
        { 0: 'Human', 1: 'GPT3', 2: 'GPT2', 3: 'GPT4', 4: 'GROVER', 5: 'Gemini' }[label]
        for label in pd.read_pickle(f'{path}/labels.pickle')
    ])

    condition = y != 'GROVER'
    X = X[condition]
    y = y[condition]

    if filter:
        if filter == 'Machine':
            y[y != 'Human'] = 'Machine'
        else:
            condition = (y == 'Human') | (y == filter)
            X = X[condition]
            y = y[condition]
    return X, y
