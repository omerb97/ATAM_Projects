.global hook

.section .data
msg: .ascii "This code was hacked by Noa Killa's gang\n"
endmsg:

.section .text
hook:
  movq $_start, %rax
  movb $0xc3, 0x1e(%rax)

  call _start

  movq $1, %rax
  movq $endmsg, %r11
  sub $msg, %r11
  movq %r11, %rdx
  movq $msg, %rsi
  syscall  

  call ending

  movq $60, %rax
  movq $0, %rdi
  syscall

