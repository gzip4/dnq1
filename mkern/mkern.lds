OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start);

SECTIONS
{
    .text 0xffc02000 : AT (0x00402000)
    {
        *(.text .text.* .gnu.linkonce.t.*);
        PROVIDE(_etext = .);
    }

    .rodata  : { *(.rodata .rodata.* .gnu.linkonce.r.*); }
    .rodata1 : { *(.rodata1); }

    .data :
    {
        PROVIDE(_data = .);
        *(.data *data.* .gnu.linkonce.d.*);
        PROVIDE(_edata = .);
    }

    .bss :
    {
        PROVIDE(__bss_start = .);
        *(.bss .bss.* .gnu.linkonce.b*);
        *(COMMON);
        PROVIDE(_ebss = .);
    }

    PROVIDE(_end = .);

    /DISCARD/ : {
        *(.eh_frame);
        *(.comment);
        *(.note.GNU-stack);
    }

}
