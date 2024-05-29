# Python interface to TEM

This directory features the Python interface to TEM, which also implements
parallelization. For examples of using this, see the [main unCover
repository](https://github.com/hpicgs/unCover).

## Usage

We recommend using Python 3.10 but other versions most likely work as well. To
get started, download and install the requirements:

- install `requirements.txt`
- run `python3 -m nltk.download punkt`
- for English: run `python -m spacy download en_core_web_md`
- for German: run `python -m spacy download de_core_news_md`

To use the interface, make sure you set the `TEM_WORD_DISTANCE_ENDPOINT`
environment variable, e.g.

```python
import os
os.environ['TEM_WORD_DISTANCE_ENDPOINT'] = 'http://localhost:8000/similarity'

from script.process import TEM
model = TEM([...])
```
