OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

. = 0x00100000;

SECTIONS
{
        .text . :
        {
            *(.mb_header)
            *(.text)
        }

        PROVIDE(_etext = .);

        .rodata . :
        {
            *(.rodata)
        }

        .data . :
        {
            *(.data)
        }

        PROVIDE(_edata = .);

        .bss . :
        {
            *(.bss)
        }

        PROVIDE(_ebss = .);
        PROVIDE(_end = .);

        /DISCARD/ :
        {
            *(.eh_frame)
            *(.note.GNU-stack)
            *(.comment)
        }
}
