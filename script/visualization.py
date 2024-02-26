import graphviz
from matplotlib import pyplot as plt
import numpy as np
import numpy.typing as npt

from model import TopicEvolution
from training import metrics

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
    m = metrics(corpora, params)
    bins = np.linspace(0, 1, 20)
    _, axs = plt.subplots(2, 2)
    for i in range(4):
        ax = axs.flat[i]
        ax.hist(m[split:, i], bins, alpha=0.5, label='Humans')
        ax.hist(m[:split, i], bins, alpha=0.5, label='Machines')
        ax.set_title(f'Metric {i + 1}')
        ax.label_outer()
    plt.legend(loc='upper right')
    plt.show()
