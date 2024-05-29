# Topic Evolution Model

TEM is a graph-based temporal topic modeling for very small corpora. Detailed
information about TEM can be found in the following publication

> Liebe L, Baum J, Cech T, Scheibel W, and Dollner J (2024). Detecting and
> Comparing LLM Capabilities to Human Writers through Linguistic Analysis

## Usage

To use TEM, first build the model executable and term distance server Docker
container.

- Build the model:
  ```sh
  mkdir build && cd build
  cmake .. && make
  ```
  An executable will be compiled into `build/topic_evolution_model`.
- Build the term distance server:
  ```sh
  make -C term-distance build
  ```
  Note that this may take a while since it downloads word2vec models (total of
  around 8GB).

Once this is set up, you can start using TEM.

- Run the term distance server, e.g. with
  ```sh
  make -C term-distance run
  ```
  This will expose the server on `localhost:8000`. To stop the server, run
  ```sh
  make -C term-distance kill
  ```
- You can then run the model executable (in `build/topic_evolution_model`)
  directly or use the provided Python interface in [the `script/`
  directory](./script), which also features parallelization.\
  Note that you need to specify the URL to the term distance server via the
  environment variable `TEM_WORD_DISTANCE_ENDPOINT`, e.g. if running the
  executable directly
  ```sh
  TEM_WORD_DISTANCE_ENDPOINT=http://localhost:8000/similarity \
    build/topic_evolution_model [...]
  ```
