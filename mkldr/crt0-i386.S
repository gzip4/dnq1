/* x3me - microkernel design research project
 * Copyright (C) 2011  Kirill Zverev <gzip4ever@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

	.code32
	.section .mb_header, "ax"
__mkldr_start:
	jmp	_start

	.ascii	"dnq-kernel"
	.align	16, 0x90
__mb_header:
	.long	0x1BADB002;
	.long	0x00000000;
	.long	- 0x00000000 - 0x1BADB002;


	.text
	.global	_start
	.align	16, 0x90
_start:

	cli	/* disable int's */
	cld	/* clear direction flag */

	/* load temporary stack pointer */
	leal	_mini_stack, %esp

	pushl	%ebx  /* mbi_ptr */
	pushl	%eax  /* mbi_magic */

	/* no need for further initialization, just call loader */
	call	loader_main
	addl	$8, %esp

	/* We should never return from the loader */
	cli
1:	hlt
	jmp	1b

	.bss
	.align	16
	.space	4096
_mini_stack:

