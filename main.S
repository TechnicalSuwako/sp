	.text
	.section	.note.gnu.property,"a",@note
	.p2align	3, 0x0
	.long	4
	.long	16
	.long	5
	.asciz	"GNU"
	.long	3221225474
	.long	4
	.long	1
	.p2align	3, 0x0
.Lsec_end0:
	.text
	.file	"main.c"
	.globl	helpme                          # -- Begin function helpme
	.p2align	4, 0xcc
	.type	helpme,@function
helpme:                                 # @helpme
	.cfi_startproc
# %bb.0:
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	sofname(%rip), %rsi
	movq	version(%rip), %rdx
	leaq	.L.str.2(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.3(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	leaq	.L.str.4(%rip), %rdi
	callq	puts@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.5(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.6(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.7(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.8(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.9(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.10(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.11(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.12(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.13(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.14(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	sofname(%rip), %rsi
	leaq	.L.str.15(%rip), %rdi
	xorl	%eax, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	jmp	printf@PLT                      # TAILCALL
.Lfunc_end0:
	.size	helpme, .Lfunc_end0-helpme
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0xcc
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	endbr64
	movq	__retguard_3977(%rip), %r11
	xorq	(%rsp), %r11
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r11
	pushq	%r15
	pushq	%r14
	subq	$520, %rsp                      # imm = 0x208
	.cfi_offset %r14, -40
	.cfi_offset %r15, -32
	.cfi_offset %r11, -24
	cmpl	$1, %edi
	jg	.LBB1_1
.LBB1_32:
	callq	helpme
.LBB1_33:
	xorl	%r14d, %r14d
.LBB1_34:
	movl	%r14d, %eax
	addq	$520, %rsp                      # imm = 0x208
	popq	%r14
	popq	%r15
	popq	%r11
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	xorq	(%rsp), %r11
	cmpq	__retguard_3977(%rip), %r11
	je	.Ltmp0
	int3
	int3
.Ltmp1:
	.zero	15-((.Ltmp1-main)&15),204
.Ltmp0:
	retq
.LBB1_1:
	.cfi_def_cfa %rbp, 16
	movq	%rsi, %r14
	movl	%edi, %r15d
	cmpl	$2, %edi
	je	.LBB1_9
# %bb.2:
	cmpl	$3, %r15d
	jne	.LBB1_31
# %bb.3:
	movq	8(%r14), %r15
	leaq	.L.str.16(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_4
# %bb.5:
	leaq	.L.str.17(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_6
# %bb.7:
	leaq	.L.str.18(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_8
# %bb.12:
	leaq	.L.str.24(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_13
# %bb.14:
	leaq	.L.str.25(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_15
# %bb.16:
	leaq	.L.str.26(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_17
# %bb.26:
	leaq	.L.str.27(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_22
# %bb.27:
	leaq	.L.str.30(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	jne	.LBB1_32
# %bb.28:
	leaq	.L.str.20(%rip), %rdi
	callq	getenv@PLT
	testq	%rax, %rax
	je	.LBB1_30
# %bb.29:
	movq	16(%r14), %r9
	leaq	.L.str.31(%rip), %rdx
	leaq	.L.str.22(%rip), %r8
	xorl	%r14d, %r14d
	leaq	-544(%rbp), %r15
	movl	$512, %esi                      # imm = 0x200
	movq	%r15, %rdi
	movq	%rax, %rcx
	xorl	%eax, %eax
	callq	snprintf@PLT
	movq	%r15, %rdi
	callq	otppass@PLT
	jmp	.LBB1_34
.LBB1_9:
	movq	8(%r14), %r14
	leaq	.L.str.19(%rip), %rsi
	movq	%r14, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_10
# %bb.23:
	leaq	.L.str.27(%rip), %rsi
	movq	%r14, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_32
# %bb.24:
	leaq	.L.str.32(%rip), %rsi
	movq	%r14, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	jne	.LBB1_32
# %bb.25:
	movq	sofname(%rip), %rsi
	movq	version(%rip), %rdx
	leaq	.L.str.33(%rip), %rdi
	xorl	%r14d, %r14d
	xorl	%eax, %eax
	callq	printf@PLT
	jmp	.LBB1_34
.LBB1_31:
	movq	8(%r14), %rdi
	leaq	.L.str.27(%rip), %rsi
	callq	strcmp@PLT
	testl	%eax, %eax
	jne	.LBB1_32
# %bb.18:
	cmpl	$4, %r15d
	jne	.LBB1_32
# %bb.19:
	movq	24(%r14), %r15
	leaq	.L.str.28(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	je	.LBB1_20
# %bb.21:
	leaq	.L.str.29(%rip), %rsi
	movq	%r15, %rdi
	callq	strcmp@PLT
	testl	%eax, %eax
	jne	.LBB1_32
.LBB1_22:
	movq	16(%r14), %rdi
	callq	atoi@PLT
	movl	%eax, %edi
	movl	$1, %esi
	callq	genpass@PLT
	jmp	.LBB1_33
.LBB1_4:
	movq	16(%r14), %rdi
	callq	initpass@PLT
	jmp	.LBB1_33
.LBB1_10:
	leaq	.L.str.20(%rip), %rdi
	callq	getenv@PLT
	testq	%rax, %rax
	je	.LBB1_30
# %bb.11:
	leaq	.L.str.23(%rip), %rdx
	leaq	.L.str.22(%rip), %r8
	xorl	%r14d, %r14d
	leaq	-544(%rbp), %r15
	movl	$512, %esi                      # imm = 0x200
	movq	%r15, %rdi
	movq	%rax, %rcx
	xorl	%eax, %eax
	callq	snprintf@PLT
	movq	%r15, %rdi
	xorl	%esi, %esi
	callq	listpass@PLT
	jmp	.LBB1_34
.LBB1_6:
	movq	16(%r14), %rdi
	callq	showpass@PLT
	jmp	.LBB1_33
.LBB1_8:
	movq	16(%r14), %rdi
	callq	yankpass@PLT
	jmp	.LBB1_33
.LBB1_20:
	movq	16(%r14), %rdi
	callq	atoi@PLT
	xorl	%r14d, %r14d
	movl	%eax, %edi
	xorl	%esi, %esi
	callq	genpass@PLT
	jmp	.LBB1_34
.LBB1_15:
	movq	16(%r14), %rdi
	xorl	%r14d, %r14d
	xorl	%esi, %esi
	callq	delpass@PLT
	jmp	.LBB1_34
.LBB1_17:
	movq	16(%r14), %rdi
	movl	$1, %esi
	callq	delpass@PLT
.LBB1_13:
	movq	16(%r14), %rdi
	callq	addpass@PLT
	jmp	.LBB1_33
.LBB1_30:
	leaq	.L.str.21(%rip), %rdi
	callq	perror@PLT
	movl	$-1, %r14d
	jmp	.LBB1_34
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"sp"
	.size	.L.str, 3

	.type	sofname,@object                 # @sofname
	.data
	.globl	sofname
	.p2align	3, 0x0
sofname:
	.quad	.L.str
	.size	sofname, 8

	.type	.L.str.1,@object                # @.str.1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.1:
	.asciz	"1.2.0"
	.size	.L.str.1, 6

	.type	version,@object                 # @version
	.data
	.globl	version
	.p2align	3, 0x0
version:
	.quad	.L.str.1
	.size	version, 8

	.type	.L.str.2,@object                # @.str.2
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.2:
	.asciz	"\357\274\220\357\274\227\357\274\226 %s %s - \343\202\267\343\203\263\343\203\227\343\203\253\343\201\252\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\203\236\343\203\215\343\203\274\343\202\270\343\203\243\343\203\274\n"
	.size	.L.str.2, 68

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"https://076.moe/ | https://gitler.moe/suwako/%s\n\n"
	.size	.L.str.3, 50

	.type	.L.str.4,@object                # @.str.4
.L.str.4:
	.asciz	"\344\275\277\343\201\204\346\226\271\357\274\232\n"
	.size	.L.str.4, 14

	.type	.L.str.5,@object                # @.str.5
.L.str.5:
	.asciz	"%s -i <gpg-id>               \357\274\232GPG\343\201\250\344\275\277\343\201\243\343\201\246\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\271\343\203\210\343\203\254\343\203\274\343\202\270\343\202\222\345\210\235\346\234\237\350\250\255\345\256\232\n"
	.size	.L.str.5, 94

	.type	.L.str.6,@object                # @.str.6
.L.str.6:
	.asciz	"%s -s <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>         \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\350\241\250\347\244\272\n"
	.size	.L.str.6, 64

	.type	.L.str.7,@object                # @.str.7
.L.str.7:
	.asciz	"%s -y <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>         \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\350\241\250\347\244\272\343\201\233\343\201\232\343\202\257\343\203\252\343\203\203\343\203\227\343\203\234\343\203\274\343\203\211\343\201\253\343\202\263\343\203\224\343\203\274\343\201\231\343\202\213\n"
	.size	.L.str.7, 109

	.type	.L.str.8,@object                # @.str.8
.L.str.8:
	.asciz	"%s -l                        \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\344\270\200\350\246\247\343\202\222\350\241\250\347\244\272\n"
	.size	.L.str.8, 64

	.type	.L.str.9,@object                # @.str.9
.L.str.9:
	.asciz	"%s -a <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>         \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\350\277\275\345\212\240\n"
	.size	.L.str.9, 64

	.type	.L.str.10,@object               # @.str.10
.L.str.10:
	.asciz	"%s -d <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>         \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\345\211\212\351\231\244\n"
	.size	.L.str.10, 64

	.type	.L.str.11,@object               # @.str.11
.L.str.11:
	.asciz	"%s -e <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>         \357\274\232\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\345\244\211\346\233\264\n"
	.size	.L.str.11, 64

	.type	.L.str.12,@object               # @.str.12
.L.str.12:
	.asciz	"%s -g <\346\226\207\345\255\227\346\225\260> [risk|secure] \357\274\232\345\270\214\346\234\233\346\226\207\345\255\227\346\225\260\343\201\247\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\343\202\222\343\203\251\343\203\263\343\203\200\343\203\240\343\201\253\344\275\234\346\210\220\343\201\231\343\202\213\343\200\202risk\357\274\235\350\213\261\346\225\260\345\255\227\343\201\256\343\201\277\357\274\210\344\270\215\345\256\211\357\274\211\343\200\201secure\357\274\235\350\213\261\346\225\260\345\255\227\357\274\213\347\211\271\345\210\245\346\226\207\345\255\227\357\274\210\343\203\207\343\203\225\343\202\251\343\203\253\343\203\210\357\274\211\343\202\222\344\275\277\347\224\250\n"
	.size	.L.str.12, 203

	.type	.L.str.13,@object               # @.str.13
.L.str.13:
	.asciz	"%s -o <\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\345\220\215>\n       \357\274\232\343\203\257\343\203\263\343\202\277\343\202\244\343\203\240\343\203\221\343\202\271\343\203\257\343\203\274\343\203\211\357\274\210TOTP\357\274\211\343\202\222\350\241\250\347\244\272\343\200\202\345\255\230\345\234\250\343\201\227\343\201\252\343\201\221\343\202\214\343\201\260\343\200\201\345\211\265\344\275\234\343\201\231\343\202\213\n"
	.size	.L.str.13, 127

	.type	.L.str.14,@object               # @.str.14
.L.str.14:
	.asciz	"%s -h                        \357\274\232\343\203\230\343\203\253\343\203\227\343\202\222\350\241\250\347\244\272\n"
	.size	.L.str.14, 52

	.type	.L.str.15,@object               # @.str.15
.L.str.15:
	.asciz	"%s -v                        \357\274\232\343\203\220\343\203\274\343\202\270\343\203\247\343\203\263\343\202\222\350\241\250\347\244\272\n"
	.size	.L.str.15, 58

	.type	.L.str.16,@object               # @.str.16
.L.str.16:
	.asciz	"-i"
	.size	.L.str.16, 3

	.type	.L.str.17,@object               # @.str.17
.L.str.17:
	.asciz	"-s"
	.size	.L.str.17, 3

	.type	.L.str.18,@object               # @.str.18
.L.str.18:
	.asciz	"-y"
	.size	.L.str.18, 3

	.type	.L.str.19,@object               # @.str.19
.L.str.19:
	.asciz	"-l"
	.size	.L.str.19, 3

	.type	.L.str.20,@object               # @.str.20
.L.str.20:
	.asciz	"HOME"
	.size	.L.str.20, 5

	.type	.L.str.21,@object               # @.str.21
.L.str.21:
	.asciz	"\343\203\233\343\203\274\343\203\240\343\203\207\343\202\243\343\203\254\343\202\257\343\203\210\343\203\252\343\202\222\345\217\227\345\217\226\343\201\253\345\244\261\346\225\227\343\200\202"
	.size	.L.str.21, 49

	.type	.L.str.22,@object               # @.str.22
.L.str.22:
	.asciz	"/.local/share/sp/"
	.size	.L.str.22, 18

	.type	.L.str.23,@object               # @.str.23
.L.str.23:
	.asciz	"%s%s"
	.size	.L.str.23, 5

	.type	.L.str.24,@object               # @.str.24
.L.str.24:
	.asciz	"-a"
	.size	.L.str.24, 3

	.type	.L.str.25,@object               # @.str.25
.L.str.25:
	.asciz	"-d"
	.size	.L.str.25, 3

	.type	.L.str.26,@object               # @.str.26
.L.str.26:
	.asciz	"-e"
	.size	.L.str.26, 3

	.type	.L.str.27,@object               # @.str.27
.L.str.27:
	.asciz	"-g"
	.size	.L.str.27, 3

	.type	.L.str.28,@object               # @.str.28
.L.str.28:
	.asciz	"risk"
	.size	.L.str.28, 5

	.type	.L.str.29,@object               # @.str.29
.L.str.29:
	.asciz	"secure"
	.size	.L.str.29, 7

	.type	.L.str.30,@object               # @.str.30
.L.str.30:
	.asciz	"-o"
	.size	.L.str.30, 3

	.type	.L.str.31,@object               # @.str.31
.L.str.31:
	.asciz	"%s%s%s.gpg"
	.size	.L.str.31, 11

	.type	.L.str.32,@object               # @.str.32
.L.str.32:
	.asciz	"-v"
	.size	.L.str.32, 3

	.type	.L.str.33,@object               # @.str.33
.L.str.33:
	.asciz	"%s-%s\n"
	.size	.L.str.33, 7

	.hidden	__retguard_2312                 # @__retguard_2312
	.type	__retguard_2312,@object
	.section	.openbsd.randomdata.retguard.2312,"aGw",@progbits,__retguard_2312,comdat
	.weak	__retguard_2312
	.p2align	3, 0x90
__retguard_2312:
	.quad	0
	.size	__retguard_2312, 8

	.hidden	__retguard_3977                 # @__retguard_3977
	.type	__retguard_3977,@object
	.section	.openbsd.randomdata.retguard.3977,"aGw",@progbits,__retguard_3977,comdat
	.weak	__retguard_3977
	.p2align	3, 0x90
__retguard_3977:
	.quad	0
	.size	__retguard_3977, 8

	.section	".note.GNU-stack","",@progbits
	.addrsig
