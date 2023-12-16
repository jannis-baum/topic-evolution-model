# Topic Evolution Model

TEM is a graph-based temporal topic modeling for very small corpora. Detailed
information about TEM can be found in the following publication

> Liebe L, Baum J, Schutze T, Cech T, Scheibel W, and Dollner J (2023). UNCOVER:
> Identifying AI Generated News Articles by Linguistic Analysis and
> Visualization

or in the [short model description in this repository](./description.md).

## Usage

- build the model by running `make`
- run the model in `.build.out/out` and provide the model parameters as
  arguments: `--delta`, `--c`, `--alpha`, `--beta`, `--gamma`,
  `--merge_threshold` `--evolution_threshold`
- the corpus is then read from `stdin`:
  - *temporal periods* are separated by blank lines
  - *documents* in periods are lines
  - *words* in documents are separated by spaces
- the yaml-encoded output is written to `stdout` as a list:
  - each list element is another list representing a single period
  - each element of the inner list is an object representing a topic found in
    that period
  - each of these objects has
    - an `id`, which is the numeric identifier of the topic's theme
    - a `health`, the topic's health as a floating point number
    - a list of `words` as strings, which are the words that make up the topic

For an example of how this model is used in practice, see the [implementation of
a Python interface to the
model](https://github.com/hpicgs/unCover/tree/main/tem) for
[unCover](https://github.com/hpicgs/unCover)
