FROM alpine:latest

RUN apk update && apk add build-base

ADD brute.sh /
ADD brute.c /

RUN gcc -o brute brute.c

RUN chmod 777 brute brute.sh



ENTRYPOINT ["./brute.sh"] 