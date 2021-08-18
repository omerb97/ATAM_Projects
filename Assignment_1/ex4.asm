.global _start

.section .text
_start:
movq $arr, %r13
movq $1, %r9
movq $0, %r10
movq $1, %r11
movq $0, %r12
movl n, %eax
dec %eax
movq $0, %rcx

forloop:
    cmp %ecx, %eax
    je outloop
    lea (%r13, %rcx, 4), %r14
    movl (%r14), %r15d
    movl 4(%r14), %ebx
    cmp %r15d, %ebx
    ja BreakStreak
    inc %r11
    jmp IncreaseLoop
    
    BreakStreak:
    cmp %r9, %r11
    ja NewMax
    movq %rcx, %r12
    movq $1, %r11
    jmp IncreaseLoop
    
    NewMax:
    movq %r11, %r9
    movq %r12, %r10
    movq %rcx, %r12
    movq $1, %r11
    jmp IncreaseLoop
    
    IncreaseLoop:
    inc %rcx
    jmp forloop
    
outloop:
    cmp %r9, %r11
    jbe IndexCheck
    
    movq %r11, %r9
    movq %r12, %r10
    
IndexCheck:
    cmp $0, %r10
    je end
    inc %r10
end:
    movl %r10d, begin
    movl %r9d, len 
    

