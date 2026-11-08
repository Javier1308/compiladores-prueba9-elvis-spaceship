.data
print_fmt:.string "%ld\n" 
.text
.globl main
buscar:
pushq %rbp
movq %rsp, %rbp
subq $48, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq %rdx, -24(%rbp)
movq $0, -32(%rbp)
movq $0, -40(%rbp)
movq $0, %rax
movq %rax, -32(%rbp)
while_0:
movq -32(%rbp), %rax
movq %rax, %rcx
movq -16(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq -8(%rbp), %rax
pushq %rax
movq -32(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rcx
movq -24(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rax
movq %rax, %rcx
movq $0, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_1
movq -32(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
jmp .Lfin_buscar
jmp endif_1
else_1:
endif_1:
movq -32(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -32(%rbp)
jmp while_0
endwhile_0:
movq $0, %rax
jmp .Lfin_buscar
movq $0, %rax
.Lfin_buscar:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $0, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $40, %rax
movq %rax, %rdi
call malloc@PLT
movq %rax, -8(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $10, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $25, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $2, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $50, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $3, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $75, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $100, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $5, %rax
pushq %rax
movq $50, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call buscar
cmpq $0, %rax
jne elvis_end_2
movq $-1, %rax
elvis_end_2:
movq %rax, -16(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $5, %rax
pushq %rax
movq $99, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call buscar
cmpq $0, %rax
jne elvis_end_3
movq $-1, %rax
elvis_end_3:
movq %rax, -24(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $5, %rax
pushq %rax
movq $10, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call buscar
cmpq $0, %rax
jne elvis_end_4
movq $999, %rax
elvis_end_4:
movq %rax, -32(%rbp)
movq -16(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -24(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -32(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
movq %rax, %rdi
call free@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
