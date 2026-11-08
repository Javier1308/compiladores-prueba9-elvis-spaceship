.data
print_fmt:.string "%ld\n" 
.text
.globl main
signo:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -8(%rbp)
movq -8(%rbp), %rax
movq %rax, %rcx
movq $0, %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
jmp .Lfin_signo
movq $0, %rax
.Lfin_signo:
leave
ret
safe_div:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq %rdx, -24(%rbp)
movq -16(%rbp), %rax
movq %rax, %rcx
movq $0, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_0
movq -24(%rbp), %rax
jmp .Lfin_safe_div
jmp endif_0
else_0:
movq -8(%rbp), %rax
movq %rax, %rcx
movq -16(%rbp), %rax
xchgq %rax, %rcx
cqto
idivq %rcx
jmp .Lfin_safe_div
endif_0:
movq $0, %rax
.Lfin_safe_div:
leave
ret
clamp:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq %rdx, -24(%rbp)
movq -8(%rbp), %rax
movq %rax, %rcx
movq -16(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
pushq %rax
movq $-1, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_1
movq -16(%rbp), %rax
jmp .Lfin_clamp
jmp endif_1
else_1:
movq -8(%rbp), %rax
movq %rax, %rcx
movq -24(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
movq %rax, %rcx
movq $1, %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je else_2
movq -24(%rbp), %rax
jmp .Lfin_clamp
jmp endif_2
else_2:
movq -8(%rbp), %rax
jmp .Lfin_clamp
endif_2:
endif_1:
movq $0, %rax
.Lfin_clamp:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
movq $-25, %rax
pushq %rax
popq %rdi
call signo
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $0, %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $1, %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $100, %rax
pushq %rax
movq $4, %rax
pushq %rax
movq $999, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call safe_div
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $100, %rax
pushq %rax
movq $0, %rax
pushq %rax
movq $999, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call safe_div
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $5, %rax
pushq %rax
movq $10, %rax
pushq %rax
movq $50, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call clamp
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $25, %rax
pushq %rax
movq $10, %rax
pushq %rax
movq $50, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call clamp
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $80, %rax
pushq %rax
movq $10, %rax
pushq %rax
movq $50, %rax
pushq %rax
popq %rdx
popq %rsi
popq %rdi
call clamp
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
