.global _start

.section .text
_start:
movq head, %r9
movq src, %r13
movq dst, %r14
movq %r9, %rbx
movq $0, %r12
movq $0, %r15
whileloop:
    movq (%r9), %rax
    cmp %rax, %r14
    je controlSwitch
    cmp %rax, %r13
    jne AfterFirstIf
    movq $1, %r12
    movq %r9, %r10
    movq %rbx, %r11
    AfterFirstIf:
    cmp $1, %r15
    je AfterSecondIf
    movq %r9, %rbx
    movq $1, %r15
    AfterSecondIf:
    movq %r9, %rbx
    movq 8(%r9), %r9
    jmp whileloop
    
controlSwitch:
movq head, %rax
cmp $0, %r12
je end
cmp %r10, %rax
je SrcIsHead
movq 8(%r10), %r15
cmp %r9, %r15 
je CloseNotHead
movq %r10, 8(%rbx)
movq %r9, 8(%r11)
movq 8(%r10), %rcx
movq 8(%r9), %rax
movq %rax, 8(%r10)
movq %rcx, 8(%r9)
jmp end

CloseNotHead:
movq %r9, 8(%r11)
movq 8(%r9), %rax
movq %rax, 8(%r10)
movq %r10, 8(%r9)
jmp end

SrcIsHead:
movq 8(%r10), %r15
cmp %r9, %r15 
je CloseHead
movq %r10, 8(%rbx)
movq 8(%r10), %rcx
movq 8(%r9), %rax
movq %rax, 8(%r10)
movq %rcx, 8(%r9)
movq %r9, head
jmp end

CloseHead:
movq 8(%r9), %rax
movq %rax, 8(%r10)
movq %r10, 8(%r9)
movq %r9, head

end:

