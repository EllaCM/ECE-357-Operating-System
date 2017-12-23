#version for 64 bit
#worked on testing machine
#Linux 4acada6c5b79 4.4.0-93-generic #116~14.04.1-Ubuntu SMP Mon Aug 14 16:07:05 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux


.global _start

.text

_start:
		# using invalid syscall no 31415 instead of 1 for write()
		mov     $400, %rax
		mov     $1, %rdi       
		mov     $message, %rsi
		mov     $13, %rdx      
		syscall                  

		# exit(5)
		mov     $60, %rax     
		mov     $5, %rdi
		syscall                  
		message:
		.ascii  "Hello, world\n"
