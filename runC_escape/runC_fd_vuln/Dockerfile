
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y build-essential 

COPY runCescape.c /
COPY intercept.sh /


RUN gcc -o runCescape runCescape.c


ENTRYPOINT ["bash","intercept.sh"]
