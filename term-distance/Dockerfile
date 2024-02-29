FROM python:3.8-slim-bullseye

WORKDIR /app

RUN pip install gensim>=4.3.2
RUN python -m gensim.downloader --download word2vec-google-news-300

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .
CMD ./run.sh
