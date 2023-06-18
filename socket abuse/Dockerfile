FROM python:3.8

RUN pip install requests

#RUN groupadd -g 142 docker <--- ubuntu
#RUN groupadd -g 985 docker <--- centOS7
# getent group docker | cut -d ":" -f 3 <-- per trovarlo genericamente
ARG dock_group
RUN groupadd -g ${dock_group} docker
RUN useradd developer && usermod -aG ${dock_group} developer

WORKDIR /home/developer
ADD dockerio.py /home/developer
RUN chmod a+rwx dockerio.py
USER developer

CMD ["/bin/bash"]
