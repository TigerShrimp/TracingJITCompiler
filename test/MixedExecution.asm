ENTER 0,0
MOV RAX, [RDI+8]
MOV RAX, [RAX]
ADD RAX, 6
ADD RAX, 7
IMUL RAX, 3
SUB RAX, 9
INC RAX
MOV RDI, [RDI+8]
MOV [RDI], RAX
MOV RAX, 0
LEAVE
RET