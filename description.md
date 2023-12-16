# Short Description of Topic Evolution Model

This document aims to serve as a short but complete description of how TEM works
and can be used.

## General terms

- A **document** $d$ is a list of words $(w_1, ..., w_k)$ with $k$ being the
  document's length. This might a sentence in an article.
- A **period** $D_t$ is a list of documents that belong to the same temporal
  section of the *corpus*. This might a paragraph in an article.
- A **corpus** the entire body of text that makes up the input to TEM, consisting
  of an ordered list of periods. This might be an article.
- A **topic** $T$ is a set of words describing a theme in a period. Each topic
  has a theme identifier. Topics with the same identifier that occur over
  multiple paragraphs make up the evolution of the given theme.

TEM expects an input of a corpus made up of periods that are separated by empty
lines, which in turn are made up of documents that are separated by line breaks,
each made up of words separated by spaces. If the `--keep_alive` flag is given,
TEM will not exit after printing the output, but instead process multiple
corpora separated by null-characters. In this case, the output will also be
separated by null-characters.

By default, TEM outputs YAML-encoded data about the topics for each period. If
the `--metrics` flag is given, it will instead output a Python-style array of
the following four metrics calculated on the corpus:

1. The absolute value of 1 minus the ratio of the number of distinct theme
   identifiers, and the total number of topics
2. The ratio of the number of topics with the most common theme identifier, and
   the total number of topics
3. The number of periods that have at least one incoming edge, i.e. a topic with
   predecessor topic, divided by the total number of periods minus 1
4. The ratio of the longest chain of connected periods, and the total number
   of periods

## Model parameters

The following is a list of parameters that have to be supplied to TEM. See the
algorithm description below to find detailed information about the effect of the
parameters.

- `--c` $\in [0, 1]$ tunes the value of $nutrition$
- `--alpha` sets the threshold of $nutrition$ for emerging terms
- `--beta` sets the threshold of $energy$ for emerging terms
- `--gamma` sets the threshold of $ENR$ for emerging terms
- `--delta` sets the threshold of *term correlation* for the construction of
  a period's semantic graph
- `--merge_threshold` defines the threshold of *topic distance* below which
  distinct topics within a period are merged into one
- `--evolution_threshold` defines the threshold of *topic distance* below which
  topics of different periods are assigned the same theme
- `--theta`, **an integer**, which defines how closely connected terms have to
  be to belong to the same topic

## TEM algorithm

The following sections describe how TEM builds its output.

### Semantic graph construction

For each period $D_t$, a semantic graph is constructed that connects the words
that occur in it.

Let $n_W$ be the number of documents in $D_t$ that contain all words $w \in W$.
Then $c_{k,z}^t$ is the *term correlation* between words (terms) $k$ and $z$ at
a time $t$.

$$
  c_{k,z}^t = \log\left(
    \frac{
      n_{\left\{ k, z \right\}} / (n_{\left\{ k \right\}} - n_{\left\{ k, z \right\}})
    }{
      (n_{\left\{ z \right\}} - n_{\left\{ k, z \right\}}) /
        (\left\lvert D_t \right\rvert
          - n_{\left\{ z \right\}}
          - n_{\left\{ k \right\}}
          + n_{\left\{ k, z \right\}})
    }
  \right) \cdot \left\lvert
    \frac{n_{\left\{ k, z \right\}}}{n_{\left\{ k \right\}}} - \frac{
        n_{\left\{ z \right\}} - n_{\left\{ k, z \right\}}
    }{
        \left\lvert D_t \right\rvert - n_{\left\{ k \right\}}
      }
  \right\rvert
$$

In the semantic graph, terms $k$ with $2 n_{\{ k \}} \leq \lvert D_t \rvert$ are
nodes, while more common terms, also called *flood words*, are omitted. The
weighted edge at time $t$ between two nodes $(u, v)$ will be $c_{u, v}^t$.

The semantic graph is then are thinned out (removing edges) according to the
tuning parameter $\delta$. An edge $(u, v)$ is kept only if their weight

$$
c_{u, v}^t > \tilde{c} + \sigma_c \cdot \delta
$$

where $\tilde{c}$ and $\sigma_c$ are the median and standard deviation of all
edge weights (correlations).

### Emerging terms

Words can be classified as *emerging* in a period based on the following values.

- **nutrition** is an indicator of a word's popularity in a single document $d_i \in D_t$
  - $nutrition(w) = (1 - c) + c \cdot tf(w) / tf(w_i^*)$
  - $w_i^*$ is most frequent word in document $d_i$
  - $tf(w)$ is the word's term frequency in $d_i$
  - $c \in [0, 1]$ is a constant
  - $nutrition(w)_t$ is sum of nutritions over $D_t$, normalized by the number
    of documents in $t$
  - *describes relative word frequency in $[1-c, 1]$*
- **energy** describes change in a word's nutrition over time
  - $energy(w) = \sum_{t=1}^s \frac{1}{i} (nutrition(w)_t^2 - nutrition(w)_{t-i}^2)$
  - $s$ is the number of time periods before $t$
  - exception: in the first period, $energy(w) = nutrition(w)_t^2$
- **Energy-Nutrition Ratio**
  - $ENR(w) = \frac{energy(w)}{nutrition(w)}$
  - describes if a word's rate of growth is accelerating, constant, or
    decelerating

If the values for a given word fall into the following thresholds, this word is
an *emerging term*.

- $\alpha$: **upper bound for $nutrition$** such that a word $w$ with
  $nutrition(w)$ greater than the period's median $nutrition$ plus its standard
  deviation times $\alpha$ is classified as a *flood word*
- $\beta$: **lower bound for $energy$** such that the remaining (non-flood)
  words' median energy plus their standard deviation times $\beta$ is the lower
  bound for $energy$
- $\gamma$: **lower bound for $ENR$** such that a word's $ENR$ in the previous
  period (or $0$) times $\gamma$ is the lower bound for $ENR$

### Topics

A topic is a subset of nodes from a period's semantic graph:

- for each emerging term $e$
  - create new *emerging topic* $t_e = \{ e \}$
  - start a BFS in the semantic graph up to depth $\theta$ (with $e$ at $0$)
  - from every discovered node $v$
    - run another BFS up to depth $\theta$
    - if $e$ is discovered, stop and add $v$ to $t_e$

Let $td_{t_1, t_2}$ be the *topic distance* between topics $t_1, t_2$.

$$
  td_{t_1, t_2} = \frac{
      \min(
        \left\lvert t_1 \setminus t_2 \right\rvert,
        \left\lvert t_2 \setminus t_1 \right\rvert
      )
    }{
      \left\lvert t_1 \cap t_2 \right\rvert
    }
$$

A distance of $0$ implies, that one topic is a subset of the other. Emerging
topics with a distance smaller than `--merge_threshold` are recursively merged
until all remaining topics have pairwise distances that are greater than the
thresholds. These topics are the final topics of the period.

Each discovered topic is compared with topics of predecessor periods. If the
distance to a predecessor topic is smaller than `--evolution_threshold`, the
topic is assigned the same theme as the predecessor.
