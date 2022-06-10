FROM ubuntu:latest

ENV FLASK_APP rentex
ENV FLASK_ENV development
# ENV MONGO_HOST mongo
# ENV REDIS_HOST redis
# ENV REDIS_CRT redis.crt
# ENV CLIENT_KEY client_2.key
# ENV CLIENT_CRT client_2.crt
# ENV KEY_FOLDER /root/tls

WORKDIR /root

COPY . .
# COPY /tls tls

RUN apt update &&\
    apt install -y python3-venv libpq-dev &&\
    python3 -m venv .venv

RUN .venv/bin/pip install /root/src/dist/rentex-0.1-py3-none-any.whl

CMD [".venv/bin/python3", "-m", "flask", "run", "-h", "0.0.0.0", "--no-debugger", "--no-reload"]