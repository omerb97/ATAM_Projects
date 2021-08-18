.global my_ili_handler
.extern what_to_do
.extern old_ili_handler

.text
.align 4, 0x90
my_ili_handler:
pushq %rbp
movq %rsp, %rbp #prologe

push %rsi
movq 16(%rsp), %rsi
movq (%rsi), %rsi
movq $0, %rdi
cmp $0x0F, %sil
jne OneByte

TwoByte:
	shrq $8, %rsi
	movq $2, %rcx
	movb %sil, %dil
	jmp Handle
OneByte:
	movq $1, %rcx	
	movb %sil, %dil
Handle:
	push %rax
	push %rsi
	push %rdx
	push %rcx
	push %r8
	push %r9
	push %r10
	push %r11
	call what_to_do
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rcx
	pop %rdx
	pop %rsi
	cmp $0, %rax
	je ReturnZero

ReturnNotZero:
	movq %rax, %rdi
	cmp $2, %rcx
	jne NotZeroOneByte
	addq $2, 24(%rsp)
	jmp Finish
NotZeroOneByte:
	addq $1, 24(%rsp)
	jmp Finish
ReturnZero:
	pop %rax
	pop %rsi
	leave
	jmp *old_ili_handler
Finish:
pop %rax
pop %rsi
leave	
iretq
