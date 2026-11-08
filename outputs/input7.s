.data
print_fmt:.string "%ld\n" 
.text
.globl main
main:
pushq %rbp
movq %rsp, %rbp
subq $64, %rsp
movq $0, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $0, -40(%rbp)
movq $0, -48(%rbp)
movq $0, -56(%rbp)
movq $56, %rax
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
movq $12, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $50, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $2, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $85, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $3, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $50, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $30, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $5, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $99, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $6, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $50, %rax
popq %rcx
movq %rax, (%rcx)
movq $50, %rax
movq %rax, -24(%rbp)
movq $0, %rax
movq %rax, -40(%rbp)
movq $0, %rax
movq %rax, -48(%rbp)
movq $0, %rax
movq %rax, -56(%rbp)
movq $0, %rax
movq %rax, -16(%rbp)
while_0:
movq -16(%rbp), %rax
movq %rax, %rcx
movq $7, %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
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
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
pushq %rax
movq $-1, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_1
movq -40(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -40(%rbp)
jmp endif_1
else_1:
movq -32(%rbp), %rax
movq %rax, %rcx
movq $0, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_2
movq -48(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -48(%rbp)
jmp endif_2
else_2:
movq -56(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -56(%rbp)
endif_2:
endif_1:
movq -16(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_0
endwhile_0:
movq -40(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -48(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -56(%rbp), %rax
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
