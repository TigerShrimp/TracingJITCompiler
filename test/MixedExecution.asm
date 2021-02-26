; Run with 23 2 18
ENTER 0,0
; Init variables
MOV RAX, [RDI+8]
MOV RAX, [RAX]
; Start of trace (pc = 2)
ADD RAX, 6 
ADD RAX, 7
IMUL RAX, 3
SUB RAX, 9
INC RAX
; Bailout code
MOV RDI, [RDI+8] 
MOV [RDI], RAX
MOV RAX, 0 ; Default exit (pc = 18)
LEAVE
RET