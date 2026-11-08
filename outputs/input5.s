.data
print_fmt:.string "%ld\n" 
.text
.globl main
get_val:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq %rdx, -24(%rbp)
movq $0, -32(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
cmpq $0, %rax
jne elvis_end_0
movq -24(%rbp), %rax
elvis_end_0:
jmp .Lfin_get_val
movq $0, %rax
.Lfin_get_val:
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
movq $48, %rax
movq %rax, %rdi
call malloc@PLT
movq %rax, -8(%rbp)
movq $0, %rax
movq %rax, -16(%rbp)
while_1:
movq -16(%rbp), %rax
movq %rax, %rcx
movq $6, %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_1
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $0, %rax
popq %rcx
movq %rax, (%rcx)
movq -16(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_1
endwhile_1:
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $100, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $250, %rax
popq %rcx
movq %rax, (%rcx)
movq $0, %rax
movq %rax, -24(%rbp)
movq $0, %rax
movq %rax, -16(%rbp)
while_2:
movq -16(%rbp), %rax
movq %rax, %rcx
movq $6, %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_2
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
pushq %rax
movq $10, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call get_val
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -24(%rbp), %rax
movq %rax, %rcx
movq -32(%rbp), %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
movq -16(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_2
endwhile_2:
movq -24(%rbp), %rax
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
