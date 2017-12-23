#for 32 bit
#worked on testing machine
#Linux 4acada6c5b79 4.4.0-93-generic #116~14.04.1-Ubuntu SMP Mon Aug 14 16:07:05 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux

.text

.global _start

_start:


# write our string to stdout.

movl    $len,%edx       # arg3: message length.
movl    $msg,%ecx       # arg2: pointer to message to write.
movl    $1,%ebx	        # arg1: file handle (stdout).
movl    $400,%eax	        # sys_write
int     $0x80           # syscall

# and exit.

movl    $5,%ebx         # exit code 0
movl    $1,%eax         # sys_exit
int     $0x80
.data

msg:
.ascii  "Hello, world!\n"
len = . - msg                  # length
