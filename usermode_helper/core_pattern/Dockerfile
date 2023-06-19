FROM alpine:latest

RUN apk add build-base
ADD corepatesc.sh /
ADD runme.sh /
ADD crash.c /
RUN gcc -o crash crash.c
ENTRYPOINT ["/bin/sh"]
