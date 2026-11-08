.data
print_fmt:.string "%ld\n" 
.text
.globl main
resolver:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq %rdx, -24(%rbp)
movq %rcx, -32(%rbp)
movq -8(%rbp), %rax
cmpq $0, %rax
jne elvis_end_0
movq -16(%rbp), %rax
cmpq $0, %rax
jne elvis_end_1
movq -24(%rbp), %rax
cmpq $0, %rax
jne elvis_end_2
movq -32(%rbp), %rax
elvis_end_2:
elvis_end_1:
elvis_end_0:
jmp .Lfin_resolver
movq $0, %rax
.Lfin_resolver:
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
movq $100, %rax
movq %rax, -8(%rbp)
movq $50, %rax
movq %rax, -16(%rbp)
movq $25, %rax
movq %rax, -24(%rbp)
movq $10, %rax
movq %rax, -32(%rbp)
movq -8(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
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
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
