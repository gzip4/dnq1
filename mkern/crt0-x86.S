#include <config.h>

#define RING3_CODE	1
#define RING3_DATA	2


/*
Stack to jump ring3:
	SS3
	ESP3
	EFLAGS3
	CS3
	EIP3
*/

	.text
	.code32
	.globl start, _start
start:
_start:
	/* we use stack of our caller */
	/* prepare stack for get back in ring3 */
	popl	%eax		/* save return addr */
	movl	%esp, %edx	/* save stack       */

	pushl	$RING3_DATA	/* SS3    */
	pushl	%edx		/* ESP3   */
	pushl	$0x3202		/* EFLAGS */
	pushl	$RING3_CODE	/* CS3    */
	pushl	%eax		/* EIP3   */

#if 1
	/* clear bss */
	pushl	%edi
	cld
	xorl	%eax, %eax
	movl	$__bss_start, %edi
	movl	$_ebss, %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx
	rep
	stosl
	popl	%edi
#endif

	call	kinit

        /* get back to roottask (ring3) */
	movw	$RING3_DATA, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	iretl

	.section .ctors
