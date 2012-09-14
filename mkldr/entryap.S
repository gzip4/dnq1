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
# It copies this code (start_ap) at 0x7000.  It puts the address of
# a initial stack in start-4, the address of the place to
# jump to (mpenter) in start-8

#define SEG_KCODE 1
#define SEG_KDATA 2
#define CR0_PE		0x00000001

	.text
	.align 256
	.code16
	.globl _start_ap
_start_ap:
	cli
	xorw    %ax,%ax
	movw    %ax,%ds
	movw    %ax,%es
	movw    %ax,%ss

	lgdt    0x7038
	movl    %cr0, %eax
	orl     $CR0_PE, %eax
	movl    %eax, %cr0

	ljmpl   $(SEG_KCODE<<3), $0x7040

	.p2align 2
gdt:
	.quad 0
	.quad 0x00cf9a000000ffff
	.quad 0x00cf92000000ffff

gdtdesc:
	.word   8 * 3 - 1
	.long   0x7020


	.align 8
	.code32
# _start_ap_32:
	movw    $(SEG_KDATA<<3), %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	xorl    %eax, %eax
	movw    %ax, %fs
	movw    %ax, %gs

	# Setup the stack
	movl    (0x7000-4), %esp
	# Call mpenter()
	call   *(0x7000-8)

	movw    $0x8a00, %ax
	movw    %ax, %dx
	outw    %ax, %dx
	movw    $0x8ae0, %ax
	outw    %ax, %dx
2:
	jmp     2b
