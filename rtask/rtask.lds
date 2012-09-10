OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

. = 0x00200000;

SECTIONS
{
    .text    : { *(.text .text.* .gnu.linkonce.t.*) }

    PROVIDE(_etext = .);

    .rodata  : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
    .rodata1 : { *(.rodata1) }

    .ctors   : { *(.ctors) }
    .dtors   : { *(.dtors) }

    .data    : { *(.data .data.* .gnu.linkonce.d.*) }
    .data1   : { *(.data1) }

    PROVIDE(_edata = .);
    PROVIDE(__bss_start = .);

    .bss : {
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
    }

    PROVIDE(_ebss = .);
    PROVIDE(_end = .);

    /DISCARD/ : {
        *(.eh_frame)
        *(.note.GNU-stack)
        *(.comment)
    }
}
