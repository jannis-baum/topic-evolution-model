# Definitions

## General terms

- **document** is an ordered list of words $(w_1, ..., w_k)$ with $k$ being the
  document's length
- **topic** $T$ is a set of words describing a subject
- **input**: set of lists of documents $D_t \in D$ where $t \in \tau$ is the
  time period for documents $D_t$
- **output**: topics  $M_t \in M$

## Nutrition & Energy

- **nutrition** is an indicator of a word's popularity in a single document $d_i
  \in D_t$
  - $nutrition(w) = (1 - c) + c \cdot tf(w) / tf(w_i^*)$
  - $w_i^*$ is most frequent word in document $d_i$
  - $tf(w)$ is the word's term frequency in $d_i$
  - $c \in [0, 1]$ is a constant
  - $nutrition(w)_t$ is sum of nutritions over $D_t$, normalized by the number
    of documents in $t$
  - *describes relative word frequency in $[1-c, 1]$*
- **energy** describes change in a word's nutrition over time
  - $energy(w) = \sum_{t=1}^s \frac{1}{i} (nutrition(w)_t^2 -
    nutrition(w)_{t-i}^2)$
  - $s$ is the number of time periods before $t$
- **Energy-Nutrition Ratio**
  - $ENR(w) = \frac{energy(w)}{nutrition(w)}$
  - describes if a word's rate of growth is accelerating, constant, or
    decelerating

## Emerging terms

Words within the following tuning parameter bounds are classified as important,
*emerging terms*

- $\alpha$: **upper bound for $nutrition$** such that a word $w$ with
  $nutrition(w)$ greater than the period's median $nutrition$ plus its standard
  deviation times $\alpha$ is classified as a *flood word*
- $\beta$: **lower bound for $energy$** such that the remaining (non-flood)
  words' median energy plus their standard deviation times $\beta$ is the lower
  bound for $energy$
- $\gamma$: **lower bound for $ENR$** such that a word's $ENR$ in the previous
  period (or $0$) times $\gamma$ is the lower bound for $ENR$

## Semantic graph construction

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

In the graph, terms $k$ with $2 n_{\{ k \}} < \lvert D_t \rvert$ are nodes. The
weighted edge at time $t$ between two nodes $(u, v)$ will be $c_{u, v}^t$.

Graphs are thinned out (removing edges) according to the tuning parameter
$\delta$. An edge $(u, v)$ is kept only if their weight

$$
c_{u, v}^t > \tilde{c} + \sigma_c \cdot \delta
$$

where $\tilde{c}$ and $\sigma_c$ are the median and standard deviation of all
edge weights (correlations).

## Finding topics

A topic is a period's subgraph.

### Emerging topics

- for each emerging term $e$
  - create new *emerging topic* $t_e = \{ e \}$
  - start a BFS up to depth $\theta$ (with $e$ at $0$)
  - from every discovered node $v$
    - run another BFS up to depth $\theta$
    - if $e$ is discovered, stop and add $v$ to $t_e$

### Merging topics

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

A distance of $0$ implies, that one topic is a subset of the other.
