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

	.bss
	.align	16
	.space	(2 * 4096)
_mini_stack:


	.section .ctors, "aw", @progbits
	.globl	__ctors_begin
	.align	16
__ctors_begin:

	.section .dtors, "aw", @progbits
	.globl	__dtors_begin
	.align	16
__dtors_begin:

