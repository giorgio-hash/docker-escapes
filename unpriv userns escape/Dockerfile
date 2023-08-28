FROM ubuntu:18.04

#disabilitare seccomp, apparmor,SELinux e abilitare unprivileged userns prima di eseguire
#utente base del container dev'essere "root" ; non funziona coi non-root users

#USER nonroot

ADD new_cgesc.sh /
ADD unshare_and_escape.sh /
ADD find_root_cgroup.sh /
RUN chmod 777 new_cgesc.sh unshare_and_escape.sh find_root_cgroup.sh

ENTRYPOINT [ "./unshare_and_escape.sh"]





