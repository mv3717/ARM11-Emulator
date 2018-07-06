mov r2,#1
lsl r2,#18
ldr r0,=0x20200004
str r2,[r0]
mov r1,#1
lsl r1,#16
str r1,[r0,#36]
mov r2,#1
loop:
str r1,[r0,#24]
ldr r3,=0x00ffffff
longloop:
sub r3,r3,#1
cmp r3,#0
bne longloop
str r1,[r0,#36]
ldr r3,=0x00ffffff
longloop1:
sub r3,r3,#1
cmp r3,#0
bne longloop1
cmp r2,#0
bne loop
