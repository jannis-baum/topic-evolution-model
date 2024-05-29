ARG BASE_IMAGE=python:3.8-slim-bullseye

FROM ${BASE_IMAGE} AS downloader

RUN pip install gensim>=4.3.2
COPY ./model.py .
RUN python model.py



FROM ${BASE_IMAGE} AS runner

COPY --from=downloader /root/gensim-data /root/gensim-data

WORKDIR /app

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .
CMD ./run.sh
