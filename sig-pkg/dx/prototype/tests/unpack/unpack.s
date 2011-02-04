	.file	"unpack.cc"
gcc2_compiled.:
__gnu_compiled_cplusplus:
	.text
.LC0:
	.ascii "dt = %lf\12\0"
	.balign 4
.LC1:
	.long 0x0,0x408f4000
	.balign 4
	.globl main
main:
	pushl %ebp
	movl %esp,%ebp
	subl $40,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	call __main
	pushl $12582912
	call __builtin_vec_new
	movl %eax,packed
	pushl $50331648
	call __builtin_vec_new
	movl %eax,unpacked
	leal -40(%ebp),%esi
	xorl %eax,%eax
	addl $8,%esp
	.balign 4,0x90
.L5:
	movb %al,%bl
	andb $3,%bl
	movb %bl,-40(%ebp)
	movl %eax,%edx
	sarl $2,%edx
	andb $3,%dl
	movb %dl,-39(%ebp)
	movl %eax,%edx
	sarl $4,%edx
	andb $3,%dl
	movb %dl,-38(%ebp)
	movl %eax,%edx
	sarl $6,%edx
	andb $3,%dl
	movb %dl,-37(%ebp)
	movl %eax,%edx
	sarl $8,%edx
	andb $3,%dl
	movb %dl,-36(%ebp)
	movl %eax,%edx
	sarl $10,%edx
	andb $3,%dl
	movb %dl,-35(%ebp)
	movl %eax,%edx
	sarl $12,%edx
	andb $3,%dl
	movb %dl,-34(%ebp)
	movl %eax,%edx
	sarl $14,%edx
	andb $3,%dl
	movb %dl,-33(%ebp)
	movl (%esi),%edx
	movl 4(%esi),%ecx
	movl %edx,xlat(,%eax,8)
	movl %ecx,xlat+4(,%eax,8)
	incl %eax
	cmpl $65535,%eax
	jle .L5
	xorl %eax,%eax
	movl packed,%esi
	.balign 4,0x90
.L10:
	xorl %ecx,%ecx
	leal (%eax,%eax,2),%edx
	movl %edx,%edi
	sall $10,%edi
	.balign 4,0x90
.L14:
	leal (%edi,%ecx,2),%edx
	movw $58596,(%esi,%edx)
	incl %ecx
	cmpl $1535,%ecx
	jle .L14
	incl %eax
	cmpl $4095,%eax
	jle .L10
	xorl %eax,%eax
	movl unpacked,%esi
	.balign 4,0x90
.L20:
	xorl %ecx,%ecx
	leal (%eax,%eax,2),%edx
	movl %edx,%edi
	sall $12,%edi
	.balign 4,0x90
.L24:
	leal (%edi,%ecx,8),%edx
	movl $-1,(%esi,%edx)
	movl $-1,4(%esi,%edx)
	incl %ecx
	cmpl $1535,%ecx
	jle .L24
	incl %eax
	cmpl $4095,%eax
	jle .L20
	leal -16(%ebp),%edx
	pushl %edx
	call ftime
	pushl $xlat
	pushl $12288
	pushl $4096
	pushl unpacked
	pushl packed
	call unpack__FPA1536_UsPA1536_UxiiPUx
	leal -32(%ebp),%edx
	pushl %edx
	call ftime
	fldl .LC1
	fld %st(0)
	fidivrl -12(%ebp)
	fiaddl -16(%ebp)
	fxch %st(1)
	fidivrl -28(%ebp)
	fiaddl -32(%ebp)
	fsubp %st,%st(1)
	subl $8,%esp
	fstpl (%esp)
	pushl $.LC0
	call printf
	xorl %eax,%eax
	leal -52(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
	.balign 4
	.globl unpack__FPA1536_UsPA1536_UxiiPUx
unpack__FPA1536_UsPA1536_UxiiPUx:
	pushl %ebp
	movl %esp,%ebp
	subl $16,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl $0,-16(%ebp)
	movl 16(%ebp),%eax
	cmpl %eax,-16(%ebp)
	jge .L38
	.balign 4,0x90
.L31:
	xorl %esi,%esi
	movl -16(%ebp),%ecx
	leal (%ecx,%ecx,2),%edx
	movl %edx,%ebx
	sall $12,%ebx
	movl %ebx,-4(%ebp)
	sall $10,%edx
	movl %edx,-8(%ebp)
	.balign 4,0x90
.L32:
	movl 20(%ebp),%edx
	testl %edx,%edx
	jge .L36
	addl $7,%edx
.L36:
	sarl $3,%edx
	cmpl %edx,%esi
	jge .L30
	movl -4(%ebp),%edi
	leal (%edi,%esi,8),%edi
	movl -8(%ebp),%eax
	leal (%eax,%esi,2),%edx
	movl 8(%ebp),%ecx
	movzwl (%ecx,%edx),%edx
	movl 12(%ebp),%ebx
	movl 24(%ebp),%eax
	movl (%eax,%edx,8),%ecx
	movl %ecx,(%ebx,%edi)
	movl 4(%eax,%edx,8),%ecx
	movl %ecx,4(%ebx,%edi)
	incl %esi
	jmp .L32
	.balign 4,0x90
.L30:
	incl -16(%ebp)
	movl 16(%ebp),%ebx
	cmpl %ebx,-16(%ebp)
	jl .L31
.L38:
	leal -28(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
	.globl xlat
.bss xlat,524288,4
	.globl packed
.bss packed,4,4
	.globl unpacked
.bss unpacked,4,4
