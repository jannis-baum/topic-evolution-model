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
  - $nutrition(w)_t$ is sum of nutritions over $D_t$
  - *describes relative word frequency in $[1-c, 1]$*
- **energy** describes change in a word's nutrition over time
  - $energy(w) = \sum_{t=1}^s (nutrition(w)_t^2 - \frac{1}{i} \cdot
    nutrition(w)_{t-i}^2)$
  - $s$ is the number of time periods before $t$
- **Energy-Nutrition Ratio**
  - $ENR(w) = \frac{energy(w)}{nutrition(w)}$
  - describes if a word's rate of growth is accelerating, constant, or
    decelerating

## Emerging terms

Words within the following tuning parameter bounds are classified as important,
*emerging terms*

- $\alpha$: upper bound for $nutrition$ so that a word $w$ with $nutrition(w) >
  \alpha$ is classified as a *flood word*
- $\beta$ : lower bound for $energy$
- $\gamma$: lower bound for $ENR$
