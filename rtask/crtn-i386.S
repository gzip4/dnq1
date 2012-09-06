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

	.section .ctors, "aw", @progbits
	.globl	__ctors_end
__ctors_end:
	.long	0

	.section .dtors, "aw", @progbits
	.globl	__dtors_end
__dtors_end:
	.long	0

