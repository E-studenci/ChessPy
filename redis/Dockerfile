FROM redis:6.2.6

COPY redis.conf /usr/local/etc/redis/redis.conf

COPY tls/server /data/
COPY tls/clients /usr/local/share/ca-certificates/

# RUN apt-get update &&\
#     apt-get install -y ca-certificates &&\
#     rm /etc/ssl/certs/* &&\
#     > /etc/ca-certificates.conf &&\
#     update-ca-certificates

EXPOSE 6379

CMD [ "redis-server", "/usr/local/etc/redis/redis.conf" ]