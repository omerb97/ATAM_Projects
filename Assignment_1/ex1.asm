.global _start

.section .text
_start:
movq num, %rax
movl $0, %ebx
movl $0x40, %ecx
begin:
    rol $0x1, %rax
    jae countercheck
    inc %ebx
countercheck:
    dec %ecx
    je end
    jmp begin
end:
    movl %ebx, countBits



    
    

