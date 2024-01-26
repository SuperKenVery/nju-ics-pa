from pyrsp.rsp import i386
import subprocess
import argparse
import os
import time

class Qemu:
    MEM_AVAILABLE=True
    def __init__(self,filename,mem_cmp_size):
        assert os.path.isfile(filename),f"File {filename} doesn't exist or isn't a file. "
        self.filename=filename
        subprocess.Popen(["qemu-system-i386","-s","-S","-machine","type=pc-i440fx-3.1","-kernel",filename])
        time.sleep(0.1)
        # exit(12)
        self.debugger=i386(port='1234',elffile=filename)
        self.debugger.load(verify=False)
        self.mem_cmp_size=mem_cmp_size

    def stepi(self):
        # Run stepi, and return registers and memory
        self.debugger.step()
        # TODO: Use official pyrsp, si is not a rsp command...
        self.debugger.refresh_regs()

        ret={}
        for reg in ['eax','ebx','ecx','edx','esp','ebp','esi','edi','eip','eflags']:
            ret[reg]=int(self.debugger.regs[reg],base=16)

        ret['mem']=self.debugger.dump(self.mem_cmp_size)

        return ret
    
class Nemu:
    MEM_AVAILABLE=False
    def __init__(self,filename,nemu_path='nemu/nemu'):
        assert os.path.isfile(filename),f"File {filename} doesn't exist or isn't a file. "
        # assume that file is one of the testcases
        from pwn import process
        self.sh=process([nemu_path,"--testcase",os.path.basename(filename)])
        self.sh.recvuntil(b"(nemu) ")

    def fetch(self,cmd):
        self.sh.sendline(cmd if isinstance(cmd,bytes) else cmd.encode('ascii'))
        result=self.sh.recvuntil(b"(nemu) ")
        # Remove the last (nemu) line
        result=b'\n'.join(result.split(b'\n')[:-1])

        return result.decode()

    def stepi(self):
        result=self.fetch('si')
        if 'GOOD' in result and 'TRAP' in result:
            # Maybe color will mess things up... So check GOOD and TRAP seperately
            print("HIT GOOD TRAP")
            exit(0)
        regs_raw=self.fetch('info r')
        regs={}
        for regline in regs_raw.split('\n'):
            splitted=regline.split('\t')
            if len(splitted)!=2: continue

            regname,val_raw=splitted
            regs[regname]=int(val_raw,base=16)
        return regs
        
    

def cmp_run(file,mem_cmp_size):
    from pwn import hexdump
    qemu=Qemu(file,mem_cmp_size)
    nemu=Nemu(file)

    while True:
        qemu_env=qemu.stepi()
        nemu_env=nemu.stepi()

        for reg in ['eax','ebx','ecx','edx','esp','ebp','esi','edi','eip','eflags']:
            if qemu_env[reg]!=nemu_env[reg]:
                print(f"nemu_eip={nemu_env['eip']},qemu_eip={qemu_env['eip']}\t{reg} mismatch: qemu is {qemu_env[reg]}, nemu is {nemu_env[reg]}")
                print("QEMU:")
                print(qemu_env)
                print("NEMU:")
                print(nemu_env)
                exit(1)
            
        if nemu.MEM_AVAILABLE and qemu.MEM_AVAILABLE:
            nmem=nemu_env['mem']
            qmem=qemu_env['mem']
            for i in range(mem_cmp_size):
                if nmem[i]!=qmem[i]:
                    print(f"Memory mismatch at byte {i}!")
                    from pwn import hexdump
                    print("QEMU:")
                    hexdump(qmem)
                    print("NEMU:")
                    hexdump(nmem)
                    exit(1)

        print(f"nemu_eip={nemu_env['eip']},qemu_eip={qemu_env['eip']}\tpass")
                    

    

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument("elf_name",help="name of the elf file",type=str)
    parser.add_argument("mem_cmp_size",help="How much memory to compare, in bytes",type=int,default=1024)
    args=parser.parse_args()

    cmp_run(args.elf_name,args.mem_cmp_size)
    
