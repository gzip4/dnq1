#include <config.h>

	.text
	.global	_start
	.align	16, 0x90
_start:
	/*movl	%edx, multiboot_info */
	/*movl	%edi, roottask_vspace */

	/* load temporary stack pointer */
	lea	_mini_stack, %esp

	/* no need for further initialization, just call loader */
	call	rt_main

	/* We should never return from the rt_main */
	int	$3
1:	jmp	1b

	.bss
	.align	16
	.space	(2 * 4096)
_mini_stack:


	.section .ctors, "a", @progbits
	.align	16
__ctors_begin:

	.section .dtors, "a", @progbits
	.align	16
__dtors_begin:

