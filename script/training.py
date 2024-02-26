import numpy as np
import numpy.typing as npt

from process import TEM

def metrics(corpora: list[str], params: npt.NDArray[np.float64]) -> npt.NDArray[np.float64]:
    model = TEM(metrics=True,
        c=params[0],
        alpha=params[1], beta=params[2], gamma=params[3], delta=params[4],
        theta=params[5],
        merge_threshold=params[6], evolution_threshold=params[7]
    )
    return np.array([eval(metrics) for metrics in model.get_outputs(corpora)])
