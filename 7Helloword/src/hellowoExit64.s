#for 64bit
#worked on testing machine
#Linux 4acada6c5b79 4.4.0-93-generic #116~14.04.1-Ubuntu SMP Mon Aug 14 16:07:05 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
.text

.global _start

_start:
	# write(1, message, 13)
	mov     $1, %rax        
	mov     $1, %rdi        
	mov     $message, %rsi
	mov     $13, %rdx      
	syscall                  

	message:
	.ascii  "Hello, world\n"
	
