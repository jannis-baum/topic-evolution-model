import os
import re

from nltk.tokenize import sent_tokenize
import spacy

# ------------------------------------------------------------------------------
# MARK: normalization

def lower_alnum(doc: str) -> str:
    characters = [
        char.lower() for char in doc if char.isalnum() or char.isspace()
    ]
    return ''.join(characters)

# https://stackoverflow.com/a/62723088
def normalize_quotes(doc: str) -> str:
    # single quotes
    doc = re.sub(r'[\u02BB\u02BC\u066C\u2018-\u201A\u275B\u275C]', '\'', doc)
    # double quotes
    doc = re.sub(r'[\u201C-\u201E\u2033\u275D\u275E\u301D\u301E]', '"', doc)
    # apostrophes
    doc = re.sub(
        r'[\u0027\u02B9\u02BB\u02BC\u02BE\u02C8\u02EE\u0301\u0313\u0315\u055A\u05F3\u07F4\u07F5\u1FBF\u2018\u2019\u2032\uA78C\uFF07]',
        '\'', doc)
    return doc

def normal_str(doc: str) -> str:
    return lower_alnum(normalize_quotes(doc))

__lang2model = { 'en': 'en_core_web_md', 'de': 'de_core_news_md' }
__lemma = spacy.load(__lang2model[os.getenv('TEM_LANG', 'en')])
def normal_tokens(doc: str) -> list[str]:
    return [token.lemma_ for token in __lemma(normal_str(doc))]

# ------------------------------------------------------------------------------
# MARK: TEM prep

# dash needs to be escaped
# other chars are en-/em-dash
__doc_separators = ',:;\\-–—'
def docs_from_period(period: str) -> list[list[str]]:
    with_seps = re.sub(f'[{__doc_separators}]', '.', period)
    return [
        tokens
        for tokens in map(normal_tokens, sent_tokenize(with_seps))
        if len(tokens) > 0
    ]

# merge period into predecessor if number of docs < min_docs
def merge_short_periods(corpus: list[list[list[str]]], min_docs = 2) -> list[list[list[str]]]:
    if len(corpus) == 0: return []
    merged = [corpus[0]]
    for period in corpus[1:]:
        if len(merged[-1]) >= min_docs:
            merged.append(period)
        else:
            merged[-1] += period
    return merged if len(merged[-1]) > min_docs else merged[:-1]

# create structured corpus from text, suitable for TEM input
def get_structured_corpus(text: str) -> str:
    corpus = [
        period
        for period in map(docs_from_period, text.split('\n'))
        if len(period) > 0
    ]
    corpus = merge_short_periods(corpus, min_docs=2)
    if len(corpus) < 2: raise ValueError('Corpus has less than 2 periods.')

    return '\n\n'.join([
        '\n'.join([
            ' '.join(word for word in doc)
        for doc in period])
    for period in corpus])
