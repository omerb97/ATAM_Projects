.global _start

.section .text
_start:
movq $0, %rcx #rcx is count
movq $0, %rax #rax is sum
movq $arr, %rbx
begin:
    lea (%rbx,%rcx, 4), %r10 
    cmp $0, (%r10)
    je end
    movzx (%r10), %r9
    addq %r9, %rax
    inc %rcx
    jmp begin
          
end:
    cmp $0, %rax
    je final
    movq $0, %rdx
    div %rcx
final: 
    movl %eax, (avg)

    