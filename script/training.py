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
    # params: c, alpha, beta, gamma, delta, merge_threshold, evolution_threshold
    initial_params: npt.NDArray[np.float64] = np.array([0.5, 0, -1, 0, 1, 100, 100]),
    # boundaries: (lower, upper) limit for params
    boundaries: tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]] = (
        np.array([0, -3, -3, -3, -3, 0, 100]),
        np.array([1,  3,  3,  3,  3, 0, 100])
    ),
) -> tuple[float, np.ndarray]:
    return (0, initial_params)
