FROM python:3.10-alpine

WORKDIR /app
COPY . .
RUN pip install -r requirements.txt
CMD python -m gunicorn -b 0.0.0.0 app:app
