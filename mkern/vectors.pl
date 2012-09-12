#!/usr/bin/perl -w

# Generate vectors.S, the trap/interrupt entry points.
# There has to be one entry point per interrupt number
# since otherwise there's no way for trap() to discover
# the interrupt number.

print "# generated by vectors.pl - do not edit\n";
print "# handlers\n";
print "\t.text\n";
print "\t.globl alltraps\n";
for(my $i = 0; $i < 256; $i++){
    print "\t.align 4\n";
    print "\t.globl vector$i\n";
    print "vector$i:\n";
    if(!($i == 8 || ($i >= 10 && $i <= 14) || $i == 17)){
        print "\tpushl \$0\n";
    }
    print "\tpushl \$$i\n";
    print "\tjmp alltraps\n";
}

print "\n# vector table\n";
print "\t.section .rodata.vectors, ", '"a"', ", \@progbits\n";
print "\t.align 4\n";
print "\t.globl vectors\n";
print "vectors:\n";
for(my $i = 0; $i < 256; $i++){
    print "\t.long vector$i\n";
}

# sample output:
#   # handlers
#   .globl alltraps
#   .globl vector0
#   vector0:
#     pushl $0
#     pushl $0
#     jmp alltraps
#   ...
#   
#   # vector table
#   .section .systab
#   .align 4
#     .long 0xc001dead
#   .globl vectors
#   vectors:
#     .long vector0
#     .long vector1
#     .long vector2
#   ...

