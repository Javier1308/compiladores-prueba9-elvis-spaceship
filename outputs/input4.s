.data
print_fmt:.string "%ld\n" 
.text
.globl main
comparar:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq -8(%rbp), %rax
movq %rax, %rcx
movq -16(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
jmp .Lfin_comparar
movq $0, %rax
.Lfin_comparar:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $48, %rsp
movq $0, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $0, -40(%rbp)
movq $5, %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rax
movq %rax, %rcx
movq $8, %rax
imulq %rcx, %rax
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
movq $64, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $34, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $2, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $25, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $3, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $12, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $22, %rax
popq %rcx
movq %rax, (%rcx)
movq $0, %rax
movq %rax, -16(%rbp)
while_0:
movq -16(%rbp), %rax
pushq %rax
movq -40(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
xchgq %rax, %rcx
subq %rcx, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq $0, %rax
movq %rax, -24(%rbp)
while_1:
movq -24(%rbp), %rax
pushq %rax
movq -40(%rbp), %rax
movq %rax, %rcx
movq -16(%rbp), %rax
xchgq %rax, %rcx
subq %rcx, %rax
movq %rax, %rcx
movq $1, %rax
xchgq %rax, %rcx
subq %rcx, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_1
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
popq %rsi
popq %rdi
call comparar
movq %rax, %rcx
movq $1, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_2
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, -32(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq -32(%rbp), %rax
popq %rcx
movq %rax, (%rcx)
jmp endif_2
else_2:
endif_2:
movq -24(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_1
endwhile_1:
movq -16(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_0
endwhile_0:
movq $0, %rax
movq %rax, -16(%rbp)
while_3:
movq -16(%rbp), %rax
movq %rax, %rcx
movq -40(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_3
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -16(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_3
endwhile_3:
movq -8(%rbp), %rax
movq %rax, %rdi
call free@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
