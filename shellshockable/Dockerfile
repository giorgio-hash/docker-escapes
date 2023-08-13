FROM ubuntu:18.04

ENV APACHE_RUN_USER www-data
ENV APACHE_RUN_GROUP www-data
ENV APACHE_LOG_DIR /var/log/apache2
ENV APACHE_LOCK_DIR /var/run/apache2
ENV APACHE_PID_FILE /var/run/apache2.pid
ENV APACHE_RUN_DIR /var/run/apache2



RUN apt-get update
RUN apt-get install -y apache2
RUN apt-get install -y libapache2-mod-python 
RUN apt-get install -y dos2unix

ADD cgi-bin/* /usr/lib/cgi-bin/

# correggi i programmi cgi se creati in Windows
RUN find /usr/lib/cgi-bin/ -type f -print0 | xargs -0 dos2unix 
RUN chmod +x /usr/lib/cgi-bin/*

COPY conf-files/apache2.conf /etc/apache2/apache2.conf
COPY conf-files/serve-cgi-bin.conf /etc/apache2/conf-available/serve-cgi-bin.conf

#attiva moduli CGI
RUN a2enmod cgid
# bash vulnerabile
RUN apt-get install -y build-essential wget
RUN wget https://snapshot.debian.org/archive/debian/20140304T040604Z/pool/main/b/bash/bash_4.1-3_amd64.deb --no-check-certificate
RUN dpkg -i bash_4.1-3_amd64.deb

EXPOSE 80

ENTRYPOINT ["/usr/sbin/apache2"]

CMD ["-D", "FOREGROUND"]
