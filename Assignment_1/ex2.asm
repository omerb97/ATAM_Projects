.global _start

.section .text
_start:
movq $source, %r9
movq $destination, %r10
movq $0, %rcx
movl num, %ecx
cmp $0, %ecx
jle end
dec %rcx
begin:
	lea (%r9, %rcx, 1),%rbx #to copy from 
	lea (%r10, %rcx, 1), %rax #copy to
	movb (%rbx), %r11b
	movb %r11b, (%rax)
	dec %rcx
	cmp $0, %rcx
	jge begin 	
end:
