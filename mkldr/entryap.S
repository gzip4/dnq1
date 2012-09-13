# Each non-boot CPU ("AP") is started up in response to a STARTUP
# IPI from the boot CPU.  Section B.4.2 of the Multi-Processor
# Specification says that the AP will start in real mode with CS:IP
# set to XY00:0000, where XY is an 8-bit value sent with the
# STARTUP. Thus this code must start at a 4096-byte boundary.
#
# Because this code sets DS to zero, it must sit
# at an address in the low 2^16 bytes.
#
# Startothers (in main.c) sends the STARTUPs one at a time.
# It copies this code (start) at 0x7000.  It puts the address of
# a newly allocated per-core stack in start-4,the address of the
# place to jump to (mpenter) in start-8, and the physical address
# of entrypgdir in start-12.
#
# This code is identical to bootasm.S except:
#   - it does not need to enable A20
#   - it uses the address at start-4, start-8, and start-12

#define SEG_KCODE 1
#define SEG_KDATA 2

#define CR0_PE		0x00000001
#define CR0_WP		0x00010000
#define CR0_PG		0x80000000
#define CR4_PSE		0x00000010
#define CR4_PAE		0x00000020
#define CR4_PGE		0x00000080

	.text
	.code16
	.globl start_ap
start_ap:
	cli

	xorw    %ax,%ax
	movw    %ax,%ds
	movw    %ax,%es
	movw    %ax,%ss

	lgdt    gdtdesc
	movl    %cr0, %eax
	orl     $CR0_PE, %eax
	movl    %eax, %cr0

	ljmpl    $(SEG_KCODE<<3), $(start_ap_32)


	.code32
	.p2align 4
start_ap_32:
	movw    $(SEG_KDATA<<3), %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	movw    $0, %ax
	movw    %ax, %fs
	movw    %ax, %gs

	# Turn on page size extension for 4Mbyte pages
	movl    %cr4, %eax
	orl     $(CR4_PSE), %eax
	movl    %eax, %cr4
	# Use enterpgdir as our initial page table
	movl    (start_ap-12), %eax
	movl    %eax, %cr3
	# Turn on paging.
	movl    %cr0, %eax
	orl     $(CR0_PE|CR0_PG|CR0_WP), %eax
	movl    %eax, %cr0

	# Switch to the stack allocated by startothers()
	movl    (start_ap-4), %esp
	# Call mpenter()
	call   *(start_ap-8)

	movw    $0x8a00, %ax
	movw    %ax, %dx
	outw    %ax, %dx
	movw    $0x8ae0, %ax
	outw    %ax, %dx
spin:
	jmp     spin

	.p2align 2
gdt:
	.quad 0
	.quad 0x00cf9a000000ffff
	.quad 0x00cf92000000ffff


	.p2align 2
gdtdesc:
	.word   (gdtdesc - gdt - 1)
	.long   gdt

