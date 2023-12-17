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
    # cost function to find local minimum of: metrics-matrix -> cost >= 0
    cost: Callable[[npt.NDArray[np.float64]], np.float64],

    # params: c, alpha, beta, gamma, delta, merge_threshold, evolution_threshold
    initial_params: npt.NDArray[np.float64] = np.array([0.5, 0, -1, 0, 1, 50, 50]),
    # boundaries: (lower, upper) limit for params
    boundaries: tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]] = (
        np.array([0, -3, -3, -3, -3, 0, 100]),
        np.array([1,  3,  3,  3,  3, 0, 100])
    ),

    # step size to "evaluate partial derivate"
    delta: np.float64 = np.float64(0.01),
    # learning rate
    rate: np.float64 = np.float64(0.1),
    # stop criterion: if cost makes change smaller than this, exit
    epsilon: np.float64 = np.float64(0.01),
    # stop criterion: stop after this many iterations
    max_iter: int = 50,
) -> tuple[np.float64, npt.NDArray[np.float64]]:

    params = initial_params
    def evaluate_cost(adjust: tuple[int, np.float64] | None = None):
        p = params
        if adjust: p[adjust[0]] = adjust[1]
        return cost(metrics(corpora, p))

    # ACTUAL GRADIENT DESCENT
    rate = rate / delta # step adjusted learning rate
    current_cost = np.float64(-100) # fine as initial value because cost is always positive
    for _ in range(max_iter):
        # update cost & check if we have converged
        old_cost = current_cost
        current_cost = evaluate_cost()
        if abs(old_cost - current_cost) < epsilon: break
        # UPDATE PARAMETERS ALONG GRADIENT
        # we check the difference a small step (delta) would make, scale it by
        # 1/delta to get the gradient, multiply with the learning rate and
        # finally add it to the current value -> step downwards
        params = np.array([
            rate * (current_cost - evaluate_cost((i, param + delta)))
        for i, param in enumerate(params)])

    return (current_cost, params)
