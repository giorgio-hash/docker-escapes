FROM python:3.8
RUN apt-get update && apt-get install -y ncat && pip install requests
COPY dockerio.py /dockerio.py
CMD ["/bin/bash"]
