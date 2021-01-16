#
#	util.S
#
.intel_syntax noprefix
.text

# RDI, RSI, RDX, RCX, R8, R9  : R10=RCX 


.global syscall
syscall:
#
	mov rax,0
	mov r10,rcx
	syscall
	jb err
	ret
err:
	push rax
	call __error
	pop rcx
	mov [rax], ecx
	mov rax,-1
	mov rdx,-1
	ret
#

## renamed to kernel_utils style naming

.global readMsr		//# u64 readmsr(u32 msr)
readMsr:
	mov ecx,edi
	rdmsr
	shl rdx,32
	or rax,rdx
	ret	
	
.global readCr0		//# u64 readcr0(void)
readCr0:
	mov rax,cr0
	ret

.global writeCr0	//# void writecr0(u64 val)
writeCr0:
	mov cr0,rdi
	ret

.global readCr3		//# u64 readcr3(void)
readCr3:
	mov rax,cr3
	ret
	
.global writeCr3	//# void writecr3(u64 val)
writeCr3:
	mov cr3,rdi
	ret



.global __sidt
__sidt:
	sidt fword ptr [rdi+0]
	ret



