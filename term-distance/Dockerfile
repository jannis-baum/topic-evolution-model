FROM python:3.8-slim-bullseye

WORKDIR /app

RUN pip install gensim>=4.3.2
COPY ./models.py .
RUN python models.py

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .
CMD ./run.sh
