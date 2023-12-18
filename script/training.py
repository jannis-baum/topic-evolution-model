from typing import Callable

import numpy as np
import numpy.typing as npt

from process import TEM

def metrics(corpora: list[str], params: npt.NDArray[np.float64]) -> npt.NDArray[np.float64]:
    model = TEM(metrics=True,
        c=params[0], alpha=params[1], beta=params[2], gamma=params[3], delta=params[4],
        merge_threshold=params[5], evolution_threshold=params[6]
    )
    return np.array([eval(metrics) for metrics in model.get_outputs(corpora)])

def gradient_descent(
    # TEM-ready corpora
    corpora: list[str],
    # loss function to find local minimum of: metrics-matrix -> loss >= 0
    loss: Callable[[npt.NDArray[np.float64]], np.float64],

    # params: c, alpha, beta, gamma, delta, merge_threshold, evolution_threshold
    initial_params: npt.NDArray[np.float64] = np.array([0.5, 0, -1, 0, 1, 50, 50]),
    # boundaries: (lower, upper) limit for params
    boundaries: tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]] = (
        np.array([0.0001, -3, -3, -3, -3, 0, 0]),
        np.array([0.9999,  3,  3,  3,  3, 100, 100])
    ),

    # step size to "evaluate partial derivate"
    delta: np.float64 = np.float64(0.01),
    # learning rate
    rate: np.float64 = np.float64(0.1),
    # stop criterion: if loss makes change smaller than this, exit
    epsilon: np.float64 = np.float64(0.01),
    # stop criterion: stop after this many iterations
    max_iter: int = 50,
) -> tuple[np.float64, npt.NDArray[np.float64]]:

    params = initial_params
    def evaluate_loss(adjust: tuple[int, np.float64] | None = None):
        p = np.copy(params)
        if adjust: p[adjust[0]] = adjust[1]
        return loss(metrics(corpora, p))

    def constrain_boundaries(index: int, value: np.float64) -> np.float64:
        return min(boundaries[1][index], max(boundaries[0][index], value))

    # ACTUAL GRADIENT DESCENT
    rate = rate / delta # step adjusted learning rate
    current_loss = np.float64(-100) # fine as initial value because loss is always positive
    for _ in range(max_iter):
        # update loss & check if we have converged
        old_loss = current_loss
        current_loss = evaluate_loss()
        if abs(old_loss - current_loss) < epsilon: break
        # UPDATE PARAMETERS ALONG GRADIENT
        # we check the difference a small step (delta) would make, scale it by
        # 1/delta to get the gradient, multiply with the learning rate and
        # finally add it to the current value -> step downwards
        params = np.array([
            constrain_boundaries(i, param + rate * (current_loss - evaluate_loss((i, param + delta))))
        for i, param in enumerate(params)])

    return (current_loss, params)

# loss for binary classification problem
# evaluates contrastive loss between classes so that first class is
# metrics_matrix[:split] and second class is metrics_matrix[split:]
def contrastive_loss(
    metrics_matrix: npt.NDArray,
    split: int,
    margin: np.float64
) -> np.float64:
    (pos, neg) = (metrics_matrix[:split], metrics_matrix[split:])
    (n_pos, n_neg) = (pos.shape[0], neg.shape[0])

    # pairwise Euclidean distances between samples
    # positive & negative
    cross_distances = np.linalg.norm(pos[:, np.newaxis] - neg, axis=-1)
    # positive
    pos_distances = np.linalg.norm(pos[:, np.newaxis] - pos, axis=-1)
    # positive
    neg_distances = np.linalg.norm(neg[:, np.newaxis] - neg, axis=-1)

    # losses of similar samples
    intra_pos_loss = np.sum(np.square(pos_distances)) / n_pos ** 2
    intra_neg_loss = np.sum(np.square(neg_distances)) / n_neg ** 2
    # losses of dissimilar samples
    inter_loss = np.sum(np.maximum(0, margin - cross_distances) ** 2) / n_pos * n_neg

    return 0.25 * intra_neg_loss + 0.25 * intra_pos_loss + 0.5 * inter_loss
