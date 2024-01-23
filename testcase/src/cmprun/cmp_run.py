#!/usr/bin/env python
# By SuperKenVery
import subprocess
import argparse
import os
import time
# from pygdbmi.gdbcontroller import GdbController

# Unused
class Qemu:
    MEM_AVAILABLE=False
    def __init__(self,filename,mem_cmp_size):
        assert os.path.isfile(filename),f"File {filename} doesn't exist or isn't a file. "
        self.filename=filename
        subprocess.Popen(["qemu-system-i386","-s","-S","-machine","type=pc-i440fx-3.1","-kernel",filename])
        time.sleep(0.1)
        self.debugger=GdbController()
        self.debugger.write(f"-file-exec-and-symbols {filename}")
        self.debugger.write("set arch i386:x64-32:intel")
        self.debugger.write("-target-select remote :1234")
        self.debugger.write("starti")
        initial_regs=self.get_regs()
        self.initial_esp=initial_regs['esp']

    def get_regs(self):
        reg_names=self.debugger.write("-data-list-register-names")[0]['payload']['register-names']
        regs=self.debugger.write("-data-list-register-values x")[0]['payload']['register-values']
        reg_vals={}
        for regname in ['eax','ebx','ecx','edx','esp','ebp','esi','edi','eip','eflags']:
            index=reg_names.index(regname)
            for reg in regs:
                if int(reg['number'])==index:
                    reg_vals[regname]=int(reg['value'],base=16)
                    break
        return reg_vals
        

    def stepi(self):
        last=self.debugger.write("x/i $pc")
        self.last_instr=last[1]['payload']
        step_res=self.debugger.write("-exec-step-instruction")
        for msg in step_res:
            if msg['message']=='stopped':
                reason=msg['payload']['reason']
                if reason!='end-stepping-range':
                    print(f"QEMU stopped: {reason}")
                    exit(1)
        regs=self.get_regs()
        env=regs.copy()
        del env['esp']
        del env['ebp']
        env['rela_esp']=regs['esp']-self.initial_esp
        env['rela_ebp']=regs['ebp']-regs['esp']
        return env
    
class Nemu:
    MEM_AVAILABLE=False
    def __init__(self,testcase,kernel=False,nemu_path='nemu/nemu'):
        assert os.path.isfile(nemu_path),f"File {nemu_path} doesn't exist or isn't a file. "
        # assume that file is one of the testcases
        from pwn import process,context
        # context.log_level='DEBUG'
        cmdline=[nemu_path,"--testcase",testcase]
        if kernel:
            cmdline.append("--kernel")
        self.sh=process(cmdline)
        self.sh.recvuntil(b"(nemu) ")

    def fetch(self,cmd):
        self.sh.sendline(cmd if isinstance(cmd,bytes) else cmd.encode('ascii'))
        # result=self.sh.recvuntil(b"(nemu) ")
        try:
            result=self.sh.recvuntil(b"nemu")
        except EOFError:
            return self.sh.recv().decode()
        # Remove the last (nemu) line and the first echo line
        result=b'\n'.join(result.split(b'\n')[1:-1])

        return result.decode()

    def stepi(self):
        result=self.fetch('si 1')
        if result=='':
            rest=self.sh.recv().decode()
            if 'GOOD' in rest and 'TRAP' in rest:
                # Maybe color will mess things up... So check GOOD and TRAP seperately
                print("HIT GOOD TRAP")
                exit(0)
            else:
                print("Did not receive anything when single stepping")
        regs=self.get_regs()
        return regs

    def skip(self,step):
        result=self.fetch(f"si {step}")
        if result=='':
            rest=self.sh.recv().decode()
            if 'TRAP' in rest:
                if 'GOOD' in rest:
                    print("HIT GOOD TRAP")
                    exit(0)
                elif 'BAD' in rest:
                    print("HIT BAD TRAP in skipped code")
                    exit(0)
            else:
                print("Didn't recieve anything while skipping")
        regs=self.get_regs()
        return regs

    def get_regs(self):
        regs_raw=self.fetch('info r')
        regs={}
        for regline in regs_raw.split('\n'):
            splitted=regline.split('\t')
            if len(splitted)!=2: continue

            regname,val_raw=splitted
            regs[regname]=int(val_raw,base=16)
        return regs
        
    
def fail(nemu_env,ref_env,mismatch,extra_msg=None):
    nemu_env=nemu_env.copy()
    ref_env=ref_env.copy()
    print(f"nemu_eip={hex(nemu_env['eip'])},nemu_ref_eip={hex(ref_env['eip'])}\t{ANSI.FAIL}{mismatch}{ANSI.ENDC} mismatch: nemu_ref is {hex(ref_env[mismatch])}, nemu is {hex(nemu_env[mismatch])}")
    if extra_msg:
        print(extra_msg)
    for i in nemu_env:
        nemu_env[i]=hex(nemu_env[i])
    for i in ref_env:
        ref_env[i]=hex(ref_env[i])
    print("NEMU_REF:")
    print(ref_env)
    print("NEMU:")
    print(nemu_env)

    go=input("Quit? (Y/n)")
    if go=="n":
        return
    else:
        exit(1)
    
def check_eflags(nemu_env,ref_env):
    neflags=nemu_env['eflags']
    qeflags=ref_env['eflags']
    CF=("CF",0)
    PF=("PF",2)
    ZF=("ZF",6)
    SF=("SF",7)
    OF=("OF",11)
    for name,bit in CF,PF,ZF,SF,OF:
        nflag=neflags & 2**bit
        qflag=qeflags & 2**bit
        if nflag!=qflag:
            fail(nemu_env,ref_env,'eflags',f"{ANSI.FAIL}{name}{ANSI.ENDC}: nemu_ref={qflag} nemu={nflag}")

def cmp_run(testcase:str,mem_cmp_size:int,kernel:bool,skip:int):
    from pwn import hexdump
    # nemu_ref=Qemu(file,mem_cmp_size)
    nemu_ref=Nemu(testcase,kernel=kernel,nemu_path='nemu/nemu_ref')
    nemu_ref.skip(skip)
    nemu=Nemu(testcase,kernel=kernel)
    nemu.skip(skip)

    step=skip
    
    while True:
        ref_env=nemu_ref.stepi()
        nemu_env=nemu.stepi()
        step+=1

        try:
            for reg in ['eax','ebx','ecx','edx','esp','ebp','esi','edi','eip']:
                if ref_env[reg]!=nemu_env[reg]:
                    fail(nemu_env,ref_env,reg)
        except KeyError:
            print("nemu exited abnormally")
            exit(1)

        # For eflags we only check cf,pf,zf,sf,of
        check_eflags(nemu_env,ref_env)

            
        if nemu.MEM_AVAILABLE and nemu_ref.MEM_AVAILABLE:
            nmem=nemu_env['mem']
            qmem=ref_env['mem']
            for i in range(mem_cmp_size):
                if nmem[i]!=qmem[i]:
                    print(f"Memory mismatch at byte {i}!")
                    from pwn import hexdump
                    print("NEMU_REF:")
                    hexdump(qmem)
                    print("NEMU:")
                    hexdump(nmem)
                    exit(1)

        print(f"{ANSI.UP}{ANSI.CLEAR}{step}\tip={hex(nemu_env['eip'])}\t{ANSI.OKGREEN}pass{ANSI.ENDC}")
                    

# https://stackoverflow.com/questions/287871/how-do-i-print-colored-text-to-the-terminal
# https://blog.finxter.com/how-to-overwrite-the-previous-print-to-stdout-in-python/
class ANSI:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    UP = '\033[1A'
    CLEAR = '\x1b[2K'    

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument("testcase_name",help="name of the test case",type=str)
    parser.add_argument("-m","--mem_cmp_size",help="How much memory to compare, in bytes. (Not implemented yet)",type=int,default=1024,required=False)
    parser.add_argument("-k","--kernel",action="store_true",help="Whether to add --kernel to nemu")
    parser.add_argument("-s","--skip",help="Skip first x instructions, to reduce wait time",type=int,default=0,required=False)
    args=parser.parse_args()

    cmp_run(args.testcase_name,args.mem_cmp_size,args.kernel,args.skip)
    
