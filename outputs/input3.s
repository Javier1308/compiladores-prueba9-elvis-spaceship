.data
print_fmt:.string "%ld\n" 
.text
.globl main
procesarPila:
pushq %rbp
movq %rsp, %rbp
subq $48, %rsp
movq %rdi, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $0, -40(%rbp)
movq $0, %rax
movq %rax, -40(%rbp)
movq -8(%rbp), %rax
cmpq $0, 8(%rax)
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_0
movq $0, %rax
jmp .Lfin_procesarPila
jmp endif_0
else_0:
endif_0:
movq -8(%rbp), %rax
movq (%rax), %rdi
cmpq $0, %rdi
je .Lpila_vacia
movq 8(%rdi), %rcx
movq %rcx, (%rax)
decq 8(%rax)
pushq (%rdi)
subq $8, %rsp
call free@PLT
addq $8, %rsp
popq %rax
movq %rax, -16(%rbp)
while_1:
movq -8(%rbp), %rax
cmpq $0, 8(%rax)
sete %al
movzbq %al, %rax
movq %rax, %rcx
movq $0, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_1
movq -8(%rbp), %rax
movq (%rax), %rdi
cmpq $0, %rdi
je .Lpila_vacia
movq 8(%rdi), %rcx
movq %rcx, (%rax)
decq 8(%rax)
pushq (%rdi)
subq $8, %rsp
call free@PLT
addq $8, %rsp
popq %rax
movq %rax, -24(%rbp)
movq -16(%rbp), %rax
movq %rax, %rcx
movq -24(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
movq %rax, -32(%rbp)
movq -40(%rbp), %rax
movq %rax, %rcx
movq -32(%rbp), %rax
addq %rcx, %rax
movq %rax, -40(%rbp)
movq -24(%rbp), %rax
movq %rax, -16(%rbp)
jmp while_1
endwhile_1:
movq -40(%rbp), %rax
jmp .Lfin_procesarPila
movq $0, %rax
.Lfin_procesarPila:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $16, %rdi
call malloc@PLT
movq $0, (%rax)
movq $0, 8(%rax)
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $10, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
pushq %rax
movq $20, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
pushq %rax
movq $30, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
pushq %rax
movq $40, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
pushq %rax
popq %rdi
call procesarPila
cmpq $0, %rax
jne elvis_end_2
movq $999, %rax
elvis_end_2:
movq %rax, -16(%rbp)
movq -16(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
pushq %rax
popq %rdi
call procesarPila
cmpq $0, %rax
jne elvis_end_3
movq $777, %rax
elvis_end_3:
movq %rax, -24(%rbp)
movq -24(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.Lpila_vacia:
andq $-16, %rsp
movq stderr@GOTPCREL(%rip), %rax
movq (%rax), %rdi
leaq pila_vacia_msg(%rip), %rsi
movl $0, %eax
call fprintf@PLT
movl $1, %edi
call exit@PLT
.data
pila_vacia_msg:.string "Error: pop/top sobre una pila vacia\n"
.section .note.GNU-stack,"",@progbits
