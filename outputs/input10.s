.data
print_fmt:.string "%ld\n" 
.text
.globl main
clamp_score:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq -8(%rbp), %rax
cmpq $0, %rax
jne elvis_end_0
movq -16(%rbp), %rax
elvis_end_0:
jmp .Lfin_clamp_score
movq $0, %rax
.Lfin_clamp_score:
leave
ret
ranking:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $0, %rax
movq %rax, -32(%rbp)
movq $0, %rax
movq %rax, -24(%rbp)
while_1:
movq -24(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
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
movq -32(%rbp), %rax
pushq %rax
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
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setg %dl
setl %al
subb %al, %dl
movsbq %dl, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -32(%rbp)
movq -24(%rbp), %rax
movq %rax, %rcx
movq $1, %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_1
endwhile_1:
movq -32(%rbp), %rax
jmp .Lfin_ranking
movq $0, %rax
.Lfin_ranking:
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
movq %rax, -24(%rbp)
movq -24(%rbp), %rax
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
movq $50, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $0, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $2, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $75, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $3, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $0, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $90, %rax
popq %rcx
movq %rax, (%rcx)
movq $0, %rax
movq %rax, -32(%rbp)
movq $0, %rax
movq %rax, -16(%rbp)
while_2:
movq -16(%rbp), %rax
movq %rax, %rcx
movq -24(%rbp), %rax
xchgq %rax, %rcx
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_2
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $60, %rax
pushq %rax
popq %rsi
popq %rdi
subq $8, %rsp
call clamp_score
addq $8, %rsp
popq %rcx
movq %rax, (%rcx)
movq -32(%rbp), %rax
pushq %rax
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -32(%rbp)
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
jmp while_2
endwhile_2:
movq -32(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
pushq %rax
popq %rsi
popq %rdi
call ranking
movq %rax, -40(%rbp)
movq -40(%rbp), %rax
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
