.global get_elemnt_from_matrix, multiplyMatrices
.extern set_elemnt_in_matrix

.section .text
get_elemnt_from_matrix:
	#rdi is matrix
	#rsi is n
	#rdx is row
	#rcx is col
	pushq %rbp #prologe
	movq %rsp, %rbp

	push %r11
	movq $0, %r11
	imul %edx,%esi
	add %ecx, %esi
	imul $4, %esi
	add %esi, %r11d
	add %rdi, %r11
	movq $0, %rax
	movl (%r11), %eax
	pop %r11
	leave #epilouge
	ret

multiplyMatrices:
	pushq %rbp
	movq %rsp, %rbp
	#r12 is i
	#r13 is j
	#r14 is k
	#r15 is sum
	movq $0, %rbx
	movl 16(%rbp),%ebx
	movq %rdi, %r11 #r11 points to first
	push %rbx
	push %r12
	push %r13
	push %r14
	push %r15

	movq $0, %r12
ResultRowLoop:
	movq $0, %r13
	ResultColLoop:
		movq $0, %r14
		movq $0, %r15
		SumLoop:
			push %rdi
			push %rsi
			push %rdx
			push %rcx
			push %r8
			push %r9
			push %r10
			push %r11
			movq %r11, %rdi #put first as parameter 1
			movq %r8, %rsi #put n as parameter 2
			movq %r12,%rdx #put i as parameter 3
			movq %r14, %rcx #put k as parameter 4
			call get_elemnt_from_matrix
			pop %r11
			pop %r10
			pop %r9
			pop %r8
			pop %rcx
			pop %rdx
			pop %rsi
			pop %rdi

			movq %rax, %r10 #put result at temp reg r10

			push %rdi
			push %rsi
			push %rdx
			push %rcx
			push %r8
			push %r9
			push %r10
			push %r11
			movq %rsi, %rdi #put second as parameter 1
			movq %r9, %rsi #put r as parameter 2 
			movq %r14, %rdx #put k as parameter 3 
			movq %r13, %rcx #put j as parameter 4
			call get_elemnt_from_matrix
			pop %r11
			pop %r10
			pop %r9
			pop %r8
			pop %rcx
			pop %rdx
			pop %rsi
			pop %rdi

			imul %r10, %rax #put mul result into rax 
			add %rax, %r15	#add result into sum
			inc %r14
			cmp %r14, %r8
			jne SumLoop
		push %rdi
		push %rsi
		push %rdx
		push %rcx
		push %r8
		push %r9
		push %r10
		push %r11
		mov %r15, %rax
		mov %rdx, %rdi #put result mat as parameter 1 
		movq $0, %rdx
		div %rbx #puts mod p res into rdx
		movq %rdx, %r8 #puts sum result as parameter 5
		movq %r9, %rsi #puts r as parameter 2
		movq %r12, %rdx #puts i as parameter 3
		movq %r13, %rcx #puts j as parameter 4	
		call set_elemnt_in_matrix
		pop %r11
		pop %r10
		pop %r9
		pop %r8
		pop %rcx
		pop %rdx
		pop %rsi
		pop %rdi

		inc %r13
		cmp %r13, %r9
		jne ResultColLoop
	inc %r12
	cmp %rcx, %r12
	jne  ResultRowLoop

finish:
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbx
	leave
	ret
