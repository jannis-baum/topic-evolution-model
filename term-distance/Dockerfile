FROM python:3.8-slim-bullseye

RUN pip install gensim>=4.3.2
RUN python -m gensim.downloader --download word2vec-google-news-300

WORKDIR /app
COPY . .
RUN pip install -r requirements.txt
CMD python -m gunicorn -b 0.0.0.0 app:app
