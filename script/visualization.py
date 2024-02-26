import graphviz
from matplotlib import pyplot as plt
import numpy as np
import numpy.typing as npt

from model import TopicEvolution
from process import TEM

def graph(te: TopicEvolution) -> graphviz.Digraph:
    g = graphviz.Digraph()
    g.attr(rankdir='TB')

    # RANK NODE FOR EACH PERIOD (i.e. "Period n" labels)
    with g.subgraph() as s:
        s.attr('node', shape='box')
        s.attr('edge', style='invis')
        for n, _ in enumerate(te.periods):
            s.node(str(n), label=f'Period {n}')
            if n < len(te.periods) - 1:
                s.edge(str(n), str(n + 1))

    # TOPIC NODES FOR EACH PERIOD
    # topic id -> ids of (graph) nodes in previous period
    previous_topics: dict[int, list[str]] = {}

    # helper for dictionary
    def append_or_set(d: dict[int, list[str]], k: int, v: str):
        if k in d: d[k].append(v)
        else: d[k] = [v]
    # create nodes & edges
    for n, period in enumerate(te.periods):
        current_topics: dict[int, list[str]] = {}
        with g.subgraph() as s:
            s.attr(rank='same')
            s.node(str(n))
            for m, topic in enumerate(period.topics):
                node_id = f'{n}-{m}'
                append_or_set(current_topics, topic.id, node_id)
                s.node(node_id, label=topic.label())
                if topic.id in previous_topics:
                    for predecessor in previous_topics[topic.id]:
                        g.edge(predecessor, node_id)
        previous_topics = current_topics

    return g

def plot_metrics(corpora: list[str], split: int, params: npt.NDArray[np.float64]):
    model = TEM.from_param_list(params, metrics=True)
    m = model.get_metrics(corpora)
    n_metrics = m.shape[1]

    _, axs = plt.subplots(int(np.ceil(n_metrics / 3)), 3)
    plt.tight_layout()

    for i in range(n_metrics):
        bins = np.linspace(np.min(m[:, i]), np.max(m[:, i]), 20)

        ax = axs.flat[i]
        ax.hist(m[split:, i], bins, alpha=0.5, label='Humans')
        ax.hist(m[:split, i], bins, alpha=0.5, label='Machines')

        ax.set_title(f'Metric {i + 1}')
        if i == 0: plt.legend(loc='upper left')

    plt.show()
